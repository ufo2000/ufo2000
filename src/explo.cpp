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
#include <sys/stat.h>
#include <fcntl.h>
#include "pck.h"
#include "video.h"
#include "explo.h"
#include "platoon.h"
#include "map.h"
#include "multiplay.h"
#include "wind.h"

IMPLEMENT_PERSISTENCE(Explosive, "Explosive");

Explosive::Explosive()
{
	reset();
}

void Explosive::reset()
{
	for (int i = 0; i < EXPLOITEMS; i++) {
		item[i] = NULL;
		delaytime[i] = 0;
	}
}

void Explosive::add(Item *it, int delay_time)
{
	assert(delay_time > 0);

	int i;
	for (i = 0; i < EXPLOITEMS; i++)
		if (item[i] == it) {	  //check for presence
			delaytime[i] = delay_time;      //re-prime
			it->set_delay_time(delay_time);
			return ;
		}
	for (i = 0; i < EXPLOITEMS; i++)
		if (item[i] == NULL) {
			item[i] = it;
			delaytime[i] = delay_time;
			item[i]->set_delay_time(delay_time);

			//char str[100];
			//sprintf(str, "explo add %d, delay_time=%d\n", i, delay_time);
			//info->printstr(str);

			return ;
		}
}

void Explosive::remove(Item *it)
{
	for (int i = 0; i < EXPLOITEMS; i++)
		if (item[i] == it) {
			item[i] = NULL;
			return ;
		}
}

int Explosive::on_hand(Item *it)
{
	int lev, col, row;
	Place *ip;
	int iplace;

	ip = map->find_item(it, lev, col, row);
	if (ip == NULL)
		ip = platoon_local->find_item(it, lev, col, row);
	if (ip == NULL)
		ip = platoon_remote->find_item(it, lev, col, row);
	assert(ip != NULL);

	iplace = map->find_place_num(ip, lev, col, row);
	assert(iplace != -1);

	return ((iplace == P_ARM_RIGHT) || (iplace == P_ARM_LEFT));
}

void Explosive::step(int crc) { // -1 if local
	for(int i=0;i<EXPLOITEMS;i++)
		if (item[i] != NULL) {
			if (!item[i]->is_explo()) {// small grenade
				if (on_hand(item[i]))
					continue;
			}

			delaytime[i]--;
			item[i]->set_delay_time(delaytime[i]);

			if ((delaytime[i] <= 0)&&(item[i]->type!=PROXIMITY_GRENADE)) {
				if (crc == -1)
					detonate(item[i]);
				else
					item[i] = NULL;
			}
		}
}


void Explosive::check_for_detonation(Item *it) {
	for(int i=0;i<EXPLOITEMS;i++)
		if (item[i] == it) {
			if ((delaytime[i] == 1)&&(item[i]->type!=PROXIMITY_GRENADE))
				detonate(it);
			return;
		}
}

int Explosive::detonate(Item *it)
{
	int lev, col, row, type, range, damage;
	Place *ip;
	int iplace;

	ip = map->find_item(it, lev, col, row);
	if (ip == NULL)
		ip = platoon_local->find_item(it, lev, col, row);
	if (ip == NULL)
		ip = platoon_remote->find_item(it, lev, col, row);
	assert(ip != NULL);

	iplace = map->find_place_num(ip, lev, col, row);
	assert(iplace != -1);

	type = it->type;
	damage = it->data()->damage;
	range = it->explo_range();

	net->send_detonate_item(lev, col, row, iplace, it->x, it->y);

	remove(it);
	int v = ip->destroy(it);
	assert(v);

	if (net->SEND) {
		net->SEND = 0;
		map->explode(lev, col, row, type, range, damage);
		net->SEND = 1;
	} else
		map->explode(lev, col, row, type, range, damage);

	return 1;
}

int Explosive::detonate(int lev, int col, int row, int iplace, int ix, int iy)
{
	Item * it;
	assert((iplace >= 0) && (iplace <= P_MAP));

	if (iplace == P_MAP)
		it = map->place(lev, col, row)->item(ix, iy);
	else
		it = map->man(lev, col, row)->item(iplace, ix, iy);

	assert(it != NULL);
	return detonate(it);
}
/*
	void setdelay(int delay) {
		delaytime = delay;
	}
 
int Item::isprimed() {
	if (delaytime != 0)
		return 1;
	return 0;
}
 
	int isprimed();
 
*/

bool Explosive::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	for (int i = 0; i < EXPLOITEMS; i++)
		PersistWriteObject(archive, item[i]);

	return true;
}

bool Explosive::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	for (int i = 0; i < EXPLOITEMS; i++)
		PersistReadObject(archive, item[i]);

	return true;
}
