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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "video.h"
#include "keys.h"
#include "spk.h"
#include "wind.h"
#include "multiplay.h"
#include "connect.h"
#include "platoon.h"
#include "units.h"
#include "version.h"
#include "music.h"

int Connect::do_chat()
{
	Wind *local_win = NULL, *remote_win = NULL, *info_win = NULL;

	bool version_check_passed = false;

	reset_video();

	set_mouse_range(0, 0, 640, 400);
	position_mouse(320, 200);

	BITMAP *scr = create_bitmap(320, 200); clear(scr);
	BITMAP *backscr = create_bitmap(640, 400);
	SPK *back09 = new SPK("$(xcom)/geograph/back09.scr");      //gamepal used

	install_int_ex(drawit_timer, BPS_TO_TIMER(10));      //ticks each second

	back09->show(scr, 0, 0);
	stretch_blit(scr, screen, 0, 0, 320, 200, 0, 0, 640, 400);
	stretch_blit(scr, backscr, 0, 0, 320, 200, 0, 0, 640, 400);

	local_win = new Wind(backscr, 15, 197, 619, 383, xcom1_color(16));
	remote_win = new Wind(backscr, 15, 12, 408, 179, xcom1_color(33));
	info_win = new Wind(backscr, 434, 17, 619, 171, xcom1_color(192));

	int DONE = 0;
	std::string buf;

	switch (net->gametype) {
		case GAME_TYPE_INTERNET_SERVER:
			break;
		default:
			ASSERT(false);
			break;
	}

	g_net_allowed_terrains.clear();

	remote_win->printstr("\n");
	remote_win->printstr("Comparing local and remote UFO2000 versions...\n");
	remote_win->printstr("Press ESC to cancel\n");

	char version_check_packet[128];
	sprintf(version_check_packet, "UFO2000 REVISION OF YOUR OPPONENT: %d", UFO_REVISION_NUMBER);
	net->send(version_check_packet);

	while (!DONE) {
		if (net->recv(buf)) {
			int remote_revision;
			if (sscanf(buf.c_str(), "UFO2000 REVISION OF YOUR OPPONENT: %d", &remote_revision) == 1) {
				if (UFO_REVISION_NUMBER == remote_revision) {
					net->send("START");
					version_check_passed = true;
				} else {
					if (remote_revision < UFO_REVISION_NUMBER) {
						remote_win->printstr("\nUnfortunately your opponent has an\n");
						remote_win->printstr("outdated UFO2000 version and you will be\n");
						remote_win->printstr("unable to play until he upgrades\n");
				    } else {
						char tmp[128];
						sprintf(tmp, "\nYou need UFO2000 %s (revision %d)\nor newer", 
							UFO_VERSION_STRING, remote_revision);
						remote_win->printstr("\nUnfortunately you have older UFO2000\n");
						remote_win->printstr("version than your opponent has.\n");
						remote_win->printstr("\nPlease visit http://ufo2000.sourceforge.net\n");
						remote_win->printstr("and upgrade your UFO2000 version\n");
						remote_win->printstr(tmp);
				    }
					net->send("QUIT");
					net->SEND = 0;
					DONE = 1;
				}
			}

			if (strstr(buf.c_str(), "QUIT") != NULL) {
				net->SEND = 0;
				DONE = 1;
			}
			if (strstr(buf.c_str(), "START") != NULL) {
				if (!version_check_passed) {
					remote_win->printstr("\nUnfortunately your opponent has an\n");
					remote_win->printstr("outdated UFO2000 version and you will be\n");
					remote_win->printstr("unable to play until he upgrades\n");
					net->send("QUIT");
					net->SEND = 0;
				}
				DONE = 1;
			}
		}

		if (!net->SEND) {
			info_win->printstr("\nconnection closed");
			readkey();
			DONE = 1;
		}

		if (keypressed()) {
			int scancode;
			ureadkey(&scancode);

			switch (scancode) {
				case KEY_F4:
				case KEY_F5:
					info_win->redraw_full();
					local_win->redraw();
					remote_win->redraw();
					break;
				case KEY_ESC:
					net->send("QUIT");
					net->SEND = 0;
					DONE = 1;
					break;
			}
		}
	}

	remove_int(drawit_timer);
	delete back09;
	delete local_win; local_win = NULL;
	delete remote_win; remote_win = NULL;
	delete info_win; info_win = NULL;
	destroy_bitmap(scr);
	destroy_bitmap(backscr);

	fade_out(10);
	clear(screen);
	return net->SEND;
}

#include "map.h"
#include "editor.h"
#include "wind.h"

Units local;
Units remote;

void Connect::reset_uds()
{
	local.reset();
	remote.reset();
}

void Connect::swap_uds()
{
	Units u;
	memcpy(&u, &local, sizeof(Units));
	memcpy(&local, &remote, sizeof(Units));
	memcpy(&remote, &u, sizeof(Units));
}

int FINISH_PLANNER = 0;

int Connect::do_planner(int F10ALLOWED, int map_change_allowed)
{
	MODE = PLANNER;

	int mouse_leftr = 1, mouse_rightr = 1;
	int DONE = 0;
	//HOST = 0;
	FINISH_PLANNER = 0;

	reset_video();
	destroy_bitmap(screen2);
	screen2 = create_bitmap(640, SCREEN2H - 1); clear(screen2);
	set_mouse_range(0, 0, 639, SCREEN2H - 1);

	g_console->set_full_redraw();
	g_console->redraw(screen, 0, SCREEN2H);

	Map *map = new Map(mapdata);
	BITMAP *map2d = map->create_bitmap_of_map(0);
	int map2d_x = (640 - map2d->w) / 2;

	if (HOST) {
		local.set_pos(POS_LEFT, map2d_x - (20 * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
		remote.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
		pd_local = &pd1;
		pd_remote = &pd2;
	} else {
		remote.set_pos(POS_LEFT, map2d_x - (20 * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
		local.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
		pd_remote = &pd1;
		pd_local = &pd2;
	}

	local.set_mouse_range(639, SCREEN2H - 1, map2d_x, 0, map2d_x + map2d->w - 1, map2d->h - 1);
	editor->build_Units(local);

    // Synchronize available terrains list
    if (net->is_network_game()) {
#undef map
		std::map<int, Terrain *>::iterator it = terrain_set->terrain.begin();
		while (it != terrain_set->terrain.end()) {
			net->send_terrain_crc32(it->first, it->second->get_crc32());
			it++;
		}
		net->send_terrain_crc32(-1, 0);
#define map ufo2000_map

		// Wait until a complete list of remote terrains is received
		while (g_net_allowed_terrains.find(-1) == g_net_allowed_terrains.end()) {
			net->check();
			yield_timeslice();
		}

		if (g_net_allowed_terrains.size() == 1) {
			// No allowed terrains received, only end marker

			alert("remote player does not have any of your maps", "", "", "OK", NULL, 0, 0);

			delete map;

			destroy_bitmap(screen2);
			screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);
			fade_out(10);
			clear(screen);
			return 0;
		}
	}

    // Make sure that we have a valid map
	if (!Map::valid_GEODATA(&mapdata)) {
		Map::new_GEODATA(&mapdata);
	}

	mapdata.load_game = 77;

	if (HOST) {
		net->send_map_data(&mapdata);
		net->send_time_limit(g_time_limit);
	}

	while (!DONE) {

		g_console->redraw(screen, 0, SCREEN2H);

		net->check();
		if (mapdata.load_game == 77) { //new	mapdata
			g_console->printf("suggested map: %s\n", 
				terrain_set->get_terrain_name(mapdata.terrain).c_str());
			mapdata.load_game = 0;
			delete map;
			destroy_bitmap(map2d);
			map = new Map(mapdata);
			map2d = map->create_bitmap_of_map(0);
			map2d_x = (640 - map2d->w) / 2;

			local.reset_selections();
			remote.reset_selections();

			if (HOST) {
				local.set_pos(POS_LEFT, map2d_x - (20 * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
				remote.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
			} else {
				remote.set_pos(POS_LEFT, map2d_x - (20 * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
				local.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
			}
			local.set_mouse_range(639, SCREEN2H - 1, map2d_x, 0, map2d_x + map2d->w - 1, map2d->h - 1);
			local.deselect();

			CHANGE = 1;
		}

		if (CHANGE) { //!!build screen
			clear(screen2);
			//map->show2d(0, 0);
			blit(map2d, screen2, 0, 0, map2d_x, 0, map2d->w, map2d->h);

			local.print(xcom1_color(1));
			//remote.print(1);
			remote.print_simple(xcom1_color(1));

			//if (local.SEND)
			// draw_pd_info(editor->pd_local, local.gx, 170);
			//if (remote.SEND)
			// draw_pd_info(pd_remote, remote.gx, 170);


			draw_sprite(screen2, mouser, mouse_x, mouse_y);
			blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);

			CHANGE = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) //left
		{
			mouse_leftr = 0;
			CHANGE = 1;
			local.execute(map, map_change_allowed);

			//	"START"
			if (mouse_inside(local.gx + 5 * 8 - 20, SCREEN2H - 20, local.gx + 5 * 8 + 20, SCREEN2H - 5))
			{
				if (F10ALLOWED && local.SEND && askmenu("FINISH")) DONE = 1;
			}
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			CHANGE = 1;
			local.execute_right();
		}

		if (!(mouse_b & 1)) {
			mouse_leftr = 1;
//			CHANGE = 1;
		}

		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
//			CHANGE = 1;
		}

		process_keyswitch();

		if (keypressed()) {
			CHANGE = 1;

			int scancode;
			int keycode = ureadkey(&scancode);

			switch (scancode) {
				case KEY_F1:
					if (FLAGS & F_RAWMESSAGES) {
						FLAGS &= ~F_RAWMESSAGES;
					} else {
						FLAGS |= F_RAWMESSAGES;
					}
					break;
				case KEY_F9:
					keyswitch(0);
					break;
				case KEY_PLUS_PAD:
					FS_IncMusicVolume();
					break;
				case KEY_MINUS_PAD:
					FS_DecMusicVolume();
					break;
				case KEY_F10:
					change_screen_mode();
					break;
				case KEY_ESC:
					if (askmenu("EXIT GAME")) {
						net->send_quit();
						net->SEND = 0;
						DONE = 1;
					}
					break;
				default:
					if (g_console->process_keyboard_input(keycode, scancode))
						net->send_message((char *)g_console->get_text());
			}
		}
		if (FINISH_PLANNER)
			break;
	}

	delete map;

	destroy_bitmap(screen2);
	screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);
	fade_out(10);
	clear(screen);
	return net->SEND;
}



void Connect::draw_pd_info(void *_pd, int gx, int gy)
{
	PLAYERDATA *pd = (PLAYERDATA *)_pd;

	textout(screen2, g_small_font, "INFO", gx, gy, xcom1_color(1));
	int i;
	int points = 0;
	for (i = 0; i < pd->size; i++) {
		points += pd->md[i].TimeUnits +
		          pd->md[i].Health +
		          pd->md[i].Firing +
		          pd->md[i].Throwing;
	}
	textprintf(screen2, g_small_font, gx, gy + 10, xcom1_color(1), "total men points=%d", points);
}
