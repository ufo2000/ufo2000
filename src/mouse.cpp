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
MouseRange *MouseRange::m_cur_mouse_range = NULL;

/**
 * Call generic set_mouse_range procedure
 */
void MouseRange::set_mouse_range()
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::set_mouse_range()");
#endif
    ::set_mouse_range(m_x_min, m_y_min, m_x_max, m_y_max);
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
    ASSERT(m_cur_mouse_range == this);
    m_x_min = xminn; m_y_min = yminn; m_x_max = xmaxn; m_y_max = ymaxn;
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
    m_x_min = xminn; m_y_min = yminn; m_x_max = xmaxn; m_y_max = ymaxn;
    m_prev_mouse_range = m_cur_mouse_range;
    m_cur_mouse_range = this;
    set_mouse_range();
}

/**
 * Only the last created MouseRange instance can be deleted
 */
MouseRange::~MouseRange()
{
#ifdef MOUSE_DEBUG
    lua_message("MouseRange::~MouseRange()");
#endif
    ASSERT(m_cur_mouse_range == this);
    m_cur_mouse_range = m_prev_mouse_range;
    if (m_cur_mouse_range != NULL) m_cur_mouse_range->set_mouse_range();
}

/*
 * Set the current mouse range again
 */
void reset_mouse_range()
{
#ifdef MOUSE_DEBUG
    lua_message("reset_mouse_range()");
#endif
    MouseRange::m_cur_mouse_range->set_mouse_range();
}

/*
 * Change the current mouse range
 */
void reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)
{
#ifdef MOUSE_DEBUG
    lua_message("reset_mouse_range(int xminn, int yminn, int xmaxn, int ymaxn)");
#endif
    MouseRange::m_cur_mouse_range->reset_mouse_range(xminn, yminn, xmaxn, ymaxn);
}
