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

#define COMMAND_NUM 32

enum Command {CMD_MESSAGE = -2, CMD_NONE = -1,
              CMD_NOTICE = 0, CMD_QUITGAME, CMD_ENDTURN,              
              CMD_OPENDOOR, CMD_CHANGEPOSE, CMD_PRIMEGRENADE,         
              CMD_UNLOAD_AMMO, CMD_LOAD_AMMO,
              CMD_TAKE_ITEM, CMD_DROP_ITEM,
              CMD_MOVE, CMD_FACE,                                     
              CMD_TARGET_ACTION,
              CMD_ADD_UNIT, CMD_UNIT_DATA_SIZE, CMD_UNIT_DATA,        
              CMD_SELECT_UNIT, CMD_DESELECT_UNIT,                     
              CMD_MAP_DATA,                                           
              CMD_FINISH_PLANNER,                                     
              CMD_RESTARTGAME,                                        
              CMD_TIME_LIMIT,                                         
              CMD_TERRAIN_CRC32,                                      
              CMD_USE_ELEVATOR,
              CMD_SCENARIO, CMD_RULES, CMD_OPTIONS,
              CMD_PANIC, CMD_MORALE, CMD_RESTIME, CMD_P2_ISSIT,
              CMD_EQUIPMENT
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
	void create(char *header);
	void create(Command cmd);
	Command command(char *buf, int buf_size);

	inline char *str() { return data; }
	inline int str_len() { return size; }
	void push(char *buf, int buf_size);
	void pop(char *buf, int buf_size);

	Packet &operator<<(int i);
	Packet &operator>>(int &i);

	Packet &operator<<(uint32 i);
	Packet &operator>>(uint32 &i);

	Packet &operator<<(REAL i);
	Packet &operator>>(REAL &i);

	Packet &operator<<(char *i);
	Packet &operator>>(char *i);

	Packet &operator<<(const std::string &i);
	Packet &operator>>(std::string &i);
};

class BQ
{
private:
	std::list<std::string> bq;

public:
	BQ(int sz);
	~BQ();

	void put(const std::string &str);
	void put(char *buf, int buf_size);
	int get(std::string &str);
	int get(char *buf, int &buf_size);
};

#endif
