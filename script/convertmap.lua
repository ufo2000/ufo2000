-- a small script that reads x-com .map files and converts it into a script
-- with human readable map representation which can be edited and executed
-- to get modified map back into x-com format

-- function that assigns ASCII character to cell info
-- it is better to call this function for widely used cells first, so these 
-- cells will be assigned the most suitable characters

-- preferable images for different cell types
local floor  = ".,:;*+-=%/^|<>'`~^_()"
local wall   = "abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ"
local object = "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞß0123456789"

local charmap = {}

function assign_char(cellinfo)
	local chr = charmap[cellinfo]
	if chr then return chr end

	local cell1 = string.byte(cellinfo, 1)
	local cell2 = string.byte(cellinfo, 2)
	local cell3 = string.byte(cellinfo, 3)
	local cell4 = string.byte(cellinfo, 4)

	if cell4 ~= 0 then
		object = string.gsub(object, "%Z", function (x) if chr then return x end chr = x return "\000" end)
	elseif cell2 ~= 0 or cell3 ~= 0 then
		wall = string.gsub(wall, "%Z", function (x) if chr then return x end chr = x return "\000" end)
	elseif cell1 ~= 0 then
		floor = string.gsub(floor, "%Z", function (x) if chr then return x end chr = x return "\000" end)
	else
		chr = " "
	end

	assert(chr ~= nil)

	charmap[cellinfo] = chr
	return chr
end

-- open map file and read dimensions
f = io.open(arg[1], "rb")

size_x = string.byte(f:read(1))
size_y = string.byte(f:read(1))
size_z = string.byte(f:read(1))

io.write("size_x, size_y, size_z = ", size_x, ", ", size_y, ", ", size_z, "\n\n")

local nextchar = string.byte(" ")
local count = 0

io.write("mapdata =\n{\n")
for z = 1, size_z do
	io.write("\t{\n")
	for x = 1, size_x do
		io.write("\t\t\"")
		for y = 1, size_y do
			local cellinfo = f:read(4)
			assert(string.len(cellinfo) == 4)
			local chr = assign_char(cellinfo)
			io.write(chr)
		end
		io.write("\",\n")
	end
	io.write("\t},\n")
end
io.write("}\n\n")

io.write("celldata =\n{\n")
for k, v in charmap do
	io.write("\t[\"", v, "\"] = {", string.byte(k, 1), ",", string.byte(k, 2), ",",
		string.byte(k, 3), ",", string.byte(k, 4), "},\n")
end
io.write("}\n\n")

-- append a piece of code which can dump map data back into x-com format when
-- filename is passed in a command line

io.write([[
if arg[1] ~= nil then
	local f = io.open(arg[1], "wb")
	f:write(string.char(size_x), string.char(size_y), string.char(size_z))
	for z = 1, size_z do
		for x = 1, size_x do
			for y = 1, size_y do
				local cell = celldata[string.sub(mapdata[z][x], y, y)]
				f:write(string.char(cell[1]), string.char(cell[2]), 
					string.char(cell[3]), string.char(cell[4]))
			end
		end
	end
	f:close()
end
]])
