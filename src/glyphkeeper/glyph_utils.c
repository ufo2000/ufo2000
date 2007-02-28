/*
 * glyph_utils.c  -  Glyph Keeper basic support routines.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



/*
 * Passing messages from Glyph Keeper to the application.
 */
void gk_set_messenger(void (*func)(const char* const))
{
    _gk_messenger = func;
}

static void _gk_msg(const char* const format,...)
{
    va_list arg;
    static char buf[1000] = "[glyph] ";
    if (!_gk_messenger) return;
    va_start(arg,format);
    vsprintf(buf+8,format,arg);
    va_end(arg);
#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"msg: %s\n",buf);
#endif
    _gk_messenger(buf);
}


/*
 * Setting font search path
 */
void gk_set_font_path(const char* const path)
{
    _gk_font_path = path;
}


/*
 * Setting exit handler.
 */
static void _gk_install_exit_handler()
{
    if (!_gk_atexit_installed)
    {
        if (atexit(gk_library_cleanup)==0)
            _gk_atexit_installed = 1;
        else
            _gk_msg("Error: Can't install exit handler with atexit().\n");
    }
}


/*
 * Returns size in bytes of one glyph.
 * Both size of GLYPH structure and size of glyph bitmap are counted.
 */
static size_t glyph_size_in_bytes(const GLYPH* const g)
{
    return sizeof(GLYPH) + g->bmpsize;
}


/*
 * UTF-8 decoder.
 * (taken from Allegro)
 */
static unsigned _gk_utf8_getx(const char** s)
{
    int c;
    CARE(s);
    c = *((unsigned char *)((*s)++));
    if (c&0x80)
    {
        int n = 1;
        while (c&(0x80>>n)) n++;
        c &= (1<<(8-n))-1;
        while (--n>0)
        {
            int t = *((unsigned char *)((*s)++));
            if ((!(t&0x80))||(t&0x40)) { (*s)--; return '?'; }
            c = (c<<6)|(t&0x3F);
        }
    }
    return c;
}


/*
 * UTF-16 decoding functions.
 * 's' and '*s' must be not null.
 */
static void _gk_utf16_start_decoding(const unsigned short** s)
{
    unsigned short a = **s;
    if (a == 0xFFFE) { _gk_utf16_opposite_byte_sex = 1; (*s)++; return; }
    _gk_utf16_opposite_byte_sex = 0;
    if (a == 0xFEFF) { (*s)++; }
}

static unsigned _gk_utf16_decode(const unsigned short** s)
{
    unsigned short a,b;
    const unsigned short* ss = *s;
    a = *ss;
    ss++;
    if (_gk_utf16_opposite_byte_sex) a = ((a&0xFF)<<8)|(a>>8);
    if (a <= 0xD7FF || a >= 0xE000) { *s = ss; return a; }
    if (a > 0xDBFF) { return 0; }
    b = *ss;
    ss++;
    if (_gk_utf16_opposite_byte_sex) b = ((b&0xFF)<<8)|(b>>8);
    if (b < 0xDC00 || b > 0xDFFF) { return 0; }
    return 0x10000 + ( (((int)a&0x3FF)<<10) | ((int)b&0x3FF) );
}


/*
 * UTF-32 decoding functions.
 * 's' and '*s' must be not null.
 */
static void _gk_utf32_start_decoding(const unsigned int** s)
{
    unsigned int a = **s;
    if (a == 0xFFFE0000) { _gk_utf32_opposite_byte_sex = 1; (*s)++; return; }
    _gk_utf32_opposite_byte_sex = 0;
    if (a == 0x0000FEFF) { (*s)++; }
}

static unsigned _gk_utf32_decode(const unsigned int** s)
{
    unsigned int a = **s;
    if (_gk_utf32_opposite_byte_sex)
    {
        a = ((a&0x000000FF)<<24) | ((a&0x0000FF00)<<8) | ((a&0x00FF0000)>>8) | (a>>24);
    }
    if (a <= 0xD7FF || a >= 0xE000) { (*s)++; return a; }
    else { return 0; }
}


#endif  /* included_from_glyph_c */
