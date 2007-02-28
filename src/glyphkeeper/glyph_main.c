/*
 * glyph_main.c  -  Glyph Keeper main routines
 * (initialization/cleanup, rendering text).
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c


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

    FT_Bitmap_New(&_gk_workout_bitmap);

    _gk_install_exit_handler();
}


void gk_library_cleanup(void)
{
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_library_cleanup() begin\n");
#endif

    while (first_renderer) gk_done_renderer(first_renderer);
    while (first_keeper) gk_done_keeper(first_keeper);
    while (first_face) gk_unload_face(first_face);

    if (ftlib)
    {
        if (_gk_workout_bitmap.buffer) { FT_Bitmap_Done(ftlib,&_gk_workout_bitmap); }
        FT_Done_FreeType(ftlib); ftlib = 0;
    }

    if (rle_buffer) { _gk_free(rle_buffer); rle_buffer = 0; }
    rle_buffer_size = 0;

#ifdef GLYPH_DEBUG_FT_MEMORY
    if (_gk_allocated_by_ft || _gk_overhead_by_ft)
        _gk_msg( "Memory leak! FreeType leaked %d + %d bytes!\n", _gk_allocated_by_ft, _gk_overhead_by_ft );
    else
        _gk_msg( "FreeType shutdown clean\n" );
#endif

#ifdef GLYPH_DEBUG_GK_MEMORY
    if (_gk_allocated_by_gk || _gk_overhead_by_gk)
        _gk_msg( "Memory leak! Glyph Keeper leaked %d + %d bytes!\n", _gk_allocated_by_gk, _gk_overhead_by_gk );
    else 
        _gk_msg("Glyph Keeper shutdown clean\n");
#endif

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_library_cleanup() end\n");
    if (glyph_log) { fclose(glyph_log); glyph_log = 0; }
#endif
}


/*
 * Checks how many bytes are allocated by Glyph Keeper.
 * (FreeType memory is not counted)
 */
size_t gk_bytes_allocated()
{
    int n = 0;
    GLYPH_FACE *a;
    GLYPH_KEEP *b;
    GLYPH_REND *c;

    for (a = first_face; a; a=a->next) n += a->allocated;
    for (b = first_keeper; b; b=b->next) n += b->allocated;
    for (c = first_renderer; c; c=c->next) n += sizeof(GLYPH_REND);
    n += rle_buffer_size;
    return n;
}


/*
 * Precaching.
 */ 
void gk_precache_char(GLYPH_REND* const rend,const unsigned unicode)
{
    GLYPH *glyph;

    if (!rend || !rend->index) return;

    if (rend->before_rend) gk_precache_char(rend->before_rend,unicode);

    glyph = _gk_rend_render(rend,unicode);
    if (glyph && !glyph->index) _gk_unload_glyph(glyph);

    if (rend->after_rend) gk_precache_char(rend->after_rend,unicode);
}


void gk_precache_range(GLYPH_REND* const rend,
    const unsigned range_start,const unsigned range_end)
{
    unsigned code;

    if (!rend || !rend->index) return;

    for (code=range_start; code<=range_end; code++) gk_precache_char(rend,code);
}


void gk_precache_set_utf8(GLYPH_REND* const rend,const char* const char_set)
{
    const char* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    while ( (code = _gk_utf8_getx(&c)) ) gk_precache_char(rend,code);
}


void gk_precache_set_utf16(GLYPH_REND* const rend,const unsigned short* const char_set)
{
    const unsigned short* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    _gk_utf16_start_decoding(&c);
    while ( (code = _gk_utf16_decode(&c)) ) gk_precache_char(rend,code);
}


void gk_precache_set_utf32(GLYPH_REND* const rend,const unsigned* const char_set)
{
    const unsigned* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    _gk_utf32_start_decoding(&c);
    while ( (code = _gk_utf32_decode(&c)) ) gk_precache_char(rend,code);
}


/*
 * Draws background rectangle.
 * pen_x and pen_y are in puxels.
 * adv_x and adv_y should not be both zero.
 */
#ifdef GLYPH_TARGET_HAS_RECTFILL
static void _gk_draw_background_rectangle(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
  const int pen_x,const int pen_y,const int adv_x,const int adv_y)
{
    if (rend->text_angle != 0)
    {
#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED
        int xlt,ylt,xlb,ylb,xrt,yrt,xrb,yrb;
        _gk_text_rectangle_by_advance(rend,adv_x,adv_y,&xlt,&ylt,&xlb,&ylb,&xrt,&yrt,&xrb,&yrb);
        _gk_driver_rectfill_angled(bmp,xlt+pen_x,ylt+pen_y,xrt+pen_x,yrt+pen_y,
                                       xrb+pen_x,yrb+pen_y,xlb+pen_x,ylb+pen_y,rend->back_color);
#endif
    }
    else
    {
        int ascender = gk_rend_ascender(rend);
        int descender = gk_rend_descender(rend);
        int xl = pen_x << 6;
        int xr = xl + adv_x;

        if (rend->italic_angle != 0)
        {
            int da = ascender ? (tan(rend->italic_angle) * ascender) : 0;
            int dd = descender ? (tan(rend->italic_angle) * descender) : 0;

            if (rend->italic_angle > 0)
            {
                xl += dd;
                xr += da;
            }
            else
            {
                xl += da;
                xr += dd;
            }
        }

        /*_gk_driver_rectfill(bmp,pen_x,pen_y-(ascender>>6),pen_x+((adv_x+63)>>6),pen_y-(descender>>6),rend->back_color);*/
        _gk_driver_rectfill(bmp,(xl+31)>>6,pen_y-(ascender>>6),(xr+31)>>6,pen_y-(descender>>6),rend->back_color);
    }
}
#endif


void gk_put_char(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
    const unsigned unicode,const int x,const int y)
{
    GLYPH *glyph = 0;
    if (!bmp || !rend) return;

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        if (rend->before_rend) gk_put_char_center(bmp,rend->before_rend,unicode,
            x + glyph->width/2 + rend->before_dx, y + glyph->height/2 + rend->before_dy);

        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph,x,y);
        _gk_done_drawing();

        if (rend->after_rend) gk_put_char_center(bmp,rend->after_rend,unicode,
            x + glyph->width/2 + rend->after_dx, y + glyph->height/2 + rend->after_dy);
    }
}


void gk_put_char_center(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,const unsigned unicode,
    const int x,const int y)
{
    GLYPH *glyph;
    if (!bmp || !rend) return;

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        if (rend->before_rend)
            gk_put_char_center(bmp,rend->before_rend,unicode,x+rend->before_dx,y+rend->before_dy);

        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph, x - glyph->center_x + glyph->left, y + glyph->center_y - glyph->top);
        _gk_done_drawing();

        if (rend->after_rend)
            gk_put_char_center(bmp,rend->after_rend,unicode,x+rend->after_dx,y+rend->after_dy);
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

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_internal_render_char()\n");
#endif

    glyph = _gk_rend_render(rend,unicode);
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
    GLYPH* glyph;
    int adv_x,adv_y;
    int draw_x,draw_y;

    if (!bmp || !rend || !pen_x || !pen_y) return;

    glyph = _gk_rend_render(rend,unicode);
    if (!glyph) return;

    adv_x = glyph->advance_x;
    adv_y = glyph->advance_y;

    draw_x = ((*pen_x+31)>>6)+glyph->left;
    draw_y = ((*pen_y+31)>>6)-glyph->top;

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_char_advance(rend,unicode,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,*pen_x,*pen_y,ax,ay);
    }
#endif

    if (rend->before_rend) gk_put_char_center(bmp,rend->before_rend,unicode,
        ((*pen_x+31)>>6) + glyph->center_x + rend->before_dx, ((*pen_y+31)>>6) - glyph->center_y + rend->before_dy);

    _gk_prepare_to_draw(bmp,rend);
    _gk_put_glyph(glyph,draw_x,draw_y);
    _gk_done_drawing();

    if (rend->after_rend) gk_put_char_center(bmp,rend->after_rend,unicode,
        ((*pen_x+31)>>6) + glyph->center_x + rend->after_dx, ((*pen_y+31)>>6) - glyph->center_y + rend->after_dy);

    *pen_x += adv_x;
    *pen_y -= adv_y;
}


void gk_render_char_center(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const unsigned unicode,
    const int x,const int y)
{
    GLYPH *glyph;
    if (!bmp || !rend) return;
    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        int asc = gk_rend_ascender_pixels(rend);
        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph,x-glyph->width/2,y - rend->text_height_pixels/2 + asc - glyph->top);
        _gk_done_drawing();
    }
}





void gk_render_line_utf8(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const char* const text,
    const int pen_x,const int pen_y)
{
    int x,y;
    const char* c = text;
    unsigned code;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_render_line_utf8() begin\n");
#endif

    if (!bmp || !rend || !text) return;

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

    if (rend->before_rend || rend->after_rend)
    {
        while ( (code = _gk_utf8_getx(&c)) ) gk_render_char(bmp,rend,code,&x,&y);
    }
    else
    {
        _gk_prepare_to_draw(bmp,rend);
        while ( (code = _gk_utf8_getx(&c)) ) _gk_internal_render_char(rend,code,&x,&y);
        _gk_done_drawing();
    }

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_render_line_utf8() end\n");
#endif
}


void gk_render_line_utf16(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const unsigned short* const text,
    const int pen_x,const int pen_y)
{
    int x,y;
    const unsigned short* c = text;
    unsigned code;

    if (!bmp || !rend || !text) return;

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_text_advance_subpixel_utf16(rend,text,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
    }
#endif

    x = pen_x<<6;
    y = pen_y<<6;
    _gk_utf16_start_decoding(&c);

    if (rend->before_rend || rend->after_rend)
    {
        while ( (code = _gk_utf16_decode(&c)) ) gk_render_char(bmp,rend,code,&x,&y);
    }
    else
    {
        _gk_prepare_to_draw(bmp,rend);
        while ( (code = _gk_utf16_decode(&c)) ) _gk_internal_render_char(rend,code,&x,&y);
        _gk_done_drawing();
    }
}


void gk_render_line_utf32(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const unsigned* const text,
  const int pen_x,const int pen_y)
{
    int x,y;
    const unsigned* c = text;
    unsigned code;

    if (!bmp || !rend || !text) return;

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_text_advance_subpixel_utf32(rend,text,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
    }
#endif

    x = pen_x<<6;
    y = pen_y<<6;
    _gk_utf32_start_decoding(&c);

    if (rend->before_rend || rend->after_rend)
    {
        while ( (code = _gk_utf32_decode(&c)) ) gk_render_char(bmp,rend,code,&x,&y);
    }
    else
    {
        _gk_prepare_to_draw(bmp,rend);
        while ( (code = _gk_utf32_decode(&c)) ) _gk_internal_render_char(rend,code,&x,&y);
        _gk_done_drawing();
    }
}


#endif  /* included_from_glyph_c */
