/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

#include "stdafx.h"

#include "global.h"

#include "pck.h"
#include "item.h"
#include "video.h"
#include "map.h"
#include "multiplay.h"
#include "sound.h"
#include "colors.h"
#include "text.h"
#include "explo.h"

IMPLEMENT_PERSISTENCE(Item, "Item");

int Item::obdata_get_int(uint32 item_index, const char *property_name)
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

ALPHA_SPRITE *Item::obdata_get_bitmap(uint32 item_index, const char *property_name, int bitmap_index)
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
    ALPHA_SPRITE *result = NULL;
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, bitmap_index);
        lua_gettable(L, -2);
    }
    if (lua_isuserdata(L, -1)) {
        ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
        result = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
    }
    lua_settop(L, stack_top);
    return result;
}

SAMPLE *Item::obdata_get_sound_sample(uint32 item_index, const char *property_name, int sample_index)
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
    SAMPLE *result = NULL;
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, sample_index);
        lua_gettable(L, -2);
    }
    if (lua_isuserdata(L, -1)) {
        ASSERT(lpcd_isuserdatatype(L, -1, "SAMPLE"));
        result = (SAMPLE *)lua_unboxpointer(L, -1);
    }
    lua_settop(L, stack_top);
    return result;
}

int Item::obdata_get_array_int(uint32 item_index, const char *property_name, int index)
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

std::string Item::obdata_get_string(uint32 item_index, const char *property_name)
{
    int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
    lua_pushstring(L, "ItemsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [item_index] table
    lua_pushnumber(L, item_index);
    lua_gettable(L, -2);
    std::string result = "";
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

/**
 * Get list of ammo which fit in this weapon.
 */
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

/**
 * Return name of item.
 * If this is a stunned soldier, return his name instead.
 */
std::string Item::name()
{
    return m_stunned_body_owner ? m_stunned_body_owner->get_name() : obdata_name(m_type); 
}

/**
 * Show object stats information in the armoury
 */
void Item::od_info(int gx, int gy, int gcol)
{
	int type = m_type;
    text_mode(-1);

    textprintf(screen2, font, gx, gy, gcol, "%s", obdata_name(type).c_str());
    gy += 15;

    if (obdata_wayPoints(type) || obdata_isGun(type)) {
      //textprintf(screen2, font, gx + 5, gy, gcol, "%13s %-8s %s",
        textprintf(screen2, font, gx + 0, gy, gcol, "%13s %-8s %10s",
                       _("Firing-Type"), _("Accuracy"), _("TU cost") );
        gy += 10;
        if (obdata_accuracy(type, 0)) {
            textprintf(screen2, font, gx, gy, gcol, "%10s(%d)     %3d%%    %5d%%",
                       _("Auto"), obdata_autoShots(type), obdata_accuracy(type, 0), 
                       obdata_time(type, 0) * obdata_autoShots(type));
            gy += 10;
        }
        if (obdata_accuracy(type, 1)) {
            textprintf(screen2, font, gx, gy, gcol, "%13s     %3d%%    %5d%%",
                       _("Snap"),
                       obdata_accuracy(type, 1), obdata_time(type, 1));
            gy += 10;
        }
        if (obdata_accuracy(type, 2)) {
            textprintf(screen2, font, gx, gy, gcol, "%13s     %3d%%    %5d%%",
                       _("Aimed"),
                       obdata_accuracy(type, 2), obdata_time(type, 2));
            gy += 10;
        }
        gy += 5;
    }

    if (obdata_isHandToHand(type) || obdata_twoHanded(type)) {
        if (obdata_isHandToHand(type)) {
            textprintf(screen2, font, gx, gy, gcol, _("Hand-to-hand weapon") );
            gy += 10;
        }

        if (obdata_twoHanded(type)) {
            textprintf(screen2, font, gx, gy, gcol, _("Two-handed weapon") );
            gy += 10;
       }
       
       gy += 5;
    }
    
	//! Print medical device data 
	if (obdata_isMed(type)) {
		textprintf(screen2, font, gx, gy, gcol, _("Medical item") );
        gy += 10;

		if (obdata_heal(type, 0) && obdata_heal_max(type, 0)) {
			textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%, max %2d%%",_("Health"), obdata_heal(type, 0), obdata_heal_max(type, 0));
			gy += 10;
		}
		if (obdata_heal(type, 1) && obdata_heal_max(type, 1)) {
			textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%, max %2d%%",_("Stun"), obdata_heal(type, 1), obdata_heal_max(type, 1));
			gy += 10;
		}
		if (obdata_heal(type, 2) && obdata_heal_max(type, 2)) {
			textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%, max %2d%%",_("Energy"), obdata_heal(type, 2), obdata_heal_max(type, 2));
			gy += 10;
		}
		if (obdata_heal(type, 3) && obdata_heal_max(type, 3)) {
			textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%, max %2d%%",_("Morale"), obdata_heal(type, 3), obdata_heal_max(type, 3));
			gy += 10;
		}
	}
	//! End print medical device data
	
    if (obdata_useTime(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%",
                    _("Usage TU cost"), obdata_useTime(type));
        gy += 10;
    }
    
    if (obdata_primeTime(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%",
                    _("Prime TU cost"), obdata_primeTime(type));
        gy += 10;
    }
    
    if (obdata_throwTime(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %2d%%",
                    _("Throw TU cost"), obdata_throwTime(type));
        gy += 10;
    }

    if (obdata_damage(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %3d (%2d%%)",
                   _("Damage"),     obdata_damage(type), obdata_dDeviation(type));
		gy += 10;
		
        textprintf(screen2, font, gx, gy, gcol, "%13s: %s",
                   _("Ammo-Type"), _(get_damage_description()));
        gy += 10;
    }

    if (obdata_isAmmo(type)) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %3d", 
                   _("Rounds"), obdata_rounds(type));
        gy += 10;
    }
    
    if (obdata_exploRange(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %3d",
                   _("Blast range"), obdata_exploRange(type));
        gy += 10;
    }
    
    if (obdata_smokeRange(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %3d",
                   _("Smoke range"), obdata_smokeRange(type));
        gy += 10;
    }
    
    if (obdata_smokeTime(type) > 0) {
        textprintf(screen2, font, gx, gy, gcol, "%13s: %3d",
                   _("Smoke time"), obdata_smokeTime(type));
        gy += 10;
    }

    std::vector<std::string> ammo;
    get_ammo_list(obdata_name(type), ammo);

    for (int i = 0; i < (int)ammo.size(); i++) {
        textprintf(screen2, font, gx, gy, gcol, "%12s%d: %s", 
                   _("Ammo#"), i + 1, ammo[i].c_str());
        gy += 10;
    }

    textprintf(screen2, font, gx, gy, gcol, "%13s: %3d", 
               _("Weight"), obdata_weight(type));
    gy += 10;
    textprintf(screen2, font, gx, gy, gcol, "%13s: %3d", 
               _("Cost"), obdata_cost(type));
    gy += 10;
}

Item::Item(int _type, Soldier *stunned_body_owner)
{
    m_type = _type;
    m_x = 0; m_y = 0;
    m_next = NULL; m_prev = NULL; m_place = NULL;
    m_rounds = obdata_rounds(_type);
    m_delay_time = 0;
    m_ammo = NULL;
    m_stunned_body_owner = stunned_body_owner;
    m_sound_sample = obdata_get_sound_sample(m_type, "sound");

    m_health = obdata_maxHealth();

    m_pInv = obdata_get_bitmap(m_type, "pInv"); // Picture for inventory
    ASSERT(m_pInv);
    m_pMap = obdata_get_bitmap(m_type, "pMap"); // Picture for battlemap
    ASSERT(m_pMap);
    for (int i = 0; i < 8; i++) {
        // Pictures for items in hands
        m_pHeld[i] = obdata_get_bitmap(m_type, "pHeld", i + 1);
        ASSERT(m_pHeld[i]);
    }
    
    std::string sound = obdata_get_string(m_type, "sound");
    m_sound = getSymCode(sound.c_str());
    if (!sound.empty() && m_sound == SS_UNKNOWN) {
        lua_safe_dostring(L, 
            (std::string("Warning([[%s]], [[Invalid sound effect identifier - '") + 
            sound + std::string("']])")).c_str());
    }
}

Item::~Item()
{
    ASSERT(m_prev == NULL);
    ASSERT(m_next == NULL);
    ASSERT(m_place == NULL);
    if (m_ammo != NULL) {
        ASSERT(m_ammo->m_next == NULL);
        ASSERT(m_ammo->m_prev == NULL);
        delete m_ammo;
    }
}

/**
 * Remove the item from the list containing it.
 */
void Item::unlink()
{
    if (m_prev != NULL) m_prev->m_next = m_next;	/* unlink from list */
    if (m_next != NULL) m_next->m_prev = m_prev;
    if (m_place != NULL && m_place->m_item == this) m_place->m_item = m_next;	/* If this was the first item on a place, delete from that */
    m_prev = NULL; m_next = NULL; m_place = NULL;
}

/**
 * Load clip into weapon.
 * @param clip The clip to load. Will contain the unloaded clip.
 * @return 1 if successful.
 */
int Item::loadclip(Item *&clip)
{
    ASSERT(clip != NULL);
    if (can_use_ammo_type(obdata_name(clip->m_type))) {
        clip->unlink();	// remove loaded clip from original container
        Item *it = m_ammo;
        m_ammo = clip;
        clip = it;	// pass the unloaded clip
        return 1;
    }
    return 0;
}

/**
 * Unload ammo from weapon and return ammo item.
 */
Item *Item::unload()
{
    Item *t;
    t = m_ammo;
    m_ammo = NULL;
    return t;
}

/**
 * Returns 1 if this weapon is loaded.
 */
int Item::haveclip()
{
    if (m_ammo != NULL)
        return 1;
    return 0;
}

/**
 * Shot one with this weapon.
 * Decrements ammo rounds, and unloads the empty clip automatically, if needed.
 */
void Item::shot()
{
	ASSERT (m_ammo != NULL);
    if (m_ammo->m_rounds > 0)
        m_ammo->m_rounds--;
    if (m_ammo->m_rounds == 0 && m_ammo->obdata_disappear()) {
        if (m_ammo != NULL) {
            elist->remove(m_ammo);
            delete m_ammo;
            m_ammo = NULL;
        }
    }
}

int Item::inside(int _x, int _y)
{
    if ((m_x <= _x) && (_x < m_x + obdata_width()) && (m_y <= _y) && (_y < m_y + obdata_height()))
        return 1;
    return 0;
}

void Item::draw_health(BITMAP *dest, int GRAPH, int gx, int gy)
{
    int color = COLOR_GREEN;        //gr
    if (m_health < obdata_maxHealth() * 2 / 3)
        color = COLOR_YELLOW01;     //yel
    if (m_health < obdata_maxHealth() / 3)
        color = COLOR_RED01;        //red
    if (GRAPH) {
        int len;     //=m_health;
        len = 14 * m_health / obdata_maxHealth();
        hline(dest, gx, gy, gx + len, color);
    } else
        printsmall_x(dest, gx, gy + 1, color, m_health);
}

/**
 * Damage item (for example an explosion)
 * @return true if item health is 0, ie. destroyed
 */
int Item::damage(int dam)
{
    if (m_stunned_body_owner != NULL) {
        if (m_stunned_body_owner->ud.CurHealth <= dam) {
            m_stunned_body_owner->ud.CurHealth = 0;
            m_stunned_body_owner = NULL;
        } else {
            m_stunned_body_owner->ud.CurHealth -= dam;
        }
    } else {
        m_health -= dam;
        if (m_health <= 0)
            return 1;
    }
    return 0;
}

/**
 * Return two letter damage name.
 */
const char* Item::get_damage_name()
{
	static const char* damage_names[7] = {"AP", "IN", "HE", "LS", "PL", "ST"};
	ASSERT (m_ammo != NULL);
    int damage_type = m_ammo->obdata_damageType();
    
    if (haveclip() && damage_type >= 0 && damage_type < 6)
        return damage_names[damage_type];
    return "";
}

/**
 * Return long damage name.
 */
const char* Item::get_damage_description()
{
	static const char* damage_longnames[7] = {N_("Armour piercing"), N_("Incendiary"),
		N_("High explosive"), N_("Laser"), N_("Plasma"), N_("Stun")};
    
    if (haveclip()) {
    	if (m_ammo == NULL)
    		return "";
	    int damage_type = m_ammo->obdata_damageType();
    	if (damage_type >= 0 && damage_type < 6)
			return damage_longnames[damage_type];
		return "";
	} else {
		int damage_type = obdata_damageType();
    	if (damage_type >= 0 && damage_type < 6)
			return damage_longnames[damage_type];
		return "";
	}
	return "";
}

/**
 * Create an exact copy of the item including loaded clip
 * @return the copy
 */
Item *Item::create_duplicate()
{
    Item *it = new Item(m_type);
    if (haveclip()) {
        Item *ammo = new Item(clip()->m_type);
        it->loadclip(ammo);
    }
    return it;
}

/**
 * Create a new item by using symbolic name
 *
 * @param item_name item name
 */
Item *create_item(const char *item_name, Soldier *stunned_body_owner)
{
    Item *result = NULL;
    int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
    lua_pushstring(L, "ItemsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [item_name] table
    lua_pushstring(L, item_name);
    lua_gettable(L, -2);
    if (!lua_isnil(L, -1)) {
        lua_pushstring(L, "index");
        lua_gettable(L, -2);
        ASSERT(lua_isnumber(L, -1));
        result = new Item((uint32)lua_tonumber(L, -1), stunned_body_owner);
    }
    lua_settop(L, stack_top);
    return result;
}
/**
 * Returns a custom color for bullets
 * 
 * @param int : 0 or 1 or 2
 */
int Item::get_color(int index, int n)
{
    int cr = 0, cg = 0, cb = 0;
    int _type = index;
    switch (n){
        case 0:
            cr = obdata_get_array_int(_type, "bulletRGB", 0);
            cg = obdata_get_array_int(_type, "bulletRGB", 1);
            cb = obdata_get_array_int(_type, "bulletRGB", 2);
        break;
        case 1:
            cr = obdata_get_array_int(_type, "glowRGB", 0);
            cg = obdata_get_array_int(_type, "glowRGB", 1);
            cb = obdata_get_array_int(_type, "glowRGB", 2);
        break;
        case 2:
            cr = obdata_get_array_int(_type, "trailRGB", 0);
            cg = obdata_get_array_int(_type, "trailRGB", 1);
            cb = obdata_get_array_int(_type, "trailRGB", 2);
        break;
        }
    return makecol(cr,cg,cb);
}
/**
 * Checks if given item has custom colors for bullets
 */
bool Item::can_set_color(int index)
{
    int _type = index;
    bool ans = true;
    if ((!obdata_get_array_int(_type, "bulletRGB", 0) && !obdata_get_array_int(_type, "bulletRGB", 1) && !obdata_get_array_int(_type, "bulletRGB", 2)) && !(obdata_damageType(_type) == DT_LAS)&& obdata_isGun(_type)) {
        ans = false;
        }
    if (!obdata_get_array_int(_type, "glowRGB", 0) && !obdata_get_array_int(_type, "glowRGB", 1) && !obdata_get_array_int(_type, "glowRGB", 2)) {
        ans = false;
        }
    if ((!obdata_get_array_int(_type, "trailRGB", 0) && !obdata_get_array_int(_type, "trailRGB", 1) && !obdata_get_array_int(_type, "trailRGB", 2)) && !(obdata_damageType(_type) == DT_LAS) && obdata_isGun(_type)) {
        ans = false;
        }
    return ans;
}

bool Item::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);

    PersistWriteObject(archive, m_next);
    PersistWriteObject(archive, m_prev);
    PersistWriteObject(archive, m_place);
    PersistWriteObject(archive, m_ammo);
    PersistWriteObject(archive, m_stunned_body_owner);

    return true;
}

bool Item::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);

    PersistReadObject(archive, m_next);
    PersistReadObject(archive, m_prev);
    PersistReadObject(archive, m_place);
    PersistReadObject(archive, m_ammo);
    PersistReadObject(archive, m_stunned_body_owner);

    m_sound_sample = obdata_get_sound_sample(m_type, "sound");
    m_pInv = obdata_get_bitmap(m_type, "pInv");
    ASSERT(m_pInv);
    m_pMap = obdata_get_bitmap(m_type, "pMap");
    ASSERT(m_pMap);
    for (int i = 0; i < 8; i++) {
        m_pHeld[i] = obdata_get_bitmap(m_type, "pHeld", i + 1);
        ASSERT(m_pHeld[i]);
    }

    return true;
}

