local function add_roads(size_x, size_y, map)
        local x, y

        if (math.random(1, 10) ~= 1) then
                x = math.random(1, size_x)
                for i = 1, size_y do map[x][i] = 0 end
        end

        if (math.random(1, 10) ~= 1) then
                y = math.random(1, size_y)
                for i = 1, size_x do map[i][y] = 1 end
        end

        if (x and y) then
                map[x][y] = 2
        end
end

local function random_normal()
	if (math.random(1, 7) > 2) then
		return math.random(14, 18)
	else
		return math.random(3, 4)
	end
end

local function random_double(x, y, map)
	a = x + 1
	b = y + 1
	if (map[x][y] > 2 and map[a][y] > 2 and map[x][b] > 2 and map[a][b] > 2) then
		map[x][y] = math.random(5,9)
		map[a][y] = -1
		map[x][b] = -1
		map[a][b] = -1
	end
end

function map(size)
        local tmp = { Name = "City", Mapdata = {} }
        size = size or math.random(4, 6)
        tmp.Width = size
        tmp.Height = size
        for i = 1, tmp.Height do
                tmp.Mapdata[i] = {}
                for j = 1, tmp.Width do
                        tmp.Mapdata[i][j] = random_normal()
                end
        end

        add_roads(tmp.Width, tmp.Height, tmp.Mapdata)

	for i = 1, tmp.Height - 1 do
		for j = 1, tmp.Width - 1 do
			if (math.random(1, 12) > 8) then
				random_double(i, j, tmp.Mapdata)
			end
		end
	end

        return tmp
end