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
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "video.h"
#include "spk.h"
#include "pfxopen.h"

SPK::SPK()
{
	m_dat = NULL;
	m_datlen = 0;
};

SPK::SPK(const char *pckfname)
{
	m_dat = NULL;
	m_datlen = 0;

	load(pckfname);
}

SPK::~SPK()
{
	if (m_dat != NULL)
		delete [] m_dat;
}

void SPK::load(const char *fname)
{
	std::string fullname = F(fname);
	assert(exists(fullname.c_str()));
	m_datlen = file_size(fullname.c_str());
	if (m_dat != NULL) delete [] m_dat;
	m_dat = new unsigned char[m_datlen];
	PACKFILE *pfh = pack_fopen(fullname.c_str(), F_READ);
	m_datlen = pack_fread(m_dat, m_datlen, pfh);
	pack_fclose(pfh);
}

void SPK::show(BITMAP *_dest, int _x, int _y)
{
	BITMAP *bmp = spk2bmp();
	draw_sprite(_dest, bmp, _x, _y);
	destroy_bitmap(bmp);
}

void SPK::show_strech(BITMAP *_dest, int _x, int _y, int _w, int _h)
{
	BITMAP *bmp = spk2bmp();
	stretch_blit(bmp, _dest, 0, 0, 320, 200, 0, 0, _w, _h);
	destroy_bitmap(bmp);
}

void SPK::show_pck(BITMAP *_dest, int _x, int _y)
{
	BITMAP *bmp = spk2bmp();
	draw_sprite(_dest, bmp, _x, _y);
	destroy_bitmap(bmp);
}

//////////////////////////////////////////////////////////////////////////////
/// SPK format:                                                            ///
/// 4 bytes: header1                                                       ///
/// ? bytes: data1                                                         ///
/// ...                                                                    ///
/// 4 bytes: headerN                                                       ///
/// ? bytes: dataN                                                         ///
///                                                                        ///
/// FD FF  : end marker                                                    ///
///                                                                        ///
/// Header format:                                                         ///
/// 2 bytes: header ID (FF FF - skip block, FE FF - normal block)          ///
/// 2 bytes: size (value should be multiplied by 2 to get size)            ///
//////////////////////////////////////////////////////////////////////////////

BITMAP *SPK::spk2bmp()
{
	BITMAP *bmp = create_bitmap(320, 200);
	clear(bmp);
	long i = 0, j = 0;

//	Process .scr files
	if (m_datlen == 64000 && (*(unsigned short *)m_dat & 0xFFF0) != 0xFFF0) {
		long size = 64000;
		while (size--) {
			putpixel(bmp, j % 320, j / 320, xcom1_color(m_dat[i++]));
			j++;
		}
		return bmp;
	}

//	Process .spk files
	while (true) {
		assert(i + 2 <= m_datlen);
		switch (*(unsigned short *)(m_dat + i)) {
			case 0xFFFF: {
				assert(i + 4 <= m_datlen);
				long size = (long)(*(unsigned short *)(m_dat + i + 2)) * 2;
				i += 4;
				assert(j + size <= 64000);
				j += size;
				break;
			}
			case 0xFFFE: {
				assert(i + 4 <= m_datlen);
				long size = (long)(*(unsigned short *)(m_dat + i + 2)) * 2;
				i += 4;
				assert(i + size <= m_datlen && j + size <= 64000);
				while (size--) {
					putpixel(bmp, j % 320, j / 320, xcom1_color(m_dat[i++]));
					j++;
				}
				break;
			}
			case 0xFFFD:
				return bmp;
			default:
				assert(false);
		}
	}
}
