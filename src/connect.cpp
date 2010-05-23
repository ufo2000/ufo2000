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
#include "keys.h"
#include "spk.h"
#include "wind.h"
#include "multiplay.h"
#include "connect.h"
#include "platoon.h"
#include "units.h"
#include "version.h"
#include "music.h"
#include "scenario.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"
#include "script_api.h"

/**
 * @file connect.cpp
 * @brief Routines for Network-play (LAN, Internet)
 */

/**
 * Synchronizing of client versions for network game
 *
 * @todo remove all the unneeded stuff here, this function does not need 
 *       displaying any graphics at all
 */
int Connect::do_version_check()
{
    const char *you_are_too_old_msg =
        _("Unfortunately you have an older UFO2000 version than your opponent has. "
          "And the changes introduced between these versions make them incompatible.\n\n"
          "Please visit http://ufo2000.sourceforge.net and upgrade "
          "(you need UFO2000 version %s.%d or newer).");
    const char *opponent_is_too_old_msg =
        _("Unfortunately your opponent has an older version of UFO2000. "
          "And the changes introduced between these versions make them incompatible.\n\n"
          "Please try again after your opponent upgrades to an up to date version.");
    bool version_check_passed = false;
    bool version_check_tried = false;
    bool result = true;

    int DONE = 0;
    std::string buf;

    switch (net->gametype) {
        case GAME_TYPE_INTERNET_SERVER:
            break;
        default:
            ASSERT(false);
            break;
    }

    clear(screen);
    text_mode(-1);
    textprintf(screen, font, 1, 1, COLOR_SYS_INFO1, "%s",
        _("Comparing local and remote UFO2000 versions..."));
    textprintf(screen, font, 1, 1 + text_height(font), COLOR_SYS_INFO1, "%s",
        _("Press ESC to cancel if it takes too long."));

    g_net_allowed_terrains.clear();

    // The version check protocol is simple:
    // First both sides send the information about their version.
    // Then they wait for a reply from the remote side with "START" or "QUIT"
    // string. If any of the clients sends "QUIT", then the game can't
    // be started and this function should return false.
    // In any case, we try to send exactly two packets to the remote side
    // (version info + START or QUIT).
    // Receiving of the same two packets from the remote side is not necessary
    // if we are not going to start the game.

    net->send(string_format("UFO2000 REVISION OF YOUR OPPONENT: %d",
        UFO_REVISION_NUMBER));

    while (!DONE) {
        if (net->recv(buf)) {
            int remote_revision;
            if (sscanf(buf.c_str(), "UFO2000 REVISION OF YOUR OPPONENT: %d", &remote_revision) == 1) {
                version_check_tried = true;
                if (UFO_REVISION_NUMBER == remote_revision) {
                    net->send("START");
                    version_check_passed = true;
                } else {
                    net->send("QUIT");
                    result = false;
                    if (remote_revision < UFO_REVISION_NUMBER) {
                        show_help(opponent_is_too_old_msg);
                    } else {
                        show_help(string_format(you_are_too_old_msg,
                            UFO_VERSION_STRING, remote_revision).c_str());
                    }
                }
            } else if (strstr(buf.c_str(), "QUIT") != NULL) {
                result = false;
                DONE = 1;
            } else if (strstr(buf.c_str(), "START") != NULL) {
                if (!version_check_passed)
                    result = false;
                DONE = 1;
            }
        } else if (keypressed()) {
            int scancode;
            ureadkey(&scancode);
            if (scancode == KEY_ESC) {
                result = false;
                DONE = 1;
            }
        }
        rest(10);
    }

    if (!version_check_tried)
        net->send("QUIT");

    clear(screen);
    net->SEND = result;
    return net->SEND;
}

#include "map.h"
#include "editor.h"
#include "wind.h"

Units local;
Units remote;
// Pointers to "Units" in order of their positions (HOST/JOIN), not local/remote choice.
Units* target_uints[2];

// uds ??
void Connect::reset_uds()
{
    local.reset();
    remote.reset();
}

void Connect::swap_uds()
{
    Units u;
    memcpy(&u,      &local,  sizeof(Units));
    memcpy(&local,  &remote, sizeof(Units));
    memcpy(&remote, &u,      sizeof(Units));
}

int FINISH_PLANNER = 0;

/**
 * Mission-Planner: select map, rules+scenario, set up team of soldiers.
 * In network-game, also chat with opponent.
 */
int Connect::do_planner(int F10ALLOWED, int map_change_allowed)
{
    lua_message( "Enter: Connect::do_planner" );
    MODE = PLANNER;
    int mouse_leftr = 1, mouse_rightr = 1;
    int DONE = 0;
	int REMOTE_LEFT = 0;
    //HOST = 0;
    FINISH_PLANNER = 0;

    int old_SCREEN2H = SCREEN2H;
    SCREEN2H = 380;

    reset_video();
    destroy_bitmap(screen2);
    screen2 = create_bitmap(640, SCREEN2H); clear(screen2);
    MouseRange temp_mouse_range(0, 0, 639, SCREEN2H - 1);

    g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
    g_console->set_full_redraw();
    g_console->redraw(screen, 0, SCREEN2H);

    g_console->printf( COLOR_SYS_HEADER, "%s", _("Welcome to the mission-planner !") );

    Map *map = new Map(mapdata);
    BITMAP *map2d = map->create_bitmap_of_map(0);
    int map2d_x = (640 - map2d->w) / 2;
	
    if (HOST) {
        target_uints[1] = &remote;
        target_uints[0] = &local;
        local.set_pos(POS_LEFT,   map2d_x - (MAN_NAME_LEN * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
        remote.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
        pd_local = &pd1;
        pd_remote = &pd2;
    } else {
        target_uints[0] = &remote;
        target_uints[1] = &local;
        remote.set_pos(POS_LEFT, map2d_x - (MAN_NAME_LEN * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
        local.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
        pd_remote = &pd1;
        pd_local = &pd2;
    }
    target_uints[0]->Position = 1;
    target_uints[1]->Position = 2;

    local.store_mouse_range(map2d_x, 0, map2d_x + map2d->w - 1, map2d->h - 1);
    editor->build_Units(local);

    if (net->is_network_game()) {
        // synchronize available equipment with remote machine
        net->send_equipment_list();
    } else {
        // synchronize available equipment with ourselves :)
        lua_pushstring(L, "SyncEquipmentInfo");
        lua_gettable(L, LUA_GLOBALSINDEX);
        lua_pushstring(L, "QueryEquipmentInfo");
        lua_gettable(L, LUA_GLOBALSINDEX);
        lua_safe_call(L, 0, 1);
        lua_safe_call(L, 1, 0);
    }

    // Synchronize available terrains list
    if (net->is_network_game()) {
#undef map
        std::map<int, Terrain *>::iterator it = terrain_set->terrain.begin();
        while (it != terrain_set->terrain.end()) {
            net->send_terrain_crc32(it->second->get_name(), it->second->get_crc32());
            it++;
        }
        net->send_terrain_crc32("", 0);
#define map g_map
		
        // Wait until a complete list of remote terrains is received
        while ((g_net_allowed_terrains.find("") == g_net_allowed_terrains.end()) && (!REMOTE_LEFT)) {
            net->check();
            rest(1);
			
			process_keyswitch();

			if (keypressed()) {
				CHANGE = 1;

				int scancode;
				int keycode = ureadkey(&scancode);

				switch (scancode) {
					case KEY_ESC:
						if (askmenu( _("EXIT MISSION-PLANNER") )) {
							net->send_quit();
							net->SEND = 0;
							REMOTE_LEFT = 1;
						}
						break;
					default:
						if (g_console->process_keyboard_input(keycode, scancode))
							net->send_message((char *)g_console->get_text());
				}
			}
        }
        // Remove end marker
        g_net_allowed_terrains.erase("");

		if (REMOTE_LEFT) {
		    delete map;
            SCREEN2H = old_SCREEN2H;
            destroy_bitmap(screen2);
            screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);
            clear(screen);
            g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
            g_console->set_full_redraw();
            return 0;
		}
		
        if (g_net_allowed_terrains.size() == 0) {
            alert( "", _("Remote player does not have any of your maps"), "",
                   _("OK"), NULL, 0, 0);
            delete map;

            SCREEN2H = old_SCREEN2H;
            destroy_bitmap(screen2);
            screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);
            clear(screen);
            g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
            g_console->set_full_redraw();
            return 0;
        }
    }

    // Make sure that we have a valid map
    if (!Map::valid_GEODATA(&mapdata)) {
        Map::new_GEODATA(&mapdata);
    }

    // Try to set the last weaponset or use the first one from the list of available
    // In the worst case we will have just empty armoury
    if (!set_current_equipment_name(g_default_weaponset.c_str())) {
        std::vector<std::string> weaponsets;
        query_equipment_sets(weaponsets);
        if (weaponsets.size() > 0) set_current_equipment_name(weaponsets[0].c_str());
    }

    mapdata.load_game = 77;

    if (HOST && !g_game_receiving) {
        // HOST player sets initial map, scenario and weaponset
        int i;
        net->send_map_data(&mapdata);
        net->send_time_limit(g_time_limit);
        net->send_scenario();
        net->send_equipment_choice();
        for (i = 0; i < 5; i++)
            net->send_rules(i, scenario->rules[i]);
        for (i = 0; i < SCENARIO_NUMBER; i++)
            for (int j = 0; j < 3; j++)
                net->send_options(i, j, scenario->options[i][j]->value);
    }
    else //send start_sit info now
    {
        net->send_p2_start_sit((FLAGS & F_SECONDSIT)?1:0);
    }

    if (map_change_allowed)
        g_console->printf( COLOR_SYS_OK,   _("You can select a map and change the match-settings.") );
    else
        g_console->printf( COLOR_SYS_FAIL, _("The map is already set for this game, and cannot be changed.") );
    g_console->printf( COLOR_SYS_INFO1,    _("To edit a soldier, CTRL-click on his name.") );
    g_console->printf( COLOR_SYS_INFO1,    _("Left-click to place a soldier on the map, right-click to remove him.") );
    g_console->printf( COLOR_SYS_PROMPT,   _("When finished, click SEND, then START.  Press ESC to quit, F1 for help.") ); 
	
    while (!DONE) {

        rest(1); // Don't eat all CPU resources
        g_console->redraw(screen, 0, SCREEN2H);

        net->check();
        if (mapdata.load_game == 77) { //new    mapdata
            //g_console->printf("suggested map: %s\n",
            //  terrain_set->get_terrain_name(mapdata.terrain).c_str());
            mapdata.load_game = 0;
            delete map;
            destroy_bitmap(map2d);
            map = new Map(mapdata);
            map2d = map->create_bitmap_of_map(0);
            map2d_x = (640 - map2d->w) / 2;

            local.reset_selections();
            remote.reset_selections();

            if (HOST) {
                local.Position = 1;
                remote.Position = 2;
                local.set_pos(POS_LEFT,   map2d_x - (MAN_NAME_LEN * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
                remote.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
            } else {
                local.Position = 2;
                remote.Position = 1;
                remote.set_pos(POS_LEFT,  map2d_x - (MAN_NAME_LEN * 8 + 20), 10, map2d_x, map2d->w, 0, map2d->h);
                local.set_pos(POS_RIGHT, map2d_x + map2d->w + 20, 10, map2d_x, map2d->w, 0, map2d->h);
            }
            local.store_mouse_range(map2d_x, 0, map2d_x + map2d->w - 1, map2d->h - 1);
            local.deselect();

            CHANGE = 1;
        }

        if (CHANGE) { //!!build screen
            clear(screen2);
            //map->show2d(0, 0);
            blit(map2d, screen2, 0, 0, map2d_x, 0, map2d->w, map2d->h);

            if(!g_game_receiving) {
                remote.print_simple(COLOR_WHITE);
                local.print(COLOR_WHITE);
            }
            
            draw_alpha_sprite(screen2, mouser, mouse_x, mouse_y);
            blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);

            CHANGE = 0;
        }

        if ((mouse_b & 1) && (mouse_leftr)) //left
        {
            mouse_leftr = 0;
            CHANGE = 1;
            local.execute(map, map_change_allowed);
            // Calls Editor::show() via Units::execute and execute_main()

            //  "START"
            if (mouse_inside(local.gx + 5 * 8 - 20, SCREEN2H - 20, local.gx + 5 * 8 + 20, SCREEN2H - 5))
            {
                if (F10ALLOWED && local.SEND && askmenu( _("PREPARATIONS FOR MISSION FINISHED") ))
                    DONE = 1;
            }
        }

        if ((mouse_b & 2) && (mouse_rightr)) { //right
            mouse_rightr = 0;
            CHANGE = 1;
            local.execute_right();
        }

        if (!(mouse_b & 1)) {
            mouse_leftr = 1;
        }

        if (!(mouse_b & 2)) {
            mouse_rightr = 1;
        }

        process_keyswitch();

        if (keypressed()) {
            CHANGE = 1;

            int scancode;
            int keycode = ureadkey(&scancode);

            switch (scancode) {
                case KEY_F1:
                    help( HELP_PLANNER );
                    break;
                // Todo: Save+Load for teams
                case KEY_F2: 
                  //Editor::save();
                    break;
                case KEY_F3: 
                  //Editor::load();
                    break;
                case KEY_F4: 
                    local.edit_unit(0);
                    break;
                case KEY_F5: // Toggle F_RAWMESSAGES
                    FLAGS ^= F_RAWMESSAGES;
                    g_console->printf( COLOR_SYS_INFO1, "%s: %lu", "RAWMESSAGES:", (FLAGS & F_RAWMESSAGES) );
                    break;
                case KEY_F9:
                    keyswitch(0);
                    break;
                case KEY_F10:
                    change_screen_mode();
                    break;
                case KEY_PRTSCR:
                    if (askmenu(_("SCREEN-SNAPSHOT"))) {
                        savescreen();
                    }
                    break;
                case KEY_ASTERISK:   // ?? ToDo: Sound+Music on/off
                    FS_MusicPlay(NULL);
                    g_console->printf(COLOR_SYS_FAIL, _("Music OFF") );
                    break;
                case KEY_ESC:
                    if (askmenu( _("EXIT MISSION-PLANNER") )) {
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

    local.restore_mouse_range();
    delete map;

    g_console->printf(COLOR_SYS_INFO2, "------");
    SCREEN2H = old_SCREEN2H;
    destroy_bitmap(screen2);
    screen2 = create_bitmap(SCREEN2W, SCREEN2H); clear(screen2);
    clear(screen);
    g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
    g_console->set_full_redraw();
    return net->SEND;
}
