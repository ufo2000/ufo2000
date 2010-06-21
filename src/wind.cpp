/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

#include "stdafx.h"

#include "global.h"
#include "wind.h"
#include "music.h"

ConsoleStatusLine::ConsoleStatusLine(int width, FONT *font, int color)
{
    m_width = width;
    m_height = text_height(font);
    m_font = font;
    m_color = color;
    m_hide_when_empty = false;
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
    clear_to_color(temp_bmp, COLOR_GRAY11);
    text_mode(-1); textout(temp_bmp, m_font, m_text.c_str(), 0, 0, m_color);
    int len = text_length(m_font, m_text.c_str());
    if (len > 0)
        line(temp_bmp, len, 0, len, m_height - 1, COLOR_WHITE);
    if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
    blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
    if (bmp == screen) unscare_mouse();
    destroy_bitmap(temp_bmp);
    release_bitmap(bmp);
}

bool ConsoleStatusLine::resize(int width, int height)
{
    if (height != -1) return false;
    m_height = text_height(m_font);
    m_width = width;
    return true;
}

/**
 * Function that processes keyboard input for the status line
 *
 * @param keycode allegro key code obtained by readkey() function
 * @param scancode allegro key scancode obtained by readkey() function
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

ConsoleWindow::ConsoleWindow(int width, int height, BITMAP *bg, FONT *font)
{
    m_width = width;
    m_height = height;
    m_font = font;
    m_background_bmp = NULL;
    if (bg) {
        m_background_bmp = create_bitmap(bg->w, bg->h);
        blit(bg, m_background_bmp, 0, 0, 0, 0, bg->w, bg->h);
    }
    m_status_line = new ConsoleStatusLine(width, font, COLOR_WHITE);
    m_need_redraw = true;
}

ConsoleWindow::~ConsoleWindow()
{
    destroy_bitmap(m_background_bmp);
    delete m_status_line;
}

void ConsoleWindow::redraw_full(BITMAP *bmp, int x, int y)
{
    m_status_line->resize(m_width, -1); //probably should be moved somewhere else

    acquire_bitmap(bmp);
    BITMAP *temp_bmp = create_bitmap(m_width, m_height);
    if (m_background_bmp) {
        int x, y;
        for (y = 0; y < m_height; y += m_background_bmp->h)
            for (x = 0; x < m_width; x += m_background_bmp->w)
                blit(m_background_bmp, temp_bmp, 0, 0, x, y,
                    m_background_bmp->w, m_background_bmp->h);
    } else {
        clear_to_color(temp_bmp, COLOR_BLACK1);
    }
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
 * @param color text message color
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
    const int maxbufsize = 4096;
    char *buf = new char[maxbufsize];
    int result = vsnprintf(buf, maxbufsize, format, arglist);
    if (result < 0 || result >= maxbufsize) {
        print("!!! error in ConsoleWindow::vprintf() !!!\n", color);
    } else {
        print(buf, color);
    }
    delete [] buf;
}

// This or next function gets called for:
//  g_console->printf("%s", "xxxx");
//  g_console->printf( COLOR_RED, "! Error !");
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
    vprintf(COLOR_WHITE, fmt, arglist);
    va_end(arglist);
}


/**
 * Function that processes all keyboard input of console
 *
 * @param keycode code from allegro readkey() function
 * @param scancode scancode from allegro readkey() function
 * @return        true if a new line has been just inserted into a buffer
 *                this line can be read by get_text() function
 */
bool ConsoleWindow::process_keyboard_input(int keycode, int scancode)
{
    if (scancode == KEY_ENTER && ((key[KEY_ALT]) || (key[KEY_ALTGR]))) {
        // Alt-Enter - switch fullscreen/windowed mode
        change_screen_mode();
        // Workaround for stuck ESC key problem (the program refuses
        // to accept ESC key immediately after switching mode)
        remove_keyboard();
        install_keyboard();
        return false;
    }
    if (scancode == KEY_ENTER) {
        if (m_status_line->get_text() == "") return false;
        printf("%s", m_status_line->get_text().c_str());
        m_status_line->set_text("");
        return true;
    }
    if (key[KEY_LSHIFT]) {
        if (scancode == KEY_PLUS_PAD) {
            set_console_font_size(get_console_font_size() + 1);
            resize(m_width, m_height);
            return false;
        }
        if (scancode == KEY_MINUS_PAD) {
            set_console_font_size(get_console_font_size() - 1);
            resize(m_width, m_height);
            return false;
        }
    } else {
        if (scancode == KEY_PLUS_PAD) {
            FS_IncMusicVolume();
//            printf(COLOR_SYS_OK, _("Music Volume: %d"), vol );
            return false;
        }
        if (scancode == KEY_MINUS_PAD) {
            FS_DecMusicVolume();
//            printf(COLOR_SYS_OK, _("Music Volume: %d"), vol );
            return false;
        }
    }
    int old_height = m_status_line->get_height();
    m_status_line->process_keyboard_input(keycode, scancode);
    if (m_status_line->get_height() != old_height)
        set_full_redraw();
    return false;
}

/**
 * Function that returns last entered text string
 *
 * @return last entered text string
 */
const char *ConsoleWindow::get_text()
{
    ASSERT(!m_lines_text.empty());
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
