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

#include "stdafx.h"
#include "global.h"

typedef RLE_SPRITE ALPHA_SPRITE;

void draw_dark_sprite(BITMAP *dst, BITMAP *src, int dx, int dy, int brightness);

ALPHA_SPRITE *get_alpha_sprite(BITMAP *bmp, bool use_alpha = false);
void destroy_alpha_sprite(ALPHA_SPRITE *spr);
void draw_alpha_sprite(BITMAP *dst, ALPHA_SPRITE *src, int dx, int dy, int brightness = 255);

#endif
