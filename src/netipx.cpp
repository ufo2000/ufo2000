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

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <dosbuff.h>
#include <jonipx.h>
#include "wind.h"
#include "netipx.h"

/*
extern Wind *info_win;
//extern int SEND;
extern int HOST;
 
extern local_address_type local_address;
*/

static node_address_type remote_address = {0, 0, 0, 0, 0, 0};
static node_address_type maxbroad_address = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static char *areyouhost = "AreYouUFO2000Host?";
static char *iamhost = "IAmUFO2000Host!";
//char *startgame = "StartUFO2000Game!";
static game_packet_type packet;

int initipxgame()
{
	Allocate_Dos_Buffers();
	if (!Init_Jonipx()) {
		info_win->printstr("can't init IPX\n");
		//SEND = 0;
		return 0;
	}
	info_win->printstr("IPX initialized\n");

	int oldsec = 0;
	time_t ltime;
	struct tm *time_of_day;
	char pkt[80];

	if (HOST) {
		info_win->printstr("waiting");
		while (1) {
			if (key[KEY_ESC]) {
				info_win->printstr("\ncanceled\n");
				//SEND = 0;
				return 0;
			}

			if (packet_recv_ipx(pkt)) {
				char *req = strstr(pkt, areyouhost);
				if (req != NULL) {
					if (memcmp(remote_address, packet.source_node, 6) != 0) {
						memcpy(remote_address, packet.source_node, 6);
						info_win->printstr("found client\n");
						packet_send_ipx(iamhost);
						packet_send_ipx(iamhost);
						return 1;
					}
				}
			}
			time(&ltime);
			time_of_day = localtime(&ltime);

			if (time_of_day->tm_sec == oldsec)
				continue;
			oldsec = time_of_day->tm_sec;
			info_win->printchr('.');
		}
	} else {
		info_win->printstr("searching");
		while (1) {
			if (key[KEY_ESC]) {
				info_win->printstr("\ncanceled\n");
				//SEND = 0;
				return 0;
			}
			if (packet_recv_ipx(pkt)) {
				char *req = strstr(pkt, iamhost);
				if (req != NULL) {
					if (memcmp(remote_address, packet.source_node, 6) != 0) {
						memcpy(remote_address, packet.source_node, 6);
						info_win->printstr("found host\n");
						return 1;
					}
				}
			}
			time(&ltime);
			time_of_day = localtime(&ltime);

			if (time_of_day->tm_sec == oldsec)
				continue;
			oldsec = time_of_day->tm_sec;

			packet_send_ipx_to(maxbroad_address, areyouhost);
			info_win->printchr('.');
		}
	}
	return 0;
}

void closeipxgame()
{
	memset(remote_address, 0, 6);
	Close_Jonipx();
}

int packet_recv_ipx(char *pkt)
{
	if (Packet_Ready()) {
		Pop_Packet(&packet);      //no size of pack
		int plen = packet.string[0] - '0';
		packet.string[plen + 1] = 0;
		memcpy(pkt, packet.string + 1, plen);
		pkt[plen] = 0;
		//strcpy(pkt, packet.string+1);
		return 1;
	}
	return 0;
}

void packet_send_ipx_to(unsigned char *adr, char *pkt)
{
	char ll[100];
	ll[0] = '0' + strlen(pkt); ll[1] = 0;
	strcat(ll, pkt);
	Jon_Send_Packet(ll, strlen(ll), adr);
}

void packet_send_ipx(char *pkt)
{
	char ll[100];
	ll[0] = '0' + strlen(pkt); ll[1] = 0;
	strcat(ll, pkt);
	Jon_Send_Packet(ll, strlen(ll), remote_address);
	//Jon_Send_Packet( str, strlen(pkt), remote_address);

	//packet_send_ipx_to(remote_address, pkt);  //the same
}

#else

int initipxgame()
{
	return 0;
}
void closeipxgame()
{}
int packet_recv_ipx(char *pkt)
{
	return 0;
}
void packet_send_ipx(char *pkt)
{}
void packet_send_ipx_to(unsigned char *adr, char *pkt)
{}

#endif
