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
#include <string>
#include "spk.h"
#include "video.h"
#include "config.h"
#include "multiplay.h"

extern void install_timers(int _speed_unit, int _speed_bullet, int _speed_mapscroll);
extern void uninstall_timers();
extern int speed_unit;
extern int speed_bullet;
extern int speed_mapscroll;
extern int mapscroll;

static int USETRANS = 1;

static char *gen = "General";
static char *edit = "Editor";

static int base_accuracy = 75;
static std::string midi_file_name;

int cfg_get_base_accuracy()
{
	return base_accuracy;
}

const char *cfg_get_music_file_name()
{
	return midi_file_name.c_str();
}

void loadini()
{
	set_config_file("ufo2000.ini");

	strcpy(PORT, get_config_string(gen, "port", "2000"));
	strcpy(HOSTNAME, get_config_string(gen, "server", "127.0.0.1"));

	SCREEN2W = get_config_int(gen, "width", 430);
	SCREEN2H = get_config_int(gen, "height", 280);

//	check screen dimensions as set in .ini for too low or too high settings

	if (SCREEN2W > 640) SCREEN2W = 640;
	if (SCREEN2H > 380) SCREEN2H = 380;

	if (SCREEN2W < 430) SCREEN2W = 430;
	if (SCREEN2H < 280) SCREEN2H = 280;

	MAP_WIDTH = get_config_int(gen, "map_width", 4);
	MAP_HEIGHT = get_config_int(gen, "map_height", 4);
	speed_unit = get_config_int(gen, "speed_unit", 30);
	speed_bullet = get_config_int(gen, "speed_bullet", 30);
	speed_mapscroll = get_config_int(gen, "speed_mapscroll", 30);
	mapscroll = get_config_int(gen, "mapscroll", 10);

/*
	SOUND = get_config_int(gen, "sound", 1);
	MODSOUND = get_config_int(gen, "modules", 0);
	digvoices = get_config_int(gen, "digvoices", 16);
	modvoices = get_config_int(gen, "modvoices", 8);
	strcpy(modname, get_config_string(gen, "modname", "templsun.xm"));
	modvol = get_config_int(gen, "modvolume", 125);
*/
	local_platoon_size = get_config_int(edit, "platoon_size", 1);
	strcpy(last_map_name, get_config_string(edit, "last_map_name", ""));
	strcpy(last_unit_name, get_config_string(edit, "last_unit_name", ""));

	g_time_limit = get_config_int(gen, "time_limit", -1);

//	base_accuracy = get_config_int(gen, "accuracy", 75);
	midi_file_name = get_config_string(gen, "music", "sound/gmtactic.mid");
}

void saveini()
{
	set_config_string(gen, "server", HOSTNAME);
	set_config_int(gen, "width", SCREEN2W);
	set_config_int(gen, "height", SCREEN2H);

	set_config_int(gen, "speed_unit", speed_unit);
	set_config_int(gen, "speed_bullet", speed_bullet);
	set_config_int(gen, "speed_mapscroll", speed_mapscroll);
	set_config_int(gen, "mapscroll", mapscroll);

	set_config_int(edit, "platoon_size", local_platoon_size);
	set_config_string(edit, "last_map_name", last_map_name);
	set_config_string(edit, "last_unit_name", last_unit_name);
}

int selectplaymode()
{
	SPK * back01 = new SPK("geograph/back01.scr");
	SPK *back03 = new SPK("geograph/back03.scr");

	install_int_ex(drawit_timer, BPS_TO_TIMER(10));      //ticks each second
	position_mouse(320, 200);
	set_mouse_range(0, 0, 639, 399);

	RGB pal[PAL_SIZE];
	setpal(pal, 0xC18);

	RGB_MAP rgb_table;
	COLOR_MAP trans_table_left;
	COLOR_MAP trans_table_right;
	int ttl = 0, ttr = 0;

	create_rgb_table(&rgb_table, pal, NULL);
	RGB_MAP *rgb_map_old = rgb_map;
	rgb_map = &rgb_table;

	//create_trans_table(&trans_table_left,  pal, 0, 0, 0, cf);
	//create_trans_table(&trans_table_right, pal, 0, 0, 0, cf);
	BITMAP *scr = create_bitmap(320, 200); clear(scr);
	BITMAP *left = create_bitmap(76, 149); clear(left);
	BITMAP *right = create_bitmap(97, 101); clear(right);
	back01->show(scr, 0, 0);
	blit(scr, left, 127, 28, 0, 0, left->w, left->h);
	blit(scr, right, 205, 98, 0, 0, right->w, right->h);

	int o;
	for (o = 0; o < right->w*right->h; o++) {
		int v = spr_get(right, o);
		if ((v == 238) || (v == 239))
			spr_set(right, o, 0);
	}

	for (o = 0; o < left->w*left->h; o++) {
		int v = spr_get(left, o);
		if ((v == 238) || (v == 239))
			spr_set(left, o, 0);
	}


	int DONE = 0;
	int ishost = -1;
	int lp = 0, rp = 0;
	DRAWIT = 1;
	text_mode( -1);

	while (!DONE) {
		if (CHANGE) {
			if (mouse_b & 1) {
				lp = 1;
			}
			if (mouse_b & 2) {
				rp = 1;
			}
			if (lp) {
				if (!(mouse_b & 1)) {
					if (mouse_inside(0 * 2, 0 * 2, 100 * 2, 150 * 2)) {
						ishost = 1;
						break;
					}
					if (mouse_inside(200 * 2, 0 * 2, 320 * 2, 150 * 2)) {
						ishost = 0;
						break;
					}
					lp = 0;
					//break;
				}
			}
			if (rp) {
				if (!(mouse_b & 2)) {
					break;
				}
			}
			CHANGE = 0;
			DRAWIT = 1;
		}
		if (DRAWIT) {
			if (mouse_inside(0 * 2, 0 * 2, 100 * 2, 150 * 2)) {
				ttl++; ttr--;
			} else if (mouse_inside(200 * 2, 0 * 2, 320 * 2, 150 * 2)) {
				ttr++; ttl--;
			} else {
				ttr--; ttl--;
			}
			if (ttl < 0) ttl = 0; if (ttl > 14) ttl = 14;
			if (ttr < 0) ttr = 0; if (ttr > 14) ttr = 14;
			if (USETRANS) {
				create_trans_table(&trans_table_left, pal, ttl * 16, ttl * 16, ttl * 16, NULL);
				create_trans_table(&trans_table_right, pal, ttr * 16, ttr * 16, ttr * 16, NULL);
			}
			back03->show(scr, 0, 0);
			if (mouse_inside(0 * 2, 0 * 2, 100 * 2, 150 * 2))
				textout(scr, font, "Server", 0, 141, xcom1_color(1));
			if (mouse_inside(200 * 2, 0 * 2, 320 * 2, 150 * 2))
				textout(scr, font, "Client", 272, 141, xcom1_color(1));
			if (USETRANS) {
				color_map = &trans_table_left;
				draw_trans_sprite(scr, left, 0, 0);
				color_map = &trans_table_right;
				draw_trans_sprite(scr, right, 223, 48);
			} else {
				if (ttl > 0)
					draw_sprite(scr, left, 0, 0);
				if (ttr > 0)
					draw_sprite(scr, right, 223, 48);
			}
			draw_sprite(scr, mouser, mouse_x / 2, mouse_y / 2);
			stretch_blit(scr, screen, 0, 0, 320, 200, 0, 0, 640, 400);

			DRAWIT = 0;
		}

		if (keypressed()) {
			int c = readkey();
			switch (c >> 8) {
				case KEY_ESC:
					DONE = 1;
					break;
			}
		}
	}

	rgb_map = rgb_map_old;

	position_mouse(320, 200);

	remove_int(drawit_timer);
	delete(back01);
	delete(back03);
	destroy_bitmap(right);
	destroy_bitmap(left);
	destroy_bitmap(scr);

	fade_out(10);
	clear(screen);

	return ishost;
}

int setsocketplay()
{
	int host = selectplaymode();
	net->gametype = SOCK;
	return host;
}

int sethotseatplay()
{
	net->gametype = HOTSEAT;
	return 1;
}

#define FG 0
#define BG 1

#define SPEED_UNIT       2
#define SPEED_BULLET     3
#define SPEED_MAPSCROLL  4
#define MAPSCROLL        5
#define OK_BUTTON   6
#define MAX_VALUE  99

static int d_slider_pro2(int msg, DIALOG *d, int c)
{
	char s[100];
	int v;
	v = d_slider_proc(msg, d, c);
	switch (msg) {
		case MSG_DRAW:
			sprintf(s, "%2d", d->d2);
			text_mode(d->bg);
			//gui_textout(screen, s, d->x+d->w, d->y+4, d->fg, 0);
			gui_textout(screen, s, d->x - 18, d->y + 4, d->fg, 0);
			break;
		default:
			break;
	}
	return v;
}

static DIALOG config_dlg[] = {
                                 { d_shadow_box_proc, 0, 0, 320, 200, FG, BG, 0, 0, 0, 0, NULL, NULL, NULL },
                                 { d_text_proc, 72, 8, 144, 16, FG, BG, 0, 0, 0, 0, (void *)"Configuration", NULL, NULL },
                                 { d_slider_pro2, 24, 40, 136, 16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
                                 { d_slider_pro2, 24, 64, 136, 16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
                                 { d_slider_pro2, 24, 88, 136, 16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
                                 { d_slider_pro2, 24, 112, 136, 16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
                                 { d_button_proc, 160, 168, 64, 16, FG, BG, 0, D_EXIT, 0, 0, (void *)"OK", NULL, NULL },
                                 { d_button_proc, 232, 168, 64, 16, FG, BG, 0, D_EXIT | D_GOTFOCUS, 0, 0, (void *)"Cancel", NULL, NULL },
                                 { d_text_proc, 176, 44, 88, 16, FG, BG, 0, 0, 0, 0, (void *)"movement speed", NULL, NULL },
                                 { d_text_proc, 176, 68, 104, 16, FG, BG, 0, 0, 0, 0, (void *)"fire speed", NULL, NULL },
                                 { d_text_proc, 176, 92, 128, 16, FG, BG, 0, 0, 0, 0, (void *)"scroll speed", NULL, NULL },
                                 { d_text_proc, 176, 116, 128, 16, FG, BG, 0, 0, 0, 0, (void *)"mapscroll points", NULL, NULL },
                                 { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
                             };

void configure()
{
	config_dlg[SPEED_UNIT].d2 = speed_unit;
	config_dlg[SPEED_BULLET].d2 = speed_bullet;
	config_dlg[SPEED_MAPSCROLL].d2 = speed_mapscroll;
	config_dlg[MAPSCROLL].d2 = mapscroll;

	if (popup_dialog(config_dlg, -1) == OK_BUTTON) {
		if (config_dlg[SPEED_UNIT].d2)
			speed_unit = config_dlg[SPEED_UNIT].d2;
		if (config_dlg[SPEED_BULLET].d2)
			speed_bullet = config_dlg[SPEED_BULLET].d2;
		if (config_dlg[SPEED_MAPSCROLL].d2)
			speed_mapscroll = config_dlg[SPEED_MAPSCROLL].d2;
		if (config_dlg[MAPSCROLL].d2)
			mapscroll = config_dlg[MAPSCROLL].d2;

		uninstall_timers();
		install_timers(speed_unit, speed_bullet, speed_mapscroll);
	}

	return ;
}
