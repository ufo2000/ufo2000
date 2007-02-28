/*
 * glyph_face.c  -  Glyph Keeper routines dealing with the font face objects.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



static void _gk_find_mapping(GLYPH_FACE* const face1,unsigned code1,GLYPH_FACE** face2,unsigned* code2);


const char* gk_face_family(const GLYPH_FACE* const f)
{
    return (f && f->face && f->face->family_name) ? f->face->family_name : _gk_na;
}


const char* gk_face_style(const GLYPH_FACE* const f)
{
    return (f && f->face && f->face->style_name) ? f->face->style_name : _gk_na;
}


const char* gk_face_postscript_name(const GLYPH_FACE* const f)
{
    const char* ps_name;
    if (!f || !f->face) return _gk_na;
    ps_name = FT_Get_Postscript_Name(f->face);
    return ps_name ? ps_name : _gk_na;
}


const char* gk_face_driver_name(const GLYPH_FACE* const f)
{
    if (!f || !f->face) return _gk_na;
/*    return (f && f->face && f->face->driver && f->face->driver->root.clazz &&  f->face->driver->root.clazz->module_name) ?
        f->face->driver->root.clazz->module_name : _gk_na;*/
    return _gk_na;
}


int gk_face_get_number_of_charmaps(const GLYPH_FACE* const f)
{
    if (!f || !f->face) return 0;
    return f->face->num_charmaps;
}


int gk_face_is_scalable(const GLYPH_FACE* const f)
{
    return (f && f->face) ? FT_IS_SCALABLE(f->face) : 0;
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


int gk_face_number_of_own_characters(GLYPH_FACE* const f)
{
    if (!f || !f->face) return 0;

    if (f->number_of_own_characters < 0)
    {
        FT_ULong charcode;
        FT_UInt gindex;
        unsigned num = 0;

        charcode = FT_Get_First_Char(f->face,&gindex);
        while (gindex != 0)
        {
            if (charcode <= 0xD7FF || (charcode >= 0xE000 && charcode <= GK_MAX_UNICODE) ) num++;
            charcode = FT_Get_Next_Char(f->face,charcode,&gindex);
        }
        f->number_of_own_characters = num;
        return num;
    }
    else
    {
        return f->number_of_own_characters;
    }
}


int gk_face_number_of_own_characters_in_range(GLYPH_FACE* const f,const unsigned start,const unsigned end)
{
    FT_ULong charcode;
    FT_UInt gindex;
    unsigned num = 0;

    if (!f || !f->face) return 0;
    if (start>=end) return 0;

    charcode = FT_Get_First_Char(f->face,&gindex);
    while (gindex != 0)
    {
        if (charcode <= 0xD7FF || (charcode >= 0xE000 && charcode <= GK_MAX_UNICODE) )
            if (charcode >= start && charcode < end) num++;
        charcode = FT_Get_Next_Char(f->face,charcode,&gindex);
    }
    return num;
}


int gk_face_number_of_characters(GLYPH_FACE* const face)
{
    int num = 0;

    if (!face) return 0;
    if (!face->remap) return gk_face_number_of_own_characters(face);

    if (face->number_of_own_characters < 0) gk_face_number_of_own_characters(face);
    if (face->number_of_own_characters >= 0) num = face->number_of_own_characters;
    return num + face->remap_increment;
}


int gk_face_has_own_character(GLYPH_FACE* const face,const unsigned code)
{
    if (!face || !face->face) return 0;
    if (code > GK_MAX_UNICODE) return 0;
    if (code >= 0xD800 && code <= 0xDFFF) return 0;
    return (FT_Get_Char_Index(face->face,code) != 0);
}


int gk_face_has_character(GLYPH_FACE* const face,const unsigned code)
{
    GLYPH_FACE* actual_face;
    unsigned actual_code;

    if (!face) return 0;
    if (code > GK_MAX_UNICODE) return 0;

    _gk_find_mapping(face,code,&actual_face,&actual_code);

    if (!actual_face || !actual_face->face) return 0;
    if (actual_code > GK_MAX_UNICODE) return 0;
    if (actual_code >= 0xD800 && actual_code <= 0xDFFF) return 0;
    return (FT_Get_Char_Index(actual_face->face,actual_code) != 0);
}


GLYPH_FACE* gk_create_empty_face()
{
    GLYPH_FACE* f;
    funcname = "gk_create_empty_face()";

    f = (GLYPH_FACE*)_gk_malloc(sizeof(GLYPH_FACE));
    if (!f) return 0;
    f->face = 0;
    f->own_size = 0;
    f->remap = 0;
    f->number_of_own_characters = 0;
    f->remap_increment = 0;
    f->first_renderer = 0;
    f->last_renderer = 0;
    f->allocated = sizeof(GLYPH_FACE);

    face_count++;
    f->id = face_first_free_id++;
    f->next = 0;
    f->prev = last_face;
    if (last_face) last_face->next = f;
    if (!first_face) first_face = f;
    last_face = f;

    return f;
}


GLYPH_FACE* _gk_load_face_from_file(const char* const fname,const int face_index,
    const int gk_header_version, const int gk_header_target )
{
    GLYPH_FACE* f;
    char buf[1000] = "";
    funcname = "gk_load_face_from_file()";

    if (gk_header_version != GK_MAKE_VERSION(_gk_version_major,_gk_version_minor,_gk_version_patch))
    {
        _gk_msg("Error: Glyph Keeper library and header versions don't match\n");
        return 0;
    }

    if (_gk_target != gk_header_target)
    {
        _gk_msg("Error: Glyph Keeper library and your program have different targets (GLYPH_TARGET)\n");
        return 0;
    }

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
            const char *c = _gk_font_path;
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
    f->own_size = 0;
    f->remap = 0;
    f->number_of_own_characters = -1;
    f->remap_increment = 0;
    f->first_renderer = 0;
    f->last_renderer = 0;
    f->allocated = sizeof(GLYPH_FACE);

    {
        int error = FT_New_Face(ftlib,*buf?buf:fname,face_index,&f->face);
        if (error || !f->face)
        {
            if (error == FT_Err_Unknown_File_Format)
                _gk_msg("Error: %s: Can't load font face from \"%s\": unknown file format\n",funcname,*buf?buf:fname);
            else
                _gk_msg("Error: %s: Can't load font face from \"%s\"\n",funcname,*buf?buf:fname);
            _gk_free(f);
            return 0;
        }
    }

    /*if (!FT_IS_SCALABLE(f->face))
    {
        _gk_msg("Error: %s: Font face is not scalable\n",funcname);
        FT_Done_Face(f->face); _gk_free(f); return 0;
    }
    if (FT_Select_Charmap(f->face,ft_encoding_unicode))
    {
        _gk_msg("Error: %s: Font face does not contain Unicode character map\n",funcname);
        FT_Done_Face(f->face); _gk_free(f); return 0;
    }*/

    FT_Select_Charmap(f->face,ft_encoding_unicode);

    f->own_size = f->face->size;
    face_count++;
    f->id = face_first_free_id++;
    f->next = 0;
    f->prev = last_face;
    if (last_face) last_face->next = f;
    if (!first_face) first_face = f;
    last_face = f;

    _gk_msg("font face <id:%d> loaded from file \"%s\"\n",f->id,*buf?buf:fname);
    return f;
}


GLYPH_FACE* _gk_load_face_from_memory(const unsigned char* const data,const int size,const int face_index,
    const int gk_header_version,const int gk_header_target)
{
    GLYPH_FACE* f;
    funcname = "gk_load_face_from_memory()";

    if (gk_header_version != GK_MAKE_VERSION(_gk_version_major,_gk_version_minor,_gk_version_patch))
    {
        _gk_msg("Error: Glyph Keeper library and header versions don't match\n");
        return 0;
    }

    if (_gk_target != gk_header_target)
    {
        _gk_msg("Error: Glyph Keeper library and your program have different targets (GLYPH_TARGET)\n");
        return 0;
    }

    if (!data || !size || face_index<0) return 0;

    if (!ftlib) gk_library_init();
    if (!ftlib) { _gk_msg("Error: %s: Can't initialize FreeType\n",funcname); return 0; }

    f = (GLYPH_FACE*)_gk_malloc(sizeof(GLYPH_FACE));
    if (!f) return 0;
    f->face = 0;
    f->own_size = 0;
    f->remap = 0;
    f->number_of_own_characters = -1;
    f->remap_increment = 0;
    f->first_renderer = 0;
    f->last_renderer = 0;
    f->allocated = sizeof(GLYPH_FACE);

    {
        int error = FT_New_Memory_Face(ftlib,data,size,face_index,&f->face);
        if (error || !f->face)
        {
            if (error == FT_Err_Unknown_File_Format)
                _gk_msg("Error: %s: Can't load font face - unknown data format\n",funcname);
            else
                _gk_msg("Error: %s: Can't load font face\n",funcname);
            _gk_free(f);
            return 0;
        }
    }

    /*if (!FT_IS_SCALABLE(f->face))
    {
        _gk_msg("Error: %s: Font face is not scalable\n",funcname);
        FT_Done_Face(f->face); _gk_free(f); return 0;
    }
    if (FT_Select_Charmap(f->face,ft_encoding_unicode))
    {
        _gk_msg("Error: %s: Font face does not contain Unicode character map\n",funcname);
        FT_Done_Face(f->face); _gk_free(f); return 0;
    }*/

    FT_Select_Charmap(f->face,ft_encoding_unicode);

    f->own_size = f->face->size;
    face_count++;
    f->id = face_first_free_id++;
    f->next = 0;
    f->prev = last_face;
    if (last_face) last_face->next = f;
    if (!first_face) first_face = f;
    last_face = f;

    _gk_msg("font face <id:%d> loaded from memory\n",f->id);
    return f;
}


void gk_unload_face(GLYPH_FACE* const f)
{
    unsigned id;

    if (!f) return;
    id = f->id;

    if (f->remap)
    {
        int a,b;
        for (a=0; a<68; a++)
            if (f->remap[a])
            {
                GLYPH_REMAP** page_b = f->remap[a];
                for (b=0; b<128; b++) if (page_b[b]) _gk_free(page_b[b]);
                _gk_free(page_b);
            }
        _gk_free(f->remap);
    }

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

    _gk_free(f);
    _gk_msg("font face <id:%d> unloaded\n",id);
}


/*
 * Remapping.
 * This function finds a substitution character and font face.
 * 'face1', 'face2' and 'code2' must be not 0.
 * 'code1' must be in range 0..GK_MAX_UNICODE.
 */
static void _gk_find_mapping(GLYPH_FACE* const face1,unsigned code1,GLYPH_FACE** face2,unsigned* code2)
{
    CARE(face1 && face2 && code2);
    CARE(code1<=GK_MAX_UNICODE);

    if (face1->remap)
    {
        GLYPH_REMAP** page_b = face1->remap[code1 >> 14];
        if (page_b)
        {
            GLYPH_REMAP* page_c = page_b[(code1 & 0x3FFF) >> 7];
            if (page_c)
            {
                GLYPH_REMAP* remap_entry = &page_c[code1 & 0x7F];
                if (remap_entry->face)
                {
                    *face2 = remap_entry->face;
                    *code2 = remap_entry->code;
                    return;
                }
            }
        }
    }

    *face2 = face1;
    *code2 = code1;
}


/*
 * Adds a mapping so that whenever face1.code1 character is requested,
 * face2.code2 glyph will be rendered instead.
 */
void gk_remap_character(GLYPH_FACE* const face1,const unsigned code1,GLYPH_FACE* const face2,const unsigned code2)
{
    GLYPH_FACE* new_face = face2;
    unsigned new_code = code2;
    int a_index, b_index;
    int a_size = 0, b_size = 0, c_size = 0;
    GLYPH_REMAP** b_page;
    GLYPH_REMAP* c_page;
    GLYPH_REMAP* remap_entry;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_remap_character(%d,%d,%d,%d) begin\n",
        (int)face1,code1,(int)face2,code2);
#endif

    funcname = "gk_remap_character()";

    if (!face1) return;
    if (code1>GK_MAX_UNICODE) return;
    if (code1>=0xD800 && code1<=0xDFFF) return;

    if (new_code>GK_MAX_UNICODE || (new_code>=0xD800 && new_code<=0xDFFF)) { new_face = 0; new_code = 0; }
    if (!new_face) { gk_unmap_character(face1,code1); return; }

    if (!face1->remap)
    {
        a_size = 68*sizeof(void*) + sizeof(int);
        face1->remap = (GLYPH_REMAP***)_gk_malloc(a_size);
        if (!face1->remap) return;
        memset(face1->remap,0,a_size);
    }

    a_index = code1 >> 14;
    if (!face1->remap[a_index])
    {
        const int b_size = 128*sizeof(void*) + sizeof(int);
        face1->remap[a_index] = (GLYPH_REMAP**)_gk_malloc(b_size);
        if (!face1->remap[a_index])
        {
            if (a_size) { _gk_free(face1->remap); face1->remap = 0; }
            return;
        }
        (*(int*)(face1->remap+68))++;
        memset(face1->remap[a_index],0,b_size);
    }

    b_page = face1->remap[a_index];
    b_index = (code1 & 0x3FFF) >> 7;
    if (!b_page[b_index])
    {
        const int c_size = 128*sizeof(GLYPH_REMAP) + sizeof(int);
        b_page[b_index] = (GLYPH_REMAP*)_gk_malloc(c_size);
        if (!b_page[b_index])
        {
            if (b_size) { _gk_free(b_page); face1->remap[a_index] = 0; (*(int*)(face1->remap+68))--; }
            if (a_size) { _gk_free(face1->remap); face1->remap = 0; }
            return;
        }
        (*(int*)(b_page+128))++;
        memset(b_page[b_index],0,c_size);
    }

    face1->allocated += a_size + b_size + c_size;
    c_page = b_page[b_index];
    remap_entry = &c_page[code1 & 0x7F];
    if (remap_entry->face == 0)
    {
        (*(int*)(c_page+128))++;
        if (!gk_face_has_own_character(face1,code1)) face1->remap_increment++;
    }
    remap_entry->face = new_face;
    remap_entry->code = new_code;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_remap_character(%d,%d,%d,%d) success\n",
        (int)face1,code1,(int)face2,code2);
#endif
}


void gk_unmap_character(GLYPH_FACE* const face,const unsigned code)
{
    GLYPH_REMAP **b_page, *c_page, *remap_entry;
    int a_index, b_index, c_index;

    if (!face || code>GK_MAX_UNICODE) return;
    if (!face->remap) return;

    a_index = code >> 14;
    b_page = face->remap[a_index];
    if (!b_page) return;

    b_index = (code & 0x3FFF) >> 7;
    c_page = b_page[b_index];
    if (!c_page) return;

    c_index = code & 0x7F;
    remap_entry = &c_page[c_index];

    /* Not remapped, nothing to do. */
    if (remap_entry->face == 0) return;

    if (!gk_face_has_own_character(face,code)) face->remap_increment--;

    remap_entry->face = 0;
    remap_entry->code = 0;
    (*(int*)(c_page+128))--;
    if (*(int*)(c_page+128) > 0) return;

    _gk_free(c_page);
    b_page[b_index] = 0;
    face->allocated -= 128*sizeof(GLYPH_REMAP) + sizeof(int);
    (*(int*)(b_page+128))--;
    if (*(int*)(b_page+128) > 0) return;

    _gk_free(b_page);
    face->remap[a_index] = 0;
    face->allocated -= 128*sizeof(void*) + sizeof(int);
    (*(int*)(face->remap+68))--;
    if (*(int*)(face->remap+68) > 0) return;

    _gk_free(face->remap);
    face->remap = 0;
    face->allocated -= 68*sizeof(void*) + sizeof(int);
}


void gk_remap_range(GLYPH_FACE* const face1,const unsigned code1,
    GLYPH_FACE* const face2,const unsigned code2,const unsigned range_size)
{
    unsigned i;
    for (i=0; (i<range_size) && (code1+i<=GK_MAX_UNICODE); i++) gk_remap_character(face1,code1+i,face2,code2+i);
}


void gk_unmap_range(GLYPH_FACE* const face,const unsigned code,const unsigned range_size)
{
    unsigned i;
    for (i=0; (i<range_size) && (code+i<=GK_MAX_UNICODE); i++) gk_unmap_character(face,code+i);
}


#endif  /* included_from_glyph_c */
