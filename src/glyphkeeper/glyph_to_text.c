/*
 * glyph_to_text.c  -  Glyph Keeper driver for FILE stream IO.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



static FILE* file = 0;


static void put_glyph_mono(const GLYPH* const glyph)
{
    char c = '*';
    int y = 0;
    if (glyph->bmp[0]==GLYPH_MONO_RLE7)
    {
        unsigned char* rlepos = glyph->bmp+1;
        for (; y<glyph->height; y++)
        {
            int x = 0;
            while (x<glyph->width)
            {
                int cc = (*rlepos)&128 ? c : ' ';
                int x2 = x + ((*rlepos)&127);
                for (; x<x2; x++) fputc(cc,file);
                rlepos++;
            }
            fputc('\n',file);
        }
    }
    else
    {
        int pitch = (glyph->width+7)>>3;
        for (; y<glyph->height; y++)
        {
            int x = 0;
            unsigned char* a = glyph->bmp + 1 + pitch*y;

            unsigned char* aend = a + pitch;
            while (a<aend && x<glyph->width)
            {
                if (*a&0x80) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x40) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x20) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x10) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x08) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x04) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x02) fputc(c,file); else fputc(' ',file); x++;
                if (*a&0x01) fputc(c,file); else fputc(' ',file); x++;
                a++;
            }
            fputc('\n',file);
        }
    }
}


/*
 * This function prints out the glyph as a text into the FILE* stream.
 * Although in text mode, the glyph is still antialiased (kind of).
 *
 * Parameters 'text_color' and 'back_color' are actually not used.
 */
static void put_glyph_aa(const GLYPH* const glyph)
{
    int y = 0;
    if (glyph->bmp[0]==GLYPH_RLEAA)
    {
        unsigned char* rlepos = glyph->bmp + 1;
        for (; y<glyph->height; y++)
        {
            int x = 0;
            while (x<glyph->width)
            {
                if (*rlepos==0 || *rlepos==255)
                {
                    char c = *rlepos ? '@' : ' ';
                    int x2;
                    rlepos++;
                    x2 = x + *rlepos;
                    for (; x<x2; x++) fputc(c,file);
                    rlepos++;
                }
                else
                {
                    if (*rlepos<64) fputc('.',file);
                    else if (*rlepos<128) fputc(':',file);
                    else if (*rlepos<192) fputc('+',file);
                    else fputc('a',file);
                    rlepos++;
                    x++;
                }
            }
            fputc('\n',file);
        }
    }
    else
    {
        unsigned char* rlepos = glyph->bmp + 1;
        for (; y<glyph->height; y++)
        {
            int x = 0;
            for (; x<glyph->width; x++)
            {
                if (*rlepos==0) fputc(' ',file);
                else if (*rlepos<64) fputc('.',file);
                else if (*rlepos<128) fputc(':',file);
                else if (*rlepos<192) fputc('+',file);
                else if (*rlepos<255) fputc('a',file);
                else fputc('@',file);
                rlepos++;
            }
            fputc('\n',file);
        }
    }
}


static void _gk_prepare_to_draw(FILE* const bmp,GLYPH_REND* const rend)
{
    CARE(bmp);
    CARE(rend);
    file = bmp;
    if (rend) {}
}


static void _gk_done_drawing()
{
}


/*
 * This function simply prints the given glyph to the output FILE* stream.
 * The coordinates 'x' and 'y' are not used at all.
 */
static void _gk_put_glyph(GLYPH* const glyph,const int x,const int y)
{
    CARE(file);
    CARE(glyph);
    if (x || y) {}

    fprintf(file,"--------- glyph for character U+%04X ---------\n",glyph->unicode);

    if (glyph->bmp)
    {
        CARE(glyph->bmpsize);
        if (glyph->bmp[0] >= GLYPH_MONO)
            put_glyph_mono(glyph);
        else
            put_glyph_aa(glyph);
    }

    if (!glyph->index) _gk_unload_glyph(glyph);
}


#endif  /* included_from_glyph_c */
