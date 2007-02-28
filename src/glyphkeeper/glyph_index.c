/*
 * glyph_index.c  -  Glyph Keeper routines implementing glyph cache.
 *
 * Copyright (c) 2003-2007 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c



static int _gk_keeper_make_space(GLYPH_KEEP* const keeper,const int bytes);


static int _gk_glyph_index_ok_for_renderer(const GLYPH_INDEX* const index,
    const GLYPH_REND* const rend)
{
    CARE(index);
    CARE(rend);

    return (index->face_id == rend->face_id &&
            index->load_flags == rend->load_flags &&
            index->render_mode == rend->render_mode &&
            index->hsize == rend->hsize &&
            index->vsize == rend->vsize &&
            index->text_angle == rend->text_angle &&
            index->italic_angle == rend->italic_angle &&
            index->bold_strength == rend->bold_strength);
}


static void _gk_glyph_index_init(GLYPH_INDEX* const index,
    const GLYPH_REND* const rend)
{
    CARE(index);
    CARE(rend);

    index->face_id = rend->face_id;
    index->load_flags = rend->load_flags;
    index->render_mode = rend->render_mode;
    index->hsize = rend->hsize;
    index->vsize = rend->vsize;
    index->text_angle = rend->text_angle;
    index->italic_angle = rend->italic_angle;
    index->bold_strength = rend->bold_strength;
}


/* Callers: gk_rend_debug. */
static int _gk_glyph_index_size(const GLYPH_INDEX* const index)
{
    int size,a,b;

    CARE(index);

    size = sizeof(GLYPH_INDEX);
    if (!index->pages) return size;

    size += 69*sizeof(void*);
    if ((long)index->pages[68])
    {
        for (a=0; a<68; a++)
        {
            if (!index->pages[a]) continue;
            size += 129*sizeof(void*);
            if ((long)index->pages[a][128])
            {
                for (b=0; b<128; b++) { if (index->pages[a][b]) size += 129*sizeof(void*); }  
            }
        }
    }
    return size;
}


/* the only way to create GLYPH_INDEX object */
/* callers: gk_rend_set_keeper */
static GLYPH_INDEX* _gk_glyph_index_create(GLYPH_KEEP* const keeper,
    GLYPH_REND* const rend)
{
    GLYPH_INDEX* index;
    funcname = "_gk_glyph_index_create()";

    CARE(keeper);
    CARE(rend);

    if (!_gk_keeper_make_space(keeper,sizeof(GLYPH_INDEX))) return 0;

    index = (GLYPH_INDEX*)_gk_malloc(sizeof(GLYPH_INDEX));
    if (!index) return 0;

    index->pages = 0;
    _gk_glyph_index_init(index,rend);

    index->keeper = keeper;
    index->prev = 0;
    index->next = keeper->first_index;
    if (keeper->first_index) keeper->first_index->prev = index;
    if (!keeper->last_index) keeper->last_index = index;
    keeper->first_index = index;

    index->first_renderer = 0;
    index->last_renderer = 0;

    keeper->allocated += sizeof(GLYPH_INDEX);
    return index;
}


/* callers: gk_done_keeper */
static void _gk_glyph_index_done(GLYPH_INDEX* const index)
{
    GLYPH_KEEP* keeper;
    GLYPH_REND* renderer;
    int a,b,c;

    CARE(index);

    keeper = index->keeper;
    for (renderer = index->first_renderer; renderer; renderer = renderer->next_for_same_index)
        gk_rend_set_keeper(renderer,0);

    if (index->pages)
    {
        for (a=0; a<68; a++)
        {
            if (index->pages[a])
            {
                for (b=0; b<128; b++)
                {
                    if (index->pages[a][b])
                    {
                        for (c=0; c<128; c++)
                        {
                            if (index->pages[a][b][c])
                            {
                                GLYPH *glyph = index->pages[a][b][c];
                                index->pages[a][b][c] = 0;

                                if (glyph == keeper->head) keeper->head = keeper->head->next;
                                if (glyph == keeper->tail) keeper->tail = keeper->tail->prev;
                                if (glyph->prev) glyph->prev->next = glyph->next;
                                if (glyph->next) glyph->next->prev = glyph->prev;
                                keeper->num_glyphs--;
                                keeper->allocated -= glyph_size_in_bytes(glyph);

                                if (glyph->bmp) _gk_free(glyph->bmp);
                                _gk_free(glyph);
                            }
                        }
                        _gk_free(index->pages[a][b]);
                        index->pages[a][b] = 0;
                        keeper->allocated -= 129*sizeof(void*);
                    }
                }
                _gk_free(index->pages[a]);
                index->pages[a] = 0;
                keeper->allocated -= 129*sizeof(void*);
            }
        }
        _gk_free(index->pages);
        index->pages = 0;
        keeper->allocated -= 69*sizeof(void*);
    }

    if (index == keeper->first_index) keeper->first_index = index->next;
    if (index == keeper->last_index) keeper->last_index = index->prev;
    if (index->next) index->next->prev = index->prev;
    if (index->prev) index->prev->next = index->next;

    _gk_free(index);
    keeper->allocated -= sizeof(GLYPH_INDEX);
}


/* Uncaches one glyph, and releases all its memory. */
/* If the glyph was the last one, referenced in it's index page, */
/* the page will also removed. All higher level pages, including root page, */
/* will be removed as well, if they appear to be empty after glyph deletion. */
/* If all index pages were deleted and GLYPH_INDEX object itself is not */
/* referenced by any GLYPH_REND objects, GLYPH_INDEX object is also deleted. */
static void _gk_unload_glyph(GLYPH* const glyph)
{
    GLYPH_INDEX *index;
    GLYPH_KEEP *keeper;
    GLYPH ***page_b,**page_c;
    int a_index,b_index,c_index;

    if (!glyph) return;

    /* if glyph is not being kept by keeper, just _gk_free() it. */
    if (!glyph->index)
    {
        if (glyph->bmp) _gk_free(glyph->bmp);
        _gk_free(glyph);
        return;
    }

    index = glyph->index;
    keeper = index->keeper;
    CARE(keeper);

    /* if GLYPH_INDEX has no pages, it means it can't keep any glyphs, crash */
    CARE(index->pages);

    a_index = glyph->unicode/16384;
    c_index = glyph->unicode - a_index*16384;

    page_b = index->pages[a_index];
    CARE(page_b);
    b_index = c_index/128;

    page_c = page_b[b_index];
    CARE(page_c);
    c_index -= b_index*128;

    /* check if the index actually keeps the same glyph we are uncaching */
    CARE(page_c[c_index]==glyph);

    /* unlinking the glyph from index */
    page_c[c_index] = 0;
    (*(long*)(page_c+128))--;

    /* uninking the glyph from keeper's list */
    if (glyph == keeper->head) keeper->head = keeper->head->next;
    if (glyph == keeper->tail) keeper->tail = keeper->tail->prev;
    if (glyph->prev) glyph->prev->next = glyph->next;
    if (glyph->next) glyph->next->prev = glyph->prev;

    /* releasing glyph's memory */
    keeper->num_glyphs--;
    keeper->allocated -= glyph_size_in_bytes(glyph);
    if (glyph->bmp) _gk_free(glyph->bmp);
    _gk_free(glyph);

    /* releasing unnecessary index pages, and index itself */
    if (((long)page_c[128])<=0)
    {
        _gk_free(page_c);
        page_b[b_index] = 0;
        keeper->allocated -= 129*sizeof(void*);
        (*(long*)(page_b+128))--;
        if (((long)page_b[128])<=0)
        {
            _gk_free(page_b);
            index->pages[a_index] = 0;
            keeper->allocated -= 129*sizeof(void*);
            (*(long*)(index->pages+68))--;
            if (((long)index->pages[68])<=0)
            {
                _gk_free(index->pages);
                index->pages = 0;
                keeper->allocated -= 69*sizeof(void*);

                /* no any glyphs left, no renderers.. -> kill the index object */
                if (!index->first_renderer) 
                {
                    if (index == keeper->first_index) keeper->first_index = index->next;
                    if (index == keeper->last_index) keeper->last_index = index->prev;
                    if (index->next) index->next->prev = index->prev;
                    if (index->prev) index->prev->next = index->next;
                    _gk_free(index);
                    keeper->allocated -= sizeof(GLYPH_INDEX);
                }
            }
        }
    }
}


/* Adds 'glyph' into cache. */
/* Returns 1 if glyph is saved, 0 if glyph is not saved. */
/* Callers: rend_workout. */
static int _gk_glyph_index_add_glyph(GLYPH_INDEX* const index,GLYPH* const glyph)
{
    GLYPH_KEEP *keeper;
    GLYPH ***page_b,**page_c;
    int a_index,b_index,c_index;
    int bytes;
    funcname = "_gk_glyph_index_add_glyph()";

    CARE(index);
    CARE(glyph);

    keeper = index->keeper;

    /* If the keeper has limitation on number of glyphs, */
    /* make sure to not exceed the limit, by removing most long-ago-used glyphs. */
    if (keeper->max_glyphs)
    {
        while ((keeper->num_glyphs >= keeper->max_glyphs) && keeper->tail)
            _gk_unload_glyph(keeper->tail);
        if (keeper->num_glyphs >= keeper->max_glyphs) return 0;
    }

    /* Asking for memory */
    bytes = glyph_size_in_bytes(glyph);
    if (!_gk_keeper_make_space(keeper,bytes+(129+129+69)*sizeof(void*))) return 0;

    /* creating root directory, if not exist */
    if (!index->pages)
    {
        index->pages = (GLYPH****)_gk_malloc(69*sizeof(void*));
        if (!index->pages) return 0;
        memset(index->pages,0,69*sizeof(void*));
        keeper->allocated += 69*sizeof(void*);
    }

    a_index = glyph->unicode/16384;
    c_index = glyph->unicode - a_index*16384;

    if (!index->pages[a_index])
    {
        index->pages[a_index] = (GLYPH***)_gk_malloc(129*sizeof(void*));
        if (!index->pages[a_index]) return 0;
        (*(long*)(index->pages+68))++;
        memset(index->pages[a_index],0,129*sizeof(void*));
        keeper->allocated += 129*sizeof(void*);
    }

    page_b = index->pages[a_index];
    b_index = c_index/128;

    if (!page_b[b_index])
    {
        page_b[b_index] = (GLYPH**)_gk_malloc(129*sizeof(void*));
        if (!page_b[b_index]) return 0;
        (*(long*)(page_b+128))++;
        memset(page_b[b_index],0,129*sizeof(void*));
        keeper->allocated += 129*sizeof(void*);
    }

    page_c = page_b[b_index];
    c_index -= b_index*128;

    /* There should be NO glyph at this place at this moment */
    CARE(page_c[c_index]==0);

    page_c[c_index] = glyph;
    (*(int*)(page_c+128))++;
    glyph->index = index;

    glyph->prev = 0;
    glyph->next = keeper->head;
    if (keeper->head) keeper->head->prev = glyph;
    if (!keeper->tail) keeper->tail = glyph;
    keeper->head = glyph;

    keeper->allocated += bytes;
    keeper->num_glyphs++;
    return 1;
}


/* Finds the glyph for a given unicode. */
/* Callers: rend_render. */
static GLYPH* glyph_index_find_glyph(GLYPH_INDEX* const index,const unsigned unicode)
{
    GLYPH *glyph;
    GLYPH_KEEP *keeper;
    GLYPH ***page_b,**page_c;
    int a_index,b_index,c_index;

    CARE(index);
    CARE(unicode<=GK_MAX_UNICODE);

    if (!index->pages) return 0;

    /*a_index = unicode/16384;*/
    a_index = unicode >> 14;

    /*c_index = unicode - a_index*16384;*/
    c_index = unicode - (a_index << 14);

    page_b = index->pages[a_index];
    if (!page_b) return 0;

    /*b_index = c_index/128;*/
    b_index = c_index >> 7;

    page_c = page_b[b_index];
    if (!page_c) return 0;

    /*c_index -= b_index*128;*/
    c_index -= b_index << 7;

    if (!page_c[c_index]) return 0;
    glyph = page_c[c_index];

    keeper = index->keeper;
    if (glyph!=keeper->head)
    {
        if (glyph->prev) glyph->prev->next = glyph->next;
        if (glyph->next) glyph->next->prev = glyph->prev;
        if (keeper->tail==glyph) keeper->tail = glyph->prev;
        if (keeper->head) keeper->head->prev = glyph;
        glyph->next = keeper->head;
        glyph->prev = 0;
        keeper->head = glyph;
    }

    return glyph;
}





/*
 * GLYPH_KEEP routines
 */


/* Tries to make free memory, to store 'bytes' bytes */
/* without exceeding 'max_memory' limit. */
/* Returns 1 on success, 0 on failure. */
/* Callers: _gk_glyph_index_create, _gk_glyph_index_add_glyph */
static int _gk_keeper_make_space(GLYPH_KEEP* const keeper,const int bytes)
{
    CARE(keeper);

    /* no need to do anything, the memory is not limited */
    if (!keeper->max_memory) return 1;

    /* trying to release glyphs, until enough memory is free
       older glyphs are released first */
    while ( (keeper->allocated + bytes > keeper->max_memory) && keeper->tail)
        _gk_unload_glyph(keeper->tail);
    if (keeper->allocated + bytes <= keeper->max_memory) return 1;
    return 0;
}


GLYPH_KEEP* gk_create_keeper(const unsigned max_glyphs,const unsigned max_memory)
{
    GLYPH_KEEP *keeper;
    funcname = "gk_create_keeper()";

    keeper = (GLYPH_KEEP*)_gk_malloc(sizeof(GLYPH_KEEP));
    if (!keeper) return 0;

    keeper->head = 0;
    keeper->tail = 0;
    keeper->num_glyphs = 0;
    keeper->allocated = sizeof(GLYPH_KEEP);

    keeper->max_glyphs = max_glyphs;
    keeper->max_memory = max_memory;

    keeper->first_index = 0;
    keeper->last_index = 0;

    keeper->next = 0;
    keeper->prev = last_keeper;
    if (last_keeper) last_keeper->next = keeper;
    if (!first_keeper) first_keeper = keeper;
    last_keeper = keeper;

    _gk_install_exit_handler();

    _gk_msg("glyph keeper created\n");
    return keeper;
}


void gk_done_keeper(GLYPH_KEEP* const keeper)
{
    int mem;

    if (!keeper) return;
    while (keeper->head) _gk_unload_glyph(keeper->head);
    while (keeper->first_index) _gk_glyph_index_done(keeper->first_index);

    if (keeper==first_keeper) first_keeper = keeper->next;
    if (keeper==last_keeper) last_keeper = keeper->prev;
    if (keeper->next) keeper->next->prev = keeper->prev;
    if (keeper->prev) keeper->prev->next = keeper->next;
    mem = keeper->allocated - sizeof(GLYPH_KEEP);
    _gk_free(keeper);
    _gk_msg("glyph keeper destroyed, (%d bytes leak)\n",mem);
}


int gk_keeper_glyph_count(const GLYPH_KEEP* const keeper)
{
    return keeper ? keeper->num_glyphs : 0;
}


int gk_keeper_glyph_limit(const GLYPH_KEEP* const keeper)
{
    return keeper ? keeper->max_glyphs : 0;
}


int gk_keeper_byte_count(const GLYPH_KEEP* const keeper)
{
    return keeper ? keeper->allocated : 0;
}


int gk_keeper_byte_limit(const GLYPH_KEEP* const keeper)
{
    return keeper ? keeper->max_memory : 0;
}


void gk_keeper_debug(const GLYPH_KEEP* const keeper)
{
    GLYPH_INDEX *index;
    GLYPH_REND *rend;
    int ni = 0, nr = 0;

    _gk_msg("GLYPH_KEEP object (address:%p):\n",keeper);

    if (!keeper) return;
    _gk_msg("  can keep %d glyphs, currently keeping %d\n",keeper->max_glyphs,keeper->num_glyphs);
    _gk_msg("  can use %d bytes of memory, now using %d bytes\n",keeper->max_memory,keeper->allocated);

    for (index=keeper->first_index;index;index=index->next)
    {
        ni++;
        for (rend=index->first_renderer;rend;rend=rend->next_for_same_index) nr++;
    }
    _gk_msg("  this keeper has %d GLYPH_INDEX objects, used by %d renderers\n",ni,nr);
}


#endif  /* included_from_glyph_c */
