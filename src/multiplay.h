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
#ifndef MULTIPLAY_H
#define MULTIPLAY_H

#include "map.h"
#include "connect.h"
#include "packet.h"
#undef map
#include "server_protocol.h"
#define map g_map

enum GameType {GAME_TYPE_HOTSEAT, GAME_TYPE_INTERNET_SERVER, GAME_TYPE_REPLAY};

void inithotseatgame();
void closehotseatgame();

/**
 * High level object providing network support. Uses some kind of RPC 
 * for sending local changes to the remote player. Network packets are received,
 * arguments unpacked and proper functions called to keep game state in sync.
 * 
 * @ingroup planner
 * @ingroup battlescape
 * @ingroup network
 */
class Net
{
public:
    int SEND;

    void send(const std::string &packet);
    void send(const char *buf, int size) { send(std::string(buf, size)); }
    int recv(std::string &pkt);

private:
    void send();

    FILE *flog;
    BQ *queue;
    Packet pkt;

    Item *itaken;
    Connect *connect;

    Soldier *findman(int NID);

    int recv_notice();
    int recv_quit();
    int recv_restart();
    int recv_endturn();
    int recv_open_door();
    int recv_change_pose();
    int recv_reserve_time(); //recv time reserving change
    int recv_prime_grenade();
    int recv_unload_ammo();
    int recv_load_ammo();

    int recv_detonate_item();
    int recv_explode();

    int recv_select_item();
    int recv_deselect_item();

    int recv_move();
    int recv_face();
    int recv_use_elevator();
    int recv_target_action();

    int recv_shoot();

    int recv_add_unit();
    int recv_select_unit();
    int recv_deselect_unit();
    int recv_unit_data_size();
    int recv_unit_data();
    int recv_map_data();
    int recv_time_limit();
    int recv_p2_start_sit(); //player 2 starts sitting
    int recv_terrain_crc32();
    int recv_scenario();
    int recv_rules();
    int recv_options();
    
    int recv_panic();
    int recv_morale_change();
    int recv_equipment_list();
    int recv_equipment_choice();
    int recv_tie();
    int recv_initrand();
    
    int recv_recovery_stop();
    int recv_start_visible_recovery();
    
public:
    std::istream *m_ireplay_file;
    std::ostream *m_oreplay_file;
    
    ClientServerUfo *m_internet_server;
    GameType gametype;

    int is_network_game() { return gametype == GAME_TYPE_INTERNET_SERVER; }

    Net();
    ~Net();
    int init();
    void close();
    void check();
    void log(const char *fmt, ...);
    void error(char *str);

    void send_message(const std::string &msg);

    void send_debug_message(const char *fmt, ...);
    void flush();

    void send_notice();
    void send_quit();
    void send_restart();
    void send_endturn(int crc, const std::string &data);

    void send_open_door(int NID);
    void send_change_pose(int NID);
    void send_reserve_time(int NID, int res); //change soldiers time reserving
    void send_prime_grenade(int NID, int iplace, int delay_time, int req_time);
    void send_unload_ammo(int NID);
    void send_load_ammo(int NID, int iplace, int srcplace);

    void send_detonate_item(int owner, int lev, int col, int row, int iplace, int ix, int iy);
    void send_explode(int owner, int lev, int col, int row, int type, int range, int damage);

    void send_select_item(int NID, int iplace, int ix, int iy);
    void send_deselect_item(int NID, int iplace, int ix, int iy, int req_time);

    void send_move(int NID, int lev, int col, int row);
    void send_face(int NID, int col, int row);
    void send_use_elevator(int NID, int dz);
    void send_target_action(int NID, int z0, int x0, int y0, int zd, int xd, int yd, Action action, int iplace);

    void send_add_unit(int num, char *name, int cost);
    void send_select_unit(int num, int lev, int col, int row);
    void send_deselect_unit(int num);

    void send_unit_data_size(int size);
    void send_unit_data(int num, int lev, int col, int row, MANDATA *md, ITEMDATA *id);
    void send_map_data(GEODATA *gd);
    void send_time_limit(int time_limit);
    void send_p2_start_sit(int is_sit);
    void send_terrain_crc32(const std::string &name, uint32 crc32);
    void send_scenario();
    void send_rules(int index, int value);
    void send_options(int scenario_type, int index, int value);
    
    void send_panic(int NID, int action);
    void send_morale_change(int delta);
    void send_equipment_list();
    void send_equipment_choice();
    void send_tie(int who);
    void send_initrand(int init_num);

    void send_finish_planner();
    int recv_finish_planner();
};

#endif
