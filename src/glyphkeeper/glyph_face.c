/*
 * glyph_face.c  -  Glyph Keeper routines dealing with the font face objects.
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

static unsigned face_count = 0;          /* number of font_faces currently loaded in memory */
static unsigned face_first_free_id = 1;  /* first id number available */
static FT_Library ftlib = 0;
static GLYPH_FACE *first_face = 0, *last_face = 0;
static const char na[] = "UNAVAILABLE";


const char* gk_face_family(const GLYPH_FACE* const f)
{
  return (f && f->face && f->face->family_name) ? f->face->family_name : na;
}


const char* gk_face_style(const GLYPH_FACE* const f)
{
  return (f && f->face && f->face->style_name) ? f->face->style_name : na;
}


const char* gk_face_postscript_name(const GLYPH_FACE* const f)
{
  const char* ps_name;
  if (!f || !f->face) return na;
  ps_name = FT_Get_Postscript_Name(f->face);
  return ps_name ? ps_name : na;
}


const char* gk_face_driver_name(const GLYPH_FACE* const f)
{
  return (f && f->face && f->face->driver && f->face->driver->root.clazz && f->face->driver->root.clazz->module_name) ?
    f->face->driver->root.clazz->module_name : na;
}


int gk_face_is_horizontal(const GLYPH_FACE* const f)
{
  return (f && f->face) ? FT_HAS_HORIZONTAL(f->face) : 0;
}


int gk_face_is_vertical(const GLYPH_FACE* const f)
{
  return (f && f->face) ? FT_HAS_VERTICAL(f->face) : 0;
}


int gk_face_ascender(const GLYPH_FACE* const f)
{
  return (f && f->face) ? f->face->ascender : 0;
}


int gk_face_descender(const GLYPH_FACE* const f)
{
  return (f && f->face) ? f->face->descender : 0;
}


int gk_face_line_spacing(const GLYPH_FACE* const f)
{
  return (f && f->face) ? f->face->height : 0;
}


int gk_face_number_of_glyphs(const GLYPH_FACE* const f)
{
  return (f && f->face) ? f->face->num_glyphs : 0;
}


/* This code is slow, but theoretically more safe */
/*int gk_face_number_of_characters_safe(const GLYPH_FACE* const f)
{
  unsigned i,num=0;
  if (!f || !f->face) return 0;
  for (i=0; i<=GK_MAX_UNICODE; i++)
    if (FT_Get_Char_Index(f->face,i)) num++;
  return num;
}*/


int gk_face_number_of_characters(GLYPH_FACE* const f)
{
  if (!f || !f->face) return 0;

  if (f->number_of_characters < 0)
  {
    FT_ULong charcode;
    FT_UInt gindex;
    unsigned num = 0;

    charcode = FT_Get_First_Char(f->face,&gindex);
    while (gindex != 0)
    {
      num++;
      charcode = FT_Get_Next_Char(f->face,charcode,&gindex);
    }
    f->number_of_characters = num;
    return num;
  }
  else
  {
    return f->number_of_characters;
  }
}


GLYPH_FACE* gk_create_empty_face()
{
  GLYPH_FACE* f;
  funcname = "gk_create_empty_face()";

  f = (GLYPH_FACE*)_gk_malloc(sizeof(GLYPH_FACE));
  if (!f) return 0;
  f->face = 0;
  f->remap = 0;
  f->number_of_characters = -1;
  face_count++;
  f->id = face_first_free_id++;
  f->first_renderer = 0;
  f->last_renderer = 0;
  f->next = 0;
  f->prev = last_face;
  if (last_face) last_face->next = f;
  if (!first_face) first_face = f;
  last_face = f;

  return f;
}


GLYPH_FACE* gk_load_face_from_file(const char* const fname,const int face_index)
{
  GLYPH_FACE* f;
  char buf[1000] = "";
  funcname = "gk_load_face_from_file()";

  if (!fname || !*fname || face_index<0) return 0;

  /* Trying to initialize FreeType. */
  if (!ftlib)
  {
    gk_library_init();
    if (!ftlib) { _gk_msg("Error: %s: Can't initialize FreeType\n",funcname); return 0; }
  }

  /* characters that are normally not contained in a filename */
  /*if (strpbrk(fname,"\n\r\t\f\v\"*"))
    { _gk_msg("Error: %s: Can't load font face: invalid filename \"%s\"\n",funcname,fname); return 0; }*/

  /* Checking if the file exists and is available to open */
  {
    FILE* file = fopen(fname,"rb");
    if (!file && _gk_font_path && *_gk_font_path)
    {
      int fname_length = strlen(fname);
      char *c = _gk_font_path;
      while (!file && c && *c)
      {
        int dirname_length;
        char *c2 = strchr(c,';');
        if (!c2) c2 = strchr(c,0);
        dirname_length = c2-c;
        if (dirname_length + fname_length < 1000)
        {
          strncpy(buf,c,dirname_length);
          strncpy(buf+dirname_length,fname,fname_length);
          buf[dirname_length+fname_length] = 0;
#ifdef GLYPH_LOG
          if (glyph_log) fprintf(glyph_log,"Trying to open file :::%s:::\n",buf);
#endif
          file = fopen(buf,"rb");
        }
        if (!*c2) break;
        c = c2+1;
      }
    }
    if (!file) { _gk_msg("Error: %s: Can't find font file \"%s\"\n",funcname,fname); return 0; }
    fclose(file);
  }
  /* _gk_msg("loading font face %d from file \"%s\"\n",face_index,fname); */

  f = (GLYPH_FACE*)_gk_malloc(sizeof(GLYPH_FACE));
  if (!f) return 0;
  f->face = 0;
  f->remap = 0;
  f->number_of_characters = -1;

  {
    int error;
    error = FT_New_Face(ftlib,*buf?buf:fname,face_index,&f->face);

    if (error == FT_Err_Unknown_File_Format)
      { _gk_msg("Error: %s: Can't load font face from \"%s\": unknown file format\n",funcname,*buf?buf:fname); free(f); return 0; }
    if (error)
      { _gk_msg("Error: %s: Can't load font face from \"%s\"\n",funcname,*buf?buf:fname); free(f); return 0; }
  }

  if (!FT_IS_SCALABLE(f->face))
  {
    _gk_msg("Error: %s: Font face is not scalable\n",funcname);
    FT_Done_Face(f->face); free(f); return 0;
  }
  if (FT_Select_Charmap(f->face,ft_encoding_unicode))
  {
    _gk_msg("Error: %s: Font face does not contain Unicode character map\n",funcname);
    FT_Done_Face(f->face); free(f); return 0;
  }

  face_count++;
  f->id = face_first_free_id++;
  f->first_renderer = 0;
  f->last_renderer = 0;
  f->next = 0;
  f->prev = last_face;
  if (last_face) last_face->next = f;
  if (!first_face) first_face = f;
  last_face = f;

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
  f->number_of_ranges = -1;
  f->range_start = 0;
  f->range_end = 0;
#endif

  _gk_msg("font face <id:%d> loaded from file \"%s\"\n",f->id,*buf?buf:fname);

  return f;
}


GLYPH_FACE* gk_load_face_from_memory(const unsigned char* const data,const int size,const int face_index)
{
  GLYPH_FACE* f;
  funcname = "gk_load_face_from_memory()";

  if (!data || !size || face_index<0) return 0;

  if (!ftlib) gk_library_init();
  if (!ftlib) { _gk_msg("Error: %s: Can't initialize FreeType\n",funcname); return 0; }

  f = (GLYPH_FACE*)_gk_malloc(sizeof(GLYPH_FACE));
  if (!f) return 0;
  f->face = 0;
  f->remap = 0;
  f->number_of_characters = -1;

  {
    int error = FT_New_Memory_Face(ftlib,data,size,face_index,&f->face);
    if (error == FT_Err_Unknown_File_Format)
      { _gk_msg("Error: %s: Can't load font face - unknown data format\n",funcname); free(f); return 0; }
    if (error)
      { _gk_msg("Error: %s: Can't load font face\n",funcname); free(f); return 0; }
  }

  if (!FT_IS_SCALABLE(f->face))
  {
    _gk_msg("Error: %s: Font face is not scalable\n",funcname);
    FT_Done_Face(f->face); free(f); return 0;
  }
  if (FT_Select_Charmap(f->face,ft_encoding_unicode))
  {
    _gk_msg("Error: %s: Font face does not contain Unicode character map\n",funcname);
    FT_Done_Face(f->face); free(f); return 0;
  }

  face_count++;
  f->id = face_first_free_id++;
  f->first_renderer = 0;
  f->last_renderer = 0;
  f->next = 0;
  f->prev = last_face;
  if (last_face) last_face->next = f;
  if (!first_face) first_face = f;
  last_face = f;

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
  f->number_of_ranges = -1;
  f->range_start = 0;
  f->range_end = 0;
#endif

  _gk_msg("font face <id:%d> loaded from memory\n",f->id);
  return f;
}


void gk_unload_face(GLYPH_FACE* const f)
{
  unsigned id;

  if (!f) return;
  id = f->id;

  while (f->first_renderer)
  {
    GLYPH_REND* r = f->first_renderer;
    f->first_renderer = f->first_renderer->next_for_same_face;
    gk_rend_set_face(r,0);
  }

  if (f->face) { FT_Done_Face(f->face); f->face = 0; }

  if (f==first_face) first_face = f->next;
  if (f==last_face) last_face = f->prev;
  if (f->next) f->next->prev = f->prev;
  if (f->prev) f->prev->next = f->next;
  face_count--;

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
  if (f->range_start) { free(f->range_start); f->range_start = 0; }
  if (f->range_end) { free(f->range_end); f->range_end = 0; }
#endif

  free(f);
  _gk_msg("font face <id:%d> unloaded\n",id);
}
