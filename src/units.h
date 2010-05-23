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
#ifndef UNITS_H
#define UNITS_H
#include "mouse.h"

enum PanPos {POS_LEFT, POS_RIGHT};
enum PlanState {PS_MAIN, PS_SCEN, PS_MAP, PS_RULES, PS_RULES_0, PS_RULES_1, PS_RULES_2, PS_RULES_3, PS_RULES_4};

/**
 * Object responsible for units selection buttons in the planner screen.  
 * 
 * @ingroup planner
 * @ingroup gui
 */
class Units
{
    friend class Connect;
private:
    int size;
    char name[SQUAD_LIMIT][MAN_NAME_BUFSIZE];
    int cost[SQUAD_LIMIT];
    int lev[SQUAD_LIMIT], col[SQUAD_LIMIT], row[SQUAD_LIMIT];
    int selected;                   //!< number of the soldier currently selected for editing
    int mx1, my1, mx2, my2;
    int gx, gy, gmx, gmw, gmy, gmh;
    MouseRange *temp_mouse_range;
    PanPos pos;
    PlanState state;
    void draw_scenario_window();
    void draw_map_window();
    void draw_rules_window();
    void draw_rules_0_window();
    void draw_rules_1_window();
    void draw_rules_2_window();
    void draw_rules_3_window();
    void draw_rules_4_window();
    void execute_main(Map *map, int map_change_allowed);
    void execute_scenario(Map *map, int map_change_allowed);
    void execute_map(Map *map, int map_change_allowed);
    void execute_rules(Map *map, int map_change_allowed);
    void execute_rules_0(Map *map, int map_change_allowed);
    void execute_rules_1(Map *map, int map_change_allowed);
    void execute_rules_2(Map *map, int map_change_allowed);
    void execute_rules_3(Map *map, int map_change_allowed);
    void execute_rules_4(Map *map, int map_change_allowed);

    int minimap_x(int num) {return gmx + (col[num] << 2) + 2;}
    int minimap_y(int num) {return gmy + (row[num] << 2) + 2;}
public:
    int SEND;
    int START;
    int Position; //!< 1 - the player goes first, 2 - second
    int packet_num; //!< Number of packets sended by player

    Units();
    ~Units();
    void reset();
    void set_pos(PanPos pos, int gx, int gy, int gmx, int gmw, int gmy, int gmh);
    void reset_selections();

    int add(int num, const char *nm, int ct);
    void store_mouse_range(int mx1, int my1, int mx2, int my2);
    void limit_mouse_range();
    void restore_mouse_range();

    void print(int gcol);
    void print_simple(int gcol);

    int total_points();

    /// @deprecated
    void build_items_stats(ITEMDATA *id, uint32 *buf, int &len);
    int draw_items_stats(int gx, int gy);

    void execute(Map *map, int map_change_allowed = 1);
    void execute_right();
    void deselect();
    void draw_lines(int gcol);
    void draw_text();
    int select_unit(int num, int n_lev, int n_col, int n_row);
    int deselect_unit(int num);
    int is_selected(int num) {return (lev[num] != -1);}
    bool edit_unit(int num);
    friend class Platoon;
};

#endif
