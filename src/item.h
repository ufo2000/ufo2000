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
#ifndef ITEM_H
#define ITEM_H
#include <assert.h>

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

struct OBDATA
{
	char          name[20];       //The name of the object; used internally only
	unsigned char pInv;           //The image on the inventory screen
	unsigned char pMap;           //floorob//The image on the isometric map screen
	unsigned char damage;         //The damage value
	unsigned char importance;     //In a stack of items, the item with the highest importance is shown
	unsigned char width;          //The width on the inventory
	unsigned char height;         //The height on the inventory
	unsigned char ammo[3];        //The three item numbers that can be ammo, 0xFF=unused
	unsigned char pHeld;          //handob	//The picture to use on the map while it's being held
	unsigned char _1E;            //Changing this seems to have no effect...
	unsigned char damageType;     //The type of attack; refer to DT_nnnn
	unsigned char accuracy[3];    //The accuracy for each of the three kinds of shots; 0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	unsigned char time[3];        //The TU% for each of the three kinds of shots.  0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	unsigned char rounds;         //The number of rounds in the clip
	unsigned char hitType;        //The type of damage that hitting with it does.  Only matters on stun rods/thermal tazers.  Refer to DT_nnnn
	unsigned char throw_accuracy; //Always 0x64
	unsigned char _29;            //Always 0x32
	unsigned char _2A;
	unsigned char weight;         //The weight of the object
	unsigned char primeType;      //The way in which the object can be primed.  See PT_nnnn
	unsigned char _2D;
	unsigned char isShootable;    //This item can be shot. (0=no,1=yes)
	unsigned char isWeapon;       //This item is a weapon. (0=no,1=yes)
	unsigned char isGun;          //This is a gun which requires ammo. (0=no,1=yes)
	unsigned char isAmmo;         //This item is a clip. (0=no,1=yes)
	unsigned char twoHanded;      //Is more accurate with two hands (0=no,1=yes)
	unsigned char wayPoints;      //Has a 'launch missile' option  (0=no,1=yes)
	unsigned char _34;            //Scott T. Jones has this marked as 'Research'.  This makes sense, but I can find no evidence to support it...
	unsigned char _35;            //Scott T. Jones has this marked as 'Points'...
};

enum Action { NONE, THROW, PRIME, SNAPSHOT, AIMEDSHOT, AUTOSHOT, PUNCH, AIMEDTHROW };

class Place;

class Item: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Item);
public:
	static int obdata_num;
	static OBDATA *obdata;

private:
	int  m_type;
	int  m_x, m_y;
	Item *m_next, *m_prev;
	Place *m_place;

	int  m_rounds;
	int  m_delay_time;
	Item *m_ammo;

public:
	static void initobdata();
	static void initbigobs();
	static void freebigobs();
	static int explo_range(int type);
	static void od_info(int type, int gx, int gy, int gcol);

	Item();
	Item(int _type);
	virtual ~Item();

	static int health_max(int _type);
	int health_max() { return health_max(m_type); }
	static int is_laser(int _type);
	int is_laser() { return is_laser(m_type); };
	int is_grenade();
	int is_cold_weapon();
	int is_stun_rod();
	int is_knife();

	int loadclip(Item *clip);
	int haveclip();

	Item *unload();
	void shot();
	int armourpierce();
	void unlink();

	int inside(int _x, int _y);

	OBDATA *data() { return &obdata[m_type]; }
	Item *clip() { return m_ammo; }
	int cliptype() { return m_ammo->m_type; }
	int roundsremain() { return m_ammo->m_rounds; }
	void setpos(int _x, int _y) { m_x = _x; m_y = _y; }
	int explo_range() { return explo_range(m_type); }
	void set_delay_time(int dt) { m_delay_time = dt; }
	int delay_time() { return m_delay_time; }
	int is_explo();

	void od_info(int gx, int gy, int gcol)
	{
		ASSERT((m_type >= 0) && (m_type < obdata_num));
		od_info(m_type, gx, gy, gcol);
	}

	friend class Place;
	friend class Explosive;
	friend class Bullet;
	friend class Soldier;
	friend class Icon;
	friend class Inventory;
	friend class Map;
	friend class Editor;

	int m_health;
	int health();
	void draw_health(int GRAPH, int gx, int gy);
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
#define NEW_ITEMS_NUM       2

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
#define Snakeman_Corpse     0x33
#define Ethereal_Corpse     0x34
#define Muton_Corpse        0x35
#define Floater_Corpse      0x36
#define Celatid_Corpse      0x37
#define Silacoid_Corpse     0x38
#define Chryssalid_Corpse   0x39
#define reaper_corpse_1     0x3a
#define reaper_corpse_2     0x3b
#define reaper_corpse_3     0x3c
#define reaper_corpse_4     0x3d
#define cyber_1             0x3e
#define cyber_2             0x3f
#define cyber_3             0x40
#define cyber_4             0x41
#define secto_1             0x42
#define secto_2             0x43
#define secto_3             0x44
#define secto_4             0x45
#define hover_1             0x46
#define hover_2             0x47
#define hover_3             0x48
#define hover_4             0x49
#define TANK1               0x4a
#define TANK_2              0x4b
#define TANK_3              0x4c
#define TANK_4              0x4d
#define CIVM                0x4e
#define CIVF                0x4f

inline int Item::is_explo() 
{
	return (m_type == HIGH_EXPLOSIVE);
}

#endif
