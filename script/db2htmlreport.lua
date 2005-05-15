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
io.write("<tr><td>name<td>games played<td>games won<td>ELO score")

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
	io.write("<tr><td>", name, "<td>", played ,"<td>", won, "<td>", elo_score, "\n")
end

io.write("</table>")

io.write("<br> <b>UFO2000 recent games statistics</b><br>")
io.write("<table border=1>\n")
io.write("<tr><td>game<td>version<td>player1<td>player2<td>result<td>comment\n")

for html_row in db:cols([[
	select "<tr><td>"||id||"<td>"||ver||"<td>"||pl1||"<td>"||pl2||"<td>"||result||"<td>"||errors from 
	(select id,ifnull(g.client_version, "") ver,p1.player pl1, p2.player pl2, case when g.result=1 then p1.player||" won" when g.result=2 then p2.player||" won" when g.result=3 then "draw" else "not finished" end result,ifnull(g.errors,"") errors 
	from ufo2000_games g, ufo2000_game_players p1, ufo2000_game_players p2 
	where g.id=p1.game and g.id=p2.game and p1.position=1 and p2.position=2) 
	order by id desc 
	limit 100 
	]]) 
do
	io.write(html_row, "\n")
end
io.write("</table>")

io.write("<br> <b>UFO2000 bugs statistics table</b><br>")
io.write("<table border=1>\n")
io.write("<tr><td>game<td>version<td>error report\n")
for game, version, error_report in db:cols([[
    select d.game, g.client_version, d.value from 
        ufo2000_debug_packets d, ufo2000_games g where 
        d.game=g.id and (param="crash" or param="assert")
        ]])
do
    io.write("<tr><td>", game, "<td>", version, "<td>", error_report, "\n")
end
io.write("</table>")

io.write(string.format("<br>report generated on %s<br>time %.2f seconds", os.date(), os.clock()))
io.write("</body></html>")
db:close()
