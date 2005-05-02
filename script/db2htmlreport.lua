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

for html_row in db:cols("SELECT * FROM ufo2000_report_html") do
out:write(html_row)
end


out:write(string.format("<br>report generated on %s<br>server log parsing performed for %.2f seconds", os.date(), os.clock()))
out:write("</body></html>")
out:close()
db:close()