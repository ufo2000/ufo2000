/*
 * glyph_to_allegro_aa.c  -  part of Glyph Keeper Allegro driver.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_to_allegro_c


static int last_aa_alpha_used = -1;
static unsigned char gradient[256];


/*
 * General case code for printing an anti-aliased glyph to Allegro bitmap.
 */
static void put_glyph_aa(const GLYPH* const glyph,const int x0,const int y0)
{
    int bxend = x0 + glyph->width;
    int byend = y0 + glyph->height;
    if (byend>cb) byend = cb;

    if (last_aa_alpha_used != alpha)
    {
        int a1 = (alpha<<16)/255;
        int i=0; for (; i<256; i++) gradient[i] = (i*a1)>>16;
        last_aa_alpha_used = alpha;
    }

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        unsigned char* rlepos = glyph->bmp + 1;
        int by = y0;
        for (; by<ct; by++)
        {
            int bx = x0, bx1 = x0 + glyph->width;
            while (bx<bx1) { if (*rlepos==0 || *rlepos==255) { rlepos++; bx += *rlepos++; } else { rlepos++; bx++; } }
        }

        for (; by<byend; by++)
        {
            int bx = x0;
            while (bx<bxend)
            {
                if (*rlepos==0) { rlepos++; bx += *rlepos++; }
                else
                {
                    int a = gradient[*rlepos];
                    if (a==255) { solid_mode(); }
                    else { drawing_mode(DRAW_MODE_TRANS,0,0,0); set_trans_blender(0,0,0,a); }
                    if (*rlepos==255)
                    {
                        int bx2;
                        rlepos++;
                        bx2 = bx + *rlepos;
                        while (bx<bx2) { putpixel(bmp,bx++,by,text_color); }
                        rlepos++;
                    }
                    else
                    {
                        rlepos++;
                        putpixel(bmp,bx++,by,text_color);
                    }
                }
            }
        }
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */
    {
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            int bx = x0;
            unsigned char* bmppos = glyph->bmp + 1 + glyph->width*(by-y0);
            while (bx<bxend)
            {
                int a = gradient[*bmppos++];
                if (a==255) { solid_mode(); }
                else { drawing_mode(DRAW_MODE_TRANS,0,0,0); set_trans_blender(0,0,0,a); }
                if (a>0) putpixel(bmp,bx,by,text_color);
                bx++;
            }
        }
    }

    solid_mode();
}


/*
 * Fast code for printing an anti-aliased glyph to 32-bit bitmap.
 */
static void put_glyph_aa_32(const GLYPH* const glyph,const int x0,const int y0)
{
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    int byend = y0 + glyph->height;
    int r = (alpha_color>>16)&0xFF;
    int g = (alpha_color>>8)&0xFF;
    int b = alpha_color&0xFF;

    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    if (last_aa_alpha_used != alpha)
    {
        int a1 = (alpha<<16)/255;
        int i=0; for (; i<256; i++) gradient[i] = (i*a1)>>16;
        last_aa_alpha_used = alpha;
    }

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        int alpha1 = (255-alpha)*257;
        int r_a = r*alpha*257+256;
        int g_a = g*alpha*257+256;
        int b_a = b*alpha*257+256;

        unsigned char* rlepos = glyph->bmp + 1;
        int by = y0;
        for (; by<ct; by++)
        {
            int bx = x0, bx1 = x0 + glyph->width;
            while (bx<bx1) { if (*rlepos==0 || *rlepos==255) { rlepos++; bx += *rlepos++; } else { rlepos++; bx++; } }
        }

        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                if (*rlepos==0) { rlepos++; addr += *rlepos++; }
                else
                {
                    int a = gradient[*rlepos];
                    if (*rlepos==255)
                    {
                        unsigned *addr2;
                        rlepos++;
                        addr2 = addr + *rlepos;
                        if (a==255)
                            while (addr<addr2) { if (addr>=addr_l && addr<addr_r) { bmp_write32(addr,text_color); } addr++; }
                        else
                            while (addr<addr2)
                            {
                                if (addr>=addr_l && addr<addr_r)
                                {
                                    unsigned int value = bmp_read32(addr);
                                    bmp_write32(addr,makecol32((getr32(value)*alpha1+r_a)>>16,
                                                               (getg32(value)*alpha1+g_a)>>16,
                                                               (getb32(value)*alpha1+b_a)>>16));
                                }
                                addr++;
                            }
                        rlepos++;
                    }
                    else
                    {
                        rlepos++;
                        if (addr>=addr_l && addr<addr_r)
                        {
                            int a1 = (255-a)*257;
                            int r_a1 = r*a*257+256;
                            int g_a1 = g*a*257+256;
                            int b_a1 = b*a*257+256;
                            unsigned int value = bmp_read32(addr);
                            bmp_write32(addr,makecol32((getr32(value)*a1+r_a1)>>16,
                                                       (getg32(value)*a1+g_a1)>>16,
                                                       (getb32(value)*a1+b_a1)>>16));
                        }
                        addr++;
                    }
                }
            }
        }
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */
    {
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + xl;
            unsigned *addr_r = line + xr;
            unsigned char* bmppos = glyph->bmp + 1 + glyph->width*(by-y0) + (xl-x0);
            while (addr<addr_r)
            {
                int a = gradient[*bmppos++];
                if (a==255) { bmp_write32(addr,text_color); }
                else if (a>0)
                {
                    int a1 = (255-a)*257;
                    int r_a1 = r*a*257+256;
                    int g_a1 = g*a*257+256;
                    int b_a1 = b*a*257+256;
                    unsigned int value = bmp_read32(addr);
                    bmp_write32(addr,makecol32((getr32(value)*a1+r_a1)>>16,
                                               (getg32(value)*a1+g_a1)>>16,
                                               (getb32(value)*a1+b_a1)>>16));
                }
                addr++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}



/*
 * Printing an anti-aliased glyph to 32-bit bitmap with known background color.
 */
static void put_glyph_aa_32_back(const GLYPH* const glyph,const int x0,const int y0)
{
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    int byend = y0 + glyph->height;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;
    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        unsigned char* rlepos = glyph->bmp + 1;
        int by = y0;
        for (; by<ct; by++)
        {
            int bx = x0, bx1 = x0 + glyph->width;
            while (bx<bx1) { if (*rlepos==0 || *rlepos==255) { rlepos++; bx += *rlepos++; } else { rlepos++; bx++; } }
        }

        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + x0;
            unsigned *addr_l = line + xl;
            unsigned *addr_r = line + xr;
            unsigned *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                int a = *rlepos++;
                if (a==0) { addr += *rlepos++; }
                else if (a<255) { if (addr>=addr_l && addr<addr_r) bmp_write32(addr,_gk_driver_back_to_front[a]); addr++; }
                else
                {
                    int ac = _gk_driver_back_to_front[a];
                    unsigned *addr2 = addr + *rlepos++;
                    while (addr<addr2) { if (addr>=addr_l && addr<addr_r) { bmp_write32(addr,ac); } addr++; }
                }
            }
        }
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */
    {
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned *line = (unsigned*)bmp_write_line(bmp,by);
            unsigned *addr = line + xl;
            unsigned *addr_r = line + xr;
            unsigned char* bmppos = glyph->bmp + 1 + glyph->width*(by-y0) + (xl-x0);
            while (addr<addr_r)
            {
                int a = *bmppos++;
                if (a>0) bmp_write32(addr,_gk_driver_back_to_front[a]);
                addr++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}


/*
 * Fast code for printing an anti-aliased glyph to 16-bit bitmap.
 */
static void put_glyph_aa_16(const GLYPH* const glyph,const int x0,const int y0)
{
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    int byend = y0 + glyph->height;
    int r = (alpha_color>>16)&0xFF;
    int g = (alpha_color>>8)&0xFF;
    int b = alpha_color&0xFF;

    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    if (last_aa_alpha_used != alpha)
    {
        int a1 = (alpha<<16)/255;
        int i=0; for (; i<256; i++) gradient[i] = (i*a1)>>16;
        last_aa_alpha_used = alpha;
    }

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        int alpha1 = (255-alpha)*257;
        int r_a = r*alpha*257+256;
        int g_a = g*alpha*257+256;
        int b_a = b*alpha*257+256;

        unsigned char* rlepos = glyph->bmp + 1;
        int by = y0;
        for (; by<ct; by++)
        {
            int bx = x0, bx1 = x0 + glyph->width;
            while (bx<bx1) { if (*rlepos==0 || *rlepos==255) { rlepos++; bx += *rlepos++; } else { rlepos++; bx++; } }
        }

        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + x0;
            unsigned short *addr_l = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned short *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                if (*rlepos==0) { rlepos++; addr += *rlepos++; }
                else
                {
                    int a = gradient[*rlepos];
                    if (*rlepos==255)
                    {
                        unsigned short *addr2;
                        rlepos++;
                        addr2 = addr + *rlepos;
                        if (a==255)
                            while (addr<addr2) { if (addr>=addr_l && addr<addr_r) { bmp_write16(addr,text_color); } addr++; }
                        else
                            while (addr<addr2)
                            {
                                if (addr>=addr_l && addr<addr_r)
                                {
                                    unsigned short value = bmp_read16(addr);
                                    bmp_write16(addr,makecol16((getr16(value)*alpha1+r_a)>>16,
                                                               (getg16(value)*alpha1+g_a)>>16,
                                                               (getb16(value)*alpha1+b_a)>>16));
                                }
                                addr++;
                            }
                        rlepos++;
                    }
                    else
                    {
                        rlepos++;
                        if (addr>=addr_l && addr<addr_r)
                        {
                            int a1 = (255-a)*257;
                            int r_a1 = r*a*257+256;
                            int g_a1 = g*a*257+256;
                            int b_a1 = b*a*257+256;
                            unsigned short value = bmp_read16(addr);
                            bmp_write16(addr,makecol16((getr16(value)*a1+r_a1)>>16,
                                                       (getg16(value)*a1+g_a1)>>16,
                                                       (getb16(value)*a1+b_a1)>>16));
                        }
                        addr++;
                    }
                }
            }
        }
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */
    {
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned short *line = (unsigned short*)bmp_write_line(bmp,by);
            unsigned short *addr = line + xl;
            unsigned short *addr_r = line + xr;
            unsigned char *bmppos = glyph->bmp + 1 + glyph->width*(by-y0) + (xl-x0);
            while (addr<addr_r)
            {
                int a = gradient[*bmppos++];
                if (a==255) { bmp_write16(addr,text_color); }
                else if (a>0)
                {
                    int a1 = (255-a)*257;
                    int r_a1 = r*a*257+256;
                    int g_a1 = g*a*257+256;
                    int b_a1 = b*a*257+256;
                    unsigned short value = bmp_read16(addr);
                    bmp_write16(addr,makecol16((getr16(value)*a1+r_a1)>>16,
                                               (getg16(value)*a1+g_a1)>>16,
                                               (getb16(value)*a1+b_a1)>>16));
                }
                addr++;
            }
        }
    }

    bmp_unwrite_line(bmp);
}


/*
 * Fast code for printing an anti-aliased glyph to 8-bit bitmap.
 */
static void put_glyph_aa_8(const GLYPH* const glyph,const int x0,const int y0)
{
    int xl = (x0 >= cl) ? x0 : cl;
    int xr = x0 + glyph->width;
    int byend = y0 + glyph->height;
    if (xr > cr) xr = cr;
    if (byend>cb) byend = cb;

    bmp_select(bmp);

#ifdef GLYPH_TARGET_KNOWS_RLEAA
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        unsigned char* rlepos = glyph->bmp + 1;
        int by = y0;
        for (; by<ct; by++)
        {
            int bx = x0, bx1 = x0 + glyph->width;
            while (bx<bx1) { if (*rlepos==0 || *rlepos==255) { rlepos++; bx += *rlepos++; } else { rlepos++; bx++; } }
        }

        for (; by<byend; by++)
        {
            unsigned char *line = (unsigned char*)bmp_write_line(bmp,by);
            unsigned char *addr = line + x0;
            unsigned char *addr_l = line + xl;
            unsigned char *addr_r = line + xr;
            unsigned char *addr_end = addr + glyph->width;
            while (addr<addr_end)
            {
                if (*rlepos==0) { rlepos++; addr += *rlepos++; }
                else
                {
                    if (*rlepos==255)
                    {
                        unsigned char *addr2;
                        rlepos++;
                        addr2 = addr + *rlepos;
                        while (addr<addr2) { if (addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color); addr++; }
                        rlepos++;
                    }
                    else
                    {
                        if (*rlepos>127 && addr>=addr_l && addr<addr_r) bmp_write8(addr,text_color);
                        rlepos++;
                        addr++;
                    }
                }
            }
        }
    }
    else
#endif  /* GLYPH_TARGET_KNOWS_RLEAA */
    {
        int by = (y0 >= ct) ? y0 : ct;
        for (; by<byend; by++)
        {
            unsigned char *line = (unsigned char*)bmp_write_line(bmp,by);
            unsigned char *addr = line + xl;
            unsigned char *addr_r = line + xr;
            unsigned char *bmppos = glyph->bmp + 1 + glyph->width*(by-y0) + (xl-x0);
            while (addr<addr_r) { if (*bmppos>127) bmp_write8(addr,text_color); bmppos++; addr++; }
        }
    }

    bmp_unwrite_line(bmp);
}


#endif  /* included_from_glyph_to_allegro_c */
