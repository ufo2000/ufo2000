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
#include <stdio.h>
#include "video.h"
#include "icon.h"
#include "sound.h"
#include "explo.h"
#include "map.h"
#include "multiplay.h"
#include "platoon.h"
#include "soldier.h"
#include "wind.h"
#include "config.h"

extern void next_turn(int crc);

void cprintf(char *str)
{
	text_mode(0); textprintf(screen, font, 1, 1, xcom1_color(1), "%s", str);
}


Icon::Icon(int x2, int y2)
{
	x = x2; y = y2;
	tac00 = new SPK("ufograph/tac00.scr");

	SPK *spk = new SPK("ufograph/icons.pck");
	BITMAP *image = create_bitmap(320, 200); clear(image);
	spk->show(image, 0, 0);
	delete(spk);

	iconsbmp = create_bitmap(320, 57);
	blit(image, iconsbmp, 0, 144, 0, 0, iconsbmp->w, iconsbmp->h);
	destroy_bitmap(image);
}

Icon::~Icon()
{
	destroy_bitmap(iconsbmp);
	delete(tac00);
}

void Icon::draw()
{
	blit(iconsbmp, screen2, 0, 0, x, y, iconsbmp->w, iconsbmp->h);
	info();
}


#define AUTO   0
#define SNAP   1
#define AIMED  2

#define APUNCH 0
#define ATHROW 1

bool firemenu_dialog_proc_exit = 0;

int firemenu_dialog_proc(int msg, DIALOG * d, int c)
{
	if (mouse_b & 2) {
		while (mouse_b & 2) yield_timeslice();
		firemenu_dialog_proc_exit = 1;
		return D_CLOSE;
	}
	return d_button_proc(msg, d, c);
}

void Icon::firemenu(int iplace)
{
	TARGET = 0;

	if (sel_man == NULL)
		return ;
	if (sel_man->ismoving())
		return ;

	Item *it = sel_man->item(iplace);
	if (it == NULL)
		return ;

	static char dstr[5][100];
	static DIALOG the_dialog[] = {
	                                 //(dialog proc)  (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags)  (d1) (d2)	 (dp)					(dp2) (dp3)
	                                 { firemenu_dialog_proc, 41, 150, 237, 25, 1, 0, 0, D_EXIT, 0, 0, (void *)dstr[0], NULL, NULL},
	                                 { firemenu_dialog_proc, 41, 120, 237, 25, 1, 0, 0, D_EXIT, 0, 0, (void *)dstr[1], NULL, NULL},
	                                 { firemenu_dialog_proc, 41, 90, 237, 25, 1, 0, 0, D_EXIT, 0, 0, (void *)dstr[2], NULL, NULL},
	                                 { firemenu_dialog_proc, 41, 60, 237, 25, 1, 0, 0, D_EXIT, 0, 0, (void *)dstr[3], NULL, NULL},
	                                 { firemenu_dialog_proc, 41, 30, 237, 25, 1, 0, 0, D_EXIT, 0, 0, (void *)dstr[4], NULL, NULL},
	                                 { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	                             };


	for (int d = 0; d < 5; d++) {
		the_dialog[d].x = (SCREEN2W - 237) / 2;
		the_dialog[d].y = y + 6 - d * 30;
	}

	int waccur[5], wtime[5];
	Action waction[5];

	if (it != NULL) {
		int i = 0;
		sprintf(dstr[i], "CANCEL");
		the_dialog[i].proc = firemenu_dialog_proc;
		i++;

		waccur[i] = sel_man->TAccuracy(it->data()->throw_accuracy);
		wtime[i] = sel_man->required(25);
		if (sel_man->havetime(wtime[i])) {
			sprintf(dstr[i], "THROW       ACC>%02d%% TUS>%02d", waccur[i], wtime[i]);
			the_dialog[i].proc = firemenu_dialog_proc;
			waction[i] = THROW;
			i++;
		}

		if (it->is_cold_weapon()) {
			waccur[i] = sel_man->TAccuracy(it->data()->accuracy[APUNCH]);
			wtime[i] = sel_man->required(25);
			if (sel_man->havetime(wtime[i])) {
				sprintf(dstr[i], "PUNCH       ACC>%02d%% TUS>%02d", waccur[i], wtime[i]);
				the_dialog[i].proc = firemenu_dialog_proc;
				waction[i] = PUNCH;
				i++;
			}
			if (it->is_knife()) {
				waccur[i] = sel_man->TAccuracy(it->data()->accuracy[ATHROW]);
				wtime[i] = sel_man->required(50);
				if (sel_man->havetime(wtime[i])) {
					sprintf(dstr[i], "AIMED THROW ACC>%02d%% TUS>%02d", waccur[i], wtime[i]);
					the_dialog[i].proc = firemenu_dialog_proc;
					waction[i] = AIMEDTHROW;
					i++;
				}
			}
		} else
			if (it->is_grenade()) {
				wtime[i] = sel_man->required(24);
				if (sel_man->havetime(wtime[i])) {
					if (it->is_explo()) {
						if (!it->delay_time()) {
							sprintf(dstr[i], "PRIME EXPLOSIVE     TUS>%02d", wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = PRIME;
							i++;
						}
					} else {
						sprintf(dstr[i], "PRIME GRENADE       TUS>%02d", wtime[i]);
						the_dialog[i].proc = firemenu_dialog_proc;
						waction[i] = PRIME;
						i++;
					}
				}
			} else {
				if ((it->data()->isGun && it->haveclip()) || it->is_laser()) {
					if (it->data()->accuracy[AUTO]) {
						waccur[i] = sel_man->FAccuracy(it->data()->accuracy[AUTO], it->data()->twoHanded);
						wtime[i] = sel_man->required(it->data()->time[AUTO]);
						if (sel_man->havetime(wtime[i])) {
							sprintf(dstr[i], "AUTO SHOT   ACC>%02d%% TUS>%02d", waccur[i], (wtime[i] + 2) / 3 * 3);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = AUTOSHOT;
							wtime[i] = (wtime[i] + 2) / 3;      // per 3
							i++;
						}
					}
					if (it->data()->accuracy[SNAP]) {
						waccur[i] = sel_man->FAccuracy(it->data()->accuracy[SNAP], it->data()->twoHanded);
						wtime[i] = sel_man->required(it->data()->time[SNAP]);
						if (sel_man->havetime(wtime[i])) {
							sprintf(dstr[i], "SNAP SHOT   ACC>%02d%% TUS>%02d", waccur[i], wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = SNAPSHOT;
							i++;
						}
					}
					if (it->data()->accuracy[AIMED]) {
						waccur[i] = sel_man->FAccuracy(it->data()->accuracy[AIMED], it->data()->twoHanded);
						wtime[i] = sel_man->required(it->data()->time[AIMED]);
						if (sel_man->havetime(wtime[i])) {
							sprintf(dstr[i], "AIMED SHOT  ACC>%02d%% TUS>%02d", waccur[i], wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = AIMEDSHOT;
							i++;
						}
					}
				}
			}
		if (i > 1) {
			the_dialog[i].proc = NULL;
			//set_mouse_sprite(mouser);
			firemenu_dialog_proc_exit = 0;
			int sel = do_dialog(the_dialog, -1);
			//if (sel == i) return;
			if (firemenu_dialog_proc_exit || sel == 0) return ;

			if (waction[sel] != PRIME) {
				target.accur = waccur[sel];
				target.time = wtime[sel];
				target.action = waction[sel];
				target.item = it;
				target.place = iplace;
				TARGET = 1;
			} else {
				target.time = wtime[sel];
				int delay_time = doprime(it);
				if (delay_time > 0)
					sel_man->prime_grenade(iplace, delay_time, target.time);
			}
		}
	}
}


int Icon::doprime(Item *it)
{
	int DX = (SCREEN2W - 98) / 2;
	int DY = y - 119;
	int DS = 18;

	static DIALOG dPrime[] = {
	                             // (dialog proc)  (x)			 (y)			 (w)  (h) (fg) (bg) (key) (flags)  (d1)  (d2)  (dp)					(dp2) (dp3)
	                             { d_button_proc, DX, DY - 17, 98, 15, 1, 0, 0, D_EXIT, 0, 0, (void *)"Cancel", NULL, NULL},
	                             { d_button_proc, DX, DY, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"0", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 1, DY, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"1", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 2, DY, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"2", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 3, DY, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"3", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 4, DY, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"4", NULL, NULL},
	                             { d_button_proc, DX, DY + (DS + 2) * 1, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"5", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 1, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"6", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 1, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"7", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 1, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"8", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 1, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"9", NULL, NULL},
	                             { d_button_proc, DX, DY + (DS + 2) * 2, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"10", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 2, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"11", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 2, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"12", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 2, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"13", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 2, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"14", NULL, NULL},
	                             { d_button_proc, DX, DY + (DS + 2) * 3, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"15", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 3, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"16", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 3, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"17", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 3, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"18", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 3, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"19", NULL, NULL},
	                             { d_button_proc, DX, DY + (DS + 2) * 4, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"20", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 4, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"21", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 4, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"22", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 4, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"23", NULL, NULL},
	                             { d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 4, DS, DS, 1, 0, 0, D_EXIT, 0, 0, (void *)"24", NULL, NULL},
	                             { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	                         };

	return do_dialog(dPrime, -1);
}


void Icon::execute(int mx, int my)
{
	int n;
	mx -= x; my -= y;

	if (mx < 48) {
		if ((mx >= 8) && (mx <= 39) && (my >= 4) && (my <= 52)) { //cprintf("left");
			firemenu(P_ARM_LEFT);
		}
	} else if (mx > 271) {
		if ((mx >= 280) && (mx <= 311) && (my >= 4) && (my <= 52)) { //cprintf("right");
			firemenu(P_ARM_RIGHT);
		}
	} else if (my < 32) {
		n = (mx - 52) / 31;
		if (my < 16) {
			switch (n) {
				case 0: cprintf("0");
					break;
				case 1:      //cprintf("1");
					if (map->sel_lev < map->level - 1)
						map->sel_lev++;
					break;
				case 2:      //cprintf("2"); //map2d
					MODE = MAP2D;
					break;
				case 3:      //cprintf("3"); //soldier manage
					TARGET = 0;
					if ((sel_man != NULL) && (!sel_man->ismoving())) {
						MODE = MAN;
					}
					break;
				case 4:      //cprintf("4"); //next
					TARGET = 0;
					if ((sel_man != NULL) && (!sel_man->ismoving())) {
						Soldier * s = sel_man;
						sel_man = platoon_local->next_not_moved_man(sel_man);
						if (s != sel_man)
							map->center(sel_man);
					}
					break;
				case 5:      //cprintf("5");
					if (FLAGS & F_SHOWLEVELS) {
						FLAGS &= ~F_SHOWLEVELS;
					} else {
						FLAGS |= F_SHOWLEVELS;
					}
					break;
				case 6:      //cprintf("6"); //eot
					TARGET = 0;
					if (platoon_local->nomoves()) {
						next_turn( -1);
					}
					break;
			}
		} else {
			switch (n) {
				case 0: cprintf("7");
					break;
				case 1:      //cprintf("8");
					if (map->sel_lev > 0)
						map->sel_lev--;
					break;
				case 2:      //cprintf("9");	//sit/stand
					if ((sel_man != NULL) && (!sel_man->ismoving())) {
						sel_man->change_pose();
					}
					break;
				case 3:      //cprintf("a"); //center
					if (sel_man != NULL) {
						map->center(sel_man);
					}
					break;
				case 4:      //cprintf("b"); //next !moved
					TARGET = 0;
					if ((sel_man != NULL) && (!sel_man->ismoving())) {
						sel_man->MOVED = 1;
						Soldier *s = sel_man;
						sel_man = platoon_local->next_not_moved_man(sel_man);
						if (s != sel_man)
							map->center(sel_man);
					}
					break;
				case 5:    //cprintf("c");
					configure();
					break;
				case 6:      //cprintf("d");
					//quitmenu();
					simulate_keypress(KEY_ESC << 8);
					break;

			}
		}
	} else {
		n = (mx - 48) / 29;
		if (n == 2) {
			if (sel_man != NULL)
				MODE = UNIT_INFO;
		}

		if (my < 44) {
			if (n == 0) {
				cprintf("_0");

			} else if (n == 1) {
				cprintf("_1");

			}
		} else {
			if (n == 0) {
				cprintf("_2");

			} else if (n == 1) {
				cprintf("_3");

			}
		}
	}
}

void Icon::info()
{
	text_mode(-1);
	if (sel_man != NULL) {
		sel_man->drawinfo(x, y);
	}
	textprintf(screen2, font, x + 112, y + 41, xcom1_color(1), "%02d", turn);
}

int Icon::inside(int mx, int my)
{
	if ((mx >= x) && (mx <= x + 320) && (my >= y) && (my <= y + 56))
		return 1;
	else
		return 0;
}

void Icon::show_eot()
{
	tac00->show_strech(screen, 0, 0, 640, 400);
	alert(" ", "  NEXT TURN  ", " ", "    OK    ", NULL, 1, 0);
}
