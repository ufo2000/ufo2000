/*
 * glyph_to_allegro_mono.c  -  part of Glyph Keeper Allegro driver.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_to_allegro_c


#ifdef GLYPH_TARGET_KNOWS_MONO_BITPACK
/*
 * General case code for printing a monochrome glyph to Allegro bitmap.
 */
static void put_glyph_mono(const GLYPH* const glyph,const int x0,const int y0)
{
    if ( (alpha == 0xFF) || (coldepth <= 8) ) { solid_mode(); }
    else { drawing_mode(DRAW_MODE_TRANS,0,0,0); set_trans_blender(0,0,0,alpha); }

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        int bxend = x0 + glyph->width;
        int byend = y0 + glyph->height;
        unsigned char* rlepos = glyph->bmp+1;

        if (byend>cb) byend = cb;
        while (by<ct) { int bx = x0; while (bx<bxend) bx += (*rlepos++)&127; by++; }

        for (; by<byend; by++)
        {
            int bx = x0;
            while (bx<bxend)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 ) { int bx2 = bx+n; while (bx<bx2) putpixel(bmp,bx++,by,text_color); }
                else bx += n;
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int y = (y0 >= ct) ? y0 : ct;
        int yend = y0 + glyph->height;
        int xend = x0 + glyph->width;

        if (yend > cb) yend = cb;
        for (; y<yend; y++)
        {
            int x = x0;
            unsigned char* a = glyph->bmp + 1 + pitch*(y-y0);

            unsigned char* aend = a + pitch;
            while (a<aend && x<xend)
            {
                if (*a&0x80) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x40) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x20) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x10) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x08) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x04) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x02) putpixel(bmp,x,y,text_color); x++;
                if (*a&0x01) putpixel(bmp,x,y,text_color); x++;
                a++;
            }
        }
    }

    solid_mode();
}


/*
 * Fast code for printing a monochrome opaque (alpha=255) glyph to 32-bit bitmap.
 */
static void put_glyph_mono_opaque_32(const GLYPH* const glyph,const int x0,const int y0)
{
    int byend = y0 + glyph->height;
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        unsigned char* rlepos = glyph->bmp+1;
        for (; by<ct; by++) { int bx = x0, bx1 = x0 + glyph->width; while (bx<bx1) bx += (*rlepos++)&127; }

        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 )
                {
                      unsigned *addr2 = addr + n;
                      /*unsigned *al2 = (addr2 < addr_r) ? addr2 : addr_r;
                      if (addr < addr_l) addr = addr_l;
                      while (addr < al2) { bmp_write32(addr,text_color); addr++; }
                      addr = addr2;*/
                      while (addr<addr2) { if (addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++; }
                }
                else { addr += n; }
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned char* a = glyph->bmp + 1 + pitch*(by-y0);
            unsigned char* aend = a + pitch;
            while (a<aend && addr<addr_r)
            {
                if (*a&0x80 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x40 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x20 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x10 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x08 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x04 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x02 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                if (*a&0x01 && addr>=addr_l && addr<addr_r) bmp_write32(addr,text_color); addr++;
                a++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}


/*
 * Fast code for printing a monochrome opaque (alpha=255) glyph to 16-bit bitmap.
 */
static void put_glyph_mono_opaque_16(const GLYPH* const glyph,const int x0,const int y0)
{
    int byend = y0 + glyph->height;
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        unsigned char* rlepos = glyph->bmp+1;
        for (; by<ct; by++) { int bx = x0, bx1 = x0 + glyph->width; while (bx<bx1) bx += (*rlepos++)&127; }

        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + x0;
            unsigned short *addr_l = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned short *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 )
                {
                    unsigned short *addr2 = addr + n;
                    while (addr<addr2) { if (addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++; }
                }
                else { addr += n; }
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + x0;
            unsigned short *addr_l = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned char* a = glyph->bmp + 1 + pitch*(by-y0);
            unsigned char* aend = a + pitch;
            while (a<aend && addr<addr_r)
            {
                if (*a&0x80 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x40 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x20 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x10 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x08 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x04 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x02 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                if (*a&0x01 && addr>=addr_l && addr<addr_r) bmp_write16(addr,text_color); addr++;
                a++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}


/*
 * Fast code for printing a monochrome opaque (alpha=255) glyph to 8-bit bitmap.
 */
static void put_glyph_mono_opaque_8(const GLYPH* const glyph,const int x0,const int y0)
{
    int byend = y0 + glyph->height;
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"put_glyph_mono_opaque_8() begin\n");
#endif

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        unsigned char* rlepos = glyph->bmp+1;
        for (; by<ct; by++) { int bx = x0, bx1 = x0 + glyph->width; while (bx<bx1) bx += (*rlepos++)&127; }

        for (; by<byend; by++)
        {
            unsigned char *line = (unsigned char*)bmp_write_line(bmp,by);
            unsigned char *addr = line + x0;
            unsigned char *addr_l = line + xl;
            unsigned char *addr_r = line + xr;
            unsigned char *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 )
                {
                    unsigned char *addr2 = addr + n;
                    while (addr<addr2) { if (addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++; }
                }
                else { addr += n; }
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned char *line = (unsigned char*)bmp_write_line(bmp,by);
            unsigned char *addr = line + x0;
            unsigned char *addr_l = line + xl;
            unsigned char *addr_r = line + xr;
            unsigned char* a = glyph->bmp + 1 + pitch*(by-y0);
            unsigned char* aend = a + pitch;
            while (a<aend && addr<addr_r)
            {
                if (*a&0x80 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x40 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x20 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x10 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x08 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x04 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x02 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                if (*a&0x01 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++;
                a++;
            }
        }
    }

    bmp_unwrite_line(bmp);

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"put_glyph_mono_opaque_8() end\n");
#endif
}


/*
 * Fast code for printing a monochrome transparent (alpha<255) glyph to 32-bit bitmap.
 */
static void put_glyph_mono_transparent_32(const GLYPH* const glyph,const int x0,const int y0)
{
    int byend = y0 + glyph->height;
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    if (last_monotrans_alpha_color!=alpha_color)
    {
        int alpha1 = (255-alpha)*257;
        int r = (alpha_color>>16)&0xFF;
        int g = (alpha_color>>8)&0xFF;
        int b = alpha_color&0xFF;
        int r_a = r*alpha*257+256;
        int g_a = g*alpha*257+256;
        int b_a = b*alpha*257+256;
        int n = 0;
        for (; n<256; n++)
        {
            rtrans[n] = (n*alpha1 + r_a)>>16;
            gtrans[n] = (n*alpha1 + g_a)>>16;
            btrans[n] = (n*alpha1 + b_a)>>16;
        }
        last_monotrans_alpha_color = alpha_color;
    }

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        unsigned char* rlepos = glyph->bmp+1;
        for (; by<ct; by++) { int bx = x0; while (bx<xr) bx += (*rlepos++)&127; }

        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 )
                {
                    unsigned *addr2 = addr + n;
                    while (addr<addr2)
                    {
                        if (addr>=addr_l && addr<addr_r)
                        {
                            unsigned int value = bmp_read32(addr);
                            bmp_write32(addr,makecol32(rtrans[getr32(value)],
                                                       gtrans[getg32(value)],
                                                       btrans[getb32(value)]));
                        }
                        addr++;
                    }
                }
                else { addr += n; }
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int by = (y0 >= ct) ? y0 : ct;

        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned char* a = glyph->bmp + 1 + pitch*(by-y0);
            unsigned char* aend = a + pitch;
            unsigned int v;
            while (a<aend && addr<addr_r)
            {
                if (*a&0x80 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x40 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x20 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x10 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x08 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x04 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x02 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                if (*a&0x01 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read32(addr); bmp_write32(addr,makecol32(rtrans[getr32(v)],gtrans[getg32(v)],btrans[getb32(v)])); }
                addr++;
                a++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}


/*
 * Fast code for printing a monochrome transparent (alpha<255) glyph to 16-bit bitmap.
 */
static void put_glyph_mono_transparent_16(const GLYPH* const glyph,const int x0,const int y0)
{
    int byend = y0 + glyph->height;
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    if (last_monotrans_alpha_color!=alpha_color)
    {
        int alpha1 = (255-alpha)*257;
        int r = (alpha_color>>16)&0xFF;
        int g = (alpha_color>>8)&0xFF;
        int b = alpha_color&0xFF;
        int r_a = r*alpha*257+256;
        int g_a = g*alpha*257+256;
        int b_a = b*alpha*257+256;
        int n = 0;
        for (; n<256; n++)
        {
            rtrans[n] = (n*alpha1 + r_a)>>16;
            gtrans[n] = (n*alpha1 + g_a)>>16;
            btrans[n] = (n*alpha1 + b_a)>>16;
        }
        last_monotrans_alpha_color = alpha_color;
    }

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_MONO_RLE7
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        int by = y0;
        unsigned char* rlepos = glyph->bmp+1;
        for (; by<ct; by++) { int bx = x0; while (bx<xr) bx += (*rlepos++)&127; }

        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + x0;
            unsigned short *addr_l = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned short *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int n = (*rlepos)&127;
                if ( (*rlepos)&128 )
                {
                    unsigned short *addr2 = addr + n;
                    while (addr<addr2)
                    {
                        if (addr>=addr_l && addr<addr_r)
                        {
                            unsigned short value = bmp_read16(addr);
                            bmp_write16(addr,makecol16(rtrans[getr16(value)],gtrans[getg16(value)],btrans[getb16(value)]));
                        }
                        addr++;
                    }
                }
                else { addr += n; }
                rlepos++;
            }
        }
    }
    else /*if (glyph->bmp[0]==GLYPH_MONO_BITPACK)*/
#endif  /* GLYPH_TARGET_KNOWS_MONO_RLE7 */
    {
        int pitch = (glyph->width+7)>>3;
        int by = (y0 >= ct) ? y0 : ct;

        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + x0;
            unsigned short *addr_l = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned char* a = glyph->bmp + 1 + pitch*(by-y0);
            unsigned char* aend = a + pitch;
            unsigned short v;
            while (a<aend && addr<addr_r)
            {
                if (*a&0x80 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x40 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x20 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x10 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x08 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x04 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x02 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                if (*a&0x01 && addr>=addr_l && addr<addr_r)
                    { v = bmp_read16(addr); bmp_write16(addr,makecol16(rtrans[getr16(v)],gtrans[getg16(v)],btrans[getb16(v)])); }
                addr++;
                a++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}
#endif  /* GLYPH_TARGET_KNOWS_MONO_BITPACK */


#endif  /* included_from_glyph_to_allegro_c */
