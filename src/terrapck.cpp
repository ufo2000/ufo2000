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
#include "pfxopen.h"

unsigned char TerraPCK::m_tbb[0xFFFF];
unsigned short TerraPCK::m_tbs[0xFFF];

TerraPCK::TerraPCK(char *pckfname) : PCK(pckfname)
{
	m_mcdnum = 0;
	m_mcdstart[m_mcdnum] = 0;
	assert(m_imgnum > 0);
	create_blackbmp(0, m_imgnum);
	strcpy(m_fname, pckfname);
	loadmcd(0, m_imgnum);
	memset(&empty, 0, sizeof(MCD));
	empty.No_Floor = 1;
}

TerraPCK::~TerraPCK()
{
	for (unsigned int i = 0; i < m_blackbmp.size(); i++) {
		assert(m_blackbmp[i] != NULL);
		if (m_blackbmp[i] != NULL)
			destroy_bitmap(m_blackbmp[i]);
	}
}

void TerraPCK::add(char *pckfname)
{
	strcpy(m_fname, pckfname);

	int fh = OPEN_ORIG(m_fname, O_RDONLY | O_BINARY);
	assert(fh != -1);
	int newlen = read(fh, m_tbb, 0xFFFF);
	assert(newlen > 0);
	close(fh);

	strcpy(strrchr(m_fname, '.') + 1, "tab");
	fh = OPEN_ORIG(m_fname, O_RDONLY | O_BINARY);
	assert(fh != -1);
	int newnum = read(fh, (char *)m_tbs, 0xFFF) >> 1;
	assert(newnum > 0);
	close(fh);
	m_tbs[newnum] = newlen;
#ifdef DEBUG
	cprintf(" newlen = %d newnum = %d", newlen, newnum);
#endif
	m_bmp.resize(m_imgnum + newnum);
	for (int num = 0; num < newnum; num++)
		m_bmp[m_imgnum + num] = pckdat2bmp(&m_tbb[m_tbs[num]], m_tbs[num + 1] - m_tbs[num]);

	create_blackbmp(m_imgnum, newnum);
	loadmcd(m_imgnum, newnum);
	m_imgnum += newnum;
}

void TerraPCK::loadmcd(int pck_base, int size)
{
	strcpy(strrchr(m_fname, '.') + 1, "mcd");
	int fh = OPEN_ORIG(m_fname, O_RDONLY | O_BINARY);
	assert(fh != -1);
	long fsize = filelength(fh);
	assert(fsize % 62 == 0);
	long oldcount = m_mcdstart[m_mcdnum];
	long newcount = fsize / 62;
	m_mcd.resize(oldcount + newcount);
	for (int i = 0; i < newcount; i++) {
		read(fh, &m_mcd[oldcount + i], 62);
		for (int j = 0; j < 8; j++)
			m_mcd[oldcount + i].Frame[j] += pck_base;
	}
	close(fh);
	m_mcdnum++;
	m_mcdstart[m_mcdnum] = oldcount + newcount;
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

int TerraPCK::mcdstart(int mcd_num)
{
	for (int i = 0; i < m_mcdnum; i++)
		if (mcd_num < m_mcdstart[i + 1])
			return m_mcdstart[i];
	return -1;
}
