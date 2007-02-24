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

#include "stdafx.h"

#include "global.h"
#include "video.h"
#include "spk.h"

/**
 * Process sprite-graphics in SPK-format
 */
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

/**
 * Load SPK-graphics from file
 */
void SPK::load(const char *fname)
{
	std::string fullname = F(fname);
	if (!exists(fullname.c_str())) {
		fullname = F("$(ufo2000)/arts/empty.spk");
	}
	m_datlen = file_size(fullname.c_str());
	if (m_dat != NULL) delete [] m_dat;
	m_dat = new unsigned char[m_datlen];
	PACKFILE *pfh = pack_fopen(fullname.c_str(), F_READ);
	m_datlen = pack_fread(m_dat, m_datlen, pfh);
	pack_fclose(pfh);
}

/**
 * Draw SPK-graphics on screen
 */
void SPK::show(BITMAP *_dest, int _x, int _y)
{
	BITMAP *bmp = spk2bmp(0);
	draw_sprite(_dest, bmp, _x, _y);
	destroy_bitmap(bmp);
}
void SPK::show_pal2(BITMAP *_dest, int _x, int _y)
{
	BITMAP *bmp = spk2bmp(1);
	draw_sprite(_dest, bmp, _x, _y);
	destroy_bitmap(bmp);
}

void SPK::show_strech(BITMAP *_dest, int _x, int _y, int _w, int _h)
{
	BITMAP *bmp = spk2bmp(0);
	stretch_blit(bmp, _dest, 0, 0, 320, 200, 0, 0, _w, _h);
	destroy_bitmap(bmp);
}

// ?? same as SPK::show() 
void SPK::show_pck(BITMAP *_dest, int _x, int _y)
{
	BITMAP *bmp = spk2bmp(0);
	draw_sprite(_dest, bmp, _x, _y);
	destroy_bitmap(bmp);
}

//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
/// SPK format:                                                            ///
///                                                                        ///
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

// ?? Format of .scr - files 

/**
 * Convert graphics from SPK-format to bitmap
 * @arg 0 for default palette, 1 for research palette
 */
BITMAP *SPK::spk2bmp(int _pal)
{
	BITMAP *bmp = create_bitmap(320, 200);
	if (_pal == 1){
		clear_to_color(bmp, xcom1_research_color(0));
	}else{
		clear_to_color(bmp, xcom_color(0));
	}
	long i = 0, j = 0;

//	Process .scr files
	if (m_datlen == 64000 && (intel_uint16(*(uint16 *)m_dat) & 0xFFF0) != 0xFFF0) {
		long size = 64000;
		while (size--) {
			if (_pal == 1){
				putpixel(bmp, j % 320, j / 320, xcom1_research_color(m_dat[i++]));
			}else{
				putpixel(bmp, j % 320, j / 320, xcom1_color(m_dat[i++]));
			}
			j++;
		}
		return bmp;
	}

//	Process .spk files
	while (true) {
		if (!(i + 2 <= m_datlen)) return bmp;
		switch (intel_uint16(*(uint16 *)(m_dat + i))) {
			case 0xFFFF: {	// Skip-marker
				if (!(i + 4 <= m_datlen)) return bmp;
				long size = (long)intel_uint16((*(uint16 *)(m_dat + i + 2))) * 2;
				i += 4;
				if (!(j + size <= 64000)) return bmp;
				j += size;
				break;
			}
			case 0xFFFE: { // Marker for normal data-block
				if (!(i + 4 <= m_datlen)) 
					return bmp;
				long size = (long)intel_uint16((*(uint16 *)(m_dat + i + 2))) * 2;
				i += 4;
				if (!(i + size <= m_datlen && j + size <= 64000)) 
					return bmp;
				while (size--) {
					if (_pal == 1){
						putpixel(bmp, j % 320, j / 320, xcom1_research_color(m_dat[i++]));
					}else{
						putpixel(bmp, j % 320, j / 320, xcom1_color(m_dat[i++]));
					}
					j++;
				}
				break;
			}
			case 0xFFFD:	// End-marker
				return bmp;
			default:
				// error decoding SPK file
				return bmp;
		}
	}
}
