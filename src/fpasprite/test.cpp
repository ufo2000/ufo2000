/*
   Fast and Portable Allegro/Alpha Sprite Library
   Copyright (C) 2006 by Siarhei Siamashka (ssvb@users.sourceforge.net)

   The code presented in this file may be used in any environment it is
   acceptable to use Allegro library (http://alleg.sourceforge.net).

   This library is maintained as part of UFO2000 project and the latest
   version can be found in its source tree: http://ufo2000.sourceforge.net

   Please note that UFO2000 uses more restrictive GNU GPL license itself,
   so Allegro license applies only to this particular file as an exception.
*/
#include <allegro.h>
#include <stdio.h>
#include "fpasprite.h"

static BITMAP *convert_bitmap_alpha(BITMAP *bmp_orig, bool keep_alpha)
{
    if (!bmp_orig) return NULL;

    // Non 32bpp bitmap just can't contain alpha channel
    if (bitmap_color_depth(bmp_orig) != 32) return bmp_orig;

    // Try to detect alpha channel in the bitmap
    bool has_alpha = false;
    bool has_trans = false;
    bool has_opaque = false;
    for (int y = 0; y < bmp_orig->h; y++) {
        const uint32 *line = (const uint32 *)bmp_orig->line[y];
        for (int x = 0; x < bmp_orig->w; x++) {
            uint32 a = line[x] & 0xFF000000;
            if (a == 0)
                has_trans = true;
            else if (a == 0xFF000000)
                has_opaque = true;
            else
                has_alpha = true;
        }
    }
    
    if (has_alpha || keep_alpha) {
        for (int x = 0; x < bmp_orig->w; x++)
            for (int y = 0; y < bmp_orig->h; y++) {
                int c = getpixel(bmp_orig, x, y);
                if (geta32(c) == 0)
                    putpixel(bmp_orig, x, y, MASK_COLOR_32);
            }
        return bmp_orig;
    } else {
        bool has_1bit_alpha = has_trans && has_opaque;
        BITMAP *bmp = create_bitmap(bmp_orig->w, bmp_orig->h);
        clear_to_color(bmp, bitmap_mask_color(bmp));
        for (int x = 0; x < bmp_orig->w; x++)
            for (int y = 0; y < bmp_orig->h; y++) {
                int c = getpixel(bmp_orig, x, y);
                if (!has_1bit_alpha || geta32(c) != 0) {
                    putpixel(bmp, x, y, makeacol(getr32(c), getg32(c), getb32(c), 255));
                }
            }
        destroy_bitmap(bmp_orig);
        return bmp;
    }
}

/**
 * Load bitmap (this function is aware of truecolor transparency)
 */
static BITMAP *load_bitmap_alpha(const char *filename, bool keep_alpha)
{
    // Allow any color conversions except when loaded file 
    // contains alpha channel
    int cc = get_color_conversion();
    set_color_conversion((COLORCONV_TOTAL | COLORCONV_KEEP_TRANS) & 
        ~(COLORCONV_32A_TO_8 | COLORCONV_32A_TO_15 | COLORCONV_32A_TO_16 | COLORCONV_32A_TO_24));
    BITMAP *bmp_orig = load_bitmap(filename, NULL);
    set_color_conversion(cc);
    return convert_bitmap_alpha(bmp_orig, keep_alpha);
}

BITMAP *backbuffer;

int TESTS_COUNT = 10000;

void test_dryrun(BITMAP *dst, int x, int y, int frame, int brightness)
{
}

/**
 * Run test
 */
double run_test(BITMAP *backbuffer, void (*fn)(BITMAP *dst, int x, int y, int frame, int brightness))
{
    int i;
    clock_t before, after;

    srand(0);
    before = clock();
    for (i = 0; i < TESTS_COUNT; i++) {
        int x = (rand() % (SCREEN_W + 200)) - 100;
        int y = (rand() % (SCREEN_H + 200)) - 100;
        int frame = rand();
        int brightness = rand() % 256;
        test_dryrun(backbuffer, x, y, frame, brightness);
    }
    clock_t dryrun = clock() - before;

    clear_to_color(backbuffer, makecol(0, 0, 0));
    srand(0);
    before = clock();
    for (i = 0; i < TESTS_COUNT; i++) {
        int x = (rand() % (SCREEN_W + 200)) - 100;
        int y = (rand() % (SCREEN_H + 200)) - 100;
        int frame = rand();
        int brightness = rand() % 256;
        fn(backbuffer, x, y, frame, brightness);
    }
    after = clock();
    return (double)TESTS_COUNT / ((double)(after - before - dryrun) / CLOCKS_PER_SEC);
}

bool compare_bitmaps(BITMAP *bmp1, BITMAP *bmp2)
{
    if (bmp1->w != bmp2->w) return false;
    if (bmp1->h != bmp2->h) return false;
    for (int x = 0; x < bmp1->w; x++)
        for (int y = 0; y < bmp1->h; y++) {
            if (getpixel(bmp1, x, y) != getpixel(bmp2, x, y)) {
                return false;
            }
        }
    return true;
}

#define SPRITE_COUNT 512

BITMAP *fire;
BITMAP *fire_bitmaps[SPRITE_COUNT];
RLE_SPRITE *fire_rle_sprites[SPRITE_COUNT];
ALPHA_SPRITE *fire_alpha_sprites[SPRITE_COUNT];

BITMAP *fire_ex;
BITMAP *fire_ex_bitmaps[SPRITE_COUNT];
RLE_SPRITE *fire_ex_rle_sprites[SPRITE_COUNT];
ALPHA_SPRITE *fire_ex_alpha_sprites[SPRITE_COUNT];

BITMAP *explosion;
BITMAP *explosion_bitmaps[10];
RLE_SPRITE *explosion_rle_sprites[10];
ALPHA_SPRITE *explosion_alpha_sprites[10];

// Tests for blitting alpha transparent sprites

void test_explosion_spritelib(BITMAP *dst, int x, int y, int frame, int brightness)
{
    draw_alpha_sprite(dst, explosion_alpha_sprites[frame % 10], x, y);    
}

void test_explosion_allegro(BITMAP *dst, int x, int y, int frame, int brightness)
{
    set_alpha_blender();
    draw_trans_sprite(dst, explosion_bitmaps[frame % 10], x, y);
}

void test_explosion_allegro_rle(BITMAP *dst, int x, int y, int frame, int brightness)
{
    set_alpha_blender();
    draw_trans_rle_sprite(dst, explosion_rle_sprites[frame % 10], x, y);
}

// Tests for blitting ordinary rle sprites

void test_fire_spritelib(BITMAP *dst, int x, int y, int frame, int brightness)
{
    draw_alpha_sprite(dst, fire_alpha_sprites[frame % SPRITE_COUNT], x, y);    
}

void test_fire_allegro_rle(BITMAP *dst, int x, int y, int frame, int brightness)
{
    draw_rle_sprite(dst, fire_rle_sprites[frame % SPRITE_COUNT], x, y);
}

// Tests for blitting lit rle sprites

void test_fire_lit_spritelib(BITMAP *dst, int x, int y, int frame, int brightness)
{
    draw_alpha_sprite(dst, fire_alpha_sprites[frame % SPRITE_COUNT], x, y, brightness);    
}

void test_fire_lit_allegro_rle(BITMAP *dst, int x, int y, int frame, int brightness)
{
    if (brightness >= 255) {
        draw_rle_sprite(dst, fire_rle_sprites[frame % SPRITE_COUNT], x, y);
    } else {
        set_trans_blender(0, 0, 0, 0);
        draw_lit_rle_sprite(dst, fire_rle_sprites[frame % SPRITE_COUNT], x, y, 255 - brightness);
    }
}

// Tests for blitting alpha rle sprites

void test_fire_alpha_spritelib(BITMAP *dst, int x, int y, int frame, int brightness)
{
    draw_alpha_sprite(dst, fire_ex_alpha_sprites[frame % SPRITE_COUNT], x, y);
}

void test_fire_alpha_allegro_rle(BITMAP *dst, int x, int y, int frame, int brightness)
{
    set_alpha_blender();
    draw_trans_rle_sprite(dst, fire_ex_rle_sprites[frame % SPRITE_COUNT], x, y);
}

int main(int argc, char *argv[])
{
    int i, idx, color_depth = -1;
    if (argc >= 2) color_depth = atoi(argv[1]);
    if (color_depth != 16 && color_depth != 24 && color_depth != 32) {
        printf("Usage: test [color_depth]\n");
        return -1;
    }

    allegro_init();
    install_keyboard();
    set_color_depth(color_depth);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) return -1;
    backbuffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *backbuffer1 = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *backbuffer2 = create_bitmap(SCREEN_W, SCREEN_H);

    // Load fire sprites (alpha transparency used)
    fire_ex = load_bitmap_alpha("fire.tga", true);
    for (int idx = 0; idx < SPRITE_COUNT; idx++) {
        i = idx % 8;
        fire_ex_bitmaps[idx] = create_sub_bitmap(fire_ex, (i % 4) * 33, (i / 4) * 41, 32, 40);
        fire_ex_rle_sprites[idx] = get_rle_sprite(fire_ex_bitmaps[i]);
        fire_ex_alpha_sprites[idx] = get_alpha_sprite(fire_ex_bitmaps[i]);
    }

    // Load fire sprites (no alpha transparency used)
    fire = load_bitmap_alpha("fire.tga", false);
    for (int idx = 0; idx < SPRITE_COUNT; idx++) {
        i = idx % 8;
        fire_bitmaps[idx] = create_sub_bitmap(fire, (i % 4) * 33, (i / 4) * 41, 32, 40);
        fire_rle_sprites[idx] = get_rle_sprite(fire_bitmaps[i]);
        fire_alpha_sprites[idx] = get_alpha_sprite(fire_bitmaps[i]);
    }

    // Load explosion sprites (alpha transparency used)
    explosion = load_bitmap_alpha("explosion.tga", true);
    for (i = 0; i < 10; i++) {
        explosion_bitmaps[i] = create_sub_bitmap(explosion, 0, 179 * i, 256, 178);
        explosion_rle_sprites[i] = get_rle_sprite(explosion_bitmaps[i]);
        explosion_alpha_sprites[i] = get_alpha_sprite(explosion_bitmaps[i]);
    }

    printf("---\n");
    printf("explosion sprites per second (spritelib)      = %.1lf\n", run_test(backbuffer1, test_explosion_spritelib));
    printf("explosion sprites per second (allegro rle)    = %.1lf\n", run_test(backbuffer2, test_explosion_allegro_rle));
    printf("explosion sprites per second (allegro bitmap) = %.1lf\n", run_test(backbuffer2, test_explosion_allegro));
    if (!compare_bitmaps(backbuffer1, backbuffer2)) printf("Error: results do not match!\n");
    TESTS_COUNT = 500000;
    printf("---\n");
    printf("normal fire sprites per second (spritelib)    = %.1lf\n", run_test(backbuffer1, test_fire_spritelib));
    printf("normal fire sprites per second (allegro rle)  = %.1lf\n", run_test(backbuffer2, test_fire_allegro_rle));
    if (!compare_bitmaps(backbuffer1, backbuffer2)) printf("Error: results do not match!\n");
    printf("---\n");
    printf("lit fire sprites per second (spritelib)       = %.1lf\n", run_test(backbuffer1, test_fire_lit_spritelib));
    printf("lit fire sprites per second (allegro rle)     = %.1lf\n", run_test(backbuffer2, test_fire_lit_allegro_rle));
    if (!compare_bitmaps(backbuffer1, backbuffer2)) printf("Error: results do not match!\n");
    printf("---\n");
    printf("alpha fire sprites per second (spritelib)     = %.1lf\n", run_test(backbuffer1, test_fire_alpha_spritelib));
    printf("alpha fire sprites per second (allegro rle)   = %.1lf\n", run_test(backbuffer2, test_fire_alpha_allegro_rle));
    if (!compare_bitmaps(backbuffer1, backbuffer2)) printf("Error: results do not match!\n");

    i = 0;
    while (true) {
        if (keypressed()) {
            int scancode;
            ureadkey(&scancode);
            if (scancode == KEY_ESC) break;
        }
        clear_to_color(backbuffer, makecol(64, 64, 64));
        int brightness = (i % 512) - 256;
        if (brightness < 0) brightness = -brightness;
        draw_alpha_sprite(backbuffer, explosion_alpha_sprites[i % 10], 0, 0, brightness);
        draw_alpha_sprite(backbuffer, fire_alpha_sprites[i % 4], 300, 0, brightness);
        draw_alpha_sprite(backbuffer, fire_alpha_sprites[(i % 4) + 4], 300, 50, 256 - brightness);
        draw_alpha_sprite(backbuffer, fire_alpha_sprites[i % 4], 300, 100, 250);
        draw_alpha_sprite(backbuffer, fire_alpha_sprites[(i % 4) + 4], 300, 150, 250);
        blit(backbuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(100);
        i++;
    }

    destroy_bitmap(backbuffer);
    destroy_bitmap(backbuffer1);
    destroy_bitmap(backbuffer2);

    for (idx = 0; idx < SPRITE_COUNT; idx++) {
        destroy_bitmap(fire_ex_bitmaps[idx]);
        destroy_rle_sprite(fire_ex_rle_sprites[idx]);
        destroy_alpha_sprite(fire_ex_alpha_sprites[idx]);
        destroy_bitmap(fire_bitmaps[idx]);
        destroy_rle_sprite(fire_rle_sprites[idx]);
        destroy_alpha_sprite(fire_alpha_sprites[idx]);
    }
    destroy_bitmap(fire_ex);
    destroy_bitmap(fire);

    // Load explosion sprites (alpha transparency used)
    for (i = 0; i < 10; i++) {
        destroy_bitmap(explosion_bitmaps[i]);
        destroy_rle_sprite(explosion_rle_sprites[i]);
        destroy_alpha_sprite(explosion_alpha_sprites[i]);
    }
    destroy_bitmap(explosion);

    allegro_exit();

    return 0;
}
END_OF_MAIN();
