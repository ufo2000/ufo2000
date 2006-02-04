/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "stdafx.h"

#ifdef HAVE_FREETYPE
extern "C" {
#include "glyphkeeper/glyph.h"
}
#endif

#include "global.h"
#include "font.h"
#include "video.h"
#include "config.h"


/* 
 * ATTENTION! This code uses internal data structures of Allegro 4.0.2 and  
 * is guaranteed to break on attempt to compile with Allegro WIP or later releases. 
 *
**/


/** Unicode translation table. Glyph->Unicode code mapping.
 * First 128 glyphs come from original version ; second - from unofficial russification.
 * Value of 0x10000 designates unused glyphs. 
 * Data according to The Unicode Standard, version 3.2 , see http://www.unicode.org/ 
 */
static int uni_xlate_tab[256] = {
/* latin (basic latin + some of Latin 1 starts here*/

/* basic latin (ascii) */
    
        0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 
0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    
0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 
0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    
0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 
0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,

0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 
0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,

0x00A3, /* Pound sign (at the place of grave accent) */ 
    
    0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 
0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,

0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 
    
/* parts of Latin-1 */
0x00A9, /* Copyright sign */
0x00C7, /* Latin capital c with cedilla */
0x00FC, /* Latin small letter u with diaresis */
0x00E9, /* Latin small letter c with acute */
0x00E2, /* latin small letter a with circumflex */
0x00E4, /* latin small letter a with diaresis */
0x00E0, /* latin small letter a with grave */

0x10000, /* one unused glyph position */

0x00E7, /* Latin small letter c with cedilla */
0x00EA, /* latin small letter e with circumflex */
0x00EB, /* latin small letter e with diaresis */
0x00E8, /* latin small letter e with grave */
0x00EF, /* latin small letter i with diaresis ; should include 0x00CF here too */
0x00EE, /* latin small letter i with circumflex; should include 0x00CE here too */

0x10000, /* one unused glyph position */

0x00C4, /* latin capital letter a with diaresis */

0x10000, /* one unused glyph position */

0x00C9, /* latin capital letter e with acute ; include here 00C8 (grave) */

0x10000, 0x10000, /* two unused glyph positions */

0x00F4, /* latin small letter o with circumflex */
0x00F6, /* latin small letter o with diaresis */

0x10000, /* one unused glyph position */

0x00FB, /* latin small letter u with circumflex ; should include 0x00DB (capital) here too */

0x10000, 0x10000, /* two unused glyph positions */

0x00D6, /* latin capital letter o with diaresis */
0x00DC, /* latin capital letter u with diaresis  */

0x10000, 0x10000, 0x10000, /* three unused glyph positions */

0x00DF, /* latin small letter sharp s */

0x10000, /* one unused glyph position */

0x00E1, /* latin small letter a with acute */

/* cyrillic  begins here. */    

0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, /* 32 unused glyphs - same as first 32 of latin table.*/
0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 
0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 
0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 
0x0424 , /* Cyrillic Capital F */
0x0418 , /* Cyrillic Capital I */
0x0421 , /* Cyrillic Capital S*/
0x0412 , /* Cyrillic Capital V*/
0x0423 , /* Cyrillic Capital U*/ 
0x0410 , /* Cyrillic Capital A*/
0x041F , /* Cyrillic Capital P*/
0x0420 , /* Cyrillic Capital R*/
0x0428 , /* Cyrillic Capital SH*/
0x041E , /* Cyrillic Capital O*/
0x041B , /* Cyrillic Capital L*/
0x0414 , /* Cyrillic Capital D*/
0x042C , /* Cyrillic Capital muagkii znak*/
0x0422 , /* Cyrillic Capital T*/
0x0429 , /* Cyrillic Capital SHXE*/
0x0417 , /* Cyrillic Capital Ze*/
0x0419 , /* Cyrillic Capital I short*/
0x041A , /* Cyrillic Capital K*/
0x042B , /* Cyrillic Capital UII */
0x0415 , /* Cyrillic Capital E include 0x401 (capital yu) here. */
0x0413 , /* Cyrillic Capital G*/
0x041C , /* Cyrillic Capital M*/
0x0426 , /* Cyrillic Capital Tse*/
0x0427 , /* Cyrillic Capital CHe*/
0x041D , /* Cyrillic Capital N*/
0x042F , /* Cyrillic Capital Ya*/

0x045E , /* Belarusian Small U neskladovaya */
0x040E , /* Belarusian Capital U neskladovaya */
0x0456 , /* Belarusian Small I */
0x0406 , /* Belarusian Capital I */

0x0451 , /* Cyrillic Small Yo */
0x0401 , /* Cyrillic Capital Yo */
0x0444 , /* Cyrillic Small F */
0x0438 , /* Cyrillic Small I */
0x0441 , /* Cyrillic Small S*/
0x0432 , /* Cyrillic Small V*/
0x0443 , /* Cyrillic Small U*/ 
0x0430 , /* Cyrillic Small A*/
0x043F , /* Cyrillic Small P*/
0x0440 , /* Cyrillic Small R*/
0x0448 , /* Cyrillic Small SH*/
0x043E , /* Cyrillic Small O*/
0x043B , /* Cyrillic Small L*/
0x0434 , /* Cyrillic Small D*/
0x044C , /* Cyrillic Small muagkii znak*/
0x0442 , /* Cyrillic Small T*/
0x0449 , /* Cyrillic Small SHXE*/
0x0437 , /* Cyrillic Small Ze*/
0x0439 , /* Cyrillic Small I short*/
0x043A , /* Cyrillic Small K*/
0x044B , /* Cyrillic Small UII*/
0x0435 , /* Cyrillic Small E ; include 0x0451 (small yo) here. */
0x0433 , /* Cyrillic Small G*/
0x043C , /* Cyrillic Small M*/
0x0446 , /* Cyrillic Small Tse*/
0x0447 , /* Cyrillic Small CHe*/
0x043D , /* Cyrillic Small N*/
0x044F , /* Cyrillic Small Ya*/

0x10000, 0x10000, 0x10000, 0x10000, 0x10000,  /* 5 unused glyphs */


0x0431 , /* Cyrillic Small  B*/
0x0411 , /* Cyrillic Capital B*/
0x0436 , /* Cyrillic Small  Zhe*/
0x0416 , /* Cyrillic Capital Zhe*/
0x0445 , /* Cyrillic Small  Kh*/
0x0425 , /* Cyrillic Capital Kh*/
0x044A , /* Cyrillic Small  Tverdy Znak*/
0x042A , /* Cyrillic Capital Tverdy Znak*/
0x044D , /* Cyrillic Small  Ee*/
0x042D , /* Cyrillic Capital Ee*/
0x044E , /* Cyrillic Small  Ju*/
0x042E , /* Cyrillic Capital Ju*/
    

0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, /* 21 unused glyphs. */
0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 
0x10000, 0x10000, 0x10000, 0x10000, 0x10000 

};


/* Fontcache / storage structures :

We basically need two-storey lookup table where to hold 
a) initial data in form of unsigned char *data (font_w  font_h) + glyph_w
b) generated data in form of BITMAP * (or x,y,w if storing in single bitmap)

cache key : { fg_color,  textmode }
storing in 

font_height and space_width are stored separately
404char is U005E (^). 

Supported Unicode sections are hardcoded. Each section consists of:
1. Rect-pointers: tables of uaf_glyph_rect structures. 
2. Bitmap cache for different drawing parameters.

Actual bitmaps get created on a cache miss. The bitmaps are vertical stripes.
The rect-pointer tables get created on load of glyph data. 

Non-existing glyphs have negative width (in rect-pointer) (-1 in this implementation).
Because Allegro allows user to specify his own char to substitute 
for non-existent glyphs, we perform the search of a glyph 
for a character twice (second time for allegro_404_char),  
and draw nothing if they both fail.  
*/

/** Holds 2d pointers to a glyph on a bitmap. Glyph height is the same 
 * for all glyphs and is stored in both uaf_internal_data and FONT structs
 */
struct uaf_glyph_rect {
    short y,w;
};

/** Represents a fontcache entry for given texmode and fgcolor.
 * Holds two bitmaps with glyphs for U00 and U04 unicode sections. 
 * Also the key element - textmode and foreground color values.
 */
struct uaf_fontcache {
    BITMAP *U00, *U04;
    int tmode, fgcol;
};

/** Container for all the lowlevel font data. Holds original font data, 
 * rect pointers, fontcache root and miscellaneous data.
 */
struct uaf_internal_data {
    unsigned char  *origU00, *origU04; /* origdata point to copies of loaded original data. */
    uaf_glyph_rect rectU00[256], rectU04[256];
    int cachecount;
    uaf_fontcache *cache;
    
    int max_w, max_h;
    int space_width; /* width of U0020 character. say, 2/3 of maxwidth (16 or 8) */
    int base_color; /* color to use if blitting with fg color <0. */
};
/*
static void dump_font_info(AL_CONST FONT *f) {
    uaf_internal_data *idat = (uaf_internal_data *) (f->data);
    uaf_fontcache *thecache;
    int i;
    fprintf(stderr,"\nCache entries: %d",idat->cachecount);
    if (idat->cachecount > 0) {
        for (i=0; i<idat->cachecount; i++) {
            thecache = idat->cache + i;
            fprintf(stderr,"\n\tEntry %d: tmode %d fgcol %d",i, thecache->tmode, thecache->fgcol);
        }
    }    
}
*/
/** Creates fontcache entry for given textmode and fgcolor. 
 * Gets invoked on a cache miss.
 * Assumes that bitmaps are/should be/will be in system memory and are of 8 bit depth. 
 * @param f The 'this' pointer.
 * @param tmode The textmode.
 * @param fgcol The foreground color.
 * @return Pointer to created cache entry.
 * Glyph count in a section is hardcoded to be equal to glyph count in original 
 * glyph bitmaps - 128. 
 */
static uaf_fontcache * uaf_make_cacheentry(AL_CONST FONT *f, int tmode, int fgcol) {
    uaf_internal_data *idat = (uaf_internal_data *) (f->data);
    uaf_fontcache *newcache;

    newcache = (uaf_fontcache *)malloc(sizeof(uaf_fontcache) * (idat->cachecount+1));
        
    if (idat->cachecount > 0) {
        memmove(newcache, idat->cache, sizeof(uaf_fontcache) * (idat->cachecount));
        free(idat->cache);
        idat->cache = newcache;
        newcache = idat->cache + idat->cachecount;
    } else {
        idat->cache = newcache;
    }
    idat->cachecount++;
    newcache->tmode = tmode; 
    newcache->fgcol = fgcol;
    
    int x,y,origval;
    
    /* Construct the bitmaps to put into the cache. 
       Note that all the dimensions are retained, 
       and the same rect-pointers are used for all cache entries. 
    */
    newcache->U00 = create_bitmap(idat->max_w, idat->max_h * 128);
    newcache->U04 = create_bitmap(idat->max_w, idat->max_h * 128);
    ASSERT(newcache->U00 != NULL && newcache->U04 != NULL);
    if (tmode > 0) {
        clear_to_color(newcache->U00, tmode);
        clear_to_color(newcache->U04, tmode);
    } else {
        clear_to_color(newcache->U00, xcom1_color(0));
        clear_to_color(newcache->U04, xcom1_color(0));
    }

    int font_color_table[6] = {
        xcom1_darken_color(fgcol, 0),
        xcom1_darken_color(fgcol, 1),
        xcom1_darken_color(fgcol, 2),
        xcom1_darken_color(fgcol, 3),
        xcom1_darken_color(fgcol, 4),
        xcom1_darken_color(fgcol, 5),
    };

    for (y = 0; y < idat->max_h * 128 ; y++) {
        for (x=0; x<idat->max_w; x++) {
            origval = *(idat->origU00 + x + y * idat->max_w);
            if (origval > 0 && origval < 6) {
                putpixel(newcache->U00, x, y, font_color_table[origval]);
            }
            origval = *(idat->origU04 + x + y * idat->max_w);
            if (origval > 0 && origval < 6) {
                putpixel(newcache->U04, x, y, font_color_table[origval]);
            }
        }
    }
    return newcache;
}
/** Just returns the common glyph height.
 * @param f The 'this' pointer.
 * @return Glyph height.
 */ 
static int uaf_font_height(AL_CONST FONT *f) {
    return f->height;
}

/** Finds glyph width for given UTF16 character.
 * @param f The 'this' pointer.
 * @param ch The character.
 * @return Glyph width for given character.
 * allegro_404_char handling: If given char is not found, try to find allegro_404_char. 
 * It is assumed to be in UTF16 encoding. If it is not found, return 0.
 */
static int uaf_char_length(AL_CONST FONT *f, int ch) {
    uaf_internal_data *idat = (uaf_internal_data *) (f->data);

    if (ch == 0x0020) { // special space case.
        return idat->space_width;
    }
    
    switch (ch >> 8) {
        case 0: /* basic latin & latin-1 */
            if ( idat->rectU00[ch & 0xFF].w > 0 ) {
                return idat->rectU00[ch & 0xFF].w;
            } else {
                if ( ch != allegro_404_char) {
                    return f->vtable->char_length(f,allegro_404_char);
                } else {
                    return 0;
                }
            }
        case 4: /* cyrillic */
            if ( idat->rectU04[ch & 0xFF].w > 0 ) {
                return idat->rectU04[ch & 0xFF].w;
            } else {
                if ( ch != allegro_404_char) {
                    return f->vtable->char_length(f,allegro_404_char);
                } else {
                    return 0;
                }
            }
        default: /* unicode section not implemented. */
            if (ch != allegro_404_char) {
                return idat->rectU00[allegro_404_char].w;
            } else {
                return 0;
            }
    }
}

/** Calculates a string width in pixels using char_length routine. 
 * Copied verbatim from allegro sources, src/font.c; function length() 
 * @param f The 'this' pointer.
 * @param text The text.
 * @return String width in pixels.
 */
static int uaf_text_length(AL_CONST FONT *f, AL_CONST char *text) {
    int ch = 0, w = 0;
    AL_CONST char* p = text;
    ASSERT(text);
    ASSERT(f);
   
    while( (ch = ugetxc(&p)) ) {
        w += f->vtable->char_length(f, ch);
    }
   
    return w;   
}

/** Renders a glyph corresponding to given char at given position in given bitmap.
 *
 * @param f The 'this' pointer.
 * @param ch The character.
 * @param fg Foreground color. Currently five color indexes starting with this one are used to draw the glyph.
 * @param bg Background color, AKA textmode.
 * @param bmp Destination bitmap.
 * @param  x  X coordinate of top left corner of glyph bounding box.
 * @param  y  Y coordinate of top left corner of glyph bounding box.
 * @return Glyph width in pixels.
 */
static int uaf_render_char(AL_CONST FONT *f, int ch, int fg, int bg, BITMAP *bmp, int x, int y) {
    uaf_fontcache *thecache = NULL;
    uaf_internal_data *idat = (uaf_internal_data *) (f->data);
    int i, gotcache = 0, srcy, srcw, srch;
    BITMAP *srcbmp;
    
    /* dump_font_info(f); */
    
    if (ch == 0x0020) { /* skip space. */
        return idat->space_width;
    }
    
    if (fg < 0) {
        fg = idat->base_color;
    }
    
    if ( idat->cachecount != 0 )  {
        /* try to find matching cache entry. */
        for (i=0; i<idat->cachecount; i++) {
            thecache = idat->cache + i;
            if ( (thecache->tmode == bg) && (thecache->fgcol == fg) ) {
                gotcache = 1;
                break;
            }
        }
    }
    
    /* cache miss: create */
    if ((!gotcache) || idat->cachecount == 0 ) {
        thecache = uaf_make_cacheentry(f, bg, fg);
    }
    
    srch = f->height;
    
    switch (ch >> 8) {
        case 0: /* basic latin & latin-1 */
            if (idat->rectU00[ch & 0xFF].w >= 0) {
                srcbmp = thecache->U00;
                srcw = idat->rectU00[ch & 0xFF].w;
                srcy = idat->rectU00[ch & 0xFF].y;
                break;
            } else { /* w<0 : glyph does not exist. */
                if (ch != allegro_404_char) {
                    return f->vtable->render_char(f, allegro_404_char, fg, bg, bmp, x, y);
                } else {
                    return 0; /* prevent loop */
                }
            }
            
        case 4: /* cyrillic */
            if (idat->rectU04[ch & 0xFF].w >= 0) {
                srcbmp = thecache->U04;
                srcw = idat->rectU04[ch & 0xFF].w;
                srcy = idat->rectU04[ch & 0xFF].y;
                break;
            } else { /* w<0 : glyph does not exist. */
                if (ch != allegro_404_char) {
                    return f->vtable->render_char(f, allegro_404_char, fg, bg, bmp, x, y);
                } else {
                    return 0; /* prevent loop */
                }
            }
        default: /* unicode section not implemented. */
            if (ch != allegro_404_char) {
                return f->vtable->render_char(f, allegro_404_char, fg, bg, bmp, x, y);
            } else {
                return 0;
            }
    }
    if (srcw > 0) {
        /* do ze blit. Fgcol is already normalized. */
        acquire_bitmap(bmp);
        
        if (bg > 0) {
            blit(srcbmp, bmp, 0, srcy, x, y, srcw, f->height);
        } else {
            masked_blit(srcbmp, bmp, 0, srcy, x, y, srcw, f->height);
        }
        
        release_bitmap(bmp);
    }
    return srcw;
}

/** Renders a string. @see uaf_render_char, as this one does no more than 
 * a series of calls to it.
 */
static void uaf_render(AL_CONST FONT *f, AL_CONST char *text, int fg, int bg, BITMAP *bmp, int x, int y) {
    AL_CONST char* p = text;
    int ch = 0;

    while( (ch = ugetxc(&p)) ) {
        x += f->vtable->render_char(f, ch, fg, bg, bmp, x, y);
    }
}

/** Frees all memory and resources used.
 * @param f The 'this' pointer.
 */
static void uaf_destroy(FONT *f) {
    uaf_internal_data *idat = (uaf_internal_data *) (f->data);
    
    free(idat->origU00);
    
    if (idat->origU04 != NULL) 
        free(idat->origU04);
    
    if (idat->cachecount > 0) {
        for(int i=0; i < idat->cachecount; i++) {
            if (idat->cache[i].U00 != NULL) 
                destroy_bitmap(idat->cache[i].U00);
            if (idat->cache[i].U04 != NULL) 
                destroy_bitmap(idat->cache[i].U04);
        }
        free(idat->cache);
    }
    free(f->data);
}

/** Creates a FONT structure to be ready for use. 
 * @param data00 Original glyph data for basic latin
 * @param data04 Original glyph data for cyrillic. Optional. NULL here disables cyrillic.
 * @param w Glyph bitmap width. Actual width may be smaller - blank columns 
 * on the right side are suppressed.
 * @param h Glyph height.
 * @param base_color Color to consider "supplied with the font"
 * @return Pointer to created FONT structure, ready to use in allegro text routines.
 * 
 * Both pointers are assumed to point to buffers of at least 128 * w * h bytes.
 */
static FONT *create_font(unsigned char *data00, unsigned char *data04, int w, int h, int base_color) {
    uaf_internal_data *idata;
    FONT *f;
    
    f = (FONT *) malloc(sizeof(FONT));
    ASSERT(f != NULL);
    f->vtable = (FONT_VTABLE *) malloc(sizeof(FONT_VTABLE));
    ASSERT(f->vtable != NULL);
    
    idata = (uaf_internal_data *) malloc(sizeof(uaf_internal_data));
    ASSERT(idata != NULL);
    
    f->height = h;
    f->data = idata;
    
    idata->origU00 = (unsigned char *) malloc(w*h*128);
    ASSERT(idata->origU00 != NULL);
        memmove(idata->origU00, data00, w*h*128);
        
        if (data04 != NULL) {
                idata->origU04 = (unsigned char *) malloc(w*h*128);
                ASSERT(idata->origU04 != NULL);
                memmove(idata->origU04, data04, w*h*128);
        } else {
                idata->origU04 = NULL;
        }
    
    idata->cachecount = 0;
    idata->space_width = w / 2;
    
    idata->max_w = w;
    idata->max_h = h;
    idata->base_color = base_color;
    
    f->vtable->font_height = uaf_font_height;
    f->vtable->char_length = uaf_char_length;
    f->vtable->text_length = uaf_text_length;
    f->vtable->render_char = uaf_render_char;
    f->vtable->render = uaf_render;
    f->vtable->destroy = uaf_destroy;
    
    int i;
    /* initialize rect-pointer tables with "unknown" glyphs */
    for(i=0; i<256; i++) {
        idata->rectU00[i].w = -1;
        idata->rectU04[i].w = -1;
    }
    
    
    int U, Ul, Uh, row, col, curw, maxcolw;
    /* precalculate font widths in origUxx members. */
    for(i=0; i<128; i++) { // orig font
        U = uni_xlate_tab[i];
        if (U == 0x10000) { // unused glyph. skip.
            continue;
        }
        /* Calculate glyph width. All but one empty columns at the right 
           side are stripped. Calculate maximum width of the glyph, and 
           set actual width to be one more than the value we've got, 
           but w at most.
        */
        maxcolw = 0; 
        for (row=0; row <h; row++) {
                        curw = 0;
            for (col=0; col<w; col++) {
                if ( idata->origU00[i*w*h + row*w + col] != 0 ) {
                    curw = col + 1;
                }
            }
            if (maxcolw < curw) {
                maxcolw = curw;
            }
        }
        curw = maxcolw < w ? maxcolw - 1 : w - 1;
        /* figure out where to put it. */
        Ul = U & 0xFF;
        Uh = U >> 8;
        if (Uh == 0) {
            idata->rectU00[Ul].y = i*h;
            idata->rectU00[Ul].w = curw;
            continue;
        }
        if ((Uh == 4 ) && (data04 != NULL)) {
            idata->rectU04[Ul].y = i*h;
            idata->rectU04[Ul].w = curw;
            continue;
        }
        /* hmm. unsupported section. skip.*/
        continue; 
    }
    if (data04 != NULL) { /* if we have the cyr data ... */
                for(i=0; i<128; i++) { /* unofcyr font */
                        U = uni_xlate_tab[i + 128];
                        if (U == 0x10000) { /* unused glyph. skip. */
                                continue;
                        }
                        maxcolw = 0;
                        for (row=0; row <h; row++) {
                                curw = 0;
                                for (col=0; col<w; col++) {
                                        if ( idata->origU04[i*w*h + row*w + col] != 0 ) {
                                                curw = col + 1;
                                        }
                                }
                                if (maxcolw < curw) {
                                        maxcolw = curw;
                                }
                        }
                        curw = maxcolw < w ? maxcolw - 1 : w - 1;
                        /* figure out where to put it. */
                        Ul = U & 0xFF;
                        Uh = U >> 8;
                        if (Uh == 0) {
                                idata->rectU00[Ul].y = i*h;
                                idata->rectU00[Ul].w = curw;
                                continue;
                        }
                        if ((Uh == 4 ) && (data04 != NULL)) {
                                idata->rectU04[Ul].y = i*h;
                                idata->rectU04[Ul].w = curw;
                                continue;
                        }
                        /* hmm. unsupported section. skip.*/
                        continue; 
                }   
        }
    
    return f;
}

/* UFO2000-specific code below. */

/** 'Large' UFO font. */
FONT *large = NULL;

#ifdef HAVE_FREETYPE
GLYPH_FACE *g_large_font_face;
GLYPH_KEEP *g_large_font_keep;
GLYPH_REND *g_large_font_rend;
#endif

/** 'Small' UFO font. */
FONT *g_small_font = NULL;
#ifdef HAVE_FREETYPE
GLYPH_FACE *g_small_font_face;
GLYPH_KEEP *g_small_font_keep;
GLYPH_REND *g_small_font_rend;
#endif

FONT *g_console_font = NULL;
#ifdef HAVE_FREETYPE
static GLYPH_FACE *g_console_font_face;
static GLYPH_KEEP *g_console_font_keep;
static GLYPH_REND *g_console_font_rend;
#endif
static int g_console_font_size = 9;

void set_console_font_size(int size)
{
    if (size < 9) size = 9;
    g_console_font_size = size;
#ifdef HAVE_FREETYPE    
    if (g_console_font_rend) {
        gk_rend_set_size_pixels(g_console_font_rend, g_console_font_size, g_console_font_size);
    }
#endif  
}

int get_console_font_size()
{
    return g_console_font_size;
}

static bool create_xcom_small_font()
{
    int fh = open(F("$(xcom)/geodata/smallset.dat"), O_RDONLY | O_BINARY);
    if (fh == -1) return false;
    int fl = filelength(fh);
    unsigned char *dat_lat = new unsigned char[fl];
    unsigned char *dat_cyr = (unsigned char *)datafile[DAT_SMALLSET_CYR].dat;
    read(fh, dat_lat, fl);
    close(fh);
    g_small_font = create_font(dat_lat, dat_cyr, 8, 9, 1);
    delete []dat_lat;
    return true;
}

static bool create_xcom_large_font()
{
    int fh = open(F("$(xcom)/geodata/biglets.dat"), O_RDONLY | O_BINARY);
    if (fh == -1) return false;
    int fl = filelength(fh);
    unsigned char *dat_lat = new unsigned char[fl];
    unsigned char *dat_cyr = (unsigned char *)datafile[DAT_BIGLETS_CYR].dat;
    read(fh, dat_lat, fl);
    close(fh);
    large = create_font(dat_lat, dat_cyr, 16, 16, 1);
    delete []dat_lat;
    return true;
}

/** initializes fonts */
void create_fonts() 
{
#ifdef HAVE_FREETYPE    
    g_console_font_face = gk_load_face_from_file(F(cfg_get_console_font_file()), 0);
    if (g_console_font_face) {
        g_console_font_keep = gk_create_keeper(0, 0);
        g_console_font_rend = gk_create_renderer(g_console_font_face, g_console_font_keep);
        g_console_font = gk_create_allegro_font(g_console_font_rend);
        set_console_font_size(get_console_font_size());
    }
#endif
    if (!g_console_font) g_console_font = font;

    if (!create_xcom_small_font()) {
        // If we can't load x-com font, fallback to TTF font or 
        // as the last resort - to standard allegro bitmap font
#ifdef HAVE_FREETYPE    
        g_small_font_face = gk_load_face_from_file(F("$(ufo2000)/fonts/DejaVuSansMono.ttf"), 0);
        g_small_font_keep = gk_create_keeper(0, 0);
        g_small_font_rend = gk_create_renderer(g_small_font_face, g_small_font_keep);
        gk_rend_set_size_pixels(g_small_font_rend, 8, 9);
        g_small_font = gk_create_allegro_font(g_small_font_rend);
#endif
        if (!g_small_font) g_small_font = font;
    }
    
    if (!create_xcom_large_font()) {
        // If we can't load x-com font, fallback to TTF font or 
        // as the last resort - to standard allegro bitmap font
#ifdef HAVE_FREETYPE
        g_large_font_face = gk_load_face_from_file(F("$(ufo2000)/fonts/DejaVuSansMono.ttf"), 0);
        g_large_font_keep = gk_create_keeper(0, 0);
        g_large_font_rend = gk_create_renderer(g_large_font_face, g_large_font_keep);
        gk_rend_set_size_pixels(g_large_font_rend, 15, 16);
        large = gk_create_allegro_font(g_large_font_rend);
#endif
        if (!large) large = font;
    }
}

/** Destroys fonts.
 *  Uses allegro function to reach destructor in the vtable. 
 */
void destroy_fonts() 
{
    if (g_small_font != font)
        destroy_font(g_small_font);
    if (large != font)
        destroy_font(large);
    if (g_console_font != font)
        destroy_font(g_console_font);
#ifdef HAVE_FREETYPE
    if (g_console_font_rend) gk_done_renderer(g_console_font_rend);
    if (g_small_font_rend) gk_done_renderer(g_small_font_rend);
    if (g_large_font_rend) gk_done_renderer(g_large_font_rend);
    
    if (g_console_font_keep) gk_done_keeper(g_console_font_keep);
    if (g_small_font_keep) gk_done_keeper(g_small_font_keep);
    if (g_large_font_keep) gk_done_keeper(g_large_font_keep);
    
    if (g_console_font_face) gk_unload_face(g_console_font_face);
    if (g_small_font_face) gk_unload_face(g_small_font_face);
    if (g_large_font_face) gk_unload_face(g_large_font_face);
    
    gk_library_cleanup();
#endif  
}

/** Prints a number in a 3x5 font onto a given position on the bitmap.
 * @param bmp Destination bitmap
 * @param x X coordinate
 * @param y Y coordiante
 * @param col Foreground color
 * @param value Value to print
*/
void printsmall_x(BITMAP *bmp, int x, int y, int col, int value)
{
    static char digit[10][5][4] = {
                                      {"000",      //0
                                       "0 0",
                                       "0 0",
                                       "0 0",
                                       "000"},

                                      {" 0 ",      //1
                                       "00 ",
                                       " 0 ",
                                       " 0 ",
                                       "000"},

                                      {"000",      //2
                                       "  0",
                                       "000",
                                       "0  ",
                                       "000"},

                                      {"000",      //3
                                       "  0",
                                       "000",
                                       "  0",
                                       "000"},

                                      {"0 0",      //4
                                       "0 0",
                                       "000",
                                       "  0",
                                       "  0"},

                                      {"000",      //5
                                       "0  ",
                                       "000",
                                       "  0",
                                       "000"},

                                      {"000",      //6
                                       "0  ",
                                       "000",
                                       "0 0",
                                       "000"},

                                      {"000",      //7
                                       "  0",
                                       "  0",
                                       "  0",
                                       "  0"},

                                      {"000",      //8
                                       "0 0",
                                       "000",
                                       "0 0",
                                       "000"},

                                      {"000",      //9
                                       "0 0",
                                       "000",
                                       "  0",
                                       "000"}
                                  };

    char buf[100];
    //itoa(value, buf, 10);
    sprintf(buf, "%d", value);

    for (unsigned int i = 0; i < strlen(buf); i++)
        for (int yy = 0; yy < 5; yy++)
            for (int xx = 0; xx < 3; xx++)
                if (digit[buf[i] - '0'][yy][xx] != ' ')
                    putpixel(bmp, x + i * 4 + xx, y + yy, col);

}
/** Prints a number with center alignment. @see printsmall. */
void printsmall_center_x(BITMAP *bmp, int x, int y, int col, int value)
{
    char buf[100];
    sprintf(buf, "%d", value);
    printsmall_x(bmp, x - strlen(buf) * 4 / 2, y, col, value);
}

/** Same as printsmall_center_x, but with a background. @see printsmall_center. */
void printsmall_center_back_x(BITMAP *bmp, int x, int y, int col, int back, int value)
{
    char buf[100];
    sprintf(buf, "%d", value);
    rectfill(bmp, x - strlen(buf) * 4 / 2 - 1, y - 1, x + strlen(buf) * 4 / 2 - 1, y + 5, back);
    printsmall_x(bmp, x - strlen(buf) * 4 / 2, y, col, value);
}
