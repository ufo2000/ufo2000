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
#ifndef ITEM_H
#define ITEM_H

#define DT_AP   0x00  //Armor Piercing
#define DT_INC  0x01  //Incendiary/Phosphorous
#define DT_HE   0x02  //High Explosive
#define DT_LAS  0x03  //Laser/Gauss
#define DT_PLAS 0x04  //Plasma/Sonic
#define DT_STUN 0x05  //Stun/Freeze
#define DT_NONE 0xFF  //No attack 
//Some items have an 0x08 or other values in the Hit Type.  This appears
//to be vestigal from the original HIT option...

#define PT_None      0x00
#define PT_Grenade   0x01
#define PT_Smoke     0x02
#define PT_Proximity 0x03

#include "sound.h"

class Place;
class Editor;

/**
 * Item object. Stores all the needed information about item including
 * properties, sound effects and item image.
 *
 * @ingroup planner
 * @ingroup battlescape
 */
class Item: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Item);
private:

	int  m_type;
	int  m_x, m_y;
	Item *m_next, *m_prev;
	Place *m_place;

	int  m_rounds;
	int  m_delay_time;
	Item *m_ammo;
	
	int m_health;

	BITMAP *m_pMap;
	BITMAP *m_pInv;

	SoundSym_e_t m_sound;

public:
	static void od_info(int type, int gx, int gy, int gcol);

	static int obdata_get_int(int item_index, const char *property_name);
	static int obdata_get_array_int(int item_index, const char *property_name, int index);
	static BITMAP *obdata_get_bitmap(int item_index, const char *property_name);
	static std::string obdata_get_string(int item_index, const char *property_name);

	static int obdata_maxHealth(int index) { return obdata_get_int(index, "health"); }
	static int obdata_damage(int index) { return obdata_get_int(index, "damage"); }
	static int obdata_dDeviation(int index) { return obdata_get_int(index, "dDeviation"); }
	static int obdata_exploRange(int index) { return obdata_get_int(index, "exploRange"); }
	static int obdata_smokeRange(int index) { return obdata_get_int(index, "smokeRange"); }
	static int obdata_smokeTime(int index) { return obdata_get_int(index, "smokeTime"); }
	static int obdata_cost(int index) { return obdata_get_int(index, "cost"); }
	static int obdata_isAmmo(int index) { return obdata_get_int(index, "isAmmo"); }
	static int obdata_reloadTime(int index) { return obdata_get_int(index, "reloadTime"); }
	static std::string obdata_name(int index) { return obdata_get_string(index, "name"); }
	static int obdata_damageType(int index) { return obdata_get_int(index, "damageType"); }
	static int obdata_wayPoints(int index) { return obdata_get_int(index, "wayPoints"); }
	static int obdata_accuracy(int index, int n) { return obdata_get_array_int(index, "accuracy", n); }
	static int obdata_time(int index, int n) { return obdata_get_array_int(index, "time", n); }
	static int obdata_weight(int index) { return obdata_get_int(index, "weight"); }
	static int obdata_twoHanded(int index) { return obdata_get_int(index, "twoHanded"); }
	static int obdata_hitType(int index) { return obdata_get_int(index, "hitType"); }
	static int obdata_rounds(int index) { return obdata_get_int(index, "rounds"); }
	static int obdata_disappear(int index) { return obdata_get_int(index, "disappear"); }
	static int obdata_isGun(int index) { return obdata_get_int(index, "isGun"); }

  char* get_damage_name();
    //! Get list of ammo types that can be used with this weapon
	static bool get_ammo_list(const std::string itemname, std::vector<std::string> &ammo);

	Item();
	Item(int _type);
	virtual ~Item();

	int is_laser() { return ((obdata_damageType(m_type) == DT_LAS) && obdata_isGun(m_type)); };
	int is_grenade();
	int is_cold_weapon();
	int is_stun_rod();
	int is_knife();

	int loadclip(Item *clip);
	int haveclip();

	Item *unload();
	void shot();
	void unlink();

	int inside(int _x, int _y);

	std::string name() { return obdata_name(m_type); }
	BITMAP *obdata_pMap() { return m_pMap; }
	BITMAP *obdata_pInv() { return m_pInv; }
	int obdata_pHeld() { return obdata_get_int(m_type, "pHeld"); } // FIXME
	int obdata_width() { return obdata_get_int(m_type, "width"); }
	int obdata_height() { return obdata_get_int(m_type, "height"); }
	int obdata_isAmmo() { return obdata_isAmmo(m_type); }
	int obdata_reloadTime() { return obdata_reloadTime(m_type); }
	int obdata_isGun() { return obdata_isGun(m_type); }
	int obdata_twoHanded() { return obdata_twoHanded(m_type); }
	int obdata_maxHealth() { return obdata_maxHealth(m_type); }
	int obdata_damage() { return obdata_damage(m_type); }
	int obdata_dDeviation() { return obdata_dDeviation(m_type); }
	int obdata_exploRange() { return obdata_exploRange(m_type); }
	int obdata_smokeRange() { return obdata_smokeRange(m_type); }
	int obdata_smokeTime() { return obdata_smokeRange(m_type); }
	int obdata_accuracy(int n) { return obdata_get_array_int(m_type, "accuracy", n); }
	int obdata_time(int n) { return obdata_get_array_int(m_type, "time", n); }
	int obdata_importance() { return obdata_get_int(m_type, "importance"); }
	int obdata_weight() { return obdata_weight(m_type); }
	int obdata_disappear() { return obdata_disappear(m_type); }

	bool can_use_ammo_type(const std::string &ammo_type) {
		std::vector<std::string> ammo_list;
		get_ammo_list(obdata_name(m_type), ammo_list);
		for (int i = 0; i < (int)ammo_list.size(); i++)
			if (ammo_list[i] == ammo_type) return true;
		return false;
	}
	
	Item *clip() { return m_ammo; }
	int cliptype() { ASSERT(m_ammo); return m_ammo->m_type; }
	int itemtype() { return m_type; }
	int roundsremain() { return m_ammo ? m_ammo->m_rounds : 0; }
	void setpos(int _x, int _y) { m_x = _x; m_y = _y; }
	void set_delay_time(int dt) { m_delay_time = dt; }
	int delay_time() { return m_delay_time; }
	int is_explo();

	SoundSym_e_t get_sound()
	{ 
		return m_sound; 
	}

	void od_info(int gx, int gy, int gcol)
	{
		od_info(m_type, gx, gy, gcol);
	}

	friend class Place;
	friend class Icon;
	friend class Inventory;
	friend class Map;
	friend class Editor;

	int health()
	{
		return m_health;
	}
	void draw_health(BITMAP *dest, int GRAPH, int gx, int gy);
	int damage(int dam);

	Place *get_place() { return m_place; }

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

struct Target
{
	int    accur, time;
	Action action;
	Item   *item;
	int    place;
};

#define KASTET              0x50
#define KNIFE               0x51
//#define NEW_ITEMS_NUM       2

#define PISTOL              0x00
#define PISTOL_CLIP         0x01
#define RIFLE               0x02
#define RIFLE_CLIP          0x03
#define HEAVY_CANNON        0x04
#define CANNON_AP_AMMO      0x05
#define CANNON_HE_AMMO      0x06
#define CANNON_I_AMMO       0x07
#define AUTO_CANNON         0x08
#define AUTO_CANNON_AP_AMMO 0x09
#define AUTO_CANNON_HE_AMMO 0x0a
#define AUTO_CANNON_I_AMMO  0x0b
#define ROCKET_LAUNCHER     0x0c
#define SMALL_ROCKET        0x0d
#define LARGE_ROCKET        0x0e
#define INCENDIARY_ROCKET   0x0f
#define LASER_PISTOL        0x10
#define LASER_GUN           0x11
#define HEAVY_LASER         0x12
#define GRENADE             0x13
#define SMOKE_GRENADE       0x14
#define PROXIMITY_GRENADE   0x15
#define HIGH_EXPLOSIVE      0x16
#define MOTION_SCANNER      0x17
#define MEDI_KIT            0x18
#define PSI_AMP             0x19
#define STUN_ROD            0x1a
#define Flare               0x1b 
//#define empty               0x1c
//#define empty               0x1d
//#define empty               0x1e
#define CORPSE              0x1f
#define CORPSE_ARMOUR       0x20
#define CORPSE_POWER_SUIT   0x21
#define Heavy_Plasma        0x22
#define Heavy_Plasma_Clip   0x23
#define Plasma_Rifle        0x24
#define Plasma_Rifle_Clip   0x25
#define Plasma_Pistol       0x26
#define Plasma_Pistol_Clip  0x27
#define BLASTER_LAUNCHER    0x28
#define BLASTER_BOMB        0x29
#define SMALL_LAUNCHER      0x2a
#define STUN_MISSILE        0x2b
#define ALIEN_GRENADE       0x2c
#define ELERIUM_115         0x2d
#define MIND_PROBE          0x2e 
//#define >>UNDEFINED_<<      0x2f
//#define >>_empty_<<         0x30
//#define >>_empty_<<         0x31
#define Sectoid_Corpse      0x32
//#define Snakeman_Corpse     0x33
//#define Ethereal_Corpse     0x34
#define Muton_Corpse        0x35

inline int Item::is_explo() 
{
	return (m_type == HIGH_EXPLOSIVE);
}

bool is_item_allowed(int type);

#endif
