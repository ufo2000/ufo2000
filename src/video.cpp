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

#include "stdafx.h"

#include "global.h"
#include "video.h"
#include "wind.h"
#include "../ufo2000.h"
#include "global.h"
#include "map.h"
#include "config.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"

#ifdef HAVE_FREETYPE
extern "C" {
#include "jinete/ji_font.h"
}
#endif

unsigned long FLAGS = 0;

DATAFILE *datafile;

BITMAP *mouser;                   //!< Mouse-pointer
BITMAP *selector, *selector2;     //!< Arrow above active friendly unit: standing, kneeling
BITMAP *selector3, *selector4;    //!< Arrow above active friendly unit: with items on the ground
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
 * task in win32. All the information drawn on screen is lost, 
 * so we need some way to detect this situation.
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

    mouser    = (BITMAP*)datafile[DAT_MOUSE_BMP].dat;
    selector  = (BITMAP*)datafile[DAT_SELECT_BMP].dat;
    selector2 = (BITMAP*)datafile[DAT_SELECT2_BMP].dat;
    selector3 = (BITMAP*)datafile[DAT_SELECT3_BMP].dat;
    selector4 = (BITMAP*)datafile[DAT_SELECT4_BMP].dat;

    screen2 = create_bitmap(SCREEN2W, SCREEN2H);
    clear(screen2);
#ifdef HAVE_FREETYPE
    ji_font_init();
#endif	
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
#ifdef HAVE_FREETYPE	
	ji_font_exit();
#endif
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

/**
 * Table for translation from TFTD to XCOM color palette 
 * taken from XCOMUTIL
 */
static int tftd_color_table[256] = {
    0x00, 0x01, 0x51, 0xE2, 0xE6, 0x75, 0x75, 0x8B, 
    0x00, 0x25, 0x26, 0x27, 0x29, 0x2B, 0x27, 0x00, 
    0x10, 0x10, 0x10, 0x11, 0x12, 0x13, 0x13, 0x96, 
    0x97, 0xA5, 0xA6, 0xA7, 0x4A, 0x4C, 0x4E, 0x4F, 
    0xB0, 0xB0, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
    0x57, 0x09, 0x0A, 0x5B, 0x0B, 0x0C, 0x0D, 0x0E, 
    0x01, 0x02, 0x03, 0x52, 0x04, 0x05, 0x06, 0xF1, 
    0xF2, 0xF4, 0x09, 0x0A, 0x0B, 0x0C, 0xFB, 0xFD, 
    0xA3, 0xA3, 0xA4, 0xA4, 0xA5, 0xA6, 0xA6, 0xA7, 
    0x5D, 0x5D, 0x5E, 0x5F, 0x5F, 0x0E, 0x0E, 0xAF, 
    0x40, 0x41, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 
    0x46, 0x47, 0x48, 0x49, 0x4A, 0x4C, 0x3F, 0x0E, 
    0xD0, 0xD1, 0x33, 0x33, 0x34, 0x34, 0x35, 0x36, 
    0x38, 0x38, 0x39, 0x3B, 0x4B, 0x4B, 0x4C, 0xAD, 
    0xAD, 0x4E, 0x4F, 0x4F, 0x4F, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0x15, 
    0x97, 0x47, 0x47, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 
    0x90, 0x91, 0x92, 0x93, 0x13, 0x96, 0x45, 0x47, 
    0x48, 0x4A, 0x13, 0x17, 0x9B, 0xC3, 0xC6, 0xEC, 
    0x11, 0x12, 0x12, 0x13, 0x14, 0x14, 0x15, 0x66, 
    0x67, 0x99, 0xA7, 0xA8, 0xA8, 0xA9, 0x5F, 0x4E, 
    0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 
    0x19, 0x1A, 0x9B, 0x9C, 0xAA, 0xAA, 0xAB, 0xAC, 
    0xE0, 0x03, 0x04, 0xE4, 0xF1, 0xF3, 0xF6, 0xF8, 
    0xF8, 0xF9, 0xFA, 0xFB, 0x0D, 0xFD, 0xFD, 0x0E, 
    0xB3, 0xB4, 0xB4, 0xB5, 0xA1, 0xA2, 0xA3, 0x59, 
    0x5A, 0x5B, 0x5B, 0x5C, 0x5D, 0x5E, 0x5E, 0x5F, 
    0xB2, 0xB3, 0xB3, 0xB4, 0xB5, 0xB5, 0xB7, 0xB7, 
    0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x0C, 0xEF, 0xFC, 
    0xD0, 0xD0, 0xD1, 0xD1, 0xD2, 0xD3, 0xD4, 0xD4, 
    0x87, 0x89, 0x3A, 0x3B, 0x3C, 0x3D, 0xFE, 0xFF
};

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
    ASSERT(c >= 0 && c < 256);
    return xcom1_color_table[c]; 
}
static int xcom1_menu_color_high_bpp(int c)
{ 
    ASSERT(c >= 0 && c < 256);
    return xcom1_menu_color_table[c]; 
}
static int xcom1_darken_color_high_bpp(int c, int level)
{ 
    return makecol(
        getr(c) * (8 - level) / 8, 
        getg(c) * (8 - level) / 8, 
        getb(c) * (8 - level) / 8);
}
int tftd_color(int c)
{
    ASSERT(c >= 0 && c < 256);
    return tftd_color_table[c];
}

static void ufo2k_set_gfx_mode(int gfx_driver, int min_color_depth)
{
    ASSERT(min_color_depth % 8 == 0);
    int color_depth = min_color_depth;

    while (true) {

        if (color_depth > 32) {
        //  Still did not manage to set video mode
            fprintf(stderr, "Error: set_gfx_mode() failed (%s).\n", allegro_error);
            exit(1);
        }

        set_color_depth(color_depth);

    //  Try both the video modes selected in the ini-file and 640x480.
        int exit_code = set_gfx_mode(gfx_driver, cfg_get_screen_x_res(), cfg_get_screen_y_res(), 0, 0);
        if (exit_code == 0) break;
        exit_code = set_gfx_mode(gfx_driver, 640, 480, 0, 0);
        if (exit_code == 0) break;
    //  Try next color depth
        color_depth += 8;
    }

    if (color_depth == 8) {
        xcom1_color        = xcom1_color_8bpp;
        xcom1_menu_color   = xcom1_menu_color_8bpp;
        xcom1_darken_color = xcom1_darken_color_8bpp;
    } else {
    //  Create tables for translation from xcom1 palette colors
    //  to colors for currently selected video mode
        xcom1_color_table[0]      = makecol(255, 0, 255);
        xcom1_menu_color_table[0] = makecol(255, 0, 255);
        tftd_color_table[0]       = makecol(255, 0, 255);

        for (int c = 1; c < 256; c++)
        {
            const RGB & rgb = ((RGB *)datafile[DAT_GAMEPAL_BMP].dat)[c];
            xcom1_color_table[c] = makecol(rgb.r << 2, rgb.g << 2, rgb.b << 2);
            const RGB & menu_rgb = ((RGB *)datafile[DAT_MENUPAL_BMP].dat)[c];
            xcom1_menu_color_table[c] = makecol(menu_rgb.r << 2, menu_rgb.g << 2, menu_rgb.b << 2);
            const RGB & tftd_rgb = ((RGB *)datafile[DAT_TFTDPAL_BMP].dat)[c];
            tftd_color_table[c] = makecol(tftd_rgb.r << 2, tftd_rgb.g << 2, tftd_rgb.b << 2);
        }

        xcom1_color        = xcom1_color_high_bpp;
        xcom1_menu_color   = xcom1_menu_color_high_bpp;
        xcom1_darken_color = xcom1_darken_color_high_bpp;
    }
}

static void normalize_screen2_size()
{
    if (SCREEN2W > SCREEN_W) SCREEN2W = SCREEN_W;
    if (SCREEN2H > SCREEN_H - 20) SCREEN2H = SCREEN_H - 20;

    if (SCREEN2W < 320) SCREEN2W = 320;
    if (SCREEN2H < 330) SCREEN2H = 330;
}

void set_video_mode()
{
    if (FLAGS & F_FULLSCREEN)
        ufo2k_set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, cfg_get_min_color_depth());
    else
        ufo2k_set_gfx_mode(GFX_AUTODETECT_WINDOWED, cfg_get_min_color_depth());

    normalize_screen2_size();

    if (set_display_switch_mode(SWITCH_BACKGROUND) == -1) set_display_switch_mode(SWITCH_BACKAMNESIA);
    set_display_switch_callback(SWITCH_IN, switch_in_callback);
}

void reset_video()
{
    set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);
    position_mouse(160, 100);
    set_mouse_sprite(mouser);
    set_mouse_speed(1, 1);
    reset_mouse_range();
    gui_fg_color = COLOR_BLACK1;
    gui_bg_color = COLOR_WHITE;
    g_switch_in_counter++;
}

/**
 * Save screenshot to file
 */
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
            // Todo: test if save was successful
            destroy_bitmap(scr);
            g_console->printf(COLOR_SYS_OK, _("Screenshot saved as %s"), filename);
            return;
        }
        num++;
    }
}

#define POLY 0x8408
/**
 * Calculate the CCITT CRC 16 polynomial
 */
//                                                16    12  5
// This is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  
// The high bit is always assumed to be set, thus we 
// only use 16 bits to represent the 17 bit value.
uint16 crc16(const char *data_p)
{
    unsigned char i;
    unsigned int data;
    unsigned int crc = 0xffff;
    long length = strlen(data_p);

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

    normalize_screen2_size();

    destroy_bitmap(screen2);
    screen2 = create_bitmap(SCREEN2W, SCREEN2H);

    map->m_minimap_area->resize(SCREEN_W - SCREEN2W, SCREEN2H);
    g_console->resize(SCREEN_W, SCREEN_H - SCREEN2H);
    icon->setxy();
    position_mouse(SCREEN2W / 2, SCREEN2H / 2);
    reset_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
}

/**
 * Yes/No style messagebox
 * @param mess  question that is asked
 */
int askmenu(const char *mess)
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    int sel = alert(mess, "", "", _("OK"), _("Cancel"), 0, 0);
    return (sel == 1);
}

/**
 * Function for background image loading. 
 * Can load JPG, BMP, LBM, SPK, SCR formats
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

const static std::vector<std::string> *current_list;

static const char *gui_select_from_list_proc(int index, int *list_size)
{
    if (index < 0) {
        *list_size = current_list->size();
        return NULL;
    }
    ASSERT(index < (int)current_list->size());
    return (*current_list)[index].c_str();
}

/**
 * High level function that shows GUI dialog with a choice from a list of variants
 * @param width          width of dialog in pixels
 * @param height         height of dialog in pixels
 * @param title          dialog title message
 * @param data           std::vector with a number of variants to be suggested to user
 * @param default_value  value that is active at start of dialog
 * @returns              index of user's choice
 */
int gui_select_from_list(
    int width, int height,
    const std::string &title, 
    const std::vector<std::string> &data,
    int default_value)
{
    current_list = &data;
    
    DIALOG list_dialog[] = {
        //(dialog proc)      (x)           (y)                   (w)      (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
        { d_agup_shadow_box_proc, 0, 0, width, height, -1,  -1, 0, 0, 0, 0, NULL, NULL, NULL},
        { d_agup_ctext_proc,      0, 8, width, 16,  -1, -1, 0, 0, 0, 0, (void *)title.c_str(), NULL, NULL},
        { d_agup_list_proc,       0 + 8, 0 + 16 + 8, width - 16, height - 16 - 16,  -1,  -1, 0, D_EXIT, 0, 0, (void *)gui_select_from_list_proc, NULL, NULL},
        { d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
        { NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
    };

    while (mouse_b & 3) yield_timeslice();

    list_dialog[2].d1 = default_value;
    set_dialog_color(list_dialog, gui_fg_color, gui_bg_color);
    centre_dialog(list_dialog);
    popup_dialog(list_dialog, 2);
    current_list = NULL;
    return list_dialog[2].d1;
}

static char buffer[512];

static int d_custom_list_proc(int msg, DIALOG *d, int c)
{
    int result = d_agup_list_proc(msg, d, c);
    if (d->flags & D_GOTFOCUS) {
        strcpy(buffer, (*current_list)[d->d1].c_str());
        if (result == D_O_K) result = D_REDRAW;
    }
    return result;
}

/**
 * High level function that shows GUI dialog with a choice from a list of variants
 * @param width          width of dialog in pixels
 * @param height         height of dialog in pixels
 * @param title          dialog title message
 * @param data           std::vector with a number of variants to be suggested to user
 * @param default_value  value that is active at start of dialog
 * @returns              user's choice
 */
std::string gui_select_from_list_ex(
    int width, int height,
    const std::string &title, 
    const std::vector<std::string> &data,
    const std::string &default_value)
{
    current_list = &data;
    strcpy(buffer, default_value.c_str());
    
    DIALOG list_dialog[] = {
        //(dialog proc)      (x)           (y)                   (w)      (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
        { d_agup_shadow_box_proc, 0, 0, width, height, -1,  -1, 0, 0, 0, 0, NULL, NULL, NULL},
        { d_agup_ctext_proc,      0, 8, width, 16,  -1, -1, 0, 0, 0, 0, (void *)title.c_str(), NULL, NULL},
        { d_agup_edit_proc,       0 + 8, 8 + 16, width - 16, 16,  -1, -1, 0, D_EXIT, 48, 0, &buffer, NULL, NULL},
        { d_custom_list_proc,     0 + 8, 0 + 16 + 16 + 8, width - 16, height - 16 - 16 - 16,  -1,  -1, 0, D_EXIT, 0, 0, (void *)gui_select_from_list_proc, NULL, &list_dialog[2]},
        { d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
        { NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
    };

    while (mouse_b & 3) yield_timeslice();

    list_dialog[3].d1 = 0;
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i] == default_value) {
            list_dialog[3].d1 = i;
            break;
        }
    }
    set_dialog_color(list_dialog, gui_fg_color, gui_bg_color);
    centre_dialog(list_dialog);
    popup_dialog(list_dialog, 2);
    current_list = NULL;
    return buffer;
}

/**
 * Select existing files with some specific extension which
 * all reside in a single directory
 * @param width            width of dialog in pixels
 * @param height           height of dialog in pixels
 * @param title            dialog title message
 * @param dir              directory name
 * @param ext              file extension name
 * @param edit_box_enabled if set, enables edit box for entering file name
 * @returns                name of the selected file
 */
std::string gui_file_select(
    int width, int height,
    const std::string &title, 
    const std::string &dir,
    const std::string &ext,
    bool edit_box_enabled)
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    std::vector<std::string> data;
    al_ffblk info;
    std::string pattern = dir + "/*." + ext;
    if (al_findfirst(pattern.c_str(), &info, FA_RDONLY | FA_ARCH) == 0) {
        do {
            data.push_back(std::string(info.name, info.name + strlen(info.name) - ext.size() - 1));
        } while(al_findnext(&info) == 0);
        al_findclose(&info);
    }
    if (data.empty()) return "";
    std::sort(data.begin(), data.end());
    if (edit_box_enabled) {
        std::string result = gui_select_from_list_ex(width, height, title, data, "");
        return dir + "/" + result + "." + ext;
    } else {
        int result = gui_select_from_list(width, height, title, data, 0);
        return dir + "/" + data[result] + "." + ext;
    }
}
