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
#include "global.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "video.h"
#include "terrapck.h"

TerraPCK::TerraPCK(const char *pckfname, int tftd_flag) : PCK(pckfname, tftd_flag)
{
	ASSERT(m_imgnum > 0);
	create_blackbmp(0, m_imgnum);
	strcpy(m_fname, pckfname);
	loadmcd(0, m_imgnum);
	memset(&empty, 0, sizeof(MCD));
	empty.No_Floor = 1;
}

TerraPCK::~TerraPCK()
{
	for (unsigned int i = 0; i < m_blackbmp.size(); i++) {
		ASSERT(m_blackbmp[i] != NULL);
		if (m_blackbmp[i] != NULL)
			destroy_bitmap(m_blackbmp[i]);
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
	strcpy(strrchr(m_fname, '.') + 1, "mcd");
	int fh = open(F(m_fname), O_RDONLY | O_BINARY);
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
		m_mcd[oldcount + i].pck_base = pck_base;
		m_mcd[oldcount + i].tftd_flag = m_tftd_flag;
	}
	close(fh);
}

void TerraPCK::create_blackbmp(int start, int size)
{
	m_blackbmp.resize(start + size);
	for (int num = start; num < start + size; num++) {
		m_blackbmp[num] = create_bitmap(32, 48);
		clear_to_color(m_blackbmp[num], xcom1_color(0));
		for (int i = 0; i < 32 * 48; i++)
			if (getpixel(m_bmp[num], i % 32, i / 32) != bitmap_mask_color(m_bmp[num]))
				putpixel(m_blackbmp[num], i % 32, i / 32, xcom1_color(15));
	}
}

void TerraPCK::showblackpck(int num, int xx, int yy)
{
	draw_sprite(screen2, m_blackbmp[num], xx, yy - 6);
}
