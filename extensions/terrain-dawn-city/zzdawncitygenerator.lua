AddXcomTerrain {
	Name = "Dawn City Beta",
	Tiles =	{
		"destroyed",
		"dawn"
	},
	Maps = {
		"$(extension)/dawn00.map",
		"$(extension)/dawn01.map",
		"$(extension)/dawn02.map",
		"$(extension)/dawn03.map",
		"$(extension)/dawn04.map",
		"$(extension)/dawn05.map",
		"$(extension)/dawn06.map",
		"$(extension)/dawn07.map",
		"$(extension)/dawn08.map",
		"$(extension)/dawn09.map",
		"$(extension)/dawn10.map",
		"$(extension)/dawn11.map",
		"$(extension)/dawn12.map",
		"$(extension)/dawn13.map",
		"$(extension)/dawn14.map",
		"$(extension)/dawn15.map",
		"$(extension)/dawn16.map",
		"$(extension)/dawn17.map",
		"$(extension)/dawn18.map",
		"$(extension)/dawn19.map",
		"$(extension)/dawn20.map",
		"$(extension)/dawn21.map",
		"$(extension)/dawn22.map",
		"$(extension)/dawn23.map",
		"$(extension)/dawn24.map",
		"$(extension)/dawn25.map",
		"$(extension)/dawn26.map",
		"$(extension)/dawn27.map",
		"$(extension)/dawn28.map",
		"$(extension)/dawn29.map",
		"$(extension)/dawn30.map",
		"$(extension)/dawn31.map",
		"$(extension)/dawn32.map",
		"$(extension)/dawn33.map",
		"$(extension)/dawn34.map",
		"$(extension)/dawn35.map",
		"$(extension)/dawn36.map",
		"$(extension)/dawn37.map",
		"$(extension)/dawn38.map",
		"$(extension)/dawn39.map",
		"$(extension)/dawn40.map",
		"$(extension)/dawn41.map",
		"$(extension)/dawn42.map",
		"$(extension)/dawn43.map",
		"$(extension)/dawn44.map",
		"$(extension)/dawn45.map",
		"$(extension)/dawn46.map",
		"$(extension)/dawn47.map",
		"$(extension)/dawn48.map",
		"$(extension)/dawn49.map",
		"$(extension)/dawn50.map",
		"$(extension)/dawn51.map",
		"$(extension)/dawn52.map",
		"$(extension)/dawn53.map",
		"$(extension)/dawn54.map",
		"$(extension)/dawn55.map",
		"$(extension)/dawn56.map",
		"$(extension)/dawn57.map",
		"$(extension)/dawn58.map",
		"$(extension)/dawn59.map",
		"$(extension)/dawn60.map",
		"$(extension)/dawn61.map",
		"$(extension)/dawn62.map",
		"$(extension)/dawn63.map",
		"$(extension)/dawn64.map",
		"$(extension)/dawn65.map",
		"$(extension)/dawn66.map",
		"$(extension)/dawn67.map",
		"$(extension)/dawn68.map",
		"$(extension)/dawn69.map",
		"$(extension)/dawn70.map",
		"$(extension)/dawn71.map",
		"$(extension)/dawn72.map",
		"$(extension)/dawn73.map",
		"$(extension)/dawn74.map",
		"$(extension)/dawn75.map",
		"$(extension)/dawn76.map",
		"$(extension)/dawn77.map",
		"$(extension)/dawn78.map",
		"$(extension)/dawn79.map",
		"$(extension)/dawn80.map",
		"$(extension)/dawn81.map",
		"$(extension)/dawn82.map",
		"$(extension)/dawn83.map",
		"$(extension)/dawn84.map",
		"$(extension)/dawn85.map",
		"$(extension)/dawn86.map",
		"$(extension)/dawn87.map",
		"$(extension)/dawn88.map",
		"$(extension)/dawn89.map",
		"$(extension)/dawn90.map",
		"$(extension)/dawn91.map",
		"$(extension)/dawn92.map",
		"$(extension)/dawn93.map",
		"$(extension)/dawn94.map",
		"$(extension)/dawn95.map",
		"$(extension)/dawn96.map",
		"$(extension)/dawn97.map",
		"$(extension)/dawn98.map",
		"$(extension)/dawn99.map"

	},

	MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y
	
			if (math.random(1, size_x) > 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] = random {09, 09, 06, 09, 07, 08} end
			end
	
			if (math.random(1, size_y) > 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = random {01, 01, 00, 03, 01, 02} end
			end
			
			if (x and y) then
				map[x][y] = random {12, 13}
			end

			if (x == 1 and y == 1) then 
				map[x][y] = 17
			end

			if (x == 1 and y == size_y) then
				map[x][y] = 15
			end

			if (x == size_x and y == 1) then
				map[x][y] = 16
			end

			if (x == size_x and y == size_y) then
				map[x][y] = 14
			end

		end

		local function random_10x10()
			return random {26, 27, 28, 35, 36, 37, 43, 47, 50, 53}
		end	

		local function random_20x20(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 25 and map[a][y] > 25 and map[x][b] > 25 and map[a][b] > 25 and map[x][y] < 56 and map[x][b] < 56 and map[a][y] and map[a][b] < 56 ) then
				map[x][y] = random {56, 57, 58, 59}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end

		local function random_20x30(x, y, map)
			local a = x + 1
			local b = y + 1
			local d = y + 2

			if (map[x][y] > 25 and map [x][y] < 56 and map [x][b] > 25 and map[x][b] < 56 and map[x][d] > 25 and map[x][d] < 56 and map[a][y] > 25 and map[a][y] < 56 and map[a][b] > 25 and map[a][b] < 56 and map[a][d] > 25 and map[a][d] < 56) then
				map[x][y] = random {60, 61}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
				map[a][d] = -1
				map[x][d] = -1
			end
		end

		local function corner_lse(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3
			local f = y + 3

			if (map[x][y] > 9 and map[x][y] < 12 and map[e][b] < 56 and map [e][d] < 56 and map [e][f] < 56 and map [a][f] < 56 and map [c][f] < 56) then
				map[a][f] = 0
				map[c][f] = 2
				map[e][b] = 6
				map[e][d] = 8	
				map[a][b] = random {56, 57, 58, 59}
				map[a][d] = -1
				map[c][b] = -1
				map[c][d] = -1
				map[e][y] = 20
				map[x][f] = 19
				map[e][f] = 14
			end
		end

		local function corner_lne(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3
			local f = y + 3

			if (map[e][y] > 11 and map[e][y] < 14 and map [x][b] > 25 and map[x][b] < 56 and map [x][d] > 25 and map [x][d] < 56 and map [x][f] > 25 and map [x][f] < 56 and map [a][f] > 25 and map [a][f] < 56 and map [c][f] > 25 and map [c][f] < 56) then
				map[a][f] = 2
				map[c][f] = 0
				map[x][b] = 8
				map[x][d] = 6
				map[a][b] = random {56, 57, 58, 59}
				map[a][d] = -1
				map[c][b] = -1
				map[c][d] = -1
				map[x][y] = 20
				map[x][f] = 15
				map[e][f] = 18
			end
		end

		local function corner_lnw(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3
			local f = y + 3

			if (map[e][f] > 11 and map[e][f] < 14 and map [x][b] > 25 and map[x][b] < 56 and map [x][d] > 25 and map [x][d] < 56 and map [x][y] > 25 and map [x][y] < 56 and map [a][y] > 25 and map [a][y] < 56 and map [c][y] > 25 and map [c][y] < 56) then
				map[a][y] = 0
				map[c][y] = 2
				map[x][b] = 6
				map[x][d] = 8
				map[a][b] = random {56, 57, 58, 59}
				map[a][d] = -1
				map[c][b] = -1
				map[c][d] = -1
				map[x][y] = 17
				map[x][f] = 21
				map[e][y] = 18
			end
		end

		local function corner_lsw(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3
			local f = y + 3

			if (map[x][f] > 11 and map[x][f] < 14 and map [a][y] > 25 and map[a][y] < 56 and map [c][y] > 25 and map [c][y] < 56 and map [e][y] > 25 and map [e][y] < 56 and map [e][b] > 25 and map [e][b] < 56 and map [e][d] > 25 and map [e][d] < 56) then
				map[a][y] = 2
				map[c][y] = 0
				map[e][b] = 8
				map[e][d] = 6
				map[a][b] = random {56, 57, 58, 59}
				map[a][d] = -1
				map[c][b] = -1
				map[c][d] = -1
				map[e][y] = 16
				map[x][y] = 19
				map[e][f] = 21
			end
		end

		local function corner_nw(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][y] > 25 and map[x][y] < 51 and map[c][y] > 5 and map[c][y] < 9 and map [c][d] > 11 and map [c][d] < 14 and map [x][d] > -1 and map [x][d] < 4) then
				map[a][y] = 0
				map[x][b] = 8
				map[a][b] = random {26, 27, 28, 35, 36, 37, 43}
				map[x][d] = 21
				map[c][y] = 18
				map[x][y] = 17
			end
		end

		local function corner_ne(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map [c][y] > 25 and map[c][y] < 51 and map[x][y] > 5 and map[x][y] < 9 and map [x][d] > 10 and map [x][d] < 14 and map [c][d] > -1 and map [c][d] < 4) then
				map[a][y] = 2
				map[c][b] = 6
				map[a][b] = random {26, 27, 28, 35, 36, 37, 43}
				map[x][y] = 19
				map[c][d] = 21
				map[c][y] = 16
			end
		end

		local function corner_sw(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][d] > 25 and map[x][d] < 51 and map[c][d] > 5 and map[c][d] < 9 and map [c][y] > 11 and map [c][y] < 14 and map [x][y] > -1 and map [x][y] < 4) then
				map[a][d] = 0
				map[x][b] = 8
				map[a][b] = random {26, 27, 28, 35, 36, 37, 43}
				map[x][d] = 15
				map[c][d] = 18
				map[x][y] = 20
			end
		end

		local function corner_se(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[c][d] > 25 and map[c][d] < 51 and map[x][d] > 5 and map[x][d] < 9 and map [x][y] > 11 and map [x][y] < 14 and map [c][y] > -1 and map [c][y] < 4) then
				map[a][d] = 2
				map[c][b] = 6
				map[a][b] = random {26, 27, 28, 35, 36, 37, 43}
				map[x][d] = 19
				map[c][d] = 14
				map[c][y] = 20
			end
		end

		local function sroad_s(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3

			if (map[x][y] < 12 and map[x][y] > -1 and map[x][b] < 12 and map[x][b] > -1 and map[x][d] < 12 and map[x][d] > -1 and map[a][b] > 25 and map[a][b] < 51 and map[c][b] > 25 and map[c][b] < 51 and map[e][b] > 25 and map[e][b] < 51 and map[e][y] > 25 and map[e][y] < 51 and map[e][d] > 25 and map[e][d] < 51) then
				map [x][b] = 19	
				map [a][b] = 3
				map [c][b] = 0
				map [e][b] = 18
				map [e][y] = 11		
				map [e][d] = 10

			end
		end

		local function sroad_n(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local e = x + 3

			if (map[e][y] < 12 and map[e][y] > -1 and map[e][b] < 12 and map[e][b] > -1 and map[e][d] < 12 and map[e][d] > -1 and map[a][b] > 25 and map[a][b] < 51 and map[x][b] > 25 and map[x][b] < 51 and map[c][b] > 25 and map[c][b] < 51 and map[x][y] > 25 and map[x][y] < 51 and map[x][d] > 25 and map[x][d] < 51) then
				map [x][b] = 19
				map [x][y] = 11
				map [x][d] = 10
				map [a][b] = 0
				map [c][b] = 3
				map [e][b] = 18

			end
		end

		local function sroad_w(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local f = y + 3

			if (map[c][f] < 12 and map[c][f] > -1 and map[a][f] < 12 and map[a][f] > -1 and map[x][f] < 12 and map[x][f] > -1 and map[a][y] > 25 and map[a][y] < 51 and map[a][b] > 25 and map[a][b] < 51 and map[a][d] > 25 and map[a][d] < 51 and map[c][y] > 25 and map[c][y] < 51 and map[x][y] > 25 and map[x][y] < 51) then
				map [a][f] = 21
				map [c][y] = 4
				map [x][y] = 5
				map [a][b] = 6
				map [a][d] = 9
				map [a][y] = 20

			end
		end

		local function sroad_e(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2
			local f = y + 3

			if (map[c][y] < 12 and map[c][y] > -1 and map[a][y] < 12 and map[a][y] > -1 and map[x][y] < 12 and map[x][y] > -1 and map[a][f] > 25 and map[a][f] < 51 and map[a][b] > 25 and map[a][b] < 51 and map[a][d] > 25 and map[a][d] < 51 and map[c][f] > 25 and map[c][f] < 51 and map[x][f] > 25 and map[x][f] < 51) then
				map [a][f] = 21
				map [c][f] = 4
				map [x][f] = 5
				map [a][b] = 9
				map [a][d] = 6
				map [a][y] = 20

			end
		end

		local function paralel_roadv(x, y, map)
			local a = x + 1

			if (map [x][y] == 4 and map [a][y] == 5) then 
				map [x][y] = random {00, 01, 03}	
				map [a][y] = random {00, 01, 03}

			end
		end

		local function paralel_roadh(x, y, map)
			local b = y + 1

			if (map [x][y] == 10 and map [x][b] == 11) then 
				map [x][y] = random {06, 07, 09}
				map [x][b] = random {06, 07, 09}

			end
		end

		local function alley_e(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][y] < 12 and map[x][y] > -1 and map[x][b] < 12 and map[x][b] > -1 and map[x][d] < 12 and map[x][d] > -1 and map[a][b] > 25 and map[a][b] < 51 and map[c][b] > 25 and map[c][b] < 51 and map[a][y] > 25 and map[a][y] < 51 and map[a][d] > 25 and map[a][d] < 51) then
				map [x][b] = 19
				map [a][b] = 0
				map [c][b] = 22

			end
		end

		local function alley_w(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[c][y] < 12 and map[c][y] > -1 and map[c][b] < 12 and map[c][b] > -1 and map[c][d] < 12 and map[c][d] > -1 and map[a][b] > 25 and map[a][b] < 51 and map[x][b] > 25 and map[x][b] < 51 and map[a][y] > 25 and map[a][y] < 51 and map[a][d] > 25 and map[a][d] < 51) then
				map [x][b] = 24
				map [a][b] = 0
				map [c][b] = 18

			end
		end

		local function alley_s(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][y] < 12 and map[x][y] > -1 and map[a][y] < 12 and map[a][y] > -1 and map[c][y] < 12 and map[c][y] > -1 and map[a][b] > 25 and map[a][b] < 51 and map[a][d] > 25 and map[a][d] < 51 and map[x][b] > 25 and map[x][b] < 51 and map[c][b] > 25 and map[c][b] < 51) then
				map [a][y] = 20
				map [a][b] = 6
				map [a][d] = 23

			end
		end

		local function alley_n(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][d] < 12 and map[x][d] > -1 and map[a][d] < 12 and map[a][d] > -1 and map[c][d] < 12 and map[c][d] > -1 and map[a][y] > 25 and map[a][y] < 51 and map[a][b] > 25 and map[a][b] < 51 and map[x][b] > 25 and map[x][b] < 51 and map[c][b] > 25 and map[c][b] < 51) then
				map [a][y] = 25
				map [a][b] = 6
				map [a][d] = 21

			end
		end

		local function merger_n(x, y, map)
			local a = x + 1
			
			if (map [x][y] > 5 and map[x][y] < 10 and map [a][y] == 5) then 
				map [x][y] = 19
				map [a][y] = 0

			end
		end

		local function merger_na(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 22 and map [a][y] == 5) then 
				map [x][y] = 2	
				map [a][y] = 0

			end
		end

		local function merger_ne(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 16 and map [a][y] == 5) then 
				map [x][y] = 18	
				map [a][y] = 0

			end
		end

		local function merger_nw(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 14 and map [a][y] == 5) then 
				map [x][y] = 19	
				map [a][y] = 0

			end
		end

		local function merger_s(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 4 and map[a][y] > 5 and map [a][y] < 10 ) then 
				map [x][y] = 0	
				map [a][y] = 18

			end
		end

		local function merger_sa(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 4 and map[a][y] == 24) then 
				map [x][y] = 0	
				map [a][y] = 2

			end
		end

		local function merger_se(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 4 and map [a][y] == 17) then 
				map [x][y] = 0		
				map [a][y] = 20

			end
		end

		local function merger_sw(x, y, map)
			local a = x + 1
			
			if (map [x][y] == 4 and map [a][y] == 15) then 
				map [x][y] = 0
				map [a][y] = 21

			end
		end

		local function merger_e(x, y, map)
			local b = y + 1

			if (map [x][y] == 10 and map [x][b] > -1 and map[x][b] < 5) then 
				map [x][y] = 6
				map [x][b] = 21
			end
		end

		local function merger_ea(x, y, map)
			local b = y + 1

			if (map [x][y] == 10 and map [x][b] == 25) then 
				map [x][y] = 6
				map [x][b] = 8
			end
		end

		local function merger_en(x, y, map)
			local b = y +1
			
			if (map [x][y] == 10 and map [x][b] == 16) then 
				map [x][y] = 6
				map [x][b] = 18
			end
		end

		local function merger_es(x, y, map)
			local b = y + 1
			
			if (map [x][y] == 10 and map [x][b] == 17) then 
				map [x][y] = 6
				map [x][b] = 19
			end
		end

		local function merger_w(x, y, map)
			local b = y + 1

			if (map [x][b] == 11 and map [x][y] > -1 and map[x][y] < 5) then 
				map [x][y] = 20
				map [x][b] = 6
			end
		end

		local function merger_wa(x, y, map)
			local b = y + 1

			if (map [x][b] == 11 and map [x][y] == 23) then 
				map [x][y] = 8
				map [x][b] = 6
			end
		end

		local function merger_wn(x, y, map)
			local b = y + 1
			
			if (map [x][y] == 14 and map [x][b] == 11) then 
				map [x][y] = 18
				map [x][b] = 6
			end
		end

		local function merger_ws(x, y, map)
			local b = y + 1
			
			if (map [x][y] == 15 and map [x][b] == 11) then 
				map [x][y] = 19
				map [x][b] = 6
			end
		end


		local function roadfix(x, y, map)
			local b = y + 1
			
			if (map [x][y] > -1  and map [x][y] <  4 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 86

			end

			if (map [x][y] > -1  and map [x][y] <  4 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 87

			end

			if (map [x][y] == 4 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 88

			end

			if (map [x][y] == 4 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 89

			end


			if (map [x][y] == 5 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 90

			end

			if (map [x][y] == 5 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 91

			end

			if (map [x][y] == 10 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 96

			end

			if (map [x][y] == 10 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 97

			end

			if (map [x][y] == 14 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 92

			end

			if (map [x][y] == 14 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 93

			end


			if (map [x][y] == 15 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 94

			end

			if (map [x][y] == 15 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 95

			end


			if (map [x][y] == 21 and map [x][b] > 34 and map [x][b] < 43) then 
				map [x][y] = 98

			end

			if (map [x][y] == 21 and map [x][b] > 21 and map [x][b] < 35) then 
				map [x][y] = 99

			end

		end


		local function verylargebuildingfix(x, y, map)
			local a = x + 1
			local b = y + 1
			local d = y + 2
			local f = y + 3


			if (map [x][y] == 60  and map [x][f] > 34 and map [x][f] < 43) then 
				map [x][d] = 78

			end

			if (map [x][y] == 60  and map [a][f] > 34 and map [a][f] < 43) then 
				map [a][d] = 80

			end

			if (map [x][y] == 60  and map [x][f] > 21 and map [x][f] < 35) then 
				map [x][d] = 79
			end

			if (map [x][y] == 60  and map [a][f] > 21 and map [a][f] < 35) then 
				map [a][d] = 81

			end

			if (map [x][y] == 61  and map [x][f] > 34 and map [x][f] < 43) then 
				map [x][d] = 82

			end

			if (map [x][y] == 61  and map [a][f] > 34 and map [a][f] < 43) then 
				map [a][d] = 84

			end

			if (map [x][y] == 61  and map [x][f] > 21 and map [x][f] < 35) then 
				map [x][d] = 83
			end

			if (map [x][y] == 61  and map [a][f] > 21 and map [a][f] < 35) then 
				map [a][d] = 85

			end

		end

		local function largebuildingfix(x, y, map)
			local a = x + 1
			local b = y + 1
			local d = y + 2

			if (map [x][y] == 56  and map [x][d] > 34 and map [x][d] < 43) then 
				map [x][b] = 62

			end

			if (map [x][y] == 56  and map [a][d] > 34 and map [a][d] < 43) then 
				map [a][b] = 64

			end

			if (map [x][y] == 56  and map [x][d] > 21 and map [x][d] < 35) then 
				map [x][b] = 63

			end

			if (map [x][y] == 56  and map [a][d] > 21 and map [a][d] < 35) then 
				map [a][b] = 65

			end


			if (map [x][y] == 57  and map [x][d] > 34 and map [x][d] < 43) then 
				map [x][b] = 66

			end

			if (map [x][y] == 57  and map [a][d] > 34 and map [a][d] < 43) then 
				map [a][b] = 68

			end

			if (map [x][y] == 57  and map [x][d] > 21 and map [x][d] < 35) then 
				map [x][b] = 67
			end

			if (map [x][y] == 57  and map [a][d] > 21 and map [a][d] < 35) then 
				map [a][b] = 69

			end


			if (map [x][y] == 58  and map [x][d] > 34 and map [x][d] < 43) then 
				map [x][b] = 70

			end

			if (map [x][y] == 58  and map [a][d] > 34 and map [a][d] < 43) then 
				map [a][b] = 72

			end

			if (map [x][y] == 58  and map [x][d] > 21 and map [x][d] < 35) then 
				map [x][b] = 71
			end

			if (map [x][y] == 58  and map [a][d] > 21 and map [a][d] < 35) then 
				map [a][b] = 73

			end

			if (map [x][y] == 59  and map [x][d] > 34 and map [x][d] < 43) then 
				map [x][b] = 74

			end

			if (map [x][y] == 59  and map [a][d] > 34 and map [a][d] < 43) then 
				map [a][b] = 76

			end

			if (map [x][y] == 59  and map [x][d] > 21 and map [x][d] < 35) then 
				map [x][b] = 75
			end

			if (map [x][y] == 59  and map [a][d] > 21 and map [a][d] < 35) then 
				map [a][b] = 77

			end

		end

		local function sbuildingfix(x, y, map)
			local b = y + 1

			if (map [x][y] == 26 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = random {30, 34}	
			
			end


			if (map [x][y] == 26 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 29
			
			end


			if (map [x][y] == 27 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 32
			
			end


			if (map [x][y] == 27 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 31
			
			end


			if (map [x][y] == 28 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 34
			
			end


			if (map [x][y] == 28 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 33
			
			end


			if (map [x][y] == 35 and map [x][b] > 21 and map[x][b] < 43) then
				map[x][y] = 41
			
			end


			if (map [x][b] == 36 and map [x][y] > 34 and map[x][y] < 43) then
				map[x][y] = 38
			
			end


			if (map [x][y] == 36 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 40
			
			end


			if (map [x][y] == 36 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 39
			
			end

			if (map [x][y] == 37 and map [x][b] > 21 and map[x][b] < 43) then
				map[x][y] = 42
			
			end

			if (map [x][y] == 43 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 45
			
			end


			if (map [x][y] == 43 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 44

			end

			if (map [x][y] == 43 and map [x][b] == 43) then
				map[x][y] = 46
				map[x][b] = -1
			
			end

			if (map [x][y] == 47 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 49
			
			end


			if (map [x][y] == 47 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 48

			end


			if (map [x][y] == 50 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 52
			
			end


			if (map [x][y] == 50 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 51
			
			end


			if (map [x][y] == 53 and map [x][b] > 21 and map[x][b] < 35) then
				map[x][y] = 54
			
			end


			if (map [x][y] == 53 and map [x][b] > 34 and map[x][b] < 43) then
				map[x][y] = 55
			
			end

		end

------------------------START OF RUNNING FUNCTIONS---------------------------------


		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_10x10()
			end
		end

		add_roads(tmp.SizeX, tmp.SizeY, tmp.Mapdata)

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 3 do
				if (math.random(1, 12) > 6) then
					corner_lne(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 3 do
				if (math.random(1, 12) > 6) then
					corner_lse(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 3 do
				if (math.random(1, 12) > 6) then
					corner_lnw(i, j, tmp.Mapdata)
				end
			end		
		end

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 3 do
				if (math.random(1, 12) > 6) then
					corner_lsw(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 6) then
					corner_sw(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 6) then
					corner_nw(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 6) then
					corner_se(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 6) then
					corner_ne(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 2 do
					sroad_s(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 3 do
					sroad_w(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 3 do
			for j = 1, tmp.SizeX - 2 do
					sroad_n(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 3 do
					sroad_e(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
					alley_e(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
					alley_w(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				alley_s(i, j, tmp.Mapdata)
			end
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				alley_n(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				paralel_roadv(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				paralel_roadh(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_ne(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_nw(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_se(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_sw(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_en(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_es(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_wn(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_ws(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_e(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_w(i, j, tmp.Mapdata)
			end	
		end


		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_n(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_s(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_ea(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				merger_wa(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_na(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				merger_sa(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 1 do
				if (math.random(1, 12) > 10) then
					random_20x20(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 4) then
					random_20x30(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				roadfix(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 3 do
				verylargebuildingfix(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 2 do
				largebuildingfix(i, j, tmp.Mapdata)
			end	
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				sbuildingfix(i, j, tmp.Mapdata)
			end	
		end

		return tmp
	end
}



