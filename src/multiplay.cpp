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
#include <time.h>
#include <list>
#include "video.h"
#include "wind.h"
#include "explo.h"
#include "soldier.h"
#include "platoon.h"
#include "multiplay.h"
#include "map.h"
#include "sound.h"
#include "netsock.h"
#include "packet.h"
#include "units.h"
#include "pfxopen.h"

static std::list<std::string> g_hotseat_cmd_queue;

void packet_send_hotseat(const std::string &data)
{
	g_hotseat_cmd_queue.push_back(data);
}

int packet_recv_hotseat(std::string &packet)
{
	if (g_hotseat_cmd_queue.empty()) {
		packet = "";
		return 0;
	}

	packet = g_hotseat_cmd_queue.front();
	g_hotseat_cmd_queue.pop_front();
	return packet.size();
}

void inithotseatgame()
{
	while (!g_hotseat_cmd_queue.empty()) g_hotseat_cmd_queue.pop_front();
}

void closehotseatgame()
{
	while (!g_hotseat_cmd_queue.empty()) g_hotseat_cmd_queue.pop_front();
}

void Net::error(char *str)
{
	g_console->printf("___error in %s___", str);
	log("%s\n", str);
}

Net::Net()
{
	SEND = 1;
	queue = new BQ(1000);
	connect = new Connect();
	flog = FOPEN_RTEMP("ufo2000.log", "wt");
	assert(flog != NULL);
	log("%s\n", "_____________Net()");
}

Net::~Net()
{
	log("%s\n", "_____________~Net()");
	fclose(flog);
	delete connect;
	delete queue;
}

void Net::log(const char *fmt, ...)
{
	time_t now = time(NULL);
	struct tm * t = localtime(&now);
	char timebuf[1000];
	strftime(timebuf, 1000, "%d/%m/%Y %H:%M:%S", t);

	va_list arglist;
	va_start(arglist, fmt);

	fprintf(flog, "%s ", timebuf);
	vfprintf(flog, fmt, arglist);
	fflush(flog);

	va_end(arglist);
}

int Net::init()
{
	log("%s\n", "init()");
	SEND = 1;

	connect->reset_uds();
	if (gametype == HOTSEAT) {
		HOST = 1;
		if (!connect->do_planner(1))
			return 0;
		clear(screen);
		reset_video();
		set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
		alert(" ", "  NEXT PLAYER  ", " ", "    OK    ", NULL, 1, 0);
		HOST = 0;
		connect->swap_uds();
		if (!connect->do_planner(1, 0))
			return 0;
		HOST = 1;
		clear(screen);
		reset_video();
		set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
		alert(" ", "  GAME START  ", " ", "    OK    ", NULL, 1, 0);
		inithotseatgame();
	} else {
		if (!connect->do_chat() || !connect->do_planner(0)) {
			close();
			return 0;
		}
	}
	return 1;
}

void Net::close()
{
	log("%s\n", "close()");
	switch (gametype) {
		case SOCK:
			closesocketgame();
			break;
		case HOTSEAT:
			closehotseatgame();
			break;
		default:
			break;
	}
}

Soldier *Net::findman(int NID)
{
	Soldier *ss = platoon_remote->findman(NID);
	if (ss == NULL) ss = platoon_local->findman(NID);

	return ss;
}

void Net::send_message(const std::string &msg)
{
	send(std::string("_Xmes_") + msg);
}

void Net::send()
{
	assert(pkt.str_len() > 0);
	send(pkt.str());
}

void Net::send(const std::string &pkt)
{
	if (FLAGS & F_RAWMESSAGES) g_console->printf("send:[%s]", pkt.c_str());

	log("send:[%s]\n", pkt.c_str());

	switch (gametype) {
		case SOCK:
			packet_send_socket((char *)pkt.data(), pkt.size()); // $$$
			break;
		case HOTSEAT:
			if (MODE != PLANNER)
				packet_send_hotseat(pkt);
			break;
		case GAME_TYPE_INTERNET_SERVER:
			m_internet_server->send_packet(SRV_GAME_PACKET, pkt);
			break;
		default:
			assert(false);
	}
}

int Net::recv(std::string &pkt)
{
	pkt = "";
	switch (gametype) {
		case SOCK: { // $$$
			char temp[1000];
			int size = packet_recv_socket(temp);
			pkt = std::string(temp, size);
			return size;
		}
		case HOTSEAT:
			if (MODE != WATCH) return 0;
			packet_recv_hotseat(pkt);
			return pkt.size();
		case GAME_TYPE_INTERNET_SERVER:
			NLulong id;
			if (!m_internet_server->recv_packet(id, pkt) || id != SRV_GAME_PACKET) {
				pkt = "";
				return 0;
			} else {
				return pkt.size();
			}
		default:
			assert(false);
			break;
	}
	return 0;
}

extern int GAMELOOP;

void Net::check()
{
	std::string packet;

	if (gametype == GAME_TYPE_INTERNET_SERVER)
		m_internet_server->send_delayed_packet();

	recv(packet);

	if (!packet.empty()) {
		queue->put((char *)packet.data(), packet.size());
		if (FLAGS & F_RAWMESSAGES) {
			g_console->printf("put:[%d]", packet.size());
			g_console->printf("%s", packet.c_str());
		}
	}

	if (GAMELOOP && (!nomoves()))
		return ;

	if (!queue->get(packet)) return;

	if (FLAGS & F_RAWMESSAGES) {
		g_console->printf("get:[%d]", packet.size());
		g_console->printf("%s", packet.c_str());
	}

	log("recv:[%s]\n", packet.c_str());
	switch (pkt.command((char *)packet.data(), packet.size())) {
		case CMD_NOTICE:
			recv_notice();
			break;
		case CMD_QUITGAME:
			recv_quit();
			break;
		case CMD_RESTARTGAME:
			recv_restart();
			break;
		case CMD_ENDTURN:
			recv_endturn();
			break;
		case CMD_OPENDOOR:
			recv_open_door();
			break;
		case CMD_CHANGEPOSE:
			recv_change_pose();
			break;
		case CMD_PRIMEGRENADE:
			recv_prime_grenade();
			break;
		case CMD_UNLOAD_AMMO:
			recv_unload_ammo();
			break;
		case CMD_LOAD_AMMO:
			recv_load_ammo();
			break;
		case CMD_DETONATE_ITEM:
			recv_detonate_item();
			break;
		case CMD_EXPLODE:
			recv_explode();
			break;
		case CMD_TAKE_ITEM:
			recv_select_item();
			break;
		case CMD_DROP_ITEM:
			recv_deselect_item();
			break;
		case CMD_MOVE:
			recv_move();
			break;
		case CMD_FACE:
			recv_face();
			break;
		case CMD_PUNCH:
			recv_punch();
			break;
		case CMD_THROW_ITEM:
			recv_thru();
			break;
		case CMD_AIMEDTHROW:
			recv_aimedthrow();
			break;
		case CMD_BEAM_LASER:
			recv_beam();
			break;
		case CMD_FIRE_GUN:
			recv_fire();
			break;
		case CMD_ADD_UNIT:
			recv_add_unit();
			break;
		case CMD_SELECT_UNIT:
			recv_select_unit();
			break;
		case CMD_DESELECT_UNIT:
			recv_deselect_unit();
			break;
		case CMD_UNIT_DATA_SIZE:
			recv_unit_data_size();
			break;
		case CMD_UNIT_DATA:
			recv_unit_data();
			break;
		case CMD_MAP_DATA:
			recv_map_data();
			break;
		case CMD_TIME_LIMIT:
			recv_time_limit();
			break;
		case CMD_TERRAIN_CRC32:
			recv_terrain_crc32();
			break;
		case CMD_FINISH_PLANNER:
			recv_finish_planner();
			break;
		case CMD_MESSAGE:
			soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
			g_console->print(pkt.str(), xcom1_color(32));
			break;
		case CMD_NONE:
			assert(false);
			break;
	};
}

void Net::send_notice()
{
	if (!SEND) return ;

	pkt.create(CMD_NOTICE);
	send();
}

int Net::recv_notice()
{ // "NOTE"
	NOTICEremote = 0;
	return 1;
}

void Net::send_quit()
{
	if (!SEND) return ;

	pkt.create(CMD_QUITGAME);
	send();
}

int Net::recv_quit()
{ // "QUIT"
	error("Remote exit from game");
	return 1;
}

extern void restartgame();
int CONFIRM_REQUESTED = 0;

void Net::send_restart()
{
	if (!SEND) return ;

	pkt.create(CMD_RESTARTGAME);
	pkt << CONFIRM_REQUESTED;
	send();

	if (CONFIRM_REQUESTED) {
		restartgame();
		CONFIRM_REQUESTED = 0;
	}
}

int Net::recv_restart()
{ //"REST"
	int CONFIRMED;

	pkt >> CONFIRMED;
	if (CONFIRMED) {
		restartgame();
		CONFIRM_REQUESTED = 0;
	}

	if (!CONFIRMED) {
		error("Remote want restart game, press F5 to confirm");
		CONFIRM_REQUESTED = 1;
	}
	return 1;
}

void Net::send_endturn(int crc)
{
	if (!SEND) return ;

	pkt.create(CMD_ENDTURN);
	pkt << crc;
	send();
}

int Net::recv_endturn()
{ // "TURN"
	int crc; pkt >> crc;
	recv_turn(crc);
	return 1;
}


void Net::send_open_door(int NID)
{
	if (!SEND) return ;

	pkt.create(CMD_OPENDOOR);
	pkt << NID;
	send();
}

int Net::recv_open_door()
{ //DOOR
	int NID;

	pkt >> NID;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->open_door())
			error("NID can't open door");
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}


void Net::send_change_pose(int NID)
{
	if (!SEND) return ;

	pkt.create(CMD_CHANGEPOSE);
	pkt << NID;
	send();
}

int Net::recv_change_pose()
{ // "POSE"
	int NID;

	pkt >> NID;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->change_pose()) {
			error("NID can't change_pose");
		}
		SEND = 1;
		return 1;
	} else {
		error("NID");
	}
	return 0;
}



void Net::send_prime_grenade(int NID, int iplace, int delay_time, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_PRIMEGRENADE);
	pkt << NID;
	pkt << iplace;
	pkt << delay_time;
	pkt << req_time;
	send();
}

int Net::recv_prime_grenade()
{ // "PRIM"
	int NID, iplace, delay_time, req_time;

	pkt >> NID;
	pkt >> iplace;
	pkt >> delay_time;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->prime_grenade(iplace, delay_time, req_time))
			error("NID can't prime_grenade");
		SEND = 1;
		return 1;
	} else {
		error("NID");
	}
	return 0;
}



void Net::send_unload_ammo(int NID)
{
	if (!SEND) return ;

	pkt.create(CMD_UNLOAD_AMMO);
	pkt << NID;
	send();
}

int Net::recv_unload_ammo()
{ // "UNLO"
	int NID;

	pkt >> NID;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->unload_ammo(itaken)) {
			error("NID can't unload ammo");
		}
		SEND = 1;
		return 1;
	} else {
		error("NID");
	}
	return 0;
}


void Net::send_load_ammo(int NID, int iplace)
{
	if (!SEND) return ;

	pkt.create(CMD_LOAD_AMMO);
	pkt << NID;
	pkt << iplace;
	send();
}

int Net::recv_load_ammo()
{ // "LOAD"
	int NID, iplace;

	pkt >> NID;
	pkt >> iplace;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->load_ammo(iplace, itaken)) {
			error("NID can't load ammo");
			return 1;
		}
		SEND = 1;
		return 1;
	} else {
		error("NID");
	}
	return 0;
}

/*
void Net::send_detonate_item(int owner, int lev, int col, int row, int iplace, int ix, int iy)
{
	if (!SEND) return ;

	pkt.create(CMD_DETONATE_ITEM);
	pkt << lev;
	pkt << col;
	pkt << row;
	pkt << iplace;
	pkt << ix;
	pkt << iy;
	pkt << owner;
	send();
}
*/

int Net::recv_detonate_item()
{ // "DETO"
	int lev, col, row, iplace, ix, iy, owner;

	pkt >> lev;
	pkt >> col;
	pkt >> row;
	pkt >> iplace;
	pkt >> ix;
	pkt >> iy;
	pkt >> owner;

	SEND = 0;
	if (!elist->detonate(owner, lev, col, row, iplace, ix, iy)) {
		error("EXPLO can't detonate item");
	}
	SEND = 1;
	return 1;
}

/*
void Net::send_explode(int owner, int lev, int col, int row, int type, int range, int damage)
{
	if (!SEND) return ;

	pkt.create(CMD_EXPLODE);
	pkt << lev;
	pkt << col;
	pkt << row;
	pkt << type;
	pkt << range;
	pkt << damage;
	pkt << owner;
	send();
}
*/

int Net::recv_explode()
{ // "EXPL"
	int lev, col, row, type, range, damage, owner;

	pkt >> lev;
	pkt >> col;
	pkt >> row;
	pkt >> type;
	pkt >> range;
	pkt >> damage;
	pkt >> owner;

	SEND = 0;
	if (!map->explode(owner, lev, col, row, type, range, damage)) {
		error("MAP can't explode");
	}
	SEND = 1;
	return 1;
}



void Net::send_select_item(int NID, int iplace, int ix, int iy)
{
	if (!SEND) return ;

	pkt.create(CMD_TAKE_ITEM);
	pkt << NID;
	pkt << iplace;
	pkt << ix;
	pkt << iy;
	send();
}

int Net::recv_select_item()
{ //"TAKE"
	int NID, iplace, ix, iy;

	pkt >> NID;
	pkt >> iplace;
	pkt >> ix;
	pkt >> iy;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		itaken = ss->getitem(iplace, ix, iy);
		SEND = 1;
		if (itaken == NULL) {
			error("takeitem==NULL");
		} else
			return 1;
	} else {
		error("NID");
	}
	return 0;
}


void Net::send_deselect_item(int NID, int iplace, int ix, int iy, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_DROP_ITEM);
	pkt << NID;
	pkt << iplace;
	pkt << ix;
	pkt << iy;
	pkt << req_time;
	send();
}

int Net::recv_deselect_item()
{ //"DROP"
	int NID, iplace, ix, iy, req_time;

	pkt >> NID;
	pkt >> iplace;
	pkt >> ix;
	pkt >> iy;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		if (itaken != NULL) {
			SEND = 0;
			if (!ss->putitem(itaken, iplace, ix, iy))
				error("can't put item");
			ss->spend_time(req_time);
			SEND = 1;
			return 1;
		} else {
			error("dropitem==NULL");
		}
	} else {
		error("NID");
	}

	return 0;
}


void Net::send_move(int NID, int lev, int col, int row)
{
	if (!SEND) return ;

	pkt.create(CMD_MOVE);
	pkt << NID;
	pkt << lev;
	pkt << col;
	pkt << row;
	send();
}

int Net::recv_move()
{ // "MOVE"
	int NID, lev, col, row;

	pkt >> NID;
	pkt >> lev;
	pkt >> col;
	pkt >> row;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		ss->wayto(lev, col, row);
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}


void Net::send_face(int NID, int col, int row)
{
	if (!SEND) return ;

	pkt.create(CMD_FACE);
	pkt << NID;
	pkt << col;
	pkt << row;
	send();
}

int Net::recv_face()
{ // "FACE"
	int NID, col, row;

	pkt >> NID;
	pkt >> col;
	pkt >> row;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		ss->faceto(col, row);
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}

void Net::send_thru(int NID, int z0, int x0, int y0, REAL ro, REAL fi, REAL te, REAL zA, int iplace, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_THROW_ITEM);
	pkt << NID;
	pkt << z0;
	pkt << x0;
	pkt << y0;
	pkt << ro;
	pkt << fi;
	pkt << te;
	pkt << zA;
	pkt << iplace;
	pkt << req_time;
	send();
}

int Net::recv_thru()
{
	int NID, z0, x0, y0, iplace, req_time;
	REAL ro, fi, te, zA;

	pkt >> NID;
	pkt >> z0;
	pkt >> x0;
	pkt >> y0;
	pkt >> ro;
	pkt >> fi;
	pkt >> te;
	pkt >> zA;
	pkt >> iplace;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->thru(z0, x0, y0, ro, fi, te, zA, iplace, req_time)) {
			error("NID can't throw item");
		}
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}

void Net::send_beam(int NID, int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_BEAM_LASER);
	pkt << NID;
	pkt << z0;
	pkt << x0;
	pkt << y0;
	pkt << fi;
	pkt << te;
	pkt << iplace;
	pkt << req_time;
	send();
}

int Net::recv_beam()
{
	int NID, z0, x0, y0, iplace, req_time;
	REAL fi, te;

	pkt >> NID;
	pkt >> z0;
	pkt >> x0;
	pkt >> y0;
	pkt >> fi;
	pkt >> te;
	pkt >> iplace;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->beam(z0, x0, y0, fi, te, iplace, req_time)) {
			error("NID can't beam laser");
		}
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}


void Net::send_fire(int NID, int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_FIRE_GUN);
	pkt << NID;
	pkt << z0;
	pkt << x0;
	pkt << y0;
	pkt << fi;
	pkt << te;
	pkt << iplace;
	pkt << req_time;
	send();
}

int Net::recv_fire()
{
	int NID, z0, x0, y0, iplace, req_time;
	REAL fi, te;

	pkt >> NID;
	pkt >> z0;
	pkt >> x0;
	pkt >> y0;
	pkt >> fi;
	pkt >> te;
	pkt >> iplace;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->fire(z0, x0, y0, fi, te, iplace, req_time)) {
			error("NID can't fire gun");
		}
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}


void Net::send_punch(int NID, int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_PUNCH);
	pkt << NID;
	pkt << z0;
	pkt << x0;
	pkt << y0;
	pkt << fi;
	pkt << te;
	pkt << iplace;
	pkt << req_time;
	send();
}

int Net::recv_punch()
{
	int NID, z0, x0, y0, iplace, req_time;
	REAL fi, te;

	pkt >> NID;
	pkt >> z0;
	pkt >> x0;
	pkt >> y0;
	pkt >> fi;
	pkt >> te;
	pkt >> iplace;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->punch(z0, x0, y0, fi, te, iplace, req_time)) {
			error("NID can't punch");
		}
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}


void Net::send_aimedthrow(int NID, int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!SEND) return ;

	pkt.create(CMD_AIMEDTHROW);
	pkt << NID;
	pkt << z0;
	pkt << x0;
	pkt << y0;
	pkt << fi;
	pkt << te;
	pkt << iplace;
	pkt << req_time;
	send();
}

int Net::recv_aimedthrow()
{
	int NID, z0, x0, y0, iplace, req_time;
	REAL fi, te;

	pkt >> NID;
	pkt >> z0;
	pkt >> x0;
	pkt >> y0;
	pkt >> fi;
	pkt >> te;
	pkt >> iplace;
	pkt >> req_time;

	Soldier *ss = findman(NID);
	if (ss != NULL) {
		SEND = 0;
		if (!ss->aimedthrow(z0, x0, y0, fi, te, iplace, req_time)) {
			error("NID can't aimedthrow");
		}
		SEND = 1;
		return 1;
	} else
		error("NID");
	return 0;
}

extern Units local;
extern Units remote;

void Net::send_add_unit(int num, char *name, int cost)
{
	if (!SEND) return ;

	pkt.create(CMD_ADD_UNIT);
	pkt << num;
	pkt << name;
	pkt << cost;
	send();

	local.SEND = 0;
	local.START = 0;
	remote.START = 0;
}

int Net::recv_add_unit()
{
	int num;
	char name[26];
	int cost;

	pkt >> num;
	pkt >> name;
	pkt >> cost;

	if (!remote.add(num, name, cost)) {
		error("can't add to remote units");
		return 0;
	}

	remote.SEND = 0;
	local.START = 0;
	remote.START = 0;
	return 1;
}

void Net::send_select_unit(int num, int mx, int my)
{
	if (!SEND) return ;

	pkt.create(CMD_SELECT_UNIT);
	pkt << num;
	pkt << mx;
	pkt << my;
	send();

	local.SEND = 0;
	local.START = 0;
	remote.START = 0;
}

int Net::recv_select_unit()
{
	int num, mx, my;

	pkt >> num;
	pkt >> mx;
	pkt >> my;

	if (!remote.select_unit(num, mx, my)) {
		error("can't select remote unit");
		return 0;
	}

	remote.SEND = 0;
	local.START = 0;
	remote.START = 0;
	return 1;
}

void Net::send_deselect_unit(int num)
{
	if (!SEND) return ;

	pkt.create(CMD_DESELECT_UNIT);
	pkt << num;
	send();

	local.SEND = 0;
	local.START = 0;
	remote.START = 0;
}

int Net::recv_deselect_unit()
{
	int num;

	pkt >> num;

	if (!remote.deselect_unit(num)) {
		error("can't deselect remote unit");
		return 0;
	}

	remote.SEND = 0;
	local.START = 0;
	remote.START = 0;
	return 1;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
extern int FINISH_PLANNER;

void Net::send_finish_planner()
{
	if (!SEND) return ;

	if ((!local.SEND) || (!remote.SEND))
		return ;

	local.START = 1;

	pkt.create(CMD_FINISH_PLANNER);
	if (!remote.START) {
		pkt << FINISH_PLANNER;
	} else {
		FINISH_PLANNER = 1;
		pkt << FINISH_PLANNER;
	}
	send();
}

int Net::recv_finish_planner()
{
	remote.START = 1;
	int CNF;
	pkt >> CNF;

	if (CNF) {
		FINISH_PLANNER = 1;
	}
	return 1;
}

void Net::send_unit_data_size(int size)
{
	if (!SEND) return ;

	pkt.create(CMD_UNIT_DATA_SIZE);
	pkt << size;
	send();

	if (size)
		local.SEND = 1;
}

extern PLAYERDATA *pd_remote;

int Net::recv_unit_data_size()
{
	int size;

	pkt >> size;

	if (size == 0) { //begin send
		memset(pd_remote, 0, sizeof(PLAYERDATA));
		g_console->printf("%s", "pd recv begin");
	} else { //end send
		//info->printstr("pd recv end\n");
		pd_remote->size = size;
		g_console->printf("pd recv end. size %d", pd_remote->size);
		remote.SEND = 1;
	}
	return 1;
}

void Net::send_unit_data(int num, int lev, int col, int row, MANDATA *md, ITEMDATA *id)
{
	if (!SEND) return ;

	pkt.create(CMD_UNIT_DATA);
	pkt << num;
	pkt << lev;
	pkt << col;
	pkt << row;
	pkt.push((char *)md, sizeof(MANDATA));
	pkt.push((char *)id, sizeof(ITEMDATA));
	//send();
	send(pkt.str(), pkt.str_len());
}

int Net::recv_unit_data()
{
	int num, lev, col, row;
	MANDATA md;
	ITEMDATA id;

	pkt >> num;
	pkt >> lev;
	pkt >> col;
	pkt >> row;
	pkt.pop((char *) & md, sizeof(MANDATA));
	pkt.pop((char *) & id, sizeof(ITEMDATA));

	if (num > 19) {
		error("PD num > 19");
		return 0;
	}

	pd_remote->lev[num] = lev;
	pd_remote->col[num] = col;
	pd_remote->row[num] = row;
	memcpy(&pd_remote->md[num], &md, sizeof(MANDATA));
	memcpy(&pd_remote->id[num], &id, sizeof(ITEMDATA));

	return 1;
}

void Net::send_map_data(GEODATA *gd)
{
	if (!SEND) return ;

	pkt.create(CMD_MAP_DATA);
	pkt.push((char *)gd, sizeof(GEODATA));
	send(pkt.str(), pkt.str_len());
}

int Net::recv_map_data()
{
	pkt.pop((char *) & mapdata, sizeof(GEODATA));
	mapdata.load_game = 77;
	return 1;
}

void Net::send_time_limit(int time_limit)
{
	if (!SEND) return ;

	pkt.create(CMD_TIME_LIMIT);
	pkt << time_limit;

	send(pkt.str(), pkt.str_len());
}

int Net::recv_time_limit()
{
	int time_limit;
	pkt >> time_limit;
	g_time_limit = time_limit;
	return 1;
}

void Net::send_terrain_crc32(int index, unsigned long crc32)
{
	if (!SEND) return ;

	int crc32_lo = (int)(crc32 & 0xFFFF);
	int crc32_hi = (int)(crc32 >> 16);

	pkt.create(CMD_TERRAIN_CRC32);
	pkt << index;
	pkt << crc32_lo;
	pkt << crc32_hi;

	send(pkt.str(), pkt.str_len());
}

int Net::recv_terrain_crc32()
{
	int index;
	int crc32_lo;
	int crc32_hi;

	pkt >> index;
	pkt >> crc32_lo;
	pkt >> crc32_hi;

	unsigned long crc32 = ((unsigned long)crc32_hi << 16) | (unsigned long)crc32_lo;

	std::string name = terrain_set->get_terrain_name(index);

	if (!name.empty() && crc32 != terrain_set->get_terrain_crc32(index)) {
		g_console->printf("remote player has different maps in '%s' tileset\n", name.c_str());
	}
	
	return 1;
}

void Net::send_error_report(char *report)
{
	if (gametype == GAME_TYPE_INTERNET_SERVER)
		m_internet_server->send_packet(SRV_DEBUG_MESSAGE, report);
}
