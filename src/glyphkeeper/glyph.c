/*
 * glyph.c  -  Glyph Keeper source code, main part.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#include "glyph.h"

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_SYNTHESIS_H


/* Maximum possible Unicode code point. */
#define GK_MAX_UNICODE   0x10FFFFu

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


#ifdef GLYPH_LOG
static FILE* glyph_log = 0;
#endif


/*
 * Passing messages from Glyph Keeper to the application.
 */
static void (*_gk_messenger)(const char* const) = 0;

void gk_set_messenger(void (*func)(const char* const))
{
  _gk_messenger = func;
}


static void _gk_msg(const char* const format,...)
{
  va_list arg;
  static char buf[1000] = "[glyph] ";
  if (!_gk_messenger) return;
  va_start(arg,format);
  vsprintf(buf+8,format,arg);
  va_end(arg);
#ifdef GLYPH_LOG
  if (glyph_log) fprintf(glyph_log,"msg: %s\n",buf);
#endif
  _gk_messenger(buf);
}



/*
 * Font path
 */
static char* _gk_font_path = 0;

void gk_set_font_path(char* const path)
{
  _gk_font_path = path;
}



static char* funcname = "";

static void* _gk_malloc(const size_t size)
{
  void* buf = malloc(size);
  if (!buf) _gk_msg("Error: %s: Not enough memory! Can't allocate %d bytes\n",funcname,size);
  return buf;
}


/* This variable is set to 1, if gk_library_cleanup() */
/* shutdown handler is already installed with atexit(). */
static int _gk_atexit_installed = 0;

static void _gk_install_exit_handler()
{
  if (!_gk_atexit_installed)
  {
    if (atexit(gk_library_cleanup)==0)
      _gk_atexit_installed = 1;
    else
      _gk_msg("Error: Can't install exit handler with atexit().\n");
  }
}


/* Internal buffer used for RLE compression. */
static unsigned char* rle_buffer = 0;
static unsigned rle_buffer_size = 0;


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
 * Structures used in Glyph Keeper.
 */
#include "glyph_structs.c"



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



/*
 * Returns size in bytes of one glyph.
 * Both size of GLYPH structure and size of glyph bitmap are counted.
 */
static unsigned glyph_size_in_bytes(const GLYPH* const g)
{
  return sizeof(GLYPH) + g->bmpsize;
}


/*
 * UTF-8 decoder.
 * (taken from Allegro)
 */
static unsigned _gk_utf8_getx(const char** s)
{
  int c;
  CARE(s);
  c = *((unsigned char *)((*s)++));
  if (c&0x80)
  {
    int n = 1;
    while (c&(0x80>>n)) n++;
    c &= (1<<(8-n))-1;
    while (--n>0)
    {
      int t = *((unsigned char *)((*s)++));
      if ((!(t&0x80))||(t&0x40)) { (*s)--; return '?'; }
      c = (c<<6)|(t&0x3F);
    }
  }
  return c;
}



/*
 * Endianness flag for UTF-16 decoding.
 * 0 if UTF-16 string being processed currently has same
 * byte order with the machine the program's running on.
 * 1 if the UTF-16 string being decoded currently has
 * wrong byte order, and need byte swapping.
 */
static int _gk_utf16_opposite_byte_sex = 0;

/*
 * UTF-16 decoding functions.
 * 's' and '*s' must be not null.
 */
static void _gk_utf16_start_decoding(const unsigned short** s)
{
  unsigned short a = **s;
  if (a == 0xFFFE) { _gk_utf16_opposite_byte_sex = 1; (*s)++; return; }
  _gk_utf16_opposite_byte_sex = 0;
  if (a == 0xFEFF) { (*s)++; }
}

static unsigned _gk_utf16_decode(const unsigned short** s)
{
  unsigned short a,b;
  const unsigned short* ss = *s;
  a = *ss;
  ss++;
  if (_gk_utf16_opposite_byte_sex) a = ((a&0xFF)<<8)|(a>>8);
  if (a <= 0xD7FF || a >= 0xE000) { *s = ss; return a; }
  if (a > 0xDBFF) { return 0; }
  b = *ss;
  ss++;
  if (_gk_utf16_opposite_byte_sex) b = ((b&0xFF)<<8)|(b>>8);
  if (b < 0xDC00 || b > 0xDFFF) { return 0; }
  return 0x10000 + ( (((int)a&0x3FF)<<10) | ((int)b&0x3FF) );
}



/*
 * Endianness flag for UTF-32 decoding.
 */
static int _gk_utf32_opposite_byte_sex = 0;

static void _gk_utf32_start_decoding(const unsigned int** s)
{
  unsigned int a = **s;
  if (a == 0xFFFE0000) { _gk_utf32_opposite_byte_sex = 1; (*s)++; return; }
  _gk_utf32_opposite_byte_sex = 0;
  if (a == 0x0000FEFF) { (*s)++; }
}

static unsigned _gk_utf32_decode(const unsigned int** s)
{
  unsigned int a = **s;
  if (_gk_utf32_opposite_byte_sex)
  {
    a = ((a&0x000000FF)<<24) | ((a&0x0000FF00)<<8) | ((a&0x00FF0000)>>8) | (a>>24);
  }
  if (a <= 0xD7FF || a >= 0xE000) { (*s)++; return a; }
  else { return 0; }
}



/*
 * GLYPH_FACE routines.
 */
#include "glyph_face.c"


/*
 * GLYPH_INDEX and GLYPH_KEEP routines.
 */
#include "glyph_index.c"


/*
 * GLYPH_REND routines.
 */
#include "glyph_rend.c"



/*
 * Init/cleanup functions
 */

void gk_library_init()
{
  int error;
  FT_Int amajor,aminor,apatch;

  if (ftlib) return;

#ifdef GLYPH_LOG
  if (!glyph_log)
  {
    glyph_log = fopen("glyph.log","w");
    if (!glyph_log)
    {
      _gk_msg("Error: gk_library_init(): Can't create a log-file \"glyph.log\"\n");
      exit(1);
    }

    /* It's a log-file, nothing should be lost. */
    setbuf(glyph_log,0);
    fprintf(glyph_log,"glyph.log started\n");
  }
#endif

  error = FT_Init_FreeType(&ftlib);
  if (error || !ftlib) { _gk_msg("Error: gk_library_init(): can't initialize FreeType\n"); ftlib = 0; return; }
  FT_Library_Version(ftlib,&amajor,&aminor,&apatch);
  _gk_msg("FreeType %d.%d.%d initialized\n",amajor,aminor,apatch);

  _gk_install_exit_handler();
}


void gk_library_cleanup(void)
{
  while (first_renderer) gk_done_renderer(first_renderer);
  while (first_keeper) gk_done_keeper(first_keeper);
  while (first_face) gk_unload_face(first_face);

  if (ftlib) { FT_Done_FreeType(ftlib); ftlib = 0; }

  if (rle_buffer) { free(rle_buffer); rle_buffer = 0; }
  rle_buffer_size = 0;

#ifdef GLYPH_LOG
  if (glyph_log) { fclose(glyph_log); glyph_log = 0; }
#endif
}


/*
 * Checks how many bytes are allocated by Glyph Keeper.
 * (FreeType memory is not counted)
 */
int gk_bytes_allocated()
{
  int n = 0;
  GLYPH_FACE *a;
  GLYPH_KEEP *b;
  GLYPH_REND *c;

  for (a = first_face; a; a=a->next)
  {
    n += sizeof(GLYPH_FACE);
#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
    if (a->range_start) n += a->number_of_ranges*sizeof(int);
    if (a->range_end) n += a->number_of_ranges*sizeof(int);
#endif
  }
  for (b = first_keeper; b; b=b->next) n += b->allocated;
  for (c = first_renderer; c; c=c->next) n += sizeof(GLYPH_REND);
  n += rle_buffer_size;
  return n;
}



void gk_precache_char(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph;
  if (!rend->index) return;
  glyph = rend_render(rend,unicode);
  if (glyph && !glyph->index) unload_glyph(glyph);
}



/*
 * Routines finding text size
 */
#include "glyph_dimensions.c"


/*
 * Draws background rectangle.
 * adv_x and adv_y should not be both zero.
 */
#ifdef GLYPH_TARGET_HAS_RECTFILL
static void _gk_draw_background_rectangle(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
  const int pen_x,const int pen_y,const int adv_x,const int adv_y)
{
  if (rend->text_angle != 0)
  {
#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED
    int ascender = gk_rend_ascender(rend);
    int descender = gk_rend_descender(rend);
    int x1 = pen_x - (int)(sin(rend->text_angle)*(ascender>>6));
    int y1 = pen_y - (int)(cos(rend->text_angle)*(ascender>>6));
    int x2 = x1 + ((adv_x+63)>>6);
    int y2 = y1 + ((adv_y+63)>>6);
    int x4 = pen_x - (int)(sin(rend->text_angle)*(descender>>6));
    int y4 = pen_y - (int)(cos(rend->text_angle)*(descender>>6));
    int x3 = x4 + ((adv_x+63)>>6);
    int y3 = y4 + ((adv_y+63)>>6);
    _gk_driver_rectfill_angled(bmp,x1,y1,x2,y2,x3,y3,x4,y4,rend->back_color);
#endif
  }
  else
  {
    int ascender = gk_rend_ascender(rend);
    int descender = gk_rend_descender(rend);
    _gk_driver_rectfill(bmp,pen_x,pen_y-(ascender>>6),pen_x+((adv_x+63)>>6),pen_y-(descender>>6),rend->back_color);
  }
}
#endif





void gk_put_char(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
  const unsigned unicode,const int x,const int y)
{
  GLYPH *glyph = 0;
  if (!bmp || !rend) return;
  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    _gk_prepare_to_draw(bmp,rend);
    _gk_put_glyph(glyph,x,y);
    _gk_done_drawing();
  }
}

void gk_put_char_center(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,const unsigned unicode,
  const int x,const int y)
{
  GLYPH *glyph;
  if (!bmp || !rend) return;
  glyph = rend_render(rend,unicode);
  if (glyph)
  {
    _gk_prepare_to_draw(bmp,rend);
    _gk_put_glyph(glyph,x-glyph->width/2,y-glyph->height/2);
    _gk_done_drawing();
  }
}





/*
 * 'rend' must be not 0.
 * 'pen_x' and 'pen_y' are expressed in 26.6 pixels here.
 */
static void _gk_internal_render_char(GLYPH_REND* const rend,const unsigned unicode,int *pen_x,int *pen_y)
{
  GLYPH* glyph;
  int adv_x,adv_y;
  CARE(rend);

  glyph = rend_render(rend,unicode);
  if (!glyph) return;
  adv_x = glyph->advance_x;
  adv_y = glyph->advance_y;
  _gk_put_glyph(glyph,((*pen_x+31)>>6)+glyph->left,((*pen_y+31)>>6)-glyph->top);
  *pen_x += adv_x;
  *pen_y -= adv_y;
}





void gk_render_char(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const unsigned unicode,
  int* const pen_x,int* const pen_y)
{
  int x,y;

  if (!bmp || !rend || !pen_x || !pen_y) return;

  _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (rend->back_color >= 0)
  {
    int ax,ay;
    gk_char_advance(rend,unicode,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,rend,*pen_x,*pen_y,ax,ay);
  }
#endif

  x = *pen_x<<6;
  y = *pen_y<<6;
  _gk_internal_render_char(rend,unicode,&x,&y);
  _gk_done_drawing();
  *pen_x = x>>6;
  *pen_y = y>>6;
}






void gk_render_line_utf8(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const char* const text,
  const int pen_x,const int pen_y)
{
  int x,y;
  const char* c = text;
  unsigned code;

  if (!bmp || !rend || !text) return;

  _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (rend->back_color >= 0)
  {
    int ax,ay;
    gk_text_advance_subpixel_utf8(rend,text,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
  }
#endif

  x = pen_x<<6;
  y = pen_y<<6;
  while ( (code = _gk_utf8_getx(&c)) )
    _gk_internal_render_char(rend,code,&x,&y);
  _gk_done_drawing();
}


void gk_render_line_utf16(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const unsigned short* const text,
  const int pen_x,const int pen_y)
{
  int x,y;
  const unsigned short* c = text;
  unsigned code;

  if (!bmp || !rend || !text) return;

  _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (rend->back_color >= 0)
  {
    int ax,ay;
    gk_text_advance_subpixel_utf16(rend,text,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
  }
#endif

  _gk_utf16_start_decoding(&c);
  x = pen_x<<6;
  y = pen_y<<6;
  while ( (code = _gk_utf16_decode(&c)) )
    _gk_internal_render_char(rend,code,&x,&y);
  _gk_done_drawing();
}


void gk_render_line_utf32(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const unsigned* const text,
  const int pen_x,const int pen_y)
{
  int x,y;
  const unsigned* c = text;
  unsigned code;

  if (!bmp || !rend || !text) return;

  _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
  if (rend->back_color >= 0)
  {
    int ax,ay;
    gk_text_advance_subpixel_utf32(rend,text,&ax,&ay);
    if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
  }
#endif

  _gk_utf32_start_decoding(&c);
  x = pen_x<<6;
  y = pen_y<<6;
  while ( (code = _gk_utf32_decode(&c)) )
    _gk_internal_render_char(rend,code,&x,&y);
  _gk_done_drawing();
}


/*
 * Driver code
 */
#include GK_DRIVER_CODE
