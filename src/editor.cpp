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
#include "global.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "video.h"
#include "editor.h"

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
	STUN_ROD , SMALL_LAUNCHER , STUN_MISSILE
};

/**
 * Returns true is the weapon can be used. The weapon will be colored darkgray
 * and not allowed to select if this function returns false (this function
 * may be useful when points limit is exceeded for example)
 */
static bool is_weapon_allowed(int type)
{
	return memchr(weapon_in_use, type, sizeof(weapon_in_use)) != NULL;
}

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

	lua_pushstring(L, "Armoury");
	LUA_PUSH_OBJECT_POINTER(L, Place, m_armoury);
	lua_settable(L, LUA_GLOBALSINDEX);

	lua_safe_dostring(L, "SetEquipment('Standard')");
	
	if (local_platoon_size > 10) local_platoon_size = 10;      //!!!!!!!!!!!
	ASSERT(local_platoon_size > 0);
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
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);
	
	char path[1000];
	strcpy(path, last_unit_name);
	if (file_select("LOAD UNITS DATA", path, "UNITS")) {
		m_plt->load_FULLDATA(path);
		strcpy(last_unit_name, path);
	}
	
	set_mouse_range(0, 0, 639, 399);
}


void Editor::save()
{
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);

	char path[1000];
	strcpy(path, last_unit_name);
	if (file_select("SAVE UNITS DATA", path, "UNITS")) {
		m_plt->save_FULLDATA(path);
		strcpy(last_unit_name, path);
	}
	
	set_mouse_range(0, 0, 639, 399);
}

int Editor::set_man(char *name)
{
	Soldier * ss = m_plt->findman(name);
	if (ss == NULL)
		return 0;
	man = ss;
	return 1;
}

/**
 * Handle left mouse click in soldier equipment screen
 * @returns  true if the user decided to exit from equipment screen
 */
bool Editor::handle_mouse_leftclick()
{
	int i;

	// Mouse click in unit stats area - edit unit stats	
	if (mouse_inside(320, 0, 639, 200)) {
		// unibord
		edit_soldier();
		return false;
	}
	
	// Mouse click in item info panel (right bottom part of the screen) - 
	// change equipment dialog
	if (mouse_inside(320, 200, 639, 400)) {
		change_equipment();
		return false;
	}
	
	if ((sel_item == NULL) || (dup_item != NULL)) {
		if (mouse_inside(237, 1, 271, 22)) {  //ok
			return true;
		} else
			if (mouse_inside(273, 1, 295, 22)) {  // <
				man = man->prevman();
			} else
				if (mouse_inside(297, 1, 319, 22)) {  // >
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
				if (is_weapon_allowed(sel_item->m_type)) {
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
					ASSERT(del != NULL);
					delete del;
					sel_item = NULL;
				}
			}
		}
	}
	if (sel_item != NULL)
		load_clip();
	return false;
}

/**
 * Activate unit stats and inventory edit screen
 */
void Editor::show()
{
	reset_video();
	destroy_bitmap(screen2);
    screen2 = create_bitmap(640, 400); 
    clear(screen2);
	
    // Prepare background picture for editor screen to improve 
    // performance a bit (static image that is never changed)
    BITMAP *editor_bg = create_bitmap(640, 400);
    clear_to_color(editor_bg, xcom_color(15));
    tac01->show(editor_bg, 0, 0);
    draw_sprite_h_flip(editor_bg, b5, 255, 137);
    text_mode(-1);
    textout(editor_bg, large, "F2 Save   F3 Load", 8, 380, xcom_color(130));

	position_mouse(320, 200);
	set_mouse_range(0, 0, 639, 399);

	int DONE = 0;
	int mouse_leftr = 1, mouse_rightr = 1;
	int i;

	while (!DONE) {

        rest(1); // Don't eat all CPU resources
	
		if (CHANGE) {
            blit(editor_bg, screen2, 0, 0, 0, 0, editor_bg->w, editor_bg->h);
            man->showspk();

            text_mode(-1);
			textout(screen2, large, man->md.Name, 0, 0, xcom1_color(66));
			
			for (i = 0; i < NUMBER_OF_PLACES; i++) //man->drawgrid();
				man->place(i)->drawgrid(i);
			m_armoury->drawgrid(P_ARMOURY);

			man->draw_unibord(320, 0);
			if (sel_item != NULL) {
				if (dup_item != NULL)
					sel_item->od_info(330, 220, xcom1_color(1));
				else
					sel_item->od_info(330, 220, xcom1_color(69));

				//textprintf(screen2, font, 129, 141, color, "%s", sel_item->data()->name);
				if (dup_item == NULL)
					textprintf(screen2, g_small_font, 128, 140, xcom1_color(50), "%s", sel_item->name().c_str());
				else
					textprintf(screen2, g_small_font, 128, 208, xcom1_color(3), "%s", sel_item->name().c_str());

				if (sel_item->haveclip()) {
					//textprintf(screen2, font, 272, 80, color, "%d", sel_item->roundsremain());
					textout(screen2, g_small_font, "AMMO:", 272, 64, xcom1_color(66));
					textout(screen2, g_small_font, "ROUNDS", 272, 72, xcom1_color(66));
					textout(screen2, g_small_font, "LEFT=", 272, 80, xcom1_color(66));
					textprintf(screen2, g_small_font, 299, 80, xcom1_color(18), "%d", sel_item->roundsremain());
					rect(screen2, 272, 88, 303, 135, xcom1_color(8));      //clip
					PCK::showpck(sel_item->clip()->obdata_pInv(), 272, 88 + 8);
				} else if (sel_item->obdata_isAmmo()) {
					//textprintf(screen2, font, 272, 80, color, "%d", sel_item->rounds);
					textout(screen2, g_small_font, "AMMO:", 272, 64, xcom1_color(66));
					textout(screen2, g_small_font, "ROUNDS", 272, 72, xcom1_color(66));
					textout(screen2, g_small_font, "LEFT=", 272, 80, xcom1_color(66));
					textprintf(screen2, g_small_font, 299, 80, xcom1_color(18), "%d", sel_item->m_rounds);
					rect(screen2, 272, 88, 303, 135, xcom1_color(8));      //clip
					PCK::showpck(sel_item->obdata_pInv(), 272, 88 + 8);
				}
				PCK::showpck(sel_item->obdata_pInv(),
				                mouse_x - sel_item->obdata_width() * 16 / 2,
				                mouse_y - sel_item->obdata_height() * 16 / 2 + 8);
			} else {
				Item *it = m_armoury->item_under_mouse();
				if (it != NULL) {
					if (is_weapon_allowed(it->m_type))
						it->od_info(330, 220, xcom1_color(5));
					else
						it->od_info(330, 220, xcom1_color(10));
				}
			}
			
			int wht = man->count_weight();
			int max_wht = man->md.Strength;
			int color = max_wht < wht ? xcom1_color(35) : xcom1_color(2);
			textprintf(screen2, g_small_font, 0, 20, color, "Equipment weight: %d/%d", wht, max_wht);
			textprintf(screen2, g_small_font, 120, 20, xcom1_color(2), "Equipment cost: %d", man->calc_full_ammunition_cost());

			draw_sprite(screen2, mouser, mouse_x, mouse_y);
			blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
			CHANGE = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			mouse_leftr = 0;
			CHANGE = 1;

			if (handle_mouse_leftclick())
				DONE = 1;
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
		}

		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
		}

		if (keypressed()) {
            CHANGE = 1;
			int c = readkey();
			switch (c >> 8) {
				case KEY_F1:
					//edit_soldier();
					if (askmenu("SAVE ARMOURY")) {
						m_armoury->save_to_file("$(home)/armoury.lua", "Armoury");
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
				case KEY_F10:
					change_screen_mode();
					break;
				case KEY_ESC:
					DONE = 1;
					break;
			}
		}
	}
	
	m_plt->save_MANDATA(F("$(home)/soldier.dat"));
	m_plt->save_ITEMDATA(F("$(home)/items.dat"));

    destroy_bitmap(editor_bg);
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

#define D_WIDTH 320
#define D_HEIGHT 340

#define DX  0
#define DY  0
#define FG  -1
#define BG  -1
#define SSX 105
#define STX 35
#define SSY 40
#define STY SSY+4
#define SSW 170
#define STW 65
#define SH  20

#define D_NAME         3 
#define D_POINTS       4
#define D_RACE         6
#define D_ARMOUR       8
#define D_APPEARANCE   10
#define D_TIME         12
#define D_STAMINA      D_TIME+2
#define D_HEALTH       D_TIME+4
#define D_BRAVERY      D_TIME+6
#define D_REACTION     D_TIME+8
#define D_FIRE_ACCUR   D_TIME+10
#define D_THRU_ACCUR   D_TIME+12
#define D_STRENGTH     D_TIME+14

static int d_agup_slider_pro2(int msg, DIALOG *d, int c);
static char slider_text[8][14];

static int points;
static char points_str[100];

/**
 * Gets number of elements in C strings array before NULL value
 */
static int get_list_size(const char **list)
{
	int i = 0;
	while (list[i] != NULL) i++;
	return i;
}

static void fixup_unit_info();

static const char *race_names[] = { 
	"human",
	"sectoid",
	"muton",
	NULL
};

static const char *armour_names_human[] = { 
	"none",
	"standard",
	"power",
	"flying",
	NULL
};

static const char *armour_names_alien[] = { 
	"standard",
	NULL
};

static const char **armour_names;

static const char *appearance_names_human[] = { 
	"blonde guy", 
	"hispanic guy", 
	"oriental guy",
	"black guy",
	"blonde girl", 
	"brunette girl", 
	"oriental girl",
	"black girl",
	NULL
};

static const char *appearance_names_alien[] = { 
	"standard", 
	NULL
};

static const char **appearance_names = NULL;

static int common_change_button_proc(
	const char *title, 
	const char **names, 
	int msg, DIALOG *d, int c)
{
	int result = d_button_proc(msg, d, c);
	if (result == D_CLOSE) {
		
		std::vector<std::string> gui_list;
		for (int i = 0; names[i] != NULL; i++)
			gui_list.push_back(names[i]);

		if (gui_list.size() > 1)
			d->d1 = gui_select_from_list(D_WIDTH, D_HEIGHT, title, gui_list, d->d1);
		fixup_unit_info();
		return D_REDRAW;
	}
	return result;
}

static int race_change_button_proc(int msg, DIALOG *d, int c)
{
	return common_change_button_proc("Select unit race", race_names, msg, d, c);
}

static int armour_change_button_proc(int msg, DIALOG *d, int c)
{
	return common_change_button_proc("Select unit armour", armour_names, msg, d, c);
}

static int appearance_change_button_proc(int msg, DIALOG *d, int c)
{
	return common_change_button_proc("Select unit appearance", appearance_names, msg, d, c);
}

static DIALOG sol_dialog[] = {
	//(dialog proc)      (x)           (y)                   (w)      (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
	{ d_agup_shadow_box_proc, DX,           DY,                   D_WIDTH,     D_HEIGHT, FG,  BG, 0, 0, 0, 0, NULL, NULL, NULL},
	{ d_agup_button_proc,     DX + 200,     DY + SSY + SH*13 + 4, 100,     20,  FG,  BG, 0, D_EXIT, 0, 0, (void *)"OK", NULL, NULL},
	{ d_agup_rtext_proc,      DX + STX,     DY + SSY - SH*1,      STW,     16,  FG,  BG, 0, 0, 0, 0, (void *)"Name:", NULL, NULL},
	{ d_agup_edit_proc,       DX + SSX,     DY + SSY - SH*1 - 4, 23*8,     16,  FG,  BG, 0, 0, 22, 0, NULL, NULL, NULL},
	{ d_agup_text_proc,       DX + 100,     DY + SSY + SH*12 + 2,  100,     16,  FG,  BG, 0, 0, 0, 0, (void *)points_str, NULL, NULL},

	{ d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*0 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)"Race:", NULL, NULL},
	{ race_change_button_proc,DX + SSX,     DY + SSY + SH*0 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

	{ d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*1 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)"Armour:", NULL, NULL},
	{ armour_change_button_proc,DX + SSX,     DY + SSY + SH*1 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

	{ d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*2 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)"Appearance:", NULL, NULL},
	{ appearance_change_button_proc,DX + SSX,     DY + SSY + SH*2 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*3,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[0], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*3,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*4,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[1], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*4,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*5,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[2], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*5,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*6,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[3], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*6,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*7,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[4], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*7,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*8,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[5], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*8,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*9,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[6], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*9,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_agup_rtext_proc,  DX + STX, DY + STY + SH*10, STW, 16, FG, BG, 0, 0, 0, 0, (void *)slider_text[7], NULL, NULL},
	{ d_agup_slider_pro2, DX + SSX, DY + SSY + SH*10, SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
	{ d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
	{ NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

static int d_agup_slider_pro2(int msg, DIALOG * d, int c)
{
	char s[100];
	int v;
	v = d_agup_slider_proc(msg, d, c);
	switch (msg) {
		case MSG_DRAW:
			sprintf(s, "%3d", d->d2);
			text_mode(agup_bg_color);
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

			if (points > MAXPOINTS && d == &sol_dialog[D_STRENGTH]) {
				points -= d->d2 * 2;
				d->d2 = (MAXPOINTS - points) / 2;
				points += d->d2 * 2;
			} else if (points > MAXPOINTS) {
				points -= d->d2;
				d->d2 = MAXPOINTS - points;
				points += d->d2;
			}

			scare_mouse();

			SEND_MESSAGE(d, MSG_DRAW, 0);
			sprintf(points_str, "points remain: %d  ", MAXPOINTS - points);
			SEND_MESSAGE(&sol_dialog[D_POINTS], MSG_DRAW, 0);

			unscare_mouse();
			break;
		default:
			break;
	}
	return v;
}

/**
 * Sets correct value ranges in GUI controls after the user changes
 * information related to skins
 */
static void fixup_unit_info()
{
	if (sol_dialog[D_RACE].d1 == 0) {
		armour_names = armour_names_human;
		appearance_names = appearance_names_human;
	} else {
		armour_names = armour_names_alien;
		appearance_names = appearance_names_alien;
	}
	if (sol_dialog[D_ARMOUR].d1 >= get_list_size(armour_names))
		sol_dialog[D_ARMOUR].d1 = 0;
	if (sol_dialog[D_APPEARANCE].d1 >= get_list_size(appearance_names))
		sol_dialog[D_APPEARANCE].d1 = 0;
	
	DIALOG *d;
	d = &sol_dialog[D_RACE];
	d->w = text_length(font, race_names[d->d1]) + 6;
	d->dp = (void *)race_names[d->d1];
	d = &sol_dialog[D_APPEARANCE];
	d->w = text_length(font, appearance_names[d->d1]) + 6;
	d->dp = (void *)appearance_names[d->d1];
	d = &sol_dialog[D_ARMOUR];
	d->w = text_length(font, armour_names[d->d1]) + 6;
	d->dp = (void *)armour_names[d->d1];
}

/**
 * Shows unit stats edit dialog and allows to edit unit stats, change
 * name, armour and skin
 */
void Editor::edit_soldier()
{
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);

	// Attributes
	sprintf(slider_text[0], "Time Units");
	sprintf(slider_text[1], "Stamina");
	sprintf(slider_text[2], "Health");
	sprintf(slider_text[3], "Bravery");
	sprintf(slider_text[4], "Reactions");
	sprintf(slider_text[5], "Firing");
	sprintf(slider_text[6], "Throwing");
	sprintf(slider_text[7], "Strength");

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

	points = sol_dialog[D_TIME].d2 + sol_dialog[D_HEALTH].d2 +
	         sol_dialog[D_FIRE_ACCUR].d2 + sol_dialog[D_THRU_ACCUR].d2 +
			 sol_dialog[D_STAMINA].d2 + (sol_dialog[D_STRENGTH].d2 * 2) +
			 sol_dialog[D_REACTION].d2;
	sprintf(points_str, "points remain: %d  ", MAXPOINTS - points);
	
	sol_dialog[D_POINTS].fg = gui_fg_color;
	sol_dialog[D_POINTS].bg = gui_bg_color;

    while (mouse_b & 3) rest(1);

	if (man->md.SkinType == S_XCOM_0 || man->md.SkinType == S_XCOM_1 ||
			man->md.SkinType == S_XCOM_2 || man->md.SkinType == S_XCOM_3) {
		appearance_names = appearance_names_human;
		armour_names = armour_names_human;
		if (man->md.Appearance >= 4) man->md.Appearance = 0;
		sol_dialog[D_RACE].d1 = 0;
	} else {
		appearance_names = appearance_names_alien;
		armour_names = armour_names_alien;
		if (man->md.SkinType == S_SECTOID)
			sol_dialog[D_RACE].d1 = 1;
		else
			sol_dialog[D_RACE].d1 = 2;
		man->md.Appearance = 0;
	}

	switch (man->md.SkinType) {
		case S_XCOM_0: sol_dialog[D_ARMOUR].d1 = 0; break;
		case S_XCOM_1: sol_dialog[D_ARMOUR].d1 = 1; break;
		case S_XCOM_2: sol_dialog[D_ARMOUR].d1 = 2; break;
		case S_XCOM_3: sol_dialog[D_ARMOUR].d1 = 3; break;
		case S_SECTOID: sol_dialog[D_ARMOUR].d1 = 0; break;
		default: sol_dialog[D_ARMOUR].d1 = 0; break;
	}
	
	sol_dialog[D_APPEARANCE].d1 = man->md.Appearance + (man->md.fFemale ? 4 : 0);
	
	fixup_unit_info();

	set_dialog_color(sol_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(sol_dialog);
    popup_dialog(sol_dialog, -1);

	man->md.fFemale = sol_dialog[D_APPEARANCE].d1 >= 4;
	man->md.Appearance = sol_dialog[D_APPEARANCE].d1 % 4;
	switch (sol_dialog[D_RACE].d1) {
		case 0: man->md.SkinType = sol_dialog[D_ARMOUR].d1 + 1; break;
		case 1: man->md.SkinType = S_SECTOID; break;
		default: man->md.SkinType = S_MUTON; break;
	}

	man->md.TimeUnits  = sol_dialog[D_TIME].d2;
	man->md.Health     = sol_dialog[D_HEALTH].d2;
	man->md.Stamina    = sol_dialog[D_STAMINA].d2;
	man->md.Reactions  = sol_dialog[D_REACTION].d2;
	man->md.Strength   = sol_dialog[D_STRENGTH].d2;
	man->md.Firing     = sol_dialog[D_FIRE_ACCUR].d2;
	man->md.Throwing   = sol_dialog[D_THRU_ACCUR].d2;
	man->md.Bravery    = sol_dialog[D_BRAVERY].d2;

	man->process_MANDATA();
	
	set_mouse_range(0, 0, 639, 399);
}

static std::vector<std::string> eqsets;

void change_equipment_callback(const char *name)
{
	eqsets.push_back(name);
}

void Editor::change_equipment()
{
	// Get list of available equipment sets
	eqsets.clear();
	LUA_REGISTER_FUNCTION(L, change_equipment_callback);
	lua_safe_dostring(L, "for name in EquipmentTable do change_equipment_callback(name) end");
	
	int result = gui_select_from_list(
		300, 200, "Select equipment set", 
		eqsets, 0);

	lua_pushstring(L, "SetEquipment");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, eqsets[result].c_str());
	lua_safe_call(L, 1, 0);
}

int Editor::do_mapselect()
{
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
