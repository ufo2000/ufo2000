/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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
#include "keys.h"

static int keyswitch_mode = 0;
static int keyswitch_lang = 0;

void keyswitch(int keyswitch_lang)
{
	push_config_state();
	set_config_file(F("$(home)/ufo2000.ini"));

	switch (keyswitch_lang) {
		case 0: {
			const char *current_keyboard = get_config_string("system", "keyboard", "us");
			const char *primary_keyboard = get_config_string("system", "primary_keyboard", "us");
			const char *secondary_keyboard = get_config_string("system", "secondary_keyboard", "ru");
			if (strcmp(current_keyboard, primary_keyboard) == 0)
				set_config_string("system", "keyboard", secondary_keyboard);
			else
				set_config_string("system", "keyboard", primary_keyboard);
			break;
		}
		case KEY_E: 
			set_config_string("system", "keyboard", "us"); 
			break;
		case KEY_R: 
			set_config_string("system", "keyboard", "ru"); 
			break;
		case KEY_B: 
			set_config_string("system", "keyboard", "by"); 
			break;
		case KEY_P: 
			set_config_string("system", "keyboard", "pl"); 
			break;
		case KEY_D: 
			set_config_string("system", "keyboard", "de"); 
			break;
		default: 
			pop_config_state(); 
			return;
	}
	
	clear_keybuf();
	remove_keyboard();
	install_keyboard();
	pop_config_state();
}

void process_keyswitch()
{
	if (!keyswitch_mode && key[KEY_RCONTROL]) {
		keyswitch_mode = 1;
		keyswitch_lang = 0;
	}

	if (keyswitch_mode && !key[KEY_RCONTROL]) {
		keyswitch(keyswitch_lang);
		keyswitch_mode = 0;
	}

	if (keyswitch_mode && keypressed()) {
		keyswitch_lang = readkey() >> 8;
	}
}
