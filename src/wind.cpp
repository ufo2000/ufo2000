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
#include <stdio.h>
#include <allegro.h>
#include <string.h>
#include "wind.h"
#include "pfxopen.h"


Wind::Wind(BITMAP *_backscr, int x1, int y1, int x2, int y2, int col, FONT *f)
{
	if (f != NULL) {
		m_font = f;
	} else {
		m_font = font;
	}
	
	m_backscr = _backscr;
	m_x = x1; m_y = y1;
	m_w = x2 - x1; m_h = y2 - y1;
	m_screen = create_bitmap(m_w, m_h); clear(m_screen);
	m_charh = text_height(m_font); 
	m_charw = text_length(m_font, "m"); //FIXME: we don't actually need to know this, do we?

	m_scrw = m_w / m_charw; m_scrh = m_h / m_charh;
	m_scrcol = col;
	m_curx = 0; m_cury = 0;

	m_txtw = 100; m_txth = 25;
	m_txtcolor = new int[m_txth];
	memset(m_txtcolor, 1, m_txth);
	m_txt = new char *[m_txth];
	for (int i = 0; i < m_txth; i++) {
		m_txt[i] = new char [m_txtw];
		m_txt[i][0] = 0;
	}
	m_txtbeg = 0; m_txtend = 0; m_txtvis = 0;

	m_dirty = new DirtyList(1000);
	m_dirty2 = new DirtyList(1000);
}

Wind::~Wind()
{
	delete m_dirty;
	delete m_dirty2;
	for (int i = 0; i < m_txth; i++)
		delete []m_txt[i];
	delete []m_txt;
	delete []m_txtcolor;
	destroy_bitmap(m_screen);
}

void Wind::setfont(FONT *f) 
{
	m_font = (f == NULL) ? font : f;
}	

void Wind::redraw()
{
	int i = 0;
	text_mode(-1);
	blit(m_backscr, m_screen, m_x, m_y, 0, 0, m_w + 10, m_h + 10);

	m_dirty2->copy(m_dirty);
	m_dirty->clear();
	do {
		int l = (m_txtvis + i) % m_txth;
		if ((l == m_txtend) && (m_curx == 0)) break;
		writestr(m_screen, 0, i * m_charh, m_txt[l], m_txtcolor[l]);
		m_dirty->add(0, i * m_charh, ustrlen(m_txt[l]) * m_charw, m_charh);
		i++;
	} while (i <= m_scrh);

	m_dirty2->add(m_dirty);
	m_dirty2->sort();
	m_dirty2->apply(m_screen, screen, m_x, m_y);
	m_dirty2->clear();
	showcursor();
}


void Wind::redraw_full()
{
	blit(m_backscr, screen, 0, 0, 0, 0, m_backscr->w, m_backscr->h);
	redraw();
}


void Wind::scroll()
{
	/*m_cury = 0;
	m_dirty->apply(m_backscr, screen);
	m_dirty->clear();*/
	m_txtvis++;
	if (m_txtvis == m_txth)
		m_txtvis = 0;
	redraw();
}

void Wind::newline()
{
	//m_dirty->add(m_x, m_y+m_cury*m_charh, m_curx*m_charw, m_charh);
	m_dirty->add(0, m_cury * m_charh, m_curx * m_charw, m_charh);
	m_curx = 0; m_cury++;

	m_txtend++;
	if (m_txtend == m_txth) {
		m_txtend = 0;
	}
	if (m_txtend == m_txtbeg) {
		m_txtbeg++;
		if (m_txtbeg == m_txth) {
			m_txtbeg = 0;
		}
	}

	if (m_cury > m_scrh) {
		m_cury = m_scrh;
		scroll();
	}
}

void Wind::printstr(const char *str)
{
	int c;
	while ((c = ugetxc(&str)) != 0) printchr(c);
}

void Wind::printstr(const char *str, int color)
{
	int oc = m_scrcol;
	m_scrcol = color;
	printstr(str);
	m_scrcol = oc;
}


void Wind::printchr(int c)
{
	hidecursor();
	//if ((c == 13) || (c == 10)) {
	if (c == '\r') {}
	else if (c == '\n') {
		m_txt[m_txtend][m_curx] = 0;
		newline();
	} else {
		text_mode( -1);
		textprintf(screen, m_font, m_x + m_curx * m_charw, m_y + m_cury * m_charh, m_scrcol, "%c", c);
		//writechr(m_screen, m_curx*m_charw, m_cury*m_charh, c);
		//m_dirty->add(m_x+m_curx*m_charw, m_y+m_cury*m_charh, m_charw, m_charh);
		m_txt[m_txtend][m_curx] = c;
		m_txtcolor[m_txtend] = m_scrcol;
		m_curx++;
		m_txt[m_txtend][m_curx] = 0;

		if (m_curx > m_scrw) {
			newline();
		}
	}
	showcursor();
}

#define CVN 0
#define SRPL 20

void Wind::writestr(int _cx, int _cy, const char *str)
{
	int l = ustrlen(str);
	int x = m_x + m_w - l * m_charw - _cx;
	int y = m_y - CVN - _cy;
	BITMAP *txt = create_bitmap(l * m_charw + SRPL, m_charh);
	blit(m_backscr, txt, x - SRPL, y, 0, 0, txt->w, txt->h);
	text_mode(-1);
	writestr(txt, SRPL, 0, str);
	blit(txt, screen, 0, 0, x - SRPL, y, txt->w, txt->h);
	destroy_bitmap(txt);
}

void Wind::writestr(BITMAP *_bmp, int _x, int _y, const char *str)
{
	int c;
	while ((c = ugetxc(&str)) != 0) {
		writechr(_bmp, _x, _y, c);
		_x += m_charw;
	}
}

void Wind::writechr(BITMAP *_bmp, int _x, int _y, int c)
{
	textprintf(_bmp, m_font, _x, _y, m_scrcol, "%c", c);
}

void Wind::writestr(BITMAP *_bmp, int _x, int _y, const char *str, int color)
{
	int c;
	while ((c = ugetxc(&str)) != 0) {
		writechr(_bmp, _x, _y, c, color);
		_x += m_charw;
	}
}

void Wind::writechr(BITMAP *_bmp, int _x, int _y, int c, int color)
{
	textprintf(_bmp, m_font, _x, _y, color, "%c", c);
}

void Wind::showcursor()
{
	rectfill(screen, m_x + m_curx * m_charw, m_y + m_cury * m_charh + m_charh - 2,
	         m_x + m_curx * m_charw + m_charw - 3, m_y + m_cury * m_charh + m_charh - 1, m_scrcol);
}

void Wind::hidecursor()
{
	blit(m_backscr, screen, m_x + m_curx * m_charw, m_y + m_cury * m_charh + m_charh - 2,
	     m_x + m_curx * m_charw, m_y + m_cury * m_charh + m_charh - 2, m_charw - 2, 2);
}

void Wind::dump()
{
	FILE * f = FOPEN_RTEMP("m_txt.dump", "wt");
	for (int i = 0; i < m_txth; i++)
		fprintf(f, "%d:[%s]\n", i, m_txt[i]);
	fflush(f);
}

void Wind::info(int _x, int _y)
{
	text_mode(1);
	textprintf(screen, m_font, _x, _y, 0,
	           "m_txtbeg=%d, m_txtend=%d, m_txtvis=%d, m_curx=%d, m_cury=%d, m_w=%d, m_h=%d ",
	           m_txtbeg, m_txtend, m_txtvis, m_curx, m_cury, m_w, m_h);
}
