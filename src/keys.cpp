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

#include "global.h"
#include <string.h>
#include "keys.h"

static int keyswitch_mode = 0;
static int keyswitch_lang = 0;
static int lang = 'e';

/**
 * Array for translation of keyboard input into unicode characters for currently 
 * selected language. By default this array performs no conversion (for english 
 * language ASCII codes map directly to unicode)
 */
static int keymap[128] = 
{
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15, 
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31, 
	 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63, 
	 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79, 
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95, 
	 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};

/**
 * Function that fills keymap array for selected language. It maps each character 
 * from eng string to characters from lng string in the same position.
 *
 * @param eng string of characters for english language
 * @param lng string of characters for selected language in UTF-8 encoding
 */
void prepare_keymap(const char *eng, const char *lng)
{
	assert(get_uformat() == U_UTF8);
	assert(strlen(eng) == ustrlen(lng));

	for (int i = 0; i < 128; i++) {
		const char * p = strchr(eng, (char)i);
		keymap[i] = (p == NULL) ? i : ugetat(lng, p - eng);
	}
}

void keyswitch(int keyswitch_lang)
{
	switch (keyswitch_lang) {
		case 'e': lang = 'e'; break;
		case 'r': lang = 'r'; break;
		case 'b': lang = 'b'; break;
		default:  lang = (lang == 'e') ? 'r' : 'e'; break;
	}

	switch (lang) {
		case 'e': 
			prepare_keymap(
				"", 
				""); 
			break;
		case 'r': 
			prepare_keymap(
				"`qwertyuiop[]asdfghjkl;'zxcvbnm,./~QWERTYUIOP{}ASDFGHJKL:" "\"" "ZXCVBNM<>?",
				"ёйцукенгшщзхъфывапролджэячсмитьбю.ЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖ" "Э"  "ЯЧСМИТЬБЮ,"); 
			break;
		case 'b': 
			prepare_keymap(
				"`qwertyuiop[]asdfghjkl;'zxcvbnm,./~QWERTYUIOP{}ASDFGHJKL:" "\"" "ZXCVBNM<>?",
				"ёйцукенгшўзх'фывапролджэячсмітьбю.ЁЙЦУКЕНГШЎЗХ'ФЫВАПРОЛДЖ" "Э"  "ЯЧСМІТЬБЮ,"); 
			break;
	}
}

int keymaper(int c)
{
	if (c < 32 || c > 127) return '?';
	return keymap[c];
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
		int c = readkey();
		int key = scancode_to_ascii(c >> 8);
		if (key != 0) keyswitch_lang = key;
	}
}
