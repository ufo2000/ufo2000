#!/usr/bin/env lua

if arg[1] == nil then
    print("Usage: db2htmlreport.lua [database file]")
    os.exit()
end

------------------------------------------------------------------------------
-- start making html report
------------------------------------------------------------------------------

io.write([[
<html><head><style type="text/css" media=screen><!--
table {
  border-collapse: collapse;
  empty-cells: show;
  font-family: arial;
  font-size: small;
  white-space: nowrap;
  background: #F0F0F0;
}
--></style><meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head><body>]])


-- complete table of played games
db = sqlite3.open(arg[1])

io.write("<br> <b>UFO2000 players rating table</b><br>")
io.write("<table border=1>")
io.write("<tr><td>pos<td>name<td>games played<td>games won<td>ELO score")

local pos = 1

for name, played, won, elo_score in db:cols([[
	select name, played, won, elo_score from 
	( 
	select name, 
	(select count(*) from ufo2000_game_players p,ufo2000_games g 
	where p.player=u.name and p.game=g.id and g.is_finished='Y') played, 
	(select count(*) from ufo2000_game_players p,ufo2000_games g 
	where p.player=u.name and p.game=g.id and g.is_finished='Y' and g.result=p.position) won, 
	elo_score 
	from ufo2000_users u 
	) 
	where won>0 
	order by elo_score desc 
	]]) 
do
    if elo_score then
        io.write("<tr><td>", pos, "<td>", name, "<td>", played ,"<td>", won, "<td>", math.floor(elo_score or 0), "\n")
        pos = pos + 1
    end
end

io.write("</table>")

io.write("<br> <b>UFO2000 recent games statistics</b><br>")
io.write("<table border=1>\n")
io.write("<tr><td>start time<td>game id<td>version<td>player1<td>player2<td>result<td>comment\n")

local get_version_request = db:prepare([[
    SELECT sender, command FROM ufo2000_game_packets 
    WHERE game=? AND (id<10) AND command LIKE "UFO2000 REVISION OF YOUR OPPONENT: %"]])
    
local function get_client_versions(game_id)
    local tmp = {}
    for pid, version in get_version_request:bind(game_id):cols() do tmp[pid] = version end
    
    if type(tmp[1]) ~= "string" or type(tmp[2]) ~= "string" then return end
    local _, _, x = string.find(tmp[1], "(%d+)$")
    local _, _, y = string.find(tmp[2], "(%d+)$")
    
    return x, y
end
    
for id, ver, pl1, pl2, result in db:cols([[
	select id, ver, pl1, pl2, result from 
	(select id,ifnull(g.client_version, "") ver,p1.player pl1, p2.player pl2, case when g.result=1 then p1.player||" won" when g.result=2 then p2.player||" won" when g.result=3 then "draw" else "not finished" end result,ifnull(g.errors,"") errors 
	from ufo2000_games g, ufo2000_game_players p1, ufo2000_game_players p2 
	where g.id=p1.game and g.id=p2.game and p1.position=1 and p2.position=2) 
	order by id desc 
	limit 100 
	]]) 
do
    local comment = ""
    
    local v1, v2 = get_client_versions(id)
    if v1 ~= v2 then comment = string.format("version check failed (%d vs %d)", v1, v2) end
    
    if result == "not finished" then result = "-" end

    local julian_day = db:first_cols("select time from ufo2000_game_packets where id=1 and game=" .. id)
    local date_string = ""
    if julian_day then 
        date_string = os.date("%Y-%m-%d %H:%M:%S", (julian_day - 2440587.5) * 86400.0 + 0.5) 
        io.write("<tr><td>", date_string, "<td>", id, "<td>", ver, "<td>", pl1, "<td>", pl2, "<td>", result, "<td>", comment, "\n")
    end
end
io.write("</table>")

io.write("<br> <b>UFO2000 bugs statistics table</b><br>")
io.write("<table border=1>\n")
io.write("<tr><td>game id<td>version<td>error report\n")
for game, version, param, error_report in db:cols([[
    select d.game, g.client_version, param, d.value from 
        ufo2000_debug_packets d, ufo2000_games g where 
        d.game=g.id and (param="crash" or param="assert" or param="crc error")
        ]])
do
    if param == "crc error" then error_report = "crc error" end
    io.write("<tr><td>", game, "<td>", version, "<td>", error_report, "\n")
end
io.write("</table>")

io.write(string.format("<br>report generated on %s<br>time %.2f seconds", os.date(), os.clock()))
io.write("</body></html>")
db:close()
