AddXcomTerrain {
	Name = "Modified City",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/roads.*",
		"$(xcom)/terrain/urbits.*",
		"$(xcom)/terrain/urban.*",
		"$(xcom)/terrain/frniture.*"
	},
	Maps = {
		"$(xcom)/maps/urban00.map",
		"$(xcom)/maps/urban01.map",
		"$(xcom)/maps/urban02.map",
		"$(xcom)/maps/urban03.map",
		"$(xcom)/maps/urban04.map",
		"$(xcom)/maps/urban05.map",
		"$(xcom)/maps/urban06.map",
		"$(xcom)/maps/urban07.map",
		"$(xcom)/maps/urban08.map",
		"$(xcom)/maps/urban09.map",
		"$(xcom)/maps/urban14.map",
		"$(xcom)/maps/urban15.map",
		"$(xcom)/maps/urban16.map",
		"$(xcom)/maps/urban17.map",
		"$(xcom)/maps/urban18.map",
		"$(ufo2000)/newmaps/urban19.map",
		"$(ufo2000)/newmaps/urban20.map",
		"$(ufo2000)/newmaps/urban21.map",
		"$(ufo2000)/newmaps/urban22.map",
		"$(ufo2000)/newmaps/urban23.map",
		"$(ufo2000)/newmaps/urban24.map"
	},
	MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] = 1 end
			end
	
			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = 0 end
			end
	
			if (x and y) then
				map[x][y] = 2
			end
		end

		local function random_normal()
			return random {03, 04, 14, 15, 16, 17, 18, 21, 22}
		end	

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 2 and map[a][y] > 2 and map[x][b] > 2 and map[a][b] > 2) then
				map[x][y] = random {05, 06, 07, 08, 09, 19, 20, 23, 24}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_normal()
			end
		end

		add_roads(tmp.SizeX, tmp.SizeY, tmp.Mapdata)

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					random_double(i, j, tmp.Mapdata)
				end
			end
		end

		return tmp
	end
}
