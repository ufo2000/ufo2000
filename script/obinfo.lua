#!/bin/lua

require "obdata.lua"

-- script used for analyzing data structures

function group_items(proplist)
	local groups = {}
	for k, v in ObjectsTable do
		local id
		for x, y in proplist do
			if not id then id = "" else id = id .. " " end
			id = id .. y .. "=" .. v[y]
		end

		if not groups[id] then groups[id] = {} end
		table.insert(groups[id], v.name)
	end
		
	io.write(string.rep("-", 70), "\n")
	for k, v in groups do
		io.write("(", k, "):\n")
		local text = ""
		for k2, v2 in v do
			text = text .. "\"" .. v2 .. "\" "
			if string.len(text) > 60 then io.write("\t", text, "\n") text = "" end
		end
		if text ~= "" then io.write("\t", text, "\n") end
	end
end

-- some sample calls to group_items() function to show how it works

group_items{"_1E"}
group_items{"_29"}
group_items{"_2A"}
group_items{"_2D"}
group_items{"_35"}
group_items{"primeType"}
group_items{"isAlien", "isWeapon", "isGun", "isShootable"}
