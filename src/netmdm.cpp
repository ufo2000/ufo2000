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
#ifdef DJGPP

#include <stdio.h>
#include <string.h>
#include <com.h>
#include "wind.h"
#include "netmdm.h"

/*
extern Wind *info_win;
*/

//extern int SEND;
int MODEMPORT = COM2;

int packet_len;

int initmdmgame()
{
	COMInit();
	COMSetDtr(MODEMPORT, 1);
	COMSetRts(MODEMPORT, 1);
	COMSetTXQueueSize(MODEMPORT, 0x4000);
	COMSetRXQueueSize(MODEMPORT, 0x4000);
	int nError = COMPortOpen(COM2, 115200, 8, 'N', 1, 0, NULL);
	if (nError != 0) {
		char str[100];
		sprintf(str, "can't open COM%d\n", MODEMPORT + 1);
		info_win->printstr(str);
		//getch();
		//SEND = 0;
		return 0;
	} else {
		char str[100];
		sprintf(str, "COM%d opened\n", MODEMPORT + 1);
		info_win->printstr(str);
	}
	packet_len = 0;
	return 1;
}

void closemdmgame()
{
	//COMSetDtr(MODEMPORT, 0);
	//COMSetRts(MODEMPORT, 0);
	COMShutDown();
}

int packet_recv_mdm(char *pkt)
{
	char cc;
	//int packet_len = strlen(pkt);
	while (COMReadChar(MODEMPORT, &cc, NULL) == 0) { //if is slow
		//text_mode(0); textprintf(screen, font, 0, 0, 1, "%c-%d", cc, cc);
		//if (cc != '\r') {
		pkt[packet_len++] = cc;
		pkt[packet_len] = 0;
		//}
		//if ((cc == 13)||(cc == 10)||(packet_len>78)) {
		if ((cc == '\n') || (packet_len > 78)) {
			packet_len = 0;
			return 1;
		}
	}
	return 0;
}

int packet_recv_mdm_char(char &cc)
{
	if (COMReadChar(MODEMPORT, &cc, NULL) == 0)
		return 1;
	return 0;
}

void packet_send_mdm(char *pkt)
{
	//int l = strlen(pkt);
	//pkt[l] = 13;
	//pkt[l+1] = 0;
	//COMWriteBuffer(MODEMPORT, pkt, NULL, l+1, NULL);
	COMWriteBuffer(MODEMPORT, pkt, NULL, strlen(pkt), NULL);
}

#else

int MODEMPORT = 0;

int initmdmgame()
{
	return 0;
}
void closemdmgame()
{}
int packet_recv_mdm(char *pkt)
{
	return 0;
}
int packet_recv_mdm_char(char &cc)
{
	return 0;
}
void packet_send_mdm(char *pkt)
{}

#endif
