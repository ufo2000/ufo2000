AddXcomTerrain {
	Name = "Airfield",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(tftd)/terrain/port01.*",
		"$(tftd)/terrain/port02.*",
		"$(xcom)/terrain/plane.*",
		"$(xcom)/terrain/xbase2.*"
	},
	Maps = {
		"$(extension)/air00.map",
		"$(extension)/air01.map",
		"$(extension)/air02.map",
		"$(extension)/air03.map",
		"$(extension)/air04.map",
		"$(extension)/air05.map",
		"$(extension)/air06.map",
		"$(extension)/air07.map",
		"$(extension)/air08.map",
		"$(extension)/air09.map",
		"$(extension)/air10.map",
		"$(extension)/air11.map",
		"$(extension)/air12.map",
		"$(extension)/air13.map",
		"$(extension)/air14.map",
		"$(extension)/air15.map",
		"$(extension)/air16.map"
	},
	MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local row = {}
            local col

            if (math.random(1, 2) ~= 1) then
                col = math.random(1, size_y / 2)
                col = (col * 2) - 1
                
                map[size_x - 3][col] = 12
                map[size_x - 2][col] = -1
                map[size_x - 3][col + 1] = -1
                map[size_x - 2][col + 1] = -1
                
                map[size_x - 1][col] = 11
                map[size_x][col] = -1
                map[size_x - 1][col + 1] = -1
                map[size_x][col + 1] = -1
            end

            for i = 1, 2 do
                if (col) then
                    if (i > 1 and size_x >= 4 + 2) then
                        if (math.random(1, 2) ~= 1) then
                            row[i] = math.random(1, (size_x - 4) / 2)
                            row[i] = (row[i] * 2) - 1
                        end
                    else
                        row[i] = size_x - 1
                    end
                else
                    if (math.random(1, 3 - i) ~= 1) then
				        row[i] = math.random(1, size_x / 2)
				        row[i] = (row[i] * 2) - 1
                    end
                end
                
                if (row[i]) then
	       			for j = 1, size_y, 2 do
                        if (j == size_y and not col) then
                            break
                        end
                        
                        if (map[row[i]][j] ~= 11) then
                            if (j == size_y - 2 and not col) then
                                map[row[i]][j] = random {05, 16}
                            elseif (j == size_y - 1) then
                                map[row[i]][j] = random {01, 02, 05, 16}
                            else
                                map[row[i]][j] = random {01, 02}
                            end
                            
                            map[row[i] + 1][j] = -1
                            map[row[i]][j + 1] = -1
                            map[row[i] + 1][j + 1] = -1
                        end
                    end
                end
			end


		end

		local function random_normal()
			return random {00, 03, 04, 06, 08, 09, 13}
		end

		local function random_double(x, y, map)
            local do_not_place = {-1, 01, 02, 05, 07, 10, 11, 12, 14, 15, 16}
            local dnp_num = 11

            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 1, dnp_num do
                        if (map[x + i][y + j] == do_not_place[k]) then
                            return
                        end
                    end
                end
			end

			map[x][y] = random {07, 10, 14, 15}
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
				if (math.random(2, 3) > 2) then
					random_double(i, j, tmp.Mapdata)
				end
			end
		end

		return tmp
	end
}
