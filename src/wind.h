/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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
#include <stdio.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winalleg.h>
#endif

#include "global.h"
#include "dirty.h"
#include "keys.h"
#include "font.h"

/**
 * Base class for objects that are displayed on screen with dirty rectangles 
 * support
 */
class VisualObject
{
	int m_switch_in_counter;
protected:
	int m_width, m_height;
public:
	VisualObject() : m_switch_in_counter(-1), m_width(0), m_height(0) { }
	virtual ~VisualObject()	{ }

//!	Draws complete object image
	virtual void redraw_full(BITMAP *bmp, int x, int y) = 0;
//!	Draws only changes since last draw (default implementation just calls draw_full)
	virtual void redraw_fast(BITMAP *bmp, int x, int y)
	{
		redraw_full(bmp, x, y);
	}
//!	Function that determines whether full redraw is needed and calls proper function
	void redraw(BITMAP *bmp, int x, int y)
	{
		if (m_switch_in_counter != g_switch_in_counter) {
			m_switch_in_counter = g_switch_in_counter;
			redraw_full(bmp, x, y);
		} else {
			redraw_fast(bmp, x, y);
		}
	}
//!	Gets the width of object
	int get_width() const { return m_width; }
//!	Gets the height of object
	int get_height() const { return m_height; }
//!	Sets internal variable so that the next redraw will be full redraw
	void set_full_redraw() { m_switch_in_counter = -1; }
};

/**
 * Visual object for console status line - bottom line in which the new
 * text is entered. Contains code for keyboard input and displaying.
 */
class ConsoleStatusLine : public VisualObject
{
	std::string m_text;
	FONT *m_font;
	int m_color;
	bool m_need_redraw;

	bool backspace();

public:
	ConsoleStatusLine(int width, FONT *font = g_small_font, int color = xcom1_color(1));
	virtual ~ConsoleStatusLine();

	void redraw_full(BITMAP *bmp, int x, int y);
	void redraw_fast(BITMAP *bmp, int x, int y);

	bool process_keyboard_input(int keycode, int scancode);

	const std::string &get_text() const { return m_text; }
	void set_text(const std::string & text) { m_text = text; }
};

/**
 * Visual object for rectangular area with chat console. It provides 
 * log window for previous text messages and also incorporates status
 * line for editing new text.
 */
class ConsoleWindow : public VisualObject
{
	std::vector<std::string>  m_lines_text;
	std::vector<int>          m_lines_color;
	ConsoleStatusLine        *m_status_line;
	FONT                     *m_font;
	bool                      m_need_redraw;
public:
	ConsoleWindow(int width, int height, FONT *font = font);
	virtual ~ConsoleWindow();

	void redraw_full(BITMAP *bmp, int x, int y);
	void redraw_fast(BITMAP *bmp, int x, int y);

	void print(const char *text, int color = xcom1_color(48));

	void vprintf(int color, const char *fmt, va_list arglist);
	void printf(int color, const char *fmt, ...);
	void printf(const char *fmt, ...);

	bool process_keyboard_input(int keycode, int scancode);

	const char *get_text();
	bool resize(int width, int height);
};

class Wind
{
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

	void writechr(BITMAP *_bmp, int _x, int _y, int c);
	void writestr(BITMAP *_bmp, int _x, int _y, const char *str);
	void writechr(BITMAP *_bmp, int _x, int _y, int c, int color);
	void writestr(BITMAP *_bmp, int _x, int _y, const char *str, int color);

	void showcursor();
	void hidecursor();
	void newline();
	void scroll();

	void info(int _x, int _y);
	
	void setfont(FONT *f);

public:
	Wind(BITMAP *_backscr, int x1, int y1, int x2, int y2, int col, FONT *f = NULL);
	~Wind();

	void redraw();
	void redraw_full();

	void printchr(int c);
	void printstr(const char *str);
	void printstr(const char *str, int color);
	void writestr(int x, int y, const char *str);

	void dump();
};

#endif
