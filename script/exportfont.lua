-- script for X-COM font data files loading and conversion

local xlat = {[0] = ".", [1] = "#", [2] = "$", [3] = "%", [4] = "*", [5] = "+"}

function export_file(filename, width, height)
	local f = io.open(filename, "rb")
	local size = f:seek("end")
	assert(math.mod(size, width * height) == 0)
	f:seek("set")
	local count = size / (width * height)

	io.write("XCOM_Font(\"", filename, "\", ", count, ", ", width, ", ", height, ",\n{\n")
	for index = 1, count do
		io.write("\t[", index, "] =\n\t{\n")
		for i = 1, height do
			io.write("\t\t\"")
			for j = 1, width do
				local ch = xlat[string.byte(f:read(1))]
				io.write(ch)
			end
			io.write("\",\n")
		end
		io.write("\t},\n")
	end

	io.write("})\n\n")
end

io.write([[
local xlat = {["."] = 0, ["#"] = 1, ["$"] = 2, ["%"] = 3, ["*"] = 4, ["+"] = 5}

function XCOM_Font(filename, count, width, height, tbl)
	local f = io.open(filename, "wb")
	assert(table.getn(tbl) == count)
	for index = 1, count do
		assert(table.getn(tbl[index]) == height)
		for i = 1, height do
			assert(string.len(tbl[index][i]) == width)
			f:write((string.gsub(tbl[index][i], ".", function (x) return string.char(xlat[x]) end)))
		end
	end
end

]])

export_file("../datfile/fonts/biglets.cyr", 16, 16)
export_file("../datfile/fonts/smallset.cyr", 8, 9)
