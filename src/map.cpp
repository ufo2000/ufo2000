/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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
#include "colors.h"
#include "sound.h"
#include "pck.h"
#include "map.h"
#include "multiplay.h"
#include "platoon.h"
#include "crc32.h"
#include "scenario.h"
#include "text.h"
#include "random.h"

#define SCANGSIZE 4

uint16 *Map::m_loftemp = NULL;
int Map::m_loftemp_num = 0;
char *Map::m_scang_xcom = NULL;
char *Map::m_scang_tftd = NULL;
SPK *Map::scanbord = NULL;
PCK *Map::smoke = NULL, *Map::cursor = NULL;
int Map::m_animation_cycle = 0;

//			  dirs		0  1  2  3  4  5  6  7
int Map::dir2ofs[8] = {1, 1, 0, -1, -1, -1, 0, 1};
//  y  x	  -1  0  1	x
char Map::ofs2dir[3][3] = {{5, 6, 7},
                           {4, 8, 0},
                           {3, 2, 1}};

IMPLEMENT_PERSISTENCE(Map, "Map");

void load_terrain_pck(const std::string &tid, TerraPCK *&terrain_pck)
{
	terrain_pck = NULL;

	int stack_top = lua_gettop(L);
    // Enter 'TerrainTable' table
	lua_pushstring(L, "TerrainTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [tid] table
	lua_pushstring(L, tid.c_str());
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
    // Enter 'Tiles' table
	lua_pushstring(L, "Tiles");
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
	int tiles_table = lua_gettop(L);
    // Enter 'Palettes' table
	lua_pushstring(L, "Palettes");
	lua_gettable(L, -3);
	ASSERT(lua_istable(L, -1));
	int palettes_table = lua_gettop(L);

    // Load all tiles from 'Tiles' table
	int index = 1;
	while (1) {
		lua_pushnumber(L, index);
		lua_gettable(L, tiles_table);
		if (!lua_isstring(L, -1)) break;
		const char *pckname = lua_tostring(L, -1);

		lua_pushnumber(L, index);
		lua_gettable(L, palettes_table);
		ASSERT(lua_isnumber(L, -1));
		int tftd_flag = (int)lua_tonumber(L, -1);

		if (terrain_pck == NULL)
			terrain_pck = new TerraPCK(pckname, tftd_flag);
		else
			terrain_pck->add(pckname, tftd_flag);

		lua_pop(L, 2);

		index++;
	}

	lua_settop(L, stack_top);
}

void Map::initpck()
{
	cursor	 = new PCK("$(xcom)/ufograph/cursor.pck");
	scanbord = new SPK("$(xcom)/ufograph/scanbord.pck");
	smoke	 = new PCK("$(xcom)/ufograph/smoke.pck");

	int fh = open(F("$(xcom)/geodata/scang.dat"), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	int fl = filelength(fh);
	m_scang_xcom = new char[fl];
	read(fh, m_scang_xcom, fl);
	close(fh);

	fh = open(F("$(tftd)/geodata/scang.dat"), O_RDONLY | O_BINARY);
	if (fh != -1) {
		fl = filelength(fh);
		m_scang_tftd = new char[fl];
		read(fh, m_scang_tftd, fl);
		close(fh);
	}

	fh = open(F("$(xcom)/geodata/loftemps.dat"), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	fl = filelength(fh);
	m_loftemp = new uint16[fl / 2];
	read(fh, m_loftemp, fl);
	close(fh);
	m_loftemp_num = fl / 32;
}

void Map::freepck()
{
	delete cursor;
	delete scanbord;
	delete smoke;

	delete [] m_scang_xcom;
	delete [] m_scang_tftd;
	delete [] m_loftemp;
}

void Map::create(int l, int w, int h)
{
	int i, j, k;
	level = l; width = w; height = h;

	m_cell = new Cell***[level];
	for (i = 0; i < level; i++) {
		m_cell[i] = new Cell ** [10 * width];
		for (j = 0; j < 10 * width; j++) {
			m_cell[i][j] = new Cell * [10 * height];
			for (k = 0; k < 10 * height; k++) {
				m_cell[i][j][k] = new Cell();
			}
		}
	}
}

Map::Map(GEODATA &mapdata)
{
	sel_row = 0; sel_col = 0; sel_lev = 0;

	create(mapdata.z_size, mapdata.x_size, mapdata.y_size);

	std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
	ASSERT(terrain_name.size() < sizeof(m_terrain_name));
	strcpy(m_terrain_name, terrain_name.c_str());
	load_terrain_pck(m_terrain_name, m_terrain);
	loadmaps(mapdata.mapdata);

	build_visi();

	m_minimap_area = new MinimapArea(this, SCREEN_W - SCREEN2W, SCREEN2H);
}

Map::~Map()
{
	int i, j, k;
	for (i = 0; i < level; i++) {
		for (j = 0; j < 10 * width; j++) {
			for (k = 0; k < 10 * height; k++) {
				delete m_cell[i][j][k];
			}
			delete [] m_cell[i][j];
		}
		delete [] m_cell[i];
	}

	delete [] m_cell;
	delete m_terrain;

	delete m_minimap_area;
}

void Map::loadmaps(unsigned char *_map)
{
	int stack_top = lua_gettop(L);
	lua_pushstring(L, "TerrainTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1));
	lua_pushstring(L, m_terrain_name);
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
	lua_pushstring(L, "Maps");
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));

	int i = 0;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (_map[i] != 0xFE) {
				
				lua_pushnumber(L, _map[i]);
				lua_gettable(L, -2);
				// Todo: fix ufo200 crashing
				// when loading invalid map-number
				// ?? maybe just replace map with "00" ??
				ASSERT(lua_isstring(L, -1));
				loadmap(lua_tostring(L, -1), col * 10, row * 10);
				lua_pop(L, 1);
			}
			i++;
		}
	}

	lua_settop(L, stack_top);
}

int Map::loadmap(const char *fname, int _c, int _r)
{
	int fh = open(fname, O_RDONLY | O_BINARY);
	if (fh == -1) return 0;
	long size = filelength(fh);
	char *mbuf = new char[size];
	read(fh, mbuf, size);
	close(fh);

	int w = mbuf[0];
	int h = mbuf[1];
	int l = mbuf[2];

	_r = _r - 10 + h;
	_c = _c - 10 + w;

	for (int lev = 0; lev < l; lev++) {
		int i = 3 + (l - lev - 1) * 4 * w * h;
		if (i < 3 || i >= size) continue;
		if (lev >= level) continue;

		for (int col = _c + w - 1 - (w - 10); col >= _c - (w - 10); col--) {
			for (int row = _r - (h - 10); row < _r + h - (h - 10); row++, i += 4) {
				if (i < 3 || i + 4 > size) continue;
				if (col < 0 || col >= width * 10) continue;
				if (row < 0 || row >= height * 10) continue;

				m_cell[lev][col][row]->type[0] = mbuf[i + 0];
				m_cell[lev][col][row]->type[1] = mbuf[i + 1];
				m_cell[lev][col][row]->type[2] = mbuf[i + 2];
				m_cell[lev][col][row]->type[3] = mbuf[i + 3];
				m_cell[lev][col][row]->set_soldier(NULL);
			}
		}
	}
	delete [] mbuf;
	return 1;
}

void Map::drawitem(BITMAP *itype, int gx, int gy)
{
	PCK::showpck(itype, gx, gy);
}


void Map::draw_cell_pck(int _x, int _y, int _lev, int _col, int _row, int _type, int _seen)
{
	int i = m_cell[_lev][_col][_row]->type[_type];
	ASSERT(i < (int)m_terrain->m_mcd.size());

	int frame;
	
	if (!m_terrain->m_mcd[i].UFO_Door)
		frame = m_terrain->m_mcd[i].pck_base + m_terrain->m_mcd[i].Frame[m_animation_cycle];
	else
		frame = m_terrain->m_mcd[i].pck_base + m_terrain->m_mcd[i].Frame[7];

	_y -= m_terrain->m_mcd[i].P_Level;

	ASSERT(frame < m_terrain->m_imgnum);

	if (frame && frame < m_terrain->m_imgnum) {
		if (_seen)
			m_terrain->showpck(frame, _x, _y);
		else
			m_terrain->showblackpck(frame, _x, _y);
	}
}

extern volatile unsigned int ANIMATION;

/**
 * Draw "3D" Battlescape-Map
 */
void Map::draw()
{
	m_animation_cycle = (ANIMATION / 3) % 8;

	int sx, sy;
	int mtype = 0;

	m_cell[sel_lev][sel_col][sel_row]->MOUSE = 1;

	int r1 = 0, r2 = height * 10 - 1;
	int c1 = 0, c2 = width * 10 - 1;
	int l1 = 0, l2;
	if (FLAGS & F_SHOWLEVELS)
		l2 = level - 1;
	else
		l2 = sel_lev;


	for (int lev = l1; lev <= l2; lev++) {
		for (int row = r1; row <= r2; row++) {
			for (int col = c2; col >= c1; col--) {
				sx = x + CELL_SCR_X * col + CELL_SCR_X * row;
				sy = y - (col) * CELL_SCR_Y + CELL_SCR_Y * row - 26 - lev * CELL_SCR_Z - 1;

				if ((sx > -32) && (sx < SCREEN2W) && (sy >= -34) && (sy < SCREEN2H)) {

					draw_cell_pck(sx, sy, lev, col, row, 0, seen(lev, col, row));

					if (m_cell[sel_lev][col][row]->MOUSE) {
						if (lev == sel_lev) {
							if ((m_cell[lev][col][row]->soldier_here()) && (visible(lev, col, row)))
								mtype = 1;
							else
								mtype = 0;
							if ((!TARGET) || (target.action == THROW))
								cursor->showpck(mtype, sx, sy);
						} else if (lev < sel_lev) {
							cursor->showpck(2, sx, sy);
						}
					}

					if (seen(lev, col, row)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 1);
						draw_cell_pck(sx, sy, lev, col, row, 2, 1);
					} else if ((mcd(lev, col, row, 1)->Door || mcd(lev, col, row, 1)->UFO_Door) &&
					        (row > 0) && seen(lev, col, row - 1)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 1);
						draw_cell_pck(sx, sy, lev, col, row, 2, 0);
					} else if ((mcd(lev, col, row, 2)->Door || mcd(lev, col, row, 2)->UFO_Door) &&
					        (col < width * 10 - 1) && seen(lev, col + 1, row)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 0);
						draw_cell_pck(sx, sy, lev, col, row, 2, 1);
					} else {
						draw_cell_pck(sx, sy, lev, col, row, 1, 0);
						draw_cell_pck(sx, sy, lev, col, row, 2, 0);
					}

					draw_cell_pck(sx, sy, lev, col, row, 3, seen(lev, col, row));

					if (seen(lev, col, row)) {
						int gy = sy + mcd(lev, col, row, 0)->T_Level;
						gy += mcd(lev, col, row, 3)->T_Level;
						//m_cell[lev][col][row]->get_place()->draw(sx, gy);
						if (platoon_local->get_seen_item_index(lev, col, row) != -1)
                            drawitem(Item::obdata_get_bitmap(platoon_local->get_seen_item_index(lev, col, row), "pMap"), sx, gy);
					}

					if (visible(lev, col, row)) {
						if (m_cell[lev][col][row]->soldier_here())
							m_cell[lev][col][row]->get_soldier()->draw();

						if (lev < level - 1) {
							if (m_cell[lev + 1][col][row]->soldier_here()) {
                                if (isStairs(lev, col, row) && mcd(lev + 1, col, row, 0)->No_Floor) {
									m_cell[lev + 1][col][row]->get_soldier()->draw();
								}
							}
						}
					}

					if (m_cell[sel_lev][col][row]->MOUSE) {
						if (lev == sel_lev) {
							if ((TARGET) && (target.action == THROW))
								cursor->showpck(15, sx, sy);
							if ((TARGET) && (target.action != THROW))
								mtype += 6;
							else
								mtype += 3;
							cursor->showpck(mtype, sx, sy);
						} else if (lev < sel_lev) {
							cursor->showpck(5, sx, sy);
						}
					}

					if (seen(lev, col, row)) {
 						int s = fire_state(lev, col, row);
 						if (fire_time(lev,col,row)>0) {
 							smoke->showpck(8-s, sx, sy);
 						} else {
 							s = smog_state(lev, col, row);
 							if (smog_time(lev,col,row)>0)
 								smoke->showpck(s-1, sx, sy);
 						}
 					}
				}
			}
		}
	}

	m_cell[sel_lev][sel_col][sel_row]->MOUSE = 0;
}


void Map::step()
{
 	for(int k=0; k<level;k++)
 		for(int i=0; i<10*width;i++)
 			for(int j=0; j<10*height;j++)
 				if (fire_time(k,i,j)>0) {
 					dec_fire_time(k,i,j);
 					for (int h=0; h<4; h++)
 						damage_cell_part(k,i,j,h,PISTOL_CLIP);
 					if (man(k, i, j) != NULL)
 						man(k, i, j)->hit(0, 10, DT_INC, 8); //DAMAGEDIR_UNDER
 					if (fire_time(k,i,j) > 1)
 						set_fire_state(k,i,j,1);
 					if (fire_time(k,i,j) == 1)
 						set_fire_state(k,i,j,5);
 					if (fire_time(k,i,j) <= 0)
 						set_fire_state(k,i,j,0);
 				} else {
 					if (smog_time(k, i, j)>0) {
 						dec_smog_time(k, i, j);
 						if (smog_time(k, i, j) > 2)
 							set_smog_state(k, i, j, 17);
 						if (smog_time(k, i, j) == 2)
 							set_smog_state(k, i, j, 13);
 						if (smog_time(k, i, j) == 1)
 							set_smog_state(k, i, j, 9);
 						if (smog_time(k, i, j) == 0)
 							set_smog_state(k, i, j, 0);
 					}
				}
}

void Map::smoker()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10 * width; i++)
			for (int j = 0; j < 10 * height; j++)
				cell(k, i, j)->cycle_smoke();
}


#define Cx 1
#define Cy -8

/**
 * Set position of selection-'cage' on map
 */
void Map::set_sel(int mx, int my)
{
	my += sel_lev * CELL_SCR_Z;      //!!
	sel_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx ;
	sel_row = (my - 3) - y + 8 - Cy + sel_col / 4;
	sel_row /= CELL_SCR_Y; sel_col /= CELL_SCR_X * 2; // 32;

	if (sel_col < 0) sel_col = 0;
	if (sel_row < 0) sel_row = 0;
	if (sel_col >= 10 * width) sel_col = 10 * width - 1;
	if (sel_row >= 10 * height) sel_row = 10 * height - 1;
//	text_mode(0); textprintf(screen, font, SCREEN2W + 8, 0, COLOR_WHITE, "x=%02d, y=%02d", sel_col, sel_row);
}

/**
 * Scroll the map by specified x and y offsets
 */
void Map::move(int ofs_x, int ofs_y)
{
	int sx = x, sy = y;
	x += ofs_x;
	y += ofs_y;

	int mx = SCREEN2W / 2;
	int my = SCREEN2H / 2;

	int old_center_col = mx - sx - 2 * (my - 3) + 2 * sy - 16 + 2 * Cy - Cx;
	int old_center_row = (my - 3) - sy + 8 - Cy + old_center_col / 4;
	old_center_row >>= 3; old_center_col >>= 5;

	int center_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx;
	int center_row = (my - 3) - y + 8 - Cy + center_col / 4;
	center_row >>= 3; center_col >>= 5;

	// When the center of view is already outside of the map, it should be
	// possible to scroll in the direction of the map.
	if (((center_col < 0) && (center_col < old_center_col)) ||
		((center_row < 0) && (center_row < old_center_row)) ||
    	((center_col >= 10 * width) && (center_col > old_center_col)) ||
    	((center_row >= 10 * height) && (center_row > old_center_row))) {
		x = sx; y = sy;
	}
}

#define S 2

/**
 * Redraw minimap
 */
void Map::svga2d()
{
	m_minimap_area->redraw(screen, SCREEN2W, 0);
}

/**
 * Minimap in "xcom-mapviewer"-look
 */
void Map::draw2d()
{
	int cx = SCREEN2W / 2;
	int cy = SCREEN2H / 2;

	set_sel(cx, cy);

	BITMAP *tmp = create_bitmap_of_map(sel_lev);

	int r1 = sel_row - 18; if (r1 < 0) r1 = 0;
	int r2 = sel_row + 18; if (r2 >= height * 10) r2 = height * 10 - 1;

	int c1 = sel_col - 27; if (c1 < 0) c1 = 0;
	int c2 = sel_col + 27; if (c2 >= width * 10) c2 = width * 10 - 1;

	for (int lev = 0; lev <= sel_lev; lev++)
		for (int row = r1; row <= r2; row++)
			for (int col = c1; col <= c2; col++) {
				if (!seen(lev, col, row)) continue;

				blit(tmp, screen2, 
					col * 4 + 3, 
					row * 4,
					cx + ( -sel_col + col) * 4 + 3,
					cy + ( -sel_row + row) * 4,
					4, 4);

				if (man(lev, col, row) == NULL) continue;
				
				if (platoon_local->belong(man(lev, col, row)))
					rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
					         cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
					         COLOR_YELLOW);
				else
					if (visible(lev, col, row))
						rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
						         cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
						         COLOR_RED00);
			}

	destroy_bitmap(tmp);
	scanbord->show(screen2, cx - 160 + 4, cy - 100 + 12);
	text_mode(-1);
	textprintf(screen2, large, cx - 160 + 281 + 4, cy - 100 + 74 + 12, COLOR_LT_OLIVE, "%d", sel_lev);
}


BITMAP *Map::create_bitmap_of_map(int max_lev)
{
	int scang4x4[16];
	BITMAP *bmp = create_bitmap(width * 10 * 4, height * 10 * 4);
	clear_to_color(bmp, COLOR_BLACK1);

	for (int lev = 0; lev <= max_lev; lev++) {
		for (int row = 0; row < height*10; row++) {
			for (int col = 0; col < width*10; col++) {
				int i;

				for (i = 0; i < 16; i++) scang4x4[i] = xcom_color(0);

				for (int j = 0; j < 4; j++) {
					int mcd_index = m_cell[lev][col][row]->type[j];
					if (mcd_index == 0) continue;
					MCD *m = &m_terrain->m_mcd[mcd_index];

					int mt = m->ScanG;
					int tftd_flag = m->tftd_flag;
					char *scang = tftd_flag ? m_scang_tftd : m_scang_xcom;

					ASSERT(scang != NULL);
					/* if (mt > 0) */ {
						mt += 35;
						for (int i = 0; i < 16; i++) {
							if (scang[mt * 16 + i])
								scang4x4[i] = tftd_flag ? 
									tftd_color(scang[mt * 16 + i]) :
									xcom_color(scang[mt * 16 + i]);
						}
					}
				}

				for (i = 0; i < 16; i++)
					if (scang4x4[i] != xcom_color(0))
						putpixel(bmp, col * 4 + (3 - i / 4), row * 4 + i % 4, scang4x4[i]);
			}
		}
	}
	return bmp;
}


int Map::center2d(int xx, int yy)
{
	int cx = SCREEN2W / 2;
	int cy = SCREEN2H / 2;
	xx -= cx - 160 + 4;
	yy -= cy - 100 + 11;

	//if ((xx<X)||(xx>X+width*10*SCANGSIZE) || (yy<Y)||(yy>Y+height*10*SCANGSIZE)) {
	if ((xx > 278) && (yy > 148) && (xx < 302) && (yy < 173)) { //OK
		return 1;
	}
	if ((xx > 24) && (yy > 62) && (xx < 40) && (yy < 81)) { //Up
		if (sel_lev < level - 1)
			sel_lev++;
		return 0;
	}
	if ((xx > 24) && (yy > 88) && (xx < 40) && (yy < 107)) { //Down
		if (sel_lev > 0)
			sel_lev--;
		return 0;
	}

	//	return 0;
	//}
	int col, row;
	col = sel_col - (160 - xx - 4) / SCANGSIZE;
	row = sel_row - (100 - yy - 11) / SCANGSIZE;
	center(0, col, row);
	return 0;
}

void Map::clearseen()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10*width; i++)
			for (int j = 0; j < 10*height; j++)
				set_seen(k, i, j, 0);
}

void Map::unhide()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10*width; i++)
			for (int j = 0; j < 10*height; j++)
				set_seen(k, i, j, 1);
}

void Map::center(int lev, int col, int row)
{
	sel_lev = lev;
	x = SCREEN2W / 2 - CELL_SCR_X - CELL_SCR_X * col - CELL_SCR_X * row;     	  //320	 320/2-20
	y = SCREEN2H / 2 + CELL_SCR_Y * (col + 1) - CELL_SCR_Y * row + CELL_SCR_Z * lev;     	  //200	 200/2-8
}

int Map::stopLOS_level(int dx, int dy, int lev, int col, int row)
{
	if ((col + dx < 0) || (col + dx >= width * 10) || (row + dy < 0) || (row + dy >= height * 10))
		return 1;
	int ld = ofs2dir[dy + 1][dx + 1];
	if (viewable(lev, col, row, ld))
		return 0;
	return 1;
}


int Map::haveGROUND(int lev, int col, int row)
{
	if ((lev < 0) || (lev >= level) ||
	        (col < 0) || (col >= width * 10) ||
	        (row < 0) || (row >= height * 10)
	   )
		return 0;

	int ct = m_cell[lev][col][row]->type[0];
	if (ct != 0)
		return 1;
	return 0;
}


void Map::build_visi_cell(int lev, int col, int row)
{
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int dz = k - 1;
				int dx = i - 1;
				int dy = j - 1;

				m_cell[lev][col][row]->visi[k][i][j] = 0; //so below we found if cell is(become) visible, but if it become INvisible?
				switch (dz) {
					case 0:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
					case - 1:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						if ((lev > 0) && stopLOS_level(dx, dy, lev - 1, col, row))
							continue;
						if (haveGROUND(lev, col, row))
							continue;
						if (haveGROUND(lev, col + dx, row + dy))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
					case 1:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						if ((lev < level - 1) && stopLOS_level(dx, dy, lev + 1, col, row))
							continue;
						if ((lev < level - 1) && haveGROUND(lev + 1, col, row))
							continue;
						if ((lev < level - 1) && haveGROUND(lev + 1, col + dx, row + dy))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
				}
			}
		}
	}

	if (stopLOS(lev, col, row, 3))
		m_cell[lev][col][row]->visi[1][1][1] = 0;

}

void Map::build_visi()
{
	for (int lev = 0; lev < level; lev++) {
		for (int col = 0; col < width*10; col++) {
			for (int row = 0; row < height*10; row++) {
				build_visi_cell(lev, col, row);
			}
		}
	}
}

void Map::rebuild_visi(int z, int x, int y)
{
	platoon_local->set_visibility_changed();
	platoon_remote->set_visibility_changed();

	for (int lev = z - 1; lev <= z + 1; lev++) {
		if ((lev < 0) || (lev >= level))
			continue;
		for (int col = x - 1; col <= x + 1; col++) {
			if ((col < 0) || (col >= width * 10))
				continue;
			for (int row = y - 1; row <= y + 1; row++) {
				if ((row < 0) || (row >= height * 10))
					continue;
				build_visi_cell(lev, col, row);
			}
		}
	}
}

int Map::stopLOS(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 1;

	int ct = cell(oz, ox, oy)->type[part];
	return m_terrain->m_mcd[ct].Stop_LOS;
}

int Map::viewable(int oz, int ox, int oy, int dir)
{
	switch (dir) {
		case 0:
			if (stopLOS(oz, ox, oy, 2))
				return 0;
			break;
		case 1:
			if (stopLOS(oz, ox, oy, 2) || stopLOS(oz, ox + 1, oy + 1, 1) ||
			        stopLOS(oz, ox, oy + 1, 2) || stopLOS(oz, ox, oy + 1, 1))
				return 0;
			break;
		case 2:
			if (stopLOS(oz, ox, oy + 1, 1))
				return 0;
			break;
		case 3:
			if (stopLOS(oz, ox - 1, oy, 2) || stopLOS(oz, ox, oy + 1, 1) ||
			        stopLOS(oz, ox - 1, oy + 1, 2) || stopLOS(oz, ox - 1, oy + 1, 1))
				return 0;
			break;
		case 4:
			if (stopLOS(oz, ox - 1, oy, 2))
				return 0;
			break;
		case 5:
			if (stopLOS(oz, ox - 1, oy - 1, 2) || stopLOS(oz, ox, oy, 1) ||
			        stopLOS(oz, ox - 1, oy, 2) || stopLOS(oz, ox - 1, oy, 1))
				return 0;
			break;
		case 6:
			if (stopLOS(oz, ox, oy, 1))
				return 0;
			break;
		case 7:
			if (stopLOS(oz, ox, oy - 1, 2) || stopLOS(oz, ox + 1, oy, 1) ||
			        stopLOS(oz, ox, oy, 2) || stopLOS(oz, ox, oy, 1))
				return 0;
			break;
	}
	return 1;
}

int Map::viewable_further(int vz, int vx, int vy)
{
	if (stopLOS(vz, vx, vy, 0) || stopLOS(vz, vx, vy, 3))
		return 0;
	if (isStairs(vz, vx, vy))
		return 0;

	return 1;
}


int Map::stopWALK(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 1;

	int ct = cell(oz, ox, oy)->type[part];

	if (m_terrain->m_mcd[ct].TU_Walk == 255) return 1;

    if (part == 0 || part == 3) {
        switch (m_pathfind_mode) {
            case PF_TRUE:
                if (man(oz, ox, oy)) return 1;
                break;
            case PF_DISPLAY:
                if (man(oz, ox, oy) && visible(oz, ox, oy)) return 1;
                break;
        }
        if (isStairs(oz, ox, oy) && ((oz + 1 >= level) || !passable(oz + 1, ox, oy))) return 1;
        for (int z = oz; z > 0 && mcd(z, ox, oy, 0)->No_Floor && !isStairs(z - 1, ox, oy); z--) {
            if (!passable(z - 1, ox, oy)) return 1;
        }
    }

	return 0;
}

int Map::stopDOOR(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 0;

	int ct = cell(oz, ox, oy)->type[part];
	if (m_terrain->m_mcd[ct].Door == 1)
		return 1;
	return 0;
}

int Map::passable(int oz, int ox, int oy)
{
	if (stopWALK(oz, ox, oy, 0) || stopWALK(oz, ox, oy, 3))
		return 0;
	return 1;
}

int Map::passable(int oz, int ox, int oy, int dir)
{
//	Check if the destination point is too high
	int dx = ox + DIR_DELTA_X(dir);
	int dy = oy + DIR_DELTA_Y(dir);

	if (!cell_inside(oz, ox, oy) || !cell_inside(oz, dx, dy)) return 0;

	int height_o = -mcd(oz, ox, oy, 3)->T_Level;
	int height_d = -mcd(oz, dx, dy, 3)->T_Level;

	if (height_d - height_o > 10) return 0;

//	Do the rest of checks
	if (!passable(oz, dx, dy)) return 0;

	switch (dir) {
		case DIR_EAST:
			if (stopWALK(oz, ox, oy, 2))
				return 0;
			if (stopDOOR(oz, ox, oy, 2))
				return 0;
			break;
		case DIR_NORTHEAST:
			if (stopWALK(oz, ox, oy, 2) || stopWALK(oz, ox + 1, oy + 1, 1) ||
			        stopWALK(oz, ox, oy + 1, 2) || stopWALK(oz, ox, oy + 1, 1)) {
				return 0;
			}
			break;
		case DIR_NORTH:
			if (stopWALK(oz, ox, oy + 1, 1))
				return 0;
			if (stopDOOR(oz, ox, oy + 1, 1))
				return 0;
			break;
		case DIR_NORTHWEST:
			if (stopWALK(oz, ox - 1, oy, 2) || stopWALK(oz, ox, oy + 1, 1) ||
			        stopWALK(oz, ox - 1, oy + 1, 2) || stopWALK(oz, ox - 1, oy + 1, 1)) {
				return 0;
			}
			break;
		case DIR_WEST:
			if (stopWALK(oz, ox - 1, oy, 2))
				return 0;
			if (stopDOOR(oz, ox - 1, oy, 2))
				return 0;
			break;
		case DIR_SOUTHWEST:
			if (stopWALK(oz, ox - 1, oy - 1, 2) || stopWALK(oz, ox, oy, 1) ||
			        stopWALK(oz, ox - 1, oy, 2) || stopWALK(oz, ox - 1, oy, 1)) {
				return 0;
			}
			break;
		case DIR_SOUTH:
			if (stopWALK(oz, ox, oy, 1))
				return 0;
			if (stopDOOR(oz, ox, oy, 1))
				return 0;
			break;
		case DIR_SOUTHEAST:
			if (stopWALK(oz, ox, oy - 1, 2) || stopWALK(oz, ox + 1, oy, 1) ||
			        stopWALK(oz, ox, oy, 2) || stopWALK(oz, ox, oy, 1)) {
				return 0;
			}
			break;
	}

	return 2;
}


extern int mapscroll;

/**
 * Scroll the map by putting mouse on the edge of its movement area
 */
int Map::scroll(int mx, int my)
{
	int r = 0;
	if (mx == 0) {
		move(mapscroll, 0); r = 1;
	}
	if (mx >= SCREEN2W - 1) {
		move( -mapscroll, 0); r = 1;
	}
	if (my == 0) {
		move(0, mapscroll); r = 1;
	}
	if (my >= SCREEN2H - 1) {
		move(0, -mapscroll); r = 1;
	}
	return r;
}


int Map::open_door(int z, int x, int y, int dir)
{
	int door = 0, doorx = 0, doory = 0, doorz = z;

	switch (dir) {
		case 0:
			if (stopDOOR(z, x, y, 2)) {
				doorx = x;
				doory = y;
				door = 2;
			}
			break;
		case 2:
			if (stopDOOR(z, x, y + 1, 1)) {
				doorx = x;
				doory = y + 1;
				door = 1;
			}
			break;
		case 4:
			if (stopDOOR(z, x - 1, y, 2)) {
				doorx = x - 1;
				doory = y;
				door = 2;
			}
			break;
		case 6:
			if (stopDOOR(z, x, y, 1)) {
				doorx = x;
				doory = y;
				door = 1;
			}
			break;
	}
	if (door > 0) {

		int ct = cell(doorz, doorx, doory)->type[door];
		int mcd = m_terrain->m_mcd[ct].Alt_MCD;
		ASSERT(mcd < (int)m_terrain->m_mcd.size());

		m_cell[doorz][doorx][doory]->type[door] = 0;
		m_cell[doorz][doorx][doory]->type[3 - door] = mcd;
		rebuild_visi(doorz, doorx, doory);

		return 1;
	}
	return 0;
}


void Map::build_lof_cell(int _z, int _x, int _y, uint16 *lof_cell)
{
	memset(lof_cell, 0, 2 * 16 * 12);

	for (int lev = 0; lev < 12; lev++) {
		for (int part = 0; part < 4; part++) {
			int ct = m_cell[_z][_x][_y]->type[part];
			if (!ct)
				continue;

			int mcd = m_terrain->m_mcd[ct].LOFT[lev];

			for (int i = 0; i < 16; i++) {
				lof_cell[lev * 16 + i] |= m_loftemp[mcd * 16 + i];
			}
		}
	}
}

void Map::build_lof_cell_part(int _z, int _x, int _y, int _part, uint16 *lof_cell)
{
	memset(lof_cell, 0, 2 * 16 * 12);

	for (int lev = 0; lev < 12; lev++) {
		int ct = m_cell[_z][_x][_y]->type[_part];
		if (!ct)
			continue;

		int mcd = m_terrain->m_mcd[ct].LOFT[lev];

		for (int i = 0; i < 16; i++) {
			lof_cell[lev * 16 + i] |= m_loftemp[mcd * 16 + i];
		}

	}
}

BITMAP *Map::create_lof_bitmap(int lev, int col, int row)
{
	uint16 lof_cell[16 * 12];
	build_lof_cell(lev, col, row, lof_cell);

	BITMAP *bmp = create_bitmap(20 * 4, 20 * 3);
	clear_to_color(bmp, COLOR_BLACK1);

	int dir = -1, s = 0;     //, tl = 0;
	if (visible(lev, col, row))
		if (man(lev, col, row) != NULL) {
			dir = man(lev, col, row)->dir;
			if (man(lev, col, row)->m_state == SIT)
				s = 1;
			//tl = mcd(lev, col, row, 0)->T_Level;
			//tl += mcd(lev, col, row, 3)->T_Level;
		}

	for (int j = 0; j < 12; j++) {
		/*int Tlev = lev - tl/2;
		if (Tlev > 11) Tlev = 11;
		if (Tlev < 0) Tlev = 0;*/

		for (int i = 0; i < 16; i++) {
			uint16 l = lof_cell[j * 16 + i];
			//textprintf(screen, font, SCREEN2W, SCREEN2H-80+i*8, 1, "%04x ", l);
			for (int k = 0; k < 16; k++) {
				/*if (l & 0x8000) {
					putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 1);
				} else {
					if ((dir != -1) && (Soldier::bof[s][dir][j][i][15-k]))
						putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 52);
					else
						putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 12);
				}*/

				if (dir != -1) {
					if (Skin::m_bof[s][dir][j][i][15 - k])
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GREEN04);
					else if (l & 0x8000)
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_WHITE);
					else
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GRAY12);
				} else {
					if (l & 0x8000)
						//putpixel(screen, SCREEN2W+k  +(j/3)*20, SCREEN2H+i  +(2 - j%3 )*20, 1);
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_WHITE);
					else
						//putpixel(screen, SCREEN2W+k  +(j/3)*20, SCREEN2H+i  +(2 - j%3 )*20, 12);
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GRAY12);
				}
				l <<= 1;
			}
		}

	}
	return bmp;
}


void Map::show_lof_cell()
{
	BITMAP *bmp = create_lof_bitmap(sel_lev, sel_col, sel_row);
	blit(bmp, screen, 0, 0, SCREEN2W, SCREEN2H - 70, bmp->w, bmp->h);
	destroy_bitmap(bmp);

	bmp = create_bitmap(30, 90); clear_to_color(bmp, xcom1_color(0));
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 3; i++) {
			textprintf(bmp, font, 0, 80 - (k * 30 + i * 8), COLOR_WHITE,
			           "%d%d%d",
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][0],
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][1],
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][2]
			          );
		}
	}
	blit(bmp, screen, 0, 0, SCREEN2W + 90, SCREEN2H - 100, bmp->w, bmp->h);
	destroy_bitmap(bmp);
}


int Map::pass_lof_cell(int _z, int _x, int _y)
{
	int lev = _z / 12;
	int col = _x / 16;
	int row = _y / 16;

	uint16 lof_cell[16 * 12];
	build_lof_cell(lev, col, row, lof_cell);

	lev = _z % 12;
	col = _x % 16;
	row = _y % 16;

	int v = lof_cell[lev * 16 + (15 - col)] << row;
	if (v & 0x8000)
		return 0;

	return 1;
}

int Map::pass_lof_cell_part(int _z, int _x, int _y, int _part)
{
	int lev = _z / 12;
	int col = _x / 16;
	int row = _y / 16;

	uint16 lof_cell[16 * 12];
	build_lof_cell_part(lev, col, row, _part, lof_cell);

	lev = _z % 12;
	col = _x % 16;
	row = _y % 16;

	int v = lof_cell[lev * 16 + (15 - col)] << row;
	if (v & 0x8000)
		return 0;

	return 1;
}

void Map::destroy_cell_part(int lev, int col, int row, int _part)
{
	int ct = m_cell[lev][col][row]->type[_part];
	if (ct > 0) {
		int mcd = m_terrain->m_mcd[ct].Die_MCD;
		ASSERT(mcd < (int)m_terrain->m_mcd.size());

		if ((_part == 0) && (lev > 0)) {
			m_cell[lev][col][row]->type[_part] = 0;
			return ;
		}                                        
		
		if (_part == 0 && lev == 0 && mcd == 0)
			mcd = 1;					//"scorched earth" instead of blank space on the ground level

		if (mcd == 0 || mcd == 1)
			m_cell[lev][col][row]->type[_part] = mcd;
		else {
			if (_part != 0)
				m_cell[lev][col][row]->type[_part] = 0;
			m_cell[lev][col][row]->type[m_terrain->m_mcd[mcd].Tile_Type] = mcd;
		}
					
		if (m_terrain->m_mcd[ct].HE_Strength > 0)
			explode(-1, lev * 12 - 6, col * 16 , row * 16, HIGH_EXPLOSIVE, 3, m_terrain->m_mcd[ct].HE_Strength);

		rebuild_visi(lev, col, row);
	}
}

void Map::damage_cell_part(int lev, int col, int row, int _part, int _wtype)
{
	int dam = Item::obdata_damage(_wtype);
    // Currently just randomizing the damage to be from 0.5 to 1.5 of
    // the table value, NOT 0.0 to 2.0 as it was in X-Com.
    dam = (int) cur_random->getUniform(dam * 0.5, dam * 1.5);
	if (mcd(lev, col, row, _part)->Armour < dam) {
		destroy_cell_part(lev, col, row, _part);
	}
}


void Map::apply_hit(int _z, int _x, int _y, int _wtype)
{
	if (!pass_lof_cell(_z, _x, _y)) {
		for (int i = 0; i < 4; i++)
			if (!pass_lof_cell_part(_z, _x, _y, i)) {
				int lev = _z / 12;
				int col = _x / 16;
				int row = _y / 16;
				damage_cell_part(lev, col, row, i, _wtype);
			}
	}
}


void Map::drawline(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
	int z0, x0, y0;
	int zd, xd, yd;
	int xg, yg;
	int xg2, yg2;

	x0 = x_s * 16 + 8; y0 = y_s * 16 + 8; z0 = z_s * 12 + 8;
	xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 8;

	xg = x + x0 + y0;
	//yg = y - (x0+1.0)/2.0 + y0/2.0 - z0*23.5/12.0;
	yg = (int)(y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

	xg2 = x + xd + yd;
	//yg2 = y - (xd+1.0)/2.0 + yd/2.0 - zd*23.5/12.0;
	yg2 = (int)(y - (xd + 1) / 2.0 + yd / 2.0 - zd * 2.0 - 2);

	line(screen2, xg, yg, xg2, yg2, COLOR_YELLOW);
}



Place *Map::find_item(Item *it, int &lev, int &col, int &row)
{
	for (lev = 0; lev < level; lev++)
		for (col = 0; col < width*10; col++)
			for (row = 0; row < height*10; row++)
				if (m_cell[lev][col][row]->get_place()->isthere(it))
					return m_cell[lev][col][row]->get_place();
	return NULL;
}

int Map::find_place_num(Place *pl, int lev, int col, int row)
{
	if (man(lev, col, row) != NULL)
		return man(lev, col, row)->place(pl);
	if (pl == place(lev, col, row))
		return P_MAP;
	return -1;
}


int Map::inside(int z, int x, int y)
{
	if ((x < 0) || (x >= width * 10 * 16) ||
	        (y < 0) || (y >= height * 10 * 16) ||
	        (z < 0) || (z >= level * 12))
		return 0;
	return 1;
}

int Map::cell_inside(int z, int x, int y)
{
	if ((x < 0) || (x >= width * 10) ||
	        (y < 0) || (y >= height * 10) ||
	        (z < 0) || (z >= level))
		return 0;
	return 1;
}

int Map::find_ground(int lev, int col, int row)
{
	while (lev > 0) {
		if (!mcd(lev, col, row, 0)->No_Floor) {
			break;
		}
		lev--;
	}
	return lev;
}

#define FI_STEP (PI / 64.0)
#define TE_STEP (PI / 64.0)

static char field[8 * 6*10 * 6*10];

int Map::calc_visible_cells(Soldier *watcher, int z, int x, int y, int dir, char *visicells, int *ez, int *ex, int *ey)
{
	visicells[z * width * 10 * height * 10 + x * height * 10 + y] = 1;
	if (z > 0) {
		if (isStairs(z - 1, x, y)) {
			visicells[(z - 1) * width * 10 * height * 10 + x * height * 10 + y] = 1;
		}
	}

	memset(field, 0, level * width * 10 * height * 10);
	int en = 0;
	int ang = dir * 32;

	for (int fi = 24; fi <= 128 - 24; fi += 4) {
		fixed cos_fi = fcos(itofix(fi));
		fixed sin_fi = fsin(itofix(fi));

		for (int te = ang - 32; te <= ang + 32; te += 1) {
			fixed cos_te = fcos(itofix(te));
			fixed sin_te = fsin(itofix(te));

			int oz = z, ox = x, oy = y;
			int l;
			int vz, vx, vy;
			int smokeway = 0;

			for (l = 1; l < 18 - smokeway*3; l++) { /////////////from smoke

				vx = x + fixtoi(fmul(itofix(l), fmul(cos_te, sin_fi)));
				vy = y + fixtoi(fmul(itofix(l), fmul(sin_te, sin_fi)));
				vz = z + fixtoi(fmul(itofix(l), cos_fi));

				if (!cell_inside(vz, vx, vy))
					break;
				if ((vz == oz) && (vx == ox) && (vy == oy))
					continue;

				if (!m_cell[oz][ox][oy]->visi[vz - oz + 1][vx - ox + 1][vy - oy + 1]) break;

				visicells[vz * width * 10 * height * 10 + vx * height * 10 + vy] = 1;

				if (field[vz * width * 10 * height * 10 + vx * height * 10 + vy] == 0)
					if (man(vz, vx, vy) != NULL) {
						if (!watcher->get_platoon()->belong(man(vz, vx, vy))) {
							ez[en] = vz;
							ex[en] = vx;
							ey[en] = vy;
							en++;
						}
					}
				field[vz * width * 10 * height * 10 + vx * height * 10 + vy] = 1;

				oz = vz; ox = vx; oy = vy;

				if (!viewable_further(vz, vx, vy)) break;

				if (smog_state(vz, vx, vy) != 0) if (++smokeway > 2) break;
			}
		}
	}

	return en;
}

double distance_3d(double v1, double v2, double v3)
{
    // this procedure probably should be moved somewhere else
    return sqrt( v1*v1 + v2*v2 + v3*v3 );
}

int calculate_hitdir(double dz, double dx, double dy)
{
    double UNDER_ARMOR_ANGLE = PI/3; // to rules.h ?
    
    int dir = DAMAGEDIR_UNDER;
    double hor_dist = distance_3d(dx, dy, 0);
    if (atan(dz / hor_dist) >= -UNDER_ARMOR_ANGLE) {
        dir = int(round( - 2 + (4.0/PI)*atan2(dx, dy) ));
        if (hor_dist==0) dir = 0; // this should not really happen
        if (dir < 0) dir += 8;
    }
    //printf("dz, dx, dy: %f, %f, %f, %f, %d \n", dz, dx, dy, atan(dz/hor_dist), dir );
    return dir;
}

int Map::explode(int sniper, int z, int x, int y, int type, int max_range, int max_damage)
{
    // max range is unused, max_damage is not realy needed, as it can be get from item type

    // TODO probably there should be damage_type instead of type
    int damage_type = Item::obdata_damageType(type);
    int damage, hit_dir = 0;
    
    // move to rules.h
    double EXPL_BORDER_DAMAGE = 0.5; // how much damage does explosion on its border
    double HEIGHT_RATIO = 2; // how high is one level in squares
    
    // should be sound associated with given weapon
    soundSystem::getInstance()->play(SS_CV_GRENADE_BANG);
    
    // convert to coords relative to center of a cell
    double lev = double(z)/12 - 0.5;
    double col = double(x)/16 - 0.5;
    double row = double(y)/16 - 0.5;
    
    //printf("z, x, y        : %d, %d, %d \n", z, x, y);
    //printf("lev, col, row  : %f, %f, %f \n", lev, col, row);
    
    double range = (double(max_damage) / 10) - 2;
    for (int l = 0; l <= level; l++)
        for (int r = int(floor(row - range)); r <= int(ceil(row + range)); r++)
            for (int c = int(floor(col - range)); c <= int(ceil(col + range)); c++) {
                double distance = distance_3d(row - double(r), col - double(c), (lev - double(l)) * HEIGHT_RATIO);
                if (distance <= range) {
                    damage = int ( ( EXPL_BORDER_DAMAGE + (1 - EXPL_BORDER_DAMAGE)*(range - distance)/range )*double(max_damage) );
                    if (damage > 0 && cell_inside(l, c, r)) {
                        if (man(l, c, r) != NULL) {
                            //printf("l, c, r        : %i, %i, %i \n", l, c, r);
                            hit_dir = calculate_hitdir((lev - double(l)) * HEIGHT_RATIO, double(c) - col, double(r) - row);
                        }
                        explocell(sniper, l, c, r, damage, damage_type, hit_dir);
                    }
                }
            }
    return 1;
} 
  
bool Map::check_mine(int lev, int col, int row) 
{
	for (int c = col - 1; c <= col + 1; c++) {
		for (int r = row - 1; r <= row + 1; r++) {
			if (!cell_inside(lev, c, r)) continue;
			if (place(lev, c, r)->check_mine())
				return true;
		}
	}
	return false;
}

void Map::explocell(int sniper, int lev, int col, int row, int damage, int damage_type, int hitdir)
{
    int DEFAULT_SMOKE_TIME = 2; // move to rules.h
  
	set_smog_state(lev, col, row, 8);
	set_smog_time(lev, col, row, 0);
  
 	//for(int i=0; i<4; i++) {
 	for (int i = 3; i >= 0; i--) {
 		if (mcd(lev, col, row, i)->Fuel > smog_time(lev, col, row)) {
 			set_smog_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
 			if (mcd(lev, col, row, i)->Armour < damage) {

 				if (damage_type == DT_INC) {
 					set_fire_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
 					set_fire_state(lev, col, row, 4);
 				}
 			}
 		} else {
 			set_smog_time(lev, col, row, DEFAULT_SMOKE_TIME);
 		}
 		if ((mcd(lev, col, row, i)->Armour < damage) && (damage_type != DT_STUN)) {
 			destroy_cell_part(lev, col, row, i);
 		}
 	}
  
	place(lev, col, row)->damage_items(damage);
	if (man(lev, col, row) != NULL) {
      if (hitdir != DAMAGEDIR_UNDER)
          hitdir = (man(lev, col, row)->get_dir() + (hitdir + 4)) % 8;
	 	  man(lev, col, row)->explo_hit(sniper, damage, damage_type, hitdir);
  }
}

/**
 * End-of-turn - save game state into a buffer as a large block of text
 */                                        
int Map::eot_save(char *buf, int & buf_size)
{
	buf_size += sprintf(buf + buf_size, "\r\nmap level=%d, width=%d, height=%d\r\n", level, width, height);

	for (int k = 0; k < level; k++) {
		for (int i = 0; i < 10 * width; i++) {
			for (int j = 0; j < 10 * height; j++) {
				buf_size += sprintf(buf + buf_size, " %d_%d_%d_%d ", cell(k, i, j)->type[0], cell(k, i, j)->type[1], cell(k, i, j)->type[2], cell(k, i, j)->type[3]);
			}
			buf_size += sprintf(buf + buf_size, "\r\n");
		}
		buf_size += sprintf(buf + buf_size, "\r\n\r\n\r\n");
	}
	buf_size += saveitems(buf + buf_size);
	return buf_size;
}

int Map::saveitems(char *txt)
{
	int len = 0;
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10 * width; i++)
			for (int j = 0; j < 10 * height; j++)
				len += place(k, i, j)->save_items("(%d,%d,%d)", k, i, j, txt + len);

	return len;
}

int MAP_WIDTH, MAP_HEIGHT;

void Map::new_GEODATA(GEODATA *md)
{
	std::string terrain_name = terrain_set->get_random_terrain_name();

	if (net->is_network_game()) {
	    ASSERT(g_net_allowed_terrains.size() > 0);
		while (g_net_allowed_terrains.find(terrain_name) == g_net_allowed_terrains.end()) {
			terrain_name = terrain_set->get_random_terrain_name();
		}
	}

	new_GEODATA(md, terrain_name);
}

void Map::new_GEODATA(GEODATA *md, const std::string &terrain_name)
{
	// Check map size settings
	if (MAP_WIDTH < 2) MAP_WIDTH = 2;
	if (MAP_HEIGHT < 2) MAP_HEIGHT = 2;

	if (MAP_WIDTH > 6) MAP_WIDTH = 6;
	if (MAP_HEIGHT > 6) MAP_HEIGHT = 6;

	// Try to use map generator function defined in terrain description
	int stack_top = lua_gettop(L);
	lua_pushstring(L, "MapGenerator");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_isfunction(L, -1)); 
    // Enter [terrain_name] table
	lua_pushstring(L, terrain_name.c_str());
	lua_pushnumber(L, MAP_WIDTH);
	lua_pushnumber(L, MAP_HEIGHT);
	lua_safe_call(L, 3, 1);
	if (!lua_istable(L, -1)) {
		lua_settop(L, stack_top);
		// Fallback to default random shuffle map generation algorithm
		terrain_set->create_geodata(terrain_name, MAP_WIDTH, MAP_HEIGHT, *md);
		return;
	}
	// We should have map description returned at the top of stack here
	load_map_from_top_of_lua_stack(md);
	lua_settop(L, stack_top);
}

/**
 * Checks if GEODATA structure contains correct and usable map
 */
int Map::valid_GEODATA(GEODATA *md)
{
	std::string terrain_name = terrain_set->get_terrain_name(md->terrain);

	if ((md->x_size > 6) || (md->y_size > 6) ||
		(md->x_size < 2) || (md->y_size < 2) ||
        (md->z_size != 4) || terrain_name == "") return 0;
        
	if (net->is_network_game() && 
        g_net_allowed_terrains.find(terrain_name) == g_net_allowed_terrains.end()) return 0;

	return 1;
}

/**
 * Supplementary function that interprets a value at the top of lua stack as
 * a map description and tries to load it
 */
bool Map::load_map_from_top_of_lua_stack(GEODATA *mapdata)
{
	memset(mapdata, 0, sizeof(GEODATA));
	mapdata->z_size = 4; // !!! Hack
	
	int stack_top = lua_gettop(L);

    // we have a table with map data returned at the top of the stack
	if (!lua_istable(L, -1)) {
		lua_settop(L, stack_top);
		return false;
	}

	lua_pushstring(L, "Name");
	lua_gettable(L, -2);
	if (!lua_isstring(L, -1)) { lua_settop(L, stack_top); return false; }
	int tid = terrain_set->get_terrain_id(lua_tostring(L, -1));
	if (tid < 0) { lua_settop(L, stack_top); return false; }
	mapdata->terrain = (uint16)tid;
	lua_pop(L, 1);

	lua_pushstring(L, "SizeX");
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
	mapdata->x_size = (uint16)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "SizeY");
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
	mapdata->y_size = (uint16)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "Scenario");
	lua_gettable(L, -2);
	int mission = lua_isstring(L, -1);
	lua_pop(L, 1);
	
	if (mission) {
		lua_pushstring(L, "Scenario");
		lua_gettable(L, -2);
		if (!lua_isstring(L, -1)) { lua_settop(L, stack_top); return false; }
		if (!scenario->new_scenario(lua_tostring(L, -1))) { lua_settop(L, stack_top); return false; }
		lua_pop(L, 1);
	
		lua_pushstring(L, "TargetX1");
		lua_gettable(L, -2);
		if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
		scenario->x1 = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_pushstring(L, "TargetY1");
		lua_gettable(L, -2);
		if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
		scenario->y1 = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		lua_pushstring(L, "TargetX2");
		lua_gettable(L, -2);
		if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
		scenario->x2 = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		lua_pushstring(L, "TargetY2");
		lua_gettable(L, -2);
		if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false;	}
		scenario->y2 = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}

	lua_pushstring(L, "Mapdata");
	lua_gettable(L, -2);
	if (!lua_istable(L, -1)) { lua_settop(L, stack_top); return false; }

	int x = 0;

	for (int row = 1; row <= mapdata->y_size; row++) {
		lua_pushnumber(L, row);
		lua_gettable(L, -2);
		if (!lua_istable(L, -1)) { lua_settop(L, stack_top); return false; }

		for (int col = 1; col <= mapdata->x_size; col++) {
			lua_pushnumber(L, col);
			lua_gettable(L, -2);
			if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }

			mapdata->mapdata[x++] = lua_tonumber(L, -1) == -1 ? 0xFE : (uint8)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	lua_settop(L, stack_top);
	return true;
}

/**
 * Loads GEODATA structure from lua-file
 */
bool Map::load_GEODATA(const char *filename, GEODATA *mapdata)
{
	int stack_top = lua_gettop(L);
	lua_safe_dofile(L, F(filename));
	bool result = load_map_from_top_of_lua_stack(mapdata);
	lua_settop(L, stack_top);
	return result;
}

/**
 * Saves GEODATA structure to lua-file
 */
bool Map::save_GEODATA(const char *filename, GEODATA *mapdata)
{
	FILE *fh = fopen(F(filename), "wt");
	if (fh == NULL) return false;

	fprintf(fh, "return {\n");
	fprintf(fh, "\tName = \"%s\",\n", terrain_set->get_terrain_name(mapdata->terrain).c_str());
	fprintf(fh, "\tSizeX = %d, SizeY = %d,\n", mapdata->x_size, mapdata->y_size);
	
	fprintf(fh, "\tMapdata = {\n");

	int x = 0;
	for (int i = 0; i < mapdata->y_size; i++) {
		fprintf(fh, "\t\t{ ");
		for (int j = 0; j < mapdata->x_size; j++) {
			int ch = mapdata->mapdata[x++];
			if (ch == 0xFE) fprintf(fh, "-1"); else fprintf(fh, "%02d", ch);
			if (j + 1 < mapdata->x_size) fprintf(fh, ", ");
		}
		fprintf(fh, " },\n");
	}

	fprintf(fh, "\t}\n");
	fprintf(fh, "}\n");

	fclose(fh);
	return true;
}

int Map::walk_time(int _z, int _x, int _y)
{
	int time;
	time = (int)mcd(_z, _x, _y, 0)->TU_Walk;
	time += (int)mcd(_z, _x, _y, 3)->TU_Walk;
	if (time < 4) time = 4;
	return time;
}

bool Map::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	for (int lev = 0; lev < level; lev++)
		for (int col = 0; col < 10 * width; col++)
			for (int row = 0; row < 10 * height; row++)
				PersistWriteObject(archive, m_cell[lev][col][row]);

	return true;
}

bool Map::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	m_cell = new Cell***[level];
	for (int i = 0; i < level; i++) {
		m_cell[i] = new Cell ** [10 * width];
		for (int j = 0; j < 10 * width; j++) {
			m_cell[i][j] = new Cell * [10 * height];
		}
	}

	for (int lev = 0; lev < level; lev++)
		for (int col = 0; col < 10 * width; col++)
			for (int row = 0; row < 10 * height; row++)
				PersistReadObject(archive, m_cell[lev][col][row]);

    load_terrain_pck(m_terrain_name, m_terrain);

	m_minimap_area = new MinimapArea(this, SCREEN_W - SCREEN2W, SCREEN2H);

	return true;
}

//////////////////////////////////////////////////////////////////////////////

#undef map

#define MAP_BLOCKS_LIMIT 100

Terrain::Terrain(const std::string &terrain_name)
{
	int stack_top = lua_gettop(L);
    // Enter 'TerrainTable' table
	lua_pushstring(L, "TerrainTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
	// Enter [terrain_name] table
	lua_pushstring(L, terrain_name.c_str());
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1)); 
    // Extract terrain crc32
	lua_pushstring(L, "Crc32");
	lua_gettable(L, -2);
	ASSERT(lua_isnumber(L, -1)); 
	m_crc32 = (unsigned long)lua_tonumber(L, -1);
	lua_pop(L, 1);
	m_name = terrain_name;
	// Enter 'Maps' table
	lua_pushstring(L, "Maps");
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1)); 
	m_rand_weight = 1;

	m_blocks.resize(MAP_BLOCKS_LIMIT);
	std::vector<block_info>::size_type index;
	for (index = 0; index < m_blocks.size(); index++) {
		lua_pushnumber(L, index);
		lua_gettable(L, -2);
		if (!lua_isstring(L, -1)) {
			m_blocks[index].rand_weight = 0;
			lua_pop(L, 1);
			continue;
		}

		const char *fname = lua_tostring(L, -1);
		int fh = open(fname, O_RDONLY | O_BINARY);
		if (fh == -1)
		{
			m_blocks[index].rand_weight = 0;
			lua_pop(L, 1);
			continue;
		}

		unsigned char buffer[3];
		read(fh, buffer, 3);
		close(fh);

		ASSERT(buffer[0] % 10 == 0);
		ASSERT(buffer[1] % 10 == 0);
		m_blocks[index].x_size      = buffer[0] / 10;
		m_blocks[index].y_size      = buffer[1] / 10;
		m_blocks[index].z_size      = buffer[2];
		m_blocks[index].rand_weight = 1;

		lua_pop(L, 1);
	}

	lua_settop(L, stack_top);
}

Terrain::~Terrain()
{
}

int Terrain::get_random_block()
{
	int randmax = 0;
	std::vector<block_info>::size_type i;

	for (i = 0; i < m_blocks.size(); i++)
		randmax += m_blocks[i].rand_weight;

	ASSERT(randmax > 0);

    int randval = rand() % randmax;

	for (i = 0; i < m_blocks.size(); i++) {
		randval -= m_blocks[i].rand_weight;
		if (randval < 0) return i;
	}
	ASSERT(false);
	return -1;
}

bool Terrain::create_geodata(GEODATA &gd)
{
	unsigned char map[6][6];
	memset(map, 0xFF, 36);
	for (int y = 0; y < gd.y_size; y++)
		for (int x = 0; x < gd.x_size; x++) {
			if (map[y][x] != 0xFF) continue;

			int i;
			while (true) {
				i = get_random_block();
				if (x + m_blocks[i].x_size > gd.x_size || y + m_blocks[i].y_size > gd.y_size)
					continue;
				int no_overlap = true;
				for (int by = 0; by < m_blocks[i].y_size; by++)
					for (int bx = 0; bx < m_blocks[i].x_size; bx++)
						if (map[y + by][x + bx] != 0xFF)
							no_overlap = false;
				if (no_overlap) break;
			}

			for (int by = 0; by < m_blocks[i].y_size; by++)
				for (int bx = 0; bx < m_blocks[i].x_size; bx++) {
					map[y + by][x + bx] = 0xFE;
					gd.mapdata[(y + by) * gd.x_size + x + bx] = 0xFE;
				}

			map[y][x] = i;
			gd.mapdata[y * gd.x_size + x] = i;
		}
	return true;
}

//////////////////////////////////////////////////////////////////////////////

TerrainSet::TerrainSet()
{
	int terrain_id = 0;
	int stack_top = lua_gettop(L);
	lua_pushstring(L, "TerrainTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		ASSERT(lua_isstring(L, -2));
		std::string terrain_name = lua_tostring(L, -2);

		terrain[terrain_id++] = new Terrain(terrain_name);

		lua_pop(L, 1);
	}

	lua_settop(L, stack_top);

    // ?? gettext:
	if (terrain.empty()) {
		display_error_message(
            "Terrain data initialization failed." 
            "At least one valid terrain required." );
	}
}

TerrainSet::~TerrainSet()
{
	std::map<int, Terrain *>::iterator it;

	for (it = terrain.begin(); it != terrain.end(); ++it)
		delete it->second;
}

bool TerrainSet::create_geodata(const std::string &terrain_name, int x_size, int y_size, GEODATA &gd)
{
	int terrain_index = get_terrain_id(terrain_name);
	if (terrain_index < 0) return false;

	memset(&gd, 0, sizeof(gd));
	gd.terrain   = terrain_index;
	gd.x_size    = x_size;
	gd.y_size    = y_size;
	gd.z_size    = 4;
	
	return terrain[terrain_index]->create_geodata(gd);
}

int TerrainSet::get_random_terrain_id()
{
	int randmax = 0;
	std::map<int, Terrain *>::iterator it;

	for (it = terrain.begin(); it != terrain.end(); ++it)
		randmax += it->second->get_rand_weight();

    int randval = rand() % randmax;

	for (it = terrain.begin(); it != terrain.end(); ++it) {
		randval -= it->second->get_rand_weight();
		if (randval < 0) return it->first;
	}
	ASSERT(false);
	return -1;
}

/**
 * Displays dialog asking the user to select terrain type from the list of 
 * available terrains, additional requirement for network games is
 * that remote user should have these maps installed too.
 *
 * @param default_choice  terrain name that is active by default
 * @returns               terrain name, selected by user
 */
std::string TerrainSet::select_terrain_gui_dialog(
	const std::string &default_choice)
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
	int default_index = 0;
	std::vector<std::string> gui_list;
	std::map<int, Terrain *>::iterator it;
	for (it = terrain.begin(); it != terrain.end(); ++it) {
		if (net->is_network_game()) {
			ASSERT(g_net_allowed_terrains.size() > 0);
			if (g_net_allowed_terrains.find(it->second->get_name()) == g_net_allowed_terrains.end())
				continue;
		}
		gui_list.push_back(it->second->get_name());
	}
	std::sort(gui_list.begin(), gui_list.end());
	for (int i = 0; i < (int)gui_list.size(); i++)
		if (gui_list[i] == default_choice) {
			default_index = i;
			break;
		}
	
	int result = gui_select_from_list(
		300, 200,
        _("Select terrain type"), 
		gui_list,
		default_index);
	
	return gui_list[result];
}
