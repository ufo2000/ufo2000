/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "global.h"
#include <string.h>
#include <stdio.h>

#include "video.h"
#include "map.h"
#include "colors.h"

#define FIRST_CELL   0
#define GOOD_CELL   -1
#define LAST_CELL   -2
#define BAD_CELL    -3

#define MAX_PATH_LENGTH 30

int Map::setneibos(int oz, int ox, int oy, int value)
{
	for (int dir = 0; dir < 8; dir++) {
		if (!passable(oz, ox, oy, dir)) continue;

		int dx = ox + DIR_DELTA_X(dir);
		int dy = oy + DIR_DELTA_Y(dir);
		int dz = oz;

		if ((dx < 0) || (dy < 0) || (dx >= width * 10) || (dy >= height * 10))
			continue;

		if (pfval(dz, dx, dy) == GOOD_CELL)
			set_pfval(dz, dx, dy, value);
		else
			if (pfval(dz, dx, dy) == LAST_CELL)
				return 0;
	}

	return 1;
}

int Map::findneibo(int & _dz, int & _dx, int & _dy, int value)
{
	int dp[8] = {0, 2, 4, 6, 1, 3, 5, 7};

	int good_dir = -1;
	int good_dir_time = 11111;
	int dz = _dz, dx = _dx, dy = _dy;

	for (int i = 0; i < 8; i++) {
		int dir = dp[i];
		int ox = dx + DIR_DELTA_X(dir);
		int oy = dy + DIR_DELTA_Y(dir);
		int oz = dz;
		if ((ox < 0) || (oy < 0) || (ox >= width * 10) || (oy >= height * 10))
			continue;

		if (pfval(oz, ox, oy) != value)
			continue;

		if (passable(oz, ox, oy, DIR_REVERSE(dir))) {
			if (walk_time(oz, ox, oy) < good_dir_time) {
				_dz = oz; _dy = oy; _dx = ox;
				good_dir_time = walk_time(oz, ox, oy);
				good_dir = dir;
			}
		}
	}
	ASSERT(good_dir != -1);
	return good_dir;
}

#define PATH2DSIZE  2

int Map::pathfind(int sz, int sx, int sy, int dz, int dx, int dy, char *way)
{
	way[0] = 8;

	int k, i, j;

	for (k = 0; k < level; k++)
		for (i = 0; i < width * 10; i++)
			for (j = 0; j < height * 10; j++) {
				set_pfval(k, i, j, BAD_CELL);
				if (!stopWALK(k, i, j, 0) && !stopWALK(k, i, j, 3))
					set_pfval(k, i, j, GOOD_CELL);
			}

	set_pfval(sz, sx, sy, 0);

	if (pfval(dz, dx, dy) == BAD_CELL)
		return 0;

	set_pfval(dz, dx, dy, LAST_CELL);

	int REACHDST = 0, cur_iter = 0;

	while (cur_iter < MAX_PATH_LENGTH) {
		for (k = 0; k < level; k++)
			for (i = 0; i < width * 10; i++)
				for (j = 0; j < height * 10; j++) {
					if (pfval(k, i, j) == cur_iter) {
						if (setneibos(k, i, j, cur_iter + 1) == 0) {
							REACHDST = 1;
							goto exitloop;
						}
					}
				}
		cur_iter++;
	}
exitloop:

	if (FLAGS & F_SHOWROUTE) {
		text_mode(0);
		textprintf(screen, font, 0, SCREEN2H + 20, COLOR_WHITE, 
			"(%d,%d,%d)-(%d,%d,%d) %d  ", sz, sx, sy, dz, dx, dy, cur_iter);

		BITMAP *path2d = create_bitmap(width * 10 * PATH2DSIZE, height * 10 * PATH2DSIZE);
		for (k = 0; k < level; k++) {
			clear(path2d);

			for (i = 0; i < width*10; i++) {
				for (j = 0; j < height*10; j++) {
					int color = pfval(k, i, j) * 16;
					if (pfval(k, i, j) == -2)
						color = 39;
					else if (pfval(k, i, j) == 0)
						color = 56;

					rectfill(path2d, i * PATH2DSIZE, j * PATH2DSIZE,
					         i * PATH2DSIZE + PATH2DSIZE, j * PATH2DSIZE + PATH2DSIZE, xcom1_color(color));
				}
			}
			blit(path2d, screen, 0, 0, SCREEN2W + k * 90, SCREEN2H, path2d->w, path2d->h);
		}
		destroy_bitmap(path2d);
	}

	if (!REACHDST) return 0;

	for (i = cur_iter; i >= 0; i--) {
		int dir = findneibo(dz, dx, dy, i);
		way[i + 1] = DIR_REVERSE(dir);
	}

	//	  dirs		0  1  2  3  4  5  6  7
	int dir2xy[8] = {1, 1, 0, -1, -1, -1, 0, 1};

	for (i = 1; i < cur_iter + 2; i++) {
		int dir = way[i];
		sx += dir2xy[dir];
		sy += dir2xy[(dir + 6) % 8];
		if (mcd(sz, sx, sy, 0)->No_Floor) {
			cur_iter = i - 1;
		}
	}

	if (FLAGS & F_SHOWROUTE) {
		for (j = 0; j < cur_iter + 2; j++) {
			textprintf(screen, font, SCREEN2W + j * 8, SCREEN2H - 9, COLOR_WHITE, "%d ", way[j]);
		}
	}

	return cur_iter + 2;
}

static int TU, TU_max, TU_color;

void Map::draw_path_from(Soldier * s)
{
	char way[100];
	int waylen = pathfind(s->z, s->x, s->y, sel_lev, sel_col, sel_row, way);
	TU = s->ud.CurTU;
	if (s->state() == SIT) TU -= 8;		//time to stand up
	TU_max = s->ud.MaxTU;
	TU_color = 1;			// COLOR_WHITE
	path_show(s->z, s->x, s->y, way, waylen);
}

void Map::path_show(int _z, int _x, int _y, char *way, int waylen)
{
	//text_mode(0);
	//textprintf(screen, font, 0, SCREEN2H, COLOR_WHITE, "waylen=%d ", waylen);

	for (int i = 1; i < waylen; i++) {
		int dir = way[i];
		if (dir != 8) {
			_x += DIR_DELTA_X(dir);
			_y += DIR_DELTA_Y(dir);
		}
		if (isStairs(_z, _x, _y))
			_z++;

		if (mcd(_z, _x, _y, 0)->No_Floor && (_z > 0) && !isStairs(_z - 1, _x, _y))
			_z--;

		int sx = x + 16 * _x + 16 * _y + 16;
		int sy = y - (_x + 1) * 8 + 8 * _y - 8 - _z * CELL_SCR_Z;

		if ((sx > -32) && (sx < SCREEN2W) && (sy >= -34) && (sy < SCREEN2H)) {
			//circlefill(screen2, sx, sy, 1, 1);
			//printsmall(sx, sy, 1, i);
			int time_of_dst = walk_time(_z, _x, _y);
			
			if (DIR_DIAGONAL(dir))
				time_of_dst = time_of_dst * 3 / 2; // diagonal moves use 1.5 TUs
			//printsmall(sx, sy, 1, time_of_dst);
			TU -= time_of_dst;

			// Todo: use other color for moves in reserved-time
			
			// Keep showing consecutive turns:
			if (TU < 0) {
				TU = TU_max - time_of_dst;
				TU_color += 4;		// COLOR_GRAY04
			}
			if (TU <0) break;
			printsmall_center(sx, sy, xcom1_color(TU_color), TU);
		}

		//textprintf(screen, font, 0+i*80, SCREEN2H+20, COLOR_WHITE, "way[%d]=%d ", i, way[i]);
		//textprintf(screen, font, 0+i* 8, SCREEN2H+10, COLOR_WHITE, "%d            ", way[i]);
	}
}

