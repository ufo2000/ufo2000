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

#include "map.h"

#undef map

/**
 * Information about the shape of map cell. Each cell is represented
 * by 16x16x12 small elements called voxels. Each element is stored as
 * a single bit, so each map cell shape information takes 384 bytes.
 * When a bit descriping voxel is set, that part of map is solid
 * and can block bullets and other projectiles.
 */
struct ShapeInfo
{
    uint16 data[12][16];
};

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
    std::map<BITMAP *, RLE_SPRITE *>::iterator it = m_black_bmp.begin();
    while (it != m_black_bmp.end()) {
        destroy_rle_sprite(it->second);
        it++;
    }
}

RLE_SPRITE *TerraPCK::create_blackbmp(BITMAP *bmp)
{
    BITMAP *black_bmp = create_bitmap(bmp->w, bmp->h);
    clear_to_color(black_bmp, xcom_color(0));
    
    for (int i = 0; i < bmp->w; i++)
        for (int j = 0; j < bmp->h; j++)
            if (getpixel(bmp, i, j) != bitmap_mask_color(bmp))
                putpixel(black_bmp, i, j, COLOR_BLACK1);

    RLE_SPRITE *tmp = get_rle_sprite(black_bmp);
    destroy_bitmap(black_bmp);
    return tmp;
}

void TerraPCK::add(const char *mcd_name, int tftd_flag)
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

        int shape_index = -1;
        for (int j = 0; j < (int)shapes.size(); j++) {
            if (memcmp(&shapes[j], &s, sizeof(ShapeInfo)) == 0) {
                shape_index = j;
                break;
            }
        }
        if (shape_index >= 0) {
            m_mcd[oldcount + i].ShapeIndex = shape_index;
        } else {
            shape_index = shapes.size();
            shapes.push_back(s);
            m_mcd[oldcount + i].ShapeIndex = shape_index;
        }

        for (int j = 0; j < 8; j++) {
            BITMAP *bmp = pck_image_ex(tftd_flag, 32, 40, pck_name.c_str(), m_mcd[oldcount + i].Frame[j]);
            m_mcd[oldcount + i].FrameBitmap[j] = bmp;
            if (m_black_bmp.find(bmp) != m_black_bmp.end()) {
                m_mcd[oldcount + i].FrameBlackBitmap[j] = m_black_bmp[bmp];
            } else {
                m_mcd[oldcount + i].FrameBlackBitmap[j] = create_blackbmp(bmp);
                m_black_bmp[bmp] = m_mcd[oldcount + i].FrameBlackBitmap[j];
            }
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

/**
 * Check if voxel (z, x, y) inside of cell is solid and can stop bullet
 */
int TerraPCK::is_solid_voxel(int index, int z, int x, int y)
{
    return (shapes[m_mcd[index].ShapeIndex].data[z][15 - x] << y) & 0x8000;
}
