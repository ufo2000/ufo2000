/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2004  ufo2000 development team

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

PCK *bigobs;

IMPLEMENT_PERSISTENCE(Item, "Item");

int Item::obdata_get_int(int item_index, const char *property_name)
{
	int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
	lua_pushstring(L, "ItemsTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [item_index] table
	lua_pushnumber(L, item_index);
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
    // Get property value
	lua_pushstring(L, property_name);
	lua_gettable(L, -2);
	int result = 0;
	if (lua_isnumber(L, -1)) result = (int)lua_tonumber(L, -1);
	lua_settop(L, stack_top);
	return result;
}

int Item::obdata_get_array_int(int item_index, const char *property_name, int index)
{
	int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
	lua_pushstring(L, "ItemsTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [item_index] table
	lua_pushnumber(L, item_index);
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
    // Get property value
	lua_pushstring(L, property_name);
	lua_gettable(L, -2);
	if (!lua_istable(L, -1)) {
		lua_settop(L, stack_top);
		return 0;
	}
    // Index a value inside of property
	lua_pushnumber(L, index + 1);
	lua_gettable(L, -2);

	int result = 0;
	if (lua_isnumber(L, -1)) result = (int)lua_tonumber(L, -1);

	lua_settop(L, stack_top);
	return result;
}

std::string Item::obdata_get_string(int item_index, const char *property_name)
{
	int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
	lua_pushstring(L, "ItemsTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [item_index] table
	lua_pushnumber(L, item_index);
	lua_gettable(L, -2);
	std::string result = "<empty>";
	if (lua_istable(L, -1)) {
	    // Get property value
		lua_pushstring(L, property_name);
		lua_gettable(L, -2);
		if (lua_isstring(L, -1)) {
			result = lua_tostring(L, -1);
		}
	}
	lua_settop(L, stack_top);
	return result;
}

bool Item::get_ammo_list(const std::string item_name, std::vector<std::string> &ammo)
{
	ammo.clear();

	int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
	lua_pushstring(L, "ItemsTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [item_index] table
	lua_pushstring(L, item_name.c_str());
	lua_gettable(L, -2);
	if (!lua_istable(L, -1)) {
		lua_settop(L, stack_top);
		return false;
	}
	lua_pushstring(L, "ammo");
	lua_gettable(L, -2);
	if (!lua_istable(L, -1)) {
		lua_settop(L, stack_top);
		return true;
	}

	int i = 1;

	while (true) {
		lua_pushnumber(L, i);
		lua_gettable(L, -2);
		if (!lua_isstring(L, -1)) {
			lua_settop(L, stack_top);
			return true;
		}
		ammo.push_back(lua_tostring(L, -1));
		lua_pop(L, 1);
		i++;
	}
}

void Item::initbigobs()
{
	bigobs = new PCK("$(xcom)/units/bigobs.pck");
}

void Item::freebigobs()
{
	delete bigobs;
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

/**
 * Show object stats information in the armoury
 */
void Item::od_info(int type, int gx, int gy, int gcol)
{
	text_mode(-1);

	textprintf(screen2, font, gx, gy, gcol, "%s", obdata_name(type).c_str());
	gy += 15;

	if (obdata_wayPoints(type) || obdata_isGun(type)) {
		textprintf(screen2, font, gx + 5, gy, gcol, "Type   Accuracy  TUs cost");
		gy += 10;
		if (obdata_accuracy(type, 0)) {
			textprintf(screen2, font, gx, gy, gcol, "Auto     %3d%%      %3d%%",
			           obdata_accuracy(type, 0), obdata_time(type, 0));
			gy += 10;
		}
		if (obdata_accuracy(type, 1)) {
			textprintf(screen2, font, gx, gy, gcol, "Snap     %3d%%      %3d%%",
			           obdata_accuracy(type, 1), obdata_time(type, 1));
			gy += 10;
		}
		if (obdata_accuracy(type, 2)) {
			textprintf(screen2, font, gx, gy, gcol, "Aimed    %3d%%      %3d%%",
			           obdata_accuracy(type, 2), obdata_time(type, 2));
			gy += 10;
		}
		gy += 5;
	}

	if (obdata_twoHanded(type)) {
		textprintf(screen2, font, gx, gy, gcol, "Two-handed weapon");
		gy += 15;
	}

	if (obdata_damage(type) > 0) {
		textprintf(screen2, font, gx, gy, gcol, "Damage: %d  Type: %d", obdata_damage(type),
		(obdata_hitType(type) > 0 ? obdata_hitType(type) : obdata_damageType(type)));
		gy += 10;
	}

	if (obdata_isAmmo(type)) {
		textprintf(screen2, font, gx, gy, gcol, "Rounds: %d", obdata_rounds(type));
		gy += 10;
	}

	std::vector<std::string> ammo;
	get_ammo_list(obdata_name(type), ammo);

	for (int i = 0; i < (int)ammo.size(); i++) {
		textprintf(screen2, font, gx, gy, gcol, " Ammo%d: %s", i + 1, ammo[i].c_str());
		gy += 10;
	}

	textprintf(screen2, font, gx, gy, gcol, "Weight: %d", obdata_weight(type));
	gy += 10;
	textprintf(screen2, font, gx, gy, gcol, "  Cost: %d", obdata_cost(type));
	gy += 10;
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
	m_rounds = obdata_rounds(_type);
	m_delay_time = 0;
	m_ammo = NULL;

	m_health = health_max();
}

Item::~Item()
{
	if (m_ammo != NULL) {
		ASSERT(m_ammo->m_next == NULL);
		ASSERT(m_ammo->m_prev == NULL);
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
	ASSERT(clip != NULL);
	if ((m_ammo == NULL) && can_use_ammo_type(obdata_name(clip->m_type))) {
		clip->unlink();
		m_ammo = clip;
		return 1;
	}
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

int Item::inside(int _x, int _y)
{
	if ((m_x <= _x) && (_x < m_x + obdata_width()) && (m_y <= _y) && (_y < m_y + obdata_height()))
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
