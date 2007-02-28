/*
 * glyph_structs.h  -  Glyph Keeper structures.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifndef glyph_structs_h_included
#define glyph_structs_h_included


/*
 * GLYPH structure, representing one Glyph Keeper's glyph.
 * (Here the term 'glyph' corresponds to the rendered image of one character.)
 * This structure is completely internal, it is never used in the API.
 *
 * Glyph's bitmap is filled by horisontal lines, starting from the most
 * top-left point.
 *
 * GLYPH object may be created by GLYPH_REND object only
 * GLYPH object may be destroyed by GLYPH_KEEPER object, or with direct call
 * to '_gk_unload_glyph()'
 */
typedef struct GLYPH
{
    /* Each Glyph Keeper's glyph corresponds to an Unicode code point. */
    unsigned unicode;

    /* Glyph dimensions, in pixels. */
    unsigned short width, height;

    /*
     * Glyph bitmap, (width x height) pixels in size.
     * Bitmap data may be compressed, compression type is specified in first
     * byte:
     *    0 - Uncompressed 256-color bitmap - one byte for one pixel.
     *    1 - RLE compressed 256-color bitmap.
     *    2 - Bit-packed monochrome bitmap.
     *    3 - RLE7-compressed monochrome bitmap.
     */
    unsigned char *bmp;

    /*
     * Size of the glyph bitmap data, in bytes.
     * Bitmap data may be either compressed or uncompressed, so its size
     * in memory is not always width*height.
     */
    unsigned bmpsize;

    /*
     * Glyph bearing, in pixels.
     * It is glyph bitmap position, relative to the pen position.
     */
    short left, top;

    /*
     * Position of glyph center relative to pen position.
     * Positive x goes right, positive y goes up.
     */
    short center_x, center_y;

    /* Pen position advance, in 1/64th of pixel. */
    int advance_x, advance_y;

    /*
     * If not 0, points to GLYPH_INDEX object, where the glyph is stored.
     * It means that glyph is in cache, and will be disposed automatically
     * when not needed anymore. Don't dispose the glyph in that case.
     *
     * If 0, glyph is not cached and you must release it with '_gk_unload_glyph()'
     * after using. Be very careful.
     */
    struct GLYPH_INDEX *index;

    /*
     * Double-linked list of glyphs.
     * It is maintained and used by GLYPH_KEEP object.
     */
    struct GLYPH *prev, *next;
}
GLYPH;



/*
 * Remapping
 */
typedef struct GLYPH_REMAP
{
    /* If 'face' is 0, this entry is not remapped. */
    GLYPH_FACE *face;
    /* If code is 0 it doesn't mean the entry is not remapped. */
    unsigned code;
}
GLYPH_REMAP;



/*
 * GLYPH_FACE structure represents a font face within Glyph Keeper.
 * Basically it just wraps the reference to FreeType's FT_FaceRec_.
 */
struct GLYPH_FACE
{
    /*
     * Unique number for identifying the face.
     * Every new font face loaded will get next number.
     *
     * After loading a font from file 4294967296 times your program is not
     * guaranteed to work properly anymore.
     */
    unsigned id;

    /* FreeType face handle. */
    FT_Face face;

    /* FT_Size object, that is used for remapped characters only. */
    FT_Size own_size;

    /* Remapping */
    GLYPH_REMAP ***remap;

    /* Number of characters defined in this font face. */
    /* -1 if unknown (yet) */
    int number_of_own_characters;

    /* How many characters are gained/lost due to remapping. */
    /* Can be negative. */
    /* It it is 0 it does not necessarily mean there were no remappings. */
    int remap_increment;

    /*
     * Amount of dynamically allocated memory used by this GLYPH_FACE object.
     * Only Glyph Keeper memory is counted, FreeType memory is not included.
     * This includes size of this GLYPH_FACE structure and remapping tables.
     */
    int allocated;

    /*
     * List of all GLYPH_FACE objects in memory.
     * Used for automatic cleanup at the end of your program.
     */
    GLYPH_FACE *prev,*next;

    /*
     * List of all GLYPH_REND objects using this font face.
     * Once you unload this face all that renderers will become useless.
     */
    GLYPH_REND *first_renderer, *last_renderer;
};



/*
 * GLYPH_INDEX structure keeps index tables for cached glyphs
 *
 * GLYPH_INDEX objects should only be created with create_glyph_index()
 * and destroyed with destroy_glyph_index().
 */
typedef struct GLYPH_INDEX
{
    /*
     * Index pages.
     * 68 -> 128 -> 128
     * I need to explain what it is.
     */
    GLYPH ****pages;

    /*
     * What kind of glyphs are indexed.
     * These values are derived from the renderer - GLYPH_REND object.
     * Includes: font face, loading and rendering settings,
     * font size and angle settings.
     * Notice that this does not include any color information.
     */
    unsigned face_id;
    unsigned load_flags;
    FT_Render_Mode render_mode;
    unsigned synth_flags;
    unsigned hsize,vsize;
    double text_angle;
    double italic_angle;
    int bold_strength;

    /* GLYPH_KEEP object, using this index */
    GLYPH_KEEP *keeper;

    /*
     * List of GLYPH_REND objects, using this index
     * Once this object is dismissed, they will all be set to 'no cache' mode.
     */
    GLYPH_REND *first_renderer,*last_renderer;

    /* list of indexes, corresponding to same GLYPH_KEEP object */
    struct GLYPH_INDEX *prev,*next;
}
GLYPH_INDEX;


/*
 * GLYPH_KEEP structure - a glyph cache entity.
 */
struct GLYPH_KEEP
{
    /*
     * Maximum number of glyphs this cache can store.
     * 0 means no limit.
     */
    int max_glyphs;

    /*
     * Maximum number of bytes this cache can use.
     * This includes the memory used by GLYPH_KEEP structure.
     * Set to 0 to disable memory limit.
     */
    int max_memory;

    /* Number of currently cached glyphs. */
    int num_glyphs;

    /*
     * Amount of memory in current use by this cache, in bytes.
     * This includes size of this GLYPH_KEEP structure, size of all
     * GLYPH_INDEX structures associated with this cache,
     * and also size of all glyphs cached currently.
     */
    int allocated;

    /*
     * Head and tail of the double-linked list of GLYPH_INDEX objects.
     */
    GLYPH_INDEX *first_index,*last_index;

    /*
     * Head and tail of the list of cached glyphs.
     */
    GLYPH *head,*tail;

    /*
     * Global list of GLYPH_KEEP objects.
     * This is used for cleanup.
     */
    struct GLYPH_KEEP *prev,*next;
};


/*
 * GLYPH_REND structure - a glyph renderer.
 *
 */
struct GLYPH_REND
{
    /*
     * Font face.
     */
    GLYPH_FACE *face;
    unsigned face_id;

    /*
     * Corresponding GLYPH_INDEX object, if there is a cache assigned
     * to this renderer.
     * 0 if this renderer works without cache.
     */
    GLYPH_INDEX *index;

    /*
     * FT_Size object - FreeType's size object.
     * It is necessary, since Glyph Keeper can share the same font face
     * among several renderers.
     */
    FT_Size size;

    /*
     * One of the following:
     *   FT_LOAD_DEFAULT = 0
     *   FT_LOAD_NO_HINTING      - no any hinting is applied
     *   FT_LOAD_FORCE_AUTOHINT
     *   FT_LOAD_NO_AUTOHINT
     */
    unsigned hinting_mode;

    /*
     * One of the following:
     *   FT_LOAD_TARGET_MONO    - hint outline for monochrome or lcd displays
     *   FT_LOAD_TARGET_NORMAL  - hint and render for normal anti-aliased displays
     */
    unsigned hinting_target;

    /*
     * Glyph outline loading settings - OR'd combination of flags:
     *   FT_LOAD_DEFAULT = 0
     *   FT_LOAD_VERTICAL_LAYOUT
     *   FT_LOAD_PEDANTIC        - more careful checking, reject broken fonts.
     *   FT_LOAD_NO_BITMAP       - always used, ignore bitmaps defined in the
     *                               font file
     * Also, it gets OR'd in 'hinting_mode' and 'hinting_target'.
     */
    unsigned load_flags;

    /*
     * FreeType rendering mode. Possible values:
     *   FT_RENDER_MODE_NORMAL (default) - 256 grey tones.
     *   FT_RENDER_MODE_LIGHT  - I don't know what it is.
     *   FT_RENDER_MODE_MONO   - Monochrome rendering.
     */
    FT_Render_Mode render_mode;

    /*
     * Font size settings.
     * 'hsize' and 'vsize' is a EM box size, in 1/64th of a pixel.
     */
    unsigned hsize,vsize;

    /*
     * Substitution character for any 'undefined' character - not defined in
     * font. '?' is default (U+003F).
     */
    unsigned undefined_char;

    /*
     * Sometimes the character glyph is defined in font face, but some error
     * happens when trying to render it. In such case 'error_char' will appear
     * instead of the failed character.
     */
    unsigned error_char;

    /* Spacing between two lines of text, in 1/64th of a pixel. */
    unsigned line_spacing;

    /* Spacing between two lines of text, in pixels. */
    unsigned line_spacing_pixels;

    /* Maximum height of text in 1/64th of a pixel. */
    unsigned text_height;

    /* Maximum height of text in pixels. */
    /* Calculated as (ascender + descender) */
    unsigned text_height_pixels;

    /* Text rotation angle, in radians */
    double text_angle;

    /* Sine and cosine of the angle to not re-compute them */
    double text_angle_cos;
    double text_angle_sin;

    /* Italicizing angle, in radians */
    double italic_angle;

    /* Strength of emboldening */
    int bold_strength;

    /* True if this renderer uses glyph transformation */
    int do_matrix_transform;

    /* Transformation matrix */
    FT_Matrix matrix;
    /*long matrix[4];*/

    /* Text alpha and color  -  (a<<24) | (r<<16) | (g<<8) | b */
    unsigned text_alpha_color;

    /* Background color. Normally -1 which means transparent. */
    /* Otherwise depth-independent value: (r<<16) | (g<<8) | b */
    int back_color;

    /*
     * Renderers to be automatically used to render the same character before, and after the
     * character is rendered with this renderer.
     */
    GLYPH_REND *before_rend, *after_rend;

    /*
     * Offset of the glyph rendered before and after. Offset is relative to the test origin
     * point, so positive x axis goes right, positive y axis goes up.
     */
    int before_dx, before_dy, after_dx, after_dy;

    /*
     * Points to the target-specific info used for better integration with the target.
     * Currently only Allegro target is using this.
     */
    void* target_info;

    /*
     * Global list of GLYPH_REND objects.
     */
    GLYPH_REND *prev,*next;

    /*
     * List of GLYPH_REND objects rendering characters in the same font face.
     */
    GLYPH_REND *prev_for_same_face,*next_for_same_face;

    /*
     * List of GLYPH_REND objects sharing the same GLYPH_INDEX.
     */
    GLYPH_REND *prev_for_same_index,*next_for_same_index;
};


#endif
