/*
   Fast and Portable Allegro/Alpha Sprite Library
   Copyright (C) 2006 by Siarhei Siamashka (ssvb@users.sourceforge.net)

   The code presented in this file may be used in any environment it is
   acceptable to use Allegro library (http://alleg.sourceforge.net).

   This library is maintained as part of UFO2000 project and the latest
   version can be found in its source tree: http://ufo2000.sourceforge.net

   Please note that UFO2000 uses more restrictive GNU GPL license itself,
   so Allegro license applies only to this particular file as an exception.
*/
#include "fpasprite.h"
#include <string.h>

#define RLE_EOL_MARKER_16 MASK_COLOR_16
#define RLE_EOL_MARKER_32 MASK_COLOR_32

#define IS_MASK(c)             ((uint32)(c) == MASK_COLOR_16)

#undef INLINE
#if defined(__GNUC__) && defined(__OPTIMIZE__)
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
                    draw_rle_sprite_internal<int16, int32, (int)(int32)RLE_EOL_MARKER_32, 
                        draw_alpha_normal_line16>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int16, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_alpha_dark_line16>(dst, src, dx, dy, (brightness + 7) / 8);
                } else {
                    draw_rle_sprite_internal<int16, int32, (int)(int32)RLE_EOL_MARKER_32,
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
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32, 
                        draw_alpha_normal_line32>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_alpha_dark_line32>(dst, src, dx, dy, brightness);
                } else {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_alpha_black_line32>(dst, src, dx, dy, (uint32)0);
                }
            } else {
                if (brightness >= 255) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_normal_line32>(dst, src, dx, dy, (uint32)0);
                } else if (brightness != 0) {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_dark_line32>(dst, src, dx, dy, brightness);
                } else {
                    draw_rle_sprite_internal<int32, int32, (int)(int32)RLE_EOL_MARKER_32,
                        draw_black_line32>(dst, src, dx, dy, (uint32)0);
                }
            }
            return;
        default:
            draw_alpha_sprite_allegro(dst, src, dx, dy, brightness);
            return;
    }
}

static INLINE uint32 transform_16bpp_alpha(uint32 x)
{
    uint32 n = (geta32(x) + 1) / 8;
    x = makecol16(getr32(x), getg32(x), getb32(x));
    x = (x | (x << 16)) & 0x7E0F81F;
    return x | (n << 5);
}

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp) 
{ 
    return get_alpha_sprite(bmp, 0, 0, bmp->w, bmp->h);
}

ALPHA_SPRITE *get_alpha_sprite_fallback(BITMAP *bmp, int sx, int sy, int w, int h)
{
    BITMAP *tmp;
    if (bitmap_color_depth(bmp) == 32) {
        // This is quite ugly: we have an image with alpha channel here,
        // but allegro only respects mask color for get_rle_sprite() and blit
        // functions. This means we first need to convert all the transparent
        // pixels to mask color manually. Also semi-transparent pixels become
        // opaque.
        //
        // Fortunately we only get here in really exceptional cases, for
        // example if screen uses unsupported color format (neither 16bpp nor
        // 32bpp).
        tmp = create_bitmap(w, h);
        BITMAP *tmp2 = create_bitmap_ex(32, w, h);
        blit(bmp, tmp2, sx, sy, 0, 0, w, h);
        for (int y = 0; y < tmp2->h; y++) {
            uint32 *line = (uint32 *)tmp2->line[y];
            for (int x = 0; x < tmp2->w; x++) {
                if ((line[x] & 0xFF000000) == 0)
                    line[x] = MASK_COLOR_32;
            }
        }
        blit(tmp2, tmp, 0, 0, 0, 0, w, h);
        destroy_bitmap(tmp2);
    } else if (bitmap_color_depth(bmp) != bitmap_color_depth(screen)) {
        // get_rle_sprite() does not work well if called for a bitmap
        // having color format different from the one used for screen,
        // so a conversion (via temporary bitmap) is needed
        tmp = create_bitmap(w, h);
        blit(bmp, tmp, sx, sy, 0, 0, w, h);
    } else {
        // This part of code is used normally and it is quite fast
        tmp = create_sub_bitmap(bmp, sx, sy, w, h);
    }
    ALPHA_SPRITE *result = get_rle_sprite(tmp);
    destroy_bitmap(tmp);
    return result;
}

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp, int sx, int sy, int w, int h)
{
    if (bitmap_color_depth(bmp) != 32)
        return get_alpha_sprite_fallback(bmp, sx, sy, w, h);
    const int screen_color_depth = bitmap_color_depth(screen);
    if (screen_color_depth != 16 && screen_color_depth != 32) {
        // alpha sprites implement only 16bpp and 32bpp blenders at the moment
        return get_alpha_sprite_fallback(bmp, sx, sy, w, h);
    }

    // First pass, calculate size of resulting RLE sprite, also check whether
    // alpha channel is really required for this sprite

    int size = 0;
    const int run_limit = 127;
    int opaque_flag = 0;
    int alpha_flag = 0;

    for (int y = 0; y < h; y++) {
        const uint32 *line = (const uint32 *)bmp->line[y + sy] + sx;

        int old_x = 0;
        int x = 0;
        uint32 c;

        while (true) {
            while ((x < w) && (c = (line[x] & 0xFF000000))) { 
                if (c == 0xFF000000) opaque_flag = 1; else alpha_flag = 1;
                x++;
            }
            if (x > old_x) {
                if (x - old_x > run_limit) { x = old_x + run_limit; }
                size += 1 + (x - old_x);
                if (x == w) break;
                old_x = x;
            }
            while ((x < w) && ((line[x] & 0xFF000000) == 0)) { x++; }
            if (x > old_x) {
                if (x - old_x > run_limit) { x = old_x + run_limit; }
                if (x == w) break;
                size += 1;
                old_x = x;
            }
        }
        size += 1;
    }

    // Second pass, now we know how much memory is needed for a sprite and
    // what kind of alpha transparency is required

    if (alpha_flag || (opaque_flag && screen_color_depth >= 24)) {
        // Full alpha transparency is needed
        RLE_SPRITE *s = (RLE_SPRITE *)malloc(sizeof(RLE_SPRITE) + size * 4);
        uint32 *p = (uint32 *)s->dat;
        s->w = w;
        s->h = h;
        s->color_depth = 32 | (alpha_flag ? 0x100 : 0);
        s->size = size * 4;
        for (int y = 0; y < h; y++) {
            const uint32 *line = (const uint32 *)bmp->line[y + sy] + sx;

            int old_x = 0;
            int x = 0;

            while (true) {
                while ((x < w) && (line[x] & 0xFF000000)) { x++; }
                if (x > old_x) {
                    if (x - old_x > run_limit) { x = old_x + run_limit; }
                    *p++ = x - old_x;
                    while (old_x < x) {
                        *p++ = (screen_color_depth == 16) ? transform_16bpp_alpha(line[old_x++]) : line[old_x++];
                    }
                    if (x == w) break;
                }
                while ((x < w) && ((line[x] & 0xFF000000) == 0)) { x++; }
                if (x > old_x) {
                    if (x - old_x > run_limit) { x = old_x + run_limit; }
                    if (x == w) break;
                    *p++ = -(x - old_x);
                    old_x = x;
                }
            }
            *p++ = RLE_EOL_MARKER_32;
        }
        return s;
    } else if (opaque_flag && screen_color_depth == 16) {
        // Alpha channel is present, but it indicates that only fully opaque/transparent
        // pixels are present (1-bit alpha transparency), so we may use just ordinary RLE 
        // sprite for better performance and more efficient memory use
        RLE_SPRITE *s = (RLE_SPRITE *)malloc(sizeof(RLE_SPRITE) + size * 2);
        uint16 *p = (uint16 *)s->dat;
        int color_depth = bitmap_color_depth(screen);
        s->w = w; s->h = h; s->color_depth = color_depth; s->size = size * 2;
        for (int y = 0; y < h; y++) {
            const uint32 *line = (const uint32 *)bmp->line[y + sy] + sx; int old_x = 0; int x = 0;
            while (true) {
                while ((x < w) && (line[x] & 0xFF000000)) { x++; }
                if (x > old_x) {
                    if (x - old_x > run_limit) { x = old_x + run_limit; }
                    *p++ = x - old_x;
                    while (old_x < x) {
                        int c = line[old_x++];
                        *p++ = makecol16(getr32(c), getg32(c), getb32(c));
                    }
                    if (x == w) break;
                }
                while ((x < w) && ((line[x] & 0xFF000000) == 0)) { x++; }
                if (x > old_x) {
                    if (x - old_x > run_limit) { x = old_x + run_limit; }
                    if (x == w) break;
                    *p++ = -(x - old_x);
                    old_x = x;
                }
            }
            *p++ = bitmap_mask_color(screen);
        }
        return s;
    } else {
        return get_alpha_sprite_fallback(bmp, sx, sy, w, h);
    }
}

void destroy_alpha_sprite(ALPHA_SPRITE *spr)
{
    spr->color_depth &= ~0x100;
    destroy_rle_sprite(spr);
}
