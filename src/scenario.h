/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
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

#define dm g_console->printf

#include "global.h"
#include "platoon.h"
#include "units.h"

#define SC_DEATHMATCH   0
#define SC_ESCAPE       1
#define SC_SABOTAGE     2
#define SC_CONTROL      3
#define SC_ASSASSIN     4
#define SC_HOLD         5
#define SC_BREAK		6
#define SCENARIO_NUMBER 7

enum OptionType {OPT_NONE, OPT_SWITCH, OPT_NUMBER};

class Option 
{
public:
	Option (OptionType new_type) 
	{
		type = OPT_NONE;
		value = 0;
		reset_deploy = false;
	};

	Option (OptionType new_type, int new_value, int new_step, int new_min, int new_max, int new_value_no, char *new_caption, bool new_reset_deploy)
	{
		if (new_type == OPT_NUMBER) {
			type = new_type;
			value = new_value;
			step = new_step;
			min = new_min;
			max = new_max;
			value_no = new_value_no;
			caption = new_caption;
			reset_deploy = new_reset_deploy;
		} else {
			type = OPT_NONE;
			value = 0;
			reset_deploy = false;
		}
	}
		
	Option (OptionType new_type, int new_value, char *new_caption_on, char *new_caption_off, bool new_reset_deploy)
	{
		if (new_type == OPT_SWITCH) {
			type = OPT_SWITCH;
			value = new_value;
			caption_on = new_caption_on;
			caption_off = new_caption_off;
			reset_deploy = new_reset_deploy;
		} else {
			type = OPT_NONE;
			value = 0;
			reset_deploy = false;
		}
	}
	
	//common
	OptionType type;
	int value;
	bool reset_deploy;
	
	//number
	int step;
	int min;
	int max;
	int value_no;
	char *caption;
	
	//switch
	char *caption_on;
	char *caption_off;
};

class Scenario: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Scenario)
private:
	int             turn_hold[2]; //conditions_control
	int             num_of_men;   //conditions_hold and conditions_break
	
	void			init_deathmatch ();
	void			init_escape ();
	void			init_sabotage ();
	void			init_control ();
	void			init_assassin ();
	void			init_hold ();
	void			init_break ();      
	
	int             conditions_common ();
	int             conditions_escape ();
	int             conditions_sabotage ();
	int             conditions_control ();
	int             conditions_assassin ();
	int             conditions_hold ();
	int				conditions_break ();

	bool            minimap_escape (int lev, int col, int row, Map *m_map);
	bool            minimap_sabotage (int lev, int col, int row);
	void            minimap_control (BITMAP *bmp, int x, int y);
	bool            minimap_assassin (int lev, int col, int row, Map *m_map);
	bool			minimap_rescue (int col, int row);               
	
	bool            platoon_common (long points, PanPos pos, char buf[10000], int len);
	bool            platoon_escape (Platoon *platoon, PanPos pos, char *first_soldier);
	bool            platoon_sabotage (PanPos pos, char buf[10000], int len);
	bool            platoon_assassin (Platoon *platoon, PanPos pos, char *first_soldier);
	bool            platoon_hold (PanPos pos, int num_of_men_sel);
	bool			platoon_break (PanPos pos, int num_of_men_sel);
	
	bool            place_common (PanPos pos, int x, int y);
	bool            place_assassin (PanPos pos, int x, int y);
	bool            place_hold (PanPos pos, int x, int y);
	bool            place_break (PanPos pos, int x, int y);
	
	void			deploy_common (PanPos pos, int x, int y, int color);
	void			deploy_assassin (PanPos pos, int x, int y, int color);
	void			deploy_hold (PanPos pos, int x, int y, int color);	
	void			deploy_break (PanPos pos, int x, int y, int color);
public:
					Scenario (int sc_type);
	void            new_scenario (int sc_type);
	bool			new_scenario (std::string sc_name);
    void            new_coords ();
    
    int             rules[4]; //0 - allowed explosives, 1 - points limit(x1000), 2 - turn limit, 3 - all map explored
    Option			*options[SCENARIO_NUMBER][3];
	
	int				type;
	const char		*name[SCENARIO_NUMBER];
	const char		*briefing_left[SCENARIO_NUMBER][8];
	const char	 	*briefing_right[SCENARIO_NUMBER][8];
	
	int             x1, x2, y1, y2;
	
	int             check_conditions ();
	bool            is_target_on_minimap (int lev, int col, int row, Map *m_map);
	void			draw_minimap_rectangle (BITMAP *bmp, int x, int y);
	bool		    is_correct_platoon (long points, Platoon *platoon, char *first_soldier, PanPos pos, char buf[10000], int len, int num_of_men_sel);
	bool            is_correct_place (PanPos pos, int x, int y);
	void			draw_deploy_zone (PanPos pos, int x, int y, int color);
};
