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
#ifndef PCK_H
#define PCK_H

#include "global.h"

BITMAP *pck_image(const char *filename, int index);

void free_pck_cache();

/**
 * Class for processing graphics in PCK-format
 */
class PCK
{
protected:
	std::vector<BITMAP *> m_bmp;

	static char m_fname[0x100];

	BITMAP *pckdat2bmp(const unsigned char *data, int size, int tftd_flag);

	bool m_tftd_flag;

public:
	int m_imgnum;

	PCK(const char *pckfname, int tftd_flag = 0);
	~PCK();

	BITMAP *get_image(int index)
	{
		if (index < 0 || index >= (int)m_bmp.size()) return NULL;
		return m_bmp[index];
	}

	int loadpck(const char *pckfname);
	void showpck(int num);
	void showpck(int num, int xx, int yy);
	static void showpck(BITMAP *img, int xx, int yy);
	void drawpck(int num, BITMAP *dest, int y);
	int add_image(BITMAP *bmp);
	void save_as_bmp(const char *fname);
};

#endif
