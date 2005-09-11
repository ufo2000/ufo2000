/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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
#include "config.h"
#include "video.h"
#include "wind.h"
#include "editor.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"
#include "sound.h"
#include "multiplay.h"
#include "script_api.h"

/**
 * Returns true if the weapon can be used. The weapon will be colored darkgray
 * and not allowed to select if this function returns false (this function
 * may be useful when points limit is exceeded for example)
 */
bool is_item_allowed(int type)
{
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "IsItemAllowed");
    lua_gettable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, Item::obdata_name(type).c_str());
    lua_safe_call(L, 1, 1);
    bool result = lua_toboolean(L, -1) != 0;
    lua_settop(L, stack_top);
    return result;
}

Editor::Editor()
{
    BITMAP *image = create_bitmap(320, 200); clear(image);
    tac01 = new SPK("$(xcom)/ufograph/tac01.scr");  // Picture with buttons
    tac01->show(image, 0, 0);
    b5 = create_bitmap(32, 15); clear(b5);  // Button for Scroll-left
    blit(image, b5, 288, 137, 0, 0, 32, 15);
    destroy_bitmap(image);

    m_armoury = new Place(0, 220, 20, 11);

    // make armoury object available to lua code
    lua_pushstring(L, "Armoury");
    LUA_PUSH_OBJECT_POINTER(L, m_armoury);
    lua_settable(L, LUA_GLOBALSINDEX);

    if (local_platoon_size > 15) local_platoon_size = 15;      //Maybe we should allow more
    ASSERT(local_platoon_size > 0);
    m_plt = new Platoon(2001, local_platoon_size);
    m_plt->load_FULLDATA("$(home)/squad.lua");
    man = m_plt->captain();

    sel_item = NULL;
    //dup_item = NULL;    
    buffer.empty = true;
}

Editor::~Editor()
{
    delete m_plt;
    delete tac01;
    delete m_armoury;
    // unregister armoury object from lua
    lua_pushstring(L, "Armoury");
    lua_pushnil(L);
    lua_settable(L, LUA_GLOBALSINDEX);
}

/**
 * Load team of soldiers from file
 */
void Editor::load()
{
    std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
        _("Load squad (*.squad files)"), F("$(home)"), "squad");
    
    if (filename.empty()) {
        alert( "", _("No saved squads found!"), "", _("OK"), NULL, 0, 0);
        return;
    }
    
    m_plt->load_FULLDATA(filename.c_str());
    lua_message(std::string("Squad loaded: ") + filename);
}


/**
 * Save team of soldiers to file
 */
void Editor::save()
{
    std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
        _("Save squad (*.squad file)"), F("$(home)"), "squad", true);
    
    if (!filename.empty()) {
        m_plt->save_FULLDATA(filename.c_str());
        lua_message(std::string("Squad saved: ") + filename);
    }
}

void Editor::export_weaponset()
{
    std::string filename = "equipment.lua";
    
    if (!filename.empty()) {
        m_armoury->export_as_weaponset(filename.c_str());
        g_console->printf(COLOR_RED04, _("Armoury layout exported as weapon set template to %s"), F(filename.c_str()));
    }
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
 * Handle left mouse click in soldier equipment screen,
 * e.g. move items around, click buttons.
 * @returns  true if the user decided to exit from equipment screen
 */
bool Editor::handle_mouse_leftclick()
{
    int i;

    // Mouse click in unit stats area - edit unit stats.
    if (mouse_inside(320, 0, 639, 200)) {
        // unibord
        edit_soldier();
        return false;
    }
    if (mouse_inside( 50, 50, 110, 142)) {  // Picture of soldier (Powerarmor)
        edit_soldier();
        return false;
    } 
    // Mouse click on text "ARMORY": change equipment dialog 
    if (mouse_inside(0, 200, 105, 220)) {
        change_equipment();
        return false;
    }

    if (sel_item == NULL) {
        if (mouse_inside(237, 1, 271, 22)) {  // ok-button
            return true;
        } else if (mouse_inside(273, 1, 295, 22)) {  // <
            man = man->prevman();
        } else if (mouse_inside(297, 1, 319, 22)) {  // >
            man = man->nextman();
        }
    }

    if (mouse_inside(288, 137, 319, 151)) {  // -->
        man->place(P_MAP)->scroll_right();
    } else if (mouse_inside(255, 137, 286, 151)) {  // <--
        man->place(P_MAP)->scroll_left();
    }

    if (sel_item == NULL) {  // Pick up item from soldier
        for (i = 0; i < NUMBER_OF_PLACES; i++) {
            sel_item = man->place(i)->mselect(0, 0);
            if (sel_item != NULL) {
                sel_item_place = i;
                break;
            }
        }
        if (sel_item == NULL) {  // Pick up item from armory
            sel_item = m_armoury->mselect(0, 0);
            if (sel_item != NULL) {
                sel_item_place = P_ARMOURY;
                if (is_item_allowed(sel_item->m_type))
                    m_armoury->put(sel_item, sel_item->m_x, sel_item->m_y);
                sel_item = sel_item->create_duplicate();
            }
        }
        soundSystem::getInstance()->play(SS_ITEM_GET);
    } else {
        // Try to drop item to the soldier inventory
        if (is_item_allowed(sel_item->m_type)) {
            for (i = 0; i < NUMBER_OF_PLACES; i++) {
                if (man->place(i)->mdeselect(sel_item, 0, 0)) {

                    if (sel_item_place == P_ARMOURY && (key[KEY_LSHIFT]) || (key[KEY_RSHIFT])) {
                        // While holding SHIFT key, the same item remains selected, so we don't
                        // drop it to the soldier, but make a copy - useful for equipping with 
                        // clips or grenades
                        sel_item = sel_item->create_duplicate();
                    } else {
                        sel_item = NULL;
                    }

                    soundSystem::getInstance()->play(SS_ITEM_PUT);
                    return false;
                }
            }
        }

        // Drop item to armoury
        if (m_armoury->mdeselect(sel_item, 0, 0)) {
            // Delete all the duplicates of such item in the armoury
            while (true) {
                std::vector<Item *> items;
                m_armoury->get_items_list(items);
                int i;
                for (i = 0; i < (int)items.size(); i++) {
                    if (items[i] != sel_item && items[i]->m_place == m_armoury && 
                        items[i]->m_type == sel_item->m_type) {
                        // Do not delete item here as some pointers it items vector may
                        // become invalid (pointers to ammo in the case of deleted gun),
                        // so we just delete a single item and start the search for the
                        // next duplicate from the very beginning
                        break;
                    }
                }
                if (i == (int)items.size()) break;
                items[i]->unlink();
                delete items[i];
            }
            soundSystem::getInstance()->play(SS_ITEM_PUT);
            sel_item = NULL;
            return false;
        }

        // Try to load this item as a clip
        if (load_clip()) {
            soundSystem::getInstance()->play(SS_CLIP_LOAD);
            return false;
        }

        // Can't find proper place for this item
        if (!is_item_allowed(sel_item->m_type))
            g_console->printf(COLOR_RED04, _("This item is not allowed for currently selected weapon set, right-click to get rid of it"));
        soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
    }
    return false;
};

/**
 * Activate unit stats and inventory edit screen
 * Draw soldier's inventory when in mission-planner
 */
// See also: Inventory::draw(), Soldier::draw_inventory()
// Called by: Connect::do_planner() via Units::execute and execute_main()
void Editor::show()
{
    reset_video();
    destroy_bitmap(screen2);
    screen2 = create_bitmap(640, 400); 
    clear(screen2);

    // Prepare background picture for editor screen to improve 
    // performance a bit (static image that is never changed)
    BITMAP *editor_bg = create_bitmap(640, 400);
    clear_to_color(editor_bg, COLOR_BLACK1);
    tac01->show(editor_bg, 0, 0); // draw buttons: OK, Next-Man, Prev-Man, Unload-clip, Scroll-right
    draw_sprite_vh_flip(editor_bg, b5, 255, 137); // Button: Scroll-left
    rectfill(editor_bg, 288, 32, 319, 57, COLOR_GRAY15);    //hide unused "unload" button
    text_mode(-1);
    textout(editor_bg, g_small_font, _("Click-and-drop weapons from the armory to the soldier, right-click to remove"), 0, 364 + 22, COLOR_WHITE); 

    position_mouse(320, 200);
    MouseRange temp_mouse_range(0, 0, 639, 400);

    int DONE = 0;
    int mouse_leftr = 1, mouse_rightr = 1;
    int i;
    int color = COLOR_LT_OLIVE;
    int A1 = 0, A2 = 0;

    while (mouse_b & 3) rest(1);

    g_console->resize(SCREEN_W, SCREEN_H - 400);
    g_console->set_full_redraw();
    g_console->redraw(screen, 0, 400);

    while (!DONE) {

        net->check();

        rest(1); // Don't eat all CPU resources

        if (CHANGE) {
            g_console->redraw(screen, 0, 400);

            blit(editor_bg, screen2, 0, 0, 0, 0, editor_bg->w, editor_bg->h);
            man->showspk(screen2); // Show "bigpicture" of soldier in choosen armor

            color = COLOR_DK_GRAY;
            if (man->x != 0)   // ??? This soldier already selected for the mission ?
                color = COLOR_LT_OLIVE;
            text_mode(-1);
            textout(screen2, large, man->md.Name, 0, 0, color);

            for (i = 0; i < NUMBER_OF_PLACES; i++) //man->drawgrid();
                man->place(i)->drawgrid(screen2, i);
            m_armoury->drawgrid(screen2, P_ARMOURY);

            man->draw_unibord(1, 320, 0);  // Attribute-Barchart
            if (sel_item != NULL) {
                if (sel_item_place == P_ARMOURY)
                    sel_item->od_info(330, 220, COLOR_WHITE);
                else
                    sel_item->od_info(330, 220, COLOR_OLIVE);

                textprintf(screen2, g_small_font, 128, 140, COLOR_GREEN,  "%s", sel_item->name().c_str());

                if (sel_item->haveclip()) {
                    //textprintf(screen2, font, 272, 80, color, "%d", sel_item->roundsremain());
                    textout(screen2, g_small_font, _("AMMO:"),  272, 64, COLOR_LT_OLIVE);
                    textout(screen2, g_small_font, _("ROUNDS"), 272, 72, COLOR_LT_OLIVE);
                    textout(screen2, g_small_font, _("LEFT="),  272, 80, COLOR_LT_OLIVE);
                    textprintf(screen2, g_small_font,           299, 80, COLOR_ORANGE, "%d", sel_item->roundsremain());
                    rect(screen2, 272, 88, 303, 135, COLOR_DK_GRAY);      //clip
                    PCK::showpck(sel_item->clip()->obdata_pInv(), 272, 88 + 8);
                } else if (sel_item->obdata_isAmmo()) {
                    //textprintf(screen2, font, 272, 80, color, "%d", sel_item->rounds);
                    textout(screen2, g_small_font, _("AMMO:"),  272, 64, COLOR_LT_OLIVE);
                    textout(screen2, g_small_font, _("ROUNDS"), 272, 72, COLOR_LT_OLIVE);
                    textout(screen2, g_small_font, _("LEFT="),  272, 80, COLOR_LT_OLIVE);
                    textprintf(screen2, g_small_font,           299, 80, COLOR_ORANGE, "%d", sel_item->m_rounds);
                    rect(screen2, 272, 88, 303, 135, COLOR_DK_GRAY);      //clip
                    PCK::showpck(sel_item->obdata_pInv(), 272, 88 + 8);
                }
                PCK::showpck(sel_item->obdata_pInv(),
                                mouse_x - sel_item->obdata_width()  * 16 / 2,
                                mouse_y - sel_item->obdata_height() * 16 / 2 + 8);
            } else {
                Item *it = m_armoury->item_under_mouse(0, 0);
                if (it != NULL) {
                    if (is_item_allowed(it->m_type))
                        it->od_info(330, 235, COLOR_GRAY05);
                    else
                        it->od_info(330, 235, COLOR_GRAY10);
                } else {
                    //textprintf(screen2, large, 330, 220, COLOR_LT_BLUE, _("Click here to change equipment set"));
                    int ty = 235;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("       F1: Help")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("    F2/F3: Save/load team")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("       F4: Edit attributes")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("       F5: Change weaponset")); ty += 15;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("       F6: Save as weapon set template")); ty += 15;

                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _(" Ctrl+Ins: Copy current soldier")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("Shift+Ins: Paste on current soldier")); ty += 15;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("      Del: Delete items of current man")); /*ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("Shift+Del: Drop items of current man"));*/ ty += 15;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("      F11: Cycle through appearences")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("      F12: Cycle through human armours")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("Shift+F12: Cycle through alien races")); ty += 15;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("      Tab: Next soldier")); ty += 10;
                    textprintf(screen2, font,  330, ty, COLOR_BLUE,     _("Shift+Tab: Previous soldier")); ty += 10;
                }
            }

            int wht = man->count_weight();
            int max_wht = man->md.Strength;
            color       = max_wht < wht ? COLOR_RED03 : COLOR_GRAY02;
            textprintf(screen2, g_small_font, 0, 20, color, _("Equipment weight: %2d/%2d"), wht, max_wht);
            char str1[64]; // to adjust position of translated string
          //int x1 = 120;
            int x2 = 236;
            sprintf(str1, "%s: %4d", _("Equipment cost"), man->calc_full_ammunition_cost() );
            int w1 = text_length(g_small_font, str1);  // right-justify string
            textprintf(screen2, g_small_font, x2-w1, 20, COLOR_GRAY02, str1);

            draw_sprite(screen2, mouser, mouse_x, mouse_y);
            blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
            CHANGE = 0;
        }

        if ((mouse_b & 1) && (mouse_leftr)) { //left mouseclick
            mouse_leftr = 0;
            CHANGE = 1;

            if (handle_mouse_leftclick())
                DONE = 1;
        }

        if ((mouse_b & 2) && (mouse_rightr)) { //right mouseclick: get & put items
            mouse_rightr = 0;
            CHANGE = 1;
            if (sel_item != NULL) {
                if (sel_item_place == P_ARMOURY) {
                    // If item was taken from the armoury - just delete it
                    delete sel_item;
                    sel_item = NULL;
                } else {
                    // If item was taken from the the soldier - put it back
                    man->putitem(sel_item, sel_item_place, sel_item->m_x, sel_item->m_y);
                    sel_item = NULL;
                }
            } else {
                // Delete item under mouse cursor
                for (i = 0; i < NUMBER_OF_PLACES; i++) {
                    Item *it = man->place(i)->mselect(0, 0);
                    if (it != NULL) delete(it);
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
          //int c = readkey();
          //switch (c >> 8) {
            int scancode; int keycode = ureadkey(&scancode); 
            switch (scancode) { 
                case KEY_F1:
                    help( HELP_INVENTORY );
                    break;
                // Todo: Change from "Save&Load Team" to "Save&Load Soldier" 
                // Todo: move "Save&Load Team" to Mission-planner (connect.cpp)
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
                case KEY_F4:
                    edit_soldier();   // Edit Attributes+Armor
                    break;

                case KEY_F5:
                    change_equipment();
                    break;

                case KEY_F6:
                    export_weaponset();
                    break;

                case KEY_F10:
                    change_screen_mode();
                    break;

                case KEY_F11:  // cycle thru apperances:
                    A1 = man->md.Appearance;
                    A2 = man->md.fFemale;
                    if ((key[KEY_LSHIFT]) || (key[KEY_RSHIFT]) ) { // Shift-F11: 
                        A2++;
                        if (A2 >= 2) A2 = 0;
                        man->md.fFemale    = A2;
                    } else { // F11: 
                        A1 = A1 + (A2 ? 4 : 0);
                        A1++;
                        if (A1 >= 8) A1 = 0;
                        man->md.fFemale    = A1 >= 4;
                        man->md.Appearance = A1 % 4;
                    }
                    man->process_MANDATA();
                    break;
                case KEY_F12:  // cycle thru armor-types:
                    A1 = man->md.SkinType;
                    if ((key[KEY_LSHIFT]) || (key[KEY_RSHIFT]) ) // Shift-F12: Aliens
                        man->skin()->next_alien();
                    else // F12: Human Armor
                        man->skin()->next_human();
                    man->process_MANDATA();
                    break;
//
                case KEY_INSERT:  // Todo: Copy items from last DEL to current man
                    if ((key[KEY_LCONTROL]) || (key[KEY_RCONTROL])) {
                        copy_soldier(man);
                        break;
                    }
                    if ((key[KEY_LSHIFT]) || (key[KEY_RSHIFT])) {
                        paste_soldier(man);
                        break;
                    }
                    break;
                case KEY_DEL:  // Todo: store the deleted items (where?) for KEY_INSERT
                    if ((key[KEY_LSHIFT]) || (key[KEY_RSHIFT]) ) { // Shift-DEL:
                      // Drop all carried items:   // Todo: drop to common pool
                        Item * it;
                        for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++) {
                            it = man->item(i);
                            if (it != NULL)
                                man->putitem(it, P_MAP);
                        }
                    } else { // DEL:
                        // Destroy items of current man, including those on the ground:
                        man->destroy_all_items();
                    }
                    break;

                case KEY_TAB:   // jump to next/prev. soldier
                    if ((key[KEY_LSHIFT]) || (key[KEY_RSHIFT]) ) { // Shift-TAB:
                        man = man->prevman();
                    } else { // TAB:
                        man = man->nextman();
                    }
                    break;
                case KEY_LEFT:
                    man = man->prevman();
                    break;
                case KEY_RIGHT:
                    man = man->nextman();
                    break;     

                 case KEY_PGUP:
                    scroll_equipment(-1);
                    break;
                 case KEY_PGDN:
                    scroll_equipment(+1);
                    break;
                case KEY_PRTSCR:
                    if (askmenu(_("SCREEN-SNAPSHOT"))) {
                        savescreen();
                    }
                    break;
                case KEY_ESC:
                    DONE = 1;
                    break;
                default: 
                    if (g_console->process_keyboard_input(keycode, scancode))
                        net->send_message((char *)g_console->get_text());
            }
        }
    }

    m_plt->save_FULLDATA("$(home)/squad.lua");

    destroy_bitmap(editor_bg);
    destroy_bitmap(screen2);
    screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);

    g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
    g_console->set_full_redraw();

    clear(screen);
}

/**
 * Load clip into weapon
 */
int Editor::load_clip()
{
    if (sel_item != NULL) {
        for (int i = 0; i < NUMBER_OF_PLACES; i++) {
            Item *it = man->place(i)->item_under_mouse(0, 0);
            if ((it != NULL) && it->loadclip(sel_item)) {
                return 1;
            }
        }
    }
    return 0;
}

//#define D_WIDTH 320
#define D_WIDTH 360
#define D_HEIGHT 340

#define DX  0
#define DY  0
#define FG  -1
#define BG  -1
//#define SSX 105
//#define STX 35
#define SSX 145
#define STX 35
#define SSY 40
#define STY SSY+4
//#define SSW 170
//#define STW 65
#define SSW 170
#define STW 105
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

// Todo: gettext ??
static const char *race_names[] = { 
    "human",
    "sectoid",
    "muton",
    "chameleon",
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

static const char *armour_names_chameleon[] = { 
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

static const char *appearance_names_chameleon[512];

static const char **appearance_names = NULL;

/**
 * !!! Hack, should be removed after proper units modding implementation
 */
static void init_chameleon_appearances()
{
    int stack_top = lua_gettop(L);

    lua_pushstring(L, "UnitsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1));
    int index = 0;
    while (true) {
        if (index + 1 >= (int)(sizeof(appearance_names_chameleon) / sizeof(appearance_names_chameleon[0])))
            break;
        lua_pushnumber(L, index); // $$$
        lua_gettable(L, -2);
        if (!lua_istable(L, -1)) break;
        lua_pushstring(L, "name");
        lua_gettable(L, -2);
        ASSERT(lua_isstring(L, -1));
        appearance_names_chameleon[index] = (const char *)lua_tostring(L, -1);
        lua_pop(L, 2);
        index++;
    }

    appearance_names_chameleon[index] = NULL;
    appearance_names = appearance_names_chameleon;

    lua_settop(L, stack_top);
}

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
    return common_change_button_proc( _("Select unit race"), race_names, msg, d, c);
}

static int armour_change_button_proc(int msg, DIALOG *d, int c)
{
    return common_change_button_proc( _("Select unit armour"), armour_names, msg, d, c);
}

static int appearance_change_button_proc(int msg, DIALOG *d, int c)
{
    return common_change_button_proc( _("Select unit appearance"), appearance_names, msg, d, c);
}


/**
 * Definition of dialog for soldiers attributes and skin/armor
 */
static DIALOG *sol_dialog = NULL;

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
            sprintf(points_str, _("Points remain: %2d "), MAXPOINTS - points);
            SEND_MESSAGE(&sol_dialog[D_POINTS], MSG_DRAW, 0);

            unscare_mouse();
            break;
        default:
            break;
    }
    return v;
}

/**
 * Sets correct value ranges in GUI controls 
 * after the user changes information related to skins
 */
static void fixup_unit_info()
{
    if (sol_dialog[D_RACE].d1 == 0) {
        armour_names     = armour_names_human;
        appearance_names = appearance_names_human;
    } else if (sol_dialog[D_RACE].d1 == 3) {
        armour_names     = armour_names_chameleon;
        init_chameleon_appearances();
    } else {
        armour_names     = armour_names_alien;
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
 * Shows unit-stats edit-dialog and allows to edit unit stats, 
 * change name, and armour (=skin)
 */
void Editor::edit_soldier()
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    DIALOG sol_dialog[] = {
        //(dialog proc)      (x)           (y)                   (w)      (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
        { d_agup_shadow_box_proc, DX,           DY,                   D_WIDTH,     D_HEIGHT, FG,  BG, 0, 0, 0, 0, NULL, NULL, NULL},
        { d_agup_button_proc,     DX + 200,     DY + SSY + SH*13 + 4, 100,     20,  FG,  BG, 0, D_EXIT, 0, 0, (void *)_("OK"), NULL, NULL},
        { d_agup_rtext_proc,      DX + STX,     DY + SSY - SH*1,      STW,     16,  FG,  BG, 0, 0, 0, 0, (void *)_("Name:"), NULL, NULL},
        { d_agup_edit_proc,       DX + SSX,     DY + SSY - SH*1 - 4, 23*8,     16,  FG,  BG, 0, 0, 22, 0, NULL, NULL, NULL},
        { d_agup_text_proc,       DX + 100,     DY + SSY + SH*12 + 2,  100,     16,  FG,  BG, 0, 0, 0, 0, (void *)points_str, NULL, NULL},

        { d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*0 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)_("Race:"), NULL, NULL},
        { race_change_button_proc,DX + SSX,     DY + SSY + SH*0 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

        { d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*1 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)_("Armour:"), NULL, NULL},
        { armour_change_button_proc,DX + SSX,     DY + SSY + SH*1 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

        { d_agup_rtext_proc,      DX + STX,     DY + SSY + SH*2 + 1,  STW, 16 + 4,  FG,  BG, 0, 0, 0, 0, (void *)_("Appearance:"), NULL, NULL},
        { appearance_change_button_proc,DX + SSX,     DY + SSY + SH*2 - 6,    0, 16 + 4,  FG,  BG, 0, D_EXIT, 0, 0, NULL, NULL, NULL},

        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*3,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Time Units"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*3,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*4,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Stamina"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*4,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*5,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Health"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*5,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*6,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Bravery"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*6,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*7,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Reactions"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*7,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*8,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Firing"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*8,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*9,  STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Throwing"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*9,  SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_agup_rtext_proc,  DX + STX, DY + STY + SH*10, STW, 16, FG, BG, 0, 0, 0, 0, (void *)_("Strength"), NULL, NULL},
        { d_agup_slider_pro2, DX + SSX, DY + SSY + SH*10, SSW, 16, FG, BG, 0, 0, 100, 33, NULL, NULL, NULL},
        { d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
        { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
    };

    ::sol_dialog = sol_dialog;

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
    sprintf(points_str, _("Points remain: %2d "), MAXPOINTS - points);

    sol_dialog[D_POINTS].fg = gui_fg_color;
    sol_dialog[D_POINTS].bg = gui_bg_color;

    while (mouse_b & 3) rest(1);

    if (man->md.SkinType == S_XCOM_0 || 
        man->md.SkinType == S_XCOM_1 || 
        man->md.SkinType == S_XCOM_2 || 
        man->md.SkinType == S_XCOM_3) {
        appearance_names = appearance_names_human;
        armour_names     = armour_names_human;
        if (man->md.Appearance >= 4) man->md.Appearance = 0;
        sol_dialog[D_RACE].d1 = 0;
    } else {
        appearance_names = appearance_names_alien;
        armour_names     = armour_names_alien;
        if (man->md.SkinType == S_SECTOID)
            sol_dialog[D_RACE].d1 = 1;
        else
        if (man->md.SkinType == S_MUTON)
            sol_dialog[D_RACE].d1 = 2;
        else
            sol_dialog[D_RACE].d1 = 3;
        man->md.fFemale = 0;
        if (man->md.SkinType != S_CHAMELEON)
            man->md.Appearance = 0;
    }

    switch (man->md.SkinType) {
        case S_XCOM_0 : sol_dialog[D_ARMOUR].d1 = 0; break;
        case S_XCOM_1 : sol_dialog[D_ARMOUR].d1 = 1; break;
        case S_XCOM_2 : sol_dialog[D_ARMOUR].d1 = 2; break;
        case S_XCOM_3 : sol_dialog[D_ARMOUR].d1 = 3; break;
        case S_SECTOID: sol_dialog[D_ARMOUR].d1 = 0; break;
        default:        sol_dialog[D_ARMOUR].d1 = 0; break;
    }

    sol_dialog[D_APPEARANCE].d1 = man->md.Appearance + (man->md.fFemale ? 4 : 0);

    fixup_unit_info();

    set_dialog_color(sol_dialog, gui_fg_color, gui_bg_color);
    centre_dialog(sol_dialog);
    popup_dialog(sol_dialog, -1);

    switch (sol_dialog[D_RACE].d1) {
        case  0: 
            man->md.fFemale = sol_dialog[D_APPEARANCE].d1 >= 4;
            man->md.Appearance = sol_dialog[D_APPEARANCE].d1 % 4;
            man->md.SkinType = sol_dialog[D_ARMOUR].d1 + 1; 
            break;
        case  1: 
            man->md.SkinType = S_SECTOID; 
            break;
        case  2: 
            man->md.SkinType = S_MUTON; 
            break;
        default: 
            man->md.SkinType = S_CHAMELEON; 
            man->md.Appearance = sol_dialog[D_APPEARANCE].d1;
            break;
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

    ::sol_dialog = NULL;
}

void Editor::copy_soldier(Soldier *src)
{
    buffer.empty = false;
    
    src->build_ITEMDATA();
    
    buffer.md = src->md;
    buffer.id = src->id;
}

void Editor::paste_soldier(Soldier *dest)
{
    if (buffer.empty) return;
    
    std::string prev_name = dest->md.Name;
    
    dest->md = buffer.md;
    dest->id = buffer.id;
    
    for (int i = 0; i < 26; i++)
        dest->md.Name[i] = prev_name[i];
    
    dest->process_MANDATA();
    dest->process_ITEMDATA();
}
         
/**
 * Let the user select a set of equipment available in the armory, 
 * e.g. "standard", "no explosives", "no alien weapons" etc.
 *
 * Query which sets are defined in ./init-scripts/standard-equipment.lua,
 * then present the names of those sets in a popup-box to the user.
 */
void Editor::change_equipment()
{
    std::vector<std::string> eqsets;
    int index = query_equipment_sets(eqsets);
    if (eqsets.size() > 0) {
        int result = gui_select_from_list(
            300, 200, _("Select equipment set"), 
            eqsets, index != -1 ? index : 0);

        if (set_current_equipment_name(eqsets[result].c_str()))
            net->send_equipment_choice();

    } else {
        alert( "", _("Remote player does not have any of your weaponsets"), "", _("OK"), NULL, 0, 0);
    }
}

void Editor::scroll_equipment(int delta)
{
    std::vector<std::string> eqsets;
    int index = query_equipment_sets(eqsets);
    if (eqsets.size() == 0) return;
    int new_index = index != -1 ? index : 0;
    new_index = (eqsets.size() + new_index + delta) % eqsets.size();
    if (new_index != index && set_current_equipment_name(eqsets[new_index].c_str()))
        net->send_equipment_choice();
}
