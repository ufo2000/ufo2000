#!/usr/bin/env lua

if arg[1] == nil then
    print("Usage: db2htmlreport.lua [database file]")
    os.exit()
end

db = sqlite3.open(arg[1])

------------------------------------------------------------------------------
-- function that formats time
------------------------------------------------------------------------------

function timestring(x)
    local hours = math.floor(x / 3600)
    local minutes = math.floor((x - hours * 3600) / 60)
    local seconds = math.floor(x - hours * 3600 - minutes * 60)
    local result = ""
    if hours > 0 then
        result = result .. hours .. "h "
    end
    if hours > 0 or minutes > 0 then
        result = result .. minutes .. "m "
    end

    return result .. seconds .. "s"
end

local function convert_julian_day(julian_day)
    if not julian_day then return end
    return (julian_day - 2440587.5) * 86400.0 + 0.5
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

io.write("<br> <b>UFO2000 top players (only players active during last 15 days are listed)</b><br>")
io.write("<table border=1>")
io.write("<tr><td>pos<td>name<td>victories<td>draws<td>defeats<td>ELO score<td>last login")

local pos = 1

for name, elo_score, victories, defeats, draws, last_login in db:cols([[
    SELECT name, elo_score, victories, defeats, draws, last_login FROM ufo2000_users
    ORDER BY elo_score DESC]]) 
do
    if last_login then
        local timediff = 0
        local timediff_string = "today"
        timediff = math.floor((os.time() - convert_julian_day(last_login)) / (24 * 60 * 60))
        if timediff > 0 then timediff_string = string.format("%d days ago", timediff) end
        local games_count = (victories or 0) + (draws or 0) + (defeats or 0)

        if timediff <= 15 and games_count > 0 then
            elo_score = elo_score or 1500
            io.write("<tr><td>", pos, "<td>", name, "<td>", victories or 0, "<td>", draws or 0, "<td>", defeats or 0, "<td>", math.floor(elo_score or 0), "<td>", timediff_string, "\n")
            pos = pos + 1
        end
    end
end

io.write("</table>")

local get_version_request = assert(db:prepare([[
    SELECT sender, command FROM ufo2000_game_packets 
    WHERE game=? AND (id<10) AND command LIKE "UFO2000 REVISION OF YOUR OPPONENT: %"]]))
    
local function get_client_versions(game_id)
    local tmp = {}
    for pid, version in get_version_request:bind(game_id):cols() do tmp[pid] = version end
    
    if type(tmp[1]) ~= "string" or type(tmp[2]) ~= "string" then return end
    local _, _, x = string.find(tmp[1], "(%d+)$")
    local _, _, y = string.find(tmp[2], "(%d+)$")
    
    return x, y
end

    
local get_bugreport_request = assert(db:prepare([[
    SELECT param, value FROM ufo2000_debug_packets 
    WHERE game=? AND (param="crash" or param="assert" or param="crc error")]]))

local function get_bugreport(game_id)
    local tmp = {}
    for bug_type, bug_msg in get_bugreport_request:bind(game_id):cols() do 
        tmp["<b>" .. bug_type .. "</b><pre>" .. bug_msg .. "</pre>"] = 1 
    end
    local tmp2 = {}
    for k in tmp do table.insert(tmp2, k) end
    return table.concat(tmp2, "<br>")
end

local get_game_start_time_request = assert(db:prepare([[
    SELECT time FROM ufo2000_game_packets WHERE id=1 AND game=?]]))
    
local get_game_end_time_request = assert(db:prepare([[
    SELECT time FROM ufo2000_game_packets WHERE game=? ORDER BY id DESC LIMIT 1]]))

local function get_game_time(game_id)
    if not get_game_start_time_request:bind(game_id) then return end
    if not get_game_end_time_request:bind(game_id) then return end
    
    local start_time = convert_julian_day(get_game_start_time_request:first_cols())
    local end_time = convert_julian_day(get_game_end_time_request:first_cols())
    if not start_time or not end_time then return end
    local duration = end_time - start_time
    
    return start_time, duration
end
    
io.write("<br> <b>UFO2000 recent games statistics</b><br>")
io.write("<table border=1>\n")
io.write("<tr><td>id<td>version<td>date<td>player1<td>player2<td>time<td>result<td>comment<td></tr>\n")
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
    if v1 ~= v2 then 
        comment = string.format("version check failed (%d vs %d)", v1, v2)
    else
        comment = get_bugreport(id)
    end
    
    if result == "not finished" then result = "-" end
    
    local start_time, duration = get_game_time(id)
    
    if start_time then
        io.write("<tr>",
            "<td>", id, "<td>", ver, 
            "<td>", os.date("%Y-%m-%d %H:%M:%S", start_time),
            "<td>", pl1, "<td>", pl2,  
            "<td>", timestring(duration), 
            "<td>", result, 
            "<td>", comment,
            "<td><A href=http://ufo2000.lxnt.info/db_report.php?a=", id, ",0,0,>Compare CRC</A>\n")
    end
end
io.write("</table>")

local terain_games_table = {}
local terrain_table = {}

for _, game, terrain, t in db:cols([[
    SELECT 1, game, value, time FROM ufo2000_debug_packets WHERE param="terrain"]])
do
    local timediff = math.floor((os.time() - convert_julian_day(t)) / (24 * 60 * 60))
    if timediff < 30 and not terain_games_table[game or "?"] then 
        terrain_table[terrain] = (terrain_table[terrain] or 0) + 1
        terain_games_table[game or "?"] = 1
    end
end

local equipment_games_table = {}
local equipment_table = {}

for _, game, equipment, t in db:cols([[
    SELECT 1, game, value, time FROM ufo2000_debug_packets WHERE param="equipment"]])
do
    local timediff = math.floor((os.time() - convert_julian_day(t)) / (24 * 60 * 60))
    if timediff < 30 and not equipment_games_table[game or "?"] then 
        equipment_table[equipment] = (equipment_table[equipment] or 0) + 1
        equipment_games_table[game or "?"] = 1
    end
end

-- display terrain types popularity statistics
io.write("<br>")
io.write("<b>Terrain types statistics table</b><br>")
local count = 0
local tmp = {}
for k, v in terrain_table do
    count = count + v 
    table.insert(tmp, {k, v}) 
end 
terrain_table = tmp
table.sort(terrain_table, function (a, b) return a[2] > b[2] end)
io.write("<table border=1>")
io.write("<tr><td>terrain type<td>number of times used<td>percent share\n") 
for k, v in ipairs(terrain_table) do
    io.write(string.format("<tr><td>%s<td>%d<td>%.1f%%", v[1], v[2], v[2] / count * 100))
end
io.write("</table>")

-- display equipment types popularity statistics
io.write("<br>")
io.write("<b>Weapon set types statistics table</b><br>")
local count = 0
local tmp = {}
for k, v in equipment_table do
    count = count + v 
    table.insert(tmp, {k, v}) 
end 
equipment_table = tmp
table.sort(equipment_table, function (a, b) return a[2] > b[2] end)
io.write("<table border=1>")
io.write("<tr><td>weapon set<td>number of times used<td>percent share\n") 
for k, v in ipairs(equipment_table) do
    io.write(string.format("<tr><td>%s<td>%d<td>%.1f%%", v[1], v[2], v[2] / count * 100))
end
io.write("</table>")

------------------------------------------------------------------------------

local get_version_request_new = assert(db:prepare([[
    SELECT version FROM ufo2000_user_sessions WHERE id=?]]))

local get_version_request = assert(db:prepare([[
    SELECT value FROM ufo2000_debug_packets WHERE session=? AND param='version']]))

local function get_version(session)
    if not get_version_request_new:bind(session) then return end
    local version = get_version_request_new:first_cols()
    if version and version ~= "" then return version end
    if not get_version_request:bind(session) then return end
    return get_version_request:first_cols()
end

local bad_games = {}
local complete_crash_report = {}

local version_statistics = {}

for _, game, session, param, value in db:cols([[
    SELECT 1, game, session, param, value FROM ufo2000_debug_packets WHERE
    param IN ('crc error', 'assert', 'crash')]]) 
do
    if game and not bad_games[game] then
        local version = get_version(session) or "?"
        if param == "assert" then 
            bad_games[game] = value 
        elseif param == "crash" then
            local _, _, summary = string.find(value, "^(Access Violation at %x+)")
            if not summary then summary = "?" end
            bad_games[game] = summary
            complete_crash_report[version .. ":" .. summary] = value
        else
            bad_games[game] = param
        end
        if not version_statistics[version] then version_statistics[version] = {} end
        version_statistics[version][bad_games[game]] = (version_statistics[version][bad_games[game]] or 0) + 1
    end
end

local good_games = {}

for _, game, session in db:cols([[
    select 1, game, session from ufo2000_debug_packets where param='result']]) 
do
    if game and not good_games[game] and not bad_games[game] then
        good_games[game] = 1

        local version = get_version(session) or "?"
        if not version_statistics[version] then version_statistics[version] = {} end
        version_statistics[version].ok = (version_statistics[version].ok or 0) + 1
    end
end

------------------------------------------------------------------------------
-- start making game stability report
------------------------------------------------------------------------------

local tmp = {}
for k, v in version_statistics do
    local _, _, revision = string.find(k, "(%d+)%D*$")
    revision = tonumber(revision)
    table.insert(tmp, {revision=revision, version=k, info=v})
end
table.sort(tmp, function(a, b) return a.revision > b.revision end)
version_statistics = tmp

io.write("<br>")

local tbl_count = 0
for _, data in ipairs(version_statistics) do
    tbl_count = tbl_count + 1
    local total_games = 0
    for k, v in data.info do total_games = total_games + v end

    if tbl_count <= 1 or total_games >= 50 then
        io.write(string.format("<b>Version: %s, games played: %d, stability rating: %.2f%%</b><br>", 
            data.version, total_games, (data.info.ok or 0) / total_games * 100))
        if tbl_count <= 1 then
            io.write("<table border=1>")
            local tmp = {}
            for k, v in data.info do if k ~= "ok" then table.insert(tmp, {k, v}) end end
            table.sort(tmp, function(a, b) return a[2] > b[2] end)

            for _, v in ipairs(tmp) do
                print("<tr><td>", v[2], "<td>", complete_crash_report[data.version .. ":" .. v[1]] or v[1])
            end
            io.write("</table>")
        end
    end
end

db:close()

io.write(string.format("<br>report generated on %s<br>time %.2f seconds", os.date(), os.clock()))
io.write("</body></html>")
