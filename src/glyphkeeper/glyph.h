/*
 * glyph.h  -  Glyph Keeper API header file
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


#ifndef glyph_h_included
#define glyph_h_included


/*
 * Glyph Keeper identification
 */
#define GK_VERSION_MAJOR  0
#define GK_VERSION_MINOR  32
#define GK_VERSION_PATCH  0
#define GK_VERSION_STR    "0.32.0"
#define GK_DATE_STR       "2007-02-06"



/*
 * Define this macro to enable more safety checks.
 * Useful for debugging, but results in a little slower code.
 *
 * Note: Defining this macro will affect Glyph Keeper, not your program.
 * Enabling/disabling this macro has effect only after recompiling the
 * Glyph Keeper library.
 */
/*#define GLYPH_SAFE*/


/*
 * Define this macro to enable Glyph Keeper internal logging.
 * This is used for debugging Glyph Keeper library, so use it only if you
 * want to debug Glyph Keeper, or to understand how it works.
 *
 * When you run a program linked to Glyph Keeper compiled with GLYPH_LOG
 * defined, a logfile "glyph.log" will be created in the curent directory.
 * Many things then will be written to that file, as your program calls
 * Glyph Keeper rountines.
 *
 * Note that enabling this option slows down the Glyph Keeper dramatically,
 * so don't attempt to run a benchmark with this option on.
 */
/*#define GLYPH_LOG*/


/*
 * Debugging Glyph Keeper and FreeType memory usage.
 */
/*#define GLYPH_DEBUG_GK_MEMORY*/
/*#define GLYPH_LOG_GK_MEMORY*/
/*#define GLYPH_DEBUG_FT_MEMORY*/
/*#define GLYPH_LOG_FT_MEMORY*/


/*
 * If you want to make Glyph Keeper DLL you should
 * define GLYPH_DLL when compiling Glyph Keeper.
 * This will give  __declspec(dllexport)  to all Glyph Keeper API fuctions.
 *
 * Feel free to modify this if your compiler wants some other declaration.
 * But in that case please send me back your changes, so I can include them
 * in next version.
 */
#undef GLYPH_DECLSPEC

#ifdef GLYPH_DLL
#define GLYPH_DECLSPEC __declspec(dllexport)
#else
#define GLYPH_DECLSPEC
#endif


/*
 * Glyph Keeper supports three rendering target systems:
 *
 * 1 - Pure text mode. No any graphics library is involved.
 *     All you can do is render character glyphs one by one and write them
 *     to the FILE* stream. Kind of ASCII art.
 *
 *     This mode is useful to isolate problems related to Glyph Keeper only.
 *     It is also convenient to check if Glyph Keeper works with your
 *     FreeType installation, without introducing extra dependencies.
 *
 * 2 - Rendering target is Allegro's BITMAP.
 *     You have to supply Allegro headers and link with Allegro lib.
 *     
 * 3 - Rendering target is SDL's SDL_Surface.
 *     Requires SDL and SDL_gfx libraries.
 *
 * You have to define GLYPH_TARGET to be one of these targets.
 * To do this, add line like this (for example):
 *      #define GLYPH_TARGET = GLYPH_TARGET_ALLEGRO
 * to your program _BEFORE_ including "glyph.h"
 *
 * Also, you should define this macro to the same thing when compiling
 * Glyph Keeper itself. It's better to do in compiler's command line,
 * like this: " -DGLYPH_TARGET=GLYPH_TARGET_ALLEGRO "
 * It is also OK to do it in command line for all your project files,
 * so that you don't need to #define it in the source.
 *
 * You can't mix two targets in one program.
 *
 * If you don't define GLYPH_TARGET to anything,
 * "glyph.h" will define it to GLYPH_TARGET_TEXT, the default target.
 */
#define GLYPH_TARGET_TEXT     1
#define GLYPH_TARGET_ALLEGRO  2
#define GLYPH_TARGET_SDL      3
#define GLYPH_TARGET_OPENGL   4
#define GLYPH_TARGET_ALLEGGL  5



/*
 * Checking for defined target.
 * Aliasing the target bitmap structure with GLYPH_TARGET_SURFACE
 * and including target-specific headers.
 * GLYPH_TARGET_TEXT is the default target.
 */
#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGGL) || (GLYPH_TARGET == GLYPH_TARGET_OPENGL)

#define GLYPH_TARGET_STR "OpenGL"
#define GK_DRIVER_CODE "glyph_to_opengl.c"
#define GLYPH_TARGET_SURFACE GLYPH_TEXTURE
typedef struct GLYPH_TEXTURE GLYPH_TEXTURE;



#elif (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)

#include <allegro.h>
#include <allegro/internal/aintern.h>
#define GLYPH_TARGET_STR        "Allegro"
#define GLYPH_TARGET_SURFACE    BITMAP
#define GLYPH_TARGET_KNOWS_RLEAA
#define GLYPH_TARGET_KNOWS_MONO_BITPACK
#define GLYPH_TARGET_KNOWS_MONO_RLE7
#define GLYPH_TARGET_HAS_RECTFILL
#define GLYPH_TARGET_HAS_RECTFILL_ANGLED
#define GK_DRIVER_CODE  "glyph_to_allegro.c"



#elif (GLYPH_TARGET == GLYPH_TARGET_SDL)

#include <SDL/SDL.h>
#define GLYPH_TARGET_STR        "SDL"
#define GLYPH_TARGET_SURFACE    SDL_Surface
#define GLYPH_TARGET_HAS_RECTFILL
#define GLYPH_TARGET_HAS_RECTFILL_ANGLED
#define GK_DRIVER_CODE  "glyph_to_sdl.c"



#elif (GLYPH_TARGET == GLYPH_TARGET_TEXT)

#include <stdio.h>
#define GLYPH_TARGET_STR        "Text"
#define GLYPH_TARGET_SURFACE    FILE
#define GLYPH_TARGET_KNOWS_RLEAA
#define GLYPH_TARGET_KNOWS_MONO_BITPACK
#define GLYPH_TARGET_KNOWS_MONO_RLE7
#define GK_DRIVER_CODE  "glyph_to_text.c"

#endif  /* GLYPH_TARGET */



/*
 * Glyph Keeper objects, that your program will create and use.
 * Your program should not make any assumption about their content layout.
 * You will normally only work with pointers to them.
 * All handling of this data should be done by Glyph Keeper API calls.
 */
typedef struct GLYPH_FACE GLYPH_FACE;
typedef struct GLYPH_KEEP GLYPH_KEEP;
typedef struct GLYPH_REND GLYPH_REND;



/* Maximum possible Unicode code point. */
#define GK_MAX_UNICODE   0x10FFFFu



/*
 * Constructing color values.
 */
#ifdef GLYPH_SAFE
#define gk_makecol(r,g,b)  ( ((((int)(r))&0xFF)<<16) | ((((int)(g))&0xFF)<<8) | (((int)(b))&0xFF) )
#define gk_makeacol(a,r,g,b)  ( ((((int)(a))&0xFF)<<24) | ((((int)(r))&0xFF)<<16) | (((((int)(g))&0xFF)<<8) | (((int)(b))&0xFF) )
#else
#define gk_makecol(r,g,b)  ( ((r)<<16) | ((g)<<8) | (b) )
#define gk_makeacol(a,r,g,b)  ( ((a)<<24) | ((r)<<16) | ((g)<<8) | (b) )
#endif



/*
 * Packing version into one integer value.
 */
#define GK_MAKE_VERSION(a,b,c)  (((a)<<20)|((b)<<10)|(c))



#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function sets user-provided messenger function.
 * User-provided messenger function will be called by Glyph Keeper library
 * routines every time when some routine wants to say out some message,
 * usually in case of an error.
 * If no messenger function is provided, no text feedback or error messages
 * will come out of Glyph Keeper.
 *
 * Set to 0 to 'unset' the messenger function.
 */
GLYPH_DECLSPEC void gk_set_messenger ( void (*func)(const char* const) );

/*
 * Sets font search path - a list of directories where
 * a font files will be searched.
 */
GLYPH_DECLSPEC void gk_set_font_path ( const char* const path );

/*
 * Glyph Keeper initialization.
 * There is no need to call this function in your program -
 * it will be called automatically when loading first font.
 * This function initializes FreeType. So use it, when you want to separate
 * FreeType initialization from loading first font for some reason.
 */
GLYPH_DECLSPEC void gk_library_init();

/*
 * Glyph Keeper shutdown function.
 * Basically it does nothing more than complete cleanup.
 * All data allocated by Glyph Keeper will be disposed.
 * I mean all objects of type GLYPH, GLYPH_FACE, GLYPH_INDEX, GLYPH_KEEP
 * and GLYPH_REND will be gone, all pointers to them will become invalid.
 * FreeType library will be also dismissed.
 * 
 * It is OK to call this function and later load some new font and start
 * using Glyph Keeper again in the same program session.
 * Just be careful not to use any old pointers after this call.
 *
 * Actually you are not required to call this function in your program.
 * Glyph Keeper initialization will register this function with the atexit(),
 * and it will be called in the end of your program anyway.
 *
 * There is no problem to call this function twice or any number of times.
 * So, you can still call it at the end, if you like. Or if you want to
 * control the order of things happening.
 */
GLYPH_DECLSPEC void gk_library_cleanup(void);


/*
 * Returns number of bytes allocated by Glyph Keeper.
 * FreeType memory is not counted.
 *
 * To see the real 'raw' memory usage of Glyph Keeper and FreeType, compile
 * both Glyph Keeper and FreeType with -DGLYPH_DEBUG_GK_MEMORY and
 * -DGLYPH_DEBUG_FT_MEMORY and use the functions below.
 */
GLYPH_DECLSPEC size_t gk_bytes_allocated();


/*
 * Functions for debugging Glyph Keeper and FreeType memory usage.
 * Don't use them in your project normally.
 * Note that they will give meaningful output only if you compile Glyph Keeper
 * and FreeType with -DGLYPH_DEBUG_GK_MEMORY and -DGLYPH_DEBUG_FT_MEMORY.
 *
 * ..._allocated_bytes() - returns amount of memory allocated and used. This
 * will be the correct memory usage numbers when you switch memory debugging
 * off. When memory debugging is on, the actual allocated memory is larger
 * because there is debugging overhead.
 *
 * ..._overhead_bytes() - returns debugging overhead. This is extra memory
 * allocated to enable memory debugging. Overhead will be 0 when you switch
 * off the debugging.
 */
GLYPH_DECLSPEC size_t gk_total_gk_allocated_bytes();
GLYPH_DECLSPEC size_t gk_total_gk_overhead_bytes();
GLYPH_DECLSPEC size_t gk_total_ft_allocated_bytes();
GLYPH_DECLSPEC size_t gk_total_ft_overhead_bytes();



/*************************************************************************
 *
 *   GLYPH_FACE routines
 */

/*
 * Loads font face from a file 'fname'.
 * One file may contain several font faces, use 'face_index' to specify wich
 * one to load. Use 'face_index' = 0 to load first font face, or if there is
 * only one font face in the file.
 * This function returns 0 on failure.
 */
GLYPH_DECLSPEC GLYPH_FACE* _gk_load_face_from_file
    ( const char* const fname, const int face_index,
      const int gk_header_version, const int gk_header_target );

#define gk_load_face_from_file(a,b)  \
    _gk_load_face_from_file(a,b,GK_MAKE_VERSION(GK_VERSION_MAJOR,GK_VERSION_MINOR,GK_VERSION_PATCH),GLYPH_TARGET)


/*
 * Loads font face from a memory buffer
 * Returns 0 on failure.
 */
GLYPH_DECLSPEC GLYPH_FACE* _gk_load_face_from_memory
    ( const unsigned char* const data, const int size, const int face_index,
      const int gk_header_version, const int gk_header_target );

#define gk_load_face_from_memory(a,b,c)  \
    _gk_load_face_from_memory(a,b,c,GK_MAKE_VERSION(GK_VERSION_MAJOR,GK_VERSION_MINOR,GK_VERSION_PATCH),GLYPH_TARGET)



/*
 * You can manually dispose GLYPH_FACE object with this function.
 * All renderers using is will automatically become unuseful until you
 * set them to use another font face.
 * All currently loaded font faces can be also disposed with
 * 'gk_library_cleanup()'.
 */
GLYPH_DECLSPEC void gk_unload_face ( GLYPH_FACE* const f );

/*
 * These functions extract various information from font face.
 * Don't free() the char* after using.
 */
GLYPH_DECLSPEC const char* gk_face_family          ( const GLYPH_FACE* const f );
GLYPH_DECLSPEC const char* gk_face_style           ( const GLYPH_FACE* const f );
GLYPH_DECLSPEC const char* gk_face_postscript_name ( const GLYPH_FACE* const f );
GLYPH_DECLSPEC const char* gk_face_driver_name     ( const GLYPH_FACE* const f );

/*
 * Returns number of charmaps a given face has.
 */
GLYPH_DECLSPEC int gk_face_get_number_of_charmaps ( const GLYPH_FACE* const f );

/*
 * Returns non-zero of the face is scalable.
 */
GLYPH_DECLSPEC int gk_face_is_scalable ( const GLYPH_FACE* const f );

/*
 * These functions check the directions supported by the font face.
 * 'is_horisontal' and 'is_vertical' can be both true at the same time.
 */
GLYPH_DECLSPEC int gk_face_is_horizontal ( const GLYPH_FACE* const f );
GLYPH_DECLSPEC int gk_face_is_vertical   ( const GLYPH_FACE* const f );

/*
 * Vertical distance from the baseline to the topmost point of any glyph in the face.
 * positive, expressed in font units.
 */
GLYPH_DECLSPEC int gk_face_ascender ( const GLYPH_FACE* const f );

/*
 * Vertical distance from the baseline to the bottommost point of any glyph in the face.
 * negative, expressed in font units.
 */
GLYPH_DECLSPEC int gk_face_descender ( const GLYPH_FACE* const f );

/*
 * Vertical distance from one baseline to the next one when writing several
 * lines of text. Always positive, expressed in font units.
 */
GLYPH_DECLSPEC int gk_face_line_spacing ( const GLYPH_FACE* const f );

/*
 * Total number of glyphs in a font face.
 * This is not necessarily the same with the number of characters,
 *   available to render with that font face.
 */
GLYPH_DECLSPEC int gk_face_number_of_glyphs ( const GLYPH_FACE* const f );

/*
 * Returns total number of characters, that can be rendered
 * with this font face.
 */
GLYPH_DECLSPEC int gk_face_number_of_characters ( GLYPH_FACE* const f );

/*
 * Returns total number of characters, that are actually defined
 * in this font face, without remapping.
 */
GLYPH_DECLSPEC int gk_face_number_of_own_characters ( GLYPH_FACE* const f );

GLYPH_DECLSPEC int gk_face_number_of_own_characters_in_range(GLYPH_FACE* const f, const unsigned start, const unsigned end);

/*
 * This function checks if the face has character 'code'.
 * Remapped characters are included.
 */
GLYPH_DECLSPEC int gk_face_has_character
    ( GLYPH_FACE* const face, const unsigned code );

/*
 * This function checks if the face has character 'code'
 * among its own characters, without remapping.
 */
GLYPH_DECLSPEC int gk_face_has_own_character
    ( GLYPH_FACE* const face, const unsigned code );

/*
 * Remapping
 */
GLYPH_DECLSPEC void gk_remap_character
    ( GLYPH_FACE* const face1, const unsigned code1,
      GLYPH_FACE* const face2, const unsigned code2 );

GLYPH_DECLSPEC void gk_unmap_character
    ( GLYPH_FACE* const face, const unsigned code );

GLYPH_DECLSPEC void gk_remap_range
    ( GLYPH_FACE* const face1, const unsigned code1,
      GLYPH_FACE* const face2, const unsigned code2, const unsigned range_size );

GLYPH_DECLSPEC void gk_unmap_range
    ( GLYPH_FACE* const face, const unsigned code, const unsigned range_size );




/*************************************************************************
 *
 *   GLYPH_KEEP routines
 */

/*
 * Creates new GLYPH_KEEP object, limited to keep not more than 'max_glyphs'
 * glyphs and to use not more than 'max_memory' bytes in memory.
 * You can set 'max_glyphs' or/and 'max_memory' to 0 to have no limit.
 * This function returns 0 on failure.
 */
GLYPH_DECLSPEC GLYPH_KEEP* gk_create_keeper
    ( const unsigned max_glyphs, const unsigned max_memory );

/*
 * This function is useful to destroy GLYPH_KEEP object.
 * Normally there is no particular need to call it - all GLYPH_KEEP
 * objects will be automatically destroyed by 'gk_library_cleanup()'
 * Note that you can safely destroy GLYPH_KEEP object, even if some
 * renderer (GLYPH_REND object) is using it. Renderer will just switch
 * to work without cache.
 * But don't use in any way pointer to the destroyed keeper. The keeper is
 * gone, the pointer leads nowhere.
 */
GLYPH_DECLSPEC void gk_done_keeper ( GLYPH_KEEP* const keeper );

/*
 * Returns number of glyphs currently cached by the keeper.
 */
GLYPH_DECLSPEC int gk_keeper_glyph_count ( const GLYPH_KEEP* const keeper );

/*
 * Returns maximum number of glyphs a keeper can keep.
 */
GLYPH_DECLSPEC int gk_keeper_glyph_limit ( const GLYPH_KEEP* const keeper );

/*
 * Returns number of bytes used by a keeper.
 * This includes keeper, its indexes and all cached glyphs.
 */
GLYPH_DECLSPEC int gk_keeper_byte_count ( const GLYPH_KEEP* const keeper );

/*
 * Returns maximum number of bytes this keeper may use in memory.
 */
GLYPH_DECLSPEC int gk_keeper_byte_limit(const GLYPH_KEEP* const keeper);

/*
 * Prints some information about keeper's current state.
 * Printing goes through the messenger function.
 */
GLYPH_DECLSPEC void gk_keeper_debug(const GLYPH_KEEP* const keeper);





/*************************************************************************
 *
 *   GLYPH_REND routines
 */

/*
 * Creates new renderer - GLYPH_REND object.
 *
 * Renderer is set to use font face 'face' and glyph cache 'keeper'.
 * If 'keeper' is 0, renderer will still work fine, just without glyph
 * caching. Caching can be turned on later by introducing a keeper with
 * gk_rend_set_keeper().
 * If 'face' is 0, renderer will be still created, but can't be used until
 * some font face is specified with gk_rend_set_face().
 *
 * Antialiasing is on by default.
 * '?' (U+003F) is the default undefined character.
 */
GLYPH_DECLSPEC GLYPH_REND* gk_create_renderer
    ( GLYPH_FACE* const face, GLYPH_KEEP* const keeper );

/*
 * Creates a copy of renderer 'rend'.
 */
GLYPH_DECLSPEC GLYPH_REND* gk_create_copy_of_renderer
    ( GLYPH_REND* const rend );

/*
 * Deletes GLYPH_REND object from memory. Don't use in any way a pointer
 * to destroyed renderer, it will crash your program.
 */
GLYPH_DECLSPEC void gk_done_renderer(GLYPH_REND* const rend);



/*
 * Assigns a particular GLYPH_KEEP object to this renderer.
 * In fact one GLYPH_KEEP object can cache glyphs for several renderers
 * at the same time.
 * If 'new_keeper' is 0, glyph caching will be off for this renderer.
 */
GLYPH_DECLSPEC void gk_rend_set_keeper
    ( GLYPH_REND* const rend, GLYPH_KEEP* const new_keeper );

/*
 * Assigns a new font face for this renderer. All rendered glyphs
 * will be of that face.
 * If 'new_face' is 0, the renderer will become unuseful - it can't render
 * anything.
 */
GLYPH_DECLSPEC void gk_rend_set_face
    ( GLYPH_REND* const rend, GLYPH_FACE* const new_face );



/*
 * Sets new text size - size of EM square, in 1/64th of a pixel.
 * Returns 1 on success, 0 on failure.
 */
GLYPH_DECLSPEC int gk_rend_set_size_subpixel ( GLYPH_REND* const rend,
    const unsigned new_hsize, const unsigned new_vsize );

/*
 * Sets new text size in integer pixels.
 * Returns 1 on success, 0 on failure.
 */
GLYPH_DECLSPEC int gk_rend_set_size_pixels ( GLYPH_REND* const rend,
    const unsigned new_pixel_hsize, const unsigned new_pixel_vsize );

/*
 * Set and get angle of text rotation.
 * Increasing the angle will rotate the text counter-clockwise.
 */
GLYPH_DECLSPEC void gk_rend_set_angle_in_radians
    ( GLYPH_REND* const rend, const double new_text_angle );
GLYPH_DECLSPEC void gk_rend_set_angle_in_degrees
    ( GLYPH_REND* const rend, const double new_text_angle );
GLYPH_DECLSPEC double gk_rend_get_angle_in_radians
    ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC double gk_rend_get_angle_in_degrees
    ( const GLYPH_REND* const rend );


/*
 * Set and get angle of text obliqueness. Allowed angles are
 * from -PI/4 to PI/4 (or -45 to 45 with _in_degrees version).
 * Negative angle means text is oblique to the left, 0 means text is straight
 * normal, positive angle means text is oblique to the right.
 * Use PI/15 (12 degrees) to get common italic text.
 */
GLYPH_DECLSPEC void gk_rend_set_italic_angle_in_radians
    ( GLYPH_REND* const rend, const double new_italic_angle );
GLYPH_DECLSPEC void gk_rend_set_italic_angle_in_degrees
    ( GLYPH_REND* const rend, const double new_italic_angle );
GLYPH_DECLSPEC double gk_rend_get_italic_angle_in_radians
    ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC double gk_rend_get_italic_angle_in_degrees
    ( const GLYPH_REND* const rend );


/*
 * Set and get strength of emboldening.
 * 0 means no emboldening, positive value will produce bold text.
 * Negative values will shrink the letters making them thinner.
 * 100 is moderately bold text.
 */
GLYPH_DECLSPEC void gk_rend_set_bold_strength
    ( GLYPH_REND* const rend, const int new_bold_strength );
GLYPH_DECLSPEC int gk_rend_get_bold_strength
    ( const GLYPH_REND* const rend );


/*
 * Sets renderer and offset to render the same character before and after that
 * character is rendered with the renderer 'rend'.
 */
GLYPH_DECLSPEC void gk_rend_set_before_renderer
    ( GLYPH_REND* const rend, GLYPH_REND* const new_before_rend,
    const int new_before_dx, const int new_before_dy );

GLYPH_DECLSPEC void gk_rend_set_after_renderer
    ( GLYPH_REND* const rend, GLYPH_REND* const new_after_rend,
    const int new_after_dx, const int new_after_dy );



/*
 * Return renderer's ascender and descender in 1/64th of a pixel.
 * ..._pixels - version returning same thing in integer pixels.
 * Note, that ascender is positive, while descender is usually negative.
 */
GLYPH_DECLSPEC int gk_rend_ascender         ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC int gk_rend_ascender_pixels  ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC int gk_rend_descender        ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC int gk_rend_descender_pixels ( const GLYPH_REND* const rend );

/*
 * Returns vertical distance between lines, in 1/64th of a pixel.
 * Spacing returned by this function is not affected by changing
 * renderer's angle setting.
 */
GLYPH_DECLSPEC int gk_rend_spacing        ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC int gk_rend_spacing_pixels ( const GLYPH_REND* const rend );

/*
 * Returns 'text height' = ascender - descender
 */
GLYPH_DECLSPEC int gk_rend_height        ( const GLYPH_REND* const rend );
GLYPH_DECLSPEC int gk_rend_height_pixels ( const GLYPH_REND* const rend );

/*
 * These functions control whether this renderer will perform hinting.
 */
GLYPH_DECLSPEC void gk_rend_set_hinting_off            ( GLYPH_REND* const rend );
GLYPH_DECLSPEC void gk_rend_set_hinting_default        ( GLYPH_REND* const rend );
GLYPH_DECLSPEC void gk_rend_set_hinting_force_autohint ( GLYPH_REND* const rend );
GLYPH_DECLSPEC void gk_rend_set_hinting_no_autohint    ( GLYPH_REND* const rend );

/*
 * Control rendering mode.
 * 'light' mode does not seem to work, need to investigate (FIXME).
 */
GLYPH_DECLSPEC void gk_rend_set_antialiasing_on  ( GLYPH_REND* const rend );
GLYPH_DECLSPEC void gk_rend_set_antialiasing_off ( GLYPH_REND* const rend );
/*GLYPH_DECLSPEC void gk_rend_set_antialiasing_light  ( GLYPH_REND* const rend );*/

/*
 * You can specify which character will show up instead of the character,
 * not present in font. Default is '?'.
 * Setting it to 0 will prevent anything shown instead of undefined characters.
 */
GLYPH_DECLSPEC void gk_rend_set_undefined_char
    ( GLYPH_REND* const rend, const unsigned new_undefined_char );

/*
 * You can specify which character will show up when FreeType
 * can't render a character.
 * Default is 0 - nothing is shown.
 */
GLYPH_DECLSPEC void gk_rend_set_error_char
    ( GLYPH_REND* const rend, const unsigned new_error_char );

/*
 * Returns 1, if renderer can render a character 'code',
 * otherwise returns 0.
 * Note: a character's glyph may be empty, like for space character,
 * in such case this function still returns 1.
 */
GLYPH_DECLSPEC int gk_rend_has_character
    ( const GLYPH_REND* const rend, const unsigned code );

/*
 * Returns 1, if renderer can render a non-empty glyph for character 'code',
 * otherwise returns 0.
 */
GLYPH_DECLSPEC int gk_rend_has_glyph
    ( GLYPH_REND* const rend, const unsigned code );

/*
 * Setting and querying text alpha transparency and color.
 * 'alpha_color' is (A<<24)+(R<<16)+(G<<8)+B,
 * A, R, G and B are in 0..255 range.
 */
GLYPH_DECLSPEC void gk_rend_set_text_alpha_color
    ( GLYPH_REND* const rend, const unsigned alpha_color );

GLYPH_DECLSPEC unsigned gk_rend_get_text_alpha_color
    ( const GLYPH_REND* const rend );

/*
 * Setting color and alpha separately.
 */
GLYPH_DECLSPEC void gk_rend_set_text_color_rgb ( GLYPH_REND* const rend,
    const unsigned char r, const unsigned char g, const unsigned char b );

GLYPH_DECLSPEC void gk_rend_set_text_color_combined ( GLYPH_REND* const rend,
    const int color );

GLYPH_DECLSPEC int gk_rend_get_text_color ( const GLYPH_REND* const rend );

GLYPH_DECLSPEC void gk_rend_set_text_alpha
    ( GLYPH_REND* const rend, const unsigned char alpha );

GLYPH_DECLSPEC unsigned gk_rend_get_text_alpha
    ( const GLYPH_REND* const rend );

/*
 * Setting and checking background color.
 */
GLYPH_DECLSPEC void gk_rend_set_back_color
    ( GLYPH_REND* const rend, const int new_back_color );
GLYPH_DECLSPEC int gk_rend_get_back_color ( GLYPH_REND* const rend );



GLYPH_DECLSPEC void gk_rend_debug ( const GLYPH_REND* const rend );



/*
 * Returning text dimensions and origin point coordinates.
 */
GLYPH_DECLSPEC void gk_text_dimensions_utf8 ( GLYPH_REND* const rend,
    const char* const text, int* const size_x, int* const size_y,
    int* const origin_x, int* const origin_y );
GLYPH_DECLSPEC void gk_text_dimensions_utf16 ( GLYPH_REND* const rend,
    const unsigned short* const text, int* const size_x, int* const size_y,
    int* const origin_x, int* const origin_y );
GLYPH_DECLSPEC void gk_text_dimensions_utf32 ( GLYPH_REND* const rend,
    const unsigned int* const text, int* const size_x, int* const size_y,
    int* const origin_x, int* const origin_y );


/*
 * Measuring a single glyph.
 */
GLYPH_DECLSPEC void gk_glyph_size ( GLYPH_REND* const rend, const unsigned unicode,
    int *const width, int* const height );
GLYPH_DECLSPEC int gk_glyph_width ( GLYPH_REND* const rend, const unsigned unicode );
GLYPH_DECLSPEC int gk_glyph_height ( GLYPH_REND* const rend, const unsigned unicode );


/*
 * Measuring a single character.
 */
GLYPH_DECLSPEC void gk_char_size ( GLYPH_REND* const rend, const unsigned unicode,
    int *const width, int* const height );
GLYPH_DECLSPEC int gk_char_width ( GLYPH_REND* const rend, const unsigned unicode );
GLYPH_DECLSPEC int gk_char_height ( GLYPH_REND* const rend, const unsigned unicode );
GLYPH_DECLSPEC void gk_char_advance ( GLYPH_REND* const rend, const unsigned unicode,
    int* const adv_x, int* const adv_y );
GLYPH_DECLSPEC void gk_char_advance_subpixel ( GLYPH_REND* const rend, const unsigned unicode,
    int* const adv_x, int* const adv_y );


/*
 * These functions calculate width and height of text, in pixels.
 * Width and height are written to '*width' and '*height'.
 */
GLYPH_DECLSPEC void gk_text_size_utf8 ( GLYPH_REND* const rend,
    const char* const text, int* const width, int* const height );
GLYPH_DECLSPEC void gk_text_size_utf16 ( GLYPH_REND* const rend,
    const unsigned short* const text, int* const width, int* const height );
GLYPH_DECLSPEC void gk_text_size_utf32 ( GLYPH_REND* const rend,
    const unsigned* const text, int* const width, int* const height );

/*
 * Returns width and height of text, produced by given renderer.
 * In integer pixels.
 */
GLYPH_DECLSPEC int gk_text_width_utf8 ( GLYPH_REND* const rend, const char* const text );
GLYPH_DECLSPEC int gk_text_width_utf16 ( GLYPH_REND* const rend, const unsigned short* const text );
GLYPH_DECLSPEC int gk_text_width_utf32 ( GLYPH_REND* const rend, const unsigned* const text );
GLYPH_DECLSPEC int gk_text_height_utf8 ( GLYPH_REND* const rend, const char* const text );
GLYPH_DECLSPEC int gk_text_height_utf16 ( GLYPH_REND* const rend, const unsigned short* const text );
GLYPH_DECLSPEC int gk_text_height_utf32 ( GLYPH_REND* const rend, const unsigned* const text );

/*
 * Return text advance, in integer pixels.
 */
GLYPH_DECLSPEC void gk_text_advance_utf8 ( GLYPH_REND* const rend,
    const char* const text, int* const adv_x, int* const adv_y );
GLYPH_DECLSPEC void gk_text_advance_utf16 ( GLYPH_REND* const rend,
    const unsigned short* const text, int* const adv_x, int* const adv_y);
GLYPH_DECLSPEC void gk_text_advance_utf32 ( GLYPH_REND* const rend,
    const unsigned* const text, int* const adv_x, int* const adv_y );

/*
 * Return text advance, in 1/64th of a pixel.
 */
GLYPH_DECLSPEC void gk_text_advance_subpixel_utf8 ( GLYPH_REND* const rend,
    const char* const text, int* const adv_x, int* const adv_y );
GLYPH_DECLSPEC void gk_text_advance_subpixel_utf16 ( GLYPH_REND* const rend,
    const unsigned short* const text, int* const adv_x, int* const adv_y);
GLYPH_DECLSPEC void gk_text_advance_subpixel_utf32 ( GLYPH_REND* const rend,
    const unsigned* const text, int* const adv_x, int* const adv_y );






/*
 * This function renders the character's glyph and adds it into the cache.
 * The rendered character is not printed anywhere.
 * If the glyph is already in the cache, it is not rendered again.
 * This function can be useful to 'prepare' the glyphs of some characters,
 * that you expect to be used soon.
 */
GLYPH_DECLSPEC void gk_precache_char
    ( GLYPH_REND* const rend, const unsigned unicode );

GLYPH_DECLSPEC void gk_precache_range
    ( GLYPH_REND* const rend,
      const unsigned range_start, const unsigned range_end );

GLYPH_DECLSPEC void gk_precache_set_utf8
    ( GLYPH_REND* const rend, const char* const char_set );

GLYPH_DECLSPEC void gk_precache_set_utf16
    ( GLYPH_REND* const rend, const unsigned short* const char_set );

GLYPH_DECLSPEC void gk_precache_set_utf32
    ( GLYPH_REND* const rend, const unsigned* const char_set );





/*
 * Functions, rendering something to the target surface.
 */

#ifdef GLYPH_TARGET

/*
 * Outputs a character 'unicode' to the 'bmp', using renderer 'rend'.
 * Character glyph's left top corner will appear at (x,y).
 */
GLYPH_DECLSPEC void gk_put_char ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned unicode, const int x, const int y );

/*
 * Outputs a character so that it's center is at (x,y).
 */
GLYPH_DECLSPEC void gk_put_char_center ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned unicode, const int x, const int y );

/*
 * Renders one character, starting with positioning pen at (*pen_x,*pen_y).
 * *pen_x and *pen_y will be incremented to become starting pen position for
 * the next character.
 */
GLYPH_DECLSPEC void gk_render_char ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned unicode,
    int* const pen_x, int* const pen_y );

GLYPH_DECLSPEC void gk_render_char_center ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned unicode, const int x, const int y );


/*
 *
 */
GLYPH_DECLSPEC void gk_render_line_utf8 ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const char* const text,
    const int pen_x, const int pen_y );

GLYPH_DECLSPEC void gk_render_line_utf16 ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned short* const text,
    const int pen_x, const int pen_y );

GLYPH_DECLSPEC void gk_render_line_utf32 ( GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend, const unsigned* const text,
    const int pen_x, const int pen_y );


/*
 * Target-specific functions
 */
#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGGL) || (GLYPH_TARGET == GLYPH_TARGET_OPENGL)

GLYPH_DECLSPEC GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend,
    int rangeStart, int rangeLength );

GLYPH_DECLSPEC void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture );

GLYPH_DECLSPEC void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture,
    const char *text, int x, int y );

GLYPH_DECLSPEC void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture );

GLYPH_DECLSPEC void gk_destroy_texture( GLYPH_TEXTURE *texture );

#endif



#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGRO)

/*
 * Creates an Allegro FONT with Glyph Keeper's vtable.
 * The renderer 'rend' will be used for all text output with this font.
 * So don't dispose the renderer if you are still going to use the font.
 */
GLYPH_DECLSPEC FONT* gk_create_allegro_font ( GLYPH_REND* const rend );

/*
 * Creates a native Allegro FONT structure, with Allegro vtable.
 * In this case the renderer 'rend' is not used for output, so you can
 * safely dispose it.
 */
GLYPH_DECLSPEC FONT* gk_create_allegro_bitmap_font_for_range
    ( GLYPH_REND* const rend, const int range_start, const int range_end,
    const int color_depth );

#endif


#endif  /* GLYPH_TARGET */



#ifdef __cplusplus
}
#endif


#undef GLYPH_DECLSPEC


/*
 * Compatibility with old messy API.
 */
#ifndef GK_NO_LEGACY
#define GK_LEGACY
#endif

#ifdef GK_LEGACY

#define  GLYPH_KEEPER_VERSION_MAJOR       GK_VERSION_MAJOR
#define  GLYPH_KEEPER_VERSION_MINOR       GK_VERSION_MINOR
#define  GLYPH_KEEPER_VERSION_PATCH       GK_VERSION_PATCH
#define  GLYPH_KEEPER_VERSION_STR         GK_VERSION_STR
#define  GLYPH_KEEPER_DATE_STR            GK_DATE_STR

#define  glyph_keeper_set_messenger       gk_set_messenger
#define  set_glyph_messenger              gk_set_messenger
#define  glyph_keeper_library_init        gk_library_init
#define  glyph_keeper_library_done        gk_library_cleanup
#define  glyph_keeper_bytes_allocated     gk_bytes_allocated

#define  load_face                        gk_load_face_from_file
#define  load_face_from_file              gk_load_face_from_file
#define  load_face_from_memory            gk_load_face_from_memory
#define  unload_face                      gk_unload_face
#define  face_family                      gk_face_family
#define  face_style                       gk_face_style
#define  face_postscript_name             gk_face_postscript_name
#define  face_driver_name                 gk_face_driver_name
#define  face_is_horizontal               gk_face_is_horizontal
#define  face_is_vertical                 gk_face_is_vertical
#define  face_ascender                    gk_face_ascender
#define  face_descender                   gk_face_descender
#define  face_line_spacing                gk_face_line_spacing
#define  face_get_number_of_glyphs        gk_face_number_of_glyphs
#define  face_get_number_of_characters    gk_face_number_of_characters

#define  create_keeper                    gk_create_keeper
#define  glyph_keeper_create_keeper       gk_create_keeper
#define  done_keeper                      gk_done_keeper
#define  glyph_keeper_done_keeper         gk_done_keeper
#define  keeper_glyph_count               gk_keeper_glyph_count
#define  keeper_glyph_limit               gk_keeper_glyph_limit
#define  keeper_byte_count                gk_keeper_byte_count
#define  keeper_byte_limit                gk_keeper_byte_limit
#define  keeper_debug                     gk_keeper_debug

#define  create_renderer                  gk_create_renderer
#define  glyph_renderer_create            gk_create_renderer
#define  done_renderer                    gk_done_renderer
#define  glyph_renderer_done              gk_done_renderer
#define  rend_set_keeper                  gk_rend_set_keeper
#define  rend_set_face                    gk_rend_set_face
#define  rend_set_size                    gk_rend_set_size_subpixel
#define  gk_rend_set_size                 gk_rend_set_size_subpixel
#define  rend_set_size_pixels             gk_rend_set_size_pixels
#define  rend_set_angle                   gk_rend_set_angle
#define  rend_ascender                    gk_rend_ascender
#define  rend_ascender_pixels             gk_rend_ascender_pixels
#define  rend_descender                   gk_rend_descender
#define  rend_descender_pixels            gk_rend_descender_pixels
#define  rend_spacing                     gk_rend_spacing
#define  rend_spacing_pixels              gk_rend_spacing_pixels
#define  rend_height                      gk_rend_height
#define  rend_height_pixels               gk_rend_height_pixels
#define  rend_set_hinting_off             gk_rend_set_hinting_off
#define  rend_set_hinting_default         gk_rend_set_hinting_default
#define  rend_set_hinting_force_autohint  gk_rend_set_hinting_force_autohint
#define  rend_set_hinting_no_autohint     gk_rend_set_hinting_no_autohint
#define  rend_set_render_mode_normal      gk_rend_set_antialiasing_on
#define  rend_set_render_mode_mono        gk_rend_set_antialiasing_off
#define  rend_set_italic                  gk_rend_set_italic
#define  rend_set_undefined               gk_rend_set_undefined_char
#define  rend_set_undefined_char          gk_rend_set_undefined_char
#define  rend_set_error_char              gk_rend_set_error_char
#define  rend_has_character               gk_rend_has_character
#define  rend_has_glyph                   gk_rend_has_glyph
#define  rend_set_text_alpha_color        gk_rend_set_text_alpha_color
#define  rend_get_text_alpha_color        gk_rend_get_text_alpha_color
#define  rend_set_text_color              gk_rend_set_text_color_rgb
#define  gk_rend_set_text_color           gk_rend_set_text_color_rgb
#define  rend_set_text_alpha              gk_rend_set_text_alpha
#define  rend_precache_char               gk_rend_precache_char
#define  create_allegro_font              gk_create_allegro_font

#define  text_size_utf8                   gk_text_size_utf8
#define  text_size_utf32                  gk_text_size_utf32
#define  text_width_utf8                  gk_text_width_utf8
#define  text_width_utf32                 gk_text_width_utf32
#define  text_height_utf8                 gk_text_height_utf8
#define  text_height_utf32                gk_text_height_utf32

#define  gk_rend_precache_char            gk_precache_char
#define  put_char                         gk_put_char
#define  put_char_center                  gk_put_char_center
#define  render_char                      gk_render_char
#define  render_line_utf8                 gk_render_line_utf8
#define  render_line_utf32                gk_render_line_utf32

#define  gk_rend_set_angle                gk_rend_set_angle_in_radians
#define  gk_rend_set_italic               gk_rend_set_italic_angle_in_degrees
#define  gk_rend_set_bold                 gk_rend_set_bold_strength

#endif  /* GK_LEGACY */


#endif
