/*
 * glyph.c  -  Glyph Keeper source container, includes all other files.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#define included_from_glyph_c


/* Internal header. */
#include "glyph_internal.h"

/* Memory manager header. */
#include "glyph_memory.h"

/* Global variables. */
#include "glyph_global_vars.c"

/* Memory manager. */
#include "glyph_memory.c"

/* Simple routines. */
#include "glyph_utils.c"

/* GLYPH_FACE routines. */
#include "glyph_face.c"

/* GLYPH_INDEX and GLYPH_KEEP routines. */
#include "glyph_index.c"

/* GLYPH_REND routines. */
#include "glyph_rend.c"

/* Rendering a glyph. */
#include "glyph_workout.c"

/* Routines finding text size. */
#include "glyph_dimensions.c"

/* Main routines. */
#include "glyph_main.c"


/* Driver code. */
#include GK_DRIVER_CODE
