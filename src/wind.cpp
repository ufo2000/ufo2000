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
#include "global.h"
#include <stdio.h>
#include <allegro.h>
#include <string.h>
#include "wind.h"
#include "pfxopen.h"

ConsoleStatusLine::ConsoleStatusLine(int width, FONT *font, int color)
{
	m_width = width;
	m_height = text_height(font);
	m_font = font;
	m_color = color;
}

ConsoleStatusLine::~ConsoleStatusLine()
{
}

/**
 * Function that erases last entered character from internal buffer
 *
 * @return true if character was really removed,
 *         false if there was no text in the internal buffer
 */
bool ConsoleStatusLine::backspace()
{
	int len = ustrlen(m_text.c_str());
	if (len == 0) return false;
	int offs = uoffset(m_text.c_str(), len - 1);
	m_text.erase(offs);
	return true;
}

void ConsoleStatusLine::redraw_full(BITMAP *bmp, int x, int y)
{
	acquire_bitmap(bmp);
	BITMAP * temp_bmp = create_bitmap(m_width, m_height);
	clear_to_color(temp_bmp, xcom1_color(11));
	text_mode(-1); textout(temp_bmp, m_font, m_text.c_str(), 0, 0, m_color);
	int len = text_length(m_font, m_text.c_str());
	if (len > 0)
		line(temp_bmp, len, 0, len, m_height - 1, xcom1_color(1));
	if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
	blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
	if (bmp == screen) unscare_mouse();
	destroy_bitmap(temp_bmp);
	release_bitmap(bmp);
}

bool ConsoleStatusLine::resize(int width, int height)
{
	if (height != -1) return false;
	m_width = width;
	return true;
}

/**
 * Function that processes keyboard input for the status line
 *
 * @param keycode allegro key code obtained by readkey() function
 * @return true if there is ready line inside of internal buffer (ENTER pressed)
 */
bool ConsoleStatusLine::process_keyboard_input(int keycode, int scancode)
{
	if (scancode == KEY_ENTER) {
		if (m_text == "") return false;
		return true;
	}

	if (scancode == KEY_BACKSPACE) {
		if (backspace()) set_full_redraw();
		return false;
	}
	if (keycode == 0) return false;

	char tmp[6];
	int size = usetc(tmp, keycode);
	m_text.append(tmp, tmp + size);
	if (text_length(m_font, m_text.c_str()) > m_width) backspace();
	set_full_redraw();
	return false;
}

ConsoleWindow::ConsoleWindow(int width, int height, FONT *font)
{
	m_width = width;
	m_height = height;
	m_font = font;
	m_status_line = new ConsoleStatusLine(width, font, xcom1_color(1));
	m_need_redraw = true;
}

ConsoleWindow::~ConsoleWindow()
{
	delete m_status_line;
}

void ConsoleWindow::redraw_full(BITMAP *bmp, int x, int y)
{
	acquire_bitmap(bmp);
	BITMAP *temp_bmp = create_bitmap(m_width, m_height);
	clear_to_color(temp_bmp, xcom1_color(15));
	int lines_to_show = (m_height - m_status_line->get_height()) / text_height(m_font);
	for (int i = m_lines_text.size() - 1, j = 1; i >= 0 && j <= lines_to_show; i--, j++) {
		text_mode(-1);
		textout(temp_bmp, m_font, m_lines_text[i].c_str(), 0,
			m_height - m_status_line->get_height() - j * text_height(m_font),
			m_lines_color[i]);
	}
	m_status_line->redraw_full(temp_bmp, 0, m_height - m_status_line->get_height());
	if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
	blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
	if (bmp == screen) unscare_mouse();
	destroy_bitmap(temp_bmp);
	m_need_redraw = false;
	release_bitmap(bmp);
}

void ConsoleWindow::redraw_fast(BITMAP *bmp, int x, int y)
{
	if (m_need_redraw)
		ConsoleWindow::redraw_full(bmp, x, y);
	else
		m_status_line->redraw(bmp, x, y + m_height - m_status_line->get_height());
}

/**
 * Function that inserts a new text line into console buffer
 *
 * @param text message to be printed
 */
void ConsoleWindow::print(const char *text, int color)
{
	std::string str = "";
	while (true) {
		if (*text == '\n') {
			m_lines_text.push_back(str);
			m_lines_color.push_back(color);
			str = "";
		} else if (*text == '\0') {
			if (!str.empty()) {
				m_lines_text.push_back(str);
				m_lines_color.push_back(color);
			}
			m_need_redraw = true;
			return;
		} else {
			str.append(text, 1);
		}
		text++;
	}
}

void ConsoleWindow::vprintf(int color, const char *format, va_list arglist)
{
    int stringsize = 4096;
    char *bigbuf;

#ifdef HAVE_VSNPRINTF
    static char smallbuf[4];
	stringsize = vsnprintf(smallbuf, sizeof(smallbuf), format, arglist);
    if (stringsize < 0)  /* Pre-glibc 2.1 behaviour */
        stringsize = 4096;
    bigbuf = new char[stringsize];
    vsnprintf(bigbuf, stringsize, format, arglist);
#else
	bigbuf = new char[stringsize];
	vsprintf(bigbuf, format, arglist); //WILL break if exceeds 4K.
#endif
	print(bigbuf, color);

    delete[] bigbuf;
}

void ConsoleWindow::printf(int color, const char *fmt, ...)
{
	va_list arglist;
	va_start(arglist, fmt);
	vprintf(color, fmt, arglist);
	va_end(arglist);
}

void ConsoleWindow::printf(const char *fmt, ...)
{
	va_list arglist;
	va_start(arglist, fmt);
	vprintf(xcom1_color(1), fmt, arglist);
	va_end(arglist);
}

/**
 * Function that processes all keyboard input of console
 *
 * @param keycode code from allegro readkey() function
 * @return        true if a new line has been just inserted into a buffer
 *                this line can be read by get_text() function
 */
bool ConsoleWindow::process_keyboard_input(int keycode, int scancode)
{
	if (scancode == KEY_ENTER) {
		if (m_status_line->get_text() == "") return false;
		printf("%s", m_status_line->get_text().c_str());
		m_status_line->set_text("");
		return true;
	}
	m_status_line->process_keyboard_input(keycode, scancode);
	return false;
}

/**
 * Function that returns last entered text string
 *
 * @return last entered text string
 */
const char *ConsoleWindow::get_text()
{
	assert(!m_lines_text.empty());
	return m_lines_text.back().c_str();
}

/**
 * Function that resizes console window
 *
 * @param width  new width of console window
 * @param height new height of console window
 */
bool ConsoleWindow::resize(int width, int height)
{
	m_status_line->resize(width, -1);
	m_width = width;
	m_height = height;
	m_need_redraw = true;
	return true;
}

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
	FILE *f = FOPEN_RTEMP("m_txt.dump", "wt");
	assert(f != NULL);
	for (int i = 0; i < m_txth; i++)
		fprintf(f, "%d:[%s]\n", i, m_txt[i]);
	fflush(f);
	fclose(f);
}

void Wind::info(int _x, int _y)
{
	text_mode(1);
	textprintf(screen, m_font, _x, _y, 0,
	           "m_txtbeg=%d, m_txtend=%d, m_txtvis=%d, m_curx=%d, m_cury=%d, m_w=%d, m_h=%d ",
	           m_txtbeg, m_txtend, m_txtvis, m_curx, m_cury, m_w, m_h);
}
