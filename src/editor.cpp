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
#include <string.h>
#include "video.h"
#include "editor.h"
#include "pfxopen.h"

char last_unit_name[1000];
char last_map_name[1000];

int weapon[] = {
	PISTOL , PISTOL_CLIP ,
	RIFLE , RIFLE_CLIP ,
	LASER_PISTOL , LASER_GUN , HEAVY_LASER ,
	Plasma_Pistol , Plasma_Pistol_Clip ,
	Plasma_Rifle , Plasma_Rifle_Clip ,
	Heavy_Plasma , Heavy_Plasma_Clip ,
	HEAVY_CANNON , CANNON_AP_AMMO , CANNON_HE_AMMO , CANNON_I_AMMO ,
	AUTO_CANNON , AUTO_CANNON_AP_AMMO, AUTO_CANNON_HE_AMMO, AUTO_CANNON_I_AMMO ,
	ROCKET_LAUNCHER , SMALL_ROCKET , LARGE_ROCKET , INCENDIARY_ROCKET ,
	GRENADE , ALIEN_GRENADE , PROXIMITY_GRENADE , SMOKE_GRENADE ,
	HIGH_EXPLOSIVE ,
	Flare , MOTION_SCANNER , MEDI_KIT ,
	BLASTER_LAUNCHER , BLASTER_BOMB ,
	STUN_ROD , SMALL_LAUNCHER , STUN_MISSILE ,
	MIND_PROBE , PSI_AMP
};


static char weapon_in_use[] = {
	PISTOL , PISTOL_CLIP ,
	RIFLE , RIFLE_CLIP ,
	Plasma_Pistol , Plasma_Pistol_Clip ,
	Plasma_Rifle , Plasma_Rifle_Clip ,
	Heavy_Plasma , Heavy_Plasma_Clip ,
	LASER_PISTOL , LASER_GUN , HEAVY_LASER ,
	HEAVY_CANNON , CANNON_AP_AMMO , CANNON_HE_AMMO, CANNON_I_AMMO ,
	AUTO_CANNON , AUTO_CANNON_AP_AMMO , AUTO_CANNON_HE_AMMO, AUTO_CANNON_I_AMMO ,
	ROCKET_LAUNCHER , SMALL_ROCKET , LARGE_ROCKET , INCENDIARY_ROCKET ,
	GRENADE , ALIEN_GRENADE , HIGH_EXPLOSIVE , PROXIMITY_GRENADE , SMOKE_GRENADE ,
	STUN_ROD , SMALL_LAUNCHER , STUN_MISSILE ,
	KASTET , KNIFE
};

Editor::Editor()
{
	BITMAP *image = create_bitmap(320, 200); clear(image);
	tac01 = new SPK("$(xcom)/ufograph/tac01.scr");
	tac01->show(image, 0, 0);
	b123 = create_bitmap(83, 22); clear(b123);
	blit(image, b123, 45, 0, 0, 0, 83, 22);
	b4 = create_bitmap(32, 25); clear(b4);
	blit(image, b4, 224, 21, 0, 0, 32, 25);
	b5 = create_bitmap(32, 15); clear(b5);
	blit(image, b5, 288, 137, 0, 0, 32, 15);
	destroy_bitmap(image);

	m_armoury = new Place(0, 220, 20, 9);
	m_armoury->load_bin("$(home)/armoury.set");
	//armoury.put(new Item(KASTET));
	//armoury.put(new Item(KNIFE));
	/*for(int i=0; i<40; i++) {
		Item *it = new Item(weapon[i]);
		armoury.put(it);
	}*/

	if (local_platoon_size > 10) local_platoon_size = 10;      //!!!!!!!!!!!
	assert(local_platoon_size > 0);
	m_plt = new Platoon(2001, local_platoon_size);
	m_plt->load_MANDATA("$(home)/soldier.dat");
	m_plt->load_ITEMDATA("$(home)/items.dat");
	man = m_plt->captain();

	sel_item = NULL;
	dup_item = NULL;
}

Editor::~Editor()
{
	delete m_plt;
	delete tac01;
	delete m_armoury;
}

void Editor::load()
{
	char path[1000];
	//strcpy(path, "default.units");
	strcpy(path, last_unit_name);
	if (file_select("LOAD UNITS DATA", path, "UNITS")) {
		m_plt->load_FULLDATA(path);
		strcpy(last_unit_name, path);
	}
	/*
	//set_mouse_range(0, 0, 639, 399);
	strcpy(path, "soldier.dat");
	//if ( file_select("LOAD SOLDIER DATA", path, "DAT") ) {
		plt->load_MANDATA(path);
	//}
	//set_mouse_range(0, 0, 639, 399);
	//strcpy(path, "soldier_items.dat");
	strcpy(path, "items.dat");
	//if ( file_select("LOAD SOLDIER'S ITEMS DATA", path, "DAT") ) {
		plt->load_ITEMDATA(path);
	//}
	//set_mouse_range(0, 0, 639, 399);
	//armoury.load_bin("armoury.set");
	//plt->load_MANDATA("soldier.dat");
	//plt->load_ITEMDATA("soldier_items.dat");
	*/
}


void Editor::save()
{
	char path[1000];
	//strcpy(path, "default.units");.
	strcpy(path, last_unit_name);
	if (file_select("SAVE UNITS DATA", path, "UNITS")) {
		m_plt->save_FULLDATA(path);
		strcpy(last_unit_name, path);
	}

	/*
	//set_mouse_range(0, 0, 639, 399);
	strcpy(path, "soldier.dat");
	//if ( file_select("SAVE SOLDIER DATA", path, "DAT") ) {
		plt->save_MANDATA(path);
	//}
	//set_mouse_range(0, 0, 639, 399);
	//strcpy(path, "soldier_items.dat");
	strcpy(path, "items.dat");
	//if ( file_select("SAVE SOLDIER'S ITEMS DATA", path, "DAT") ) {
		plt->save_ITEMDATA(path);
	//}
	//set_mouse_range(0, 0, 639, 399);
	//armoury.save_bin("armoury.set");
	//plt->save_MANDATA("soldier.dat");
	//plt->save_ITEMDATA("soldier_items.dat");
	*/
}

int Editor::set_man(char *name)
{
	Soldier * ss = m_plt->findman(name);
	if (ss == NULL)
		return 0;
	man = ss;
	return 1;
}


void Editor::show(int NEXTPREV)
{
	reset_video();
	destroy_bitmap(screen2);
	screen2 = create_bitmap(640, 400); clear(screen2);

	position_mouse(320, 200);
	set_mouse_range(0, 0, 639, 399);

	int DONE = 0;
	int mouse_leftr = 1, mouse_rightr = 1;
	int i;
	//int color = 1;

	while (!DONE) {
		if (CHANGE) {
			clear(screen2);
			tac01->show(screen2, 0, 0);
			draw_sprite_h_flip(screen2, b5, 255, 137);
			man->showspk();

			textout(screen2, large, man->md.Name, 0, 0, xcom1_color(66));
			for (i = 0; i < NUMBER_OF_PLACES; i++) //man->drawgrid();
				man->place(i)->drawgrid(i);
			m_armoury->drawgrid(P_ARMOURY);

			text_mode( -1);
			textout(screen2, large, "F2 Save   F3 Load", 8, 380, xcom1_color(130));

			man->draw_unibord(320, 0);

			if (sel_item != NULL) {
				if (dup_item != NULL)
					sel_item->od_info(330, 220, xcom1_color(1));
				else
					sel_item->od_info(330, 220, xcom1_color(69));

				//textprintf(screen2, font, 129, 141, color, "%s", sel_item->data()->name);
				if (dup_item == NULL)
					textprintf(screen2, g_small_font, 128, 140, xcom1_color(50), "%s", sel_item->data()->name);
				else
					textprintf(screen2, g_small_font, 128, 208, xcom1_color(3), "%s", sel_item->data()->name);

				if (sel_item->haveclip()) {
					//textprintf(screen2, font, 272, 80, color, "%d", sel_item->roundsremain());
					textout(screen2, g_small_font, "AMMO:", 272, 64, xcom1_color(66));
					textout(screen2, g_small_font, "ROUNDS", 272, 72, xcom1_color(66));
					textout(screen2, g_small_font, "LEFT=", 272, 80, xcom1_color(66));
					textprintf(screen2, g_small_font, 299, 80, xcom1_color(18), "%d", sel_item->roundsremain());
					rect(screen2, 272, 88, 303, 135, xcom1_color(8));      //clip
					bigobs->showpck(sel_item->clip()->data()->pInv, 272, 88 + 8);
				} else if (sel_item->data()->isAmmo) {
					//textprintf(screen2, font, 272, 80, color, "%d", sel_item->rounds);
					textout(screen2, g_small_font, "AMMO:", 272, 64, xcom1_color(66));
					textout(screen2, g_small_font, "ROUNDS", 272, 72, xcom1_color(66));
					textout(screen2, g_small_font, "LEFT=", 272, 80, xcom1_color(66));
					textprintf(screen2, g_small_font, 299, 80, xcom1_color(18), "%d", sel_item->m_rounds);
					rect(screen2, 272, 88, 303, 135, xcom1_color(8));      //clip
					bigobs->showpck(sel_item->data()->pInv, 272, 88 + 8);
				}
				bigobs->showpck(sel_item->data()->pInv,
				                mouse_x - sel_item->data()->width * 16 / 2,
				                mouse_y - sel_item->data()->height * 16 / 2 + 8);
			} else {
				Item *it = m_armoury->item_under_mouse();
				if (it != NULL) {
					if (memchr(weapon_in_use, it->m_type, sizeof(weapon_in_use)) != NULL)
						it->od_info(330, 220, xcom1_color(5));
					else
						it->od_info(330, 220, xcom1_color(10));
				}
			}

			draw_sprite(screen2, mouser, mouse_x, mouse_y);
			blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
			CHANGE = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			mouse_leftr = 0;
			CHANGE = 1;

			if (mouse_inside(320, 0, 639, 200)) // unibord
				edit_soldier();

			if ((sel_item == NULL) || (dup_item != NULL)) {
				if (mouse_inside(237, 1, 271, 22)) {  //ok
					DONE = 1;
				} else
					if (mouse_inside(273, 1, 295, 22)) {  // <
						//if (NEXTPREV)
						man = man->prevman();
					} else
						if (mouse_inside(297, 1, 319, 22)) {  // >
							//if (NEXTPREV)
							man = man->nextman();
						}
			}

			if (mouse_inside(288, 32, 319, 57)) {  // clip
				//if ((dup_item == NULL) && (man->unload_ammo(sel_item)))
				//	sel_item = NULL;
				if ((dup_item == NULL) && (sel_item != NULL) && (sel_item->haveclip()) &&
				        (man->rhand_item() == NULL) && (man->lhand_item() == NULL)) {
					man->putitem(sel_item, P_ARM_RIGHT);
					man->putitem(sel_item->unload(), P_ARM_LEFT);
					sel_item = NULL;
				}
			} else
				if (mouse_inside(288, 137, 319, 151)) {  // -->
					man->place(P_MAP)->scroll_right();
				} else
					if (mouse_inside(255, 137, 286, 151)) {  // <--
						man->place(P_MAP)->scroll_left();
					}

			if (sel_item == NULL) {
				for (i = 0; i < NUMBER_OF_PLACES; i++) {
					sel_item = man->place(i)->mselect();
					if (sel_item != NULL) {
						sel_item_place = i;
						break;
					}
				}
				if (sel_item == NULL) {
					sel_item = m_armoury->mselect();
					if (sel_item != NULL) {
						if (memchr(weapon_in_use, sel_item->m_type, sizeof(weapon_in_use)) != NULL) {
							sel_item_place = 9;
							dup_item = new Item(sel_item->m_type);      //!!!!!!!!!!!!
						} else {
							m_armoury->put(sel_item, sel_item->m_x, sel_item->m_y);
							sel_item = NULL;
						}
					}
				}
			} else {
				if (dup_item != NULL) {
					for (i = 0; i < NUMBER_OF_PLACES; i++) {
						if (man->place(i)->mdeselect(dup_item)) {
							dup_item = new Item(sel_item->m_type);      //!!!!!!!!!!!!
							break;
						}
					}
				} else {
					for (i = 0; i < NUMBER_OF_PLACES; i++) {
						if (man->place(i)->mdeselect(sel_item)) {
							sel_item = NULL;
							break;
						}
					}
				}

				if (sel_item != NULL) {
					if (dup_item != NULL) {
						if (m_armoury->mdeselect(sel_item)) {
							sel_item = NULL;
							delete dup_item;
							dup_item = NULL;
						}
					} else {
						if (m_armoury->mdeselect(sel_item)) {
							Item *del = m_armoury->get(sel_item->m_x, sel_item->m_y);
							assert(del != NULL);
							delete del;
							sel_item = NULL;
						}
					}
				}
			}
			if (sel_item != NULL)
				load_clip();
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			CHANGE = 1;
			if (sel_item != NULL) {
				if (sel_item_place == 9) {
					m_armoury->put(sel_item, sel_item->m_x, sel_item->m_y);
					delete dup_item;
					dup_item = NULL;
				} else
					man->putitem(sel_item, sel_item_place, sel_item->m_x, sel_item->m_y);

				sel_item = NULL;
			} else {
				for (i = 0; i < NUMBER_OF_PLACES; i++) {
					sel_item = man->place(i)->mselect();
					if (sel_item != NULL) {
						break;
					}
				}
				if (sel_item != NULL) {
					delete(sel_item);
					sel_item = NULL;
				}
			}
		}

		if (!(mouse_b & 1)) {
			mouse_leftr = 1;
			CHANGE = 1;
		}

		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
			CHANGE = 1;
		}

		//readkey();
		if (keypressed()) {
			int c = readkey();
			switch (c >> 8) {
				case KEY_F1:
					//edit_soldier();
					if (askmenu("SAVE ARMOURY")) {
						m_armoury->save_bin("$(home)/armoury.set");
					}
					break;
				case KEY_F2:
					//if (askmenu("SAVE DATA")) {
					save();
					//}
					break;
				case KEY_F3:
					//if (askmenu("LOAD DATA")) {
					load();
					//}
					break;
				case KEY_ESC:
					DONE = 1;
					break;
			}
		}
	}
	//save(); //!!!!!!!!!!!!!!!!!!
	
	m_plt->save_MANDATA(F("$(home)/soldier.dat"));
	m_plt->save_ITEMDATA(F("$(home)/items.dat"));

	destroy_bitmap(screen2);
	screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);

	fade_out(10);
	clear(screen);
}


int Editor::load_clip()
{
	Item * it;
	int i;

	if (dup_item != NULL) {
		for (i = 0; i < NUMBER_OF_PLACES; i++) {
			it = man->place(i)->item_under_mouse();
			if ((it != NULL) && it->loadclip(dup_item)) {
				dup_item = new Item(sel_item->m_type);      //!!!!!!!!!!!!
				return 1;
			}
		}
	} else {
		for (i = 0; i < NUMBER_OF_PLACES; i++) {
			it = man->place(i)->item_under_mouse();
			if ((it != NULL) && it->loadclip(sel_item)) {
				sel_item = NULL;
				return 1;
			}
		}
	}
	return 0;
}

#define DX  140
#define DY  65
#define FG  0
#define BG  1
#define SSX 115
#define STX 45
#define SSY 40
#define STY SSY+4
#define SSW 170
#define STW 65
#define SH  20

#define D_NAME         3 
#define D_POINTS       4
#define D_ICON         6
#define D_TIME         8
#define D_HEALTH       D_TIME+2
#define D_STAMINA      D_TIME+4
#define D_REACTION     D_TIME+6
#define D_STRENGTH     D_TIME+8
#define D_FIRE_ACCUR   D_TIME+10
#define D_THRU_ACCUR   D_TIME+12
#define D_BRAVERY      D_TIME+14

static int d_slider_pro2(int msg, DIALOG *d, int c);
static char slider_text[8][14];

static int d_skin_proc(int msg, DIALOG *d, int c);

static int points;
static char points_str[100];

DIALOG sol_dialog[] = {
	//(dialog proc)      (x)           (y)                   (w)      (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
	{ d_shadow_box_proc, DX,           DY,                   360,     280, FG,  BG, 0, 0, 0, 0, NULL, NULL, NULL},
	{ d_button_proc,     DX + 130,     DY + SSY + SH*10 + 4, 100,     20,  FG,  BG, 0, D_EXIT, 0, 0, (void *)"OK", NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + SSY - SH*1,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)"Name:", NULL, NULL},
	{ d_edit_proc,       DX + SSX,     DY + SSY - SH*1,      23*8,    16,  FG,  BG, 0, 0, 22, 0, NULL, NULL, NULL},
	{ d_text_proc,       DX + SSW + 3, DY + SSY + SH*9 + 2,  100,     16,  FG,  BG, 0, 0, 0, 0, (void *)points_str, NULL, NULL},

	{ d_rtext_proc,      DX + STX,     DY + SSY + SH*0 - 4,  STW,     16 + 4,  FG,  -1, 0, 0, 0, 0, (void *)"Skin:", NULL, NULL},
	{ d_skin_proc,       DX + SSX - 3, DY + SSY + SH*0 - 10, SSW + 4, 16 + 4,  FG,  BG, 0, 0, 0, 0, NULL, NULL, NULL},

	{ d_rtext_proc,      DX + STX,     DY + STY + SH*1,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[0], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*1,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*2,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[1], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*2,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*3,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[2], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*3,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*4,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[3], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*4,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*5,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[4], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*5,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*6,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[5], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*6,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*7,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[6], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*7,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_rtext_proc,      DX + STX,     DY + STY + SH*8,      STW,     16,  FG,  -1, 0, 0, 0, 0, (void *)slider_text[7], NULL, NULL},
	{ d_slider_pro2,     DX + SSX,     DY + SSY + SH*8,      SSW,     16,  FG,  BG, 0, 0, 100, 33, NULL, NULL, NULL},

	{ NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

static int d_slider_pro2(int msg, DIALOG * d, int c)
{
	char s[100];
	int v;
	v = d_slider_proc(msg, d, c);
	switch (msg) {
		case MSG_DRAW:
			sprintf(s, "%3d", d->d2);
			text_mode(d->bg);
			gui_textout(screen, s, d->x + d->w, d->y + 4, d->fg, 0);
			break;
		case MSG_CLICK:
		case MSG_CHAR:
			if (d == &sol_dialog[D_STRENGTH])
			{
				if (d->d2 > 40) d->d2 = 40;
				if (d->d2 < 25) d->d2 = 25;
			}
			else
			{
				if (d->d2 > 80) d->d2 = 80;
				if (d->d2 < 50) d->d2 = 50;
			}

			points = sol_dialog[D_TIME].d2 +
			         sol_dialog[D_HEALTH].d2 +
			         sol_dialog[D_FIRE_ACCUR].d2 +
			         sol_dialog[D_THRU_ACCUR].d2 +
					 sol_dialog[D_STAMINA].d2 +
					 (sol_dialog[D_STRENGTH].d2 * 2) +
					 sol_dialog[D_REACTION].d2;

			if (points > MAXPOINTS) {
				points -= d->d2;
				d->d2 = MAXPOINTS - points;
				points = MAXPOINTS;
			}

			scare_mouse();

			SEND_MESSAGE(d, MSG_DRAW, 0);
			sprintf(points_str, "points remain %3d", MAXPOINTS - points);
			SEND_MESSAGE(&sol_dialog[D_POINTS], MSG_DRAW, 0);

			unscare_mouse();
			break;
		default:
			break;
	}
	return v;
}

int d_skin_proc(int msg, DIALOG *d, int c)
{
	if (d->d1 < 0) {
		d->d1 = d->d2;
		rectfill((BITMAP *)d->dp, 0, 0, SSW - 1, 19, BG);
		textout((BITMAP *)d->dp, font, g_skins[d->d1].Name, 2, 6, FG);
	}

	if (msg == MSG_CLICK) {
		if (++d->d1 >= g_skins_count) d->d1 = 0;
		rectfill((BITMAP *)d->dp, 0, 0, SSW - 1, 19, BG);
		textout((BITMAP *)d->dp, font, g_skins[d->d1].Name, 2, 6, FG);
	
		return d_icon_proc(MSG_DRAW, d, c);
	}

	return d_icon_proc(msg, d, c);
}

void Editor::edit_soldier()
{
//	Attributes
	sprintf(slider_text[0], "TimeUnits");
	sprintf(slider_text[1], "Health");
	sprintf(slider_text[2], "Stamina");
	sprintf(slider_text[3], "Reaction");
	sprintf(slider_text[4], "Strength");
	sprintf(slider_text[5], "Firing");
	sprintf(slider_text[6], "Throwing");
	sprintf(slider_text[7], "Bravery");

//	sol_dialog[D_STAMINA].flags  |= D_DISABLED;
//	sol_dialog[D_REACTION].flags |= D_DISABLED;
//	sol_dialog[D_STRENGTH].flags |= D_DISABLED;
	sol_dialog[D_BRAVERY].flags  |= D_DISABLED;

	sol_dialog[D_NAME].dp       = man->md.Name;
	sol_dialog[D_TIME].d2       = man->md.TimeUnits;
	sol_dialog[D_HEALTH].d2     = man->md.Health;
	sol_dialog[D_STAMINA].d2    = man->md.Stamina;
	sol_dialog[D_REACTION].d2   = man->md.Reactions;
	sol_dialog[D_STRENGTH].d2   = man->md.Strength;
	sol_dialog[D_FIRE_ACCUR].d2 = man->md.Firing;
	sol_dialog[D_THRU_ACCUR].d2 = man->md.Throwing;
	sol_dialog[D_BRAVERY].d2    = man->md.Bravery;

	BITMAP *icon = create_bitmap(SSW, 20);
	clear_bitmap(icon);

	sol_dialog[D_ICON].dp   = icon;
	sol_dialog[D_ICON].dp2  = icon;
	sol_dialog[D_ICON].d1   = -1;
	sol_dialog[D_ICON].d2   = get_skin_index(man->md.SkinType, man->md.fFemale);

	points = sol_dialog[D_TIME].d2 + sol_dialog[D_HEALTH].d2 +
	         sol_dialog[D_FIRE_ACCUR].d2 + sol_dialog[D_THRU_ACCUR].d2 +
			 sol_dialog[D_STAMINA].d2 + (sol_dialog[D_STRENGTH].d2 * 2) +
			 sol_dialog[D_REACTION].d2;
	sprintf(points_str, "points remain %3d", MAXPOINTS - points);

	position_mouse(320, 200);

	do_dialog(sol_dialog, -1);

	destroy_bitmap(icon);

	man->md.Appearance = 0;
	man->md.SkinType   = g_skins[sol_dialog[D_ICON].d1].SkinType;
	man->md.fFemale    = g_skins[sol_dialog[D_ICON].d1].fFemale;

	man->md.TimeUnits  = sol_dialog[D_TIME].d2;
	man->md.Health     = sol_dialog[D_HEALTH].d2;
	man->md.Stamina    = sol_dialog[D_STAMINA].d2;
	man->md.Reactions  = sol_dialog[D_REACTION].d2;
	man->md.Strength   = sol_dialog[D_STRENGTH].d2;
	man->md.Firing     = sol_dialog[D_FIRE_ACCUR].d2;
	man->md.Throwing   = sol_dialog[D_THRU_ACCUR].d2;
	man->md.Bravery    = sol_dialog[D_BRAVERY].d2;

	man->process_MANDATA();
}

BITMAP *create_terrain_bitmap(int terrain)
{
	unsigned char map[36] = {
	                            24, 19, 14, 9, 4,
	                            23, 18, 13, 8, 3,
	                            22, 17, 12, 7, 2,
	                            21, 16, 11, 6, 1,
	                            20, 15, 10, 5, 0
	                        };
	/*for(int i=0; i<36; i++)
		map[i] = i;*/

	Map *m = new Map(4, 5, 5, terrain, map);

	BITMAP *bmp = m->create_bitmap_of_map();
	delete m;
	return bmp;
}



int Editor::do_mapselect()
{
	//terrain_bmp = create_terrain_bitmap(1);

	int x = mouse_x, y = mouse_y;

	if (x + terrain_bmp->w >= 640)
		x = 640 - terrain_bmp->w;
	if (y + terrain_bmp->h >= 400)
		y = 400 - terrain_bmp->h;

	blit(terrain_bmp, screen, 0, 0, x, y, terrain_bmp->w, terrain_bmp->h);


	int mouse_leftr = 0, mouse_rightr = 0;
	if (!(mouse_b & 1)) mouse_leftr = 1;
	if (!(mouse_b & 2)) mouse_rightr = 1;

	//show_mouse(screen);
	//text_mode(0);

	while (!keypressed()) {
		if (CHANGE) {

			clear(screen2);

			//m_map->set_sel(mouse_x, mouse_y);
			m_map->draw();
			blit(terrain_bmp, screen2, 0, 0, x, y, terrain_bmp->w, terrain_bmp->h);
			draw_sprite(screen2, mouser, mouse_x, mouse_y);
			blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);

			//int tt = (mouse_y-y) / 40 * 5 + (mouse_x-x) / 40;
			//textprintf(screen, font, x, y-10, 1, "tt=%02d", tt);

			draw_sprite(screen, mouser, mouse_x, mouse_y);

			CHANGE = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			int tt = (mouse_y - y) / 40 * 5 + (mouse_x - x) / 40;
			if ((tt >= 0) && (tt < 25))
				return tt;

			mouse_leftr = 0;
			break;
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			break;
		}

		if (!(mouse_b & 1)) mouse_leftr = 1;
		if (!(mouse_b & 2)) mouse_rightr = 1;

	}
	//show_mouse(NULL);

	//destroy_bitmap(terrain_bmp);
	return -1;
}


void Editor::do_mapedit()
{
	platoon_local = m_plt;

	int mouse_leftr = 1, mouse_rightr = 1;
	m_map = new Map(mapdata);
	m_map->center(0, m_map->width * 5, m_map->height * 5);
	m_map->unhide();

	terrain_bmp = create_terrain_bitmap(TERRAIN_INDEX);

	reset_video();
	int ow = SCREEN2W, oh = SCREEN2H;

	//SCREEN2W = 1280; SCREEN2H = 800;
	SCREEN2W = 640; SCREEN2H = 400;
	destroy_bitmap(screen2);
	screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);

	position_mouse(320, 200);
	set_mouse_range(0, 0, 639, 399);

	//m_map->draw();

	int DONE = 0;
	while (!DONE) {
		if (CHANGE) { //////////////////////////build screen
			clear(screen2);

			m_map->set_sel(mouse_x, mouse_y);
			m_map->draw();

			text_mode( -1);
			textout(screen2, g_small_font, "F2 Save   F3 Load", 0, 0, xcom1_color(130));

			draw_sprite(screen2, mouser, mouse_x, mouse_y);
			blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
			//stretch_blit(screen2, screen, 0,0, screen2->w, screen2->h, 0,0, screen2->w/2, screen2->h/2);

			//stretch_blit(screen2, screen, 0,0, screen2->w, screen2->h, 0,0, 640, 480);

			CHANGE = 0;
		}

		if (m_map->scroll(mouse_x, mouse_y))
			CHANGE = 1;

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			mouse_leftr = 0;
			CHANGE = 1;
		}
		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			int col = m_map->sel_col / 10;
			int row = m_map->sel_row / 10;

			int v = do_mapselect();
			if (v != -1) {
				char mname[100];
				sprintf(mname, "maps/culta%02d.map", v);
				if (m_map->loadmap(mname, row * 10, col * 10))
					m_map->set_map_data(col, row, v);
			}
			CHANGE = 1;
		}

		if (!(mouse_b & 1)) {
			mouse_leftr = 1;
			CHANGE = 1;
		}

		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
			CHANGE = 1;
		}


		if (keypressed()) {
			int c = readkey();
			switch (c >> 8) {
				case KEY_F2:
					//if (askmenu("SAVE MAP")) {
					save_map();
					//}
					break;
				case KEY_F3:
					//if (askmenu("LOAD DATA")) {
					load_map();
					//}
					break;
				case KEY_F4:
					//create_map();
					break;
				case KEY_F10:
					change_screen_mode();
					break;
				case KEY_ESC:
					if (askmenu("FINISH"))
						DONE = 1;
					break;
			}
			CHANGE = 1;
		}
	}

	delete m_map;
	destroy_bitmap(terrain_bmp);

	destroy_bitmap(screen2);
	SCREEN2W = ow; SCREEN2H = oh;
	screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);

	fade_out(10);
	clear(screen);
}

//extern GEODATA mapdata;

void Editor::load_map()
{
	char path[1000];
	//strcpy(path, "geodata2.dat");
	strcpy(path, last_map_name);
	if (file_select("LOAD MAP", path, "DAT")) {
		//m_map->load(path);
		int fh = open(path, O_RDONLY | O_BINARY); // got  path from fileselector - don't do pfxopen.
		assert(fh != -1);
		read(fh, &mapdata, sizeof(mapdata));
		close(fh);

		delete m_map;
		m_map = new Map(mapdata);
		m_map->center(0, m_map->width * 5, m_map->height * 5);
		m_map->unhide();

		strcpy(last_map_name, path);
	}
}
/*
void Editor::create_map()
{
	int w = 5, h = 4;

	memset(&mapdata, 0, sizeof(mapdata));
	mapdata.x_size = w;
	mapdata.y_size = h;
	mapdata.z_size = 4;
	mapdata.terrain = TERRAIN_INDEX;

	delete m_map;
	m_map = new Map(mapdata);
	m_map->center(0, m_map->width * 5, m_map->height * 5);
	m_map->unhide();
}
*/

void Editor::save_map()
{
	char path[1000];
	//strcpy(path, "geodata2.dat");
	strcpy(path, last_map_name);
	if (file_select("SAVE MAP", path, "DAT")) {
		m_map->save(path);
		strcpy(last_map_name, path);
	}
}
