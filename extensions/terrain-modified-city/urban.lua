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
		"$(extension)/urban19.map",
		"$(extension)/urban20.map",
		"$(extension)/urban21.map",
		"$(extension)/urban22.map",
		"$(extension)/urban23.map",
		"$(extension)/urban24.map",
		"$(extension)/urban25.map"
	},
	MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do
                    if (i == size_y) then
                        map[x][i] = random {01, 21}
                    else
                        map[x][i] = random {01, 01, 22}
                    end
                end
			end
	
			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = random {00, 00, 25} end
			end
	
			if (x and y) then
				map[x][y] = 2
			end
		end

		local function random_normal()
			return random {03, 04, 14, 15, 16, 17, 18}
		end	

		local function random_double(x, y, map)
            local do_not_place = {-1, 00, 01, 05, 06, 07, 08, 09, 19, 20, 21, 22, 23, 24, 25}
            local dnp_num = 15

            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 1, dnp_num do
                        if (map[x + i][y + j] == do_not_place[k]) then
                            return
                        end
                    end
                end
			end

			map[x][y] = random {05, 06, 07, 08, 09, 19, 20, 23, 24}
		    map[x + 1][y] = -1
			map[x][y + 1] = -1
            map[x + 1][y + 1] = -1
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
