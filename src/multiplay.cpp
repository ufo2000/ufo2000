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
#include "video.h"
#include "wind.h"
#include "explo.h"
#include "soldier.h"
#include "platoon.h"
#include "multiplay.h"
#include "map.h"
#include "sound.h"
#include "netsock.h"
#include "netipx.h"
#include "netmdm.h"
#include "netdplay.h"
#include "packet.h"
#include "units.h"
#include "pfxopen.h"

void inithotseatgame()
{
}

void closehotseatgame()
{
}

void next_turn(int crc);

void Net::error(char *str)
{
	info->printstr("___error in ");
	info->printstr(str);
	info->printstr("___\n");
	log(str);
}

Net::Net()
{
	SEND = 1;
	queue = new BQ(1000);
	connect = new Connect();
	flog = FOPEN_RTEMP("ufo2000.log", "at");
	log("_____________Net()");
}

Net::~Net()
{
	log("_____________~Net()");
	fclose(flog);
	delete connect;
	delete queue;
}

void Net::log(char *str)
{
	time_t now = time (NULL);
	struct tm * t = localtime (&now);
	char buf[1000];
	strftime(buf, 1000, "%d/%m/%Y %H:%M:%S", t);
	fprintf(flog, "%s %s\n", buf, str);
	fflush(flog);
}

void Net::log(char *str, char *str2)
{
	time_t now = time (NULL);
	struct tm *t = localtime (&now);
	char buf[1000];
	strftime(buf, 1000, "%H:%M:%S", t);
	fprintf(flog, "%s %s %s]", buf, str, str2);
	fflush(flog);
}

int Net::init()
{
	log("init()");
	SEND = 1;

	connect->reset_uds();
	if (gametype == HOTSEAT) {
		HOST = 1;
		if (!connect->do_planner(1))
			return 0;
		clear(screen);
		reset_video();
		alert(" ", "  NEXT PLAYER  ", " ", "    OK    ", NULL, 1, 0);
		HOST = 0;
		connect->swap_uds();
		if (!connect->do_planner(1, 0))
			return 0;
		HOST = 1;
		clear(screen);
		reset_video();
		alert(" ", "  GAME START  ", " ", "    OK    ", NULL, 1, 0);
	} else {
		if (!connect->do_chat() || !connect->do_planner(0)) {
			close();
			return 0;
		}
		//connect->do_planner();
	}
	return 1;
}

void Net::close()
{
	log("close()");
	switch (gametype) {
		case MODEM:
			closemdmgame();
			break;
		case IPX:
			closeipxgame();
			break;
		case SOCK:
			closesocketgame();
			break;
		case HOTSEAT:
			closehotseatgame();
			break;
		case DPLAY:
			closedplaygame();
			break;
	}
}

void Net::send_message(char *mess)
{
	char str[1000];
	strcpy(str, "_Xmes_");
	strcat(str, mess);
	send(str);
}

void Net::send()
{
	assert(pkt.str_len() > 0);
	send(pkt.str());
	//send(pkt.str(), pkt.str_len());
}

void Net::send(char *dat, int size)
{
	if (FLAGS & F_RAWMESSAGES) {
		char frm[100];
		sprintf(frm, "send:[PLAYERDATA size %d byte\n", size);
		info->printstr(frm);
		//info->printstr("send:[");
		//info->printstr(dat); //info->printstr(", ");
	}

	switch (gametype) {
		case MODEM:
			break;
		case IPX:
			break;
		case SOCK:
			packet_send_socket(dat, size);
			break;
		case HOTSEAT:
			break;
		case DPLAY:
			packet_send_dplay(dat, size);
	}
}

void Net::send(char *_str)
{
	char str[1000];
	strcpy(str, _str);

	int l = strlen(str);
	str[l] = '\n';
	str[l + 1] = 0;

	if (FLAGS & F_RAWMESSAGES) {
		info->printstr("send:[");
		info->printstr(str);      //info->printstr(", ");
	}

	log("send:[", str);
	switch (gametype) {
		case MODEM:
			str[l] = '\r';
			str[l + 1] = '\n';
			str[l + 2] = 0;
			packet_send_mdm(str);
			break;
		case IPX:
			packet_send_ipx(str);
			break;
		case SOCK:
			packet_send_socket(str);
			break;
		case HOTSEAT:
			break;
		case DPLAY:
			packet_send_dplay(str);
	}
}


void Net::send_raw(char *str)
{
	switch (gametype) {
		case MODEM:
			packet_send_mdm(str);
			break;
		case IPX:
			packet_send_ipx(str);
			break;
		case SOCK:
			packet_send_socket(str);
			break;
		case HOTSEAT:
			break;
		case DPLAY:
			packet_send_dplay(str);
			break;
	}
}

int Net::recv_raw(char *pkt)
{
	int pr = 0;

	switch (gametype) {
		case MODEM:
			pr = packet_recv_mdm_char(pkt[0]);
			pkt[1] = 0;
			break;
		case IPX:
			pr = packet_recv_ipx(pkt);
			break;
		case SOCK:
			pr = packet_recv_socket(pkt);
			break;
		case HOTSEAT:
			break;
		case DPLAY:
			pr = packet_recv_dplay(pkt);      //!!!ret size of pkt, not 1
			break;
	}
	return pr;
}

extern int GAMELOOP;

void Net::check()
{
	//if (!platoon_remote->nomoves())
	//	return;
	char recv_str[1000];      //memset(recv_str, 0, sizeof(recv_str));
	int recv_str_len = 0;

	int pr = 0;
	switch (gametype) {
		case MODEM:
			pr = packet_recv_mdm(recv_str);
			break;
			recv_str_len = pr;
		case IPX:
			pr = packet_recv_ipx(recv_str);
			recv_str_len = pr;
			break;
		case SOCK:
			pr = packet_recv_socket(recv_str);
			recv_str_len = pr;
			break;
		case HOTSEAT:
			pr = 0;
			break;
		case DPLAY:
			pr = packet_recv_dplay(recv_str);      //!!!ret size of pkt, not 1
			recv_str_len = pr;
			break;
	}
	assert((recv_str_len >= 0) && (recv_str_len < 1000));
	//if (!recv_str_len)
	//	recv_str_len = strlen(recv_str);
	if (pr) {
		//queue->put(recv_str);
		queue->put(recv_str, recv_str_len);
		if (FLAGS & F_RAWMESSAGES) {
			char s[100];
			sprintf(s, "put:[%d]", recv_str_len);
			info->printstr(s);
			//info->printstr("put:[");
			info->printstr(recv_str);
		}
	}
	if (GAMELOOP && (!platoon_remote->nomoves()))
		return ;

	pr = 0;
	//if (queue->get(recv_str))
	if (queue->get(recv_str, recv_str_len))
		pr = 1;

	if (!pr)
		return ;

	if (FLAGS & F_RAWMESSAGES) {
		char s[100];
		sprintf(s, "get:[%d]", recv_str_len);
		info->printstr(s);
		//info->printstr("get:[");
		info->printstr(recv_str);      //info->printstr("], ");
	}

	log("recv:[", recv_str);
	//switch (pkt.command(recv_str)) {
	switch (pkt.command(recv_str, recv_str_len)) {
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
		case CMD_FINISH_PLANNER:
			recv_finish_planner();
			break;
		case CMD_MESSAGE:
			play(S_MESSAGE);
			info->printstr(pkt.str(), 32);
			break;
		case CMD_NONE:
			//play(S_MESSAGE);
			//if (FLAGS & F_RAWMESSAGES == 0) {
			info->printstr(recv_str, 144);      //info->printstr("], ");
			//}
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
	int crc;

	pkt >> crc;

	SEND = 0;
	next_turn(crc);
	SEND = 1;
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

	Soldier * ss = platoon_remote->findman(NID);
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

	Soldier * ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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


void Net::send_detonate_item(int lev, int col, int row, int iplace, int ix, int iy)
{
	if (!SEND) return ;

	pkt.create(CMD_DETONATE_ITEM);
	pkt << lev;
	pkt << col;
	pkt << row;
	pkt << iplace;
	pkt << ix;
	pkt << iy;
	send();
}


int Net::recv_detonate_item()
{ // "DETO"
	int lev, col, row, iplace, ix, iy;

	pkt >> lev;
	pkt >> col;
	pkt >> row;
	pkt >> iplace;
	pkt >> ix;
	pkt >> iy;

	SEND = 0;
	if (!elist->detonate(lev, col, row, iplace, ix, iy)) {
		error("EXPLO can't detonate item");
	}
	SEND = 1;
	return 1;
}



void Net::send_explode(int lev, int col, int row, int type, int range, int damage)
{
	if (!SEND) return ;

	pkt.create(CMD_EXPLODE);
	pkt << lev;
	pkt << col;
	pkt << row;
	pkt << type;
	pkt << range;
	pkt << damage;
	send();
}

int Net::recv_explode()
{ // "EXPL"
	int lev, col, row, type, range, damage;

	pkt >> lev;
	pkt >> col;
	pkt >> row;
	pkt >> type;
	pkt >> range;
	pkt >> damage;

	SEND = 0;
	if (!map->explode(lev, col, row, type, range, damage)) {
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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

	Soldier *ss = platoon_remote->findman(NID);
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
	//if  (CONFIRM_FINISH_PLANNER != 2)
	//if (local.START && remote.START) {
	//	send_confirm_finish_planner();
	// CONFIRM_FINISH_PLANNER = 2;
	//}
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
		info->printstr("pd recv begin\n");
	} else { //end send
		//info->printstr("pd recv end\n");
		pd_remote->size = size;
		char s[100];
		sprintf(s, "pd recv end. size %d\n", pd_remote->size);
		remote.SEND = 1;
		info->printstr(s);
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
	//send();
	send(pkt.str(), pkt.str_len());
	//info->printstr("\nsend_map_data\n");
}

int Net::recv_map_data()
{
	pkt.pop((char *) & mapdata, sizeof(GEODATA));
	mapdata.load_game = 77;
	//info->printstr("\nrecv_map_data\n");
	return 1;
}

char *replay_data = NULL;
int replay_size = 0;
int replay_cur = 0;
int REPLAY = 0;

void Net::replay_load(char *fn)
{
	REPLAY = 0;
	replay_cur = 0;

	int fh = OPEN_GTEMP(fn, O_RDONLY | O_BINARY);
	assert(fh != -1);
	if (fh == -1)
		return ;
	replay_size = filelength(fh);
	if (replay_size <= 0)
		return ;

	if (replay_data != NULL)
		delete replay_data;
	replay_data = new char[replay_size];
	replay_size = read(fh, replay_data, replay_size);
	if (replay_size <= 0)
		return ;
	::close(fh);

	REPLAY = 1;
}

extern Platoon *p1;
extern Platoon *p2;

void Net::replay()
{
	if (!REPLAY)
		return ;

	char str[10000]; memset(str, 0, sizeof(str));
	int i = 0;

	while (replay_data[replay_cur] != 0xA) {
		str[i++] = replay_data[replay_cur++];
		if (replay_cur >= replay_size) {
			REPLAY = 0;
			break;
		}
	}
	replay_cur++;

	info->printstr(str);
	info->printstr("\n");

	char *xcom = strstr(str, "send:[ ");
	if (xcom != NULL) {
		platoon_local = p2;
		platoon_remote = p1;
	} else {
		xcom = strstr(str, "recv:[ ");
		if (xcom != NULL) {
			platoon_local = p1;
			platoon_remote = p2;
		} else
			return ;
	}


	switch (pkt.command(xcom)) {
		case CMD_NOTICE:
			recv_notice();
			break;
		case CMD_QUITGAME:
			recv_quit();
			break;
		case CMD_RESTARTGAME:
			//recv_restart();
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
			//recv_add_unit();
			break;
		case CMD_SELECT_UNIT:
			//recv_select_unit();
			break;
		case CMD_DESELECT_UNIT:
			//recv_deselect_unit();
			break;
		case CMD_UNIT_DATA_SIZE:
			//recv_unit_data_size();
			break;
		case CMD_UNIT_DATA:
			//recv_unit_data();
			break;
		case CMD_MAP_DATA:
			//recv_map_data();
			break;
		case CMD_FINISH_PLANNER:
			//recv_finish_planner();
			break;
		case CMD_MESSAGE:
			play(S_MESSAGE);
			info->printstr(pkt.str(), 32);
			break;
		case CMD_NONE:
			break;
	}
}
