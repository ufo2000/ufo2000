/*
 * glyph_to_sdl.c  -  part of Glyph Keeper SDL driver.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file uses code from SDL_gfx library, written by Andreas Schiffler.
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


#ifdef GLYPH_TARGET_HAS_RECTFILL

void _gk_sdl_box_color(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
  Sint16 left, right, top, bottom;
  Uint8 *pixel, *pixellast;
  int x, dx;
  int dy;
  int pixx, pixy;
  Sint16 w, h, tmp;
  Uint8 *colorptr;

  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  if ((x1<left) && (x2<left)) return;
  if ((x1>right) && (x2>right)) return;
  if ((y1<top) && (y2<top)) return;
  if ((y1>bottom) && (y2>bottom)) return;

  if (x1<left) { x1=left; } else if (x1>right) { x1=right; }
  if (x2<left) { x2=left; } else if (x2>right) { x2=right; }
  if (y1<top) { y1=top; } else if (y1>bottom) { y1=bottom; }
  if (y2<top) { y2=top; } else if (y2>bottom) { y2=bottom; }

  if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
  if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }

  w = x2 - x1;
  h = y2 - y1;

  colorptr = (Uint8 *) & color;
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  else
    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);

  dx = w;
  dy = h;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
  pixellast = pixel + pixx * dx + pixy * dy;
  dx++;
	
  switch (dst->format->BytesPerPixel)
  {
    case 1:
      for (; pixel <= pixellast; pixel += pixy) { memset(pixel, (Uint8) color, dx); }
      break;
    case 2:
      pixy -= (pixx * dx);
      for (; pixel <= pixellast; pixel += pixy)
        for (x = 0; x < dx; x++)
        {
          *(Uint16 *) pixel = color;
          pixel += pixx;
        }
      break;
    case 3:
      pixy -= (pixx * dx);
      for (; pixel <= pixellast; pixel += pixy)
      {
        for (x = 0; x < dx; x++)
        {
          if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          {
            pixel[0] = (color >> 16) & 0xff;
            pixel[1] = (color >> 8) & 0xff;
            pixel[2] = color & 0xff;
          }
          else
          {
            pixel[0] = color & 0xff;
            pixel[1] = (color >> 8) & 0xff;
            pixel[2] = (color >> 16) & 0xff;
          }
          pixel += pixx;
        }
      }
      break;
    default:
      pixy -= (pixx * dx);
      for (; pixel <= pixellast; pixel += pixy)
        for (x = 0; x < dx; x++)
        {
          *(Uint32 *) pixel = color;
          pixel += pixx;
        }
      break;
  }
}


static void _gk_driver_rectfill(SDL_Surface* const bmp,const int x1,const int y1,const int x2,const int y2,const int color)
{
  unsigned r = (color>>16)&0xFF;
  unsigned g = (color>>8)&0xFF;
  unsigned b = (color)&0xFF;

  _gk_sdl_box_color(bmp,x1,y1,x2,y2,(r<<24)|(g<<16)|(b<<8));

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


void _gk_sdl_hline_color(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
  Sint16 left, right, top, bottom;
  Uint8 *pixel, *pixellast;
  int dx;
  int pixx, pixy;
  Sint16 w;
  Sint16 xtmp;
  Uint8 *colorptr;

  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  if ((x1<left) && (x2<left)) { return; }
  if ((x1>right) && (x2>right)) { return; }
  if ((y<top) || (y>bottom)) { return; }

  if (x1 < left) { x1 = left; }
  if (x2 > right) { x2 = right; }
  if (x1 > x2) { xtmp = x1; x1 = x2; x2 = xtmp; }

  w = x2 - x1;
  if (w < 0) { return; }

  colorptr = (Uint8 *) & color;
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  else
    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);

  dx = w;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

  switch (dst->format->BytesPerPixel)
  {
    case 1:
      memset(pixel, color, dx);
      break;
    case 2:
      pixellast = pixel + dx + dx;
      for (; pixel <= pixellast; pixel += pixx) { *(Uint16 *) pixel = color; }
      break;
    case 3:
      pixellast = pixel + dx + dx + dx;
      for (; pixel <= pixellast; pixel += pixx)
      {
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
          pixel[0] = (color >> 16) & 0xff;
          pixel[1] = (color >> 8) & 0xff;
          pixel[2] = color & 0xff;
        }
        else
        {
          pixel[0] = color & 0xff;
          pixel[1] = (color >> 8) & 0xff;
          pixel[2] = (color >> 16) & 0xff;
        }
      }
      break;
    default:
      dx = dx + dx;
      pixellast = pixel + dx + dx;
      for (; pixel <= pixellast; pixel += pixx) { *(Uint32 *) pixel = color; }
      break;
  }
}


int _gk_sdl_compare_ints(const void *a, const void *b)
{
  return (*(const int *) a) - (*(const int *) b);
}

static int *_gk_sdl_poly_ints = 0;
static int _gk_sdl_poly_allocated = 0;


void _gk_sdl_filled_polygon_color(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, Uint32 color)
{
  int i;
  int y, xa, xb;
  int miny, maxy;
  int x1, y1;
  int x2, y2;
  int ind1, ind2;
  int ints;

  if (n < 3) { return; }

  if (!_gk_sdl_poly_allocated)
  {
    _gk_sdl_poly_ints = (int *) malloc(sizeof(int) * n);
    _gk_sdl_poly_allocated = n;
  }
  else
  {
    if (_gk_sdl_poly_allocated < n)
    {
      _gk_sdl_poly_ints = (int *) realloc(_gk_sdl_poly_ints, sizeof(int) * n);
      _gk_sdl_poly_allocated = n;
    }
  }

  miny = vy[0];
  maxy = vy[0];
  for (i = 1; (i < n); i++)
    { if (vy[i] < miny) miny = vy[i]; else if (vy[i] > maxy) maxy = vy[i]; }

  for (y = miny; (y <= maxy); y++)
  {
    ints = 0;
    for (i = 0; (i < n); i++)
    {
      if (!i) { ind1 = n - 1; ind2 = 0; }
      else { ind1 = i - 1; ind2 = i; }
      y1 = vy[ind1];
      y2 = vy[ind2];
      if (y1 < y2) { x1 = vx[ind1]; x2 = vx[ind2]; }
      else if (y1 > y2) { y2 = vy[ind1]; y1 = vy[ind2]; x2 = vx[ind1]; x1 = vx[ind2]; }
      else { continue; }
      if ( ((y >= y1) && (y < y2)) || ((y == maxy) && (y > y1) && (y <= y2)) )
      {
        _gk_sdl_poly_ints[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
      } 
    }

    qsort(_gk_sdl_poly_ints, ints, sizeof(int), _gk_sdl_compare_ints);

    for (i = 0; (i < ints); i += 2)
    {
      xa = _gk_sdl_poly_ints[i] + 1;
      xa = (xa >> 16) + ((xa & 32768) >> 15);
      xb = _gk_sdl_poly_ints[i+1] - 1;
      xb = (xb >> 16) + ((xb & 32768) >> 15);
      _gk_sdl_hline_color(dst, xa, xb, y, color);
    }
  }
}


static void _gk_driver_rectfill_angled(SDL_Surface* const bmp,const int x1,const int y1,
  const int x2,const int y2,const int x3,const int y3,const int x4,const int y4,const int color)
{
  unsigned r = (color>>16)&0xFF;
  unsigned g = (color>>8)&0xFF;
  unsigned b = (color)&0xFF;
  short vx[4] = { x1, x2, x3, x4 };
  short vy[4] = { y1, y2, y3, y4 };

  _gk_sdl_filled_polygon_color(bmp,vx,vy,4,(r<<24)|(g<<16)|(b<<8));

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
#endif  /* GLYPH_TARGET_HAS_RECTFILL_ANGLED */

#endif  /* GLYPH_TARGET_HAS_RECTFILL */
