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

TerraPCK::TerraPCK(const char *pckfname, int tftd_flag) : PCK(pckfname, tftd_flag)
{
	ASSERT(m_imgnum > 0);
	create_blackbmp(0, m_imgnum);
	strcpy(m_fname, pckfname);
	loadmcd(0, m_imgnum);
}

TerraPCK::~TerraPCK()
{
    int i;
    for (i = 0; i < (int)m_blackbmp.size(); i++) {
        ASSERT(m_blackbmp[i] != NULL);
        if (m_blackbmp[i] != NULL)
            destroy_bitmap(m_blackbmp[i]);
	}
    for (i = 0; i < (int)m_mcd.size(); i++) {
        destroy_bitmap(m_mcd[i].ScangBitmap);
    }
}

void TerraPCK::add(const char *pckfname, int tftd_flag)
{
	int num;
	static unsigned char m_tbb[0xFFFF];
	static uint16        m_tbs[0xFFF];

	m_tftd_flag = tftd_flag;
	strcpy(m_fname, pckfname);

	int fh = open(F(m_fname), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	int fsize = filelength(fh);
	ASSERT(fsize < 0xFFFF);
	int newlen = read(fh, m_tbb, fsize);
	ASSERT(newlen > 0);
	close(fh);

	strcpy(strrchr(m_fname, '.') + 1, "tab");
	fh = open(F(m_fname), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	fsize = filelength(fh);
	ASSERT(fsize < 0xFFF);
	int newnum = read(fh, (char *)m_tbs, fsize) >> 1;
	ASSERT(newnum > 0);
	close(fh);

	for (num = 0; num < newnum; num++)
		m_tbs[num] = intel_uint16(m_tbs[num]);

	m_tbs[newnum] = newlen;
	m_bmp.resize(m_imgnum + newnum);

	for (num = 0; num < newnum; num++)
		m_bmp[m_imgnum + num] = pckdat2bmp(&m_tbb[m_tbs[num]], m_tbs[num + 1] - m_tbs[num], tftd_flag);

	create_blackbmp(m_imgnum, newnum);
	loadmcd(m_imgnum, newnum);
	m_imgnum += newnum;
}

void TerraPCK::loadmcd(int pck_base, int size)
{
   	int fh;
    
    // read information about displaying this tile on minimap
    if (m_tftd_flag) 
        fh = open(F("$(tftd)/geodata/scang.dat"), O_RDONLY | O_BINARY);
    else
        fh = open(F("$(xcom)/geodata/scang.dat"), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    long scang_size = filelength(fh);
    char *scang_data = new char[scang_size];
    read(fh, scang_data, scang_size);
    close(fh);

    // load mcd file itself    
    strcpy(strrchr(m_fname, '.') + 1, "mcd");
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
        for (int j = 0; j < 8; j++) {
            m_mcd[oldcount + i].FrameBitmap[j] = m_bmp[pck_base + m_mcd[oldcount + i].Frame[j]];
            m_mcd[oldcount + i].FrameBlackBitmap[j] = m_blackbmp[pck_base + m_mcd[oldcount + i].Frame[j]];
        }
        m_mcd[oldcount + i].ScangBitmap = create_bitmap(4, 4);
        int mt = m_mcd[oldcount + i].ScanG + 35;
        ASSERT(scang_size >= mt * 16 + 16);
        for (int k = 0; k < 16; k++) {
            putpixel(m_mcd[oldcount + i].ScangBitmap, 3 - k / 4, k % 4, 
                m_tftd_flag ? 
                tftd_color(scang_data[mt * 16 + k]) :
                xcom_color(scang_data[mt * 16 + k]));
        }
    }
    close(fh);
    delete [] scang_data;
}

void TerraPCK::create_blackbmp(int start, int size)
{
	m_blackbmp.resize(start + size);
	for (int num = start; num < start + size; num++) {
		m_blackbmp[num] = create_bitmap(32, 48);
		clear_to_color(m_blackbmp[num], xcom1_color(0));
		for (int i = 0; i < 32 * 48; i++)
			if (getpixel(m_bmp[num], i % 32, i / 32) != bitmap_mask_color(m_bmp[num]))
				putpixel(m_blackbmp[num], i % 32, i / 32, COLOR_BLACK1);
	}
}
