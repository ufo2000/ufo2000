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

#include "minimap.h"
#include "map.h"

Minimap::Minimap(Map *map) : m_map(map)
{
	int i, j;
	m_height_10 = m_map->height * 10;
	m_width_10 = m_map->width * 10;

	m_minimap_state = new MINIMAP_STATE*[m_height_10];
	for (i = 0; i < m_height_10; i++) {
		m_minimap_state[i] = new MINIMAP_STATE[m_width_10];
		for (j = 0; j < m_width_10; j++) {
			m_minimap_state[i][j] = MINIMAP_STATE_UNEXPLORED;
		}
	}

	m_minimap_visible = m_map->create_bitmap_of_map();
	m_minimap_seen    = m_map->create_bitmap_of_map();
	m_minimap_unknown = create_bitmap(m_minimap_visible->w, m_minimap_visible->h);
	clear_to_color(m_minimap_unknown, xcom1_color(15));

	m_width  = m_minimap_visible->w;
	m_height = m_minimap_visible->h;

	for (i = 0; i < m_minimap_seen->w; i++)	{
		for (j = 0; j < m_minimap_seen->h; j++) {
			if ((i + j) % 2 == 1)
				putpixel(m_minimap_seen, i, j, xcom1_color(15));
		}
	}
}

Minimap::~Minimap()
{
	for (int i = 0; i < m_height_10; i++)
		delete [] m_minimap_state[i];
	delete [] m_minimap_state;

	destroy_bitmap(m_minimap_visible);
	destroy_bitmap(m_minimap_seen);
	destroy_bitmap(m_minimap_unknown);
}

#define S 2

void Minimap::redraw_minimap(BITMAP *bmp, int x, int y, int full_redraw_mode)
{
	acquire_bitmap(bmp);
	for (int row = 0; row < m_height_10; row++) {
		for (int col = 0; col < m_width_10; col++) {

			MINIMAP_STATE state;

			if (!m_map->seen(0, col, row)) {
				state = MINIMAP_STATE_UNEXPLORED;
			} else if (!m_map->visible(0, col, row)) {
				state = MINIMAP_STATE_SEEN;
			} else {
				if (m_map->man(0, col, row) != NULL) {
					state = platoon_local->belong(m_map->man(0, col, row)) ? 
						MINIMAP_STATE_FRIEND : MINIMAP_STATE_ENEMY;
				} else {
					state = MINIMAP_STATE_VISIBLE;
				}
			}

			if (full_redraw_mode || state != m_minimap_state[col][row]) {
				switch (state) {
					case MINIMAP_STATE_UNEXPLORED:
						blit(m_minimap_unknown, bmp, col * 4, row * 4, x + col * 4, y + row * 4, 4, 4);
						break;
					case MINIMAP_STATE_SEEN:
						blit(m_minimap_seen, bmp, col * 4, row * 4, x + col * 4, y + row * 4, 4, 4);
						break;
					case MINIMAP_STATE_VISIBLE:
						blit(m_minimap_visible, bmp, col * 4, row * 4, x + col * 4, y + row * 4, 4, 4);
						break;
					case MINIMAP_STATE_FRIEND:
						blit(m_minimap_visible, bmp, col * 4, row * 4, x + col * 4, y + row * 4, 4, 4);
						rectfill(bmp, x + col * 4, y + row * 4, x + col * 4 + S, y + row * 4 + S, xcom1_color(144));
						break;
					case MINIMAP_STATE_ENEMY:
						blit(m_minimap_visible, bmp, col * 4, row * 4, x + col * 4, y + row * 4, 4, 4);
						rectfill(bmp, x + col * 4, y + row * 4, x + col * 4 + S, y + row * 4 + S, xcom1_color(32));
						break;
				}
				m_minimap_state[col][row] = state;
			}
		}
	}
	release_bitmap(bmp);
}

void Minimap::redraw_full(BITMAP *bmp, int x, int y)
{
	redraw_minimap(bmp, x, y, 1);
}

void Minimap::redraw_fast(BITMAP *bmp, int x, int y)
{
	redraw_minimap(bmp, x, y, 0);
}

