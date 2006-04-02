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
#include "text.h"
#include "mouse.h"
#include "script_api.h"

/**
 * Manage soldiers inventory
 */
Inventory::Inventory()
{
    x = y = 0;

    BITMAP *image = create_bitmap(320, 200); clear_to_color(image, bitmap_mask_color(image));
    SPK *tac01 = new SPK("$(xcom)/ufograph/tac01.scr");
    tac01->show(image, 0, 0);

    b4 = create_bitmap(320 - 230, 30);
    blit(image, b4, image->w - b4->w, 0, 0, 0, b4->w, b4->h);
    b5 = create_bitmap(32, 15);
    blit(image, b5, 288, 137, 0, 0, b5->w, b5->h);
    delete tac01;
    destroy_bitmap(image);
    sel_item = NULL;
}

Inventory::~Inventory()
{
    destroy_bitmap(b4);
    destroy_bitmap(b5);
}

/**
 * Draw soldiers inventory when in battle
 */
// See also: Editor::show(), Soldier::draw_inventory()
void Inventory::draw(int _x, int _y)
{ // handle killing in MAN mode
    x = _x; y = _y;        
    
    m_mouse_range = new MouseRange(x, y, x + 320, y + 200);
    
    BITMAP *temp = create_bitmap(321, 199);
    clear_bitmap(temp);                                           
    
    set_trans_blender(0, 0, 0, 160);
    draw_trans_sprite(screen2, temp, x, y);
    
    destroy_bitmap(temp);
    temp = create_sub_bitmap(screen2, x, y, 321, 199);
    
    if (mouse_inside(x + 50, y + 50, x + 110, y + 142))
        rect(temp, 50, 50, 110, 142, makecol(255, 0, 0));

    draw_sprite(temp, b4, temp->w - b4->w, 0);
    draw_sprite_vh_flip(temp, b5, 255, 137);

    ALPHA_SPRITE *button_left_arrow = lua_table_image("button_left_arrow");
    ALPHA_SPRITE *button_right_arrow = lua_table_image("button_right_arrow");
    if (button_left_arrow && button_right_arrow) {
        draw_alpha_sprite(temp, button_right_arrow, temp->w - button_right_arrow->w, 137);
        draw_alpha_sprite(temp, button_left_arrow, temp->w - button_right_arrow->w - button_left_arrow->w, 137);
    }

    sel_man->draw_inventory(temp);

    if (sel_item != NULL) {
        sel_man->draw_deselect_times(temp, sel_item, sel_item_place);

        textprintf(temp, g_small_font, 128, 140, COLOR_GREEN, "%s", sel_item->name().c_str());
        if (sel_item->is_grenade()) {
            if (sel_item->delay_time()) {
                textout(temp, g_small_font, _("DELAY"), 272, 64, COLOR_LT_OLIVE);
                textout(temp, g_small_font, _("TIME:"), 272, 72, COLOR_LT_OLIVE);
                textout(temp, g_small_font, _("LEFT="), 272, 80, COLOR_LT_OLIVE);
                textprintf(temp, g_small_font,          299, 80, COLOR_ORANGE, "%d", sel_item->delay_time() - 1);
                rect(temp, 272, 88, 303, 135, COLOR_GRAY08);      //clip
                PCK::showpck(temp, sel_item->obdata_pInv(), 272, 88 + 8);
            }
        }

        if (sel_item->haveclip()) {
            textout(temp, g_small_font, _("AMMO:"),  272, 64, COLOR_LT_OLIVE);
            textout(temp, g_small_font, _("ROUNDS"), 272, 72, COLOR_LT_OLIVE);
            textout(temp, g_small_font, _("LEFT="),  272, 80, COLOR_LT_OLIVE);
            textprintf(temp, g_small_font,           299, 80, COLOR_ORANGE, "%d", sel_item->roundsremain());

            rect(temp, 272, 88, 303, 135, COLOR_GRAY08);      //clip
            PCK::showpck(temp, sel_item->clip()->obdata_pInv(), 272, 88 + 8);
        } else if (sel_item->obdata_isAmmo()) {
            textout(temp, g_small_font, _("AMMO:"),  272, 64, COLOR_LT_OLIVE);
            textout(temp, g_small_font, _("ROUNDS"), 272, 72, COLOR_LT_OLIVE);
            textout(temp, g_small_font, _("LEFT="),  272, 80, COLOR_LT_OLIVE);
            textprintf(temp, g_small_font,           299, 80, COLOR_ORANGE, "%d", sel_item->m_rounds);
            rect(temp, 272, 88, 303, 135, COLOR_GRAY08);      //clip
            PCK::showpck(temp, sel_item->obdata_pInv(), 272, 88 + 8);
        }

        if (!key[KEY_LCONTROL]) {
            PCK::showpck(temp, sel_item->obdata_pInv(),
                            mouse_x - x - sel_item->obdata_width()  * 16 / 2,
                            mouse_y - y - sel_item->obdata_height() * 16 / 2 + 8);
        }

    }
    
    destroy_bitmap(temp);
    
    if (sel_item != NULL && key[KEY_LCONTROL])
        sel_item->od_info(mouse_x, mouse_y, COLOR_WHITE);
        
    delete m_mouse_range;
}

/**
 * Process mouse-clicks on inventory-screen during battle
 */
void Inventory::execute()
{

    if (sel_item == NULL) {
        if (mouse_inside(x + 50, y + 50, x + 110, y + 142)) {  // Picture of soldier (in Powerarmor)
            //g_console->printf(COLOR_SYS_DEBUG, "x,y: %d %d", mouse_x, mouse_y); 
            MODE = UNIT_INFO;   // switch to stats-display
        } 
        if (mouse_inside(x + 237, y + 1, x + 271, y + 22)) {  //ok
            MODE = MAP3D;
            //map->place(sel_man->z, sel_man->x, sel_man->y)->viscol=0; //!!reset vis
        } else {
            Soldier *s = sel_man;
            if (mouse_inside(x + 273, y + 1, x + 295, y + 22)) {  // <
                sel_man = sel_man->prevman();
                while ((!sel_man->is_active() && (sel_man != s)) || sel_man->is_panicking())
                    sel_man = sel_man->prevman();
                if ((sel_man == s) && !s->is_active())
                    MODE = MAP3D; // we were stunned while we were looking at the screen, so move away!
            } else if (mouse_inside(x + 297, y + 1, x + 319, y + 22)) {  // >
                sel_man = sel_man->nextman();
                while ((!sel_man->is_active() && (sel_man != s)) || sel_man->is_panicking())
                    sel_man = sel_man->nextman();
                if ((sel_man == s) && !s->is_active())
                    MODE = MAP3D; // we were stunned while we were looking at the screen, so move away!
            }
        }
    }
    
    if (mouse_inside(x + 288, y + 137, x + 319, y + 151)) {  // -->
        map->place(sel_man->z, sel_man->x, sel_man->y)->scroll_right();
    } else
        if (mouse_inside(x + 255, y + 137, x + 286, y + 151)) {  // <--
            map->place(sel_man->z, sel_man->x, sel_man->y)->scroll_left();
        } else {
            if (sel_item == NULL) {
                sel_item = sel_man->select_item(sel_item_place, x, y);
                if (sel_item != NULL)
                    net->send_select_item(sel_man->NID, sel_item_place, sel_item->m_x, sel_item->m_y);
            } else {
                Item *it = sel_man->item_under_mouse(P_ARM_LEFT, x, y);
                if ((it != NULL) && sel_man->load_ammo(P_ARM_LEFT, sel_item_place, sel_item)) {
                    soundSystem::getInstance()->play(SS_CLIP_LOAD);
                    sel_item_place = P_MAP;
                    if (sel_item) sel_item->m_x = sel_item->m_y = -1;
                    return ;
                }

                it = sel_man->item_under_mouse(P_ARM_RIGHT, x, y);
                if ((it != NULL) && sel_man->load_ammo(P_ARM_RIGHT, sel_item_place, sel_item)) {
                    soundSystem::getInstance()->play(SS_CLIP_LOAD);
                    sel_item_place = P_MAP;
                    if (sel_item) sel_item->m_x = sel_item->m_y = -1;
                    return ;
                }

                int req_time;
                int pn = sel_man->deselect_item(sel_item, sel_item_place, req_time, x, y);
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
    if (sel_item == NULL) {
        MODE = MAP3D;
    } else                                             
        backput();      //return item to original place
}

