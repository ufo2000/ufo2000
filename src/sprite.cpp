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
#include "sprite.h"

#define PIXEL_PTR              uint16*
#define OFFSET_PIXEL_PTR(p,x)  ((PIXEL_PTR) (p) + (x))

#define IS_MASK(c)             ((uint32)(c) == MASK_COLOR_16)

inline void draw_masked_lit_line16(uint16 *d, uint16 *s, uint32 n, int w)
{
    uint16 *limit = d + w;

    if (w & 1) {
        uint32 c1 = *s++;
        if (!IS_MASK(c1)) {
            c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
            c1 = (c1 - c1 * n / 32) & 0x7E0F81F;
            *d = c1 | (c1 >> 16);
        }
        d++;
    }

    while (d < limit) {
        uint32 c1 = s[0];
        if (!IS_MASK(c1)) {
            uint32 c2 = s[1];
            if (!IS_MASK(c2)) {
                c1 |= (c1 << 16);
                c2 |= (c2 << 16);
                c1 &= 0x7E0F81F;
                c2 &= 0x7E0F81F;
                c1 -= c1 * n / 32;
                c2 -= c2 * n / 32;
                c1 &= 0x7E0F81F;
                c2 &= 0x7E0F81F;
                c1 |= (c1 >> 16);
                c2 |= (c2 >> 16);
                d[0] = c1;
                d[1] = c2;
            } else {
                c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
                c1 = (c1 - c1 * n / 32) & 0x7E0F81F;
                d[0] = c1 | (c1 >> 16);
            }
        } else {
            uint32 c2 = s[1];
            if (!IS_MASK(c2)) {
                c2 = (c2 | (c2 << 16)) & 0x7E0F81F;
                c2 = (c2 - c2 * n / 32) & 0x7E0F81F;
                d[1] = c2 | (c2 >> 16);
            }
        }
        s += 2;
        d += 2;
    }
}

#define PROCESS_CLIPPING() \
    int w, h; \
    int dxbeg, dybeg; \
    int sxbeg, sybeg; \
    if (dst->clip) { \
        int tmp; \
        tmp = dst->cl - dx; \
        sxbeg = ((tmp < 0) ? 0 : tmp); \
        dxbeg = sxbeg + dx; \
        tmp = dst->cr - dx; \
        w = ((tmp > src->w) ? src->w : tmp) - sxbeg; \
        if (w <= 0) return; \
        tmp = dst->ct - dy; \
        sybeg = ((tmp < 0) ? 0 : tmp); \
        dybeg = sybeg + dy; \
        tmp = dst->cb - dy; \
        h = ((tmp > src->h) ? src->h : tmp) - sybeg; \
        if (h <= 0) return; \
    } else { \
       w = src->w; \
       h = src->h; \
       sxbeg = 0; \
       sybeg = 0; \
       dxbeg = dx; \
       dybeg = dy; \
    }

/**
 * Draws a darkened sprite, usable for night missions. It is optimized 
 * for 16bpp mode and is faster than allegro functions.
 *
 * @param dst         destination bitmap
 * @param src         source bitmap
 * @param dx          target x coordinate
 * @param dy          target y coordinate
 * @param brightness  brightness (0 - black image, 255 - original unmodified image)
 */
void draw_dark_sprite(BITMAP *dst, BITMAP *src, int dx, int dy, int brightness)
{
    if (brightness >= 255) {
        draw_sprite(dst, src, dx, dy);
        return;
    }

    if ((dst->id & (BMP_ID_VIDEO | BMP_ID_SYSTEM))
        || (src->vtable->color_depth != 16) || (dst->vtable->color_depth != 16)) {

        set_trans_blender(0, 0, 0, 0);
        draw_lit_sprite(dst, src, dx, dy, 255 - brightness);
        return;
    }

    // Process clipping
    PROCESS_CLIPPING();

    // Draw just a black sprite
    if (brightness <= 0) {
        for (int y = 0; y < h; y++) {
            uint16 *s = OFFSET_PIXEL_PTR(src->line[sybeg + y], sxbeg);
            uint16 *d = OFFSET_PIXEL_PTR(dst->line[dybeg + y], dxbeg);
            for (uint16 *limit = d + w; d < limit; d++)
                if (!IS_MASK(*s++))
                    *d = 0;
        }
        return;
    }

    // Draw a darkened sprite
    uint32 n = (255 - brightness) / 8;

    for (int y = 0; y < h; y++) {
        uint16 *s = OFFSET_PIXEL_PTR(src->line[sybeg + y], sxbeg);
        uint16 *d = OFFSET_PIXEL_PTR(dst->line[dybeg + y], dxbeg);
        draw_masked_lit_line16(d, s, n, w);
    }
}
