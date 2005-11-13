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
#ifndef SPRITE_H
#define SPRITE_H

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
