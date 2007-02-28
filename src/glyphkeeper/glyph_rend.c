/*
 * glyph_rend.c  -  Glyph Keeper routines dealing with renderer objects.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



#define DEFAULT_LOAD_FLAGS  (FT_LOAD_NO_BITMAP)

#define rend_set_hinting_mode(r,mode)                                                \
{                                                                                    \
    (r)->hinting_mode = (mode);                                                      \
    (r)->load_flags = DEFAULT_LOAD_FLAGS | (r)->hinting_mode | (r)->hinting_target;  \
}


/*
 * This function resets all parameters of a renderer to default values
 * for unscalable font face.
 */
static void _gk_rend_reset_to_fixed(GLYPH_REND* const rend)
{
    CARE(rend);

    rend->hinting_target = FT_LOAD_TARGET_MONO;
    rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
    rend->render_mode = FT_RENDER_MODE_MONO;

    rend->text_angle = 0;
    rend->text_angle_sin = 0;
    rend->text_angle_cos = 1;
    rend->italic_angle = 0;
    rend->bold_strength = 0;
    rend->do_matrix_transform = 0;
    rend->matrix.xx = 0x10000l;
    rend->matrix.xy = 0;
    rend->matrix.yx = 0;
    rend->matrix.yy = 0x10000l;
}




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
    if (!rend->face || !rend->face->face || (rend->face && rend->face->face && FT_IS_SCALABLE(rend->face->face)) )
    {
        rend->hinting_target = FT_LOAD_TARGET_NORMAL;
        rend->load_flags = DEFAULT_LOAD_FLAGS | rend->hinting_mode | rend->hinting_target;
        rend->render_mode = FT_RENDER_MODE_NORMAL;
        if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
    }
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


static void _gk_rend_remove_size_object(GLYPH_REND* const rend)
{
    CARE(rend);

    if (rend->size)
    {
        FT_Done_Size(rend->size);
        rend->size = 0;
    }
}


/*
 * If calling to set the same keeper which is already set for this renderer,
 * this function still should uncache all glyphs and set the keeper anew.
 */
void gk_rend_set_keeper(GLYPH_REND* const rend,GLYPH_KEEP* const new_keeper)
{
    if (!rend) return;

    if (rend->index)
    {
        GLYPH_INDEX *index = rend->index;

        /* No need to do anything, appropriate index is already assigned. */
        if (index->keeper == new_keeper && _gk_glyph_index_ok_for_renderer(index,rend)) return;

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
        if (!index->first_renderer && !index->pages) _gk_glyph_index_done(index);
    }

    if (new_keeper)
    {
        GLYPH_INDEX *index;
        for (index = new_keeper->first_index; index; index = index->next)
        {
            if (_gk_glyph_index_ok_for_renderer(index,rend)) break;
        }

        if (!index) index = _gk_glyph_index_create(new_keeper,rend);
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
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_rend_set_face() begin\n");
#endif

    if (!rend) return;
    if (new_face == rend->face) return;
    _gk_rend_remove_size_object(rend);

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

    /* If the face is not scalable, then we can't produce AA glyphs */
    if (!FT_IS_SCALABLE(rend->face->face)) _gk_rend_reset_to_fixed(rend);

    /* Creating new size object and setting initial size */
    {
        FT_New_Size(rend->face->face,&rend->size);

        if (!rend->size)
        {
            rend->face = 0;
            _gk_msg("Error: gk_rend_set_face(): can't create size object\n");
            return;
        }

        if (!gk_rend_set_size_subpixel(rend,rend->hsize,rend->vsize))
        {
            _gk_rend_remove_size_object(rend);
            rend->face = 0;
            _gk_msg("Error: gk_rend_set_face(): can't set initial size\n");
            return;
        }
    }

    rend->face_id = rend->face->id;

    rend->prev_for_same_face = 0;
    rend->next_for_same_face = rend->face->first_renderer;
    if (rend->next_for_same_face) rend->next_for_same_face->prev_for_same_face = rend;
    if (!rend->face->last_renderer) rend->face->last_renderer = rend;
    rend->face->first_renderer = rend;

    if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_rend_set_face() end\n");
#endif
}


int gk_rend_set_size_subpixel(GLYPH_REND* const rend,const unsigned new_hsize,const unsigned new_vsize)
{
    int error;

    if (!rend || !rend->face || !rend->face->face || !rend->size) return 0;

    funcname = "gk_rend_set_size_subpixel()";

    error = FT_Activate_Size(rend->size);
    if (error)
    {
        _gk_msg("Error: %s: Can't activate size object\n",funcname);
        return 0;
    }

    if (FT_IS_SCALABLE(rend->face->face))
    {
        error = FT_Set_Char_Size(rend->face->face,new_hsize,new_vsize,72,72);
        if (error)
        {
            _gk_msg("Error: %s: Can't set font size to %.3f x %.3f pixels\n",
                    funcname, ((double)new_hsize)/64, ((double)new_vsize)/64 );
            return 0;
        }

        rend->hsize = new_hsize;
        rend->vsize = new_vsize;
    }
    else
    {
        int try_smaller_size = (new_vsize+32)/64;
        int try_larger_size = (new_vsize+32)/64;
        int result_hsize, result_vsize;

        error = FT_Set_Pixel_Sizes(rend->face->face, (new_hsize+32)/64, (new_vsize+32)/64 );
        if (error) error = FT_Set_Pixel_Sizes(rend->face->face, 0, (new_vsize+32)/64 );
        while (error && --try_smaller_size>1) error = FT_Set_Pixel_Sizes(rend->face->face,0,try_smaller_size);
        while (error && ++try_larger_size<100) error = FT_Set_Pixel_Sizes(rend->face->face,0,try_larger_size);

        if (error)
        {
            _gk_msg("Error: %s: Can't select font pixel size to match %.3f x %.3f pixels\n",
                    funcname, ((double)new_hsize)/64, ((double)new_vsize)/64 );
            return 0;
        }

        result_hsize = rend->size->metrics.x_ppem;
        result_vsize = rend->size->metrics.y_ppem;

        rend->hsize = result_hsize * 64;
        rend->vsize = result_vsize * 64;

        _gk_rend_reset_to_fixed(rend);

#ifdef GLYPH_LOG
        if (glyph_log) fprintf(glyph_log,"Pixel size of %d x %d is selected\n",result_hsize,result_vsize);
#endif
    }

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



static void _gk_rend_update_matrix(GLYPH_REND* const rend)
{
    rend->do_matrix_transform = (rend->text_angle != 0 || rend->italic_angle != 0);
    if (rend->do_matrix_transform)
    {
        FT_Matrix a;
        rend->matrix.xx = 0x10000;
        rend->matrix.xy = (FT_Fixed)(sin(rend->italic_angle)*(GK_SQRT2*0x10000));
        rend->matrix.yx = 0;
        rend->matrix.yy = 0x10000;
        a.xx = (FT_Fixed)( rend->text_angle_cos*0x10000);
        a.xy = (FT_Fixed)(-rend->text_angle_sin*0x10000);
        a.yx = (FT_Fixed)( rend->text_angle_sin*0x10000);
        a.yy = (FT_Fixed)( rend->text_angle_cos*0x10000);
        FT_Matrix_Multiply(&a,&rend->matrix);
    }
}


void gk_rend_set_angle_in_radians(GLYPH_REND* const rend,const double new_text_angle)
{
    if (!rend) return;
    if (!rend->face || !rend->face->face || (rend->face && rend->face->face && FT_IS_SCALABLE(rend->face->face)) )
    {
        if (rend->text_angle != new_text_angle)
        {
            rend->text_angle = new_text_angle;
            rend->text_angle_sin = sin(new_text_angle);
            rend->text_angle_cos = cos(new_text_angle);
            _gk_rend_update_matrix(rend);
            if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
        }
    }
}


void gk_rend_set_angle_in_degrees(GLYPH_REND* const rend,const double new_text_angle)
{
    gk_rend_set_angle_in_radians(rend,new_text_angle*GK_PI/180);
}


double gk_rend_get_angle_in_radians(const GLYPH_REND* const rend)
{
    return rend ? rend->text_angle : 0;
}


double gk_rend_get_angle_in_degrees(const GLYPH_REND* const rend)
{
    return rend ? rend->text_angle*180/GK_PI : 0;
}


void gk_rend_set_italic_angle_in_radians(GLYPH_REND* const rend,const double new_italic_angle)
{
    if (!rend) return;
    if (new_italic_angle < -GK_PI/4 || new_italic_angle > GK_PI*4) return;
    if (!rend->face || !rend->face->face || (rend->face && rend->face->face && FT_IS_SCALABLE(rend->face->face)) )
    {
        if (rend->italic_angle != new_italic_angle)
        {
            rend->italic_angle = new_italic_angle;
            _gk_rend_update_matrix(rend);
            if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
            /*_gk_msg("new italic angle set: %.3f\n",new_italic_angle);*/
        }
    }
}


void gk_rend_set_italic_angle_in_degrees(GLYPH_REND* const rend,const double new_italic_angle)
{
    gk_rend_set_italic_angle_in_radians(rend,new_italic_angle*GK_PI/180);
}


double gk_get_italic_angle_in_radians(const GLYPH_REND* const rend)
{
    return rend ? rend->italic_angle : 0;
}


double gk_get_italic_angle_in_degrees(const GLYPH_REND* const rend)
{
    return rend ? rend->italic_angle*180/GK_PI : 0;
}


void gk_rend_set_bold_strength(GLYPH_REND* const rend,const int new_bold_strength)
{
    if (!rend) return;
    if (!rend->face || !rend->face->face || (rend->face && rend->face->face && FT_IS_SCALABLE(rend->face->face)) )
    {
        rend->bold_strength = new_bold_strength;
        if (rend->index) gk_rend_set_keeper(rend,rend->index->keeper);
    }
}


int gk_rend_get_bold_strength(const GLYPH_REND* const rend)
{
    return rend ? rend->bold_strength : 0;
}


void gk_rend_set_before_renderer ( GLYPH_REND* const rend, GLYPH_REND* const new_before_rend,
    const int new_before_dx, const int new_before_dy )
{
    if (!rend) return;
    rend->before_rend = new_before_rend;
    rend->before_dx = new_before_dx;
    rend->before_dy = new_before_dy;
}


void gk_rend_set_after_renderer ( GLYPH_REND* const rend, GLYPH_REND* const new_after_rend,
    const int new_after_dx, const int new_after_dy )
{
    if (!rend) return;
    rend->after_rend = new_after_rend;
    rend->after_dx = new_after_dx;
    rend->after_dy = new_after_dy;
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
        _gk_msg("      index size is %d bytes\n",_gk_glyph_index_size(rend->index));
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
    glyph = _gk_rend_render(rend,code);
    if (!glyph) return 0;

    has_glyph = (glyph->bmp) ? 1 : 0;
    if (!glyph->index) _gk_unload_glyph(glyph);
    return has_glyph;
}


GLYPH_REND* gk_create_renderer(GLYPH_FACE* const face,GLYPH_KEEP* const keeper)
{
    GLYPH_REND *rend;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_renderer() begin\n");
#endif

    funcname = "gk_create_renderer()";
    rend = (GLYPH_REND*)_gk_malloc(sizeof(GLYPH_REND));
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
    rend->text_angle_sin = 0;
    rend->text_angle_cos = 1;
    rend->italic_angle = 0;
    rend->bold_strength = 0;
    rend->do_matrix_transform = 0;
    rend->matrix.xx = 0x10000l;
    rend->matrix.xy = 0;
    rend->matrix.yx = 0;
    rend->matrix.yy = 0x10000l;

    rend->text_alpha_color = 0xFF0000FF;
    rend->back_color = -1;

    rend->before_rend = 0;
    rend->before_dx = 0;
    rend->before_dy = 0;
    rend->after_rend = 0;
    rend->after_dx = 0;
    rend->after_dy = 0;

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

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_renderer() end\n");
#endif

    return rend;
}


GLYPH_REND* gk_create_copy_of_renderer(GLYPH_REND* const rend)
{
    GLYPH_REND *new_rend;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_copy_of_renderer() begin\n");
#endif

    if (!rend) return 0;

    new_rend = gk_create_renderer(rend->face,0);
    if (!new_rend) return 0;

    new_rend->text_alpha_color = rend->text_alpha_color;
    new_rend->back_color = rend->back_color;
    new_rend->undefined_char = rend->undefined_char;
    new_rend->error_char = rend->error_char;

    new_rend->hinting_mode = rend->hinting_mode;
    new_rend->hinting_target = rend->hinting_target;
    new_rend->load_flags = rend->load_flags;
    new_rend->render_mode = rend->render_mode;

    new_rend->target_info = rend->target_info;

    new_rend->text_angle = rend->text_angle;
    new_rend->text_angle_sin = rend->text_angle_sin;
    new_rend->text_angle_cos = rend->text_angle_cos;
    new_rend->italic_angle = rend->italic_angle;
    new_rend->bold_strength = rend->bold_strength;
    new_rend->do_matrix_transform = rend->do_matrix_transform;
    new_rend->matrix.xx = rend->matrix.xx;
    new_rend->matrix.xy = rend->matrix.xy;
    new_rend->matrix.yx = rend->matrix.yx;
    new_rend->matrix.yy = rend->matrix.yy;

    gk_rend_set_size_subpixel(new_rend,rend->hsize,rend->vsize);

    if (rend->index) gk_rend_set_keeper(new_rend,rend->index->keeper);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_create_copy_of_renderer() end\n");
#endif

    return new_rend;
}


void gk_done_renderer(GLYPH_REND* const rend)
{
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_done_renderer() begin\n");
#endif

    if (!rend) return;

    gk_rend_set_keeper(rend,0);
    gk_rend_set_face(rend,0);

#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)
    if (rend->target_info) _gk_free(rend->target_info);
#endif

    if (rend==first_renderer) first_renderer = rend->next;
    if (rend==last_renderer) last_renderer = rend->prev;
    if (rend->next) rend->next->prev = rend->prev;
    if (rend->prev) rend->prev->next = rend->next;

    _gk_free(rend);
    _gk_msg("glyph renderer destroyed\n");

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_done_renderer() end\n");
#endif
}


#endif  /* included_from_glyph_c */
