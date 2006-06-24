/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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

/**
 * @file script_api.h
 * @brief Bindings for calling lua functions from C++ and vice versa
 *
 * This file contains all the "black magic" involved in using lua stack 
 * based api and makes other parts of the game simplier
 */

#include "stdafx.h"
#include "global.h"
#include "script_api.h"

/**
 * Gets the name of the currently selected equipment set
 */
const char *get_current_equipment_name()
{
    const char *result = NULL;
    lua_pushstring(L, "GetCurrentEquipmentName");
    lua_gettable(L, LUA_GLOBALSINDEX);
    lua_safe_call(L, 0, 1);
    if (lua_isstring(L, -1)) result = lua_tostring(L, -1);
    lua_pop(L, 1);
    return result;
}

/**
 * Tries to set new equipment
 */
bool set_current_equipment_name(const char *equipment_name)
{
    lua_pushstring(L, "SetEquipment");
    lua_gettable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, equipment_name);
    lua_safe_call(L, 1, 1);
    bool result = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (result) {
        g_default_weaponset = equipment_name;
    }
    return result;
}

/**
 * Gets the list of allowed equipment sets, returns currently active set
 *
 * @returns index of currently active equipment or -1 on error
 */
int query_equipment_sets(std::vector<std::string> &eqsets)
{
    eqsets.clear();
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "EquipmentTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        ASSERT(lua_isstring(L, -2));
        ASSERT(lua_istable(L, -1));

        lua_pushstring(L, "enabled");
        lua_gettable(L, -2);
        if (lua_toboolean(L, -1)) {
            eqsets.push_back(lua_tostring(L, -3));
        }
        lua_pop(L, 2);
    }
    lua_settop(L, stack_top);

    std::sort(eqsets.begin(), eqsets.end());

    const char *current_equipment_name = get_current_equipment_name();
    if (!current_equipment_name) return -1;

    for (int i = 0; i < (int)eqsets.size(); i++)
        if (eqsets[i] == current_equipment_name)
            return i;

    return -1;
}

int query_languages(std::vector<std::string> &languages)
{
    languages.clear();
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "LanguagesList");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        ASSERT(lua_isstring(L, -2));
        ASSERT(lua_isstring(L, -1));
        languages.push_back(lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_settop(L, stack_top);

    std::sort(languages.begin(), languages.end());
    return languages.size();
}


ALPHA_SPRITE *lua_table_image(const char *name)
{
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "ImageTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1));
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
    ALPHA_SPRITE *spr = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
    lua_settop(L, stack_top);
    
    return spr;
}

std::vector<ALPHA_SPRITE *> lua_table_image_vector(const char *name)
{
    std::vector<ALPHA_SPRITE *> res;

    int stack_top = lua_gettop(L);
    lua_pushstring(L, "ImageTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1));
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    
    int i = 1;
    while (true) {
        lua_pushnumber(L, i);
        lua_gettable(L, -2);
        if (!lua_isuserdata(L, -1)) {
            lua_settop(L, stack_top);
            return res;
        }
        ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
        res.push_back((ALPHA_SPRITE *)lua_unboxpointer(L, -1));
        lua_pop(L, 1);
        i++;
    }
}
