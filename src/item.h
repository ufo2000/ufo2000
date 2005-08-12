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

    uint32 m_type;
    int    m_x, m_y;
    Item  *m_next, *m_prev;
    Place *m_place;

    int  m_rounds;
    int  m_delay_time;
    Item *m_ammo;
    
    int m_health;

    BITMAP *m_pMap;
    BITMAP *m_pInv;
    BITMAP *m_pHeld[8];

    SoundSym_e_t m_sound;

public:
    static void od_info(int type, int gx, int gy, int gcol);

    static int obdata_get_int(uint32 item_index, const char *property_name);
    static int obdata_get_array_int(uint32 item_index, const char *property_name, int index);
    static BITMAP *obdata_get_bitmap(uint32 item_index, const char *property_name, int bitmap_index = -1);
    static std::string obdata_get_string(uint32 item_index, const char *property_name);

    static int obdata_maxHealth(int index) { return obdata_get_int(index, "health"); }
    static int obdata_damage(int index) { return obdata_get_int(index, "damage"); }
    static int obdata_dDeviation(int index) { return obdata_get_int(index, "dDeviation"); }
    static int obdata_exploRange(int index) { return obdata_get_int(index, "exploRange"); }
    static int obdata_smokeRange(int index) { return obdata_get_int(index, "smokeRange"); }
    static int obdata_smokeTime(int index) { return obdata_get_int(index, "smokeTime"); }
    static int obdata_cost(int index) { return obdata_get_int(index, "cost"); }
    static int obdata_isAmmo(int index) { return obdata_get_int(index, "isAmmo"); }
    static int obdata_isGrenade(int index) { return obdata_get_int(index, "isGrenade"); }
    static int obdata_isHighExplosive(int index) { return obdata_get_int(index, "isHighExplosive"); }
    static int obdata_isProximityGrenade(int index) { return obdata_get_int(index, "isProximityGrenade"); }
    static int obdata_reloadTime(int index) { return obdata_get_int(index, "reloadTime"); }
    static std::string obdata_name(int index) { return obdata_get_string(index, "name"); }
    static int obdata_damageType(int index) { return obdata_get_int(index, "damageType"); }
    static int obdata_wayPoints(int index) { return obdata_get_int(index, "wayPoints"); }
    static int obdata_accuracy(int index, int n) { return obdata_get_array_int(index, "accuracy", n); }
    static int obdata_time(int index, int n) { return obdata_get_array_int(index, "time", n); }
    static int obdata_useTime(int index) { return obdata_get_int(index, "useTime"); }
    static int obdata_autoShots(int index) { return obdata_get_int(index, "autoShots"); }
    static int obdata_weight(int index) { return obdata_get_int(index, "weight"); }
    static int obdata_twoHanded(int index) { return obdata_get_int(index, "twoHanded"); }
    static int obdata_rounds(int index) { return obdata_get_int(index, "rounds"); }
    static int obdata_isHandToHand(int index) { return obdata_get_int(index, "isHandToHand"); }
    static int obdata_disappear(int index) { return obdata_get_int(index, "disappear"); }
    static int obdata_isGun(int index) { return obdata_get_int(index, "isGun"); }
    static int obdata_minimapMark(int index) { return obdata_get_int(index, "minimapMark"); }

    char* get_damage_name();
    //! Get list of ammo types that can be used with this weapon
    static bool get_ammo_list(const std::string itemname, std::vector<std::string> &ammo);

    Item();
    Item(int _type);
    virtual ~Item();

    //! Laser damage
    int is_laser() { return ((obdata_damageType(m_type) == DT_LAS) && obdata_isGun(m_type)); }
    //! Hand-to-hand weapon
    int is_hand_to_hand() { return obdata_isHandToHand(m_type); }
    //! Stun damage
    int is_stun_weapon() { return (obdata_damageType(m_type) == DT_STUN); }
    //! Check if this is a grenade (something explosive that needs to be thrown)
    int is_grenade() { return obdata_isGrenade(m_type); }
    //! Check if it is high explosive (explosion triggered by timer)
    int is_high_explosive() { return obdata_isHighExplosive(m_type); }
    //! Check if it is proximity grenade (explosion triggered by movement)
    int is_proximity_grenade() { return obdata_isProximityGrenade(m_type); }

    int loadclip(Item *&clip);
    int haveclip();

    Item *unload();
    void shot();
    void unlink();

    int inside(int _x, int _y);

    std::string name() { return obdata_name(m_type); }
    BITMAP *obdata_pMap() { return m_pMap; }
    BITMAP *obdata_pInv() { return m_pInv; }
    BITMAP *obdata_pHeld(int dir) { ASSERT(dir >= 0 && dir < 8); return m_pHeld[dir]; }
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
    int obdata_useTime() { return obdata_useTime(m_type); }
    int obdata_autoShots() { return obdata_autoShots(m_type); }
    int obdata_importance() { return obdata_get_int(m_type, "importance"); }
    int obdata_weight() { return obdata_weight(m_type); }
    int obdata_disappear() { return obdata_disappear(m_type); }
    
    int get_cost() { return obdata_cost(m_type); }

    bool can_use_ammo_type(const std::string &ammo_type)
    {
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

    static SoundSym_e_t obdata_get_sound(int type)
    {
        std::string sound = obdata_get_string(type, "sound");
        return getSymCode(sound.c_str());
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

Item *create_item(const char *item_name);

bool is_item_allowed(int type);

#endif
