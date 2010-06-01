/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2006  ufo2000 development team

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
#include "gui.h"
/*
 * -- SKIN INTERFACE RELATED --
 */
SkinInterface::SkinInterface(const char *screen_name) 
{
    m_screen = screen_name;
    m_background = get_bitmap("Background");
}
SkinInterface::~SkinInterface() 
{
    //No usage here yet
}
/*
 * Feature(const char *feature_name)
 * @function: Creates a feature sub-class and returns it.
 */ 
SkinFeature *SkinInterface::Feature(const char *feature_name) 
{
    SkinFeature *tmp_feature;
    tmp_feature = new SkinFeature(feature_name, m_screen);
    return tmp_feature;
}
/*
 * skin_get_bitmap(const char *bitmap_name)
 * @function: Extracts an image of the screen's root list.
 */ 
ALPHA_SPRITE *SkinInterface::get_bitmap(const char *bitmap_name)
{
    int stack_top = lua_gettop(L);
    // Enter 'GUITable' table
    lua_pushstring(L, "GuiTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [screen] table
    lua_pushstring(L, m_screen);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Get property value
    lua_pushstring(L, bitmap_name);
    lua_gettable(L, -2);
    ALPHA_SPRITE *result = NULL;
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, -1);
        lua_gettable(L, -2);
    }
    if (lua_isuserdata(L, -1)) {
        ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
        result = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
    }
    lua_settop(L, stack_top);
    return result;
}
/*
 * background()
 * @function: Returns the m_background alpha_sprite in the form of a default sized Bitmap
 */

BITMAP *SkinInterface::background() 
{
    BITMAP *tmp_bmp;
    /* Initialised at default background image resolution */
    tmp_bmp = create_bitmap(800, 600);
    if (tmp_bmp) {
        draw_alpha_sprite(tmp_bmp, m_background, 0, 0);
    }
    return tmp_bmp;
}

/*
 * -- SKIN FEATURE RELATED --
 */
SkinFeature::SkinFeature(const char *feature_name, const char *screen_name)
{
    m_feature = feature_name;
    m_fscreen = screen_name;
}
SkinFeature::~SkinFeature()
{
    //No usage here yet
}
/*
 * feat_get_color_int(const char *property_name, int index)
 * @function: Extracts an integer from a specific feature.
 * @param: property_name (Color, Color_active, Bg_color, etc.) index (1,2,3)-RGB
 */ 
int SkinFeature::feat_get_color_int(const char *property_name, int index)
{
int stack_top = lua_gettop(L);
    // Enter 'GUITable' table
    lua_pushstring(L, "GuiTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [Screen] table
    lua_pushstring(L, m_fscreen);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Features] table
    lua_pushstring(L, "Features");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [specific feature] table
    lua_pushstring(L, m_feature);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Property] list
    lua_pushstring(L, property_name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Get property value
    lua_pushnumber(L, index);
    lua_gettable(L, -2);
    int result = 0;
    if (lua_isnumber(L, -1)) result = (int)lua_tonumber(L, -1);
    lua_settop(L, stack_top);
    return result;
}
/*
 * feat_placement_exists(const char *property_name)
 * @function: Confirms that the selected property is defined.
 * @param: Property name (Top, Left, Right, Bottom, etc.)
 */ 
bool SkinFeature::feat_placement_exists(const char *property_name)
{
    int stack_top = lua_gettop(L);
    // Enter 'GUITable' table
    lua_pushstring(L, "GuiTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [Screen] table
    lua_pushstring(L, m_fscreen);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Features] table
    lua_pushstring(L, "Features");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [specific feature] table
    lua_pushstring(L, m_feature);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Placement] table
    lua_pushstring(L, "Placement");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Placement property] table
    lua_pushstring(L, property_name);
    lua_gettable(L, -2);
    if(lua_istable(L, -1)) {
        lua_settop(L, stack_top);
        return true;
    } else {
        lua_settop(L, stack_top);
        return false;
    }

}
/*
 * feat_get_placement_int(const char *property_name, int index)
 * @function: Extracts an integer from a specific feature.
 * @param: Property name (Top, Left, Right, Bottom, etc.) and index (1 is relative value, 2 is absolute value)
 */ 
int SkinFeature::feat_get_placement_int(const char *property_name, int index)
{
int stack_top = lua_gettop(L);
    // Enter 'GUITable' table
    lua_pushstring(L, "GuiTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [Screen] table
    lua_pushstring(L, m_fscreen);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Features] table
    lua_pushstring(L, "Features");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [specific feature] table
    lua_pushstring(L, m_feature);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Placement] table
    lua_pushstring(L, "Placement");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Placement property] table
    lua_pushstring(L, property_name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Get property value
    lua_pushnumber(L, index);
    lua_gettable(L, -2);
    int result = 0;
    if (lua_isnumber(L, -1)) result = (int)lua_tonumber(L, -1);
    lua_settop(L, stack_top);
    return result; 
}
/*
 * feat_get_bitmap(const char *bitmap_name)
 * @function: Extracts an image from a specific feature.
 */ 
ALPHA_SPRITE *SkinFeature::feat_get_bitmap(const char *bitmap_name)
{
    int stack_top = lua_gettop(L);
    // Enter 'GUITable' table
    lua_pushstring(L, "GuiTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [Screen] table
    lua_pushstring(L, m_fscreen);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [Features] table
    lua_pushstring(L, "Features");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [specific feature] table
    lua_pushstring(L, m_feature);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Get property value
    lua_pushstring(L, bitmap_name);
    lua_gettable(L, -2);
    ALPHA_SPRITE *result = NULL;
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, -1);
        lua_gettable(L, -2);
    }
    if (lua_islightuserdata(L, -1)) {
        ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
        result = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
    }
    lua_settop(L, stack_top);
    return result;
}

/*
int SkinInterface::skin_get_array_int(uint32 item_index, const char *property_name, int index)
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
*/
/*
std::string SkinInterface::skin_get_string(uint32 item_index, const char *property_name)
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
}*/



BITMAP *SkinFeature::bg_norm() 
{
    BITMAP *tmp_bmp;
    /* Initialised at default background image resolution */
    tmp_bmp = create_bitmap(800, 600);
    ALPHA_SPRITE *temp_a_bmp;
    temp_a_bmp = feat_get_bitmap("Bg_image");
    if (temp_a_bmp) {
        draw_alpha_sprite(tmp_bmp, temp_a_bmp, 0, 0);
    }
    return tmp_bmp;
}

BITMAP *SkinFeature::bg_over()
{
    BITMAP *tmp_bmp;
    /* Initialised at default background image resolution */
    tmp_bmp = create_bitmap(800, 600);
    ALPHA_SPRITE *temp_a_bmp;
    temp_a_bmp = feat_get_bitmap("Bg_image_active");
    ASSERT(temp_a_bmp);
    if (temp_a_bmp) {
        draw_alpha_sprite(tmp_bmp, temp_a_bmp, 0, 0);
    }
    return tmp_bmp;
}

int SkinFeature::get_x1() 
{
    int ret_val;
    int relative;
    int absolute;
    ret_val = 0;
    if (feat_placement_exists("Right")) {
        relative = feat_get_placement_int("Right", 1);
        absolute = feat_get_placement_int("Right", 2);
        ret_val = SCREEN_W - get_width() - relative * SCREEN_W / 10000 - absolute;
    }
    if (feat_placement_exists("Left")) {
        relative = feat_get_placement_int("Left", 1);
        absolute = feat_get_placement_int("Left", 2);
        ret_val = relative * SCREEN_W / 10000 + absolute;
    }
    return ret_val;
}

int SkinFeature::get_y1() 
{
    int ret_val;
    int relative;
    int absolute;
    ret_val = 0;
    if (feat_placement_exists("Bottom")) {
        relative = feat_get_placement_int("Bottom", 1);
        absolute = feat_get_placement_int("Bottom", 2);
        ret_val = SCREEN_H - get_height() - relative * SCREEN_H / 10000 - absolute;
    }
    if (feat_placement_exists("Top")) {
        relative = feat_get_placement_int("Top", 1);
        absolute = feat_get_placement_int("Top", 2);
        ret_val = relative * SCREEN_H / 10000 + absolute;
    }
    return ret_val;
}

int SkinFeature::get_x2() 
{
    int ret_val;
    ret_val = get_width() + get_x1();
    return ret_val;
}

int SkinFeature::get_y2() 
{
    int ret_val;
    ret_val = get_height() + get_y1();
    return ret_val;
}

int SkinFeature::get_width() 
{
    int ret_val;
    int relative;
    int absolute;
    ret_val = SCREEN_W;
    if (feat_placement_exists("Width")) {
        relative = feat_get_placement_int("Width", 1);
        absolute = feat_get_placement_int("Width", 2);
        ret_val = relative * SCREEN_W / 10000 + absolute;
    }
    return ret_val;
}

int SkinFeature::get_height() 
{
    int ret_val;
    int relative;
    int absolute;
    ret_val = SCREEN_H;
    if (feat_placement_exists("Height")) {
        relative = feat_get_placement_int("Height", 1);
        absolute = feat_get_placement_int("Height", 2);
        ret_val = relative * SCREEN_H / 10000 + absolute;
    }
    return ret_val;
}

int SkinFeature::get_padding() 
{
    int ret_val;
    int relative;
    int absolute;
    ret_val = 0;
    if (feat_placement_exists("Padding")) {
        relative = feat_get_placement_int("Padding", 1);
        absolute = feat_get_placement_int("Padding", 2);
        ret_val = relative * SCREEN_H / 10000 + absolute;
    }
    return ret_val;
}

int SkinFeature::get_pd_x1() 
{
    int ret_val;
    ret_val = get_padding() + get_x1();
    return ret_val;
}

int SkinFeature::get_pd_y1() 
{
    int ret_val;
    ret_val = get_padding() + get_y1();
    return ret_val;
}

int SkinFeature::get_pd_x2() 
{
    int ret_val;
    ret_val = get_x2() - get_padding();
    return ret_val;
}

int SkinFeature::get_pd_y2() 
{
    int ret_val;
    ret_val = get_y2() - get_padding();
    return ret_val;
}

int SkinFeature::get_pd_width() 
{
    int ret_val;
    ret_val =  get_width() - 2 * get_padding();
    return ret_val;
}

int SkinFeature::get_pd_height() 
{
    int ret_val;
    ret_val = get_height() - 2 * get_padding();
    return ret_val;
}

int SkinFeature::feat_make_color(const char *property_name)
{
    int r_val;
    int g_val;
    int b_val;
    r_val = feat_get_color_int(property_name, 1);
    g_val = feat_get_color_int(property_name, 2);
    b_val = feat_get_color_int(property_name, 3);
    return makecol(r_val,g_val,b_val);
}
