/*
 * glyph_dimensions.c  -  Glyph Keeper routines for finding text size
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


/*
 * This function is useful to determine size in pixels of rendered text.
 * Smallest possible rectangle, containing rendered text, is constructed.
 * Size of this rectangle is written to 'size_x' and 'size_y'.
 * Position of text origin point, relative to the top left corner of rectangle,
 * is written to 'origin_x' and 'origin_y'.
 */
/*void text_layout_utf8(GLYPH_REND* const rend,const char* const text,
  int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
  int xmin=INT_MAX,ymin=INT_MAX,xmax=INT_MIN,ymax=INT_MIN;
  int px=0,py=0;
  const char* c = text;
  unsigned code;

  if (!rend || !text) { *size_x = *size_y = *origin_x = *origin_y = 0; return; }

  while ( (code = _gk_utf8_getx(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      int x,y;

      x = (px>>6) + glyph->left;
      if (x<xmin) xmin = x;
      x += glyph->width-1;
      if (x>xmax) xmax = x;

      y = (py>>6) - glyph->top;
      if (y<ymin) ymin = y;
      y += glyph->height-1;
      if (y>ymax) ymax = y;

      px += glyph->advance_x;
      py -= glyph->advance_y;
    }
  }

  if (xmin>xmax || ymin>ymax) { *size_x = *size_y = *origin_x = *origin_y = 0; return; }

  *size_x = xmax-xmin+1;
  *size_y = ymax-ymin+1;
  *origin_x = -xmin;
  *origin_y = -ymin;
}*/





/***************************************************************************
 *
 *   gk_text_size...
 */

void gk_glyph_size(GLYPH_REND* const rend,const unsigned unicode,int *const width,int* const height)
{
  GLYPH *glyph;

  if (!width || !height) return;
  if (!rend || !unicode) { *width = *height = 0; return; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    *width = glyph->width;
    *height = glyph->height;
    if (!glyph->index) unload_glyph(glyph);
  }
  else
  {
    *width = 0;
    *height = 0;
  }
}


void gk_char_size(GLYPH_REND* const rend,const unsigned unicode,int *const width,int* const height)
{
  GLYPH *glyph;

  if (!width || !height) return;
  if (!rend || !unicode) { *width = *height = 0; return; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    if (rend->text_angle == 0)
    {
      *width = (glyph->advance_x + 63) >> 6;
      *height = rend->text_height_pixels;
    }
    else
    {
      *width = (abs(glyph->advance_x) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
      *height = (abs(glyph->advance_y) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
    }

    if (!glyph->index) unload_glyph(glyph);
  }
  else
  {
    *width = 0;
    *height = 0;
  }
}


void gk_text_size_utf8(GLYPH_REND* const rend,const char* const text,int* const width,int* const height)
{
  int px = 0, py = 0;
  const char* c = text;
  unsigned code;

  if (!width || !height) return;
  if (!rend || !text) { *width = *height = 0; return; }

  while ( (code = _gk_utf8_getx(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    *width = (px + 63) >> 6;
    *height = rend->text_height_pixels;
  }
  else
  {
    *width = (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
    *height = (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


void gk_text_size_utf16(GLYPH_REND* const rend,const unsigned short* const text,int* const width,int* const height)
{
  int px = 0, py = 0;
  const unsigned short* c = text;
  unsigned code;

  if (!width || !height) return;
  if (!rend || !text) { *width = *height = 0; return; }

  _gk_utf16_start_decoding(&c);
  while ( (code = _gk_utf16_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    *width = (px + 63) >> 6;
    *height = rend->text_height_pixels;
  }
  else
  {
    *width = (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
    *height = (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


void gk_text_size_utf32(GLYPH_REND* const rend,const unsigned* const text,int* const width,int* const height)
{
  int px = 0, py = 0;
  const unsigned* c = text;
  unsigned code;

  if (!width || !height) return;
  if (!rend || !text) { *width = *height = 0; return; }

  _gk_utf32_start_decoding(&c);
  while ( (code = _gk_utf32_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    *width = (px + 63) >> 6;
    *height = rend->text_height_pixels;
  }
  else
  {
    *width = (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
    *height = (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}





/***************************************************************************
 *
 *   gk_text_width...
 */

int gk_glyph_width(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph;
  int w = 0;

  if (!rend || !unicode) { return 0; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    w = glyph->width;
    if (!glyph->index) unload_glyph(glyph);
  }
  return w;
}


int gk_char_width(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph;
  int w = 0;

  if (!rend || !unicode) { return 0; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    if (rend->text_angle == 0)
      w = (glyph->advance_x + 63) >> 6;
    else
      w = (abs(glyph->advance_x) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;

    if (!glyph->index) unload_glyph(glyph);
  }
  return w;
}


int gk_text_width_utf8(GLYPH_REND* const rend,const char* const text)
{
  int px = 0;
  const char* c = text;
  unsigned code;

  if (!rend || !text) return 0;

  while ( (code = _gk_utf8_getx(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    return (px + 63) >> 6;
  }
  else
  {
    return (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


int gk_text_width_utf16(GLYPH_REND* const rend,const unsigned short* const text)
{
  int px = 0;
  const unsigned short* c = text;
  unsigned code;

  if (!rend || !text) return 0;

  _gk_utf16_start_decoding(&c);
  while ( (code = _gk_utf16_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    return (px + 63) >> 6;
  }
  else
  {
    return (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


int gk_text_width_utf32(GLYPH_REND* const rend,const unsigned* const text)
{
  int px = 0;
  const unsigned* c = text;
  unsigned code;

  if (!rend || !text) { return 0; }

  _gk_utf32_start_decoding(&c);
  while ( (code = _gk_utf32_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  if (rend->text_angle == 0)
  {
    return (px + 63) >> 6;
  }
  else
  {
    return (abs(px) + abs((int)(sin(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}





/***************************************************************************
 *
 *   gk_text_height...
 */

int gk_glyph_height(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph;
  int h = 0;

  if (!rend || !unicode) { return 0; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    h = glyph->height;
    if (!glyph->index) unload_glyph(glyph);
  }
  return h;
}


int gk_char_height(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph;
  int h = 0;

  if (!rend || !unicode) { return 0; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    if (rend->text_angle == 0)
      h = rend->text_height_pixels;
    else
      h = (abs(glyph->advance_y) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;

    if (!glyph->index) unload_glyph(glyph);
  }
  return h;
}


int gk_text_height_utf8(GLYPH_REND* const rend,const char* const text)
{
  if (!rend || !text) return 0;

  if (rend->text_angle == 0)
  {
    return rend->text_height_pixels;
  }
  else
  {
    int py = 0;
    const char* c = text;
    unsigned code;

    while ( (code = _gk_utf8_getx(&c)) )
    {
      GLYPH* glyph = rend_render(rend,code);
      if (glyph)
      {
        py -= glyph->advance_y;
        if (!glyph->index) unload_glyph(glyph);
      }
    }

    return (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


int gk_text_height_utf16(GLYPH_REND* const rend,const unsigned short* const text)
{
  if (!rend || !text) return 0;

  if (rend->text_angle == 0)
  {
    return rend->text_height_pixels;
  }
  else
  {
    int py = 0;
    const unsigned short* c = text;
    unsigned code;

    _gk_utf16_start_decoding(&c);
    while ( (code = _gk_utf16_decode(&c)) )
    {
      GLYPH* glyph = rend_render(rend,code);
      if (glyph)
      {
        py -= glyph->advance_y;
        if (!glyph->index) unload_glyph(glyph);
      }
    }

    return (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


int gk_text_height_utf32(GLYPH_REND* const rend,const unsigned* const text)
{
  if (!rend || !text) return 0;

  if (rend->text_angle == 0)
  {
    return rend->text_height_pixels;
  }
  else
  {
    int py = 0;
    const unsigned* c = text;
    unsigned code;

    _gk_utf32_start_decoding(&c);
    while ( (code = _gk_utf32_decode(&c)) )
    {
      GLYPH* glyph = rend_render(rend,code);
      if (glyph)
      {
        py -= glyph->advance_y;
        if (!glyph->index) unload_glyph(glyph);
      }
    }

    return (abs(py) + abs((int)(cos(rend->text_angle)*rend->text_height)) + 63) >> 6;
  }
}


/***************************************************************************
 *
 *   gk_text_advance_subpixel...
 */

void gk_char_advance_subpixel(GLYPH_REND* const rend,const unsigned unicode,int* const adv_x,int* const adv_y)
{
  GLYPH *glyph;

  if (!adv_x || !adv_y) return;
  if (!rend || !unicode) { *adv_x = *adv_y = 0; return; }

  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    *adv_x = glyph->advance_x;
    *adv_y = -glyph->advance_y;
    if (!glyph->index) unload_glyph(glyph);
  }
  else
  {
    *adv_x = 0;
    *adv_y = 0;
  }
}


void gk_text_advance_subpixel_utf8(GLYPH_REND* const rend,const char* const text,int* const adv_x,int* const adv_y)
{
  int px = 0, py = 0;
  const char* c = text;
  unsigned code;

  if (!adv_x || !adv_y) return;
  if (!rend || !text) { *adv_x = *adv_y = 0; return; }

  while ( (code = _gk_utf8_getx(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  *adv_x = px;
  *adv_y = py;
}


void gk_text_advance_subpixel_utf16(GLYPH_REND* const rend,const unsigned short* const text,int* const adv_x,int* const adv_y)
{
  int px = 0, py = 0;
  const unsigned short* c = text;
  unsigned code;

  if (!adv_x || !adv_y) return;
  if (!rend || !text) { *adv_x = *adv_y = 0; return; }

  _gk_utf16_start_decoding(&c);
  while ( (code = _gk_utf16_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  *adv_x = px;
  *adv_y = py;
}


void gk_text_advance_subpixel_utf32(GLYPH_REND* const rend,const unsigned* const text,int* const adv_x,int* const adv_y)
{
  int px = 0, py = 0;
  const unsigned* c = text;
  unsigned code;

  if (!adv_x || !adv_y) return;
  if (!rend || !text) { *adv_x = *adv_y = 0; return; }

  _gk_utf32_start_decoding(&c);
  while ( (code = _gk_utf32_decode(&c)) )
  {
    GLYPH* glyph = rend_render(rend,code);
    if (glyph)
    {
      px += glyph->advance_x;
      py -= glyph->advance_y;
      if (!glyph->index) unload_glyph(glyph);
    }
  }

  *adv_x = px;
  *adv_y = py;
}





/***************************************************************************
 *
 *   gk_text_advance...
 */

void gk_char_advance(GLYPH_REND* const rend,const unsigned unicode,int* const adv_x,int* const adv_y)
{
  gk_char_advance_subpixel(rend,unicode,adv_x,adv_y);
  *adv_x = (*adv_x+31)>>6;
  *adv_y = (*adv_y+31)>>6;
}

void gk_text_advance_utf8(GLYPH_REND* const rend,const char* const text,int* const adv_x,int* const adv_y)
{
  gk_text_advance_subpixel_utf8(rend,text,adv_x,adv_y);
  *adv_x = (*adv_x+31)>>6;
  *adv_y = (*adv_y+31)>>6;
}

void gk_text_advance_utf16(GLYPH_REND* const rend,const unsigned short* const text,int* const adv_x,int* const adv_y)
{
  gk_text_advance_subpixel_utf16(rend,text,adv_x,adv_y);
  *adv_x = (*adv_x+31)>>6;
  *adv_y = (*adv_y+31)>>6;
}

void gk_text_advance_utf32(GLYPH_REND* const rend,const unsigned* const text,int* const adv_x,int* const adv_y)
{
  gk_text_advance_subpixel_utf32(rend,text,adv_x,adv_y);
  *adv_x = (*adv_x+31)>>6;
  *adv_y = (*adv_y+31)>>6;
}
