/*
 * glyph_to_allegro.c  -  part of Glyph Keeper Allegro driver.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


#ifndef MAKE_VERSION

#if (ALLEGRO_SUB_VERSION&1)
#define MAKE_VERSION(a, b, c) (((a)<<16)|((b)<<8)|(c))
#else
#define MAKE_VERSION(a, b, c) (((a)<<16)|((b)<<8))
#endif

#endif /* MAKE_VERSION */


#ifdef GLYPH_TARGET_HAS_RECTFILL
static void _gk_driver_rectfill(BITMAP* const bmp,const int x1,const int y1,const int x2,const int y2,const int color)
{
  int cd = bitmap_color_depth(bmp);
  int c = makecol_depth(cd,(color>>16)&0xFF,(color>>8)&0xFF,color&0xFF);
  rectfill(bmp,x1,y1,x2,y2,c);
}

#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED
static void _gk_driver_rectfill_angled(BITMAP* const bmp,const int x1,const int y1,
  const int x2,const int y2,const int x3,const int y3,const int x4,const int y4,const int color)
{
  int cd = bitmap_color_depth(bmp);
  int c = makecol_depth(cd,(color>>16)&0xFF,(color>>8)&0xFF,color&0xFF);
  triangle(bmp,x1,y1,x2,y2,x3,y3,c);
  triangle(bmp,x1,y1,x3,y3,x4,y4,c);
}
#endif  /* GLYPH_TARGET_HAS_RECTFILL_ANGLED */

#endif  /* GLYPH_TARGET_HAS_RECTFILL */


static BITMAP* bmp = 0;
static int bitmap_acquired;
static GLYPH_REND* rend = 0;
static int coldepth;
static int ct,cb,cl,cr;
static unsigned alpha_color;
static int alpha;
static unsigned text_color;
static int _gk_driver_back_color;
/*int r,g,b*/
static unsigned _gk_driver_last_text_alpha_color;
static int _gk_driver_last_back_color;
static int _gk_driver_back_to_front[256];
static void (*drawer)(const GLYPH* const glyph,const int x0,const int y0) = 0;

#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
static unsigned last_monotrans_alpha_color = 0;
static unsigned char rtrans[256], gtrans[256], btrans[256];
#endif  /* GLYPH_TARGET_KNOWS_MONO_BITPACK */





#include "glyph_to_allegro_mono.c"

#include "glyph_to_allegro_aa.c"





static void _gk_prepare_to_draw(BITMAP* const new_bmp,GLYPH_REND* const new_rend)
{
  CARE(new_bmp);
  CARE(new_rend);

#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"prepare_to_draw() begin\n");
#endif

  alpha_color = new_rend->text_alpha_color;
  alpha = alpha_color >> 24;
  if (!alpha) return;
  rend = new_rend;
  bmp = new_bmp;

  coldepth = bitmap_color_depth(bmp);
  if (bmp->clip) { ct = bmp->ct; cb = bmp->cb; cl = bmp->cl; cr = bmp->cr; }
  else { ct = 0; cb = bmp->h; cl = 0; cr = bmp->w; }
  /*int r = (alpha_color>>16)&0xFF;
  int g = (alpha_color>>8)&0xFF;
  int b = alpha_color&0xFF;
  text_color = makecol_depth(coldepth,r,g,b);*/
  text_color = makecol_depth(coldepth,(alpha_color>>16)&0xFF,(alpha_color>>8)&0xFF,alpha_color&0xFF);
  if (rend->back_color >= 0)
    _gk_driver_back_color = makecol_depth(coldepth,(rend->back_color>>16)&0xFF,(rend->back_color>>8)&0xFF,rend->back_color&0xFF);
  else
    _gk_driver_back_color = -1;

#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
  if (rend->render_mode == FT_RENDER_MODE_MONO)
  {
    if (alpha==255)
    {
      if (coldepth==32) drawer = &put_glyph_mono_opaque_32;
      else if (coldepth==16) drawer = &put_glyph_mono_opaque_16;
      else if (coldepth==8) drawer = &put_glyph_mono_opaque_8;
      else drawer = &put_glyph_mono;
    }
    else
    {
      if (coldepth==32) drawer = &put_glyph_mono_transparent_32;
      else if (coldepth==16) drawer = &put_glyph_mono_transparent_16;
      else drawer = &put_glyph_mono;
    }
  }
  else
#endif
  {
    if ( (_gk_driver_back_color >= 0) && (coldepth > 8) &&
         (alpha_color != _gk_driver_last_text_alpha_color || _gk_driver_back_color != _gk_driver_last_back_color) )
    {
      int r = (alpha_color>>16)&0xFF;
      int g = (alpha_color>>8)&0xFF;
      int b = alpha_color&0xFF;
      int back_r = (_gk_driver_back_color>>16)&0xFF;
      int back_g = (_gk_driver_back_color>>8)&0xFF;
      int back_b = _gk_driver_back_color&0xFF;
      int i_r = ((r - back_r)<<16)/255;
      int i_g = ((g - back_g)<<16)/255;
      int i_b = ((b - back_b)<<16)/255;
      int rr = (back_r << 16) + 0x7FFF;
      int gg = (back_g << 16) + 0x7FFF;
      int bb = (back_b << 16) + 0x7FFF;
      int i;
      _gk_driver_back_to_front[0] = makecol_depth(coldepth,rr>>16,gg>>16,bb>>16);

      for (i=1; i<=255; i++)
      {
        rr += i_r;
        gg += i_g;
        bb += i_b;
        _gk_driver_back_to_front[i] = makecol_depth(coldepth,rr>>16,gg>>16,bb>>16);
      }

      _gk_driver_last_text_alpha_color = alpha_color;
      _gk_driver_last_back_color = _gk_driver_back_color;
    }

    if (coldepth==32) drawer = (_gk_driver_back_color == -1) ? &put_glyph_aa_32 : &put_glyph_aa_32_back;
    else if (coldepth==16) drawer = &put_glyph_aa_16;
    else if (coldepth==8) drawer = &put_glyph_aa_8;
    else drawer = &put_glyph_aa;
  }

  acquire_bitmap(bmp);
  bitmap_acquired = 1;

#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"prepare_to_draw() end\n");
#endif
}


static void _gk_done_drawing()
{
  if (bitmap_acquired) { release_bitmap(bmp); bitmap_acquired = 0; }
}


/*
 * This function must unload the glyph, if it is not cached
 */
static void _gk_put_glyph(GLYPH* const glyph,const int x,const int y)
{
  CARE(glyph);
  CARE(bmp);

#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"put_glyph() begin: U+%04X\n",glyph->unicode);
#endif

  if (glyph->bmp && (x < cr) && (y < cb) && (x+glyph->width > cl) && (y+glyph->height > ct) && (alpha) )
  {
#ifdef GLYPH_LOG
    if (glyph_log)
    {
      unsigned char format = *glyph->bmp;
      if (format==GLYPH_UNCOMPRESSED) { fprintf(glyph_log,"   <UNCOMPRESSED>\n"); }
#ifdef GLYPH_TARGET_KNOWS_RLEAA
      else if (format==GLYPH_RLEAA) { fprintf(glyph_log,"   <RLEAA>\n"); }
#endif
#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
      else if (format==GLYPH_MONO_BITPACK) { fprintf(glyph_log,"   <MONO_BITPACK>\n"); }
#endif
#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
      else if (format==GLYPH_MONO_RLE7) { fprintf(glyph_log,"   <MONO_RLE7>\n"); }
#endif
      else fprintf(glyph_log,"   <UNKNOWN>\n");
    }
#endif

    CARE(glyph->bmpsize);
    drawer(glyph,x,y);
  }

  if (!glyph->index) unload_glyph(glyph);

#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"put_glyph() end\n");
#endif
}



/***************************************************************************************
 *
 * The following code is producing and handling Allegro FONT and VTABLE structures.
 */


static int gk_text_width_allegro(GLYPH_REND* const rend,AL_CONST char* const text)
{
  int px = 0;
  AL_CONST char* c = text;
  unsigned code;

  if (!rend || !text) return 0;

  while ( (code = ugetxc(&c)) )
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


static void gk_text_advance_subpixel_allegro(GLYPH_REND* const rend,AL_CONST char* const text,int* const adv_x,int* const adv_y)
{
  int px = 0, py = 0;
  AL_CONST char* c = text;
  unsigned code;

  if (!adv_x || !adv_y) return;
  if (!rend || !text) { *adv_x = *adv_y = 0; return; }

  while ( (code = ugetxc(&c)) )
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


/*static void gk_text_advance_allegro(GLYPH_REND* const rend,AL_CONST char* const text,int* const adv_x,int* const adv_y)
{
  gk_text_advance_subpixel_allegro(rend,text,adv_x,adv_y);
  *adv_x = (*adv_x+31)>>6;
  *adv_y = (*adv_y+31)>>6;
}*/


static int _gk_vtable_font_height(AL_CONST FONT *f)
{
  CARE(f);
  return f->height;
}


static int _gk_vtable_char_length(AL_CONST FONT *f,int ch)
{
  CARE(f);
  if (!f->data) return 0;
  return gk_char_width((GLYPH_REND*)f->data,ch);
}


static int _gk_vtable_text_length(AL_CONST FONT *f,AL_CONST char *text)
{
  GLYPH_REND* r;

  CARE(f && text);
  r = (GLYPH_REND*)f->data;
  if (!r) return 0;

  return gk_text_width_allegro(r,text);
}


static int _gk_vtable_render_char(AL_CONST FONT *f,int ch,int fg,int bg,BITMAP *bmp,int x,int y)
{
  int adv = 0;
  int pen_x = x, pen_y = y, asc;
  GLYPH_REND* r;

  CARE(f && bmp);
  if (!f->data) return 0;

  r = (GLYPH_REND*)f->data;
  asc = gk_rend_ascender_pixels(r);
  gk_rend_set_text_color_rgb(r,getr(fg),getg(fg),getb(fg));
  if (bg>=0) gk_rend_set_back_color(r,(getr(bg)<<16)|(getg(bg)<<8)|getb(bg));
  else gk_rend_set_back_color(r,-1);

  if (r->text_angle == 0) { pen_y += asc; }
  else
  {
    pen_x += (int)(sin(r->text_angle)*asc);
    pen_y += (int)(cos(r->text_angle)*asc);
  }

  _gk_prepare_to_draw(bmp,r);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (r->back_color >= 0)
  {
    int ax,ay;
    gk_char_advance_subpixel(r,ch,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,r,pen_x,pen_y,ax,ay);
  }
#endif

  pen_x <<= 6;
  pen_x <<= 6;
  _gk_internal_render_char(rend,ch,&pen_x,&pen_y);
  _gk_done_drawing();

  /*glyph = rend_render(rend,ch);
  if (glyph)
  {
    adv = (glyph->advance_x+63)>>6;
    if (bg>=0 && r->text_angle==0) rectfill(bmp,x,y,x+adv,y+r->text_height_pixels-1,bg);
    put_glyph(glyph,x+glyph->left,y-glyph->top+gk_rend_ascender_pixels(r));
  }*/

  return adv;
}


static void _gk_vtable_render(AL_CONST FONT *f,AL_CONST char* text,int fg,int bg,BITMAP *bmp,int x,int y)
{
  AL_CONST char* p = text;
  int ch = 0;
  GLYPH_REND* r;
  int tx,ty,asc;
  int pen_x = x, pen_y = y;

  CARE(f && text && bmp);
  if (!f->data) return;

  r = (GLYPH_REND*)f->data;
  asc = gk_rend_ascender_pixels(r);
  gk_rend_set_text_color_rgb(r,getr(fg),getg(fg),getb(fg));
  if (bg>=0) gk_rend_set_back_color(r,(getr(bg)<<16)|(getg(bg)<<8)|getb(bg));
  else gk_rend_set_back_color(r,-1);

  if (r->text_angle == 0) { pen_y += asc; }
  else
  {
    pen_x += (int)(sin(r->text_angle)*asc);
    pen_y += (int)(cos(r->text_angle)*asc);
  }

  _gk_prepare_to_draw(bmp,r);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (r->back_color >= 0)
  {
    int ax,ay;
    gk_text_advance_subpixel_allegro(r,text,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,r,pen_x,pen_y,ax,ay);
  }
#endif

  tx = pen_x<<6;
  ty = pen_y<<6;
  while ( (ch = ugetxc(&p)) )
    _gk_internal_render_char(rend,ch,&tx,&ty);
  _gk_done_drawing();
}


/*
 * This function only destroys the FONT structure, not the renderer.
 */
static void _gk_vtable_destroy(FONT *f)
{
  if (f)
  {
    if (f->data) ((GLYPH_REND*)f->data)->target_info = 0;
    free(f);
  }
}


#if MAKE_VERSION(ALLEGRO_VERSION,ALLEGRO_SUB_VERSION,ALLEGRO_WIP_VERSION) >= MAKE_VERSION(4,1,18)

/*void _gk_count_ranges(GLYPH_FACE *f)
{
  int charcode, prevcode = -2, index = 0;
  FT_UInt gindex;
  unsigned range_num = 0;

  CARE(f);

  range_num = 0;
  charcode = FT_Get_First_Char(f->face,&gindex);
  while (gindex != 0)
  {
    if (charcode-prevcode>1) range_num++;
    prevcode = charcode;
    charcode = FT_Get_Next_Char(f->face,charcode,&gindex);
  }
  f->number_of_ranges = range_num;
  if (range_num <= 0) return;

  f->range_start = (int*)malloc(range_num*sizeof(int));
  if (!f->range_start) return;
  f->range_end = (int*)malloc(range_num*sizeof(int));
  if (!f->range_end) { free(f->range_start); f->range_start = 0; return; }

  prevcode = -2;
  charcode = FT_Get_First_Char(f->face,&gindex);
  while (gindex != 0 && index < range_num)
  {
    if (charcode-prevcode>1) { f->range_start[index] =  }
    prevcode = charcode;
    charcode = FT_Get_Next_Char(f->face,charcode,&gindex);
  }

}*/


/*
 * Returns the number of character ranges in a font,
 * or -1 if that information is not available.
 */
/*static int _gk_get_font_ranges(FONT *f)
{
  GLYPH_FACE *face;

  if (!f || !f->data) return -1;
  face = ((GLYPH_REND*)f->data)->face;
  if (!face) return -1;
  if (face->number_of_ranges==-1) _gk_count_ranges(face);
  return face->number_of_ranges;
}*/

static struct FONT_VTABLE font_vtable_gk =
{
  _gk_vtable_font_height,
  _gk_vtable_char_length,
  _gk_vtable_text_length,
  _gk_vtable_render_char,
  _gk_vtable_render,
  _gk_vtable_destroy,

  0, /*_gk_get_font_ranges,*/
  0,
  0,
  0,
  0
};

#else  /* MAKE_VERSION(...) >= MAKE_VERSION(4,1,18) */

static struct FONT_VTABLE font_vtable_gk =
{
  _gk_vtable_font_height,
  _gk_vtable_char_length,
  _gk_vtable_text_length,
  _gk_vtable_render_char,
  _gk_vtable_render,
  _gk_vtable_destroy
};

#endif  /* MAKE_VERSION(...) >= MAKE_VERSION(4,1,18) */


FONT* gk_create_allegro_font(GLYPH_REND* const r)
{
  FONT* f;

  if (!r) return 0;
  if (r->target_info) return r->target_info;

  f = (FONT*)malloc(sizeof(FONT));
  if (!f) return 0;
  f->data = r;
  r->target_info = f;
  f->height = r->text_height_pixels;
  f->vtable = &font_vtable_gk;
  return f;
}
