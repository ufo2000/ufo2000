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

#define IS_MASK(c)             ((uint32)(c) == MASK_COLOR_16)

static inline void draw_masked_lit_line16(uint16 *d, uint16 *s, uint32 n, int w)
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

static inline void draw_lit_line16(uint16 *&d, int16 *&s, uint32 n, int w)
{
    uint16 *limit = d + w;

    if (w & 1) {
        uint32 c1 = (uint16)(*s++);
        c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
        c1 = (c1 - c1 * n / 32) & 0x7E0F81F;
        *d++ = c1 | (c1 >> 16);
    }

    while (d < limit) {
        uint32 c1 = (uint16)s[0];
        uint32 c2 = (uint16)s[1];
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
        s += 2;
        d += 2;
    }
}

#define PROCESS_CLIPPING() \
    int clipflag = 0; \
    int w, h; \
    int dxbeg, dybeg; \
    int sxbeg, sybeg; \
    if (dst->clip) { \
        int tmp; \
        tmp = dst->cl - dx; \
        if (tmp <= 0) { \
            sxbeg = 0; \
            dxbeg = dx; \
            tmp = dst->cr - dx; \
            if (tmp >= src->w) { \
                w = src->w; \
            } else if (tmp <= 0) { \
                return; \
            } else { \
                w = tmp; \
                clipflag = 1; \
            } \
        } else { \
            sxbeg = tmp; \
            dxbeg = dst->cl; \
            clipflag = 1; \
            tmp = dst->cr - dx; \
            w = ((tmp > src->w) ? src->w : tmp) - sxbeg; \
            if (w <= 0) return; \
        } \
        tmp = dst->ct - dy; \
        if (tmp <= 0) { \
            sybeg = 0; \
            dybeg = dy; \
            tmp = dst->cb - dy; \
            if (tmp >= src->h) { \
                h = src->h; \
            } else if (tmp <= 0) { \
                return; \
            } else { \
                h = tmp; \
                clipflag = 1; \
            } \
        } else { \
            sybeg = tmp; \
            dybeg = dst->ct; \
            clipflag = 1; \
            tmp = dst->cb - dy; \
            h = ((tmp > src->h) ? src->h : tmp) - sybeg; \
            if (h <= 0) return; \
        } \
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

    uint16 **sl = (uint16 **)&src->line[sybeg];
    uint16 **dl = (uint16 **)&dst->line[dybeg];

    // Draw just a black sprite
    if (brightness <= 0) {
        for (int y = 0; y < h; y++) {
            uint16 *s = *sl++ + sxbeg;
            uint16 *d = *dl++ + dxbeg;
            for (uint16 *limit = d + w; d < limit; d++)
                if (!IS_MASK(*s++))
                    *d = 0;
        }
        return;
    }

    // Draw a darkened sprite
    uint32 n = (255 - brightness) / 8;

    for (int y = 0; y < h; y++) {
        uint16 *s = *sl++ + sxbeg;
        uint16 *d = *dl++ + dxbeg;
        draw_masked_lit_line16(d, s, n, w);
    }
}

#define RLE_IS_EOL(c)          ((uint16) (c) == MASK_COLOR_16)

/**
 * Draws a darkened rle sprite, usable for night missions. It is optimized 
 * for 16bpp mode and is faster than allegro functions.
 *
 * @param dst         destination bitmap
 * @param src         source bitmap
 * @param dx          target x coordinate
 * @param dy          target y coordinate
 * @param brightness  brightness (0 - black image, 255 - original unmodified image)
 */
void draw_alpha_sprite(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, int brightness)
{
    if ((dst->id & (BMP_ID_VIDEO | BMP_ID_SYSTEM))
        || (src->color_depth != 16) || (dst->vtable->color_depth != 16)) {

        if (src->color_depth & 0x100) {
            src->color_depth &= ~0x100;
            set_alpha_blender();
            draw_trans_rle_sprite(dst, src, dx, dy);
            src->color_depth |= 0x100;
        } else if (brightness >= 255) {
            draw_rle_sprite(dst, src, dx, dy);
        } else {
            set_trans_blender(0, 0, 0, 0);
            draw_lit_rle_sprite(dst, src, dx, dy, 255 - brightness);
        }
        return;
    }

    // Process clipping
    PROCESS_CLIPPING();

    // Draw a darkened sprite
    int16 *s = (int16 *)src->dat;
    uint16 **dl = (uint16 **)&dst->line[dybeg];

    if (!clipflag) {
        if (brightness == 0) {
            while (h--) {
                uint16 *d = *dl++ + dxbeg;
                int c = *s++;
                while (!RLE_IS_EOL(c)) {
                    if (c > 0) {
                        s += c;
                        while (c--) *d++ = 0;
                    } else {
                        d -= c;
                    }
                    c = *s++;
                }
            }
        } else if (brightness >= 255) {
            while (h--) {
                uint16 *d = *dl++ + dxbeg;
                int c = *s++;
                while (!RLE_IS_EOL(c)) {
                    if (c > 0) {
                        while (c--) *d++ = *s++;
                    } else {
                        d -= c;
                    }
                    c = *s++;
                }
            }
        } else {
            uint32 n = (255 - brightness) / 8;
            while (h--) {
                uint16 *d = *dl++ + dxbeg;
                int c = *s++;
                while (!RLE_IS_EOL(c)) {
                    if (c > 0)
                        draw_lit_line16(d, s, n, c);
                    else
                        d -= c;
                    c = *s++;
                }
            }
        }
        return;
    }

    uint32 n = (255 - brightness) / 8;

    /* Clip top.  */
    while (sybeg--) {
       int16 c = *s++;
       while (!RLE_IS_EOL(c)) {
           if (c > 0)
               s += c;
           c = *s++;
       }
    }

    /* Visible part.  */
    while (h--) {
        int x;
        uint16 *d = *dl++ + dxbeg;
        int c = *s++;

        /* Clip left.  */
        for (x = sxbeg; x > 0; ) {
            if (RLE_IS_EOL(c))
                goto next_line;
            else if (c > 0) {
                /* Run of solid pixels.  */
                if ((x - c) >= 0) {
                    /* Fully clipped.  */
                    x -= c;
                    s += c;
                } else {
                    /* Visible on the right.  */
                    c -= x;
                    s += x;
                    break;
                }
            } else {
                /* Run of transparent pixels.  */
                if ((x + c) >= 0) {
                    /* Fully clipped.  */
                    x += c;
                }
                else {
                    /* Visible on the right.  */
                    c += x;
                    break;
                }
            }
            c = *s++;
        }

        /* Visible part.  */
        for (x = w; x > 0; ) {
            if (RLE_IS_EOL(c))
                goto next_line;
            else if (c > 0) {
                /* Run of solid pixels.  */
                if ((x - c) >= 0) {
                    /* Fully visible.  */
                    x -= c;
                    draw_lit_line16(d, s, n, c);
                } else {
                    /* Clipped on the right.  */
                    c -= x;
                    draw_lit_line16(d, s, n, x);
                    break;
                }
            }
            else {
                /* Run of transparent pixels.  */
                x += c;
                d -= c;
            }
            c = *s++;
        }

        /* Clip right.  */
        while (!RLE_IS_EOL(c)) {
            if (c > 0)
                s += c;
            c = *s++;
        }

    next_line:;
    }
}

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp, bool use_alpha) 
{ 
    RLE_SPRITE *spr = get_rle_sprite(bmp);
    if (spr->color_depth == 16) {
/*
        // find unnecessery skip runs
        int i;
        int cnt = spr->size / 2;
        int16 *data = (int16 *)spr->dat;
        for (i = 0; i < cnt; i++) {
            if ((uint16)data[i] == MASK_COLOR_16) continue;
            if (data[i] < 0 && i + 1 < cnt && (uint16)data[i + 1] == MASK_COLOR_16) {
                memmove(&data[i], &data[i + 1], (cnt - (i + 1)) * 2);
                cnt--;
            } else if (data[i] > 0) {
                i += data[i];
            }
        }
        // find extra lines at the bottom of sprite
        int extra_lines = -1;
        for (i = cnt - 1; i >= 0 && (uint16)data[i] == MASK_COLOR_16; i--)
            extra_lines++;
        spr->h -= extra_lines;
*/
    } else if (use_alpha) {
        spr->color_depth |= 0x100;
    }
    return spr; 
}

void destroy_alpha_sprite(ALPHA_SPRITE *spr)
{
    spr->color_depth &= ~0x100;
    destroy_rle_sprite(spr);
}
