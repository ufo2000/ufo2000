/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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
#include "font.h"
#include "text.h"

static BITMAP *texture;
static BITMAP *buffer;

static double delta_phi = 3.5;

static double xres, yres;
static int xbmp, ybmp;

static void uv_to_xyz(int u, int v, float &x, float &y, float &z)
{
    float theta = (float)v * PI / ybmp;
    float phi = (float)u * 2. * PI / xbmp + delta_phi;

    y = yres - yres * cos(theta);
    z = yres * sin(theta) * cos(phi);
    x = xres + yres * sin(theta) * sin(phi);
}

static void draw_map_fragment(int x, int y, int size_x, int size_y)
{
    if (y + size_y >= ybmp) y--;
    if (y == 0) y++;

    V3D_f v[4];

    uv_to_xyz(x, y, v[0].x, v[0].y, v[0].z);
    uv_to_xyz(x + size_x, y, v[1].x, v[1].y, v[1].z);
    uv_to_xyz(x + size_x, y + size_y, v[2].x, v[2].y, v[2].z);
    uv_to_xyz(x, y + size_y, v[3].x, v[3].y, v[3].z);

    v[0].u = x; v[0].v = y;
    v[1].u = x + size_x; v[1].v = y;
    v[2].u = x + size_x; v[2].v = y + size_y;
    v[3].u = x; v[3].v = y + size_y;

    v[0].c = 100;
    v[1].c = 100;
    v[2].c = 100;
    v[3].c = 100;

    if (polygon_z_normal_f(&v[0], &v[1], &v[2]) > 0)
        quad3d_f(buffer, POLYTYPE_ATEX, texture, &v[0], &v[1], &v[2], &v[3]);
}

/**
 * Simple geoscape demo
 */
void geoscape()
{
    texture = load_bitmap(F("$(ufo2000)/arts/geoscape_texture.jpg"), NULL);
    if (!texture)
        return;
    if (texture->w % 32 != 0 || texture->h % 32 != 0) {
        destroy_bitmap(texture);
        return;
    }
    buffer = create_bitmap(screen->w, screen->h);
    
    xbmp = texture->w;
    ybmp = texture->h;
    xres = screen->w / 2;
    yres = screen->h / 2;
    
    int STEP = texture->w / 32;

    while (1)
    {
        clear_bitmap(buffer);

        for (int i = 0; i < texture->w; i += STEP)
            for (int j = 0; j < texture->h; j += STEP)
                draw_map_fragment(i, j, STEP, STEP);
                
        textout(buffer, g_small_font, _("Use left/right arrows to rotate the globe"), 0, 0, makecol(255, 255, 255));
        textout(buffer, g_small_font, _("Press ESC to exit"), 0, text_height(g_small_font), makecol(255, 255, 255));
    
        blit(buffer, screen, 0, 0, 0, 0, screen->w, screen->h);

        int scancode;
        ureadkey(&scancode);
        if (scancode == KEY_ESC) break;

        if (scancode == KEY_RIGHT)
            delta_phi -= PI / 64;
        if (scancode == KEY_LEFT)
            delta_phi += PI / 64;
    }
    
    destroy_bitmap(texture);
    destroy_bitmap(buffer);
}
