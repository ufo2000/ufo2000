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
#ifndef PACKET_H
#define PACKET_H

#include <list>
#include <string>

#define COMMAND_NUM 28

enum Command {CMD_MESSAGE = -2, CMD_NONE = -1,
              CMD_NOTICE = 0, CMD_QUITGAME, CMD_ENDTURN,              //3
              CMD_OPENDOOR, CMD_CHANGEPOSE, CMD_PRIMEGRENADE,         //6
              CMD_DETONATE_ITEM, CMD_EXPLODE,
              CMD_UNLOAD_AMMO, CMD_LOAD_AMMO,
              CMD_TAKE_ITEM, CMD_DROP_ITEM,
              CMD_MOVE, CMD_FACE,                                     //14
              CMD_THROW_ITEM, CMD_BEAM_LASER, CMD_FIRE_GUN,           //17
              CMD_ADD_UNIT, CMD_UNIT_DATA_SIZE, CMD_UNIT_DATA,        //20
              CMD_SELECT_UNIT, CMD_DESELECT_UNIT,                     //22
              CMD_MAP_DATA,                                           //23
              CMD_FINISH_PLANNER,                                     //24
              CMD_RESTARTGAME,                                        //25
              CMD_PUNCH, CMD_AIMEDTHROW,                              //27
              CMD_TIME_LIMIT                                          //28
             };


class Packet
{
private:
	char data[1000];
	int cur, size;
	static char *strCommand[COMMAND_NUM];
public:
	Packet();
	void reset();
	void str2pkt(char *str);
	void create(char *header);
	void create(Command cmd);
	Command command(char *str);
	Command command(char *buf, int buf_size);

	inline void set_cur(int ofs) { cur = ofs; }
	inline char *str() { return data; }
	inline int str_len() { return size; }
	void push(char *buf, int buf_size);
	void pop(char *buf, int buf_size);

	Packet &operator<<(int i);
	Packet &operator>>(int &i);

	Packet &operator<<(REAL i);
	Packet &operator>>(REAL &i);

	Packet &operator<<(char *i);
	Packet &operator>>(char *i);
};

class BQ
{
private:
	std::list<std::string> bq;

public:
	BQ(int sz);
	~BQ();

	void put(char *str);
	void put(char *buf, int buf_size);
	int get(char *str);
	int get(char *buf, int &buf_size);
};

#endif
