local games = {}
local total_games = 0

function startgame(p1, p2)
	local game_info = {["p1"] = p1, ["p2"] = p2, ["exited"] = {}, ["tcrc"] = {}}
	game_info.tcrc[p1] = ""
	game_info.tcrc[p2] = ""
	games[p1] = game_info
	games[p2] = game_info
	total_games = total_games + 1
end

function endgame(p)
	local game_info = games[p]
	assert(game_info)
	if not game_info then return end
	games[p] = nil
	game_info.exited[p] = 1
	if game_info.exited[game_info.p1] and game_info.exited[game_info.p2] then
		local attrib = ""
		if game_info.version_error then attrib = attrib .. " ver" end
		if game_info.crc_error then attrib = attrib .. " crc" end
		if game_info.connection_error then attrib = attrib .. " net" end
		if game_info.crash_error then attrib = attrib .. " bug" end
		if game_info.tcrc[game_info.p1] ~= game_info.tcrc[game_info.p2] then attrib = attrib .. " map" end
--		if not string.find(attrib, "ver") then
			io.write(string.format("%d %-64s %s\n", total_games, game_info.p1 .. " vs " .. game_info.p2, attrib))
--		end
		total_games = total_games - 1
	end
end

local line_number = 0
for l in io.lines(arg[1]) do
	line_number = line_number + 1
	-- handle game start
	local _, _, p1, p2 = string.find(l, "game start: '(.-)' vs '(.-)'")
	if p1 and p2 then
		if games[p1] ~= nil or games[p2] ~= nil then
			print("error at line ", line_number)
		end
		startgame(p1, p2)
	end

	-- handle version check error
	local _, _, p = string.find(l, "packet from (.-) {id=12, data=UFO2000 VERSION CHECK FAILED!")
	if p then games[p].version_error = true	end

	-- handle crc error
	local _, _, p = string.find(l, "packet from (.-) {id=14, data=crc error}")
	if p then games[p].crc_error = true end

	-- handle crash
	local _, _, p = string.find(l, "packet from (.-) {id=14, data=crash}")
	if p then games[p].crash_error = true end

	-- handle disconnect
	local _, _, p = string.find(l, "connection closed %(name='(.-)', max_ave_traffic=")
	if p then
		if games[p] then
			games[p].connection_error = true
			endgame(p)
		end
	end

	-- handle normal game exit
	local _, _, p = string.find(l, "packet from (.-) {id=12, data=_Xcom_QUIT_}")
	if p then endgame(p) end

	local _, _, p = string.find(l, "packet from (.-) {id=12, data=QUIT")
	if p then endgame(p) end

	-- handle terrain crc info
	local _, _, p, tcrc = string.find(l, "packet from (.-) {id=12, data=_Xcom_TCRC_(%d+_%d+_%d+)")
	if p then
		games[p].tcrc[p] = games[p].tcrc[p] .. tcrc
	end
end

print("\nThe following players are still playing:\n")
for k, v in games do
	print(k)
end
