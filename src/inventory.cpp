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

#include "stdafx.h"

#include "global.h"
#include "video.h"
#include "inventory.h"
#include "map.h"
#include "multiplay.h"
#include "soldier.h"
#include "colors.h"

/**
 * Manage soldiers inventory
 */
Inventory::Inventory()
{
	BITMAP *image = create_bitmap(320, 200); clear(image);
	tac01 = new SPK("$(xcom)/ufograph/tac01.scr");
	tac01->show(image, 0, 0);

	//b123 = create_bitmap(83, 22);
	//blit(screen2, b123, 45, 0, 0, 0, 83, 22);
	//b4 = create_bitmap(32, 25);
	//blit(screen2, b4, 224, 21, 0, 0, 32, 25);
	b5 = create_bitmap(32, 15); clear(b5);
	blit(image, b5, 288, 137, 0, 0, 32, 15);

	destroy_bitmap(image);
	sel_item = NULL;
}

Inventory::~Inventory()
{
	destroy_bitmap(b5);
	delete tac01;
}

/**
 * Draw soldiers inventory when in battle
 */
// See also: Editor::show(), Soldier::draw_inventory()
void Inventory::draw()
{ // handle killing in MAN mode
	tac01->show(screen2, 0, 0);  // Buttons: OK, next & prev.man
	//spr_over(screen2, 237, 1, b123);
	//spr_over(screen2, 288, 32, b4);
	//spr_over(screen2, 288, 137, b5);
    draw_sprite_vh_flip(screen2, b5, 255, 137);  // b6 - Button: scroll left

	//sel_man->showspk();
	sel_man->draw_inventory();

	//int color = 1;

	if (sel_item != NULL) {
		sel_man->draw_deselect_times(sel_item_place);

		//textprintf(screen2, font, 129, 141, color, "%s", sel_item->data()->name);
		textprintf(screen2, g_small_font, 128, 140, COLOR_GREEN, "%s", sel_item->name().c_str());
		if (sel_item->is_grenade()) {
			if (sel_item->delay_time()) {
				textout(screen2, g_small_font, "DELAY", 272, 64, COLOR_LT_OLIVE);
				textout(screen2, g_small_font, "TIME:", 272, 72, COLOR_LT_OLIVE);
				textout(screen2, g_small_font, "LEFT=", 272, 80, COLOR_LT_OLIVE);
				textprintf(screen2, g_small_font, 299, 80, COLOR_ORANGE, "%d", sel_item->delay_time() - 1);
				//textprintf(screen2, font, 272, 80, color, "%d", sel_item->rounds);
				rect(screen2, 272, 88, 303, 135, COLOR_GRAY08);      //clip
				PCK::showpck(sel_item->obdata_pInv(), 272, 88 + 8);
			}
		}

		if (sel_item->haveclip()) {
			textout(screen2, g_small_font, "AMMO:",  272, 64, COLOR_LT_OLIVE);
			textout(screen2, g_small_font, "ROUNDS", 272, 72, COLOR_LT_OLIVE);
			textout(screen2, g_small_font, "LEFT=",  272, 80, COLOR_LT_OLIVE);
			textprintf(screen2, g_small_font, 299, 80, COLOR_ORANGE, "%d", sel_item->roundsremain());

			printsmall( 312, 58, COLOR_WHITE, 8); // 8=Time to unload weapon
			rect(screen2, 272, 88, 303, 135, COLOR_GRAY08);      //clip
			PCK::showpck(sel_item->clip()->obdata_pInv(), 272, 88 + 8);
		} else if (sel_item->obdata_isAmmo()) {
			printsmall( 34, 85, COLOR_WHITE, 15); // 15=Time to load weapon

			textout(screen2, g_small_font, "AMMO:",  272, 64, COLOR_LT_OLIVE);
			textout(screen2, g_small_font, "ROUNDS", 272, 72, COLOR_LT_OLIVE);
			textout(screen2, g_small_font, "LEFT=",  272, 80, COLOR_LT_OLIVE);
			textprintf(screen2, g_small_font, 299, 80, COLOR_ORANGE, "%d", sel_item->m_rounds);
			rect(screen2, 272, 88, 303, 135, COLOR_GRAY08);      //clip
			PCK::showpck(sel_item->obdata_pInv(), 272, 88 + 8);
		}

		if (key[KEY_LCONTROL]) {
			sel_item->od_info(mouse_x, mouse_y, COLOR_WHITE);
		} else {
			PCK::showpck(sel_item->obdata_pInv(),
			                mouse_x - sel_item->obdata_width()  * 16 / 2,
			                mouse_y - sel_item->obdata_height() * 16 / 2 + 8);
		}

	}
}

/**
 * Process mouse-clicks on inventory-screen during battle
 */
void Inventory::execute()
{

	if (sel_item == NULL) {
		if (mouse_inside( 50, 50, 110, 142)) {  // Picture of soldier (in Powerarmor)
			//g_console->printf(COLOR_SYS_DEBUG, "x,y: %d %d", mouse_x, mouse_y); 
			MODE = UNIT_INFO;	// switch to stats-display
		} 
		if (mouse_inside(237, 1, 271, 22)) {  //ok
			MODE = MAP3D;
			//map->place(sel_man->z, sel_man->x, sel_man->y)->viscol=0; //!!reset vis
		} else {
			Soldier *s = sel_man;
 			if (mouse_inside(273, 1, 295, 22)) {  // <
 				sel_man = sel_man->prevman();
				while (!sel_man->is_active() && (sel_man != s))
					sel_man = sel_man->prevman();
				if ((sel_man == s) && !s->is_active())
					MODE = MAP3D; // we were stunned while we were looking at the screen, so move away!
			} else if (mouse_inside(297, 1, 319, 22)) {  // >
 				sel_man = sel_man->nextman();
				while (!sel_man->is_active() && (sel_man != s))
					sel_man = sel_man->nextman();
				if ((sel_man == s) && !s->is_active())
					MODE = MAP3D; // we were stunned while we were looking at the screen, so move away!
			}
		}
	}
	if (mouse_inside(288, 32, 319, 57)) {  // clip
		if (sel_man->unload_ammo(sel_item))
			sel_item = NULL;
	} else
		if (mouse_inside(288, 137, 319, 151)) {  // -->
			map->place(sel_man->z, sel_man->x, sel_man->y)->scroll_right();
		} else
			if (mouse_inside(255, 137, 286, 151)) {  // <--
				map->place(sel_man->z, sel_man->x, sel_man->y)->scroll_left();
			} else {
				if (sel_item == NULL) {
					sel_item = sel_man->select_item(sel_item_place);
					//net_send("_select_item");
					if (sel_item != NULL)
						net->send_select_item(sel_man->NID, sel_item_place, sel_item->m_x, sel_item->m_y);
				} else {
					Item *it = sel_man->item_under_mouse(P_ARM_LEFT);
					if ((it != NULL) && sel_man->load_ammo(P_ARM_LEFT, sel_item)) {
						sel_item = NULL;
						return ;
					}

					it = sel_man->item_under_mouse(P_ARM_RIGHT);
					if ((it != NULL) && sel_man->load_ammo(P_ARM_RIGHT, sel_item)) {
						sel_item = NULL;
						return ;
					}

					int req_time;
					int pn = sel_man->deselect_item(sel_item, sel_item_place, req_time);
					if (pn != -1) {
						net->send_deselect_item(sel_man->NID, pn, sel_item->m_x, sel_item->m_y, req_time);
						sel_item = NULL;
					}
				}
			}

}

/**
 * Put selected item back to its old place,
 * e.g. when exiting the inventory-screen
 */
void Inventory::backput()
{
	if (sel_item != NULL) {
		sel_man->putitem(sel_item, sel_item_place, sel_item->m_x, sel_item->m_y);
		net->send_deselect_item(sel_man->NID, sel_item_place, sel_item->m_x, sel_item->m_y, 0);
		sel_item = NULL;
	}
}


void Inventory::close()
{
	if (sel_item == NULL)
		MODE = MAP3D;
	else
        backput();      //return item to original place
}

