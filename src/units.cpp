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
#include "scenario.h"

#define CAPTION			8
#define COMMENT			3
#define BUTTON			5
#define SELECTED		52
#define SWITCH_ON		55
#define SWITCH_OFF		40
#define LOCAL_COLOR		48
#define REMOTE_COLOR	32

char buf[10000];
int len = 0;

int points = 0;
int damage_points = 0;

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
	memset(buf, 0, sizeof(buf));
	SEND = 0; START = 0;
	state = PS_MAIN;
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
	int x1, y1, x2, y2, color = xcom1_color(60);
	int i;
	for (i = 0; i < size; i++) {
		if (selected == i) {
			if (pos == POS_LEFT)
				line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, xcom1_color(1));
			else
				line(screen2, gx - 4, gy + i * 15 + 3, mouse_x, mouse_y, xcom1_color(1));
			color = xcom1_color(120);
		} else
			if (x[i] != 0) {
				rectfill(screen2, gmx + col[i] * 4 + 1, gmy + row[i] * 4 + 1,
				         gmx + col[i] * 4 + 3, gmy + row[i] * 4 + 3,
				         xcom1_color(144));      //yellow
				if (pos == POS_LEFT) {
					line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
					//textprintf(screen2, font, gx - 60, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
				} else {
					line(screen2, gx - 4, gy + i * 15 + 3, x[i], y[i], gcol);
					//textprintf(screen2, font, gx + 20 * 8 + 5, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
				}
				color = xcom1_color(53);
			} else {
				color = xcom1_color(60);
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
	
	rect(screen2, gmx, SCREEN2H - 71, gmx + gmw, SCREEN2H - 5, xcom1_color(1));
	textout_centre(screen2, font, "MATCH SETTINGS", gmx + gmw / 2, SCREEN2H - 65, xcom1_color(CAPTION));
	textout_centre(screen2, font, scenario->name[scenario->type], gmx + gmw / 2, SCREEN2H - 53, xcom1_color(BUTTON));
	textout_centre(screen2, font, terrain_set->get_terrain_name(mapdata.terrain).c_str(), gmx + gmw / 2, SCREEN2H - 41, xcom1_color(BUTTON));
	
	int tmp;
	g_time_limit == -1 ? tmp = 0 : tmp = g_time_limit;                      
	textout_centre(screen2, font, "Game rules:", gmx + gmw / 2, SCREEN2H - 28, xcom1_color(BUTTON));
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 16, xcom1_color(BUTTON), "%d; %dk; %d; %d; %d; %d", scenario->rules[0], scenario->rules[1], scenario->rules[2], tmp, scenario->rules[3], scenario->rules[4]);
	
	for (int i = 0; i < len; i++) buf[i] = 0;
	len = 0;
 	points = 0;
 	damage_points = 0;

	for (int n = 0; n < size; n++) {
		if (x[n] != 0) {
			Soldier * ss = editor->platoon()->findman(name[n]);
			ASSERT(ss != NULL);
			points += ss->calc_ammunition_cost();
		}
	}
	//textout(screen2, small, "INFO", gx, 160, 1);
	// Moved down...
	// textprintf_centre(screen2, g_small_font, gx + 10 * 8, 160, xcom1_color(50), "total men points=%d", points);

	for (i = 0; i < size; i++) {
		if (x[i] == 0)
			continue;
		Soldier *ss = editor->platoon()->findman(name[i]);
		ASSERT(ss != NULL);
		if (ss != NULL) {
			//ss->build_items_stats(buf, len);
			build_items_stats(&ss->id, buf, len);
		}
	}

	/*if (pos == POS_LEFT)
		draw_items_stats(0, 160+10, buf, len);
	else*/
	damage_points = draw_items_stats(gx, 160 + 10, buf, len);

	textprintf_centre(screen2, g_small_font, gx + 10 * 8, 160, xcom1_color(50), "Total points=%d", points + damage_points);
		
	scenario->draw_deploy_zone(pos, gmx, 0, xcom1_color(LOCAL_COLOR));
	
	switch(state) {
		case PS_SCEN:
		draw_scenario_window();
		break;
		
		case PS_MAP:
		draw_map_window();
		break;
		
		case PS_RULES:
		draw_rules_window();
		break;
		
		case PS_RULES_0:
		draw_rules_window();
		draw_rules_0_window();
		break;
		
		case PS_RULES_1:
		draw_rules_window();
		draw_rules_1_window();
		break;
		
		case PS_RULES_2:
		draw_rules_window();
		draw_rules_2_window();
		break;
		
		case PS_RULES_3:
		draw_rules_window();
		draw_rules_3_window();
		break;
		
		case PS_MAIN:
		break;
	}
}

void Units::draw_scenario_window()
{
    rect(screen2, gmx + gmw / 2 - 200, SCREEN2H - 320, gmx + gmw / 2 + 200, SCREEN2H - 37, xcom1_color(1));
    rectfill(screen2, gmx + gmw / 2 - 200 + 1, SCREEN2H - 320 + 1, gmx + gmw / 2 + 200 - 1, SCREEN2H - 37 - 1, xcom1_color(14));
    
    textout_centre(screen2, font, scenario->name[scenario->type], gmx + gmw / 2, SCREEN2H - 310, xcom1_color(SELECTED));
    textout_centre(screen2, font, "<", gmx + gmw / 2 - 150 + 50, SCREEN2H - 310, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", gmx + gmw / 2 + 150 - 50, SCREEN2H - 310, xcom1_color(BUTTON));
    
    textout_centre(screen2, font, "Player 1", gmx + gmw / 2, SCREEN2H - 285, xcom1_color(CAPTION));
	for (int i = 0; i < 8; i++)
    	textprintf(screen2, font, gmx + gmw / 2 - 200 + 5, SCREEN2H - 270 + i * 9, xcom1_color(COMMENT), scenario->briefing_left[scenario->type][i]);
	if (pos == POS_LEFT)
		rect(screen2, gmx + gmw / 2 - 200 + 3, SCREEN2H - 272, gmx + gmw / 2 + 200 - 3, SCREEN2H - 198, xcom1_color(LOCAL_COLOR));    	
    	
    textout_centre(screen2, font, "Player 2", gmx + gmw / 2, SCREEN2H - 185, xcom1_color(CAPTION));
	for (int i = 0; i < 8; i++)
    	textprintf(screen2, font, gmx + gmw / 2 - 200 + 5, SCREEN2H - 170 + i * 9, xcom1_color(COMMENT), scenario->briefing_right[scenario->type][i]);
   	if (pos == POS_RIGHT)
		rect(screen2, gmx + gmw / 2 - 200 + 3, SCREEN2H - 172, gmx + gmw / 2 + 200 - 3, SCREEN2H - 98, xcom1_color(LOCAL_COLOR));    	

    	
	textout_centre(screen2, font, "Options", gmx + gmw / 2, SCREEN2H - 85, xcom1_color(CAPTION));
	for (int i = 0; i < 3; i++) {
		switch(scenario->options[scenario->type][i]->type) {
			case OPT_NONE:
			case OPT_HIDDEN:
			break;
			
			case OPT_NUMBER:
			textprintf(screen2, font, gmx + gmw / 2 - 200 + 5, SCREEN2H - 70 + i * 9, xcom1_color(COMMENT), scenario->options[scenario->type][i]->caption);
			textout_centre(screen2, font, "<", gmx + gmw / 2 + 150, SCREEN2H - 70 + i * 9, xcom1_color(BUTTON));
			textprintf_centre(screen2, font, gmx + gmw / 2 + 170, SCREEN2H - 70 + i * 9, xcom1_color(SELECTED), "%d", scenario->options[scenario->type][i]->value);
			textout_centre(screen2, font, ">", gmx + gmw / 2 + 190, SCREEN2H - 70 + i * 9, xcom1_color(BUTTON));
			break;
			
			case OPT_SWITCH:
			textprintf(screen2, font, gmx + gmw / 2 - 200 + 5, SCREEN2H - 70 + i * 9, scenario->options[scenario->type][i]->value ? xcom1_color(SWITCH_ON) : xcom1_color(SWITCH_OFF), scenario->options[scenario->type][i]->value ? scenario->options[scenario->type][i]->caption_on : scenario->options[scenario->type][i]->caption_off);
			break;
		}
	}
}

void Units::draw_map_window()
{
	rect(screen2, gmx + gmw / 2 - 80, SCREEN2H - 79, gmx + gmw / 2 + 80, SCREEN2H - 37, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 80 + 1, SCREEN2H - 79 + 1, gmx + gmw / 2 + 80 - 1, SCREEN2H - 37 - 1, xcom1_color(14));

	textout_centre(screen2, font, terrain_set->get_terrain_name(mapdata.terrain).c_str(), gmx + gmw / 2, SCREEN2H - 73, xcom1_color(BUTTON));

	textout_centre(screen2, font, "4*4", gmx + gmw / 2 - 40, SCREEN2H - 61, xcom1_color(mapdata.x_size == 4 ? SWITCH_ON : SWITCH_OFF));
	textout_centre(screen2, font, "5*5", gmx + gmw / 2, SCREEN2H - 61, xcom1_color(mapdata.x_size == 5 ? SWITCH_ON : SWITCH_OFF));
	textout_centre(screen2, font, "6*6", gmx + gmw / 2 + 40, SCREEN2H - 61, xcom1_color(mapdata.x_size == 6 ? SWITCH_ON : SWITCH_OFF));
	
	textout_centre(screen2, font, "NEW", gmx + gmw / 2 - 40, SCREEN2H - 49, xcom1_color(BUTTON));
	textout_centre(screen2, font, "LOAD", gmx + gmw / 2 + 40, SCREEN2H - 49, xcom1_color(BUTTON));
}

void Units::draw_rules_window()
{
	rect(screen2, gmx + gmw / 2 - 80, SCREEN2H - 103, gmx + gmw / 2 + 80, SCREEN2H - 25, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 80 + 1, SCREEN2H - 103 + 1, gmx + gmw / 2 + 80 - 1, SCREEN2H - 25 - 1, xcom1_color(14));
	
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 97, xcom1_color(BUTTON), "Explosives level: %d", scenario->rules[0]);
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 85, xcom1_color(BUTTON), "Points limit: %d000", scenario->rules[1]);
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 73, xcom1_color(BUTTON), scenario->rules[2] == 0 ? "No turns limit" : "Turns limit: %d", scenario->rules[2]);
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 61, xcom1_color(BUTTON), g_time_limit == -1 ? "No time limit" : "Time limit: %d sec", g_time_limit);
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 49, xcom1_color(BUTTON), scenario->rules[3] ? "All map is explored" : "Map isn't explored");
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 37, xcom1_color(BUTTON), scenario->rules[4] ? "Editor: ground on" : "Editor: ground off");
}

void Units::draw_rules_0_window()
{
	rect(screen2, gmx + gmw / 2 - 130, SCREEN2H - 139, gmx + gmw / 2 + 130, SCREEN2H - 97, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 130 + 1, SCREEN2H - 139 + 1, gmx + gmw / 2 + 130 - 1, SCREEN2H - 97 - 1, xcom1_color(13));
	
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 133, xcom1_color(SELECTED), "%d", scenario->rules[0]);
	textout_centre(screen2, font, "<", gmx + gmw / 2 - 20, SCREEN2H - 133, xcom1_color(BUTTON));
	textout_centre(screen2, font, ">", gmx + gmw / 2 + 20, SCREEN2H - 133, xcom1_color(BUTTON));
	
	switch(scenario->rules[0]) {
	    case 0:
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 121, xcom1_color(COMMENT), "All explosives are prohibited.");
	    break;
	    
     	case 1:
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 121, xcom1_color(COMMENT), "High explosives, all rockets and");
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 109, xcom1_color(COMMENT), "alien grenades are prohibited.");
	    break;
	    
	    case 2:
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 121, xcom1_color(COMMENT), "Large rockets and alien grenades");
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 109, xcom1_color(COMMENT), "are prohibited.");
	    break;
	    
		case 3:
	    textprintf(screen2, font, gmx + gmw / 2 - 130 + 3, SCREEN2H - 121, xcom1_color(COMMENT), "All explosives are allowed.");
	    break;
	}
}

void Units::draw_rules_1_window()
{
	rect(screen2, gmx + gmw / 2 - 40, SCREEN2H - 99, gmx + gmw / 2 + 40, SCREEN2H - 85, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 40 + 1, SCREEN2H - 99 + 1, gmx + gmw / 2 + 40 - 1, SCREEN2H - 85 - 1, xcom1_color(13));
	
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 95, xcom1_color(SELECTED), "%d000", scenario->rules[1]);
	textout_centre(screen2, font, "<", gmx + gmw / 2 - 30, SCREEN2H - 95, xcom1_color(BUTTON));
	textout_centre(screen2, font, ">", gmx + gmw / 2 + 30, SCREEN2H - 95, xcom1_color(BUTTON));
}

void Units::draw_rules_2_window()
{
	rect(screen2, gmx + gmw / 2 - 30, SCREEN2H - 87, gmx + gmw / 2 + 30, SCREEN2H - 73, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 30 + 1, SCREEN2H - 87 + 1, gmx + gmw / 2 + 30 - 1, SCREEN2H - 73 - 1, xcom1_color(13));

	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 83, xcom1_color(SELECTED), scenario->rules[2] == 0 ? "no" : "%d", scenario->rules[2]);
	textout_centre(screen2, font, "<", gmx + gmw / 2 - 20, SCREEN2H - 83, xcom1_color(BUTTON));
	textout_centre(screen2, font, ">", gmx + gmw / 2 + 20, SCREEN2H - 83, xcom1_color(BUTTON));
}

void Units::draw_rules_3_window()
{
	rect(screen2, gmx + gmw / 2 - 30, SCREEN2H - 75, gmx + gmw / 2 + 30, SCREEN2H - 61, xcom1_color(1));
	rectfill(screen2, gmx + gmw / 2 - 30 + 1, SCREEN2H - 75 + 1, gmx + gmw / 2 + 30 - 1, SCREEN2H - 61 - 1, xcom1_color(13));
	
	textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 71, xcom1_color(SELECTED), g_time_limit == -1 ? "no" : "%d", g_time_limit);
	textout_centre(screen2, font, "<", gmx + gmw / 2 - 20, SCREEN2H - 71, xcom1_color(BUTTON));
	textout_centre(screen2, font, ">", gmx + gmw / 2 + 20, SCREEN2H - 71, xcom1_color(BUTTON));
}

void Units::build_items_stats(ITEMDATA *id, char *buf, int &len)
{
	for (int i = 0; i < id->num; i++) {
		buf[len++] = id->type[i];
	}
}

extern int weapon[];

int Units::draw_items_stats(int gx, int gy, char *buf, int len)
{
	int aa = 0;
	int damage_points = 0;
	for (int w = 0; w < 40; w++) {
		int num = 0;
		for (int i = 0; i < len; i++) {
			if (weapon[w] == buf[i])
				num++;
		}
		// Count damage for platoon costs.
		if ((Item::obdata_cost(weapon[w]) > 0) && (num > 0))
			damage_points += (Item::obdata_cost(weapon[w]) * num);
		if (Item::obdata_isAmmo(weapon[w])) continue;
		if (num != 0) {
			textprintf(screen2, g_small_font, gx + (aa / 72) * 90, gy + (aa % 72),
			           xcom1_color(1), "%s=%d", Item::obdata_name(weapon[w]).c_str(), num);
			aa += 9;
			
		}
	}

	return damage_points;
}


void Units::print_simple(int gcol)
{
	text_mode( -1);	
	int i, x1, y1, x2, y2, color = xcom1_color(60);
	for (i = 0; i < size; i++) {
		if (x[i] != 0) {
			if (FLAGS & F_PLANNERDBG) { // show lines of remote
				if (pos == POS_LEFT) {
					line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
				} else {
					line(screen2, gx - 4, gy + i * 15 + 3, x[i], y[i], gcol);
				}
			}
			color = xcom1_color(53);
		} else {
			color = xcom1_color(60);
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
	
	scenario->draw_deploy_zone(pos, gmx, 0, xcom1_color(REMOTE_COLOR));

	if (!SEND)
		return ;

	int points = 0;
	for (i = 0; i < pd_remote->size; i++) {
/*		points += pd_remote->md[i].TimeUnits +
		          pd_remote->md[i].Health +
		          pd_remote->md[i].Firing +
		          pd_remote->md[i].Throwing; */
		points += (((pd_remote->md[i].TimeUnits * 2) + pd_remote->md[i].Stamina) / 2) +
	        pd_remote->md[i].Health +
			(pd_remote->md[i].Reactions * 2) +
	        ((pd_remote->md[i].Firing + pd_remote->md[i].Throwing) / 2) +
			(pd_remote->md[i].Strength * 2) +
			g_skins[get_skin_index(pd_remote->md[i].SkinType, pd_remote->md[i].fFemale)].armour_values[0];
	}

	char buf[10000]; memset(buf, 0, sizeof(buf));
	int len = 0;

	for (i = 0; i < pd_remote->size; i++) {
		build_items_stats(&pd_remote->id[i], buf, len);
	}

	/*if (pos == POS_LEFT)
		draw_items_stats(0, 160+10, buf, len);
	else*/
	points += draw_items_stats(gx, 160 + 10, buf, len);

	textprintf_centre(screen2, g_small_font, gx + 10 * 8, 160, xcom1_color(50), "Total points=%d", points);
}

void Units::draw_text()
{
	text_mode( -1);

	//rect(screen2, gx+50, SCREEN2H-35, gx+20*8-50, SCREEN2H-5, 1);
	//rect(screen2, gx+40, SCREEN2H-35, gx+20*8-40, SCREEN2H-5, 1);
	rect(screen2, gx, SCREEN2H - 35, gx + 20 * 8, SCREEN2H - 5, xcom1_color(1));

	if (pos == POS_LEFT) {
		//textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "SERVER");
		textout_centre(screen2, font, "SERVER", gx + 10 * 8, SCREEN2H - 29, xcom1_color(CAPTION));
	} else {
		//textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "CLIENT");
		textout_centre(screen2, font, "CLIENT", gx + 10 * 8, SCREEN2H - 29, xcom1_color(CAPTION));
	}

	int color = SWITCH_OFF;      //red
	if (SEND)
		color = SWITCH_ON;      //green

	textout_centre(screen2, font, "SEND", gx + 15 * 8, SCREEN2H - 16, xcom1_color(color));

	if (START)
		color = SWITCH_ON;      //green
	else
		color = SWITCH_OFF;      //red

	textout_centre(screen2, font, "START", gx + 5 * 8, SCREEN2H - 16, xcom1_color(color));
}

void Units::execute(Map *map, int map_change_allowed)
{
	switch (state) {
		case PS_MAIN:
		execute_main(map, map_change_allowed);
		break;
		case PS_SCEN:
		execute_scenario(map, map_change_allowed);
		break;
		case PS_MAP:
		execute_map(map, map_change_allowed);
		break;
		case PS_RULES:
		execute_rules(map, map_change_allowed);
		break;
		case PS_RULES_0:
		execute_rules_0(map, map_change_allowed);
		break;
		case PS_RULES_1:
		execute_rules_1(map, map_change_allowed);
		break;
		case PS_RULES_2:
		execute_rules_2(map, map_change_allowed);
		break;
		case PS_RULES_3:
		execute_rules_3(map, map_change_allowed);
		break;
	}
}

void Units::execute_main(Map *map, int map_change_allowed)
{
	if (selected != -1) {
		int mx = mouse_x;
		int my = mouse_y;
		int c = (mx - gmx) / 4;
		int r = (my - gmy) / 4;

		if (!(FLAGS & F_PLANNERDBG)) { // allow place at any position
			/*if (pos == POS_LEFT) {
				if (c > 9)
					return ;
			} else {
				if (c < gmw / 4 - 10)
					return ;
			}*/
			if (!scenario->is_correct_place(pos, c, r))
			    return;
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
		int index_of_first = -1;
		for (int i = 0; i < 10; i++) {
			if (x[i] != 0 && y[i] != 0) {
			    index_of_first = i;
				break;
			}
		}
		
		int num_of_men_sel = 0;
		for (int i = 0; i < 10; i++) {
			if (x[i] != 0 && y[i] != 0)
				num_of_men_sel++;
		}
		
		if (scenario->is_correct_platoon(points + damage_points, editor->platoon(), name[index_of_first], pos, buf, len, num_of_men_sel))
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

	//if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 60, SCREEN2H - 57, gmx + gmw / 2 + 60, SCREEN2H - 44)) {
		//SCENARIO
		state = PS_SCEN;
	}

	if (mouse_inside(gmx + gmw / 2 - 60, SCREEN2H - 43, gmx + gmw / 2 + 60, SCREEN2H - 30)) {
		//MAP
		state = PS_MAP;
	}

	if (mouse_inside(gmx + gmw / 2 - 60, SCREEN2H - 29, gmx + gmw / 2 + 60, SCREEN2H - 3)) {
	    //RULES
	    state = PS_RULES;
	}
}

void Units::execute_scenario(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 200, SCREEN2H - 320, gmx + gmw / 2 + 200, SCREEN2H - 37))
		state = PS_MAIN;

    if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 105, SCREEN2H - 314, gmx + gmw / 2 - 95, SCREEN2H - 301)) {
	    //"<"
	    scenario->new_scenario(scenario->type - 1);

	    //don't know why this check doesn't work from scenario::new_coords()
  		if (scenario->type == SC_SABOTAGE) {
			while (!(map->passable(0, scenario->x1, scenario->y1)) || !(map->passable(0, scenario->x2, scenario->y2)))
		    	scenario->new_coords();
		}

		net->send_scenario();
		mapdata.load_game = 77;
	}

	if (mouse_inside(gmx + gmw / 2 + 95, SCREEN2H - 314, gmx + gmw / 2 + 105, SCREEN2H - 301)) {
	    //">"
	    scenario->new_scenario(scenario->type + 1);

	    //don't know why this check doesn't work from scenario::new_coords()
  		if (scenario->type == SC_SABOTAGE) {
			while (!(map->passable(0, scenario->x1, scenario->y1)) || !(map->passable(0, scenario->x2, scenario->y2)))
		    	scenario->new_coords();
		}

		net->send_scenario();
		mapdata.load_game = 77;
	}
	
	for (int i = 0; i < 3; i++) {
		if (mouse_inside(gmx + gmw / 2 - 200 + 2, SCREEN2H - 71 + i * 10, gmx + gmw / 2 + 200 - 2, SCREEN2H - 62 + i * 10)) {
			//options
			switch(scenario->options[scenario->type][i]->type) {                                                    
				case OPT_NONE:                               
				case OPT_HIDDEN:                                                       
				return;                                                                                             
				break;
				
				case OPT_NUMBER:
				if (mouse_inside(gmx + gmw / 2 + 145, SCREEN2H - 71 + i * 10, gmx + gmw / 2 + 155, SCREEN2H - 62 + i * 10)) {
				    //"<"
				    scenario->options[scenario->type][i]->value -= scenario->options[scenario->type][i]->step;

		  			if (scenario->options[scenario->type][i]->value < scenario->options[scenario->type][i]->min) {
  			 			scenario->options[scenario->type][i]->value = scenario->options[scenario->type][i]->min;
  		 				return;
  		 			}
				}
				if (mouse_inside(gmx + gmw / 2 + 185, SCREEN2H - 71 + i * 10, gmx + gmw / 2 + 195, SCREEN2H - 61 + i * 10)) {
				    //">"
				    scenario->options[scenario->type][i]->value += scenario->options[scenario->type][i]->step;
   	
		  			if (scenario->options[scenario->type][i]->value > scenario->options[scenario->type][i]->max) {
  			 			scenario->options[scenario->type][i]->value = scenario->options[scenario->type][i]->max;
  		 				return;
					}
				}
				break;
			
				case OPT_SWITCH:
				if (scenario->options[scenario->type][i]->value)
					scenario->options[scenario->type][i]->value = 0;
				else
					scenario->options[scenario->type][i]->value = 1;
				break;
			}
		
			net->send_options(scenario->type, i, scenario->options[scenario->type][i]->value);
			if (scenario->options[scenario->type][i]->reset_deploy)
				mapdata.load_game = 77;
		}
	}
}

void Units::execute_map(Map *map, int map_change_allowed)
{
        if (!mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 79, gmx + gmw / 2 + 80, SCREEN2H - 37))
		state = PS_MAIN;

    if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 77, gmx + gmw / 2 + 75, SCREEN2H - 64)) {
	    //MAP TYPE
     	std::string terrain_name = terrain_set->get_random_terrain_name();

		if (net->is_network_game()) {
	   		ASSERT(g_net_allowed_terrains.size() > 0);
			while (g_net_allowed_terrains.find(terrain_name) == g_net_allowed_terrains.end()) {
				terrain_name = terrain_set->get_random_terrain_name();
			}
		}

		Map::new_GEODATA(&mapdata, terrain_name);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
		scenario->new_coords();
	}

	if (mouse_inside(gmx + gmw / 2 - 59, SCREEN2H - 63, gmx + gmw / 2 + 20, SCREEN2H - 50)) {
		//"4*4"
  		std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
		MAP_WIDTH = MAP_HEIGHT = 4;
		Map::new_GEODATA(&mapdata, terrain_name);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
		scenario->new_coords();
	}

	if (mouse_inside(gmx + gmw / 2 - 19, SCREEN2H - 63, gmx + gmw / 2 + 20, SCREEN2H - 50)) {
		//"5*5"
  		std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
		MAP_WIDTH = MAP_HEIGHT = 5;
		Map::new_GEODATA(&mapdata, terrain_name);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
		scenario->new_coords();
	}

	if (mouse_inside(gmx + gmw / 2 + 21, SCREEN2H - 63, gmx + gmw / 2 + 60, SCREEN2H - 50)) {
		//"6*6"
  		std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
		MAP_WIDTH = MAP_HEIGHT = 6;
		Map::new_GEODATA(&mapdata, terrain_name);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
		scenario->new_coords();
	}

	if (mouse_inside(gmx + gmw / 2 - 60, SCREEN2H - 49, gmx + gmw / 2 - 20, SCREEN2H - 36)) {
		//"NEW"
  		std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
		Map::new_GEODATA(&mapdata, terrain_name);
		net->send_map_data(&mapdata);
		mapdata.load_game = 77;
		scenario->new_coords();
	}

	if (mouse_inside(gmx + gmw / 2 + 20, SCREEN2H - 49, gmx + gmw / 2 + 60, SCREEN2H - 36)) {
		//"LOAD"
		char path[1000]; *path = 0;
		if (file_select("GEODATA file", path, "lua")) {
			GEODATA gd;

			if (!Map::load_GEODATA(path, &gd) || !Map::valid_GEODATA(&gd)) {
				g_console->printf("%s", "invalid map file.");
			} else {
				memcpy(&mapdata, &gd, sizeof(mapdata));
				net->send_map_data(&mapdata);
				mapdata.load_game = 77;
				net->send_scenario();
			}
		}
	}
}

void Units::execute_rules(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 103, gmx + gmw / 2 + 80, SCREEN2H - 25))
		state = PS_MAIN;

    if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 101, gmx + gmw / 2 + 75, SCREEN2H - 89))
        state = PS_RULES_0;

    if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 88, gmx + gmw / 2 + 75, SCREEN2H - 76))
        state = PS_RULES_1;

    if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 75, gmx + gmw / 2 + 75, SCREEN2H - 63))
        state = PS_RULES_2;
        
    if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 62, gmx + gmw / 2 + 75, SCREEN2H - 50))
        state = PS_RULES_3;
        
	if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 49, gmx + gmw / 2 + 75, SCREEN2H - 37)) {
	    if (scenario->rules[3] == 0)
	        scenario->rules[3] = 1;
		else
		    scenario->rules[3] = 0;
		net->send_rules(3, scenario->rules[3]);
	}
	
	if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 36, gmx + gmw / 2 + 75, SCREEN2H - 25)) {
		if (scenario->rules[4] == 0)
			scenario->rules[4] = 1;
		else
			scenario->rules[4] = 0;
		net->send_rules(4, scenario->rules[4]);
	}
}

void Units::execute_rules_0(Map *map, int map_change_allowed)
{
	if (!mouse_inside(gmx + gmw / 2 - 110, SCREEN2H - 139, gmx + gmw / 2 + 90, SCREEN2H - 97))
	    state = PS_RULES;

    if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 25, SCREEN2H - 137, gmx + gmw / 2 - 15, SCREEN2H - 124)) {
	    //"<"
	    scenario->rules[0]--;

  		if (scenario->rules[0] < 0) {
   			scenario->rules[0] = 0;
   			return;
		}

		net->send_rules(0, scenario->rules[0]);
	}

    if (mouse_inside(gmx + gmw / 2 + 15, SCREEN2H - 137, gmx + gmw / 2 + 25, SCREEN2H - 124)) {
	    //">"
	    scenario->rules[0]++;

  		if (scenario->rules[0] > 3) {
   			scenario->rules[0] = 3;
   			return;
		}

		net->send_rules(0, scenario->rules[0]);
	}
}

void Units::execute_rules_1(Map *map, int map_change_allowed)
{
	if (!mouse_inside(gmx + gmw / 2 - 40, SCREEN2H - 99, gmx + gmw / 2 + 40, SCREEN2H - 85))
	    state = PS_RULES;

	if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 35, SCREEN2H - 97, gmx + gmw / 2 - 25, SCREEN2H - 84)) {
	    //"<"
	    scenario->rules[1]--;

	    if (scenario->rules[1] < 2) {
			scenario->rules[1] = 2;
			return;
		}

		net->send_rules(1, scenario->rules[1]);
	}

	if (mouse_inside(gmx + gmw / 2 + 25, SCREEN2H - 97, gmx + gmw / 2 + 35, SCREEN2H - 84)) {
	    //">"
	    scenario->rules[1]++;

	    if (scenario->rules[1] > 15) {
			scenario->rules[1] = 15;
			return;
		}

		net->send_rules(1, scenario->rules[1]);
	}
}

void Units::execute_rules_2(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 30, SCREEN2H - 87, gmx + gmw / 2 + 30, SCREEN2H - 73))
	    state = PS_RULES;

	if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 25, SCREEN2H - 85, gmx + gmw / 2 - 15, SCREEN2H - 72)) {
	    //"<"
	    scenario->rules[2]--;

	    if (scenario->rules[2] < 0) {
			scenario->rules[2] = 0;
			return;
		}

		net->send_rules(2, scenario->rules[2]);
	}

	if (mouse_inside(gmx + gmw / 2 + 15, SCREEN2H - 85, gmx + gmw / 2 + 25, SCREEN2H - 72)) {
	    //">"
	    scenario->rules[2]++;

	    if (scenario->rules[2] > 20) {
			scenario->rules[2] = 20;
			return;
		}

		net->send_rules(2, scenario->rules[2]);
	}
}

void Units::execute_rules_3(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 30, SCREEN2H - 75, gmx + gmw / 2 + 30, SCREEN2H - 61))
	    state = PS_RULES;

	if (!map_change_allowed) return;

	if (mouse_inside(gmx + gmw / 2 - 25, SCREEN2H - 73, gmx + gmw / 2 - 15, SCREEN2H - 60)) {
	    //"<"
     	g_time_limit -= 15;

	    if (g_time_limit == 0)
			g_time_limit = -1;
			
		if (g_time_limit < -1) {
		    g_time_limit = -1;
			return;
		}

		net->send_time_limit(g_time_limit);
	}

	if (mouse_inside(gmx + gmw / 2 + 15, SCREEN2H - 73, gmx + gmw / 2 + 25, SCREEN2H - 60)) {
	    //">"
	    if (g_time_limit != -1)
	    	g_time_limit += 15;
		else
		    g_time_limit += 16;

	    if (g_time_limit > 600) {
			g_time_limit = 600;
			return;
		}

		net->send_time_limit(g_time_limit);
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
			line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, xcom1_color(1));
		} else
			if (x[i] != 0) {
				line(screen2, gx + 20 * 8 + 4, gy + i * 15 + 3, x[i], y[i], gcol);
			}
	}
}
