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

#include "stdafx.h"

#include "global.h"

#include "video.h"
#include "map.h"
#include "colors.h"

#define FIRST_CELL   0
#define GOOD_CELL   -1
#define LAST_CELL   -2
#define BAD_CELL    -3

#define MAX_PATH_LENGTH 30


#define PATH2DSIZE  2

int Map::pathfind(int sz, int sx, int sy, int dz, int dx, int dy, int can_fly, bool less_time, char *way, PF_MODE pf_mode)
{
    m_pathfind_mode = pf_mode;
        
	static Pathfinding pathfinding;
	return pathfinding.pathfind(this, sz, sx, sy, dz, dx, dy, can_fly, less_time, way, pf_mode);
}

static int TU, TU_max, TU_color;

void Map::draw_path_from(Soldier * s)
{
	char way[100];
	int waylen = pathfind(s->z, s->x, s->y, sel_lev, sel_col, sel_row, s->can_fly(), s->is_panicking(), way, PF_DISPLAY);
	TU = s->ud.CurTU;
	if (s->state() == SIT) TU -= 8;		//time to stand up
	TU_max = s->ud.MaxTU;
	TU_color = 1;			// COLOR_WHITE
    // Todo: change this color to have some contrast to terrain (e.g. arctic)
	path_show(s->z, s->x, s->y, way, waylen, s);
}

void Map::path_show(int _z, int _x, int _y, char *way, int waylen, Soldier *sld)
{
	//text_mode(0);
	//textprintf(screen, font, 0, SCREEN2H, COLOR_WHITE, "waylen=%d ", waylen);

	for (int i = 1; i < waylen; i++) {
		int dir = way[i];

        int time_of_dst;
        step_dest(_z, _x, _y, dir, sld->can_fly(), _z, _x, _y, time_of_dst, sld->is_panicking());

		int sx = x + 16 * _x + 16 * _y + 16;
		int sy = y - (_x + 1) * 8 + 8 * _y - 8 - _z * CELL_SCR_Z;

		if ((sx > -32) && (sx < SCREEN2W) && (sy >= -34) && (sy < SCREEN2H)) {

			TU -= time_of_dst;

            if (TU < sld->tus_reserved()) {
                TU_color = 32;      // COLOR_RED00
            }
			
			// Keep showing consecutive turns:
			if (TU < 0) {
				TU = TU_max - time_of_dst;
              //TU_color += 4;      // COLOR_GRAY04
                TU_color =  4;      // COLOR_GRAY04
			}
			if (TU < 0) break;
			printsmall_center_back(sx, sy, xcom1_color(TU_color), COLOR_GRAY15, TU);
		}

		//textprintf(screen, font, 0+i*80, SCREEN2H+20, COLOR_WHITE, "way[%d]=%d ", i, way[i]);
		//textprintf(screen, font, 0+i* 8, SCREEN2H+10, COLOR_WHITE, "%d            ", way[i]);
	}
}

int Map::step_dest(int z1, int x1, int y1, int dir, int flying, int& z2, int& x2, int& y2, int& tu_cost, bool less_time)
{
    // Is starting movement allowed?
    if (!passable(z1, x1, y1, dir))
        return 0;

    // Calculating of a final point
    z2=z1;
    x2=x1;
    y2=y1;

    //Horizontal moving
	x2 += DIR_DELTA_X(dir);
	y2 += DIR_DELTA_Y(dir);

    //Are the fly abilities necessary for this step?
    int trying_to_fly = 0;

    if (dir == DIR_UP) {
        z2++;
        tu_cost = 10;
        trying_to_fly = 1;
    }

    if (dir == DIR_DOWN) {
        z2--;
        tu_cost = 10;
        trying_to_fly = 1;
    }

    if (!cell_inside(z2, x2, y2))
        return 0;

    // If we have to fly bat we can't
    if (trying_to_fly && !(flying || map->mcd(z1, x1, y1, 0)->Gravlift && map->mcd(z2, x2, y2, 0)->Gravlift ))
        return 0;

    // TU cost of horisontal moving
    if(dir<DIR_NULL) {
        tu_cost = walk_time(z2, x2, y2);
        if (DIR_DIAGONAL(dir))
            tu_cost = tu_cost * 3 / 2;
    }
    
    if (less_time)
        tu_cost = tu_cost * 3 / 4;

    // Up over the stairs
	if (isStairs(z2, x2, y2))
		z2++;

    if (!cell_inside(z2, x2, y2))
        return 0;

    //Down if we have no ground under our feet
	while ( !support_for_feet(z2, x2, y2) && !flying)
		z2--;

    if (!passable(z2, x2, y2))
        return 0;

    return 1;
}

int Map::support_for_feet(int z, int x, int y)
{
    if (z <= 0)
        return true;
	return !mcd(z, x, y, 0)->No_Floor || isStairs(z - 1, x, y);
}

int Pathfinding::pathfind(Map* _map,int sz, int sx, int sy, int dz, int dx, int dy, int can_fly, bool less_time, char *way, PF_MODE pf_mode)
{
    SetMap(_map);

    if (map->stopWALK(dz, dx, dy, 0) || map->stopWALK(dz, dx, dy, 3))
        return 0;

	way[0] = DIR_NULL;

	int k, i, j;

    //Clearing pathfinding info for all map cells
	for (k = 0; k < map->level; k++)
		for (i = 0; i < map->width * 10; i++)
			for (j = 0; j < map->height * 10; j++)
				pf_info(k, i, j)->path_is_known = 0;

    //Marking start position
    pathfinding_cell_list.push_back(pf_info(sz, sx, sy));
    pathfinding_cell_list.front() -> path_is_known = 1;
    pathfinding_cell_list.front() -> x = sx;
    pathfinding_cell_list.front() -> y = sy;
    pathfinding_cell_list.front() -> z = sz;
    pathfinding_cell_list.front() -> tu_cost = 0;
    pathfinding_cell_list.front() -> steps_num = 0;
    pathfinding_cell_list.front() -> prev_point = NULL;

    // Processing found accesible cells in order they were found
    while(!pathfinding_cell_list.empty()) {
        // Iterating directions from current cell
        for (int dir = 0; dir < DIR_NUM; dir++) {
            int nx, ny, nz, tu_cost;
            int ox = pathfinding_cell_list.front() -> x;
            int oy = pathfinding_cell_list.front() -> y;
            int oz = pathfinding_cell_list.front() -> z;
            // If we found new best way to cell in this direction ...
            if(map->step_dest(oz, ox, oy, dir, can_fly, nz, nx, ny, tu_cost, less_time)) {
                if( (!pf_info(nz, nx, ny)->path_is_known ||
                pf_info(nz, nx, ny)->tu_cost > pf_info(oz, ox, oy)->tu_cost + tu_cost) &&
                (!pf_info(dz, dx, dy)->path_is_known ||
                pf_info(dz, dx, dy)->tu_cost > pf_info(oz, ox, oy)->tu_cost + tu_cost )
                //&& pf_info(oz, ox, oy)->tu_cost + tu_cost<80
                ) {
                    // Marking found cell for further processing
                    pf_info(nz, nx, ny)->path_is_known = 1;
                    pf_info(nz, nx, ny)->tu_cost = pf_info(oz, ox, oy)->tu_cost + tu_cost;
                    pf_info(nz, nx, ny)->steps_num = pf_info(oz, ox, oy)->steps_num + 1;
                    pf_info(nz, nx, ny)->prev_point = pf_info(oz, ox, oy);
                    pf_info(nz, nx, ny)->prev_dir=dir;
                    pathfinding_cell_list.push_back(pf_info(nz, nx, ny));
                }
            }
        }
    //Deleting processed cell
    pathfinding_cell_list.pop_front();
    }

    if(!pf_info(dz, dx, dy)->path_is_known) return 0;

    //Backward tracking of the path
    pathfinding_info* cur_pf=pf_info(dz, dx, dy);
	for (i = pf_info(dz, dx, dy)->steps_num; i > 0; i--) {
		way[i] = cur_pf->prev_dir;
        cur_pf=cur_pf->prev_point;
	}

	return pf_info(dz, dx, dy)->steps_num + 1;
}

void Pathfinding::SetMap(Map* _map)
{
    map=_map;
    // If size of existed matrix m_pf differs from a size of map, reallocate
    // memory for matrix.
    if(level != map -> level || width != map -> width || height != map -> height)
    {
        int i, j, k;

        if(level && width && height) {
            for (i = 0; i < level; i++) {
                for (j = 0; j < 10 * width; j++) {
                    for (k = 0; k < 10 * height; k++) {
                        delete m_pf[i][j][k];
                    }
        			delete [] m_pf[i][j];
        		}
        		delete [] m_pf[i];
	       }
	       delete [] m_pf;
        }
        level = map -> level;
        width = map -> width;
        height = map -> height;

        m_pf = new pathfinding_info***[map->level];

    	for (i = 0; i < level; i++) {
    		m_pf[i] = new pathfinding_info ** [10 * width];
        		for (j = 0; j < 10 * map->width; j++) {
        			m_pf[i][j] = new pathfinding_info * [10 * height];
        			for (k = 0; k < 10 * height; k++) {
        				m_pf[i][j][k] = new pathfinding_info();
        				m_pf[i][j][k] -> z = i;
        				m_pf[i][j][k] -> x = j;
        				m_pf[i][j][k] -> y = k;
        			}
        		}
	    }
     }
}

Pathfinding::Pathfinding(): level(0), width(0), height(0)
{
}

Pathfinding::~Pathfinding()
{
    // If matrix is not zero size, we have to free memory
    if(level && width && height) {
        int i, j, k;
        for (i = 0; i < level; i++) {
    		for (j = 0; j < 10 * width; j++) {
	      		for (k = 0; k < 10 * height; k++) {
	       			delete m_pf[i][j][k];
		      	}
			    delete [] m_pf[i][j];
            }
            delete [] m_pf[i];
        }
        delete [] m_pf;
	}
}
