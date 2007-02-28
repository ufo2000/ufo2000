/*
 * glyph_memory.h  -  Glyph Keeper memory manager for debugging.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



size_t gk_total_gk_allocated_bytes()
{
    return _gk_allocated_by_gk;
}


size_t gk_total_gk_overhead_bytes()
{
    return _gk_overhead_by_gk;
}


size_t gk_total_ft_allocated_bytes()
{
    return _gk_allocated_by_ft;
}


size_t gk_total_ft_overhead_bytes()
{
    return _gk_overhead_by_ft;
}



#ifdef GLYPH_DEBUG_GK_MEMORY

void* _gk_malloc(size_t size)
{
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_malloc(%d) (now %d + %d allocated) : ",
                            size, _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif

    buf = malloc( size + sizeof(size_t) );
    if (buf)
    {
        *(size_t*)buf = size;
        _gk_allocated_by_gk += size;
        _gk_overhead_by_gk += sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_malloc() failed to allocate %d bytes!\n", size );
        return 0;
    }
}


void* _gk_calloc(size_t num_elements, size_t size)
{
    size_t bytes = num_elements * size;
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_calloc(%d,%d) (now %d + %d allocated) : ",
                            num_elements, size, _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif

    buf = malloc( bytes + sizeof(size_t) );
    if (buf)
    {
        memset( buf, 0, bytes + sizeof(size_t) );
        *(size_t*)buf = bytes;

        _gk_allocated_by_gk += bytes;
        _gk_overhead_by_gk += sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d bytes allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_calloc() failed to allocate %d bytes!\n", bytes );
        return 0;
    }
}


void* _gk_realloc(void *ptr, size_t size)
{
    size_t old_size;
    size_t* real_ptr;
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_realloc(%p,%d) (now %d + %d allocated) : ",
                            ptr, size, _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif

    if (ptr) real_ptr = (size_t*)ptr - 1;
    else real_ptr = 0;

    if (real_ptr) old_size = *real_ptr;
    else old_size = 0;

    buf = realloc( real_ptr, size + sizeof(size_t) );
    if (buf)
    {
        *(size_t*)buf = size;
        _gk_allocated_by_gk += size - old_size;
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d bytes allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_realloc() failed to allocate %d bytes!\n", size );
        return 0;
    }
}


void _gk_free(void *ptr)
{
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_free(%p) (now %d + %d allocated) : ",
                            ptr, _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif
    if (ptr)
    {
        size_t size = *((size_t*)ptr - 1);

        free( (size_t*)ptr - 1 );
        _gk_allocated_by_gk -= size;
        _gk_overhead_by_gk -= sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Freed %d bytes (now %d + %d allocated)\n",
                                size, _gk_allocated_by_gk, _gk_overhead_by_gk );
#endif
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_GK_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Trying to free a 0 pointer!\n" );
#endif
        _gk_msg( "Error: trying to _gk_free() a 0 pointer\n" );
    }
}

#endif  /* GLYPH_DEBUG_GK_MEMORY */




#ifdef GLYPH_DEBUG_FT_MEMORY

void* _gk_for_ft_malloc(size_t size)
{
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_for_ft_malloc(%d) (now %d + %d allocated) : ",
                            size, _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif

    buf = malloc( size + sizeof(size_t) );
    if (buf)
    {
        *(size_t*)buf = size;
        _gk_allocated_by_ft += size;
        _gk_overhead_by_ft += sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_for_ft_malloc() failed to allocate %d bytes!\n", size );
        return 0;
    }
}


void* _gk_for_ft_calloc(size_t num_elements, size_t size)
{
    size_t bytes = num_elements * size;
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_for_ft_calloc(%d,%d) (now %d + %d allocated) : ",
                            num_elements, size, _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif

    buf = malloc( bytes + sizeof(size_t) );
    if (buf)
    {
        memset( buf, 0, bytes + sizeof(size_t) );
        *(size_t*)buf = bytes;

        _gk_allocated_by_ft += bytes;
        _gk_overhead_by_ft += sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d bytes allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_for_ft_calloc() failed to allocate %d bytes!\n", bytes );
        return 0;
    }
}


void* _gk_for_ft_realloc(void *ptr, size_t size)
{
    size_t old_size;
    size_t* real_ptr;
    void *buf;

#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_for_ft_realloc(%p,%d) (now %d + %d allocated) : ",
                            ptr, size, _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif

    if (ptr) real_ptr = (size_t*)ptr - 1;
    else real_ptr = 0;

    if (real_ptr) old_size = *real_ptr;
    else old_size = 0;

    buf = realloc( real_ptr, size + sizeof(size_t) );
    if (buf)
    {
        *(size_t*)buf = size;
        _gk_allocated_by_ft += size - old_size;
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Success (%p), now %d + %d bytes allocated\n",
                                (void*)((size_t*)buf + 1), _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif
        return (size_t*)buf + 1;
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Failure!\n" );
#endif
        _gk_msg( "Error: _gk_for_ft_realloc() failed to allocate %d bytes!\n", size );
        return 0;
    }
}


void _gk_for_ft_free(void *ptr)
{
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
    if (glyph_log)
        fprintf( glyph_log, "_gk_for_ft_free(%p) (now %d + %d allocated) : ",
                            ptr, _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif
    if (ptr)
    {
        size_t size = *((size_t*)ptr - 1);

        free( (size_t*)ptr - 1 );
        _gk_allocated_by_ft -= size;
        _gk_overhead_by_ft -= sizeof(size_t);
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log)
            fprintf( glyph_log, "Freed %d bytes (now %d + %d allocated)\n",
                                size, _gk_allocated_by_ft, _gk_overhead_by_ft );
#endif
    }
    else
    {
#if defined(GLYPH_LOG) && defined(GLYPH_LOG_FT_MEMORY)
        if (glyph_log) fprintf( glyph_log, "Trying to free a 0 pointer!\n" );
#endif
        _gk_msg( "Error: trying to _gk_for_ft_free() a 0 pointer\n" );
    }
}

#endif  /* GLYPH_DEBUG_FT_MEMORY */




#endif  /* included_from_glyph_c */
