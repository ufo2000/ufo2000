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

#include "sprite.h"
#include <string.h>

#define RLE_EOL_MARKER_16 MASK_COLOR_16

#define IS_MASK(c)             ((uint32)(c) == MASK_COLOR_16)

#undef INLINE
#ifdef __GNUC__
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

INLINE uint16 *mempcpy16_inline(uint16 *d, uint16 *s, int w)
{
    while (--w >= 0) *d++ = *s++;
    return d;
}
INLINE uint16 *mempset16_inline(uint16 *d, uint16 *s, int w)
{
    while (--w >= 0) *d++ = 0;
    return d;
}

INLINE uint16 *draw_masked_dark_line16(uint16 *d, const uint16 *s, int w, uint32 n)
{
    uint16 *limit = d + w;

    if (w & 1) {
        uint32 c1 = *s++;
        if (!IS_MASK(c1)) {
            c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
            c1 = (c1 * n / 32) & 0x7E0F81F;
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
                c1 = c1 * n / 32;
                c2 = c2 * n / 32;
                c1 &= 0x7E0F81F;
                c2 &= 0x7E0F81F;
                c1 |= (c1 >> 16);
                c2 |= (c2 >> 16);
                d[0] = c1;
                d[1] = c2;
            } else {
                c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
                c1 = (c1 * n / 32) & 0x7E0F81F;
                d[0] = c1 | (c1 >> 16);
            }
        } else {
            uint32 c2 = s[1];
            if (!IS_MASK(c2)) {
                c2 = (c2 | (c2 << 16)) & 0x7E0F81F;
                c2 = (c2 * n / 32) & 0x7E0F81F;
                d[1] = c2 | (c2 >> 16);
            }
        }
        s += 2;
        d += 2;
    }
    return d;
}

INLINE int16 *draw_normal_line16(int16 *d, const int16 *s, int w, const uint32 n)
{
    return (int16 *)mempcpy16_inline((uint16 *)d, (uint16 *)s, w);
}

INLINE int16 *draw_black_line16(int16 *d, const int16 *s, int w, const uint32 n)
{
    return (int16 *)mempset16_inline((uint16 *)d, 0, w);
}

INLINE int16 *draw_dark_line16(int16 *d, const int16 *s, int w, const uint32 n)
{
    int16 *limit = d + w;

    if (w & 1) {
        uint32 c1 = (uint16)(*s++);
        c1 = (c1 | (c1 << 16)) & 0x7E0F81F;
        c1 = (c1 * n / 32) & 0x7E0F81F;
        *d++ = c1 | (c1 >> 16);
    }

    while (d < limit) {
        uint32 c1 = (uint16)s[0];
        uint32 c2 = (uint16)s[1];
        c1 |= (c1 << 16);
        c2 |= (c2 << 16);
        c1 &= 0x7E0F81F;
        c2 &= 0x7E0F81F;
        c1 = c1 * n / 32;
        c2 = c2 * n / 32;
        c1 &= 0x7E0F81F;
        c2 &= 0x7E0F81F;
        c1 |= (c1 >> 16);
        c2 |= (c2 >> 16);
        d[0] = c1;
        d[1] = c2;
        s += 2;
        d += 2;
    }
    return d;
}

INLINE int16 *draw_alpha_dark_line16(int16 *d, const int32 *s, int w, const uint32 n)
{
    while (--w >= 0) {
        uint32 x = *s++;
        uint32 y = (uint16)*d;
        uint32 result = (x >> 5) & 0x3F;
        x = ((x & 0x7E0F81F) * n / 32) & 0x7E0F81F;
        y = (y | (y << 16)) & 0x7E0F81F;
        result = ((x - y) * result / 32 + y) & 0x7E0F81F;
        *d++ = (result | (result >> 16));
    }
    return d;
}

INLINE int16 *draw_alpha_normal_line16(int16 *d, const int32 *s, int w, const uint32 n)
{
    return draw_alpha_dark_line16(d, s, w, 32);
}

INLINE int16 *draw_alpha_black_line16(int16 *d, const int32 *s, int w, const uint32 n)
{
    return draw_alpha_dark_line16(d, s, w, 0);
}

INLINE int32 *draw_normal_line32(int32 *d, const int32 *s, int w, const uint32 n)
{
    while (--w >= 0) *d++ = *s++;
    return d;
}

INLINE int32 *draw_black_line32(int32 *d, const int32 *s, int w, const uint32 n)
{
    while (--w >= 0) *d++ = 0;
    return d;
}

INLINE int32 *draw_dark_line32(int32 *d, const int32 *s, int w, uint32 n)
{
    n = 256 - n;
    while (--w >= 0) {
#if 0
        uint32 x = *s++;
        uint32 xrb = (x & 0xFF00FF) * n / 256;
        uint32 xg = (x & 0xFF00) * n / 256;
        *d++ = (xrb & 0xFF00FF) | (xg & 0xFF00);
#else
        uint32 x = 0;
        uint32 y = *s++;
        uint32 res = ((x & 0xFF00FF) - (y & 0xFF00FF)) * n / 256 + y;
        y &= 0xFF00;
        x &= 0xFF00;
        uint32 g = (x - y) * n / 256 + y;

        res &= 0xFF00FF;
        g &= 0xFF00;

        *d++ = res | g;
#endif
    }
    return d;
}

INLINE int32 *draw_alpha_dark_line32(int32 *d, const int32 *s, int w, uint32 n)
{
    while (--w >= 0) {
        uint32 x = *s++;
        uint32 y = *d;

        uint32 xrb = ((x & 0xFF00FF) * n / 256) & 0xFF00FF;
        uint32 xg = ((x & 0xFF00) * n / 256) & 0xFF00;

        uint32 nn = geta32(x);
        if (nn) nn++;

        uint32 yrb = (y & 0xFF00FF);
        uint32 rb = (xrb - yrb) * nn / 256 + y; // yrb ???
        uint32 yg = (y & 0xFF00);
        uint32 g = (xg - yg) * nn / 256 + yg;

        *d++ = (rb & 0xFF00FF) | (g & 0xFF00);
    }
    return d;
}

INLINE int32 *draw_alpha_normal_line32(int32 *d, const int32 *s, int w, const uint32 n)
{
    return draw_alpha_dark_line32(d, s, w, 256);
}

INLINE int32 *draw_alpha_black_line32(int32 *d, const int32 *s, int w, const uint32 n)
{
    return draw_alpha_dark_line32(d, s, w, 0);
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

void draw_sprite(BITMAP *dst, BITMAP *src, int dx, int dy, unsigned int brightness)
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

    const uint16 **sl = (const uint16 **)&src->line[sybeg];
    uint16 **dl = (uint16 **)&dst->line[dybeg];

    // Draw just a black sprite
    if (brightness <= 0) {
        for (int y = 0; y < h; y++) {
            const uint16 *s = *sl++ + sxbeg;
            uint16 *d = *dl++ + dxbeg;
            for (uint16 *limit = d + w; d < limit; d++)
                if (!IS_MASK(*s++))
                    *d = 0;
        }
        return;
    }

    // Draw a darkened sprite
    uint32 n = (brightness + 7) / 8;

    for (int y = 0; y < h; y++) {
        const uint16 *s = *sl++ + sxbeg;
        uint16 *d = *dl++ + dxbeg;
        draw_masked_dark_line16(d, s, w, n);
        d += w;
        s += w;
    }
}

static unsigned long alpha_normal_blender16(unsigned long x, unsigned long y, unsigned long n)
{
    n = (x >> 5) & 0x3F;
    x = (x & 0x7E0F81F);
    y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;
    uint32 result = ((x - y) * n / 32 + y) & 0x7E0F81F;
    return ((result & 0xFFFF) | (result >> 16));
}

static unsigned long alpha_black_blender16(unsigned long x, unsigned long y, unsigned long n)
{
    n = (x >> 5) & 0x3F;
    y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;
    uint32 result = (y - y * n / 32) & 0x7E0F81F;
    return ((result & 0xFFFF) | (result >> 16));
}

static unsigned long alpha_dark_blender16(unsigned long x, unsigned long y, unsigned long n)
{
    uint32 result = (x >> 5) & 0x3F;
    x = ((x & 0x7E0F81F) * n / 32) & 0x7E0F81F;
    y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;
    result = ((x - y) * result / 32 + y) & 0x7E0F81F;
    return ((result & 0xFFFF) | (result >> 16));
}

unsigned long alpha_dark_blender32(unsigned long x, unsigned long y, unsigned long n)
{
    uint32 xrb = ((x & 0xFF00FF) * n / 256) & 0xFF00FF;
    uint32 xg = ((x & 0xFF00) * n / 256) & 0xFF00;

    n = geta32(x);
    if (!n++) return y;

    uint32 yrb = (y & 0xFF00FF);
    uint32 rb = (xrb - yrb) * n / 256 + yrb;
    uint32 yg = (y & 0xFF00);
    uint32 g = (xg - yg) * n / 256 + yg;

    return (rb & 0xFF00FF) | (g & 0xFF00);
}

//////////////////////////////////////////////////////////////////////////////

template<typename TD, typename TS, int eol_marker, TD *draw_line(TD *, const TS *, int, const uint32)> 
INLINE void draw_rle_sprite_internal(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, const uint32 brightness)
{
    // Process clipping
    PROCESS_CLIPPING();

    // Draw a darkened sprite
    const TS *s = (const TS *)src->dat;
    TD **dl = (TD **)&dst->line[dybeg];

    if (!clipflag) {
        while (--h >= 0) {
            TD *d = *dl++ + dxbeg;
            int c = *s++;
            while (c != eol_marker) {
                if (c > 0) {
                    d = draw_line(d, s, c, brightness);
                    s += c;
                } else {
                    d -= c;
                }
                c = *s++;
            }
        }
        return;
    }

    /* Clip top.  */
    while (--sybeg >= 0) {
       int c = *s++;
       while (c != eol_marker) {
           if (c > 0)
               s += c;
           c = *s++;
       }
    }

    /* Visible part.  */
    while (--h >= 0) {
        int x;
        TD *d = *dl++ + dxbeg;
        int c = *s++;

        /* Clip left.  */
        for (x = sxbeg; x > 0; ) {
            if (c == eol_marker)
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
            if (c == eol_marker)
                goto next_line;
            else if (c > 0) {
                /* Run of solid pixels.  */
                if ((x - c) >= 0) {
                    /* Fully visible.  */
                    x -= c;
                    d = draw_line(d, s, c, brightness);
                    s += c;
                } else {
                    /* Clipped on the right.  */
                    c -= x;
                    d = draw_line(d, s, x, brightness);
                    s += x;
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
        while (c != eol_marker) {
            if (c > 0)
                s += c;
            c = *s++;
        }
    next_line:;
    }
}

/**
 * Draw alpha sprite using only standard allegro functions
 */
void draw_alpha_sprite_allegro(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, unsigned int brightness)
{
    if (src->color_depth & 0x100) {
        src->color_depth &= ~0x100;
        ASSERT(dst->vtable->color_depth >= 16);
        if (dst->vtable->color_depth == 16) {
            if (brightness == 0) {
                set_blender_mode_ex(NULL, NULL, NULL, NULL, NULL, alpha_black_blender16, NULL, 0, 0, 0, 0);
                draw_trans_rle_sprite(dst, src, dx, dy);
            } else if (brightness >= 255) {
                set_blender_mode_ex(NULL, NULL, NULL, NULL, NULL, alpha_normal_blender16, NULL, 0, 0, 0, 0);
                draw_trans_rle_sprite(dst, src, dx, dy);
            } else {
                uint32 brightness16 = (brightness + 7) / 8;
                set_blender_mode_ex(NULL, NULL, NULL, NULL, NULL, alpha_dark_blender16, NULL, 0, 0, 0, brightness16);
                draw_trans_rle_sprite(dst, src, dx, dy);
            }
        } else {
            if (brightness >= 255) {
                set_alpha_blender();
                draw_trans_rle_sprite(dst, src, dx, dy);
            } else {
                uint32 brightness32 = brightness > 0 ? brightness + 1 : 0;
                set_blender_mode_ex(NULL, NULL, alpha_dark_blender32, alpha_dark_blender32, NULL, NULL, alpha_dark_blender32, 0, 0, 0, brightness32);
                draw_trans_rle_sprite(dst, src, dx, dy);
            }
        }
        src->color_depth |= 0x100;
    } else if (brightness >= 255) {
        draw_rle_sprite(dst, src, dx, dy);
    } else {
        set_trans_blender(0, 0, 0, 0);
        draw_lit_rle_sprite(dst, src, dx, dy, 255 - brightness);
    }
}

void draw_alpha_sprite(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, unsigned int brightness)
{
    if (dst->id & (BMP_ID_VIDEO | BMP_ID_SYSTEM)) {
        // non memory bitmap blitting, so fallback to standard allegro 
        // functions (with our custom blenders)
        draw_alpha_sprite_allegro(dst, src, dx, dy, brightness);
        return;
    }

    switch (dst->vtable->color_depth) {
        case 16:
            if (src->color_depth & 0x100) {
                if (brightness >= 255) {
                    draw_rle_sprite_internal<int16, int32, (int)(int32)MASK_COLOR_32, 
                        draw_alpha_normal_line16>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int16, int32, (int)(int32)MASK_COLOR_32,
                        draw_alpha_dark_line16>(dst, src, dx, dy, (brightness + 7) / 8);
                } else {
                    draw_rle_sprite_internal<int16, int32, (int)(int32)MASK_COLOR_32,
                        draw_alpha_black_line16>(dst, src, dx, dy, (uint32)0);
                }
            } else {
                if (brightness >= 255) {
                    draw_rle_sprite_internal<int16, int16, (int)(int16)RLE_EOL_MARKER_16,
                        draw_normal_line16>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int16, int16, (int)(int16)RLE_EOL_MARKER_16,
                        draw_dark_line16>(dst, src, dx, dy, (brightness + 7) / 8);
                } else {
                    draw_rle_sprite_internal<int16, int16, (int)(int16)RLE_EOL_MARKER_16,
                        draw_black_line16>(dst, src, dx, dy, (uint32)0);
                }
            }
            return;
        case 32:
            if (src->color_depth & 0x100) {
                if (brightness >= 255) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32, 
                        draw_alpha_normal_line32>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32,
                        draw_alpha_dark_line32>(dst, src, dx, dy, brightness);
                } else {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32,
                        draw_alpha_black_line32>(dst, src, dx, dy, (uint32)0);
                }
            } else {
                if (brightness >= 255) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32,
                        draw_normal_line32>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32,
                        draw_dark_line32>(dst, src, dx, dy, brightness);
                } else {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)MASK_COLOR_32,
                        draw_black_line32>(dst, src, dx, dy, (uint32)0);
                }
            }
            return;
        default:
            draw_alpha_sprite_allegro(dst, src, dx, dy, brightness);
            return;
    }
}

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp) 
{ 
    RLE_SPRITE *spr = get_rle_sprite(bmp);
    if (spr->color_depth == 16) {
        // allegro rle sprite optimization - find unnecessery skip runs,
	// replace MASK_COLOR_16 with proper EOL marker
        int i;
        int cnt = spr->size / 2;
        int16 *data = (int16 *)spr->dat;
        for (i = 0; i < cnt; i++) {
            if ((uint16)data[i] == MASK_COLOR_16) {
                data[i] = RLE_EOL_MARKER_16;
                continue;
            }
            if (data[i] < 0 && i + 1 < cnt && (uint16)data[i + 1] == MASK_COLOR_16) {
                memmove(&data[i], &data[i + 1], (cnt - (i + 1)) * 2);
                data[i] = RLE_EOL_MARKER_16;
                cnt--;
            } else if (data[i] > 0) {
                i += data[i];
            }
        }
    } else if (spr->color_depth == 32) {
        bool has_alpha = bitmap_color_depth(screen) != 32;
        for (int y = 0; y < bmp->h; y++) {
            const uint32 *line = (const uint32 *)bmp->line[y];
            for (int x = 0; x < bmp->w; x++) {
                uint32 a = line[x] & 0xFF000000;
                if (a != 0 && a != 0xFF000000) {
                    has_alpha = true;
                }
            }
        }

        if (has_alpha) {
            if (bitmap_color_depth(screen) == 16) {
                // Reorder RGBA data for more convenient 16-bit alpha blending
                int i;
                int cnt = spr->size / 4;
                int32 *data = (int32 *)spr->dat;
                for (i = 0; i < cnt; i++) {
                    if ((uint32)data[i] == MASK_COLOR_32) continue;
                    if (data[i] > 0) {
                        for (int j = data[i]; j > 0; j--) {
                            uint32 x = (uint32)data[++i];
                            uint32 n = (geta32(x) + 1) / 8;
                            x = makecol16(getr32(x), getg32(x), getb32(x));
                            x = (x | (x << 16)) & 0x7E0F81F;
                            data[i] = x | (n << 5);
                        }
                    }
                }
            }
            spr->color_depth |= 0x100;
        }
    }
    return spr; 
}

void destroy_alpha_sprite(ALPHA_SPRITE *spr)
{
    spr->color_depth &= ~0x100;
    destroy_rle_sprite(spr);
}
