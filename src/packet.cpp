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
#include "packet.h"

char *Packet::strCommand[COMMAND_NUM] = {
	"NOTE", "QUIT", "TURN", "DOOR", "POSE", "PRIM", "DETO", "EXPL", "UNLO", "LOAD",
	"TAKE", "DROP", "MOVE", "FACE", "THRU", "BEAM", "FIRE", "ADUN", "PDSZ", "PDAT",
	"SEUN", "DEUN", "MDAT", "STRT", "REST", "PNCH", "AMTR", "TLIM"
};

Packet::Packet()
{
	reset();
}

void Packet::reset()
{
	cur = 0; size = 0;
	memset(data, 0, sizeof(data));
}

void Packet::str2pkt(char *str)
{
	reset();
	strcpy(data, str);
	size = strlen(data);
}

void Packet::create(char *header)
{
	reset();
	assert(strchr(header, '_') == NULL);
	int len = sprintf(data + size, "%s_", header);
	size += len;
}

void Packet::create(Command cmd)
{
	reset();
	int len = sprintf(data + size, "_Xcom_%s_", strCommand[(int)cmd]);
	size += len;
}

Command Packet::command(char *str)
{
	cur = 0;
	if (strstr(str, "_Xmes_") != NULL) {
		strcpy(data, strstr(str, "_Xmes_") + 6);
		return CMD_MESSAGE;
	}

	if (strstr(str, "_Xcom_") != NULL)
		for (int i = 0; i < COMMAND_NUM; i++) {
			char *pkt = strstr(str, strCommand[i]);

			if (pkt != NULL) {
				strcpy(data, pkt + strlen(strCommand[i]) + 1);
				return (Command)i;
			}
		}
	return CMD_NONE;
}

//01234567890123
//23_Xcom_TURN_
//		  8	2
//^		 ^	^
//pkt - buf
Command Packet::command(char *buf, int buf_size)
{
	memset(data, 0, sizeof(data));
	cur = 0;
	if (strstr(buf, "_Xmes_") != NULL) {
		strcpy(data, strstr(buf, "_Xmes_") + 6);
		return CMD_MESSAGE;
	}

	if (strstr(buf, "_Xcom_") != NULL)
		for (int i = 0; i < COMMAND_NUM; i++) {
			char *xcom = strstr(buf, "_Xcom_");
			char *pkt = NULL;
			if (!memcmp(xcom + strlen("_Xcom_"), strCommand[i], strlen(strCommand[i])))
				pkt = strstr(buf, strCommand[i]);

			if (pkt != NULL) {
				memcpy(data, pkt + strlen(strCommand[i]) + 1, buf_size);
				size = buf_size;
				return (Command)i;
			}
		}
	return CMD_NONE;
}

void Packet::push(char *buf, int buf_size)
{
	for (int n = 0; n < buf_size; n++)
		data[size++] = buf[n];
	data[size++] = '_';
}

void Packet::pop(char *buf, int buf_size)
{
	for (int n = 0; n < buf_size; n++)
		buf[n] = data[cur++];
	cur++;
}

Packet &Packet::operator<<(char *i)
{
	assert(strchr(i, '_') == NULL);
	int n = 0;
	while (i[n])
		data[size++] = i[n++];
	data[size++] = '_';
	return *this;
}

Packet &Packet::operator>>(char *i)
{
	int n = 0;
	while (data[cur] != '_')
		i[n++] = data[cur++];
	i[n] = 0; cur++;
	return *this;
}

Packet &Packet::operator<<(int i)
{
	int len = sprintf(data + size, "%d_", i);
	size += len;
	return *this;
}

Packet &Packet::operator>>(int &i)
{
	sscanf(data + cur, "%d_", &i);

	int len = 0;
	while (data[cur + len] != '_')
		len++;

	cur += len + 1;
	return *this;
}

Packet &Packet::operator<<(REAL i)
{
	assert(sizeof(i) == sizeof(long));
	int len = sprintf(data + size, "%lx(%f)_", *(long *) & i, i);
	size += len;
	return *this;
}

Packet &Packet::operator>>(REAL &i)
{
	assert(sizeof(i) == sizeof(long));
	sscanf(data + cur, "%lx", (long *) & i);
	int len = 0;
	while (data[cur + len] != '_')
		len++;

	cur += len + 1;
	return *this;
}

BQ::BQ(int sz) {
}

BQ::~BQ() {
}

void BQ::put(const std::string &str)
{
	bq.push_back(str);
}

void BQ::put(char *buf, int buf_size)
{
	bq.push_back(std::string(buf, buf_size));
}

int BQ::get(std::string &str)
{
	if (bq.empty()) return 0;

	str = bq.front();
	bq.pop_front();
	return 1;
}

int BQ::get(char *buf, int &buf_size)
{
	if (bq.empty()) return 0;

	buf_size = bq.front().size();
	memcpy(buf, bq.front().data(), buf_size);
	buf[buf_size] = 0;
	bq.pop_front();
	return 1;
}
