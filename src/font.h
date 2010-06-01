/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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
#ifndef FONT_H
#define FONT_H

extern FONT *g_small_font;
extern FONT *g_console_font;
extern FONT *large;

void set_console_font_size(int size);
int get_console_font_size();

void create_fonts();
void destroy_fonts();

void printsmall_x(BITMAP *bmp, int x, int y, int col, int value);
void printsmall_center_x(BITMAP *bmp, int x, int y, int col, int value);
void printsmall_center_back_x(BITMAP *bmp, int x, int y, int col, int back, int value);

#define printsmall(x, y, col, val) printsmall_x(screen2, (x), (y), (col), (val) )
#define printsmall_center(x, y, col, val) printsmall_center_x(screen2, (x), (y), (col), (val) )
#define printsmall_center_back(x, y, col, back, val) printsmall_center_back_x(screen2, (x), (y), (col), (back), (val) )

#endif
