/*
 * glyph_memory.h  -  Glyph Keeper header for memory manager.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */


#ifdef GLYPH_DEBUG_GK_MEMORY

void* _gk_malloc(size_t size);
void* _gk_calloc(size_t num_elements, size_t size);
void* _gk_realloc(void *ptr, size_t size);
void _gk_free(void *ptr);

#else

#define _gk_malloc malloc
#define _gk_calloc calloc
#define _gk_realloc realloc
#define _gk_free free

#endif



#ifdef GLYPH_DEBUG_FT_MEMORY

void* _gk_for_ft_malloc(size_t size);
void* _gk_for_ft_calloc(size_t num_elements, size_t size);
void* _gk_for_ft_realloc(void *ptr, size_t size);
void _gk_for_ft_free(void *ptr);

#else

#define _gk_for_ft_malloc malloc
#define _gk_for_ft_calloc calloc
#define _gk_for_ft_realloc realloc
#define _gk_for_ft_free free

#endif
