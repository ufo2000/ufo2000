/*
 * glyph_dimensions.c  -  Glyph Keeper routines for finding text size.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c


/*
 * Advance is subpixel, returned coordinates are integer pixels.
 */
static void _gk_text_rectangle_by_advance(GLYPH_REND* const rend,const int adv_x,const int adv_y,
    int* const xlt,int* const ylt,int* const xlb,int* const ylb,
    int* const xrt,int* const yrt,int* const xrb,int* const yrb)
{
    int asc = gk_rend_ascender(rend);
    int desc = gk_rend_descender(rend);
    int up_x, up_y;

    up_x = (int)(-rend->text_angle_sin * rend->text_height);
    up_y = (int)(-rend->text_angle_cos * rend->text_height);

    *xlb = (int)(rend->text_angle_sin * -desc);
    *ylb = (int)(rend->text_angle_cos * -desc);

    *xrb = *xlb + adv_x;
    *yrb = *ylb + adv_y;

    if (rend->italic_angle != 0)
    {
        double da = asc ? (tan(rend->italic_angle) * asc) : 0;
        double dd = desc ? (tan(rend->italic_angle) * desc) : 0;

        if (rend->italic_angle > 0)
        {
            *xlb += dd * rend->text_angle_cos;
            *ylb -= dd * rend->text_angle_sin;

            *xrb += da * rend->text_angle_cos;
            *yrb -= da * rend->text_angle_sin;
        }
        else
        {
            *xlb += da * rend->text_angle_cos;
            *ylb -= da * rend->text_angle_sin;

            *xrb += dd * rend->text_angle_cos;
            *yrb -= dd * rend->text_angle_sin;
        }
    }

    *xlt = (*xlb + up_x + 31) >> 6;
    *ylt = (*ylb + up_y + 31) >> 6;

    *xlb = (*xlb + 31) >> 6;
    *ylb = (*ylb + 31) >> 6;

    *xrt = (*xrb + up_x + 31) >> 6;
    *yrt = (*yrb + up_y + 31) >> 6;

    *xrb = (*xrb + 31) >> 6;
    *yrb = (*yrb + 31) >> 6;
}


/*
 * Advance is subpixel.
 */
static void _gk_text_dimensions_by_advance_angled(GLYPH_REND* const rend,const int adv_x,const int adv_y,
    int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
    int xmin,ymin,xmax,ymax;
    int xlb,ylb,xrt,yrt,xrb,yrb;

    _gk_text_rectangle_by_advance(rend,adv_x,adv_y,&xmin,&ymin,&xlb,&ylb,&xrt,&yrt,&xrb,&yrb);

    xmax = xmin;
    ymax = ymin;

    if (xlb < xmin) xmin = xlb;
    if (xlb > xmax) xmax = xlb;
    if (ylb < ymin) ymin = ylb;
    if (ylb > ymax) ymax = ylb;

    if (xrt < xmin) xmin = xrt;
    if (xrt > xmax) xmax = xrt;
    if (yrt < ymin) ymin = yrt;
    if (yrt > ymax) ymax = yrt;

    if (xrb < xmin) xmin = xrb;
    if (xrb > xmax) xmax = xrb;
    if (yrb < ymin) ymin = yrb;
    if (yrb > ymax) ymax = yrb;

    *size_x = xmax-xmin+1;
    *size_y = ymax-ymin+1;
    *origin_x = -xmin;
    *origin_y = -ymin;
}


/*
 * Advance is subpixel.
 */
static void _gk_text_dimensions_by_advance_horizontal(GLYPH_REND* const rend,const int adv_x,
    int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
    int sx = adv_x;
    int ox = 0;
    if (rend->italic_angle != 0)
    {
        sx += abs(tan(rend->italic_angle) * rend->text_height);

        if (rend->italic_angle > 0)
            ox -= tan(rend->italic_angle) * gk_rend_descender(rend);
        else
            ox -= tan(rend->italic_angle) * gk_rend_ascender(rend);
    }

    *size_x = (sx + 31) >> 6;
    *size_y = rend->text_height_pixels;
    *origin_x = (ox + 31) >> 6;
    *origin_y = (gk_rend_ascender(rend) + 31) >> 6;
}


/*
 * This function is useful to determine size in pixels of rendered text.
 * Smallest possible rectangle, containing rendered text, is constructed.
 * Size of this rectangle is written to 'size_x' and 'size_y'.
 * Position of text origin point, relative to the top left corner of rectangle,
 * is written to 'origin_x' and 'origin_y'.
 */
void gk_text_dimensions_utf8(GLYPH_REND* const rend,const char* const text,
    int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
    int adv_x,adv_y;
    if (!rend || !text) { *size_x = *size_y = *origin_x = *origin_y = 0; return; }
    gk_text_advance_subpixel_utf8(rend,text,&adv_x,&adv_y);
    if (rend->text_angle != 0)
    {
        _gk_text_dimensions_by_advance_angled(rend,adv_x,adv_y,size_x,size_y,origin_x,origin_y);
    }
    else
    {
        _gk_text_dimensions_by_advance_horizontal(rend,adv_x,size_x,size_y,origin_x,origin_y);
    }
}


void gk_text_dimensions_utf16(GLYPH_REND* const rend,const unsigned short* const text,
    int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
    int adv_x,adv_y;
    if (!rend || !text) { *size_x = *size_y = *origin_x = *origin_y = 0; return; }
    gk_text_advance_subpixel_utf16(rend,text,&adv_x,&adv_y);
    if (rend->text_angle != 0)
    {
        _gk_text_dimensions_by_advance_angled(rend,adv_x,adv_y,size_x,size_y,origin_x,origin_y);
    }
    else
    {
        _gk_text_dimensions_by_advance_horizontal(rend,adv_x,size_x,size_y,origin_x,origin_y);
    }
}


void gk_text_dimensions_utf32(GLYPH_REND* const rend,const unsigned* const text,
    int* const size_x,int* const size_y,int* const origin_x,int* const origin_y)
{
    int adv_x,adv_y;
    if (!rend || !text) { *size_x = *size_y = *origin_x = *origin_y = 0; return; }
    gk_text_advance_subpixel_utf32(rend,text,&adv_x,&adv_y);
    if (rend->text_angle != 0)
    {
        _gk_text_dimensions_by_advance_angled(rend,adv_x,adv_y,size_x,size_y,origin_x,origin_y);
    }
    else
    {
        _gk_text_dimensions_by_advance_horizontal(rend,adv_x,size_x,size_y,origin_x,origin_y);
    }
}



/***************************************************************************
 *
 *   gk_text_size...
 */

void gk_glyph_size(GLYPH_REND* const rend,const unsigned unicode,int *const width,int* const height)
{
    GLYPH *glyph;

    if (!width || !height) return;
    if (!rend || !unicode) { *width = *height = 0; return; }

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        *width = glyph->width;
        *height = glyph->height;
        if (!glyph->index) _gk_unload_glyph(glyph);
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

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        if (rend->text_angle == 0)
        {
            *width = (glyph->advance_x + 63) >> 6;
            *height = rend->text_height_pixels;
        }
        else
        {
            *width = (abs(glyph->advance_x) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
            *height = (abs(glyph->advance_y) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
        }

        if (!glyph->index) _gk_unload_glyph(glyph);
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
        GLYPH* glyph = _gk_rend_render(rend,code);
        if (glyph)
        {
            px += glyph->advance_x;
            py -= glyph->advance_y;
            if (!glyph->index) _gk_unload_glyph(glyph);
        }
    }

    if (rend->text_angle == 0)
    {
        *width = (px + 63) >> 6;
        *height = rend->text_height_pixels;
    }
    else
    {
        *width = (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
        *height = (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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
        GLYPH* glyph = _gk_rend_render(rend,code);
        if (glyph)
        {
            px += glyph->advance_x;
            py -= glyph->advance_y;
            if (!glyph->index) _gk_unload_glyph(glyph);
        }
    }

    if (rend->text_angle == 0)
    {
        *width = (px + 63) >> 6;
        *height = rend->text_height_pixels;
    }
    else
    {
        *width = (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
        *height = (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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
        GLYPH* glyph = _gk_rend_render(rend,code);
        if (glyph)
        {
            px += glyph->advance_x;
            py -= glyph->advance_y;
            if (!glyph->index) _gk_unload_glyph(glyph);
        }
    }

    if (rend->text_angle == 0)
    {
        *width = (px + 63) >> 6;
        *height = rend->text_height_pixels;
    }
    else
    {
        *width = (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
        *height = (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        w = glyph->width;
        if (!glyph->index) _gk_unload_glyph(glyph);
    }
    return w;
}


int gk_char_width(GLYPH_REND* const rend,const unsigned unicode)
{
    GLYPH *glyph;
    int w = 0;

    if (!rend || !unicode) { return 0; }

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        if (rend->text_angle == 0)
            w = (glyph->advance_x + 63) >> 6;
        else
            w = (abs(glyph->advance_x) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;

        if (!glyph->index) _gk_unload_glyph(glyph);
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
        return (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
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
        return (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
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
        return (abs(px) + abs((int)(rend->text_angle_sin*rend->text_height)) + 63) >> 6;
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

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        h = glyph->height;
        if (!glyph->index) _gk_unload_glyph(glyph);
    }
    return h;
}


int gk_char_height(GLYPH_REND* const rend,const unsigned unicode)
{
    GLYPH *glyph;
    int h = 0;

    if (!rend || !unicode) { return 0; }

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        if (rend->text_angle == 0)
            h = rend->text_height_pixels;
        else
            h = (abs(glyph->advance_y) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;

        if (!glyph->index) _gk_unload_glyph(glyph);
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
            GLYPH* glyph = _gk_rend_render(rend,code);
            if (glyph)
            {
                py -= glyph->advance_y;
                if (!glyph->index) _gk_unload_glyph(glyph);
            }
        }

        return (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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
            GLYPH* glyph = _gk_rend_render(rend,code);
            if (glyph)
            {
                py -= glyph->advance_y;
                if (!glyph->index) _gk_unload_glyph(glyph);
            }
        }

        return (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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
            GLYPH* glyph = _gk_rend_render(rend,code);
            if (glyph)
            {
                py -= glyph->advance_y;
                if (!glyph->index) _gk_unload_glyph(glyph);
            }
        }

        return (abs(py) + abs((int)(rend->text_angle_cos*rend->text_height)) + 63) >> 6;
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

    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        *adv_x = glyph->advance_x;
        *adv_y = -glyph->advance_y;
        if (!glyph->index) _gk_unload_glyph(glyph);
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


#endif  /* included_from_glyph_c */
