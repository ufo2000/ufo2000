/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2004  ufo2000 development team

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
#include "multiplay.h"
#include "units.h"
#include "editor.h"
#include "map.h"
#include "wind.h"
#include "scenario.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"

#define CAPTION          8
#define COMMENT          3
#define BUTTON           5
#define SELECTED        52
#define SWITCH_ON       55
#define SWITCH_OFF      40
#define LOCAL_COLOR     48
#define REMOTE_COLOR    32
// From colors.h :
//  8: COLOR_GRAY08
//  3: COLOR_GRAY03
//  5: COLOR_GRAY05
// 52: COLOR_GREEN04
// 55: COLOR_GREEN07
// 40: COLOR_RED08
// 48: COLOR_GREEN00
// 32: COLOR_RED00

static uint32 buf32[10000];
static int buf32_len = 0;

int points = 0;        //!< Total points: soldier, attributes, armor + items
int damage_points = 0; //!< Point-value from items only (weapons and equipment)

Units::Units()
{
    Position = 1; // has to be deleted later
    reset();
    temp_mouse_range = NULL;
    packet_num = 0;
}

Units::~Units()
{
    restore_mouse_range();
}

/**
 * Initialize data for units
 */
void Units::reset()
{
    size = 0;
    memset(cost, 0, sizeof(cost));
    // memset(buf32,  0, sizeof(buf32));
    buf32_len = 0;
    state = PS_MAIN;
    reset_selections();
}

void Units::reset_selections()
{
    int i;
    for (i = 0; i < SQUAD_LIMIT; i++)
        lev[i] = -1;
    memset(col, 0, sizeof(col));
    memset(row, 0, sizeof(row));
    SEND = 0; START = 0;
    selected = -1;
}

void Units::set_pos(PanPos pos, int gx, int gy, int gmx, int gmw, int gmy, int gmh)
{
    this->pos = pos;
    this->gx  = gx;
    this->gy  = gy;
    this->gmx = gmx;
    this->gmw = gmw;
    this->gmy = gmy;
    this->gmh = gmh;
}

/**
 * Select unit (for going on the mission) by putting it onto the map
 */
int Units::select_unit(int num, int n_lev, int n_col, int n_row)
{
    if (num < 0 || num >= size)
        return 0;
    lev[num] = n_lev;
    col[num] = n_col;
    row[num] = n_row;
    SEND = 0; START = 0;
    return 1;
}

/**
 * Deselect unit (for going on the mission) by removing it from the map
 */
int Units::deselect_unit(int num)
{
    if (num < 0 || num >= size)
        return 0;
    lev[num] = -1;
    col[num] = 0;
    row[num] = 0;
    SEND = 0; START = 0;
    return 1;
}


int Units::add(int num, const char *nm, int ct)
{
    if (num < 0 || num >= SQUAD_LIMIT) {
        ASSERT(false);
        return 0;
    }
    if (ustrlen(nm) > MAN_NAME_LEN) {
        ASSERT(false);
        return 0;
    }
    size = num;
    strcpy(name[size], nm);
    cost[size] = ct;
    size++;
    SEND = 0; START = 0;

    return 1;
}


/**
 * Print list of soldiers to select from,
 * and box for selecting map and scenario.
 */
void Units::print(int gcol)
{
    text_mode( -1);
    int x0;
    int x1, y1, x2, y2, color = COLOR_GREEN12;
    int i;
    // Todo: function to calculate position of name-fields,
    //  to allow to pack the names a bit closer together
    for (i = 0; i < size; i++) {
        if (selected == i) {
            if (pos == POS_LEFT)
                line(screen2, gx + MAN_NAME_LEN * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, COLOR_WHITE);
            else
                line(screen2, gx - 4, gy + i * 15 + 3, mouse_x, mouse_y, COLOR_WHITE);
            color = COLOR_DK_BLUE;
        } else
            if (is_selected(i)) {    // soldier selected for mission has a position on the map
                rectfill(screen2, gmx + col[i] * 4 + 1, gmy + row[i] * 4 + 1,
                         gmx + col[i] * 4 + 3, gmy + row[i] * 4 + 3,
                         COLOR_YELLOW);
                if (pos == POS_LEFT) {
                    line(screen2, gx + MAN_NAME_LEN * 8 + 4, gy + i * 15 + 3, minimap_x(i), minimap_y(i), gcol);
                    //textprintf(screen2, font, gx - 60, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
                } else {
                    line(screen2, gx - 4, gy + i * 15 + 3, minimap_x(i), minimap_y(i), gcol);
                    //textprintf(screen2, font, gx + 20 * 8 + 5, gy + i * 15, gcol, "(%d,%d)", col[i], row[i]);
                }
                color = COLOR_GREEN05;   // background for selected man
            } else {
                color = COLOR_GREEN12;   // background for unselected man
            }

        x1 = gx - 2;          x2 = gx + MAN_NAME_LEN * 8 + 2;
        y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;

        // Highlight the soldiers who are equipped with forbidden items in in red color
        Soldier *ss = editor->platoon()->findman(name[i]);
        ASSERT(ss != NULL);
        if (ss->has_forbidden_equipment()) {
            color = COLOR_RED12;
        }

        rectfill(screen2, x1, y1, x2, y2, color);   // Background for name-field

        //textprintf(screen2, font, gx, gy+i*15, gcol, "%d", cost[i]);
        textprintf(screen2, font, gx, gy + i * 15, gcol, "%s", name[i]);

        /*if (mouse_inside(x1, y1, x2, y2)) {
            textprintf(screen2, font, gx, gy+200, gcol, "%d", cost[i]);
        }*/

    }
    draw_text();

//
// Draw box for selecting map and scenario:
//

// Todo: rewrite to allow easier resizing this window, for longer translated strings
    x0 = gmx -  8;  x1 = gmw + 16;
    rect(screen2, x0, SCREEN2H - 71, x0 + x1, SCREEN2H - 5, COLOR_WHITE);
    textout_centre(screen2, font, _("MATCH SETTINGS"), gmx + gmw / 2, SCREEN2H - 65, xcom1_color(CAPTION));
    textout_centre(screen2, font, scenario->name[scenario->type], gmx + gmw / 2, SCREEN2H - 53, xcom1_color(BUTTON));
    textout_centre(screen2, font, terrain_set->get_terrain_name(mapdata.terrain).c_str(), gmx + gmw / 2, SCREEN2H - 41, xcom1_color(BUTTON));

    int tmp;
    g_time_limit == -1 ? tmp = 0 : tmp = g_time_limit;
    textout_centre(screen2, font, _("Game rules:"), gmx + gmw / 2, SCREEN2H - 28, xcom1_color(BUTTON));
    textprintf_centre(screen2, font, gmx + gmw / 2, SCREEN2H - 16, xcom1_color(BUTTON), "%d; %dk; %d; %d; %d; %d", scenario->rules[0], scenario->rules[1], scenario->rules[2], tmp, scenario->rules[3], scenario->rules[4]);

    points = 0;
    damage_points = 0;

    for (int n = 0; n < size; n++) if (is_selected(n)) {
        Soldier * ss = editor->platoon()->findman(name[n]);
        ASSERT(ss != NULL);
        points += ss->calc_ammunition_cost();
    }

    // !!! Hack - initialize weapons buffer, need to get rid of it in the future
    buf32_len = 0;
    for (i = 0; i < size; i++) if (is_selected(i)) {
        Soldier *ss = editor->platoon()->findman(name[i]);
        ASSERT(ss != NULL);
        if (ss != NULL) {
            build_items_stats(&ss->id, buf32, buf32_len);
        }
    }

    int yy = gy + size * 15 - 3;
    damage_points = draw_items_stats(gx, yy + 10);

    textprintf_centre(screen2, g_small_font, gx + 10 * 8, yy, COLOR_GREEN,
            _("Total points=%d (of %d)"), points + damage_points, total_points());

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

        case PS_RULES_0:       // Explosives
        draw_rules_window();
        draw_rules_0_window();
        break;

        case PS_RULES_1:       // Point-Limit
        draw_rules_window();
        draw_rules_1_window();
        break;

        case PS_RULES_2:       // Turns-Limit
        draw_rules_window();
        draw_rules_2_window();
        break;

        case PS_RULES_3:       // Timelimit
        draw_rules_window();
        draw_rules_3_window();
        break;

        case PS_RULES_4:       // Map-exploration
        draw_rules_window();
        draw_rules_4_window();
        break;

        case PS_MAIN:
        break;
    }
}

void Units::draw_scenario_window()
{
    int i;
// Todo: rewrite to allow easier resizing this window, for longer translated strings,
// or change whole thing to allegro-dialog

  //rect(    screen2, gmx + gmw / 2 - 200,     SCREEN2H - 320,     gmx + gmw / 2 + 200,     SCREEN2H - 37,     COLOR_WHITE);
  //rectfill(screen2, gmx + gmw / 2 - 200 + 1, SCREEN2H - 320 + 1, gmx + gmw / 2 + 200 - 1, SCREEN2H - 37 - 1, COLOR_GRAY14);
    int x0 = gmx + gmw / 2, x1 = 200;
  //int x0 = gmx + gmw / 2, x1 = 200;
    rect(    screen2, x0 - x1,     SCREEN2H - 320,     x0 + x1,     SCREEN2H - 37,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 320 + 1, x0 + x1 - 1, SCREEN2H - 37 - 1, COLOR_GRAY14);

    textout_centre(screen2, font, scenario->name[scenario->type], x0, SCREEN2H - 310, xcom1_color(SELECTED));
    textout_centre(screen2, font, "<", x0 - 150 + 50, SCREEN2H - 310, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 150 - 50, SCREEN2H - 310, xcom1_color(BUTTON));

    textout_centre(screen2, font, _("Player 1"), x0, SCREEN2H - 285, xcom1_color(CAPTION));
    for (i = 0; i < 8; i++)
        textprintf(screen2, font, x0 - x1  + 5, SCREEN2H - 270 + i * 9, xcom1_color(COMMENT), "%s", scenario->briefing_left[scenario->type][i]);
    if (pos == POS_LEFT)
        rect(screen2, x0 - x1  + 3, SCREEN2H - 272, x0 + x1  - 3, SCREEN2H - 198, xcom1_color(LOCAL_COLOR));

    textout_centre(screen2, font, _("Player 2"), x0, SCREEN2H - 185, xcom1_color(CAPTION));
    for (i = 0; i < 8; i++)
        textprintf(screen2, font, x0 - x1  + 5, SCREEN2H - 170 + i * 9, xcom1_color(COMMENT), "%s", scenario->briefing_right[scenario->type][i]);
    if (pos == POS_RIGHT)
        rect(screen2, x0 - x1  + 3, SCREEN2H - 172, x0 + x1  - 3, SCREEN2H - 98, xcom1_color(LOCAL_COLOR));


    textout_centre(screen2, font, _("Options"), x0, SCREEN2H - 85, xcom1_color(CAPTION));
    for (i = 0; i < 3; i++) {
        switch(scenario->options[scenario->type][i]->type) {
            case OPT_NONE:
            case OPT_HIDDEN:
            break;

            case OPT_NUMBER:
                textprintf(screen2, font, x0 - x1  + 5, SCREEN2H - 70 + i * 9, xcom1_color(COMMENT), "%s", scenario->options[scenario->type][i]->caption);
                textout_centre(screen2, font, "<", x0 + 150, SCREEN2H - 70 + i * 9, xcom1_color(BUTTON));
                textprintf_centre(screen2, font, x0 + 170, SCREEN2H - 70 + i * 9, xcom1_color(SELECTED), "%d", scenario->options[scenario->type][i]->value);
                textout_centre(screen2, font, ">", x0 + 190, SCREEN2H - 70 + i * 9, xcom1_color(BUTTON));
            break;

            case OPT_SWITCH:
                textprintf(screen2, font, x0 - x1  + 5, SCREEN2H - 70 + i * 9, scenario->options[scenario->type][i]->value ? xcom1_color(SWITCH_ON) : xcom1_color(SWITCH_OFF), "%s", scenario->options[scenario->type][i]->value ? scenario->options[scenario->type][i]->caption_on : scenario->options[scenario->type][i]->caption_off);
            break;
        }
    }
}

void Units::draw_map_window()
{
// Todo: table of coordinates for all the controls, texts, buttons etc., 
// to make resizing easier, e.g. for adapting to longer translated strings etc.
  //int x0 =  80, x1 = 40;
  //int x0 = 112, x1 = 64;
    int x0 = 120, x1 = 76;

    rect(    screen2, gmx + gmw / 2 - x0,     SCREEN2H - 79,     gmx + gmw / 2 + x0,     SCREEN2H - 37,     COLOR_WHITE);
    rectfill(screen2, gmx + gmw / 2 - x0 + 1, SCREEN2H - 79 + 1, gmx + gmw / 2 + x0 - 1, SCREEN2H - 37 - 1, COLOR_GRAY14);

    textout_centre(screen2, font, terrain_set->get_terrain_name(mapdata.terrain).c_str(), gmx + gmw / 2, SCREEN2H - 73, xcom1_color(BUTTON));

    // see also: execute_map
    textout_centre(screen2, font, "4*4",     gmx + gmw / 2 - x1, SCREEN2H - 61, xcom1_color(mapdata.x_size == 4 ? SWITCH_ON : SWITCH_OFF));
    textout_centre(screen2, font, "5*5",     gmx + gmw / 2,      SCREEN2H - 61, xcom1_color(mapdata.x_size == 5 ? SWITCH_ON : SWITCH_OFF));
    textout_centre(screen2, font, "6*6",     gmx + gmw / 2 + x1, SCREEN2H - 61, xcom1_color(mapdata.x_size == 6 ? SWITCH_ON : SWITCH_OFF));

    textout_centre(screen2, font, _("NEW"),  gmx + gmw / 2 - x1, SCREEN2H - 49, xcom1_color(BUTTON));
    textout_centre(screen2, font, _("LOAD"), gmx + gmw / 2,      SCREEN2H - 49, xcom1_color(BUTTON));
    textout_centre(screen2, font, _("SAVE"), gmx + gmw / 2 + x1, SCREEN2H - 49, xcom1_color(BUTTON));
}

void Units::draw_rules_window()
{
// Todo: rewrite to allow easier resizing this window, for longer translated strings
    int x0 = gmx + gmw / 2;
  //int x1 =  85;
    int x1 = 145;
    rect(    screen2, x0 - x1,     SCREEN2H - 103,     x0 + x1,     SCREEN2H - 25,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 103 + 1, x0 + x1 - 1, SCREEN2H - 25 - 1, COLOR_GRAY14);

    textprintf_centre(screen2, font, x0, SCREEN2H - 97, xcom1_color(BUTTON), _("Light level: %d"), scenario->rules[0]);
    textprintf_centre(screen2, font, x0, SCREEN2H - 85, xcom1_color(BUTTON), _("Points limit: %d000"), scenario->rules[1]);
    textprintf_centre(screen2, font, x0, SCREEN2H - 73, xcom1_color(BUTTON), scenario->rules[2] == 0 ? _("No turns limit") : _("Turns limit: %d"), scenario->rules[2]);
    textprintf_centre(screen2, font, x0, SCREEN2H - 61, xcom1_color(BUTTON), g_time_limit == -1 ? _("No time limit") : _("Time limit: %d sec"), g_time_limit);
    textprintf_centre(screen2, font, x0, SCREEN2H - 49, xcom1_color(BUTTON), _("Exploration level: %d"), scenario->rules[3]);
    textprintf_centre(screen2, font, x0, SCREEN2H - 37, xcom1_color(BUTTON), scenario->rules[4] ? _("Editor: ground on") : _("Editor: ground off") );
}

/**
 * Rules0: Light level
 */
void Units::draw_rules_0_window()
{
// Todo: rewrite to allow easier resizing this window, for longer translated strings
    int x0 = gmx + gmw / 2;
  //int x1 = 130;
    int x1 = 144;
    rect(    screen2, x0 - x1,     SCREEN2H - 139,     x0 + x1,     SCREEN2H - 97,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 139 + 1, x0 + x1 - 1, SCREEN2H - 97 - 1, COLOR_GRAY13);

    textprintf_centre(screen2, font, x0, SCREEN2H - 133, xcom1_color(SELECTED), "%d", scenario->rules[0]);
    textout_centre(screen2, font, "<", x0 - 20, SCREEN2H - 133, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 20, SCREEN2H - 133, xcom1_color(BUTTON));

    switch(scenario->rules[0]) {
        case 1: case 2: case 3:
        case 4: case 5: case 6:
        case 7: case 8: case 9:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 121, xcom1_color(COMMENT), _("Night.") );
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 109, xcom1_color(COMMENT), _("Low visibility.") );
        break;

        case 10: case 11: case 12:
        case 13:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 121, xcom1_color(COMMENT), _("Twilight.") );
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 109, xcom1_color(COMMENT), _("Medium visibility.") );
        break;
        
        case 14: case 15: case 16: 
        default:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 121, xcom1_color(COMMENT), _("Day.") );
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 109, xcom1_color(COMMENT), _("Good visibility.") );
        break;
    }
}

/**
 * Rules1: Point-Limit
 */
void Units::draw_rules_1_window()
{
    int x0 = gmx + gmw / 2;
  //int x1 =  40;
    int x1 =  40;
    rect(    screen2, x0 - x1,     SCREEN2H - 99,     x0 + x1,     SCREEN2H - 85,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 99 + 1, x0 + x1 - 1, SCREEN2H - 85 - 1, COLOR_GRAY13);

    textprintf_centre(screen2, font, x0, SCREEN2H - 95, xcom1_color(SELECTED), "%d000", scenario->rules[1]);
    textout_centre(screen2, font, "<", x0 - 30, SCREEN2H - 95, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 30, SCREEN2H - 95, xcom1_color(BUTTON));
}

/**
 * Rules2: Turns-Limit
 */
void Units::draw_rules_2_window()
{
    int x0 = gmx + gmw / 2;
  //int x1 =  30;
    int x1 =  30;
    rect(    screen2, x0 - x1,     SCREEN2H - 87,     x0 + x1,     SCREEN2H - 73,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 87 + 1, x0 + x1 - 1, SCREEN2H - 73 - 1, COLOR_GRAY13);

    textprintf_centre(screen2, font, x0, SCREEN2H - 83, xcom1_color(SELECTED), scenario->rules[2] == 0 ? _("no") : "%d", scenario->rules[2]);
    textout_centre(screen2, font, "<", x0 - 20, SCREEN2H - 83, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 20, SCREEN2H - 83, xcom1_color(BUTTON));
}

/**
 * Rules3: Timelimit
 */
void Units::draw_rules_3_window()
{
    int x0 = gmx + gmw / 2;
  //int x1 =  30;
    int x1 =  30;
    rect(    screen2, x0 - x1,     SCREEN2H - 75,     x0 + x1,     SCREEN2H - 61,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 75 + 1, x0 + x1 - 1, SCREEN2H - 61 - 1, COLOR_GRAY13);

    textprintf_centre(screen2, font, x0, SCREEN2H - 71, xcom1_color(SELECTED), g_time_limit == -1 ? _("no") : "%d", g_time_limit);
    textout_centre(screen2, font, "<", x0 - 20, SCREEN2H - 71, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 20, SCREEN2H - 71, xcom1_color(BUTTON));
}

/**
 * Rules4: Map Exploration Level
 */
void Units::draw_rules_4_window()
{
    int x0 = gmx + gmw / 2;
  //int x1 = 115;
    int x1 = 128;
  //rect(    screen2, x0 - x1,     SCREEN2H - 63,     x0 + 110,    SCREEN2H - 35,     COLOR_WHITE);
    rect(    screen2, x0 - x1,     SCREEN2H - 63,     x0 + x1,     SCREEN2H - 35,     COLOR_WHITE);
    rectfill(screen2, x0 - x1 + 1, SCREEN2H - 63 + 1, x0 + x1 - 1, SCREEN2H - 35 - 1, COLOR_GRAY13);

    textprintf_centre(screen2, font, x0, SCREEN2H - 59, xcom1_color(SELECTED), "%d", scenario->rules[3]);
    textout_centre(screen2, font, "<", x0 - 20, SCREEN2H - 59, xcom1_color(BUTTON));
    textout_centre(screen2, font, ">", x0 + 20, SCREEN2H - 59, xcom1_color(BUTTON));

    switch(scenario->rules[3]) {
        case 0:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 47, xcom1_color(COMMENT), _("Map isn't explored at all.") );
        break;

        case 1:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 47, xcom1_color(COMMENT), _("Deployment area is explored.") );
        break;

        case 2:
        textprintf(screen2, font, x0 - x1 + 3, SCREEN2H - 47, xcom1_color(COMMENT), _("The entire map is explored.") );
        break;
    }
}

/**
 * Calculate total point-cost of all soldiers (selected + unselected) of a team,
 * including soldiers & attributes, armor, weapons & equipment
 */
int Units::total_points()
{
    int points = 0;

    for (int i = 0; i < size; i++) {
        Soldier * ss = editor->platoon()->findman(name[i]);
        ASSERT(ss != NULL);
        points += ss->calc_full_ammunition_cost();
    }
    return points;
}

void Units::build_items_stats(ITEMDATA *id, uint32 *buf, int &len)
{
    for (int i = 0; i < id->num; i++) {
        buf[len++] = intel_uint32(id->item_type[i]);
    }
}

/**
 * Show summary of equipment for the platoon:
 * list of type and number of weapons to go on the mission.
 */
int Units::draw_items_stats(int gx, int gy)
{
    #undef map
    std::map<std::string, int> tbl;
    int damage_points = 0;
    for (int i = 0; i < size; i++) if (is_selected(i)) {
        Soldier *ss = editor->platoon()->findman(name[i]);
        ASSERT(ss != NULL);
        if (ss != NULL) {
            std::vector<Item *> items;
            ss->get_inventory_list(items);
            for (int i = 0; i < (int)items.size(); i++) {
                damage_points += items[i]->get_cost();
                if (!items[i]->obdata_isAmmo()) tbl[items[i]->name()]++;
            }
        }
    }
    
    std::map<std::string, int>::iterator it = tbl.begin();
    int aa = 0;
    while (it != tbl.end()) {
        textprintf(screen2, g_small_font, gx + (aa / 72) * 90, gy + (aa % 72),
            COLOR_WHITE, "%s=%d", it->first.c_str(), it->second);
        aa += 9;
        it++;
    }
    return damage_points;
}


/**
 * Print table of remote player soldier-names in the planner screen, 
 * and short statistics about the equipment selected.
 */
void Units::print_simple(int gcol)
{
    text_mode(-1);
    int i, x1, y1, x2, y2, color = COLOR_GREEN12;
    for (i = 0; i < size; i++) {
        if (is_selected(i)) {
            if (FLAGS & F_PLANNERDBG) { // show lines of remote
                if (pos == POS_LEFT) {
                    line(screen2, gx + MAN_NAME_LEN * 8 + 4, gy + i * 15 + 3, minimap_x(i), minimap_y(i), gcol);
                } else {
                    line(screen2, gx - 4, gy + i * 15 + 3, minimap_x(i), minimap_y(i), gcol);
                }
            }
            color = COLOR_GREEN05;
        } else {
            color = COLOR_GREEN12;
        }
        x1 = gx - 2;          x2 = gx + MAN_NAME_LEN * 8 + 2;
        y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;
        rectfill(screen2, x1, y1, x2, y2, color);  // Background for name-field

        textprintf(screen2, font, gx, gy + i * 15, gcol, "%s", name[i]);
    }
    draw_text();
    
    scenario->draw_deploy_zone(pos, gmx, 0, xcom1_color(REMOTE_COLOR));

    if (!SEND)
        return ;

    //Show statistics about equipment points used by the opponent,
    //via PLAYERDATA *pd_remote since platoon_remote is empty here.
    int remote_total_unit_cost = 0, remote_total_equipment_cost = 0,
        remote_unit_equipment_cost = 0;

    //cycle through all remote units, which are placed side-by-side in ManData
    for (int i = 0; i < pd_remote->size; i++) {
        //calculate a cost of current unit without equipment, add it to total
        remote_total_unit_cost += Soldier::calc_mandata_cost(pd_remote->md[i]);

        //calculate a cost of the current unit's equipment, add it to total
        //can't use Soldier::calc_full_ammunition_cost() it is not static
        remote_unit_equipment_cost = 0;
        //cycle over all unit's items which are placed side-by-side in ItemData
        uint32 item_type;
        int item_quantity = pd_remote->id[i].num;
        for (int j = 0; j < item_quantity; j++){
            item_type = intel_uint32(pd_remote->id[i].item_type[j]);
            remote_unit_equipment_cost += Item::obdata_cost(item_type);
        }
        remote_total_equipment_cost += remote_unit_equipment_cost;
    }

    int yy = gy + size * 15 - 3;
    textprintf_centre(screen2, g_small_font, gx + 10 * 8, yy, COLOR_GREEN, _("Ready to play!"));
    yy = yy + 10; //under "Ready to play!"
    textprintf_centre(screen2, g_small_font, gx + 10 * 8, yy, COLOR_GREEN,
        _("Total points=%d "), remote_total_unit_cost + remote_total_equipment_cost);
}

/**
 * Draw boxes with SEND and START-buttons
 */
void Units::draw_text()
{
    text_mode( -1);

    //rect(screen2, gx+50, SCREEN2H-35, gx+20*8-50, SCREEN2H-5, 1);
    //rect(screen2, gx+40, SCREEN2H-35, gx+20*8-40, SCREEN2H-5, 1);
    rect(screen2, gx, SCREEN2H - 35, gx + 20 * 8, SCREEN2H - 5, COLOR_WHITE);

    if (pos == POS_LEFT) {
        //textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "SERVER");
        textout_centre(screen2, font, _("SERVER"), gx + 10 * 8, SCREEN2H - 29, xcom1_color(CAPTION));
    } else {
        //textprintf(screen2, font, gx+56, SCREEN2H-29, 8, "%s", "CLIENT");
        textout_centre(screen2, font, _("CLIENT"), gx + 10 * 8, SCREEN2H - 29, xcom1_color(CAPTION));
    }

    int color = SWITCH_OFF;      //red
    if (SEND)
        color = SWITCH_ON;      //green

    textout_centre(screen2, font, _("SEND"), gx + 15 * 8, SCREEN2H - 16, xcom1_color(color));

    if (START)
        color = SWITCH_ON;      //green
    else
        color = SWITCH_OFF;     //red

    textout_centre(screen2, font, _("START"), gx + 5 * 8, SCREEN2H - 16, xcom1_color(color));
}

/**
 * Depending on PlannerState, execute different procedures of mission-planner:
 * Edit soldiers, select map, scenario, game-rules etc.
 */
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
        case PS_RULES_4:
        execute_rules_4(map, map_change_allowed);
        break;
    }
}

/**
 * Activate unit inventory editor
 */
bool Units::edit_unit(int num)
{
    if (editor->set_man(name[num])) {
        clear(screen);

        editor->show();
        editor->build_Units(*this);
        reset_video();

        destroy_bitmap(screen2);
        screen2 = create_bitmap(640, SCREEN2H - 1); clear(screen2);
        return true;
    } else {
        return false;
    }
}

/**
 * Mission-planner: select position for a soldier in deployment-area on map.
 * When user ctrl-clicks on a soldier-name, call editor for soldier-equipment.
 */
void Units::execute_main(Map *map, int map_change_allowed)
{
    int i;
    if (selected != -1) {
        int l;
        int c = (mouse_x - gmx) / 4;
        int r = (mouse_y - gmy) / 4;

        if (!(FLAGS & F_PLANNERDBG)) { // allow place at any position
            if (!scenario->is_correct_place(pos, c, r))
                return;
        }

        for (l = 0; l < map->level && (!map->passable(l, c, r) || map->isStairs(l, c, r)); l++) ;
        if (l == map->level) return;
        if (!map->support_for_feet(l, c, r)) return;

        for (int s = 0; s < size; s++) {
            if (s == selected)
                continue;
            if (is_selected(s) && (lev[s] == l) && (col[s] == c) && (row[s] == r))
                return ;
        }

        lev[selected] = l;
        col[selected] = c;
        row[selected] = r;
        net->send_select_unit(selected, lev[selected], col[selected], row[selected]);
        deselect();
        return ;
    }

    int x1, y1, x2, y2;
    x1 = gx - 2; x2 = gx + MAN_NAME_LEN * 8 + 2;

    for (i = 0; i < size; i++) {
        y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;

        if (mouse_inside(x1, y1, x2, y2)) {
            if (key[KEY_LCONTROL]) { // do editor
                edit_unit(i);
                return ;
            }
            selected = i;
            limit_mouse_range();
            return ;
        }
        if (is_selected(i)) {
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
        if (SEND == 1) return; // Already sent the unit data.
        Soldier *first_ss = NULL;
        int num_of_men_sel = 0;
        
        for (i = 0; i < editor->platoon()->num_of_men(); i++) {
            if (is_selected(i)) {
                Soldier *ss = editor->platoon()->findman(name[i]);
                ASSERT(ss != NULL);
                if (ss->has_forbidden_equipment()) {
                    g_console->printf(COLOR_RED04, _("Some of the soldiers selected for battle are equipped with forbidden weapons"));
                    return;
                }
                num_of_men_sel++;
                if (!first_ss) first_ss = ss;
            }
        }
        
        if (scenario->is_correct_platoon(points + damage_points, editor->platoon(), first_ss, pos, buf32, buf32_len, num_of_men_sel))
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
            {
                mapdata.load_game = 77;
                // We need to update the deployment type not only in
                // the option, but also in the scenario.
                scenario->update_deploy_type();
            }
        }
    }
}

/**
 * Select terrain-type & map-size, load map from file,
 * or generate a new map.
 */
void Units::execute_map(Map *map, int map_change_allowed)
{

    if (!mouse_inside(gmx + gmw / 2 - 120, SCREEN2H - 79, gmx + gmw / 2 + 120, SCREEN2H - 37))
        state = PS_MAIN;

    if (!map_change_allowed) return;

    // see also: draw_map_window
    if (mouse_inside(gmx + gmw / 2 - 75, SCREEN2H - 77, gmx + gmw / 2 + 75, SCREEN2H - 64)) {
        //MAP TYPE

        std::string current_terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
        std::string terrain_name = terrain_set->select_terrain_gui_dialog(current_terrain_name);

        if (current_terrain_name != terrain_name) {
            Map::new_GEODATA(&mapdata, terrain_name);
            net->send_map_data(&mapdata);
            mapdata.load_game = 77;
            scenario->new_coords();
        }
    }

    // Todo: adjust button-coordinates for translated strings
    // see also: draw_map_window
    int x0 = gmx + gmw / 2;
  //int x1a = 59, x1b =  20;  // old
  //int x2a = 19, x2b =  20;
  //int x3a = 21, x3b =  60;
  //g_console->printf(COLOR_SYS_DEBUG, "x0=%d mouse_x=%d diff=%d", x0, mouse_x, mouse_x-x0 );
  //int x1a = 84, x1b =  25;  // wide enough for german translation
  //int x2a = 20, x2b =  20;
  //int x3a = 25, x3b =  84;
    int x1a = 85, x1b =  36;  // for translation: ru
    int x2a = 37, x2b =  37;
    int x3a = 38, x3b =  85;
    if (mouse_inside(x0 - x1a, SCREEN2H - 63, x0 + x1b, SCREEN2H - 50)) {
        //"4*4"
        std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
        MAP_WIDTH = MAP_HEIGHT = 4;
        Map::new_GEODATA(&mapdata, terrain_name);
        net->send_map_data(&mapdata);
        mapdata.load_game = 77;
        scenario->new_coords();
    }

    if (mouse_inside(x0 - x2a, SCREEN2H - 63, x0 + x2b, SCREEN2H - 50)) {
        //"5*5"
        std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
        MAP_WIDTH = MAP_HEIGHT = 5;
        Map::new_GEODATA(&mapdata, terrain_name);
        net->send_map_data(&mapdata);
        mapdata.load_game = 77;
        scenario->new_coords();
    }

    if (mouse_inside(x0 + x3a, SCREEN2H - 63, x0 + x3b, SCREEN2H - 50)) {
        //"6*6"
        std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
        MAP_WIDTH = MAP_HEIGHT = 6;
        Map::new_GEODATA(&mapdata, terrain_name);
        net->send_map_data(&mapdata);
        mapdata.load_game = 77;
        scenario->new_coords();
    }

    if (mouse_inside(x0 - x1a, SCREEN2H - 49, x0 - x1b, SCREEN2H - 36)) {
        //"NEW"
        std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
        Map::new_GEODATA(&mapdata, terrain_name);
        net->send_map_data(&mapdata);
        mapdata.load_game = 77;
        scenario->new_coords();
    }

    if (mouse_inside(x0 - x2a, SCREEN2H - 49, x0 + x2b, SCREEN2H - 36)) {
        //"LOAD"
        std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
            _("Load map"), F("$(home)"), "area");
                
        if (!filename.empty()) {
            GEODATA gd;

            if (!Map::load_GEODATA(filename.c_str(), &gd) || !Map::valid_GEODATA(&gd)) {
                g_console->printf(COLOR_RED02, _("Invalid map file.") );
            } else {
                memcpy(&mapdata, &gd, sizeof(mapdata));
                net->send_map_data(&mapdata);
                mapdata.load_game = 77;
                net->send_scenario();
            }
        }
    }

    if (mouse_inside(x0 + x3a, SCREEN2H - 49, x0 + x3b, SCREEN2H - 36)) {
        //"SAVE"
        std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
            _("Save map"), F("$(home)"), "area", true);
        
        if (!filename.empty()) {
            if(!Map::save_GEODATA(filename.c_str(), &mapdata))
                g_console->printf(COLOR_RED02, _("Can't save map file.") );
        }
    }
}

void Units::execute_rules(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 145, SCREEN2H - 103, gmx + gmw / 2 + 145, SCREEN2H - 25))
        state = PS_MAIN;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 101, gmx + gmw / 2 + 80, SCREEN2H - 89))
        state = PS_RULES_0;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 88, gmx + gmw / 2 + 80, SCREEN2H - 76))
        state = PS_RULES_1;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 75, gmx + gmw / 2 + 80, SCREEN2H - 63))
        state = PS_RULES_2;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 62, gmx + gmw / 2 + 80, SCREEN2H - 50))
        state = PS_RULES_3;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 49, gmx + gmw / 2 + 80, SCREEN2H - 37))
        state = PS_RULES_4;

    if (mouse_inside(gmx + gmw / 2 - 80, SCREEN2H - 36, gmx + gmw / 2 + 80, SCREEN2H - 25)) {
        if (scenario->rules[4] == 0)
            scenario->rules[4] = 1;
        else
            scenario->rules[4] = 0;
        net->send_rules(4, scenario->rules[4]);
    }
}

void Units::execute_rules_0(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 144, SCREEN2H - 139, gmx + gmw / 2 + 144, SCREEN2H - 97))
        state = PS_RULES;

    if (!map_change_allowed) return;

    if (mouse_inside(gmx + gmw / 2 - 25, SCREEN2H - 137, gmx + gmw / 2 - 15, SCREEN2H - 124)) {
        //"<"
        scenario->rules[0]--;

        if (scenario->rules[0] < 6) {
            scenario->rules[0] = 6;
            return;
        }

        net->send_rules(0, scenario->rules[0]);
    }

    if (mouse_inside(gmx + gmw / 2 + 15, SCREEN2H - 137, gmx + gmw / 2 + 25, SCREEN2H - 124)) {
        //">"
        scenario->rules[0]++;

        if (scenario->rules[0] > 16) {
            scenario->rules[0] = 16;
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

        if (scenario->rules[1] > 50) {
            scenario->rules[1] = 50;
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

void Units::execute_rules_4(Map *map, int map_change_allowed)
{
    if (!mouse_inside(gmx + gmw / 2 - 128, SCREEN2H - 63, gmx + gmw / 2 + 128, SCREEN2H - 35))
        state = PS_RULES;

    if (!map_change_allowed) return;

    if (mouse_inside(gmx + gmw / 2 - 25, SCREEN2H - 63, gmx + gmw / 2 - 15, SCREEN2H - 51)) {
        //"<"
        scenario->rules[3]--;

        if (scenario->rules[3] < 0) {
            scenario->rules[3] = 0;
            return;
        }

        net->send_rules(3, scenario->rules[3]);
    }

    if (mouse_inside(gmx + gmw / 2 + 15, SCREEN2H - 63, gmx + gmw / 2 + 25, SCREEN2H - 51)) {
        //">"
        scenario->rules[3]++;

        if (scenario->rules[3] > 2) {
            scenario->rules[3] = 2;
            return;
        }

        net->send_rules(3, scenario->rules[3]);
    }
}

/**
 * Process right-click in mission-planner:
 * select & deselect soldier from list.
 */
void Units::execute_right()
{
    if (selected != -1) {
        deselect();
    } else {
        int x1, y1, x2, y2;
        x1 = gx - 2; x2 = gx + MAN_NAME_LEN * 8 + 2;

        for (int i = 0; i < size; i++) {
            y1 = gy + i * 15 - 2; y2 = y1 + 8 + 3;

            if (mouse_inside(x1, y1, x2, y2)) {
                if (is_selected(i)) {
                    lev[i] = -1; // Deselect the current unit
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

void Units::store_mouse_range(int mx1n, int my1n, int mx2n, int my2n)
{
    mx1 = mx1n;
    my1 = my1n;
    mx2 = mx2n;
    my2 = my2n;
}

void Units::limit_mouse_range()
{
    ASSERT (temp_mouse_range == NULL);
    temp_mouse_range = new MouseRange(mx1, my1, mx2, my2);
    if (is_selected(selected))
        position_mouse(minimap_x(selected), minimap_y(selected));
    else {
        if (pos == POS_LEFT)
            position_mouse(mx1 + 20, mouse_y);
        else
            position_mouse(mx2 - 20, mouse_y);
    }
}

void Units::restore_mouse_range()
{
    if (temp_mouse_range != NULL)
    {
        delete temp_mouse_range;
        temp_mouse_range = NULL;
    }
}

/**
 * Draw lines from list of soldiers to their positions in the deployment-area
 */
void Units::draw_lines(int gcol)
{
    for (int i = 0; i < size; i++) {
        if (selected == i) {
            line(screen2, gx + MAN_NAME_LEN * 8 + 4, gy + i * 15 + 3, mouse_x, mouse_y, COLOR_WHITE);
        } else
            if (is_selected(i)) {
                line(screen2, gx + MAN_NAME_LEN * 8 + 4, gy + i * 15 + 3, minimap_x(i), minimap_y(i), gcol);
            }
    }
}
