/*
 * glyph_workout.c  -  Glyph Keeper routines rendering a character glyph.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c


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
        if (rle_buffer) { _gk_free(rle_buffer); rle_buffer = 0; }
        rle_buffer_size = 0;
        rle_buffer = (unsigned char*)_gk_malloc(max_rlesize*2);
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
        if (rle_buffer) { _gk_free(rle_buffer); rle_buffer = 0; }
        rle_buffer_size = 0;
        rle_buffer = (unsigned char*)_gk_malloc(max_rlesize*2);
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


static GLYPH* _gk_rend_render(GLYPH_REND* const rend,const unsigned unicode);


static void GK_Outline_Embolden(FT_Outline* outline,FT_Pos strength,int* x_left,int* x_right,int* y_bottom,int* y_top)
{
    FT_Vector*  points;
    FT_Vector   v_prev, v_first, v_next, v_cur;
    FT_Angle    rotate, angle_in, angle_out;
    FT_Int      c, n, first = 0;
    FT_Int      orientation;
    FT_Int      min_x = 35565, max_x = -35565, min_y = 35565, max_y = -35565;
    FT_Int      new_min_x = 35565, new_max_x = -35565, new_min_y = 35565, new_max_y = -35565;

    if (!outline) return;

    strength /= 2;
    if ( strength == 0 ) return;

    orientation = FT_Outline_Get_Orientation(outline);
    if (orientation != FT_ORIENTATION_TRUETYPE && orientation != FT_ORIENTATION_POSTSCRIPT) return;

#ifdef GLYPH_LOG
    if (glyph_log) { fprintf(glyph_log,"            Emboldening the outline by %ld:\n",strength); }
#endif

    if (orientation == FT_ORIENTATION_TRUETYPE) rotate = -FT_ANGLE_PI2;
    else rotate = FT_ANGLE_PI2;

    points = outline->points;

    for ( c = 0; c < outline->n_contours; c++ )
    {
        int last = outline->contours[c];

        v_first = points[first];
        v_prev  = points[last];
        v_cur   = v_first;

        for ( n = first; n <= last; n++ )
        {
            FT_Vector  in, out;
            FT_Angle   angle_diff;
            FT_Pos     d;
            FT_Fixed   scale;

            if ( n < last ) v_next = points[n + 1];
            else v_next = v_first;

            /* compute the in and out vectors */
            in.x = v_cur.x - v_prev.x;
            in.y = v_cur.y - v_prev.y;

            out.x = v_next.x - v_cur.x;
            out.y = v_next.y - v_cur.y;

            angle_in   = FT_Atan2( in.x, in.y );
            angle_out  = FT_Atan2( out.x, out.y );
            angle_diff = FT_Angle_Diff( angle_in, angle_out );
            scale      = FT_Cos( angle_diff / 2 );

            if ( scale < 0x4000L && scale > -0x4000L ) in.x = in.y = 0;
            else
            {
                d = FT_DivFix( strength, scale );
                FT_Vector_From_Polar( &in, d, angle_in + angle_diff / 2 - rotate );
            }

            outline->points[n].x = v_cur.x + strength + in.x;
            outline->points[n].y = v_cur.y + strength + in.y;

            if (v_cur.x < min_x) min_x = v_cur.x;
            if (v_cur.x > max_x) max_x = v_cur.x;
            if (v_cur.y < min_y) min_y = v_cur.y;
            if (v_cur.y > max_y) max_y = v_cur.y;

            if (outline->points[n].x < new_min_x) new_min_x = outline->points[n].x;
            if (outline->points[n].x > new_max_x) new_max_x = outline->points[n].x;
            if (outline->points[n].y < new_min_y) new_min_y = outline->points[n].y;
            if (outline->points[n].y > new_max_y) new_max_y = outline->points[n].y;

            v_prev = v_cur;
            v_cur  = v_next;
        }

        first = last + 1;
    }

    *x_left = min_x - new_min_x;
    *x_right = new_max_x - max_x;
    *y_top = new_max_y - max_y;
    *y_bottom = min_y - new_min_y;

#ifdef GLYPH_LOG
    if (glyph_log)
    {
        fprintf(glyph_log,"                (%d..%d x %d..%d) -> (%d..%d x %d..%d)\n",
                           min_x,max_x,min_y,max_y,new_min_x,new_max_x,new_min_y,new_max_y);
    }
#endif
}


/*
 * Renders a glyph immediately, then tries to puts it into cache.
 * rend and rend->face must be not 0
 */
static GLYPH* _gk_rend_workout(GLYPH_REND* const rend,const unsigned unicode)
{
    unsigned glyph_index;
    int bmp_size;
    int error;
    int center_x = 0,center_y = 0;
    FT_Glyph ft_glyph = 0;
    GLYPH* glyph;
    GLYPH_FACE* actual_face;
    unsigned actual_code;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_rend_workout(%p,%d) begin\n",(void*)rend,unicode);
#endif

    CARE(rend && rend->face);
    CARE(unicode > 0);
    CARE(unicode <= GK_MAX_UNICODE);

    funcname = "_gk_rend_workout()";

    _gk_find_mapping(rend->face,unicode,&actual_face,&actual_code);
    if (!actual_face || !actual_code) return 0;
    if (!actual_face->face) return 0;

    glyph_index = FT_Get_Char_Index(actual_face->face,actual_code);
    if (!glyph_index)
    {
        if (unicode==rend->undefined_char || unicode==rend->error_char) return 0;
        else return _gk_rend_render(rend,rend->undefined_char);
    }

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"    glyph_index is %u\n",glyph_index);
#endif

    /* Preparing for glyph loading: setting size */
    /*FT_Activate_Size(size);*/
    if (actual_face == rend->face)
    {
        error = (rend->size == 0);
        if (!error) actual_face->face->size = rend->size;
    }
    else
    {
        error = (actual_face->own_size == 0);
        if (!error)
        {
            actual_face->face->size = actual_face->own_size;
            error = FT_Set_Char_Size(actual_face->face,rend->hsize,rend->vsize,72,72);
        }
    }

#ifdef GLYPH_LOG
    if (error)
    {
        if (glyph_log) fprintf(glyph_log,"    Failed to select size\n");
    }
    else
    {
        if (glyph_log) fprintf(glyph_log,"    Size selected successfully\n");
    }
#endif

    /* Loading a glyph with FreeType */
    if (!error)
    {
        error = FT_Load_Glyph(actual_face->face,glyph_index,rend->load_flags);
        if (error)
            _gk_msg("Error: %s: FT_Load_Glyph() bugs on glyph (#%d) for character U+%04X\n",funcname,glyph_index,unicode);
        else
        {
            error = (actual_face->face->glyph == 0);
            if (error) _gk_msg("Error: %s: Empty glyph slot after FT_Load_Glyph()\n",funcname);
        }

/*#ifdef GLYPH_LOG
        if (error)
        {
            if (glyph_log) fprintf(glyph_log,"    Could not load a glyph with FT_Load_Glyph()\n");
        }
        else
        {
            if (glyph_log) fprintf(glyph_log,"    Loaded a glyph with FT_Load_Glyph()\n");
        }
#endif*/
    }


    /* Getting center coordinates (MEGA-HACK) */
    /* This whole idea should be re-made in more optimal way */
    if (!error)
    {
/*#ifdef GLYPH_LOG
        if (glyph_log) fprintf(glyph_log,"    Getting a center point\n");
#endif*/
        FT_Glyph g = 0;
        error = FT_Get_Glyph(actual_face->face->glyph, &g);
/*#ifdef GLYPH_LOG
        if (glyph_log)
        {
            if (error)
                fprintf(glyph_log,"    FT_Get_Glyph() failed\n");
            else
                fprintf(glyph_log,"    FT_Get_Glyph() succeeded\n");
            if (g)
                fprintf(glyph_log,"    FT_Get_Glyph() returned not 0\n");
            else
                fprintf(glyph_log,"    FT_Get_Glyph() returned 0\n");
        }
#endif*/

        error = error || !g;
        if (!error && g->format==FT_GLYPH_FORMAT_OUTLINE)
        {
/*#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"    Calling FT_Glyph_To_Bitmap()\n");
#endif*/
            error = FT_Glyph_To_Bitmap(&g,rend->render_mode,0,1);
/*#ifdef GLYPH_LOG
            if (glyph_log)
            {
                if (error)
                    fprintf(glyph_log,"    FT_Glyph_To_Bitmap() failed\n");
                else
                    fprintf(glyph_log,"    FT_Glyph_To_Bitmap() succeeded\n");
            }
#endif*/
            if (!error)
            {
                center_x = 64 * ((FT_BitmapGlyph)g)->left + 64 * ((FT_BitmapGlyph)g)->bitmap.width / 2;
                center_y = 64 * ((FT_BitmapGlyph)g)->top - 64 * ((FT_BitmapGlyph)g)->bitmap.rows / 2;
                /*center_x = 64 * ((FT_BitmapGlyph)g)->bitmap.width / 2;
                center_y = -64 * ((FT_BitmapGlyph)g)->bitmap.rows / 2;*/
            }
        }
        if (g)
        {
/*#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"    Calling FT_Done_Glyph()\n");
#endif*/
            FT_Done_Glyph(g);
        }
/*#ifdef GLYPH_LOG
        if (glyph_log)
        {
            if (error)
                fprintf(glyph_log,"    Could not find a center point\n");
            else
                fprintf(glyph_log,"    Computed a center point\n");
        }
#endif*/
    }




    /* Emboldening the glyph */
    if (!error)
    {
        if (rend->bold_strength && actual_face->face->glyph->format==FT_GLYPH_FORMAT_OUTLINE)
        {
            int xstr, ystr, xstr2, ystr2;
            int xmin,xmax,ymin,ymax;
            int center_dx, center_dy;
            FT_GlyphSlot slot = actual_face->face->glyph;

#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"        Emboldening the glyph by %d\n",rend->bold_strength);
#endif

            xstr = rend->bold_strength * 
                   FT_MulFix( actual_face->face->units_per_EM, actual_face->face->size->metrics.y_scale ) / 2400;
            ystr = xstr;
#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"            xstr = %d, ystr = %d\n",xstr,ystr);
#endif
            GK_Outline_Embolden(&slot->outline,xstr,&xmin,&xmax,&ymin,&ymax);
#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"            xmin = %d, xmax = %d\n",xmin,xmax);
            if (glyph_log) fprintf(glyph_log,"            ymin = %d, ymax = %d\n",ymin,ymax);
#endif
            /*xstr = xstr * 2;
            ystr = xstr;*/
#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"            xstr = %d, ystr = %d\n",xstr,ystr);
#endif

            xstr2 = xmin+xmax;
            ystr2 = ymin+ymax;
#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"            xstr2 = %d, ystr2 = %d\n",xstr2,ystr2);
            if (glyph_log) fprintf(glyph_log,"            xscale = %.5f\n",((double)actual_face->face->size->metrics.x_scale)/65536);
            if (glyph_log) fprintf(glyph_log,"            yscale = %.5f\n",((double)actual_face->face->size->metrics.y_scale)/65536);
#endif

            if (slot->advance.x) slot->advance.x += xstr;
            if (slot->advance.y) slot->advance.y += ystr;

            slot->metrics.width        += xstr * 2;
            slot->metrics.height       += ystr * 2;
            /*slot->metrics.horiBearingX -= xmin;*/
            slot->metrics.horiBearingY += ystr * 2;
            slot->metrics.horiAdvance  += xstr * 2;
            slot->metrics.vertBearingX -= xstr;
            slot->metrics.vertBearingY += ystr * 2;
            slot->metrics.vertAdvance  += ystr * 2;

            /*center_dx = (int)( (double)xstr / 2 * (double)actual_face->face->size->metrics.x_scale / (80000) );
            center_dy = (int)( (double)ystr / 2 * (double)actual_face->face->size->metrics.y_scale / (65536) );*/

            center_dx = (int)( (double)rend->bold_strength * (double)actual_face->face->size->metrics.x_ppem / 80);
            center_dy = (int)( (double)rend->bold_strength * (double)actual_face->face->size->metrics.y_ppem / 80);

#ifdef GLYPH_LOG
            if (glyph_log) fprintf(glyph_log,"            center_x += %.1f\n",((double)center_dx)/64);
            if (glyph_log) fprintf(glyph_log,"            center_y += %.1f\n",((double)center_dx)/64);
#endif

            center_x += center_dx;
            center_y += center_dy;

            /*center_x += xstr;*/
            /*center_y += ymin;*/

            /*FT_GlyphSlot_Embolden(slot);*/
        }
    }

    /* Getting a glyph from FreeType */
    if (!error)
    {
        error = FT_Get_Glyph(actual_face->face->glyph, &ft_glyph);
        error = error || !ft_glyph;
        if (error)
            _gk_msg("Error: %s: Can't get a glyph with FT_Get_Glyph() call. Glyph (#%d) for character U+%04X\n",
            funcname,glyph_index,unicode);
    }

    /* Transforming the glyph to apply rotation and italics */
    if (!error && ft_glyph->format==FT_GLYPH_FORMAT_OUTLINE && rend->do_matrix_transform)
    {
        error = FT_Glyph_Transform(ft_glyph,&rend->matrix,0);
        if (!error)
        {
            FT_Vector c;
            c.x = center_x;
            c.y = center_y;
            FT_Vector_Transform(&c,&rend->matrix);
            center_x = c.x;
            center_y = c.y;
        }
    }

    /* Converting glyph to bitmap */
    if (!error && ft_glyph->format==FT_GLYPH_FORMAT_OUTLINE)
    {
        error = FT_Glyph_To_Bitmap(&ft_glyph,rend->render_mode,0,1);
        if (error) _gk_msg("Error: %s: FT_Glyph_To_Bitmap() bugs on character U+%04X\n",funcname,unicode);
    }

    /* Checking if we have bitmap now */
    if (!error)
    {
        error = (ft_glyph->format!=FT_GLYPH_FORMAT_BITMAP);
        if (error) _gk_msg("Error: %s: Glyph is not FT_GLYPH_FORMAT_BITMAP after rendering with FT_Glyph_To_Bitmap()\n",funcname);
    }

#ifdef GLYPH_LOG
/*    {
        if (glyph_log)
        {
            fprintf(glyph_log,"    We got a bitmap glyph from FreeType!\n");
            if (ft_glyph->format==FT_GLYPH_FORMAT_BITMAP)
            {
                fprintf(glyph_log,"        Format: FT_GLYPH_FORMAT_BITMAP\n");
                fprintf(glyph_log,"        Size: %dx%d, pitch: %d\n",
                                  ((FT_BitmapGlyph)ft_glyph)->bitmap.width,
                                  ((FT_BitmapGlyph)ft_glyph)->bitmap.rows,
                                  ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch);
                if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_MONO)
                {
                    int x,y;
                    int pitch = ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch;
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_MONO\n");
                    fprintf(glyph_log,"        Buffer:\n");
                    for (y=0; y < ((FT_BitmapGlyph)ft_glyph)->bitmap.rows; y++)
                    {
                        fprintf(glyph_log,"            ");
                        for (x=0; x < ((FT_BitmapGlyph)ft_glyph)->bitmap.width; x++)
                        {
                            unsigned char byte = ((unsigned char*)((FT_BitmapGlyph)ft_glyph)->bitmap.buffer)[y*pitch+x];
                            unsigned char mask = 128 >> (x%8);
                            fprintf(glyph_log,"%s",(byte&mask)?" *":" .");
                        }
                        fprintf(glyph_log,"\n");
                    }
                }
                else if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_GRAY)
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_GRAY\n");
                else if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_GRAY2)
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_GRAY2\n");
                else if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_GRAY4)
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_GRAY4\n");
                else if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_LCD)
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_LCD\n");
                else if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode==FT_PIXEL_MODE_LCD_V)
                    fprintf(glyph_log,"        Pixel mode: FT_PIXEL_MODE_LCD_V\n");
            }
            else if (ft_glyph->format==FT_GLYPH_FORMAT_OUTLINE)
            {
                fprintf(glyph_log,"        Format: FT_GLYPH_FORMAT_OUTLINE\n");
            }
            fprintf(glyph_log,"        Advance: ( %.1f , %.1f ) pixels\n",
                    ((double)ft_glyph->advance.x)/0x10000,((double)ft_glyph->advance.y)/0x10000);
        }
    }*/
#endif

    if (!error)
    {
        error = ( ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch < 0 );
        if (error) _gk_msg("Error: rend_workout(): Rendered glyph has negative pitch value, can't handle\n"); /* FIXME */
    }

    if (!error)
    {
        glyph = (GLYPH*)_gk_malloc(sizeof(GLYPH));
        error = (glyph == 0);
    }

    if (error || !ft_glyph)
    {
        if (ft_glyph) FT_Done_Glyph(ft_glyph);

        if (unicode==rend->error_char) return 0;
        else return _gk_rend_render(rend,rend->error_char);
    }

    glyph->unicode = unicode;
    glyph->width = ((FT_BitmapGlyph)ft_glyph)->bitmap.width;
    glyph->height = ((FT_BitmapGlyph)ft_glyph)->bitmap.rows;
    glyph->left = ((FT_BitmapGlyph)ft_glyph)->left;
    glyph->top = ((FT_BitmapGlyph)ft_glyph)->top;
    glyph->advance_x = ft_glyph->advance.x >> 10;
    glyph->advance_y = ft_glyph->advance.y >> 10;
    glyph->center_x = (center_x + 31) / 64;
    glyph->center_y = (center_y + 31) / 64;

    glyph->index = 0;
    glyph->prev = 0;
    glyph->next = 0;
    glyph->bmp = 0;
    glyph->bmpsize = 0;

    _gk_msg("rendering character '%c'\n",unicode);

    bmp_size = glyph->width*glyph->height;
    if (!bmp_size) /* empty glyph, like space (' ') character */
    {
        if (rend->index) _gk_glyph_index_add_glyph(rend->index,glyph);
        FT_Done_Glyph(ft_glyph);
        return glyph;
    }

    if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
    {
#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
        int pitch1 = (glyph->width+7)>>3;
        int bitpack_size = glyph->height*pitch1;

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
        int rle_size = 0;

        if (rend->index)
              rle_size = _gk_make_RLE7( ((FT_BitmapGlyph)ft_glyph)->bitmap.buffer,
                         glyph->width, glyph->height, ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch);

        if (rle_size>0 && rle_size<=bitpack_size)
        {
            glyph->bmp = (unsigned char*)_gk_malloc(rle_size);
            if (!glyph->bmp) { _gk_free(glyph); return 0; }
            memcpy(glyph->bmp,rle_buffer,rle_size);
            glyph->bmpsize = rle_size;
        }
        else
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
        {
            glyph->bmp = (unsigned char*)_gk_malloc(bitpack_size+1);
            if (!glyph->bmp) { _gk_free(glyph); return 0; }
            glyph->bmp[0] = GLYPH_MONO_BITPACK;
            if ( ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch == pitch1 )
                memcpy( glyph->bmp+1, ((FT_BitmapGlyph)ft_glyph)->bitmap.buffer, bitpack_size );
            else
            {
                unsigned char *d = glyph->bmp+1;
                int y = 0;
                for (; y<glyph->height; y++,d+=pitch1)
                    memcpy( d, ((FT_BitmapGlyph)ft_glyph)->bitmap.buffer + y * ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch, pitch1 );
            }
            glyph->bmpsize = bitpack_size+1;
        }
#else  /* GLYPH_TARGET_KNOWS_MONO_BITPACK */
        {
            int y = 0;
            unsigned char* b;

            glyph->bmp = (unsigned char*)_gk_malloc(bmp_size+1);
            if (!glyph->bmp) { _gk_free(glyph); return 0; }
            glyph->bmp[0] = GLYPH_UNCOMPRESSED;
            glyph->bmpsize = bmp_size+1;
            b = glyph->bmp + 1;

            for (; y<glyph->height; y++)
            {
                int x = 0;
                unsigned char* a = ((FT_BitmapGlyph)ft_glyph)->bitmap.buffer + y * ((FT_BitmapGlyph)ft_glyph)->bitmap.pitch;
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
    else  /* if (((FT_BitmapGlyph)ft_glyph)->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) */
    {
#ifdef GLYPH_TARGET_KNOWS_RLEAA
        int rle_size = 0;
#endif
        FT_Bitmap *ft_bmp;

        if (((FT_BitmapGlyph)ft_glyph)->bitmap.num_grays!=256)
        {
            FT_Bitmap_Convert(ftlib,&((FT_BitmapGlyph)ft_glyph)->bitmap,&_gk_workout_bitmap,4);
            ft_bmp = &_gk_workout_bitmap;
        }
        else { ft_bmp = &((FT_BitmapGlyph)ft_glyph)->bitmap; }

        /* FIXME: handle case when ((FT_BitmapGlyph)ft_glyph)->bitmap.num_grays is not 256 */
        /*CARE(((FT_BitmapGlyph)ft_glyph)->bitmap.num_grays==256);*/

#ifdef GLYPH_TARGET_KNOWS_RLEAA
        if (rend->index)
            rle_size = _gk_make_RLEAA(ft_bmp->buffer,glyph->width,glyph->height);

        if (rle_size>0 && rle_size<bmp_size)
        {
            glyph->bmp = (unsigned char*)_gk_malloc(rle_size);
            if (!glyph->bmp) { _gk_free(glyph); return 0; }
            memcpy(glyph->bmp,rle_buffer,rle_size);
            glyph->bmpsize = rle_size;
        }
        else
#endif
        {
            glyph->bmp = (unsigned char*)_gk_malloc(bmp_size+1);
            if (!glyph->bmp) { _gk_free(glyph); return 0; }
            glyph->bmp[0] = GLYPH_UNCOMPRESSED;
            memcpy(glyph->bmp+1,ft_bmp->buffer,bmp_size);
            glyph->bmpsize = bmp_size+1;
        }
    }

    if (rend->index) _gk_glyph_index_add_glyph(rend->index,glyph);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_rend_workout(%p,%d) end\n",(void*)rend,unicode);
#endif

    FT_Done_Glyph(ft_glyph);
    return glyph;
}


static GLYPH* _gk_rend_render(GLYPH_REND* const rend,const unsigned unicode)
{
    GLYPH *glyph = 0;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_rend_render(%p,%d) begin\n",(void*)rend,unicode);
#endif

    if (!unicode || unicode>GK_MAX_UNICODE) return 0;
    if (!rend || !rend->face) return 0;

    /* First look in cache */
    if (rend->index) glyph = glyph_index_find_glyph(rend->index,unicode);
    /* Then try to render */
    if (!glyph) glyph = _gk_rend_workout(rend,unicode);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"_gk_rend_render(%p,%d) end\n",(void*)rend,unicode);
#endif

    return glyph;
}


#endif  /* included_from_glyph_c */
