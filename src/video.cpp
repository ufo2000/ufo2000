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
#include <stdio.h>

#ifdef LINUX
#include <stdlib.h>
#include <unistd.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "video.h"
#include "wind.h"
#include "../ufo2000.h"
#include "global.h"
#include "map.h"
#include "config.h"

unsigned long FLAGS = 0;

DATAFILE *datafile;

BITMAP *mouser, *selector;
BITMAP *screen2;

int SCREEN2W = 320, SCREEN2H = 200;

volatile int DRAWIT = 0;
void drawit_timer()
{
	DRAWIT++;
}
END_OF_FUNCTION(drawit_timer);

//! Counter for number of switches to ufo2000 from other programs
volatile int g_switch_in_counter;

/**
 * This function is called when we switch back to ufo2000 from other 
 * task in win32. All the information drawn on screen is lost, so we
 * need some way to detect this situation.
 */
void switch_in_callback()
{
	g_switch_in_counter++;
}
END_OF_FUNCTION(switch_in_callback);

void initvideo()
{
	LOCK_VARIABLE(DRAWIT);
	LOCK_FUNCTION(drawit_timer);
	LOCK_VARIABLE(g_switch_in_counter);
	LOCK_FUNCTION(switch_in_callback);

	selector = (BITMAP*)datafile[DAT_SELECT_BMP].dat;
	mouser = (BITMAP*)datafile[DAT_MOUSE_BMP].dat;

	screen2 = create_bitmap(SCREEN2W, SCREEN2H);
	clear(screen2);

	font = (FONT*)datafile[DAT_UNIFONT_8X8].dat;
	create_small_font();
	create_large_font();

	if (FLAGS & F_LARGEFONT) {
		font = large;
	}
	
	if (FLAGS & F_SMALLFONT) {
		font = g_small_font;
	}
	
}

void closevideo()
{
	free_large_font();
	free_small_font();

	destroy_bitmap(screen2);
}

void change_screen_mode()
{
	if (FLAGS & F_SWITCHVIDEO) {
		FLAGS ^= F_FULLSCREEN;
		set_video_mode();
		reset_video();
		g_switch_in_counter++;
	}
}

int xcom1_color_table[256];
int xcom1_menu_color_table[256];

/**
 * Function that converts color from xcom1 game palette to current
 * representation
 *
 * @param c  xcom1 color
 * @return   allegro color
 */
int (*xcom1_color)(int c);
int (*xcom1_menu_color)(int c);
int (*xcom1_darken_color)(int c, int level);

static int xcom1_color_8bpp(int c) { return c; }
static int xcom1_menu_color_8bpp(int c) { return c; }
static int xcom1_darken_color_8bpp(int c, int level) { return c + level * 2; }

static int xcom1_color_high_bpp(int c)
{
	assert(c >= 0 && c < 256);
	return xcom1_color_table[c]; 
}
static int xcom1_menu_color_high_bpp(int c)
{ 
	assert(c >= 0 && c < 256);
	return xcom1_menu_color_table[c]; 
}
static int xcom1_darken_color_high_bpp(int c, int level)
{ 
	return makecol(
		getr(c) * (8 - level) / 8, 
		getg(c) * (8 - level) / 8, 
		getb(c) * (8 - level) / 8);
}

static void ufo2k_set_gfx_mode(int gfx_driver, int min_color_depth)
{
	assert(min_color_depth % 8 == 0);
	int color_depth = min_color_depth;

	while (true) {

		if (color_depth > 32) {
        //	Still did not manage to set video mode
			fprintf(stderr, "Error: set_gfx_mode() failed (%s).\n", allegro_error);
			exit(1);
		}

		set_color_depth(color_depth);

	//	Try both selected in ini-file and 640x480 video modes
		int exit_code = set_gfx_mode(gfx_driver, cfg_get_screen_x_res(), cfg_get_screen_y_res(), 0, 0);
		if (exit_code == 0) break;
		exit_code = set_gfx_mode(gfx_driver, 640, 480, 0, 0);
		if (exit_code == 0) break;
	//	Try next color depth
		color_depth += 8;
	}

	if (color_depth == 8) {
		xcom1_color = xcom1_color_8bpp;
		xcom1_menu_color = xcom1_menu_color_8bpp;
		xcom1_darken_color = xcom1_darken_color_8bpp;
	} else {
	//	Create tables for translation from xcom1 palette colors to colors
	//	for currently selected video mode
		xcom1_color_table[0] = makecol(255, 0, 255);
		xcom1_menu_color_table[0] = makecol(255, 0, 255);

		for (int c = 1; c < 256; c++)
		{
			const RGB & rgb = ((RGB *)datafile[DAT_GAMEPAL_BMP].dat)[c];
			xcom1_color_table[c] = makecol(rgb.r << 2, rgb.g << 2, rgb.b << 2);
			const RGB & menu_rgb = ((RGB *)datafile[DAT_MENUPAL_BMP].dat)[c];
			xcom1_menu_color_table[c] = makecol(menu_rgb.r << 2, menu_rgb.g << 2, menu_rgb.b << 2);
		}

		xcom1_color = xcom1_color_high_bpp;
		xcom1_menu_color = xcom1_menu_color_high_bpp;
		xcom1_darken_color = xcom1_darken_color_high_bpp;
	}
}

void set_video_mode()
{
	if (FLAGS & F_FULLSCREEN)
		ufo2k_set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, cfg_get_min_color_depth());
	else
		ufo2k_set_gfx_mode(GFX_AUTODETECT_WINDOWED, cfg_get_min_color_depth());

	set_display_switch_mode(SWITCH_AMNESIA);
	set_display_switch_callback(SWITCH_IN, switch_in_callback);
}

void reset_video()
{
	set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);
	position_mouse(160, 100);
	set_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
	set_mouse_sprite(mouser);
	set_mouse_speed(1, 1);
	gui_fg_color = xcom1_color(15);
	gui_bg_color = xcom1_color(1);
	g_switch_in_counter++;
}

void savescreen()
{
	BITMAP *scr = create_bitmap(SCREEN_W, SCREEN_H);
	blit(screen, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	int num = 1;
	while (true) {
		char filename[128];
		sprintf(filename, "snapshot_%d.pcx", num);
		if (!exists(filename)) {
			save_pcx(filename, scr, (RGB *)datafile[DAT_GAMEPAL_BMP].dat);
			destroy_bitmap(scr);
			return;
		}
		num++;
	}
}

#define POLY 0x8408
//												  16	12	5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.

unsigned short crc16(char *data_p)
{
	unsigned char i;
	unsigned int data;
	unsigned int crc = 0xffff;
	unsigned short length = strlen(data_p);

	if (length == 0)
		return (~crc);
	do {
		for (i = 0, data = (unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
		}
	} while (--length);

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | (data >> 8 & 0xff);

	return (crc);
}

#include "icon.h"

void resize_screen2(int vw, int vh)
{
	SCREEN2W += vw; SCREEN2H += vh;
	if (SCREEN2W > SCREEN_W) SCREEN2W = SCREEN_W;
	if (SCREEN2W < 320) SCREEN2W = 320;
	if (SCREEN2H > SCREEN_H - 40) SCREEN2H = SCREEN_H - 40;
	if (SCREEN2H < 200) SCREEN2H = 200;

	destroy_bitmap(screen2);
	screen2 = create_bitmap(SCREEN2W, SCREEN2H);

	map->m_minimap_area->resize(SCREEN_W - SCREEN2W, SCREEN2H);
	g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
	icon->setxy((SCREEN2W - 320) / 2, SCREEN2H - 56);
	set_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
	position_mouse(SCREEN2W / 2, SCREEN2H / 2);
}

int askmenu(char *mess)
{
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);
	int sel = alert(mess, "", "", "OK", "Cancel", 0, 0);
	return (sel == 1);
}

/**
 * Function for background image loading. Can load JPG, 
 * BMP, LBM, SPK, SCR formats
 *
 * @param filename  name of the file with the image to be loaded
 */
BITMAP *load_back_image(const char *filename)
{
	BITMAP *tmp = NULL;
	tmp = load_jpg(F(filename), NULL);
	if (tmp != NULL) return tmp;
	tmp = load_bitmap(F(filename), NULL);
	if (tmp != NULL) return tmp;
	SPK *spk = new SPK(F(filename));
	tmp = create_bitmap(320, 200);
	clear(tmp);
	spk->show(tmp, 0, 0);
	delete spk;
	return tmp;
}
