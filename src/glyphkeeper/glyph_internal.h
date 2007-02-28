/*
 * glyph_internal.h  -  Glyph Keeper internal header file.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


#ifndef glyph_internal_h_included
#define glyph_internal_h_included


#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_GLYPH_H
#include FT_SIZES_H
#include FT_SYNTHESIS_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

#include "glyph.h"
#include "glyph_structs.h"


#define GK_PI     3.14159265358979323846
#define GK_SQRT2  1.41421356237309504880

#define GK_MIN(a,b)  ((a)<(b)?(a):(b))
#define GK_MAX(a,b)  ((a)>(b)?(a):(b))


/*
 * This macro should be used to check internal consistensy only.
 * All things coming from user through API should be checked in some other way.
 * (Because this checking can be turned on/off by GLYPH_SAFE macro)
 */
#ifdef GLYPH_SAFE
#define CARE(thing)  \
{ if (!(thing)) { _gk_msg("Error: some really bad thing happened: %s, line %d\n",__FILE__,__LINE__); exit(1); } }
#else
#define CARE(thing)  {}
#endif


#define GLYPH_UNCOMPRESSED  0

#ifdef GLYPH_TARGET_KNOWS_RLEAA
#define GLYPH_RLEAA         1
#endif

#define GLYPH_MONO          2

#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
#define GLYPH_MONO_BITPACK  2
#endif

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
#define GLYPH_MONO_RLE7     3
#endif


/*
 * Functions that must be defined by the target driver code.
 */
#ifdef GLYPH_TARGET_HAS_RECTFILL
static void _gk_driver_rectfill(GLYPH_TARGET_SURFACE* const bmp,
    const int x1,const int y1,const int x2,const int y2,const int color);
#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED
static void _gk_driver_rectfill_angled(GLYPH_TARGET_SURFACE* const bmp,const int x1,const int y1,
    const int x2,const int y2,const int x3,const int y3,const int x4,const int y4,const int color);
#endif  /* GLYPH_TARGET_HAS_RECTFILL_ANGLED */
#endif  /* GLYPH_TARGET_HAS_RECTFILL */

static void _gk_prepare_to_draw(GLYPH_TARGET_SURFACE* const new_bmp,GLYPH_REND* const new_rend);
static void _gk_put_glyph(GLYPH* const glyph,const int x,const int y);
static void _gk_done_drawing();
/*
 * End of target driver's functions.
 */


static void _gk_unload_glyph(GLYPH* const glyph);

static GLYPH* _gk_rend_render(GLYPH_REND* const rend,const unsigned unicode);

static void _gk_msg(const char* const format,...);


#endif
