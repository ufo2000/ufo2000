------------------------------------------------------------------------------
-- A script for ufo2000 server log parsing
-- It displays some statistics in html form that can be used to hold
-- UFO2000 tournaments
------------------------------------------------------------------------------

if arg[1] == nil then
	print("you need to specify ufo2000 server log file in a command line")
	os.exit()
end

-- if there is no second command line argument, write output to stdout
-- if it exists, write output to file
out = io.stdout 
if arg[2] ~= nil then out = io.open(arg[2], "wt") end

local time_start = os.clock()
-- history of played games
local games_history = {}
-- table of operating systems used by clients
local os_table = {}
-- table of terrain types used in games
local terrain_table = {}
-- scores and other information for each player
local tournament_table = {}

------------------------------------------------------------------------------
-- server log parsing function
------------------------------------------------------------------------------

function process_log(filename, history)

	local games = {}
	local users = {}

	local login_name = nil
	local login_line_number = nil

	local function gettime(l)
		local _
		local t = { isdst = 0 }
		_, _, t.day, t.month, t.year, t.hour, t.min, t.sec = 
			string.find(l, "^(%d+)/(%d+)/(%d+) (%d+)%:(%d+)%:(%d+)")
		return os.time(t)
	end

	local function user_online(name, l)
		if not name or users[name] then return end
		users[name] = {login_time = gettime(l)}
		if not tournament_table[name] then 
			tournament_table[name] = {
				win_count      = 0, 
				games_started  = 0, 
				games_finished = 0, 
				time_online    = 0,
				battle_time    = 0
			}
		end
	end

	local function user_offline(name, l)
		if name and users[name] then 
			local time_bonus = gettime(l) - users[name].login_time
			tournament_table[name].time_online = tournament_table[name].time_online + time_bonus
			users[name] = nil
		end
	end

	local function user_win(name)
		if tournament_table[name] then
			tournament_table[name].win_count = tournament_table[name].win_count + 1
		end
	end

	local function user_startgame(name, time_bonus)
		if tournament_table[name] then
			tournament_table[name].games_started = tournament_table[name].games_started + 1
			tournament_table[name].battle_time = tournament_table[name].battle_time + time_bonus
		end
	end

	local function user_endgame(name)
		if tournament_table[name] then
			tournament_table[name].games_finished = tournament_table[name].games_finished + 1
		end
	end

	local function startgame(p1, p2, l)
		user_online(p1, l)
		user_online(p2, l)
		local game_info = {["p1"] = p1, ["p2"] = p2, ["exited"] = {}}
		game_info.start_time = gettime(l)
		games[p1] = game_info
		games[p2] = game_info
	end

	local function endgame(p, l)
		user_online(p, l)
		local game_info = games[p]
		if not game_info then return end
		games[p] = nil
		if not game_info.end_time then game_info.end_time = gettime(l) end
	
		game_info.exited[p] = 1
		if game_info.exited[game_info.p1] and game_info.exited[game_info.p2] then
			if game_info.terrain then
				terrain_table[game_info.terrain] = (terrain_table[game_info.terrain] or 0) + 1 
			end
			if not game_info.version_error then
				user_startgame(game_info.p1, game_info.end_time - game_info.start_time)
				user_startgame(game_info.p2, game_info.end_time - game_info.start_time)
			end
			if game_info.winner then
				user_endgame(game_info.p1)
				user_endgame(game_info.p2)
				user_win(game_info.winner)
			end
			table.insert(history, game_info)
		end
	end

	local previous_line = nil
	local line_number = 0
	for l in io.lines(filename) do
		line_number = line_number + 1

		local _, _, p, packet_id, packet_data = string.find(l, "packet from (.-) {id=(%d+), data=(.-)[\013}]?$")

		if packet_id == "12" then

			if packet_data == "_Xcom_QUIT_" or packet_data == "QUIT" then
				-- handle normal game exit
				endgame(p, l)
			else
				-- handle version number
				local _, _, version_number = string.find(packet_data, "^UFO2000 REVISION OF YOUR OPPONENT: (%d+)")
				if version_number then
					if games[p].version and games[p].version ~= version_number then
						-- handle version check error
						games[p].version_error = true
					end
					games[p].version = version_number 
				end
			end

		elseif packet_id == "14" then

			if packet_data == "crc error" then
				-- handle crc error
				games[p].crc_error = true 
			elseif packet_data == "crash" then
				-- handle crash
				games[p].crash_error = true 
			else
		    	local _, _, id, value = string.find(packet_data, "^(.-)%:(.*)")
				
				if id == "system" then
					-- handle operating system information
					if not os_table[value] then os_table[value] = {} end
					os_table[value][p] = 1
				elseif id == "terrain" then
					-- handle terrain type
					games[p].terrain = value
				elseif id == "result" and value == "victory" then
					-- detect who is the winner in a battle
					games[p].winner = p
				elseif id == "assert" then
					-- detect who is the winner in a battle
					games[p].assert_error = value
				end
			end

		elseif not packet_id then

			local _, _, msg = string.find(l, "^%S+%s+%S+%s+(.*)")
			if not msg then msg = "" end

			-- handle game start
			local _, _, p1, p2 = string.find(msg, "^game start: '(.-)' vs '(.-)'")
			if p1 and p2 then
				startgame(p1, p2, l)
			end
		
			-- handle disconnect
			local _, _, p = string.find(msg, "^connection closed %(name='(.-)', max_ave_traffic=")
			if p then
				user_offline(p, l)
				if games[p] then
					games[p].connection_error = true
					endgame(p, l)
				end
			end

			if string.find(msg, "^server started") then
			-- handle server restart
				previous_line = previous_line or l
				for k, v in games do endgame(k, previous_line) end
				games = {}
				for k, v in users do user_offline(k, previous_line) end
				users = {}
			end

			-- handle user login
			local _, _, p = string.find(msg, "^user login %(name='(.-)', pwd=")
			if p then login_name = p end

			if msg == "login ok" and login_name then
				if games[login_name] then endgame(login_name, l) end
				if users[login_name] then user_offline(login_name, l) end
				user_online(login_name, l)
				login_name = nil
			end

			if string.find(msg, "^login failed") or string.find(msg, "^registration failed") then
				login_name = nil
			end
		
		end

		previous_line = l
	end
	for k, v in users do user_offline(k, previous_line) end
end

------------------------------------------------------------------------------
-- start making html report
------------------------------------------------------------------------------

process_log(arg[1], games_history)

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

out:write("<html><head></head><body>")

-- tournament table
out:write("<br>")
out:write(string.format("<b>UFO2000 players rating table calculated for last 7 days</b><br>"))
out:write("<table border=1>")
out:write("<tr><td>rank<td>name<td>games played<td>games won<td>server chat time<td>battle time<td>score\n")
local tmp = {}

-- process pager suffixes
for name, data in tournament_table do
	if data.games_started == 0 then 
		local _, _, stripped_name = string.find(name, "(.*) P%d+$")
		if stripped_name and stripped_name ~= name and tournament_table[stripped_name] then
			tournament_table[stripped_name].time_online = 
				tournament_table[stripped_name].time_online + data.time_online
			data = nil
		end
	end
end

for name, data in tournament_table do
	-- calculate score
	data.score = data.win_count * 10 + (data.games_finished - data.win_count) * 3 + 
		math.sqrt(data.time_online - data.battle_time) / 20
	if data.games_finished > 0 then table.insert(tmp, {name, data}) end
end
table.sort(tmp, function(a, b) return a[2].score > b[2].score end)
tournament_table = tmp

for index, data in tournament_table do
	out:write(string.format(
		"<tr><td>%d<td>%s<td>%d<td>%d<td>%s<td>%s<td>%d\n", 
		index,
		data[1], 
		data[2].games_finished,
		data[2].win_count,
		timestring(data[2].time_online - data[2].battle_time),
		timestring(data[2].battle_time),
		math.floor(data[2].score)))
end
out:write("</table>")

-- complete table of played games
out:write("<br>")
out:write("<b>UFO2000 played games statistics table</b><br>")
out:write("<table border=1>")
out:write("<tr><td>version<td>player 1<td>player 2<td>terrain type<td>winner<td>time<td>comment\n")
for k, game_info in ipairs(games_history) do
	local attrib = ""
	if game_info.version_error then attrib = attrib .. "ver<br>" end
	if game_info.crc_error then attrib = attrib .. "crc problems<br>" end
	if game_info.crash_error then attrib = attrib .. "game crashed<br>" end
	if game_info.assert_error then attrib = attrib .. game_info.assert_error .. "<br>" end
	if game_info.connection_error then attrib = attrib .. "connection lost<br>" end
	if not string.find(attrib, "ver") then
		out:write(string.format(
			"<tr><td>%d<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s\n", 
			game_info.version, 
			game_info.p1,
			game_info.p2,
			game_info.terrain or "-",
			game_info.winner or "-",
			timestring(os.difftime(game_info.end_time, game_info.start_time)), 
			attrib))
	end
end
out:write("</table>")

-- display terrain types popularity statistics
out:write("<br>")
out:write("<b>Terrain types statistics table</b><br>")
local count = 0
local tmp = {} 
for k, v in terrain_table do 
	count = count + v 
	table.insert(tmp, {k, v}) 
end 
terrain_table = tmp
table.sort(terrain_table, function (a, b) return a[2] > b[2] end)
out:write("<table border=1>")
out:write("<tr><td>terrain type<td>number of times used<td>percent share\n") 
for k, v in ipairs(terrain_table) do
	out:write(string.format("<tr><td>%s<td>%d<td>%.1f%%", v[1], v[2], v[2] / count * 100))
end
out:write("</table>")

-- display operating systems popularity statistics
out:write("<br>")
out:write("<b>Operating systems statistics table</b><br>")
local count = 0
local tmp = {} 
for os_name, os_users in os_table do 
	local n = 0
	local userlist = nil
	for k, v in os_users do 
		if userlist == nil then userlist = k else userlist = userlist .. ", " .. k end
		n = n + 1 
	end
	count = count + n
	table.insert(tmp, {os_name, userlist, n}) 
end 
terrain_table = tmp
table.sort(terrain_table, function (a, b) return a[3] > b[3] end)
out:write("<table border=1>")
out:write("<tr><td>operating system<td>number of people using it<td>percent share\n") 
for k, v in ipairs(terrain_table) do
	out:write(string.format("<tr><td>%s<td>%d<td>%.1f%%", v[1], v[3], v[3] / count * 100))
end
out:write("</table>")

-- game versions stability statistics
out:write("<br>")
out:write("<b>UFO2000 versions stability comparison table</b><br>")

local versions = {}
for k, game_info in ipairs(games_history) do
	-- require that the game must last at least 5 minutes
	if not game_info.version_error then
		if not versions[game_info.version] then 
			versions[game_info.version] = {total = 0, fail = 0, bad_fail = 0}
		end
		local vinfo = versions[game_info.version];
		vinfo.version = game_info.version
		local fail_flag, bad_fail_flag
		for k, v in game_info do
			if string.find(k, "_error$") then 
				vinfo[k] = (vinfo[k] or 0) + 1 
				fail_flag = 1
				if k ~= "connection_error" then bad_fail_flag = 1 end
			end
		end
		if fail_flag then
			vinfo.fail = vinfo.fail + 1 
			if bad_fail_flag then vinfo.bad_fail = vinfo.bad_fail + 1 end
			vinfo.total = vinfo.total + 1
		elseif game_info.winner or os.difftime(game_info.end_time, game_info.start_time) > 3 * 60 then
			vinfo.total = vinfo.total + 1
		end
	end
end

local tmp = {} for k, v in versions do table.insert(tmp, v) end versions = tmp

table.sort(versions, 
	function (a, b)
		return a.version < b.version
	end)

out:write("<table border=1>")
out:write("<tr><td>version<td>games played<td>problems encountered<td>internal game problems (crc errors or crashes)\n") 
for k, v in ipairs(versions) do
	out:write(string.format(
		"<tr><td>%d<td>%d<td>%d<td>%d\n", 
		v.version, 
		v.total,
		v.fail,
		v.bad_fail))
end
out:write("</table>")
out:write(string.format("<br>report generated on %s<br>server log parsing performed for %.2f seconds", os.date(), os.clock()))
out:write("</body></html>")
out:close()
