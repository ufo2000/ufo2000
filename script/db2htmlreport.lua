#!/usr/bin/lua

------------------------------------------------------------------------------
-- A script for ufo2000 server log parsing
-- It displays some statistics in html form that can be used to hold
-- UFO2000 tournaments
------------------------------------------------------------------------------

if arg[1] == nil then
    print("Usage: db2htmlreport.lua [database file]")
    os.exit()
end

-- if there is no second command line argument, write output to stdout
-- if it exists, write output to file
out = io.stdout
if arg[2] ~= nil then out = io.open(arg[2], "wt") end

------------------------------------------------------------------------------
-- start making html report
------------------------------------------------------------------------------

-- function that formats time
function timestring(x)
    local hours = math.floor(x / 3600)
    local minutes = math.floor((x - hours * 3600) / 60)
    local seconds = x - hours * 3600 - minutes * 60
    local result = ""
    if hours > 0 then
        result = result .. hours .. "h "
    end
    if hours > 0 or minutes > 0 then
        result = result .. minutes .. "m "
    end

    return result .. seconds .. "s"
end

out:write([[
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

out:write("<br> <b>UFO2000 players rating table</b><br>")
out:write("<table border=1>")
out:write("<tr><td>name<td>games played<td>gamees won<td>ELO score<td>")

for html_row in db:cols("\
select \"<tr><td>\"||name||\"<td>\"||played||\"<td>\"||won||\"<td>\"||ifnull(elo_score,\"\")||\"<td>\" from \
( \
select name, \
(select count(*) from ufo2000_game_players p,ufo2000_games g \
where p.player=u.name and p.game=g.id and g.is_finished='Y') played, \
(select count(*) from ufo2000_game_players p,ufo2000_games g \
where p.player=u.name and p.game=g.id and g.is_finished='Y' and g.result=p.position) won, \
elo_score \
from ufo2000_users u \
) \
where won>0 \
order by elo_score desc \
") do
out:write(html_row)
end
out:write("</table>")


out:write("<br> <b>UFO2000 played games statistics table</b><br>")
out:write("<table border=1>\n")
out:write("<tr><td>game<td>version<td>player1<td>player2<td>result<td>comment<td>\n")

for html_row in db:cols("\
select \"<tr><td>\"||id||\"<td>\"||ver||\"<td>\"||pl1||\"<td>\"||pl2||\"<td>\"||result||\"<td>\"||errors||\"<td>\" from \
(select id,ifnull(g.client_version, \"\") ver,p1.player pl1, p2.player pl2, case when g.result=1 then p1.player||\" won\" when g.result=2 then p2.player||\" won\" when g.result=3 then \"draw\" else \"not finished\" end result,ifnull(g.errors,\"\") errors \
from ufo2000_games g, ufo2000_game_players p1, ufo2000_game_players p2 \
where g.id=p1.game and g.id=p2.game and p1.position=1 and p2.position=2) \
order by id desc \
limit 100 \
") do
out:write(html_row)
out:write("\n")
end
out:write("</table>")

out:write(string.format("<br>report generated on %s<br>server log parsing performed for %.2f seconds", os.date(), os.clock()))
out:write("</body></html>")
out:close()
db:close()
