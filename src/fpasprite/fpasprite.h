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
#ifndef FPASPRITE_H
#define FPASPRITE_H

#include <allegro.h>

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;

#if UINT_MAX == 0xFFFFFFFFUL
typedef signed int int32;
typedef unsigned int uint32;
#elif ULONG_MAX == 0xFFFFFFFFUL
typedef signed long int32;
typedef unsigned long uint32;
#else
#error Can not define int32 type
#endif

typedef RLE_SPRITE ALPHA_SPRITE;

/**
 * Draws a darkened sprite, usable for implementing lighting effects
 * (night missions). It is optimized for 16bpp mode.
 *
 * @param dst         destination bitmap
 * @param src         source bitmap
 * @param dx          target x coordinate
 * @param dy          target y coordinate
 * @param brightness  brightness (0 - black image, 255 - original unmodified image)
 */
void draw_sprite(BITMAP *dst, BITMAP *src, int dx, int dy, unsigned int brightness);

/**
 * Create a versatile sprite, which can support alpha transparency
 * and different brightness levels.
 */
ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp);

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp, int sx, int sy, int w, int h);

/**
 * Destroy alpha sprite.
 */
void destroy_alpha_sprite(ALPHA_SPRITE *spr);

/**
 * Draws a darkened sprite with alpha transparency support. It is optimized 
 * for 16bpp mode and is faster than allegro functions.
 *
 * @param dst         destination bitmap
 * @param src         source bitmap
 * @param dx          target x coordinate
 * @param dy          target y coordinate
 * @param brightness  brightness (0 - black image, 255 - original unmodified image)
 */
void draw_alpha_sprite(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, unsigned int brightness = 255);

#endif
