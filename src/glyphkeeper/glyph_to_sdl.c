/*
 * glyph_to_sdl.c  -  part of Glyph Keeper SDL driver.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * _gk_sdl_putpixel() and _gk_sdl_putpixel_alpha() routines
 * use code from SDL_gfx library, written by Andreas Schiffler.
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


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


#include "glyph_to_sdl_rect.c"


static void (*_gk_driver_drawer)(const GLYPH* const glyph,const int x0,const int y0) = 0;


void _gk_sdl_putpixel_alpha(int x,int y,int alpha)
{
  unsigned R, G, B, A = 0;
  unsigned char* p;

  if (x<_gk_clip_left || x>=_gk_clip_right) return;
  if (y<_gk_clip_top || y>=_gk_clip_bottom) return;

  p = (unsigned char*) _gk_bmp->pixels + y * _gk_bmp->pitch + x * _gk_driver_bytes_per_pixel;

  switch (_gk_driver_bytes_per_pixel)
  {
    case 1:
    {
      if (alpha == 255) { *p = _gk_color; }
      else
      {
        Uint8 dR = _gk_bmp->format->palette->colors[*p].r;
        Uint8 dG = _gk_bmp->format->palette->colors[*p].g;
        Uint8 dB = _gk_bmp->format->palette->colors[*p].b;
        Uint8 sR = _gk_bmp->format->palette->colors[_gk_color].r;
        Uint8 sG = _gk_bmp->format->palette->colors[_gk_color].g;
        Uint8 sB = _gk_bmp->format->palette->colors[_gk_color].b;
        dR = dR + ((sR - dR) * alpha >> 8);
        dG = dG + ((sG - dG) * alpha >> 8);
        dB = dB + ((sB - dB) * alpha >> 8);
        *p = SDL_MapRGB(_gk_bmp->format, dR, dG, dB);
      }
      break;
    }

    case 2:
    {
      if (alpha == 255) { *(unsigned short*)p = _gk_color; }
      else
      {
        unsigned dc = *(unsigned short*)p;
        R = ((dc & _gk_r_mask) + (((_gk_color & _gk_r_mask) - (dc & _gk_r_mask)) * alpha >> 8)) & _gk_r_mask;
        G = ((dc & _gk_g_mask) + (((_gk_color & _gk_g_mask) - (dc & _gk_g_mask)) * alpha >> 8)) & _gk_g_mask;
        B = ((dc & _gk_b_mask) + (((_gk_color & _gk_b_mask) - (dc & _gk_b_mask)) * alpha >> 8)) & _gk_b_mask;
        if (_gk_a_mask) A = ((dc & _gk_a_mask) + (((_gk_color & _gk_a_mask) - (dc & _gk_a_mask)) * alpha >> 8)) & _gk_a_mask;
        *(unsigned short*)p = R | G | B | A;
      }
      break;
    }

    case 3:
    {
      Uint8 rshift8 = _gk_bmp->format->Rshift / 8;
      Uint8 gshift8 = _gk_bmp->format->Gshift / 8;
      Uint8 bshift8 = _gk_bmp->format->Bshift / 8;
      Uint8 ashift8 = _gk_bmp->format->Ashift / 8;

      if (alpha == 255)
      {
        *(p + rshift8) = _gk_color >> _gk_bmp->format->Rshift;
        *(p + gshift8) = _gk_color >> _gk_bmp->format->Gshift;
        *(p + bshift8) = _gk_color >> _gk_bmp->format->Bshift;
        *(p + ashift8) = _gk_color >> _gk_bmp->format->Ashift;
      }
      else
      {
        Uint8 dR, dG, dB, dA = 0;
        Uint8 sR, sG, sB, sA = 0;
        dR = *((p) + rshift8);
        dG = *((p) + gshift8);
        dB = *((p) + bshift8);
        dA = *((p) + ashift8);
        sR = (_gk_color >> _gk_bmp->format->Rshift) & 0xff;
        sG = (_gk_color >> _gk_bmp->format->Gshift) & 0xff;
        sB = (_gk_color >> _gk_bmp->format->Bshift) & 0xff;
        sA = (_gk_color >> _gk_bmp->format->Ashift) & 0xff;
        dR = dR + ((sR - dR) * alpha >> 8);
        dG = dG + ((sG - dG) * alpha >> 8);
        dB = dB + ((sB - dB) * alpha >> 8);
        dA = dA + ((sA - dA) * alpha >> 8);
        *((p) + rshift8) = dR;
        *((p) + gshift8) = dG;
        *((p) + bshift8) = dB;
        *((p) + ashift8) = dA;
      }
      break;
    }

    case 4:
    {
      if (alpha == 255) { *(unsigned*)p = _gk_color; }
      else
      {
        Uint32 dc = *(unsigned*)p;
        R = ((dc & _gk_r_mask) + (((_gk_color & _gk_r_mask) - (dc & _gk_r_mask)) * alpha >> 8)) & _gk_r_mask;
        G = ((dc & _gk_g_mask) + (((_gk_color & _gk_g_mask) - (dc & _gk_g_mask)) * alpha >> 8)) & _gk_g_mask;
        B = ((dc & _gk_b_mask) + (((_gk_color & _gk_b_mask) - (dc & _gk_b_mask)) * alpha >> 8)) & _gk_b_mask;
        if (_gk_a_mask) A = ((dc & _gk_a_mask) + (((_gk_color & _gk_a_mask) - (dc & _gk_a_mask)) * alpha >> 8)) & _gk_a_mask;
        *(unsigned*)p = R | G | B | A;
      }
      break;
    }
  }
}


void _gk_sdl_putpixel(int x,int y,int a)
{
  unsigned char* p;

  if (x<_gk_clip_left || x>=_gk_clip_right) return;
  if (y<_gk_clip_top || y>=_gk_clip_bottom) return;

  p = (unsigned char*) _gk_bmp->pixels + y * _gk_bmp->pitch + x * _gk_driver_bytes_per_pixel;

  switch (_gk_driver_bytes_per_pixel)
  {
    case 1: *p = _gk_color; break;
    case 2: *(unsigned short*)p = _gk_color; break;
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (_gk_color>>16) & 0xFF;
        p[1] = (_gk_color>>8) & 0xFF;
        p[2] = _gk_color & 0xFF;
      }
      else
      {
        p[0] = _gk_color & 0xFF;
        p[1] = (_gk_color>>8) & 0xFF;
        p[2] = (_gk_color>>16) & 0xFF;
      }
      break;
    case 4: *(unsigned int*)p = _gk_color; break;
  }
}


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
          if (a==255) _gk_sdl_putpixel(bx,by,a);
          else _gk_sdl_putpixel_alpha(bx,by,a);
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

  if (!glyph->index) unload_glyph(glyph);

#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"_gk_put_glyph() end\n");
#endif
}
