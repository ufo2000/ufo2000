/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
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
// Uncomment the next line to get some nice debug messages for this class
//#define MOUSE_DEBUG
#ifdef MOUSE_DEBUG
#include "stdafx.h"
#include "global.h"
#include "text.h"
#endif
#include "mouse.h"
MouseRange *MouseRange::cur_mouse_range = NULL;

/**
 * Call generic set_mouse_range procedure
 */
void MouseRange::set_mouse_range()
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::set_mouse_range()");
#endif
    ::set_mouse_range(x_min, y_min, x_max, y_max);
}

/**
 * Reinitialize the current mouse range
 */
void MouseRange::reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)");
#endif
    ASSERT(xminn <= xmaxn); ASSERT(yminn <= ymaxn);
    ASSERT(cur_mouse_range == this);
    x_min = xminn; y_min = yminn; x_max = xmaxn; y_max = ymaxn;
    set_mouse_range();
}

/**
 * Create a new MouseRange instance and mark it as last created
 */
MouseRange::MouseRange(int xminn, int yminn, int xmaxn, int ymaxn)
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::MouseRange(int xminn, int yminn, int xmaxn, int ymaxn)");
#endif
    ASSERT(xminn <= xmaxn); ASSERT(yminn <= ymaxn);
    x_min = xminn; y_min = yminn; x_max = xmaxn; y_max = ymaxn;
    set_mouse_range();
    prev_mouse_range = cur_mouse_range;
    cur_mouse_range = this;
}

/**
 * Only the last created MouseRange instance can be deleted
 */
MouseRange::~MouseRange()
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::~MouseRange()");
#endif
    ASSERT(cur_mouse_range == this);
    cur_mouse_range = prev_mouse_range;
    if (cur_mouse_range != NULL) cur_mouse_range->set_mouse_range();
}

void reset_mouse_range()
{
#ifdef MOUSE_DEBUG
    lua_message("reset_mouse_range()");
#endif
    MouseRange::cur_mouse_range->set_mouse_range();
}

void reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)
{
#ifdef MOUSE_DEBUG
    lua_message("reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)");
#endif
    MouseRange::cur_mouse_range->reset_mouse_range(xminn, yminn, xmaxn, ymaxn);
}
