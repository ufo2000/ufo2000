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
#ifndef WIND_H
#define WIND_H
#include <allegro.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winalleg.h>
#endif
#include "dirty.h"

class Wind
{
protected:
	int m_x, m_y, m_w, m_h;
	int m_charw, m_charh;

	char **m_txt;
	int *m_txtcolor;
	int m_txtw, m_txth;
	int m_txtbeg, m_txtend;
	int m_txtvis;

	int m_scrw, m_scrh;
	int m_scrcol;
	int m_curx, m_cury;

	DirtyList *m_dirty, *m_dirty2;
	BITMAP *m_backscr, *m_screen;
	FONT *m_font;

public:
	Wind(BITMAP *_backscr, int x1, int y1, int x2, int y2, int col, FONT *f = NULL);
	~Wind();

	void showcursor();
	void hidecursor();
	void newline();
	void scroll();
	void redraw();
	void redraw_full();

	void printchr(int c);
	void printstr(const char *str);
	void printstr(const char *str, int color);

	void writestr(int x, int y, const char *str);

	void writechr(BITMAP *_bmp, int _x, int _y, int c);
	void writestr(BITMAP *_bmp, int _x, int _y, const char *str);
	void writechr(BITMAP *_bmp, int _x, int _y, int c, int color);
	void writestr(BITMAP *_bmp, int _x, int _y, const char *str, int color);

	void dump();
	void info(int _x, int _y);
	
	void setfont(FONT *f);
};

#endif
