/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2004  ufo2000 development team

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
#ifndef MOUSE_H
#define MOUSE_H
#include "stdafx.h"
#include "global.h"

class MouseRange
{
friend void reset_mouse_range();
friend void reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn);
private:
    int m_x_min, m_y_min, m_x_max, m_y_max;
    static MouseRange *m_cur_mouse_range;
    MouseRange * m_prev_mouse_range;
    void set_mouse_range();
    void reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn);
public:
    MouseRange(int xminn, int yminn, int xmaxn, int ymaxn);
    ~MouseRange();
};

void reset_mouse_range();
void reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn);

inline int mouse_inside(int x1, int y1, int x2, int y2)
{
	return ((mouse_x >= x1) && (mouse_x <= x2) && (mouse_y >= y1) && (mouse_y <= y2));
}

inline void set_mouse_sens(int sens)
{
    set_mouse_speed(16 - sens, 16 - sens);
}
#endif
