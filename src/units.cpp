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
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "video.h"
#include "multiplay.h"
#include "units.h"
#include "editor.h"
#include "map.h"
#include "wind.h"


Units::Units()
{
	reset();
}

void Units::reset()
{
	size = 0;
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));
	memset(cost, 0, sizeof(cost));
	memset(lev, 0, sizeof(lev));
	memset(col, 0, sizeof(col));
	memset(row, 0, sizeof(row));
	SEND = 0; START = 0;
	selected = -1;
}

void Units::reset_selections()
{
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));
	memset(lev, 0, sizeof(lev));
	memset(col, 0, sizeof(col));
	memset(row, 0, sizeof(row));
	SEND = 0; START = 0;
	selected = -1;
}

void Units::set_pos(PanPos pos, int gx, int gy, int gmx, int gmw, int gmy, int gmh)
{
	this->pos = pos;
	this->gx = gx;
	this->gy = gy;
	this->gmx = gmx;
	this->gmw = gmw;
	this->gmy = gmy;
	this->gmh = gmh;
}

int Units::select_unit(int num, int mx, int my)
{
	if (num >= size)
		return 0;
	x[num] = mx;
	y[num] = my;
	SEND = 0; START = 0;
	return 1;
}

int Units::deselect_unit(int num)
{
	if (num >= size)
		return 0;
	x[num] = 0;
	y[num] = 0;
	SEND = 0; START = 0;
	return 1;
}


int Units::add(int num, char *nm, int ct)
{
	size = num;
	if (size == 19)
		return 0;
	strcpy(name[size], nm);
	cost[size] = ct;
	size++;
	SEND = 0; START = 0;

	/*if (size == 19)
		return 0;
	strcpy(name[size], nm);
	cost[size] = ct;
	size++;
	SEND = 0; START = 0;*/ 
	return 1;
}

void Units::print(int gcol)
{
	text_mode( -1);
	int x1, y1, x2, y2, color = 60;
	int i;
	for (i = 0; i < size; i++) {
		if (selected == i) {
			if (pos == POS_LEFT)
				line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, 1);
			else
				line(screen2, gx - 4, gy + i * 15 + 3, mouse_x, mouse_y, 1);
			color = 120;
		} else
			if (x[i] != 0) {
				rectfill(screen2, gmx + col[i] * 4 + 1, gmy + row[i] * 4 + 1,
				         gmx + col[i] * 4 + 3, gmy + row[i] * 4 + 3,
				         144);      //yellow
				if (pos == POS_LEFT) {
					line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
					textprintf(screen2, font, gx - 60, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
				} else {
					line(screen2, gx - 4, gy + i * 15 + 3, x[i], y[i], gcol);
					textprintf(screen2, font, gx + 20 * 8 + 5, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
				}
				color = 53;
			} else {
				color = 60;
			}

		x1 = gx - 2; x2 = gx + 20 * 8 + 2;
		y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;
		rectfill(screen2, x1, y1, x2, y2, color);

		//textprintf(screen2, font, gx, gy+i*15, gcol, "%d", cost[i]);
		textprintf(screen2, font, gx, gy + i * 15, gcol, "%s", name[i]);

		/*if (mouse_inside(x1, y1, x2, y2)) {
			textprintf(screen2, font, gx, gy+200, gcol, "%d", cost[i]);
		}*/

	}
	draw_text();

	rect(screen2, gmx, SCREEN2H - 35, gmx + gmw, SCREEN2H - 5, 1);
	textout_centre(screen2, font, "MAP", gmx + gmw / 2, SCREEN2H - 29, 8);
	textout_centre(screen2, font, "NEW", gmx + gmw / 2 - gmw / 4, SCREEN2H - 17, 1);
	textout_centre(screen2, font, "LOAD", gmx + gmw / 2 + gmw / 4, SCREEN2H - 17, 1);


	//textout_centre(screen2, large, "START", gmx+gmw/2, SCREEN2H-49, 33);
	//textout_centre(screen2, large, "CONFIRM", gmx+gmw/2, SCREEN2H-69, 33);

	int points = 0;
	for (int n = 0; n < size; n++) {
		if (x[n] != 0) {
			Soldier * ss = editor->platoon()->findman(name[n]);
			assert(ss != NULL);
			points += ss->calc_ammunition_cost();
		}
	}
	//textout(screen2, small, "INFO", gx, 160, 1);
	textprintf_centre(screen2, small, gx + 10 * 8, 160, 50, "total men points=%d", points);


	char buf[10000]; memset(buf, 0, sizeof(buf));
	int len = 0;

	for (i = 0; i < size; i++) {
		if (x[i] == 0)
			continue;
		Soldier *ss = editor->platoon()->findman(name[i]);
		assert(ss != NULL);
		if (ss != NULL) {
			//ss->build_items_stats(buf, len);
			build_items_stats(&ss->id, buf, len);
		}
	}

	/*if (pos == POS_LEFT)
		draw_items_stats(0, 160+10, buf, len);
	else*/
	draw_items_stats(gx, 160 + 10, buf, len);
}

void Units::build_items_stats(ITEMDATA *id, char *buf, int &len)
{
	for (int i = 0; i < id->num; i++) {
		buf[len++] = id->type[i];
	}
}

//extern char weapon_in_use[16];
extern int weapon[];

void Units::draw_items_stats(int gx, int gy, char *buf, int len)
{
	int aa = 0;
	for (int w = 0; w < 40; w++) {
		if (Item::obdata[weapon[w]].isAmmo)
			continue;
		int num = 0;
		for (int i = 0; i < len; i++) {
			if (weapon[w] == buf[i])
				num++;
		}
		if (num != 0) {
			textprintf(screen2, small, gx + (aa / 72) * 90, gy + (aa % 72),
			           1, "%s=%d", Item::obdata[weapon[w]].name, num);
			aa += 9;
		}
	}
}


void Units::print_simple(int gcol)
{
	text_mode( -1);
	int i, x1, y1, x2, y2, color = 60;
	for (i = 0; i < size; i++) {
		if (x[i] != 0) {
			if (FLAGS & F_PLANNERDBG) { // show lines of remote
				if (pos == POS_LEFT) {
					line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
				} else {
					line(screen2, gx - 4, gy + i * 15 + 3, x[i], y[i], gcol);
				}
			}
			color = 53;
		} else {
			color = 60;
		}

		x1 = gx - 2; x2 = gx + 20 * 8 + 2;
		y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;
		rectfill(screen2, x1, y1, x2, y2, color);

		//textprintf(screen2, font, gx, gy+i*15, gcol, "%d", cost[i]);
		textprintf(screen2, font, gx, gy + i * 15, gcol, "%s", name[i]);

		/*if (mouse_inside(x1, y1, x2, y2)) {
			textprintf(screen2, font, gx, gy+200, gcol, "%d", cost[i]);
		}*/
	}
	draw_text();

	if (!SEND)
		return ;

	int points = 0;
	for (i = 0; i < pd_remote->size; i++) {
		points += pd_remote->md[i].TimeUnits +
		          pd_remote->md[i].Health +
		          pd_remote->md[i].Firing +
		          pd_remote->md[i].Throwing;
	}
	textprintf_centre(screen2, small, gx + 10 * 8, 160, 50, "total men points=%d", points);

	char buf[10000]; memset(buf, 0, sizeof(buf));
	int len = 0;

	for (i = 0; i < pd_remote->size; i++) {
		build_items_stats(&pd_remote->id[i], buf, len);
	}

	/*if (pos == POS_LEFT)
		draw_items_stats(0, 160+10, buf, len);
	else*/
	draw_items_stats(gx, 160 + 10, buf, len);
}

void Units::draw_text()
{
	text_mode( -1);

	//rect(screen2, gx+50, SCREEN2H-35, gx+20*8-50, SCREEN2H-5, 1);
	//rect(screen2, gx+40, SCREEN2H-35, gx+20*8-40, SCREEN2H-5, 1);
	rect(screen2, gx, SCREEN2H - 35, gx + 20 * 8, SCREEN2H - 5, 1);

	if (pos == POS_LEFT) {
		//textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "SERVER");
		textout_centre(screen2, font, "SERVER", gx + 10 * 8, SCREEN2H - 29, 8);
	} else {
		//textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "CLIENT");
		textout_centre(screen2, font, "CLIENT", gx + 10 * 8, SCREEN2H - 29, 8);
	}

	int color = 40;      //red
	if (SEND)
		color = 55;      //green

	textout_centre(screen2, font, "SEND", gx + 15 * 8, SCREEN2H - 16, color);

	if (START)
		color = 55;      //green
	else
		color = 40;      //red

	textout_centre(screen2, font, "START", gx + 5 * 8, SCREEN2H - 16, color);
}

void Units::execute(Map *map, int map_change_allowed)
{
	if (selected != -1) {
		int mx = mouse_x;
		int my = mouse_y;
		int c = (mx - gmx) / 4;
		int r = (my - gmy) / 4;

		if (!(FLAGS & F_PLANNERDBG)) { // allow place at any position
			if (pos == POS_LEFT) {
				if (c > 9)
					return ;
			} else {
				if (c < gmw / 4 - 10)
					return ;
			}
		}

		if (!map->passable(0, c, r)) return;

		for (int s = 0; s < size; s++) {
			if (s == selected)
				continue;
			if ((x[s] != 0) && (col[s] == c) && (row[s] == r))
				return ;
		}

		x[selected] = mx;
		y[selected] = my;
		lev[selected] = 0;
		col[selected] = c;     //(x[selected] - gx - (20*8+20)) / 4;
		row[selected] = r;     //(y[selected] - gy + 10) / 4;
		net->send_select_unit(selected, x[selected], y[selected]);
		deselect();
		return ;
	}

	int x1, y1, x2, y2;
	x1 = gx - 2; x2 = gx + 20 * 8 + 2;

	for (int i = 0; i < size; i++) {
		y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;

		if (mouse_inside(x1, y1, x2, y2)) {
			if (key[KEY_LCONTROL]) { // do editor
				//if (x[i]) // don't edit selected
				//	return;
				if (editor->set_man(name[i])) {
					fade_out(10);
					clear(screen);

					editor->show(0);      //!NEXTPREV select
					editor->build_Units(*this);
					reset_video();

					destroy_bitmap(screen2);
					screen2 = create_bitmap(640, SCREEN2H - 1); clear(screen2);
					::set_mouse_range(0, 0, 639, SCREEN2H - 1);
					/*if (x[i]) {
						x[i] = 0; y[i] = 0;
						net->send_deselect_unit(i);
					}*/
				}
				return ;
			}
			selected = i;
			limit_mouse_range();
			return ;
		}
		if (x[i] != 0) {
			if (mouse_inside(gmx + col[i] * 4 + 0, gmy + row[i] * 4 + 0,
			                 gmx + col[i] * 4 + 3, gmy + row[i] * 4 + 3)) {
				selected = i;
				limit_mouse_range();
				return ;
			}

		}
	}
	selected = -1;

	/*if (mouse_inside(gx+50, SCREEN2H-35, gx+20*8-50, SCREEN2H-5)) {
		editor->send_Units(*this);
		return;
	}*/

	if (mouse_inside(gx + 15 * 8 - 20, SCREEN2H - 20, gx + 15 * 8 + 20, SCREEN2H - 5)) {
		//"SEND"
		editor->send_Units(*this);
		return ;
	}

	if (mouse_inside(gx + 5 * 8 - 20, SCREEN2H - 20, gx + 5 * 8 + 20, SCREEN2H - 5)) {
		//"START"
		//editor->send_Units(*this);
		net->send_finish_planner();
		//CONFIRM_FINISH_PLANNER = 1;
		return ;
	}

	if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - gmw / 4 - 20, SCREEN2H - 20, gmx + gmw / 2 - gmw / 4 + 20, SCREEN2H - 5)) {
		//"NEW"
		Map::new_GEODATA(&mapdata);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
	}

	if (mouse_inside(gmx + gmw / 2 + gmw / 4 - 20, SCREEN2H - 20, gmx + gmw / 2 + gmw / 4 + 20, SCREEN2H - 5)) {
		//"LOAD"
		char path[1000]; *path = 0;
		if (file_select("GEODATA file", path, "MAP;DAT")) {
			GEODATA gd;
			memset(&gd, 0, sizeof(gd));

			int fh = open(path, O_RDONLY | O_BINARY); 
			assert(fh != -1);
			read(fh, &gd, sizeof(gd));
			close(fh);

			if (Map::valid_GEODATA(&gd)) {
				memcpy(&mapdata, &gd, sizeof(mapdata));
				net->send_map_data(&mapdata);
				mapdata.load_game = 77;
			} else {
				info->printstr("invalid GEODATA file.\n");
			}
		}
	}
}

void Units::execute_right()
{
	if (selected != -1) {
		deselect();
	} else {
		int x1, y1, x2, y2;
		x1 = gx - 2; x2 = gx + 20 * 8 + 2;

		for (int i = 0; i < size; i++) {
			y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;

			if (mouse_inside(x1, y1, x2, y2)) {
				if (x[i] != 0) {
					x[i] = 0; y[i] = 0;
					net->send_deselect_unit(i);
				}
				return ;
			}
		}
	}
}

void Units::deselect()
{
	selected = -1;
	restore_mouse_range();
}

void Units::set_mouse_range(int mx, int my, int mx1, int my1, int mx2, int my2)
{
	this->mx = mx;
	this->my = my;
	this->mx1 = mx1;
	this->my1 = my1;
	this->mx2 = mx2;
	this->my2 = my2;
}

void Units::limit_mouse_range()
{
	::set_mouse_range(mx1, my1, mx2, my2);
	if (x[selected] != 0)
		position_mouse(x[selected], y[selected]);
	else {
		if (pos == POS_LEFT)
			position_mouse(mx1 + 20, mouse_y);
		else
			position_mouse(mx2 - 20, mouse_y);
	}
}

void Units::restore_mouse_range()
{
	::set_mouse_range(0, 0, mx, my);
}

void Units::draw_lines(int gcol)
{
	for (int i = 0; i < size; i++) {
		if (selected == i) {
			line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, 1);
		} else
			if (x[i] != 0) {
				line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
			}
	}
}
