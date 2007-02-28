/*
 * glyph_to_allegro.c  -  part of Glyph Keeper Allegro driver.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c

#define included_from_glyph_to_allegro_c



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
    /*if (glyph_log) fprintf(glyph_log,"prepare_to_draw() begin\n");*/
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
    /*if (glyph_log) fprintf(glyph_log,"prepare_to_draw() end\n");*/
#endif
}


static void _gk_done_drawing()
{
#ifdef GLYPH_LOG
    /*if (glyph_log) fprintf(glyph_log,"done_drawing() begin\n");*/
#endif

    if (bitmap_acquired) { release_bitmap(bmp); bitmap_acquired = 0; }

#ifdef GLYPH_LOG
    /*if (glyph_log) fprintf(glyph_log,"done_drawing() end\n");*/
#endif
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

    if (!glyph->index) _gk_unload_glyph(glyph);

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
        GLYPH* glyph = _gk_rend_render(rend,code);
        if (glyph)
        {
            px += glyph->advance_x;
            if (!glyph->index) _gk_unload_glyph(glyph);
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
        GLYPH* glyph = _gk_rend_render(rend,code);
        if (glyph)
        {
            px += glyph->advance_x;
            py -= glyph->advance_y;
            if (!glyph->index) _gk_unload_glyph(glyph);
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

    /*glyph = _gk_rend_render(rend,ch);
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
        pen_x += (int)(r->text_angle_sin*asc);
        pen_y += (int)(r->text_angle_cos*asc);
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


#if MAKE_VERSION(ALLEGRO_VERSION,ALLEGRO_SUB_VERSION,ALLEGRO_WIP_VERSION) > MAKE_VERSION(4,1,18)

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
    0,
    0
};

#elif MAKE_VERSION(ALLEGRO_VERSION,ALLEGRO_SUB_VERSION,ALLEGRO_WIP_VERSION) == MAKE_VERSION(4,1,18)

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

#else  /* MAKE_VERSION(...) < MAKE_VERSION(4,1,18) */

static struct FONT_VTABLE font_vtable_gk =
{
    _gk_vtable_font_height,
    _gk_vtable_char_length,
    _gk_vtable_text_length,
    _gk_vtable_render_char,
    _gk_vtable_render,
    _gk_vtable_destroy
};

#endif  /* MAKE_VERSION(...) */


FONT* gk_create_allegro_font(GLYPH_REND* const rend)
{
    FONT* f;

    if (!rend) return 0;
    if (rend->target_info) return rend->target_info;

    f = (FONT*)malloc(sizeof(FONT));
    if (!f) return 0;
    f->data = rend;
    rend->target_info = f;
    f->height = rend->text_height_pixels;
    f->vtable = &font_vtable_gk;
    return f;
}

#if 0
FONT* gk_create_allegro_bitmap_font_for_range(GLYPH_REND* const rend,
   const int range_start,const int range_end,const int color_depth)
{
    FONT* f;
    double temp_text_angle;
    int temp_undefined_char, temp_error_char;
    int gk_back_color, gk_alpha;
    unsigned gk_text_alpha_color;
    int max_height_up = 0, max_height_down = 0, max_height = 0;
    int max_width_left = 0, max_width_right = 0, max_width = 0;
    int asc, desc;

    int i;
    BITMAP* temp_bmp = 0;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_allegro_bitmap_font_for_range() begin\n");
#endif

    if (!rend) return 0;
    if (range_end < range_start) return 0;

    f = (FONT*)malloc(sizeof(FONT));
    if (!f) return 0;

    f->height = 0;
    f->data = 0;
    f->vtable = 0;

    /* Setting renderer for horizontal text, no error chars. */
    temp_text_angle = rend->text_angle;
    if (rend->text_angle != 0) gk_rend_set_angle_in_radians(rend,0);
    temp_undefined_char = rend->undefined_char;
    gk_rend_set_undefined_char(rend,0);
    temp_error_char = rend->error_char;
    gk_rend_set_error_char(rend,0);

    gk_back_color = gk_rend_get_back_color(rend);
    gk_text_alpha_color = gk_rend_get_text_alpha_color(rend);
    gk_alpha = (gk_text_alpha_color >> 24) & 0xFF;

    asc = gk_rend_ascender_pixels(rend);
    desc = gk_rend_descender_pixels(rend);
    if (asc > max_height_up) max_height_up = asc;
    if (-desc > max_height_down) max_height_down = -desc;

    /* Finding max glyph size. */
    for (i=range_start; i<=range_end; i++)
    {
        if (gk_rend_has_character(rend,i))
        {
            GLYPH* glyph = _gk_rend_render(rend,i);
            if (glyph)
            {
                if (-glyph->left > max_width_left) max_width_left = -glyph->left;
                if (glyph->left + glyph->width > max_width_right) max_width_right = glyph->left + glyph->width;
                if (((glyph->advance_x+31)>>6) > max_width_right) max_width_right = ((glyph->advance_x+31)>>6);

                if (glyph->top > max_height_up) max_height_up = glyph->top;
                if (glyph->height - glyph->top > max_height_down) max_height_down = glyph->height - glyph->top;

                if (glyph && !glyph->index) _gk_unload_glyph(glyph);
            }
        }
    }

    max_width = max_width_left + max_width_right;
    max_height = max_height_up + max_height_down;

    f->height = max_height;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"    max glyph size: %d x %d\n",max_width,max_height);
#endif

    temp_bmp = create_bitmap_ex(32,max_width,max_height);
    if (!temp_bmp) { if (f) free(f); return 0; }

    if ( (rend->render_mode == FT_RENDER_MODE_MONO) && (gk_alpha == 0xFF) )
    {
        FONT_MONO_DATA* first_data = 0;
        FONT_MONO_DATA* prev_data = 0;
        FONT_MONO_DATA* cur_data = 0;
        int c = range_start;

#ifdef GLYPH_LOG
        if (glyph_log) fprintf(glyph_log,"    making mono font\n");
#endif
        _gk_msg("    Creating a monochrome FONT\n");

        gk_rend_set_text_alpha_color(rend,0xFFFFFFFF);
        gk_rend_set_back_color(rend,0);

        while (c <= range_end)
        {
            int begin, end;

            while (c <= range_end && !gk_rend_has_character(rend,c)) c++;
            if (c > range_end) break;
            begin = end = c;
            while (end <= range_end && gk_rend_has_character(rend,end)) end++;

#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"    Range: %d .. %d\n",begin,end-1);
#endif
            /*_gk_msg("        Range: %d .. %d\n",begin,end-1);*/

            cur_data = (FONT_MONO_DATA*)calloc(sizeof(FONT_MONO_DATA),1);
            if (!cur_data) break;

            cur_data->begin = begin;
            cur_data->end = end;
            cur_data->glyphs = (FONT_GLYPH**)calloc(end-begin,sizeof(FONT_GLYPH*));
            cur_data->next = 0;

            for (c = begin; c < end; c++)
            {
                FONT_GLYPH* alglyph = 0; 
                GLYPH* glyph = 0;

#ifdef GLYPH_LOG
                if (glyph_log) fprintf(glyph_log,"        Char: %d\n",c);
#endif
                /*_gk_msg("            Char: %d\n",c);*/

                glyph = _gk_rend_render(rend,c);


                if (glyph)
                {
                    int wl=0,wr=0,w,h,sx,j,k;

#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            rendered glyph: size = %d x %d, pos = (%d,%d)\n",glyph->width,glyph->height,glyph->left,glyph->top);
#endif
                    /*_gk_msg("                rendered %dx%d glyph\n",glyph->width,glyph->height);*/

                    if (-glyph->left > wl) wl = -glyph->left;
                    if (glyph->left + glyph->width > wr) wr = glyph->left + glyph->width;
                    if (((glyph->advance_x+31)>>6) > wr) wr = ((glyph->advance_x+31)>>6);
                    w = wl + wr;
                    h = max_height;
                    sx = ((w + 7) / 8);
#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            output glyph size: %d x %d\n",w,h);
                    if (glyph_log) fprintf(glyph_log,"            glyph data is %d bytes\n",sx*h);
#endif
                    /*_gk_msg("                effective size: %dx%d\n",w,h);*/

                    alglyph = (FONT_GLYPH*)calloc(sizeof(FONT_GLYPH) + sx * h, 1);
                    if (!alglyph)
                    {
#ifdef GLYPH_LOG
                        if (glyph_log) fprintf(glyph_log,"            can't allocate memory for alglyph\n");
#endif
                        exit(1);
                    }
                    alglyph->w = w;
                    alglyph->h = h;
                    /*_gk_msg("                alglyph created\n");*/

                    clear(temp_bmp);
                    /*_gk_msg("                bitmap cleared\n");*/

                    if (glyph->bmp)
                    {
                        /*_gk_msg("                printing a glyph\n");*/
                        _gk_prepare_to_draw(temp_bmp,rend);
#ifdef GLYPH_LOG
                        if (glyph_log) fprintf(glyph_log,"            printing a %dx%d glyph at (%d,%d)\n",glyph->width,glyph->height,glyph->left+wl,max_height_up-glyph->top);
#endif
                        _gk_put_glyph(glyph,glyph->left+wl,max_height_up-glyph->top);
                        _gk_done_drawing();
                        /*_gk_msg("                printed!\n");*/
                    }
                    else
                    {
                        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
                    }

#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            printed\n");
#endif
                    /*_gk_msg("                converting\n");*/

                    for (j=0; j<sx*h; j++) alglyph->dat[j] = 0;
                    /*_gk_msg("                target cleared\n");*/
                    for (j=0; j<h; j++)
                        for (k=0; k<w; k++)
                        {
                            if (getpixel(temp_bmp,k,j))
                                alglyph->dat[(j*sx)+(k/8)] |= 0x80 >> (k & 7);
                        }
#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            converted\n");
#endif
                    /*_gk_msg("                converted!\n");*/

                }
                else
                {
                    int j;
                    _gk_msg("                no glyph!\n");
                    alglyph = (FONT_GLYPH*)malloc(sizeof(FONT_GLYPH) + 8);
                    alglyph->w = 8;
                    alglyph->h = 8;
                    for (j=0; j<8; j++) alglyph->dat[j] = 0;
                }

                cur_data->glyphs[c-begin] = alglyph;
            }

            if (prev_data) prev_data->next = cur_data;
            else { first_data = cur_data; }
            prev_data = cur_data;
            cur_data = 0;
        }

        f->data = first_data;
        f->vtable = font_vtable_mono;
    }
    else
    {
        FONT_COLOR_DATA* first_data = 0;
        FONT_COLOR_DATA* prev_data = 0;
        FONT_COLOR_DATA* cur_data = 0;
        int c = range_start;
        int al_back_color;
        int gk_text_r, gk_text_g, gk_text_b;

        int transparent = (color_depth == 32) && ((rend->back_color < 0) || (gk_alpha < 0xFF));

#ifdef GLYPH_LOG
        if (glyph_log) fprintf(glyph_log,"    making color font\n");
#endif
        if (transparent)
            _gk_msg("    Creating a color transparent FONT\n");
        else
            _gk_msg("    Creating a color opaque FONT\n");

        al_back_color = makecol_depth(color_depth,(gk_back_color>>16)&0xFF,(gk_back_color>>8)&0xFF,gk_back_color&0xFF);

        gk_text_r = (gk_text_alpha_color >> 16) & 0xFF;
        gk_text_g = (gk_text_alpha_color >> 8) & 0xFF;
        gk_text_b = gk_text_alpha_color & 0xFF;


        if (transparent)
        {
            gk_rend_set_text_alpha_color( rend, gk_text_alpha_color | 0xFFFFFF );
            gk_rend_set_back_color(rend,0);
        }

        while (c <= range_end)
        {
            int begin, end;

            while (c <= range_end && !gk_rend_has_character(rend,c)) c++;
            if (c > range_end) break;
            begin = end = c;
            while (end <= range_end && gk_rend_has_character(rend,end)) end++;

#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"    Range: %d .. %d\n",begin,end-1);
#endif
            /*_gk_msg("        Range: %d .. %d\n",begin,end-1);*/

            /*cur_data = (FONT_COLOR_DATA*)calloc(sizeof(FONT_COLOR_DATA),1);*/
            cur_data = (FONT_COLOR_DATA*)_AL_MALLOC(sizeof(FONT_COLOR_DATA));
            if (!cur_data)
            {
                _gk_msg("Can't allocate memory for FONT_COLOR_DATA!\n");
                break;
            }

            cur_data->begin = begin;
            cur_data->end = end;
            /*cur_data->bitmaps = (BITMAP**)calloc(end-begin,sizeof(BITMAP*));*/
            cur_data->bitmaps = (BITMAP**)_AL_MALLOC( (end-begin) * sizeof(BITMAP*) );
            if (!cur_data->bitmaps)
            {
                _gk_msg("Can't allocate memory for array of bitmaps!\n");
                break;
            }
            memset( cur_data->bitmaps, 0, (end-begin) * sizeof(BITMAP*) );
            cur_data->next = 0;

            for (c = begin; c < end; c++)
            {
                BITMAP* albmp = 0; 
                GLYPH* glyph = 0;

#ifdef GLYPH_LOG
                if (glyph_log) fprintf(glyph_log,"        Char: %d\n",c);
#endif
                /*_gk_msg("            Char: %d\n",c);*/

                glyph = _gk_rend_render(rend,c);

                if (glyph)
                {
                    int wl=0,wr=0,w,h;

#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            rendered glyph: size = %d x %d, pos = (%d,%d)\n",glyph->width,glyph->height,glyph->left,glyph->top);
#endif

                    if (-glyph->left > wl) wl = -glyph->left;
                    if (glyph->left + glyph->width > wr) wr = glyph->left + glyph->width;
                    if (((glyph->advance_x+31)>>6) > wr) wr = ((glyph->advance_x+31)>>6);
                    w = wl + wr;
                    h = max_height;
#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            output glyph size: %d x %d\n",w,h);
#endif

                    albmp = create_bitmap_ex(color_depth,w,h);
                    if (!albmp)
                    {
                        _gk_msg("Can't create allegro bitmap!\n");
                        break;
                    }

                    if (transparent)
                        clear_to_color(albmp,0);
                    else
                        clear_to_color(albmp,al_back_color);


                    if (glyph->bmp)
                    {
                        _gk_prepare_to_draw(albmp,rend);
#ifdef GLYPH_LOG
                        if (glyph_log) fprintf(glyph_log,"            printing a %dx%d glyph at (%d,%d)\n",glyph->width,glyph->height,glyph->left+wl,max_height_up-glyph->top);
#endif
                        _gk_put_glyph(glyph,glyph->left+wl,max_height_up-glyph->top);
                        _gk_done_drawing();

                        if (transparent)
                        {
                            int x,y;
                            for (y=0; y<h; y++)
                                for (x=0; x<w; x++)
                                {
                                    unsigned tc = _getpixel32(albmp,x,y);
                                    if (tc)
                                    {
                                        unsigned ta = tc & 0xFF;
                                        ta = ta * gk_alpha / 255;
                                        _putpixel32(albmp,x,y,makeacol32(gk_text_r,gk_text_g,gk_text_b,ta));
                                    }
                                }
                        }
                    }
                    else
                    {
                        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
                    }

#ifdef GLYPH_LOG
                    if (glyph_log) fprintf(glyph_log,"            printed\n");
#endif
                }
                else
                {
                    albmp = create_bitmap_ex(color_depth,8,8);
                    if (!albmp)
                    {
                        _gk_msg("Can't create allegro bitmap!\n");
                        break;
                    }
                    clear_to_color(albmp,al_back_color);
                }

                cur_data->bitmaps[c-begin] = albmp;
            }

            if (prev_data) prev_data->next = cur_data;
            else { first_data = cur_data; }
            prev_data = cur_data;
            cur_data = 0;
        }

        f->data = first_data;
        if (transparent)
            f->vtable = font_vtable_trans;
        else
            f->vtable = font_vtable_color;
    }

    if (temp_text_angle) gk_rend_set_angle_in_radians(rend,temp_text_angle);
    gk_rend_set_undefined_char(rend,temp_undefined_char);
    gk_rend_set_error_char(rend,temp_error_char);
    gk_rend_set_text_alpha_color(rend,gk_text_alpha_color);
    gk_rend_set_back_color(rend,gk_back_color);

    if (temp_bmp) { destroy_bitmap(temp_bmp); temp_bmp = 0; }

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_allegro_bitmap_font_for_range() end\n");
#endif

    return f;
}
#endif

#endif  /* included_from_glyph_c */
