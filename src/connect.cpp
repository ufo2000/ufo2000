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
#include "netsock.h"
#include "multiplay.h"
#include "connect.h"
#include "platoon.h"
#include "units.h"
#include "version.h"

int MAP_WIDTH = 5, MAP_HEIGHT = 5;

Wind *local_win = NULL, *remote_win = NULL, *info_win = NULL;

#define DX  200
#define DY  150
#define DW  240
#define OKBUT  3
#define CANBUT 4
DIALOG Connect::hostaddr_dialog[] = {
	//(dialog proc)		(x)		 (y)	(w)	(h)  (fg)  (bg)  (key)		 (flags)	(d1)  (d2)							(dp)  (dp2)	(dp3)
	{ d_shadow_box_proc, DX + 0, DY + 0, DW, 80, 0, 1, 0, 0, 0, 0, NULL, NULL, NULL },
	{ d_ctext_proc, DX + DW / 2, DY + 10, DW, 10, 0, 1, 0, 0, 0, 0, (void *)"Enter IP addres", NULL, NULL },
	{ d_edit_proc, DX + 10, DY + 30, 128, 10, 0, 1, 0, 0, 15, 0, (void *)HOSTNAME, NULL, NULL },
	{ d_button_proc, DX + 100, DY + 55, 60, 18, 0, 1,       /* 13*/0, D_EXIT, 0, 0, (void *)"OK", NULL, NULL },
	{ d_button_proc, DX + 170, DY + 55, 60, 18, 0, 1,       /* 27*/0, D_EXIT, 0, 0, (void *)"Cancel", NULL, NULL },
	{ NULL }
	//{ d_edit_proc,		DX,	 DY,	128,	9,	1,	 0,	  0,		0,	  15,	 0,	 (void *)HOSTNAME, NULL,	NULL},
	//{ NULL,				  0,	  0,	  0,	0,	0,	 0,	  0,		0,		0,	 0,	 NULL,				 NULL,	NULL}
};

void Connect::new_mapdata()
{
	if (MAP_WIDTH * MAP_HEIGHT > 36) {
		MAP_WIDTH = 5;
		MAP_HEIGHT = 5;
	}

	/*memset(MAPDATA, 0, 92);
	MAPDATA[0] = MAP_WIDTH;
	MAPDATA[2] = MAP_HEIGHT;
	MAPDATA[4] = 4; //!!!!
	MAPDATA[0xA] = 1; //cultivat
	for (int i=0xC; i<0xC+MAP_WIDTH*MAP_HEIGHT; i++) {
		if (rand()%5 == 0)
			MAPDATA[i] = 0x0d;
		else
			MAPDATA[i] = rand()%19;
	}
	char str[100];
	sprintf(str, "generated map %dx%d\n", MAP_WIDTH, MAP_HEIGHT);
	info_win->printstr(str);
	net->send_raw(str);*/
}

void Connect::send_mapdata()
{

	/*char pkt[111];
	memset(pkt, 0, 111);
	for (int i=0; i<0x30; i++)
		pkt[i] = MAPDATA[i] + 'A';
	char crcpkt[222];
	sprintf(crcpkt, "GEODATA crc=%d %s ", crc16(pkt), pkt);
	net->send_raw(crcpkt);
	info_win->printstr("geodata send\n");*/
}

void Connect::recv_mapdata(char *crcpkt)
{
	/*int crc=0;
	char pkt[111];
	sscanf(crcpkt, "GEODATA crc=%d %s ", &crc, pkt);
	if (crc == crc16(pkt)) {
		for (int i=0; i<0x30; i++)
			MAPDATA[i] = pkt[i] - 'A';
		info_win->printstr("geodata received\n");
	} else {
		info_win->printstr("geodata wrong crc\n");
	}*/
}

void Connect::new_mendata()
{
	/*int i;
	pkt.create("MENDATA");

	pkt << P1S;
	pkt << P1Z;
	pkt << P1X;
	pkt << P1Y;
	delete mandata1;
	mandata1 = new MANDATA[P1S];
	for(i=0; i<P1S; i++) {
		mandata1[i].m_maxtime = rand()%50+50;
		mandata1[i].m_maxstamina = rand()%50+50;
		mandata1[i].m_maxhealth = rand()%50+50;
		mandata1[i].m_maxmorale = rand()%50+50;
		mandata1[i].m_firing_accuracy = rand()%50+50;
		mandata1[i].m_throwing_accuracy = rand()%50+50;
		mandata1[i].m_sex = rand()%2;
		mandata1[i].m_body = rand()%4;

		pkt << mandata1[i].m_maxtime;
		pkt << mandata1[i].m_maxstamina;
		pkt << mandata1[i].m_maxhealth;
		pkt << mandata1[i].m_maxmorale;
		pkt << mandata1[i].m_firing_accuracy;
		pkt << mandata1[i].m_throwing_accuracy;
		pkt << mandata1[i].m_sex;
		pkt << mandata1[i].m_body;
	}

	pkt << P2S;
	pkt << P2Z;
	pkt << P2X;
	pkt << P2Y;
	delete mandata2;
	mandata2 = new MANDATA[P2S];
	for(i=0; i<P2S; i++) {
		mandata2[i].m_maxtime = rand()%50+50;
		mandata2[i].m_maxstamina = rand()%50+50;
		mandata2[i].m_maxhealth = rand()%50+50;
		mandata2[i].m_maxmorale = rand()%50+50;
		mandata2[i].m_firing_accuracy = rand()%50+50;
		mandata2[i].m_throwing_accuracy = rand()%50+50;
		mandata2[i].m_sex = rand()%2;
		mandata2[i].m_body = rand()%4;

		pkt << mandata2[i].m_maxtime;
		pkt << mandata2[i].m_maxstamina;
		pkt << mandata2[i].m_maxhealth;
		pkt << mandata2[i].m_maxmorale;
		pkt << mandata2[i].m_firing_accuracy;
		pkt << mandata2[i].m_throwing_accuracy;
		pkt << mandata2[i].m_sex;
		pkt << mandata2[i].m_body;
	}
	char str[100];
	sprintf(str, "mendata generated: p1=%d p2=%d\n", P1S, P2S);
	info_win->printstr(str);
	*/
}

void Connect::send_mendata()
{
	/*
	//char str[1000]; memset(str, 0, sizeof(str));
	//sprintf(str, "crc=%d %s", crc16(pkt.str()), pkt.str());

	//net->send_raw(str);
	net->send_raw(pkt.str());
	info_win->printstr("mendata send\n");
	//local_win->printstr(pkt.str());
	*/
}

void Connect::recv_mendata(char *_str)
{
	/*
	int i;
	pkt.str2pkt(_str); //!!!super
	pkt.set_cur(8);	//"MENDATA_"

	pkt >> P1S;
	pkt >> P1Z;
	pkt >> P1X;
	pkt >> P1Y;
	delete mandata1;
	mandata1 = new MANDATA[P1S];
	for(i=0; i<P1S; i++) {
		pkt >> mandata1[i].m_maxtime;
		pkt >> mandata1[i].m_maxstamina;
		pkt >> mandata1[i].m_maxhealth;
		pkt >> mandata1[i].m_maxmorale;
		pkt >> mandata1[i].m_firing_accuracy;
		pkt >> mandata1[i].m_throwing_accuracy;
		pkt >> mandata1[i].m_sex;
		pkt >> mandata1[i].m_body;
	}


	pkt >> P2S;
	pkt >> P2Z;
	pkt >> P2X;
	pkt >> P2Y;
	delete mandata2;
	mandata2 = new MANDATA[P2S];
	for(i=0; i<P2S; i++) {
		pkt >> mandata2[i].m_maxtime;
		pkt >> mandata2[i].m_maxstamina;
		pkt >> mandata2[i].m_maxhealth;
		pkt >> mandata2[i].m_maxmorale;
		pkt >> mandata2[i].m_firing_accuracy;
		pkt >> mandata2[i].m_throwing_accuracy;
		pkt >> mandata2[i].m_sex;
		pkt >> mandata2[i].m_body;
	}
	char str[100];
	sprintf(str, "mendata received: p1=%d p2=%d\n", P1S, P2S);
	info_win->printstr(str);
	*/
}


int Connect::do_chat()
{
	bool version_check_passed = false;

	reset_video();

	set_mouse_range(0, 0, 640, 400);
	position_mouse(320, 200);

	BITMAP *scr = create_bitmap(320, 200); clear(scr);
	BITMAP *backscr = create_bitmap(640, 400);
	SPK *back09 = new SPK("geograph/back09.scr");      //gamepal used

	install_int_ex(drawit_timer, BPS_TO_TIMER(10));      //ticks each second

	back09->show(scr, 0, 0);
	stretch_blit(scr, screen, 0, 0, 320, 200, 0, 0, 640, 400);
	stretch_blit(scr, backscr, 0, 0, 320, 200, 0, 0, 640, 400);

	local_win = new Wind(backscr, 15, 197, 619, 383, 16);
	remote_win = new Wind(backscr, 15, 12, 408, 179, 33);
	info_win = new Wind(backscr, 434, 17, 619, 171, 192);

	int DONE = 0;
	char buf[10000]; buf[0] = 0;

	switch (net->gametype) {
		case SOCK:
			if (!HOST) {
				do {
					if (popup_dialog(hostaddr_dialog, 2) != OKBUT) {
						net->SEND = 0;
						goto g_return;
					}
				} while (!initsocketgame());
			} else
				if (!initsocketgame()) {
					net->SEND = 0;
					goto g_return;
				}
			break;
		case HOTSEAT:
			//inithotseatgame();
			break;
		default:
			assert(false);
			break;
	}

	remote_win->printstr("\n");
	remote_win->printstr("Comparing local and remote UFO2000 versions...\n");
	remote_win->printstr("Press ESC to cancel\n");

	char version_check_packet[128];
	sprintf(version_check_packet, "UFO2000 REVISION OF YOUR OPPONENT: %d\r\n", UFO_REVISION_NUMBER);
	net->send_raw(version_check_packet);

	while (!DONE) {
		if (net->recv_raw(buf)) {
			int remote_revision;
			if (sscanf(buf, "UFO2000 REVISION OF YOUR OPPONENT: %d", &remote_revision) == 1) {
				if (UFO_REVISION_NUMBER == remote_revision) {
					net->send_raw("START\r\n");
					version_check_passed = true;
				} else {
					if (remote_revision < UFO_REVISION_NUMBER) {
						net->send_raw("UFO2000 VERSION CHECK FAILED!\r\n");
						net->send_raw("YOU UFO2000 VERSION IS OUTDATED\r\n");
						net->send_raw("\r\n");
						net->send_raw("PLEASE VISIT http://ufo2000.sourceforge.net\r\n");
						net->send_raw("AND UPGRADE YOUR UFO2000 VERSION\r\n");
						net->send_raw("\r\n");
						remote_win->printstr("\nUnfortunately your opponent has an\n");
						remote_win->printstr("outdated UFO2000 version and you will be\n");
						remote_win->printstr("unable to play until he upgrades\n");
				    } else {
						remote_win->printstr("\nUnfortunately you have older UFO2000\n");
						remote_win->printstr("version than your opponent has.\n");
						remote_win->printstr("Please visit http://ufo2000.sourceforge.net\n");
						remote_win->printstr("and upgrade your UFO2000 version\n");
				    }
					net->send_raw("QUIT\r\n");
					net->SEND = 0;
					DONE = 1;
				}
			}

			if (strstr(buf, "QUIT") != NULL) {
				net->SEND = 0;
				DONE = 1;
			}
			if (strstr(buf, "START") != NULL) {
				if (!version_check_passed) {
					net->send_raw("UFO2000 VERSION CHECK FAILED!\r\n");
					net->send_raw("YOU UFO2000 VERSION IS OUTDATED\r\n");
					net->send_raw("\r\n");
					net->send_raw("PLEASE VISIT http://ufo2000.sourceforge.net\r\n");
					net->send_raw("AND UPGRADE YOUR UFO2000 VERSION\r\n");
					net->send_raw("\r\n");
					remote_win->printstr("\nUnfortunately your opponent has an\n");
					remote_win->printstr("outdated UFO2000 version and you will be\n");
					remote_win->printstr("unable to play until he upgrades\n");
					net->send_raw("QUIT\r\n");
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
			int keycode = ureadkey(&scancode);

			switch (scancode) {
				case KEY_F4:
				case KEY_F5:
					info_win->redraw_full();
					local_win->redraw();
					remote_win->redraw();
					break;
				case KEY_ESC:
					net->send_raw("QUIT\r\n");
					net->SEND = 0;
					DONE = 1;
					break;
			}
		}
	}
g_return:
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
	BITMAP *map2d = map->create_bitmap_of_map();
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

	if (HOST) {
		net->send_map_data(&mapdata);
		net->send_time_limit(g_time_limit);
	}

	while (!DONE) {

		g_console->redraw(screen, 0, SCREEN2H);

		net->check();
		if (mapdata.load_game == 77) { //new	mapdata
			g_console->print("mapdata generated");
			mapdata.load_game = 0;
			delete map;
			destroy_bitmap(map2d);
			map = new Map(mapdata);
			map2d = map->create_bitmap_of_map();
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
					/*case KEY_F2:
						  if (askmenu("SEND UNIT DATA"))
							  editor->send_Units(local);
						  break;
					case KEY_F3:
						  if (askmenu("SEND MAP DATA")) {
							  //map->send_GEODATA(&mapdata);
							  net->send_map_data(&mapdata);
						  }
						  break;
					case KEY_F4:
						  if (askmenu("NEW MAP DATA")) {
							  map->new_GEODATA(&mapdata);
							  net->send_map_data(&mapdata);
							  mapdata.load_game = 77;
						  }
						  break;*/
				case KEY_F10:
					if (F10ALLOWED)
						if ((local.SEND) && askmenu("FINISH"))
							DONE = 1;
					break;
				case KEY_F11:
					change_screen_mode();
					break;
				case KEY_ESC:
					if (askmenu("EXIT GAME")) {
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

		/*if (local.START && remote.START) {
			if (CONFIRM_FINISH_PLANNER == 0) {
				net->send_finish_planner();
				CONFIRM_FINISH_PLANNER = 1;
			}
		} else {
			CONFIRM_FINISH_PLANNER = 0;
		}*/
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
