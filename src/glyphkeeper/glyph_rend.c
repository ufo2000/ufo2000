/*
 * glyph_rend.c  -  Glyph Keeper routines dealing with renderer objects.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


static GLYPH_REND *first_renderer = 0, *last_renderer = 0;



void gk_rend_set_text_alpha_color(GLYPH_REND* const rend,const unsigned alpha_color)
{
  if (rend) rend->text_alpha_color = alpha_color;
}

unsigned gk_rend_get_text_alpha_color(const GLYPH_REND* const rend)
{
  return rend ? rend->text_alpha_color : 0;
}



void gk_rend_set_text_color_rgb(GLYPH_REND* const rend,
  const unsigned char r,const unsigned char g,const unsigned char b)
{
  if (rend) rend->text_alpha_color = (rend->text_alpha_color&0xFF000000) | (r<<16) | (g<<8) | b;
}

void gk_rend_set_text_color_combined(GLYPH_REND* const rend, const int color)
{
  if (rend) rend->text_alpha_color = (rend->text_alpha_color&0xFF000000) | (color & 0x00FFFFFF);
}

int gk_rend_get_text_color(const GLYPH_REND* const rend)
{
  return rend ? (rend->text_alpha_color&0x00FFFFFF) : 0;
}



void gk_rend_set_text_alpha(GLYPH_REND* const rend,const unsigned char alpha)
{
  if (rend) rend->text_alpha_color = (rend->text_alpha_color&0x00FFFFFF) | (alpha<<24);
}

unsigned gk_rend_get_text_alpha(const GLYPH_REND* const rend)
{
  return rend ? (rend->text_alpha_color>>24) : 0;
}



void gk_rend_set_back_color(GLYPH_REND* const rend,const int new_back_color)
{
  if (rend) rend->back_color = (new_back_color < 0) ? -1 : (new_back_color & 0x00FFFFFF);
}

int gk_rend_get_back_color(GLYPH_REND* const rend)
{
  return rend ? rend->back_color : -1;
}




int gk_rend_ascender(const GLYPH_REND* const rend)
{
  return (rend && rend->size) ? rend->size->metrics.ascender : 0;
}

int gk_rend_ascender_pixels(const GLYPH_REND* const rend)
{
  return (rend && rend->size) ? (rend->size->metrics.ascender+63)>>6 : 0;
}

int gk_rend_descender(const GLYPH_REND* const rend)
{
  return (rend && rend->size) ? rend->size->metrics.descender : 0;
}

int gk_rend_descender_pixels(const GLYPH_REND* const rend)
{
  return (rend && rend->size) ? (rend->size->metrics.descender+63)>>6 : 0;
}



int gk_rend_spacing(const GLYPH_REND* const rend)
{
  return rend ? rend->line_spacing : 0;
}

int gk_rend_spacing_pixels(const GLYPH_REND* const rend)
{
  return rend ? rend->line_spacing_pixels : 0;
}

int gk_rend_height(const GLYPH_REND* const rend)
{
  return rend ? rend->text_height : 0;
}

int gk_rend_height_pixels(const GLYPH_REND* const rend)
{
  return rend ? rend->text_height_pixels : 0;
}




#define DEFAULT_LOAD_FLAGS  (FT_LOAD_NO_BITMAP)

#define rend_set_hinting_mode(r,mode)                                              \
{                                                                                  \
  (r)->hinting_mode = (mode);                                                      \
  (r)->load_flags = DEFAULT_LOAD_FLAGS | (r)->hinting_mode | (r)->hinting_target;  \
}


void gk_rend_set_hinting_off(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend_set_hinting_mode(rend,FT_LOAD_NO_HINTING);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_hinting_default(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend_set_hinting_mode(rend,0);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_hinting_force_autohint(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend_set_hinting_mode(rend,FT_LOAD_FORCE_AUTOHINT);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_hinting_no_autohint(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend_set_hinting_mode(rend,FT_LOAD_NO_AUTOHINT);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_antialiasing_on(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend->hinting_target = FT_LOAD_TARGET_NORMAL;
  rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
  rend->render_mode = FT_RENDER_MODE_NORMAL;
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_antialiasing_off(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend->hinting_target = FT_LOAD_TARGET_MONO;
  rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
  rend->render_mode = FT_RENDER_MODE_MONO;
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


/*void rend_set_render_mode_light(GLYPH_REND* const rend)
{
  if (!rend) return;
  rend->hinting_target = FT_LOAD_TARGET_NORMAL;
  rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
  rend->render_mode = FT_RENDER_MODE_LIGHT;
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}*/


void gk_rend_set_undefined_char(GLYPH_REND* const rend,const unsigned new_undefined_char)
{
  if (rend) rend->undefined_char = (new_undefined_char<=GK_MAX_UNICODE) ? new_undefined_char : 0;
}


void gk_rend_set_error_char(GLYPH_REND* const rend,const unsigned new_error_char)
{
  if (rend) rend->error_char = (new_error_char<=GK_MAX_UNICODE) ? new_error_char : 0;
}


static void rend_remove_size_object(GLYPH_REND* const rend)
{
  CARE(rend);

  if (rend->size)
  {
    FT_Done_Size(rend->size);
    rend->size = 0;
  }
}


/*
 * If calling to set the same keeper which is already set for this keeper,
 * this function still should uncache all glyphs and set the keeper anew.
 */
void gk_rend_set_keeper(GLYPH_REND* const rend,GLYPH_KEEP* const new_keeper)
{
  if (!rend) return;

  if (rend->index)
  {
    GLYPH_INDEX *index = rend->index;

    /* No need to do anything, appropriate index is already assigned. */
    if (index->keeper == new_keeper && glyph_index_ok_for_renderer(index,rend)) return;

    /* Unlink renderer and index from each other */
    if (index->first_renderer==rend) index->first_renderer = rend->next_for_same_index;
    if (index->last_renderer==rend) index->last_renderer = rend->prev_for_same_index;
    if (rend->prev_for_same_index)
      rend->prev_for_same_index->next_for_same_index = rend->next_for_same_index;
    if (rend->next_for_same_index)
      rend->next_for_same_index->prev_for_same_index = rend->prev_for_same_index;
    rend->index = 0;
    rend->prev_for_same_index = 0;
    rend->next_for_same_index = 0;

    /* If dropped index has no other renderers and doesn't index any glyphs, */
    /* it should be deleted. */
    if (!index->first_renderer && !index->pages) glyph_index_done(index);
  }

  if (new_keeper)
  {
    GLYPH_INDEX *index;
    for (index = new_keeper->first_index; index; index = index->next)
    {
      if (glyph_index_ok_for_renderer(index,rend)) break;
    }

    if (!index) index = glyph_index_create(new_keeper,rend);
    if (!index) return;

    rend->index = index;
    rend->prev_for_same_index = 0;
    rend->next_for_same_index = index->first_renderer;
    if (index->first_renderer) index->first_renderer->prev_for_same_index = rend;
    if (!index->last_renderer) index->last_renderer = rend;
    index->first_renderer = rend;
  }
}


void gk_rend_set_face(GLYPH_REND* const rend,GLYPH_FACE* const new_face)
{
  if (!rend) return;
  if (new_face == rend->face) return;
  rend_remove_size_object(rend);

  if (rend->face)
  {
    if (rend->face->first_renderer==rend)
      rend->face->first_renderer = rend->next_for_same_face;
    if (rend->face->last_renderer==rend)
      rend->face->last_renderer = rend->prev_for_same_face;
    if (rend->prev_for_same_face)
      rend->prev_for_same_face->next_for_same_face = rend->next_for_same_face;
    if (rend->next_for_same_face)
      rend->next_for_same_face->prev_for_same_face = rend->prev_for_same_face;
    rend->face = 0;
    rend->prev_for_same_face = 0;
    rend->next_for_same_face = 0;
  }

  if (!new_face || !new_face->face) return;
  rend->face = new_face;

  /* creating new size object and setting initial size */
  {
    FT_New_Size(rend->face->face,&rend->size);
    if (!rend->size) { rend->face = 0; _gk_msg("Error: gk_rend_set_face(): can't create size object\n"); return; }
    if (!gk_rend_set_size_subpixel(rend,rend->hsize,rend->vsize))
      { rend_remove_size_object(rend); rend->face = 0; _gk_msg("Error: gk_rend_set_face(): can't set initial size\n"); return; }
  }

  rend->face_id = rend->face->id;

  rend->prev_for_same_face = 0;
  rend->next_for_same_face = rend->face->first_renderer;
  if (rend->next_for_same_face) rend->next_for_same_face->prev_for_same_face = rend;
  if (!rend->face->last_renderer) rend->face->last_renderer = rend;
  rend->face->first_renderer = rend;

  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


int gk_rend_set_size_subpixel(GLYPH_REND* const rend,const unsigned new_hsize,const unsigned new_vsize)
{
  int error;

  if (!rend || !rend->face || !rend->size) return 0;
  FT_Activate_Size(rend->size);
  error = FT_Set_Char_Size(rend->face->face,new_hsize,new_vsize,72,72);
  if (error)
  {
    _gk_msg("Error: gk_rend_set_size_sublixel(): Can't set font size to %.3fx%.3f pixels\n",((double)new_hsize)/64,((double)new_vsize)/64);
    return 0;
  }
  rend->hsize = new_hsize;
  rend->vsize = new_vsize;
  rend->line_spacing = rend->face->face->size->metrics.height;
  rend->line_spacing_pixels = (rend->line_spacing+63)>>6;
  rend->text_height = gk_rend_ascender(rend) - gk_rend_descender(rend);
  rend->text_height_pixels = ((gk_rend_ascender(rend)+63)>>6) - ((gk_rend_descender(rend)+63)>>6);

  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
  if (rend->target_info) ((FONT*)rend->target_info)->height = rend->text_height_pixels;
#endif

  return 1;
}


int gk_rend_set_size_pixels(GLYPH_REND* const rend,const unsigned new_pixel_hsize,const unsigned new_pixel_vsize)
{
  return gk_rend_set_size_subpixel(rend,new_pixel_hsize<<6,new_pixel_vsize<<6);
}



static void rend_update_matrix(GLYPH_REND* const rend)
{
  rend->do_matrix_transform = (rend->text_angle != 0 || rend->italic_angle != 0);
  if (rend->do_matrix_transform)
  {
    FT_Matrix a;
    rend->matrix.xx = 0x10000;
    rend->matrix.xy = (FT_Fixed)(sin(rend->italic_angle)*(GK_SQRT2*0x10000));
    rend->matrix.yx = 0;
    rend->matrix.yy = 0x10000;
    a.xx = (FT_Fixed)( cos(rend->text_angle)*0x10000);
    a.xy = (FT_Fixed)(-sin(rend->text_angle)*0x10000);
    a.yx = (FT_Fixed)( sin(rend->text_angle)*0x10000);
    a.yy = (FT_Fixed)( cos(rend->text_angle)*0x10000);
    FT_Matrix_Multiply(&a,&rend->matrix);
  }
}


void gk_rend_set_angle(GLYPH_REND* const rend,const double new_text_angle)
{
  if (!rend) return;
  rend->text_angle = new_text_angle;
  rend_update_matrix(rend);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


void gk_rend_set_italic(GLYPH_REND* const rend,const int new_italic_angle)
{
  if (!rend) return;
  if (new_italic_angle < -45 || new_italic_angle > 45) return;
  rend->italic_angle = (double)(new_italic_angle)*GK_PI/180;
  rend_update_matrix(rend);
  if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
}


/*
 * Compress monochrome glyph with RLE7 to ensure fastest output.
 */
#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
static unsigned _gk_make_RLE7(const unsigned char* const src,const int width,const int height,const int pitch)
{
  unsigned char *rlepos;
  int x, y;
  unsigned max_rlesize = width*height+1;

  if (rle_buffer_size < max_rlesize)
  {
    if (rle_buffer) { free(rle_buffer); rle_buffer = 0; }
    rle_buffer_size = 0;
    rle_buffer = (unsigned char*)malloc(max_rlesize*2);
    if (!rle_buffer) return 0;
    rle_buffer_size = max_rlesize*2;
  }
  rlepos = rle_buffer;
  *rlepos++ = GLYPH_MONO_RLE7;

  for (y=0; y<height; y++)
  {
    const unsigned char* s = src + y*pitch;
    unsigned char mask = 64;
    unsigned char run = 1;
    unsigned char color = ((*s)>>7) ? 255 : 0;
    for (x=1; x<width; x++)
    {
      unsigned char a = (*s&mask) ? 255 : 0;
      if (color==a && run<126) { run++; }
      else { *rlepos++ = (color&128)|run; color = a; run = 1; }
      mask >>= 1;
      if (!mask) { mask = 128; s++; }
    }
    *rlepos++ = (color&128)|run;
  }
  return rlepos - rle_buffer;
}
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */


/*
 * RLE-compress anti-aliased glyph.
 */
#ifdef GLYPH_TARGET_KNOWS_RLEAA
static unsigned _gk_make_RLEAA(const unsigned char* const src,const int width,const int height)
{
  unsigned char *rlepos;
  const unsigned char *srcpos, *srcend;
  unsigned max_rlesize = 1+2*width*height;

  if (rle_buffer_size < max_rlesize)
  {
    if (rle_buffer) { free(rle_buffer); rle_buffer = 0; }
    rle_buffer_size = 0;
    rle_buffer = (unsigned char*)malloc(max_rlesize*2);
    if (!rle_buffer) return 0;
    rle_buffer_size = max_rlesize*2;
  }

  rlepos = rle_buffer;
  *rlepos++ = GLYPH_RLEAA;
  srcpos = src;
  srcend = src + width*height;

  while (srcpos < srcend)
  {
    const unsigned char *lineend = srcpos + width;
    while (srcpos < lineend)
    {
      if (*srcpos>0 && *srcpos<255) { *rlepos++ = *srcpos++; }
      else
      {
        int n = 1, c = *srcpos++;
        *rlepos++ = c;
        while (srcpos<lineend && *srcpos==c && n<255) { n++; srcpos++; }
        *rlepos++ = n;
      }
    }
  }

  return rlepos - rle_buffer;
}
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */


static GLYPH* rend_render(GLYPH_REND* const rend,const unsigned unicode);


/*
 * Renders a glyph immediately, then tries to puts it into cache.
 * rend and rend->face must be not 0
 */
static GLYPH* rend_workout(GLYPH_REND* const rend,const unsigned unicode)
{
  unsigned glyph_index;
  int bmp_size;
  int error;
  FT_GlyphSlot slot;
  GLYPH* glyph;

  CARE(rend);
  CARE(rend->face);
  CARE(rend->face->face);
  CARE(unicode);
  CARE(unicode<=GK_MAX_UNICODE);

  funcname = "rend_workout()";

  if (!rend->size) return 0;

  glyph_index = FT_Get_Char_Index(rend->face->face,unicode);
  if (!glyph_index)
  {
    if (unicode==rend->undefined_char || unicode==rend->error_char) return 0;
    else return rend_render(rend,rend->undefined_char);
  }

  /* Preparing for glyph loading: setting size */
  /* FT_Activate_Size(size); */
  rend->face->face->size = rend->size;

  /* Preparing for glyph loading: setting transformation matrix for rotation */
  if (rend->do_matrix_transform)
  {
    rend->face->face->internal->transform_matrix = rend->matrix;
    rend->face->face->internal->transform_flags = 1;
  }
  else rend->face->face->internal->transform_flags = 0;

  /* loading glyph */
  error = FT_Load_Glyph(rend->face->face,glyph_index,rend->load_flags);
  if (error) _gk_msg("Error: rend_workout(): FreeType can't load glyph (#%d) for character U+%04X\n",glyph_index,unicode);

  if (!error)
  {
    slot = rend->face->face->glyph;
    if (!slot) { _gk_msg("Error: rend_workout(): FreeType returned empty glyph slot from FT_Load_Glyph()\n"); error = 1; }
  }

  if (!error)
  {
    error = FT_Render_Glyph(slot,rend->render_mode);
    if (error) _gk_msg("Error: rend_workout(): FreeType can't render glyph for character U+%04X\n",unicode);
  }

  if (!error)
  {
    error = (slot->format!=FT_GLYPH_FORMAT_BITMAP);
    if (error) _gk_msg("Error: rend_workout(): Glyph's 'slot->format' is not FT_GLYPH_FORMAT_BITMAP after rendering with FT_Render_Glyph()\n");
  }

  if (!error)
  {
    error = (slot->bitmap.pitch < 0);
    if (error) _gk_msg("Error: rend_workout(): Rendered glyph has negative pitch value, can't handle\n"); /* FIXME */
  }

  if (!error)
  {
    glyph = (GLYPH*)_gk_malloc(sizeof(GLYPH));
    error = (glyph==0);
  }

  if (error)
  {
    if (unicode==rend->error_char) return 0;
    else return rend_render(rend,rend->error_char);
  }

  glyph->unicode = unicode;
  glyph->width = slot->bitmap.width;
  glyph->height = slot->bitmap.rows;
  glyph->left = slot->bitmap_left;
  glyph->top = slot->bitmap_top;
  glyph->advance_x = slot->advance.x;
  glyph->advance_y = slot->advance.y;

  glyph->index = 0;
  glyph->prev = 0;
  glyph->next = 0;
  glyph->bmp = 0;
  glyph->bmpsize = 0;

  _gk_msg("rendering character '%c'\n",unicode);

  bmp_size = glyph->width*glyph->height;
  if (!bmp_size) /* empty glyph, like space (' ') character */
  {
    if (rend->index) glyph_index_add_glyph(rend->index,glyph);
    return glyph;
  }

  if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
  {
#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
    int pitch1 = (glyph->width+7)>>3;
    int bitpack_size = glyph->height*pitch1;

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    int rle_size = 0;

    if (rend->index)
      rle_size = _gk_make_RLE7(slot->bitmap.buffer,glyph->width,glyph->height,slot->bitmap.pitch);

    if (rle_size>0 && rle_size<=bitpack_size)
    {
      glyph->bmp = (unsigned char*)_gk_malloc(rle_size);
      if (!glyph->bmp) { free(glyph); return 0; }
      memcpy(glyph->bmp,rle_buffer,rle_size);
      glyph->bmpsize = rle_size;
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
      glyph->bmp = (unsigned char*)_gk_malloc(bitpack_size+1);
      if (!glyph->bmp) { free(glyph); return 0; }
      glyph->bmp[0] = GLYPH_MONO_BITPACK;
      if (slot->bitmap.pitch == pitch1)
        memcpy(glyph->bmp+1,slot->bitmap.buffer,bitpack_size);
      else
      {
        unsigned char *d = glyph->bmp+1;
        int y = 0;
        for (; y<glyph->height; y++,d+=pitch1) memcpy(d,slot->bitmap.buffer+y*slot->bitmap.pitch,pitch1);
      }
      glyph->bmpsize = bitpack_size+1;
    }
#else  /* GLYPH_TARGET_KNOWS_MONO_BITPACK */
    {
      int y = 0;
      unsigned char* b;

      glyph->bmp = (unsigned char*)_gk_malloc(bmp_size+1);
      if (!glyph->bmp) { free(glyph); return 0; }
      glyph->bmp[0] = GLYPH_UNCOMPRESSED;
      glyph->bmpsize = bmp_size+1;
      b = glyph->bmp + 1;

      for (; y<glyph->height; y++)
      {
        int x = 0;
        unsigned char* a = slot->bitmap.buffer + y*slot->bitmap.pitch;
        while (x<glyph->width)
        {
          if (x < glyph->width) { *b++ = *a&0x80 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x40 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x20 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x10 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x08 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x04 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x02 ? 255 : 0; x++; }
          if (x < glyph->width) { *b++ = *a&0x01 ? 255 : 0; x++; }
          a++;
        }
      }
    }
#endif  /* GLYPH_TARGET_KNOWS_MONO_BITPACK */
  }
  else
  {
#ifdef GLYPH_TARGET_KNOWS_RLEAA
    int rle_size = 0;
#endif

    /* FIXME: handle case when slot->bitmap.num_grays is not 256 */
    CARE(slot->bitmap.num_grays==256);

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (rend->index)
      rle_size = _gk_make_RLEAA(slot->bitmap.buffer,glyph->width,glyph->height);

    if (rle_size>0 && rle_size<bmp_size)
    {
      glyph->bmp = (unsigned char*)_gk_malloc(rle_size);
      if (!glyph->bmp) { free(glyph); return 0; }
      memcpy(glyph->bmp,rle_buffer,rle_size);
      glyph->bmpsize = rle_size;
    }
    else
#endif
    {
      glyph->bmp = (unsigned char*)_gk_malloc(bmp_size+1);
      if (!glyph->bmp) { free(glyph); return 0; }
      glyph->bmp[0] = GLYPH_UNCOMPRESSED;
      memcpy(glyph->bmp+1,slot->bitmap.buffer,bmp_size);
      glyph->bmpsize = bmp_size+1;
    }
  }

  if (rend->index) glyph_index_add_glyph(rend->index,glyph);
  return glyph;
}


static GLYPH* rend_render(GLYPH_REND* const rend,const unsigned unicode)
{
  GLYPH *glyph = 0;

  if (!unicode || unicode>GK_MAX_UNICODE) return 0;
  if (!rend || !rend->face) return 0;

  /* First look in cache */
  if (rend->index) glyph = glyph_index_find_glyph(rend->index,unicode);
  /* Then try to render */
  if (!glyph) glyph = rend_workout(rend,unicode);

  return glyph;
}


void gk_rend_debug(const GLYPH_REND* const rend)
{
  _gk_msg("GLYPH_REND object (%d):\n",rend);
  if (!rend) return;

  if (rend->face)
    _gk_msg("   there is a font_face object associated, id is %d\n",rend->face->id);
  else
    _gk_msg("   no font_face object associated\n");

  if (rend->size)
  {
    _gk_msg("   there is a size object associated:\n");
    _gk_msg("      EM size in pixels is %dx%d\n",rend->size->metrics.x_ppem,rend->size->metrics.y_ppem);
    _gk_msg("      ascender is %d\n",rend->size->metrics.ascender>>6);
    _gk_msg("      descender is %d\n",rend->size->metrics.descender>>6);
    _gk_msg("      line interval is %d\n",rend->size->metrics.height>>6);
  }
  else _gk_msg("   no size object associated\n");

  if (rend->index)
  {
    _gk_msg("   there is a GLYPH_INDEX / GLYPH_KEEP object associated:\n");
    _gk_msg("   GLYPH_INDEX object (%d):\n",rend->index);
    _gk_msg("      index size is %d bytes\n",glyph_index_size(rend->index));
    _gk_msg("      rend->prev_for_same_index is %d\n",rend->prev_for_same_index);
    _gk_msg("      rend->next_for_same_index is %d\n",rend->next_for_same_index);
    _gk_msg("   GLYPH_KEEP object (%d):\n",rend->index->keeper);
    if (gk_keeper_glyph_limit(rend->index->keeper)) 
      _gk_msg("      keeping %d glyphs, can keep %d glyphs maximum\n",
        gk_keeper_glyph_count(rend->index->keeper),gk_keeper_glyph_limit(rend->index->keeper));
    else
      _gk_msg("      keeping %d glyphs, not limited in glyph number\n",
        gk_keeper_glyph_count(rend->index->keeper));

    if (gk_keeper_byte_limit(rend->index->keeper))
      _gk_msg("      using %d bytes of memory, can use %d bytes maximum\n",
        gk_keeper_byte_count(rend->index->keeper),gk_keeper_byte_limit(rend->index->keeper));
    else
      _gk_msg("      using %d bytes of memory, not limited in memory usage\n",
        gk_keeper_byte_count(rend->index->keeper));
  }
  else _gk_msg("   no GLYPH_INDEX / GLYPH_KEEP object associated\n");

  _gk_msg("   ascender is %.3f pixels\n",((double)gk_rend_ascender(rend))/64);
  _gk_msg("   descender is %.3f pixels\n",((double)gk_rend_descender(rend))/64);
  _gk_msg("   line spacing is %.3f ~ %d pixels\n",
    ((double)(rend->line_spacing))/64,rend->line_spacing_pixels);
  _gk_msg("   text_height is %.3f ~ %d pixels\n",
    ((double)(rend->text_height))/64,rend->text_height_pixels);
}


int gk_rend_has_character(const GLYPH_REND* const rend,const unsigned code)
{
  if (!rend || !rend->face || !rend->face->face || code>GK_MAX_UNICODE || !code) return 0;
  if (FT_Get_Char_Index(rend->face->face,code)!=0) return 1;
  else return 0;
}


int gk_rend_has_glyph(GLYPH_REND* const rend,const unsigned code)
{
  GLYPH *glyph;
  int has_glyph;

  if (!gk_rend_has_character(rend,code)) return 0;
  glyph = rend_render(rend,code);
  if (!glyph) return 0;

  has_glyph = (glyph->bmp) ? 1 : 0;
  if (!glyph->index) unload_glyph(glyph);
  return has_glyph;
}


GLYPH_REND* gk_create_renderer(GLYPH_FACE* const face,GLYPH_KEEP* const keeper)
{
  GLYPH_REND *rend;

  rend = (GLYPH_REND*)malloc(sizeof(GLYPH_REND));
  if (!rend) return 0;

  rend->face = 0;
  rend->size = 0;
  rend->index = 0;

  rend->hinting_mode = 0;
  rend->hinting_target = FT_LOAD_TARGET_NORMAL;
  rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
  rend->render_mode = FT_RENDER_MODE_NORMAL;

  rend->hsize = rend->vsize = 16<<6;
  rend->undefined_char = '?';
  rend->error_char = 0;
  rend->line_spacing = rend->line_spacing_pixels = 0;
  rend->text_height = rend->text_height_pixels = 0;
  rend->text_angle = 0;
  rend->italic_angle = 0;
  rend->do_matrix_transform = 0;
  rend->matrix.xx = 0x10000l;
  rend->matrix.xy = 0;
  rend->matrix.yx = 0;
  rend->matrix.yy = 0x10000l;
  rend->text_alpha_color = 0xFF0000FF;
  rend->back_color = -1;
  rend->prev_for_same_face = rend->next_for_same_face = 0;
  rend->prev_for_same_index = rend->next_for_same_index = 0;
  rend->target_info = 0;

  gk_rend_set_face(rend,face);
  gk_rend_set_keeper(rend,keeper);
  rend->next = 0;
  rend->prev = last_renderer;
  if (last_renderer) last_renderer->next = rend;
  if (!first_renderer) first_renderer = rend;
  last_renderer = rend;

  _gk_install_exit_handler();

  _gk_msg("glyph renderer created\n");
  return rend;
}


void gk_done_renderer(GLYPH_REND* const rend)
{
  if (!rend) return;

  gk_rend_set_keeper(rend,0);
  gk_rend_set_face(rend,0);

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
  if (rend->target_info) free(rend->target_info);
#endif

  if (rend==first_renderer) first_renderer = rend->next;
  if (rend==last_renderer) last_renderer = rend->prev;
  if (rend->next) rend->next->prev = rend->prev;
  if (rend->prev) rend->prev->next = rend->next;

  free(rend);
  _gk_msg("glyph renderer destroyed\n");
}

