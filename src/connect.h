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
#ifndef CONNECT_H
#define CONNECT_H
#include "packet.h"

class Connect
{
private:
	static DIALOG hostaddr_dialog[];
	Packet pkt;

	void new_mapdata();
	void send_mapdata();
	void recv_mapdata(char *crcpkt);

	void new_mendata();
	void send_mendata();
	void recv_mendata(char *str);

public:
	int do_chat();
	void reset_uds();
	void swap_uds();
	int do_planner(int F10ALLOWED, int map_change_allowed = 1);

	void draw_pd_info(void *_pd, int gx, int gy);
};

#endif
