/*
 * glyph_global_vars.c  -  Glyph Keeper global variables.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



/*
 * Version guard.
 */
static const int _gk_version_major = 0;
static const int _gk_version_minor = 32;
static const int _gk_version_patch = 0;
static const int _gk_target = GLYPH_TARGET;

/*
 * Log-file,
 */
#ifdef GLYPH_LOG
static FILE* glyph_log = 0;
#endif

/*
 * Messenger function.
 */
static void (*_gk_messenger)(const char* const) = 0;



/*
 * Font path
 */
static const char* _gk_font_path = 0;

/*
 * Name of currently working routine, used for error reporting.
 * (FIXME: It is actually bad way to report errors.)
 */
static char* funcname = "";

/*
 * This variable is set to 1, if gk_library_cleanup()
 * shutdown handler is already installed with atexit().
 */
static int _gk_atexit_installed = 0;

/*
 * Internal buffer used for RLE compression.
 */
static unsigned char* rle_buffer = 0;
static unsigned rle_buffer_size = 0;



/*
 * FT_Bitmap for processing glyph rendered by FreeType.
 */
static FT_Bitmap _gk_workout_bitmap;



/*
 * Endianness flag for UTF-16 decoding.
 * 0 if UTF-16 string being processed currently has same
 * byte order with the machine the program's running on.
 * 1 if the UTF-16 string being decoded currently has
 * wrong byte order, and need byte swapping.
 */
static int _gk_utf16_opposite_byte_sex = 0;

/*
 * Endianness flag for UTF-32 decoding.
 */
static int _gk_utf32_opposite_byte_sex = 0;



/*
 * FreeType library instance handle.
 */
static FT_Library ftlib = 0;



/*
 * Number of currently used GLYPH_FACE objects.
 */
static unsigned face_count = 0;

/*
 * First face ID number available.
 */
static unsigned face_first_free_id = 1;



/*
 * Global list of GLYPH_FACE objects.
 */
static GLYPH_FACE *first_face = 0, *last_face = 0;

/*
 * Global list of renderers.
 */
static GLYPH_REND *first_renderer = 0, *last_renderer = 0;

/*
 * Global list of GLYPH_KEEP objects.
 */
static GLYPH_KEEP *first_keeper = 0, *last_keeper = 0;



/*
 * Default return string for unavailable font properties.
 */
static const char _gk_na[] = "UNAVAILABLE";



/*
 * True measures of memory allocated by Glyph Keeper and FreeType.
 */
size_t _gk_allocated_by_gk = 0;
size_t _gk_overhead_by_gk = 0;
size_t _gk_allocated_by_ft = 0;
size_t _gk_overhead_by_ft = 0;


#endif  /* included_from_glyph_c */
