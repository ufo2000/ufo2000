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

#ifndef MINIMAP_H
#define MINIMAP_H

#include <allegro.h>
#include "wind.h"

class Minimap : public VisualObject
{
	int m_height_10, m_width_10;

	enum MINIMAP_STATE {
		MINIMAP_STATE_UNEXPLORED,
		MINIMAP_STATE_SEEN,
		MINIMAP_STATE_VISIBLE,
		MINIMAP_STATE_FRIEND,
		MINIMAP_STATE_ENEMY
	};

	MINIMAP_STATE **m_minimap_state;

	Map *m_map;
	BITMAP *m_minimap_visible;
	BITMAP *m_minimap_seen;
	BITMAP *m_minimap_unknown;

	void redraw_minimap(BITMAP *bmp, int x, int y, int full_redraw_mode);

public:
	Minimap(Map *map);
	virtual ~Minimap();

	void redraw_full(BITMAP *bmp, int x, int y);
	void redraw_fast(BITMAP *bmp, int x, int y);
};

class MinimapArea : public VisualObject
{
	Minimap *m_minimap;
	int      m_last_time_left;

	void show_time_left(BITMAP *bmp, int x, int y, int full_redraw_mode)
	{
		int time_left = g_time_left;

		if (!full_redraw_mode && m_last_time_left == time_left) return;

		rectfill(bmp, 
			x + m_width - m_minimap->get_width(),
			y + m_minimap->get_height(),
			x + m_width - 1,
			y + m_minimap->get_height() + text_height(font),
			xcom1_color(11));

		if (time_left > 0) {
			text_mode(-1);
			textprintf_centre(bmp, font, 
				x + m_width - m_minimap->get_width() / 2,
				y + m_minimap->get_height(), 
				xcom1_color(1),
				"Time left: %d", time_left); // $$$
		}

		m_last_time_left = time_left;
	}

public:
	MinimapArea(Map *map, int width, int height)
	{
		m_last_time_left = -1;
		m_minimap = new Minimap(map);
	}
	virtual ~MinimapArea()
	{
		delete m_minimap;
	}

	void redraw_full(BITMAP *bmp, int x, int y)
	{
		acquire_bitmap(bmp);
		BITMAP *temp_bmp = create_bitmap(m_width, m_height);
		clear_to_color(temp_bmp, xcom1_color(11));

		m_minimap->set_full_redraw();
		m_minimap->redraw(temp_bmp, m_width - m_minimap->get_width(), 0);
		show_time_left(temp_bmp, 0, 0, 1);

		blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
		destroy_bitmap(temp_bmp);
		release_bitmap(bmp);
	}

	void redraw_fast(BITMAP *bmp, int x, int y)
	{
		m_minimap->redraw(bmp, x + m_width - m_minimap->get_width(), y);
		show_time_left(bmp, x, y, 0);
	}

	bool resize(int width, int height)
	{
		m_width  = width;
		m_height = height;
		set_full_redraw();
		return true;
	}
};

#endif
