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
#include "global.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winalleg.h>
#endif
#include "font.h"

FONT *small;
static int small_w[256];

FONT *large;
static int large_w[256];

static void calc_glyph_width(AL_CONST FONT *f, int *width)
{
	FONT_COLOR_DATA *cf = (FONT_COLOR_DATA*)(f->data);

	for (int ch = cf->begin; ch < cf->end; ch++) {
		//BITMAP *g = find_glyph(f, ch);
		BITMAP *g = cf->bitmaps[ch - cf->begin];

		if (g == NULL) {
			width[ch] = 0;
			continue;
		}
		for (int x = g->w - 1; x >= 0; x--)
			for (int y = 0; y < g->h; y++) {
				if (getpixel(g, x, y) != 0) {
					width[ch] = x;
					goto next_ch;
				}
			}
		width[ch] = 5;
next_ch: ;
	}
}

static BITMAP *find_glyph(AL_CONST FONT *f, int ch)
{
	FONT_COLOR_DATA *cf = (FONT_COLOR_DATA*)(f->data);

	while (cf) {
		if (ch >= cf->begin && ch < cf->end) return cf->bitmaps[ch - cf->begin];
		cf = cf->next;
	}

	if (ch != allegro_404_char) return find_glyph(f, allegro_404_char);
	return NULL;
}


static void render_glyph(BITMAP *bmp, BITMAP *g, int x, int y, int fg)
{
	BITMAP *cg = create_bitmap(g->w, g->h);
	//memcpy(cg->dat, g->dat, g->w*g->h);
	for (int i = 0; i < g->w*g->h; i++) {
		char a = ((char *)g->dat)[i];
		if (a)
			((char *)cg->dat)[i] = a * 2 + fg;
		else
			((char *)cg->dat)[i] = 0;
	}
	bmp->vtable->draw_256_sprite(bmp, cg, x, y);
	destroy_bitmap(cg);
}


static int small_length(AL_CONST FONT *f, AL_CONST char *text)
{
	AL_CONST char *p = text;
	int ch = 0, w = 0;

	while ((ch = ugetxc(&p))) {
		BITMAP *g = find_glyph(f, ch);
		//if(g) w += g->w;
		if (g != NULL)
			w += small_w[ch];
	}
	return w + 1;
}

static int large_length(AL_CONST FONT *f, AL_CONST char *text)
{
	AL_CONST char *p = text;
	int ch = 0, w = 0;

	while ((ch = ugetxc(&p))) {
		BITMAP *g = find_glyph(f, ch);
		//if(g) w += g->w;
		if (g != NULL)
			w += large_w[ch];
	}
	return w + 1;
}


static void small_render(AL_CONST FONT *f, AL_CONST char *text, int fg, int bg, BITMAP *bmp, int x, int y)
{
	AL_CONST char *p = text;
	int ch = 0;

	int old_textmode = _textmode;
	_textmode = bg;

	acquire_bitmap(bmp);

	if (fg < 0 && bg >= 0) rectfill(bmp, x, y, x + text_length(f, text) - 1, y + text_height(f) - 1, bg);

	while ((ch = ugetxc(&p))) {
		BITMAP *g = find_glyph(f, ch);
		if (g) {
			if (fg < 0) {
				bmp->vtable->draw_256_sprite(bmp, g, x, y);
			} else {
				//bmp->vtable->draw_character(bmp, g, x, y, fg);
				render_glyph(bmp, g, x, y, fg);
			}
			//x += g->w;
			x += small_w[ch];
		}
	}

	release_bitmap(bmp);
	_textmode = old_textmode;
}

static void large_render(AL_CONST FONT *f, AL_CONST char *text, int fg, int bg, BITMAP *bmp, int x, int y)
{
	AL_CONST char *p = text;
	int ch = 0;

	int old_textmode = _textmode;
	_textmode = bg;
	acquire_bitmap(bmp);
	if (fg < 0 && bg >= 0) rectfill(bmp, x, y, x + text_length(f, text) - 1, y + text_height(f) - 1, bg);

	while ((ch = ugetxc(&p))) {
		BITMAP *g = find_glyph(f, ch);
		if (g) {
			if (fg < 0) {
				bmp->vtable->draw_256_sprite(bmp, g, x, y);
			} else {
				//bmp->vtable->draw_character(bmp, g, x, y, fg);
				render_glyph(bmp, g, x, y, fg);
			}
			//x += g->w;
			x += large_w[ch];
		}
	}

	release_bitmap(bmp);
	_textmode = old_textmode;
}


static void destroy_glyph(FONT *f)
{
	FONT_COLOR_DATA *cf = (FONT_COLOR_DATA*)(f->data);

	if (f == NULL) return ;

	while (cf) {
		FONT_COLOR_DATA *next = cf->next;
		int i = 0;

		for (i = cf->begin; i < cf->end; i++) destroy_bitmap(cf->bitmaps[i - cf->begin]);

		free(cf->bitmaps);
		free(cf);

		cf = next;
	}
	free(f);
}


static FONT_VTABLE font_vtable_small = {
                                           NULL,
                                           NULL,
                                           small_length,
                                           NULL,
                                           small_render,
                                           destroy_glyph
                                       };

static FONT_VTABLE font_vtable_large = {
                                           NULL,
                                           NULL,
                                           large_length,
                                           NULL,
                                           large_render,
                                           destroy_glyph
                                       };

static BITMAP **glyph_bmp_small, **glyph_bmp_large;
static int glyph_num_small, glyph_num_large;
static FONT_COLOR_DATA *fcd_small, *fcd_large;

void create_small_font()
{
	int fh = open("geodata/smallset.dat", O_RDONLY | O_BINARY);
	assert(fh != -1);
	int fl = filelength(fh);
	char *dat = new char[fl];
	read(fh, dat, fl);
	close(fh);

	int glyph_num = fl / 72 + 1;
	glyph_num_small = glyph_num;
	BITMAP **glyph_bmp = new BITMAP *[glyph_num];
	glyph_bmp_small = glyph_bmp;
	glyph_bmp[0] = create_bitmap(8, 9); clear(glyph_bmp[0]);      // 32 ' '
	int i = 0;
	for (int n = 1; n < glyph_num; n++) {
		glyph_bmp[n] = create_bitmap(8, 9);
		for (int h = 0; h < 9; h++)
			for (int w = 0; w < 8; w++)
				putpixel(glyph_bmp[n], w, h, dat[i++]);
		//draw_sprite(screen, glyph_bmp[n], (n%32)*10, (n/32)*10);
	}
	delete []dat;

	FONT_COLOR_DATA *fcd;
	fcd = new FONT_COLOR_DATA;
	fcd_small = fcd;
	fcd->begin = 32; fcd->end = 32 + glyph_num;
	fcd->bitmaps = glyph_bmp;
	fcd->next = NULL;

	small = new FONT;
	small->data = fcd;
	small->height = 9;
	small->vtable = &font_vtable_small;
	calc_glyph_width(small, small_w);
}

void free_small_font()
{
	delete small;
	delete fcd_small;

	for (int n = 0; n < glyph_num_small; n++)
		destroy_bitmap(glyph_bmp_small[n]);

	delete [] glyph_bmp_small;
} 

void create_large_font()
{
	int fh = open("geodata/biglets.dat", O_RDONLY | O_BINARY);
	assert(fh != -1);
	int fl = filelength(fh);
	char *dat = new char[fl];
	read(fh, dat, fl);
	close(fh);

	int glyph_num = fl / 256 + 1;
	glyph_num_large = glyph_num;
	BITMAP **glyph_bmp = new BITMAP *[glyph_num];
	glyph_bmp_large = glyph_bmp;
	glyph_bmp[0] = create_bitmap(16, 16); clear(glyph_bmp[0]);      // 32 ' '
	int i = 0;
	for (int n = 1; n < glyph_num; n++) {
		glyph_bmp[n] = create_bitmap(16, 16);
		for (int h = 0; h < 16; h++)
			for (int w = 0; w < 16; w++)
				putpixel(glyph_bmp[n], w, h, dat[i++]);
		//draw_sprite(screen, glyph_bmp[n], (n%32)*10, (n/32)*10);
	}
	delete []dat;

	FONT_COLOR_DATA *fcd;
	fcd = new FONT_COLOR_DATA;
	fcd_large = fcd;
	fcd->begin = 32; fcd->end = 32 + glyph_num;
	fcd->bitmaps = glyph_bmp;
	fcd->next = NULL;

	large = new FONT;
	large->data = fcd;
	large->height = 16;
	large->vtable = &font_vtable_large;
	calc_glyph_width(large, large_w);
}

void free_large_font()
{
	delete large;
	delete fcd_large;

	for (int n = 0; n < glyph_num_large; n++)
		destroy_bitmap(glyph_bmp_large[n]);

	delete [] glyph_bmp_large;
} 

void printsmall(int x, int y, int col, int value)
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
					putpixel(screen2, x + i * 4 + xx, y + yy, col);

}

void printsmall_center(int x, int y, int col, int value)
{
	char buf[100];
	sprintf(buf, "%d", value);
	printsmall(x - strlen(buf) * 4 / 2, y, col, value);
}
