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
#ifndef VIDEO_H
#define VIDEO_H
#include <allegro.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winalleg.h>
#endif
#include "font.h"

extern BITMAP *screen2;
extern int SCREEN2W, SCREEN2H;
extern BITMAP *mouser, *selector;

extern volatile int DRAWIT;

extern void drawit_timer();

void initvideo();
void closevideo();
void change_screen_mode();
void set_video_mode();
void reset_video();
void initpal(char *fname);
void setpal(RGB pal[PAL_SIZE], int pofs);

extern int xcom1_color_table[256];
extern int xcom1_menu_color_table[256];

/**
 * Function that converts color from xcom1 game palette to current
 * representation
 *
 * @param c  xcom1 color
 * @return   allegro color
 */
#ifdef USE_HICOLOR
inline int xcom1_color(int c)
{
	assert(c >= 0 && c < 256);
	return xcom1_color_table[c]; 
}
inline int xcom1_menu_color(int c)
{ 
	assert(c >= 0 && c < 256);
	return xcom1_menu_color_table[c]; 
}
inline int xcom1_darken_color(int c, int level)
{ 
	return makecol(
		getr(c) * (8 - level) / 8, 
		getg(c) * (8 - level) / 8, 
		getb(c) * (8 - level) / 8);
}
#else
inline int xcom1_color(int c) { return c; }
inline int xcom1_menu_color(int c) { return c; }
inline int xcom1_darken_color(int c, int level) { return c + level * 2; }
#endif

inline void spr_set(BITMAP *dest, int ofs, char val)
{
	*((char *)dest->dat + ofs) = val;
}

inline int spr_get(BITMAP *dest, int ofs)
{
	return * ((char *)dest->dat + ofs);
}

inline int mouse_inside(int x1, int y1, int x2, int y2)
{
	return ((mouse_x >= x1) && (mouse_x <= x2) && (mouse_y >= y1) && (mouse_y <= y2));
}

void savescreen();
void resize_screen2(int vw, int vh);
int askmenu(char *mess);

unsigned short crc16(char *data_p);

extern unsigned long FLAGS;
#define F_CLEARSEEN		0x00000001
#define F_SHOWROUTE		0x00000002
#define F_SHOWLOFCELL		0x00000004
#define F_SHOWLEVELS		0x00000008
#define F_FASTSTART		0x00000010
#define F_FULLSCREEN		0x00000020
#define F_RAWMESSAGES		0x00000040
#define F_SEL_ANY_MAN		0x00000080
#define F_SWITCHVIDEO		0x00000100
#define F_PLANNERDBG		0x00000200
#define F_ENDLESS_TU		0x00000400
#define F_SAFEVIDEO		0x00000800
#define F_SELECTENEMY		0x00001000
#define F_FILECHECK		0x00002000
#define F_LARGEFONT		0x00004000
#define F_SMALLFONT		0x00008000

#endif
