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
#include "item.h"
#include "video.h"
#include "map.h"
#include "multiplay.h"
#include "sound.h"
#include "pfxopen.h"

PCK *bigobs;
OBDATA *Item::obdata = NULL;
int Item::obdata_num = 0;

IMPLEMENT_PERSISTENCE(Item, "Item");

void Item::initobdata()
{
	int fh = open(F("$(xcom)/geodata/obdata.dat"), O_RDONLY | O_BINARY);
	assert(fh != -1);
	int buflen = filelength(fh);
	char *buf = new char[buflen];
	buflen = read(fh, buf, buflen);
	assert(buflen > 0);
	close(fh);
	//#define NEW_ITEMS_NUM       0

	obdata_num = buflen / sizeof(OBDATA) + NEW_ITEMS_NUM;
	obdata = new OBDATA[obdata_num];
	for (int i = 0; i < obdata_num - NEW_ITEMS_NUM; i++)
		memcpy(&obdata[i], buf + i * sizeof(OBDATA), sizeof(OBDATA));


	memset(&obdata[KASTET], 0, sizeof(OBDATA));
	strcpy(obdata[KASTET].name, "KASTET");     	//The name of the object; used internally only

	obdata[KASTET].pInv = 0;     		//The image on the inventory screen
	obdata[KASTET].pMap = 4;     //floorob//The image on the isometric map screen
	obdata[KASTET].damage = 20;     		//The damage value
	obdata[KASTET].importance = 0;     	//In a stack of items, the item with the highest importance is shown
	obdata[KASTET].width = 1;     		//The width on the inventory
	obdata[KASTET].height = 1;     		//The height on the inventory
	obdata[KASTET].ammo[0] = 0xFF;     	//The three item numbers that can be ammo, 0xFF=unused
	obdata[KASTET].ammo[1] = 0xFF;
	obdata[KASTET].ammo[2] = 0xFF;
	obdata[KASTET].pHeld = 15;     //handob	//The picture to use on the map while it's being held
	//obdata[KASTET]._1E;		//Changing this seems to have no effect...
	//obdata[KASTET].damageType;	//The type of attack; refer to DT_nnnn
	obdata[KASTET].accuracy[0] = 80;     	//The accuracy for each of the three kinds of shots; 0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	obdata[KASTET].time[0] = 0;     	//The TU% for each of the three kinds of shots.  0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	obdata[KASTET].time[1] = 0;
	obdata[KASTET].time[2] = 0;
	//obdata[KASTET].rounds;		//The number of rounds in the clip
	//obdata[KASTET].hitType;	//The type of damage that hitting with it does.  Only matters on stun rods/thermal tazers.  Refer to DT_nnnn
	obdata[KASTET].throw_accuracy = 0x64;     		//Always 0x64
	//obdata[KASTET]._29;		//Always 0x32
	//obdata[KASTET]._2A;
	obdata[KASTET].weight = 3;     		//The weight of the object
	//obdata[KASTET].primeType;	//The way in which the object can be primed.  See PT_nnnn
	//obdata[KASTET]._2D;
	//obdata[KASTET].isShootable;	//This item can be shot. (0=no,1=yes)
	obdata[KASTET].isWeapon = 1;     	//This item is a weapon. (0=no,1=yes)
	//obdata[KASTET].isGun;		//This is a gun which requires ammo. (0=no,1=yes)
	//obdata[KASTET].isAmmo;		//This item is a clip. (0=no,1=yes)
	//obdata[KASTET].twoHanded;	//Is more accurate with two hands (0=no,1=yes)
	//obdata[KASTET].wayPoints;	//Has a 'launch missile' option  (0=no,1=yes)
	//obdata[KASTET]._34;		//Scott T. Jones has this marked as 'Research'.  This makes sense, but I can find no evidence to support it...
	//obdata[KASTET]._35;		//Scott T. Jones has this marked as 'Points'...


	memset(&obdata[KNIFE], 0, sizeof(OBDATA));
	strcpy(obdata[KNIFE].name, "KNIFE");     	//The name of the object; used internally only
	obdata[KNIFE].pMap = 4;     //floorob//The image on the isometric map screen
	obdata[KNIFE].pHeld = 15;     //handob	//The picture to use on the map while it's being held
	obdata[KNIFE].damage = 30;     		//The damage value
	obdata[KNIFE].accuracy[0] = 80;     	//apunch
	obdata[KNIFE].accuracy[1] = 50;     	//athrow
	obdata[KNIFE].throw_accuracy = 0x64;     		//Always 0x64
	obdata[KNIFE].width = 1;     		//The width on the inventory
	obdata[KNIFE].height = 2;     		//The height on the inventory
	obdata[KNIFE].ammo[0] = 0xFF;     	//The three item numbers that can be ammo, 0xFF=unused
	obdata[KNIFE].ammo[1] = 0xFF;
	obdata[KNIFE].ammo[2] = 0xFF;
	obdata[KNIFE].weight = 5;     		//The weight of the object
	obdata[KNIFE].isWeapon = 1;     	//This item is a weapon. (0=no,1=yes)
	obdata[SMOKE_GRENADE].damage = 0;
	obdata[INCENDIARY_ROCKET].damage = 20;
	obdata[AUTO_CANNON_I_AMMO].damage = 15;
	obdata[CANNON_I_AMMO].damage = 15;

	obdata[PROXIMITY_GRENADE].damage = 30;

	obdata[STUN_ROD].accuracy[0] = 100;     	//apunch
	obdata[STUN_ROD].damage = 100;

	delete []buf;
}

void Item::initbigobs()
{
	bigobs = new PCK("$(xcom)/units/bigobs.pck");
	obdata[KASTET].pInv = bigobs->add_image((BITMAP *)datafile[DAT_KASTET_BMP].dat);
	obdata[KNIFE].pInv = bigobs->add_image((BITMAP *)datafile[DAT_KNIFE_BMP].dat);
}

void Item::freebigobs()
{
	delete bigobs;
	delete [] obdata; // $$$
}

int Item::explo_range(int type)
{
	int range = 5;

	switch (type) {
		case CANNON_HE_AMMO:
		case CANNON_I_AMMO:
			range = 4;
			break;
		case AUTO_CANNON_HE_AMMO:
			range = 3;
			break;
		case AUTO_CANNON_I_AMMO:
			range = 3;
			break;
		case GRENADE:
		case SMOKE_GRENADE:
			range = 5;
			break;
		case HIGH_EXPLOSIVE:
		case ALIEN_GRENADE:
			range = 8;
			break;
		case SMALL_ROCKET:
		case INCENDIARY_ROCKET:
			range = 6;
			break;
		case LARGE_ROCKET:
			range = 10;
			break;
		case PROXIMITY_GRENADE:
			range = 3;
			break;
	}
	return range;
}

void Item::od_info(int type, int gx, int gy, int gcol)
{
	text_mode(-1);
	OBDATA *od = &obdata[type];
	textprintf(screen2, font, gx, gy, gcol, "%d name=%s ", type, od->name);
	textprintf(screen2, font, gx, gy + 10, gcol,
	           "weight=%d width=%d height=%d twoHand=%d",
	           od->weight, od->width, od->height, od->twoHanded);
	textprintf(screen2, font, gx, gy + 20, gcol,
	           "rounds=%d dam=%d damType=%d hitType=%d",
	           od->rounds, od->damage, od->damageType, od->hitType);
	textprintf(screen2, font, gx, gy + 30, gcol,
	           "ammo[0]=%d ammo[1]=%d ammo[2]=%d",
	           od->ammo[0], od->ammo[1], od->ammo[2]);
	textprintf(screen2, font, gx, gy + 40, gcol,
	           "accur[0]=%d accur[1]=%d accur[2]=%d",
	           od->accuracy[0], od->accuracy[1], od->accuracy[2]);
	textprintf(screen2, font, gx, gy + 50, gcol,
	           "time[0]=%d time[1]=%d time[2]=%d",
	           od->time[0], od->time[1], od->time[2]);
	textprintf(screen2, font, gx, gy + 60, gcol,
	           "Shotable=%d Weap=%d Gun=%d Ammo=%d",
	           od->isShootable, od->isWeapon, od->isGun, od->isAmmo);
	textprintf(screen2, font, gx, gy + 70, gcol,
	           "imp=%d pInv=%d pMap=%d pHeld=%d",
	           od->importance, od->pInv, od->pMap, od->pHeld);
}


Item::Item()
{
	m_type = 0;
	m_x = 0; m_y = 0;
	m_next = NULL; m_prev = NULL; m_place = NULL;
	m_rounds = 0;
	m_delay_time = 0;
	m_ammo = NULL;

	m_health = 100;
}

Item::Item(int _type)
{
	m_type = _type;
	m_x = 0; m_y = 0;
	m_next = NULL; m_prev = NULL; m_place = NULL;
	m_rounds = data()->rounds;
	m_delay_time = 0;
	m_ammo = NULL;

	m_health = health_max();
}

Item::~Item()
{
	if (m_ammo != NULL) {
		assert(m_ammo->m_next == NULL);
		assert(m_ammo->m_prev == NULL);
		delete m_ammo;
	}
}

void Item::unlink()
{
	if (m_prev != NULL) m_prev->m_next = m_next;
	if (m_next != NULL) m_next->m_prev = m_prev;
	if (m_place != NULL && m_place->m_item == this) m_place->m_item = m_next;
	m_prev = NULL; m_next = NULL; m_place = NULL;
}

int Item::loadclip(Item *clip)
{
	assert(clip != NULL);
	//if (data()->isGun && clip->data()->isAmmo) {
	if ((m_ammo == NULL) &&
	        (memchr(data()->ammo, clip->m_type, 3) != NULL)) {
		clip->unlink();
		m_ammo = clip;
		//textprintf(screen, font, 1, 150, 1, "ammo=%s", ammo); readkey();
		return 1;
	}
	//}
	return 0;
}

Item *Item::unload()
{
	Item *t;
	t = m_ammo;
	m_ammo = NULL;
	return t;
}

int Item::haveclip()
{
	if (m_ammo != NULL)
		return 1;
	return 0;
}

void Item::shot()
{
	if (m_ammo->m_rounds > 0)
		m_ammo->m_rounds--;
	if (m_ammo->m_rounds == 0) {
		if (m_ammo != NULL) {
			delete m_ammo;
			m_ammo = NULL;
		}
	}
}

int Item::is_grenade()
{
	char grenades[6] = {GRENADE, SMOKE_GRENADE, PROXIMITY_GRENADE,
	                    HIGH_EXPLOSIVE, ALIEN_GRENADE};

	if (memchr(grenades, m_type, 5) != NULL)
		return 1;
	return 0;
}

int Item::is_laser(int type)
{
	if ((type == LASER_PISTOL) || (type == LASER_GUN) || (type == HEAVY_LASER))
		return 1;
	return 0;
}

int Item::is_cold_weapon()
{
	// Stun rod hack, so it can be used to stun!
	if ((m_type == KASTET) || (m_type == KNIFE) || (m_type == STUN_ROD))
		return 1;
	return 0;
}

int Item::is_stun_rod()
{
	if ((m_type == STUN_ROD))
		return 1;
	return 0;
}

int Item::is_knife()
{
	if ((m_type == KNIFE))
		return 1;
	return 0;
}

int Item::armourpierce()
{
	if (!data()->isGun)
		return data()->damage;
	if (m_ammo != NULL)
		return m_ammo->data()->damage;
	return 0;
}

int Item::inside(int _x, int _y)
{
	if ((m_x <= _x) && (_x < m_x + data()->width) && (m_y <= _y) && (_y < m_y + data()->height))
		return 1;
	return 0;
}

int Item::health()
{
	return m_health;
}

void Item::draw_health(int GRAPH, int gx, int gy)
{
	int color = xcom1_color(50);      //gr
	if (m_health < health_max() * 2 / 3)
		color = xcom1_color(145);      //yel
	if (m_health < health_max() / 3)
		color = xcom1_color(33);      //red
	if (GRAPH) {
		int len;     //=m_health;
		len = 14 * m_health / health_max();
		hline(screen2, gx, gy, gx + len, color);
	} else
		printsmall(gx, gy + 1, color, m_health);
}

int Item::damage(int dam)
{
	m_health -= dam;
	if (m_health <= 0)
		return 1;
	return 0;
}

int Item::health_max(int _type)
{
	int val = 5;

	switch (_type) {
		case PISTOL:
		case RIFLE:
			val = 90;
			break;
		case LASER_PISTOL:
		case LASER_GUN:
			val = 70;
			break;
		case Plasma_Pistol:
		case Plasma_Rifle:
			val = 75;
			break;
		case HEAVY_CANNON:
		case AUTO_CANNON:
			val = 80;
			break;
		case GRENADE:
		case HIGH_EXPLOSIVE:
			val = 60;
			break;
		case PISTOL_CLIP:
		case RIFLE_CLIP:
		case Plasma_Pistol_Clip:
		case Plasma_Rifle_Clip :
		case CANNON_HE_AMMO :
		case AUTO_CANNON_HE_AMMO:
			val = 100;
			break;
		default:
			val = 200;
			break;
	}
	return val;
}

bool Item::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, m_next);
	PersistWriteObject(archive, m_prev);
	PersistWriteObject(archive, m_place);
	PersistWriteObject(archive, m_ammo);

	return true;
}

bool Item::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, m_next);
	PersistReadObject(archive, m_prev);
	PersistReadObject(archive, m_place);
	PersistReadObject(archive, m_ammo);

	return true;
}
