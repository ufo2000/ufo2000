/*
 * glyph_to_sdl.c  -  part of Glyph Keeper SDL driver.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c


#ifdef GLYPH_TARGET_HAS_RECTFILL
#include "SDL/SDL_gfxPrimitives.h"
#endif



static GLYPH_REND* _gk_rend = 0;
static unsigned _gk_alpha_color;
static unsigned _gk_alpha;
static unsigned _gk_r;
static unsigned _gk_g;
static unsigned _gk_b;
static unsigned _gk_color;
static unsigned _gk_last_alpha_used = 257;
static unsigned _gk_alpha_gradient[256];

static SDL_Surface* _gk_bmp = 0;
static int _gk_driver_coldepth;
static int _gk_driver_bytes_per_pixel;
static int _gk_clip_left;    /* inclusive */
static int _gk_clip_top;     /* inclusive */
static int _gk_clip_right;   /* exclusive */
static int _gk_clip_bottom;  /* exclusive */
static int _gk_r_mask;
static int _gk_g_mask;
static int _gk_b_mask;
static int _gk_a_mask;

static int _gk_x_min;   /* inclusive */
static int _gk_y_min;   /* inclusive */
static int _gk_x_max;   /* exclusive */
static int _gk_y_max;   /* exclusive */



#ifdef GLYPH_TARGET_HAS_RECTFILL

static void _gk_driver_rectfill(SDL_Surface* const bmp,const int x1,const int y1,const int x2,const int y2,const int color)
{
    unsigned r = (color>>16)&0xFF;
    unsigned g = (color>>8)&0xFF;
    unsigned b = (color)&0xFF;

    boxColor(bmp,x1,y1,x2,y2,(r<<24)|(g<<16)|(b<<8)|255);

    if (x1 < _gk_x_min) _gk_x_min = x1;
    if (x2 < _gk_x_min) _gk_x_min = x2;

    if (x1 > _gk_x_max) _gk_x_max = x1;
    if (x2 > _gk_x_max) _gk_x_max = x2;

    if (y1 < _gk_y_min) _gk_y_min = y1;
    if (y2 < _gk_y_min) _gk_y_min = y2;

    if (y1 > _gk_y_max) _gk_y_max = y1;
    if (y2 > _gk_y_max) _gk_y_max = y2;
}

#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED

static void _gk_driver_rectfill_angled(SDL_Surface* const bmp,const int x1,const int y1,
    const int x2,const int y2,const int x3,const int y3,const int x4,const int y4,const int color)
{
    unsigned r = (color>>16)&0xFF;
    unsigned g = (color>>8)&0xFF;
    unsigned b = (color)&0xFF;
    short vx[4], vy[4];

    vx[0] = x1;
    vx[1] = x2;
    vx[2] = x3;
    vx[3] = x4;

    vy[0] = y1;
    vy[1] = y2;
    vy[2] = y3;
    vy[3] = y4;

    filledPolygonColor(bmp,vx,vy,4,(r<<24)|(g<<16)|(b<<8)|255);

    if (x1 < _gk_x_min) _gk_x_min = x1;
    if (x2 < _gk_x_min) _gk_x_min = x2;
    if (x3 < _gk_x_min) _gk_x_min = x3;
    if (x4 < _gk_x_min) _gk_x_min = x4;

    if (x1 > _gk_x_max) _gk_x_max = x1;
    if (x2 > _gk_x_max) _gk_x_max = x2;
    if (x3 > _gk_x_max) _gk_x_max = x3;
    if (x4 > _gk_x_max) _gk_x_max = x4;

    if (y1 < _gk_y_min) _gk_y_min = y1;
    if (y2 < _gk_y_min) _gk_y_min = y2;
    if (y3 < _gk_y_min) _gk_y_min = y3;
    if (y4 < _gk_y_min) _gk_y_min = y4;

    if (y1 > _gk_y_max) _gk_y_max = y1;
    if (y2 > _gk_y_max) _gk_y_max = y2;
    if (y3 > _gk_y_max) _gk_y_max = y3;
    if (y4 > _gk_y_max) _gk_y_max = y4;
}

#endif

#endif




static void (*_gk_driver_drawer)(const GLYPH* const glyph,const int x0,const int y0) = 0;


static void _gk_put_glyph_any(const GLYPH* const glyph,const int x0,const int y0)
{
    int bxend = x0 + glyph->width;
    int byend = y0 + glyph->height;
    if (byend > _gk_clip_bottom) byend = _gk_clip_bottom;

    CARE(glyph);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_put_glyph_any() begin\n");
#endif

    {
        int by = (y0 >= _gk_clip_top) ? y0 : _gk_clip_top;
        for (; by<byend; by++)
        {
            int bx = x0;
            unsigned char* bmppos = glyph->bmp + 1 + glyph->width*(by-y0);
            while (bx<bxend)
            {
                int a = _gk_alpha_gradient[*bmppos++];
                if (a>0)
                {
                    pixelRGBA(_gk_bmp,bx,by,_gk_r,_gk_g,_gk_b,a);
                }
                bx++;
            }
        }
    }

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_put_glyph_any() end\n");
#endif
}


static void _gk_prepare_to_draw(SDL_Surface* const new_bmp,GLYPH_REND* const new_rend)
{
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_prepare_to_draw() begin\n");
#endif

    _gk_bmp = 0;

    CARE(new_bmp);
    CARE(new_rend);

    if (!new_bmp->format) return;

    _gk_alpha_color = new_rend->text_alpha_color;
    _gk_alpha = _gk_alpha_color >> 24;
    if (!_gk_alpha) return;

    if (SDL_MUSTLOCK(new_bmp))
        if (SDL_LockSurface(new_bmp) < 0) return;

    _gk_rend = new_rend;
    _gk_bmp = new_bmp;
    _gk_driver_coldepth = new_bmp->format->BitsPerPixel;
    _gk_driver_bytes_per_pixel = new_bmp->format->BytesPerPixel;

    _gk_clip_left = _gk_bmp->clip_rect.x;
    _gk_clip_right = _gk_clip_left + _gk_bmp->clip_rect.w;
    _gk_clip_top = _gk_bmp->clip_rect.y;
    _gk_clip_bottom = _gk_clip_top + _gk_bmp->clip_rect.h;

    _gk_r_mask = _gk_bmp->format->Rmask;
    _gk_g_mask = _gk_bmp->format->Gmask;
    _gk_b_mask = _gk_bmp->format->Bmask;
    _gk_a_mask = _gk_bmp->format->Amask;

    _gk_x_min = _gk_bmp->w;
    _gk_y_min = _gk_bmp->h;
    _gk_x_max = 0;
    _gk_y_max = 0;

    _gk_r = (_gk_alpha_color>>16)&0xFF;
    _gk_g = (_gk_alpha_color>>8)&0xFF;
    _gk_b = (_gk_alpha_color)&0xFF;
    _gk_color = SDL_MapRGBA(_gk_bmp->format,_gk_r,_gk_g,_gk_b,255);

    if (_gk_last_alpha_used != _gk_alpha)
    {
        int a1 = (_gk_alpha<<16)/255;
        int i=0;
        for (; i<256; i++) { _gk_alpha_gradient[i] = (i*a1)>>16; }
        _gk_last_alpha_used = _gk_alpha;
    }

    _gk_driver_drawer = &_gk_put_glyph_any;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_prepare_to_draw() end\n");
#endif
}


static void _gk_done_drawing()
{
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_done_drawing() begin\n");
#endif
    if (_gk_bmp)
    {
        if (SDL_MUSTLOCK(_gk_bmp)) { SDL_UnlockSurface(_gk_bmp); }
        if (_gk_x_min < _gk_clip_left) _gk_x_min = _gk_clip_left;
        if (_gk_x_max > _gk_clip_right) _gk_x_max = _gk_clip_right;
        if (_gk_y_min < _gk_clip_top) _gk_y_min = _gk_clip_top;
        if (_gk_y_max > _gk_clip_bottom) _gk_y_max = _gk_clip_bottom;

        if ( (_gk_x_min < _gk_x_max) && (_gk_y_min < _gk_y_max) )
        {
            SDL_UpdateRect(_gk_bmp, _gk_x_min, _gk_y_min,
                _gk_x_max-_gk_x_min, _gk_y_max-_gk_y_min);
        }
        _gk_bmp = 0;
    }
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_done_drawing() end\n");
#endif
}


static void _gk_put_glyph(GLYPH* const glyph,const int x,const int y)
{
    CARE(glyph);
    /*CARE(_gk_bmp);*/

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"\n_gk_put_glyph() begin\n");
    if (glyph_log) fprintf(glyph_log,"   glyph U+%04X [%dx%d] --> (%d,%d)\n",
        glyph->unicode,glyph->width,glyph->height,x,y);
#endif

    if (glyph->bmp && _gk_bmp &&
        (x < _gk_clip_right) &&
        (y < _gk_clip_bottom) &&
        (x + glyph->width > _gk_clip_left) &&
        (y + glyph->height > _gk_clip_top) &&
        _gk_alpha)
    {
        CARE(glyph->bmpsize);
        _gk_driver_drawer(glyph,x,y);
        if (x < _gk_x_min) _gk_x_min = x;
        if (y < _gk_y_min) _gk_y_min = y;
        if (x + glyph->width > _gk_x_max) _gk_x_max = x + glyph->width;
        if (y + glyph->height > _gk_y_max) _gk_y_max = y + glyph->height;
    }

    if (!glyph->index) _gk_unload_glyph(glyph);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_put_glyph() end\n");
#endif
}


#endif  /* included_from_glyph_c */
