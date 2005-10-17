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

#include "stdafx.h"

#include "global.h"

#include "video.h"
#include "terrapck.h"
#include "colors.h"
#include "pck.h"

//! Global table which contains all the possible unique 
//! cell shapes, which can be used with current map, 
//! MCD::ShapeIndex is a reference in this table
static std::vector<ShapeInfo> shapes;

TerraPCK::TerraPCK(const char *mcd_name, int tftd_flag)
{
    add(mcd_name, tftd_flag);
}

TerraPCK::~TerraPCK()
{
    for (int i = 0; i < (int)m_mcd.size(); i++) {
        destroy_bitmap(m_mcd[i].ScangBitmap);
    }
}

/**
 * Load X-COM style tileset
 */
void TerraPCK::add_xcom_tileset(const char *mcd_name, int tftd_flag)
{
    char m_fname[512];
    int fh;
    
    // read information about displaying this tile on minimap
    if (tftd_flag) 
        fh = open(F("$(tftd)/geodata/scang.dat"), O_RDONLY | O_BINARY);
    else
        fh = open(F("$(xcom)/geodata/scang.dat"), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    long scang_size = filelength(fh);
    char *scang_data = new char[scang_size];
    read(fh, scang_data, scang_size);
    close(fh);
    char *scang_rgb_data = new char[scang_size * 3];
    for (int i = 0; i < scang_size; i++) {
        if (tftd_flag) {
            scang_rgb_data[i * 3 + 0] = getr(tftd_color(scang_data[i]));
            scang_rgb_data[i * 3 + 1] = getg(tftd_color(scang_data[i]));
            scang_rgb_data[i * 3 + 2] = getb(tftd_color(scang_data[i]));
        } else {
            scang_rgb_data[i * 3 + 0] = getr(xcom_color(scang_data[i]));
            scang_rgb_data[i * 3 + 1] = getg(xcom_color(scang_data[i]));
            scang_rgb_data[i * 3 + 2] = getb(xcom_color(scang_data[i]));
        }
    }
        
    // Read x-com terrains shape information
    fh = open(F("$(xcom)/geodata/loftemps.dat"), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    int loftemps_size = filelength(fh);
    ASSERT(loftemps_size % 2 == 0);
    char *loftemps_data = new char[loftemps_size];
    read(fh, loftemps_data, loftemps_size);
    close(fh);
    for (int i = 0; i < loftemps_size; i += 2)
        *(uint16 *)(loftemps_data + i) = intel_uint16(*(uint16 *)(loftemps_data + i));
    
    // get pck name
    strcpy(m_fname, mcd_name);
    strcpy(strrchr(m_fname, '.') + 1, "pck");
    std::string pck_name = m_fname;

    // load mcd file itself    
    strcpy(m_fname, mcd_name);
    strcpy(strrchr(m_fname, '.') + 1, "mcd");
    
    if (FLAGS & F_CONVERT_XCOM_DATA) {
        std::string filename;
        char *p = get_filename(mcd_name);
        while (*p != '.' && *p != 0) {
            filename += *p++;
        }
        std::string dir = F("$(home)/converted_xcom_data");
#ifdef LINUX
        mkdir(dir.c_str(), 0755);
#else
        mkdir(dir.c_str());
#endif
        std::string html_name = dir + "/" + filename + ".html";
        lua_pushstring(L, "convert_xcom_tileset");
        lua_gettable(L, LUA_GLOBALSINDEX);
        lua_pushstring(L, F(m_fname));
        lua_pushlstring(L, scang_rgb_data, scang_size * 3);
        lua_pushlstring(L, loftemps_data, loftemps_size);
        lua_safe_call(L, 3, 1);
        ASSERT(lua_isstring(L, -1));
        FILE *f = fopen(F(html_name.c_str()), "wt");
        fprintf(f, "%s", lua_tostring(L, -1));
        fclose(f);
        lua_pop(L, 1);
    }
    
    fh = open(F(m_fname), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    long fsize = filelength(fh);
    ASSERT(fsize % 62 == 0);
    long oldcount = m_mcd.size();
    long newcount = fsize / 62;
    m_mcd.resize(oldcount + newcount);

    for (int i = 0; i < newcount; i++) {
        ASSERT(offsetof(MCD, ufo2000_data_start_marker) == 62);
        read(fh, &m_mcd[oldcount + i], 62);
        m_mcd[oldcount + i].ScanG = intel_int16(m_mcd[oldcount + i].ScanG);
        if (m_mcd[oldcount + i].Alt_MCD)
            m_mcd[oldcount + i].Alt_MCD += oldcount;
        if (m_mcd[oldcount + i].Die_MCD)
            m_mcd[oldcount + i].Die_MCD += oldcount;

        ShapeInfo s;
        for (int j = 0; j < 12; j++)
            memcpy(&s.data[j], loftemps_data + m_mcd[oldcount + i].LOFT[j] * 32, 32);

        m_mcd[oldcount + i].ShapeIndex = get_shape_index(s);

        for (int j = 0; j < 8; j++) {
            BITMAP *bmp = pck_image_ex(tftd_flag, 32, 40, pck_name.c_str(), m_mcd[oldcount + i].Frame[j]);
            m_mcd[oldcount + i].FrameBitmap[j] = bmp;
        }
        m_mcd[oldcount + i].ScangBitmap = create_bitmap(4, 4);
        int mt = m_mcd[oldcount + i].ScanG + 35;
        ASSERT(scang_size >= mt * 16 + 16);
        for (int k = 0; k < 16; k++) {
            putpixel(m_mcd[oldcount + i].ScangBitmap, 3 - k / 4, k % 4, 
                tftd_flag ? 
                tftd_color(scang_data[mt * 16 + k]) :
                xcom_color(scang_data[mt * 16 + k]));
        }
    }
    close(fh);
    delete [] scang_data;
    delete [] scang_rgb_data;
    delete [] loftemps_data;
}

int TerraPCK::get_shape_index(const ShapeInfo &s)
{
    int shape_index = -1;
    for (int j = 0; j < (int)shapes.size(); j++) {
        if (memcmp(&shapes[j], &s, sizeof(ShapeInfo)) == 0) {
            shape_index = j;
            break;
        }
    }
    if (shape_index >= 0) {
        return shape_index;
    } else {
        shape_index = shapes.size();
        shapes.push_back(s);
        return shape_index;
    }
}

int TerraPCK::tileset_get_int(uint32 index, const char *tileset_name, const char *property_name, int defval)
{
    int stack_top = lua_gettop(L);
    // Enter 'TilesetsTable' table
    lua_pushstring(L, "TilesetsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [tileset_name] table
    lua_pushstring(L, tileset_name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter 'Tiles' table
    lua_pushstring(L, "Tiles");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter [index] table
    lua_pushnumber(L, index);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Get property value
    lua_pushstring(L, property_name);
    lua_gettable(L, -2);
    int result = defval;
    if (lua_isnumber(L, -1)) result = (int)lua_tonumber(L, -1);
    lua_settop(L, stack_top);
    return result;
}

/**
 * Add a new UFO2000 style tileset defined in lua file
 */
void TerraPCK::add_ufo2000_tileset(const char *tileset_name)
{
    // UFO2000 style tileset
    int stack_top = lua_gettop(L);
    // Enter 'TilesetsTable' table
    lua_pushstring(L, "TilesetsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [tileset_name] table
    lua_pushstring(L, tileset_name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter 'Tiles' table
    lua_pushstring(L, "NumberOfTiles");
    lua_gettable(L, -2);
    ASSERT(lua_isnumber(L, -1));
    int newcount = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    if (m_mcd.size() == 0) {
        m_mcd.resize(1);
        memset(&m_mcd[0], 0, sizeof(MCD));
        m_mcd[0].No_Floor = 1;

        ShapeInfo s;
        memset(&s, 0, sizeof(s));
        m_mcd[0].ShapeIndex = get_shape_index(s);
    }

    int oldcount = m_mcd.size();
    m_mcd.resize(oldcount + newcount);

    lua_pushstring(L, "Tiles");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));

    for (int i = 0; i < newcount; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, -2);

        memset(&m_mcd[oldcount + i], 0, sizeof(MCD));

        lua_pushstring(L, "Shape");
        lua_gettable(L, -2);
        ASSERT(lua_isstring(L, -1));
        const uint8 *shape_data = (const uint8 *)lua_tostring(L, -1);
        for (int j = 0; j < 384; j += 2)
            *(uint16 *)(shape_data + j) = intel_uint16(*(uint16 *)(shape_data + j));

        ShapeInfo s;
        memcpy(&s, shape_data, sizeof(s));
        lua_pop(L, 1);

        m_mcd[oldcount + i].ShapeIndex = get_shape_index(s);

        lua_pushstring(L, "IsometricImage");
        lua_gettable(L, -2);
        if (lua_islightuserdata(L, -1)) {
            BITMAP *bmp = (BITMAP *)lua_topointer(L, -1);
            for (int j = 0; j < 8; j++) {
                m_mcd[oldcount + i].FrameBitmap[j] = bmp;
            }
            lua_pop(L, 1);
        } else if (lua_istable(L, -1)) {
            for (int j = 0; j < 8; j++) {
                lua_pushnumber(L, j + 1);
                lua_gettable(L, -2);
                ASSERT(lua_islightuserdata(L, -1));
                BITMAP *bmp = (BITMAP *)lua_topointer(L, -1);
                m_mcd[oldcount + i].FrameBitmap[j] = bmp;
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        }

        m_mcd[oldcount + i].ScangBitmap = create_bitmap(4, 4);

        lua_pushstring(L, "MinimapImage");
        lua_gettable(L, -2);
        ASSERT(lua_isstring(L, -1));
        const uint8 *scang_data = (const uint8 *)lua_tostring(L, -1);

        for (int k = 0; k < 16; k++) {
            putpixel(m_mcd[oldcount + i].ScangBitmap, k % 4, k / 4,
                makecol(scang_data[k * 3 + 0], scang_data[k * 3 + 1], scang_data[k * 3 + 2]));
        }

        #define GET_LUA_TILESET_PROP(propname, defval) \
            m_mcd[oldcount + i].propname = tileset_get_int(i + 1, tileset_name, #propname, defval)

        GET_LUA_TILESET_PROP(UFO_Door, 0);
        GET_LUA_TILESET_PROP(No_Floor, 0);
        GET_LUA_TILESET_PROP(Big_Wall, 0);
        GET_LUA_TILESET_PROP(Gravlift, 0);
        GET_LUA_TILESET_PROP(Door, 0);
        GET_LUA_TILESET_PROP(TU_Walk, 255);
        GET_LUA_TILESET_PROP(TU_Fly, 255);
        GET_LUA_TILESET_PROP(TU_Slide, 255);
        GET_LUA_TILESET_PROP(Light_Source, 0);
        GET_LUA_TILESET_PROP(HE_Type, 0);
        GET_LUA_TILESET_PROP(HE_Strength, 0);
        GET_LUA_TILESET_PROP(Smoke_Blockage, 0);
        GET_LUA_TILESET_PROP(T_Level, 0);
        GET_LUA_TILESET_PROP(P_Level, 0);
        GET_LUA_TILESET_PROP(Stop_LOS, 0);
        GET_LUA_TILESET_PROP(Block_Fire, 0);
        GET_LUA_TILESET_PROP(Block_Smoke, 0);
        GET_LUA_TILESET_PROP(Footstep, 2);
        GET_LUA_TILESET_PROP(Target_Type, 0);
        GET_LUA_TILESET_PROP(Fuel, 1);
        GET_LUA_TILESET_PROP(HE_Block, 0);
        GET_LUA_TILESET_PROP(Light_Block, 0);
        GET_LUA_TILESET_PROP(Alt_MCD, 0);
        GET_LUA_TILESET_PROP(Die_MCD, 0);
        GET_LUA_TILESET_PROP(Armour, 0);

        if (m_mcd[oldcount + i].Alt_MCD)
            m_mcd[oldcount + i].Alt_MCD += oldcount - 1;
        if (m_mcd[oldcount + i].Die_MCD)
            m_mcd[oldcount + i].Die_MCD += oldcount - 1;

        lua_pop(L, 2);
    }

    lua_settop(L, stack_top);
}

/**
 * Load tileset
 */
void TerraPCK::add(const char *mcd_name, int tftd_flag)
{
    if (tftd_flag != 2) {
        add_xcom_tileset(mcd_name, tftd_flag);
    } else {
        add_ufo2000_tileset(mcd_name);
    }
}

/**
 * Check if voxel (z, x, y) inside of cell is solid and can stop bullet
 */
int TerraPCK::is_solid_voxel(int index, int z, int x, int y)
{
    return (shapes[m_mcd[index].ShapeIndex].data[z][15 - x] << y) & 0x8000;
}
