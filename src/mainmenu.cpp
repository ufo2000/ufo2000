/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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
#include "../ufo2000.h"
#include "mainmenu.h"
#include "version.h"
#include "config.h"

#include "sound.h"
#include "music.h"
#include "text.h"

static int old_mouse_x = -1, old_mouse_y = -1;

static BITMAP *menuback;

/** Event handler for main menu buttons. */
/*

Button states:
1. Disabled
2. Enabled, idle (mouse out)
3. Enabled, selected (mouse in)
4. Clicked (mouse in && LMB down)

Button state description:

- box border width (up to 5)
- box border color set (color of each line, inward) (-1 if transparent)
- box background color (-1 if transparent)
- font color set
- font

*/
struct buttonState {
    int border_width;
    int border_colors[5];
    int background_color;
    int font_color;
    FONT *font;
};

buttonState BS_DISABLED = { 5, { 183, 190, 197, 204, 211 }, 195,  85, NULL };
buttonState BS_IDLE     = { 5, { 183, 190, 197, 204, 211 }, 187, 220, NULL };
buttonState BS_GOTFOCUS = { 5, { 183, 190, 197, 204, 211 }, 190, 220, NULL };
buttonState BS_SELECTED = { 5, { 183, 190, 197, 204, 211 }, 195, 220, NULL };

static void d_draw_baton(BITMAP *bmp, int x, int y, int w, int h, buttonState *state, char *text) {
    int i, in_x, in_y, in_w, in_h, tmode, text_x, text_y;
    
    for (i=0; i< state->border_width; i++) {
        if (xcom1_menu_color(state->border_colors[i]) != xcom1_menu_color(0))
            rect(bmp, x+i, y+i, x + w - i, y + h - i, xcom1_menu_color(state->border_colors[i]));
    }
    
    in_x = x + state->border_width; in_y = y + state->border_width; 
    in_h = h - 2 * state->border_width; in_w = w - 2 * state->border_width; 
    if (xcom1_menu_color(state->background_color) != xcom1_menu_color(0)) {
        rectfill(bmp, in_x, in_y, in_x + in_w, in_y + in_h, xcom1_menu_color(state->background_color));
    }
    
    text_x = in_x + in_w/2;
    text_y = in_y + in_h/2 - text_height(state->font)/2;
    
    tmode = text_mode(-1);
    textout_centre(bmp, state->font, text, text_x, text_y, xcom1_menu_color(state->font_color));
    text_mode(tmode);
}

/**
 * Draw background-image for mainmenu,
 * plus current version-number of ufo2000
 */
static int d_mainmenu_background_proc(int msg, DIALOG *d, int c) 
{
	if (msg == MSG_DRAW) {
		stretch_blit(menuback, screen, 0, 0, menuback->w, menuback->h, 0, 0, SCREEN_W, SCREEN_H);
		text_mode(-1);
		if (strcmp(UFO_SVNVERSION, "unknown") == 0 || strcmp(UFO_SVNVERSION, "exported") == 0 || strcmp(UFO_SVNVERSION, "") == 0) {
			textprintf(screen, g_small_font, 0, 0, xcom1_menu_color(220), 
				"UFO2000 %s (revision >=%d)", UFO_VERSION_STRING, UFO_REVISION_NUMBER);
		} else {
			textprintf(screen, g_small_font, 0, 0, xcom1_menu_color(220), 
				"UFO2000 %s.%s", UFO_VERSION_STRING, UFO_SVNVERSION);
		}
	}
	return D_O_K;
}

static int d_mainmenu_button_proc(int msg, DIALOG *d, int c) 
{
    
	switch (msg) {
        case MSG_GOTMOUSE:
            d->dp2 = &BS_GOTFOCUS;
            return D_REDRAWME;
        
        case MSG_LOSTMOUSE:
            d->dp2 = &BS_IDLE;
            d->d2 = 0;
            return D_REDRAWME;
        
		case MSG_DRAW:
            d_draw_baton(screen, d->x, d->y, d->w, d->h, (buttonState *)d->dp2, (char *)d->dp);
            break;
        
        case MSG_LPRESS:
        case MSG_MPRESS:
        case MSG_RPRESS:
            d->dp2 = &BS_SELECTED;
            d->d2 = 1;
            return D_REDRAWME;
        
        case MSG_KEY:     // for processing ESC
            return D_CLOSE;

        case MSG_LRELEASE: 
        case MSG_MRELEASE: 
        case MSG_RRELEASE:
            if (d->d2) 
                return D_CLOSE;
            else
                break;
        
        case MSG_IDLE:
            usleep(5000);
            break;
            
		default:
            break;
	}
    
	return D_O_K;
}

//#define MENU_LEFT           (SCREEN_W - 640 + 341)
//#define MENU_TOP            115
#define MENU_LEFT           (SCREEN_W - 640 + 358)
#define MENU_TOP            55 
#define MENU_BTN_STEP       8
#define MENU_BTN_W          237
#define MENU_BTN_H          32


extern MIDI *g_menu_midi_music;


/** Initializes and runs button-based main menu. 
 * It is implemented as an Allegro gui dialog.
 */
int do_mainmenu()
{
    lua_message( "Enter: do_mainmenu" );
	clear_keybuf();

	// static added to workaround a weird crash bug on exit
	// (looks like after exit from do_mainmenu() function, Allegro 
	// still tries to use the_dialog data on already invalid stack)
    static DIALOG the_dialog[MAINMENU_TOTAL_COUNT + 1];
    memset(the_dialog, 0, sizeof(the_dialog));
    
    int i;
    
    for (i = 0; i < MAINMENU_COUNT; i++) {
        the_dialog[i].proc = d_mainmenu_button_proc;
        the_dialog[i].x = MENU_LEFT;
        the_dialog[i].y = MENU_TOP + (MAINMENU_COUNT - i - 1) * (MENU_BTN_STEP + MENU_BTN_H);
        the_dialog[i].w = MENU_BTN_W;
        the_dialog[i].h = MENU_BTN_H;
        the_dialog[i].fg    =  0;	// COLOR_WHITE
        the_dialog[i].bg    = 255;	// COLOR_BLACK2
        the_dialog[i].flags = D_EXIT;
        the_dialog[i].key = 0;
        the_dialog[i].d1  = 0;
        the_dialog[i].d2  = 0;
        the_dialog[i].dp  = NULL;
        the_dialog[i].dp2 = &BS_IDLE;
        the_dialog[i].dp3 = NULL;
    }
    
    the_dialog[MAINMENU_QUIT].key   = 27;  // ESC
    the_dialog[MAINMENU_YIELD].proc = d_yield_proc;
    the_dialog[MAINMENU_COUNT].proc = NULL;
    
    the_dialog[MAINMENU_BACKGROUND].proc = d_mainmenu_background_proc;

    the_dialog[MAINMENU_INTERNET].dp    = (void *) _("connect to internet server");
    the_dialog[MAINMENU_HOTSEAT].dp     = (void *) _("start hotseat game");
    the_dialog[MAINMENU_EDITOR].dp      = (void *) _("editor");
    the_dialog[MAINMENU_LOADGAME].dp    = (void *) _("load saved game");
    the_dialog[MAINMENU_ABOUT].dp       = (void *) _("about");
    the_dialog[MAINMENU_TIP_OF_DAY].dp  = (void *) _("tip of the day");
  //the_dialog[MAINMENU_DEMO].dp        = (void *) _("demo");
    the_dialog[MAINMENU_QUIT].dp        = (void *) _("quit");
    
    BS_DISABLED.font    = large;
    BS_IDLE.font        = large;    
    BS_SELECTED.font    = large;
    BS_GOTFOCUS.font    = large;

	BITMAP *mouser2 = (BITMAP*)datafile[DAT_MOUSE2_BMP].dat;

	set_mouse_sprite(mouser2);
	set_palette((RGB *)datafile[DAT_MENUPAL_BMP].dat);
	set_mouse_speed(1, 1);
	set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);

	menuback = load_back_image(cfg_get_menu_image_file_name());
	
	if (old_mouse_x != -1)
		position_mouse(old_mouse_x, old_mouse_y);
    else  // initial mouse-position on button #2 "Hotseat/Mission-planner":
        position_mouse(550, MENU_TOP + 2 * MENU_BTN_H - 10);

	soundSystem::getInstance()->play(SS_WINDOW_OPEN_1);
	// Todo: wait with start of music until sound is finished ?
	FS_MusicPlay(F(cfg_get_menu_music_file_name()));

	int v = do_dialog(the_dialog, -1);

	destroy_bitmap(menuback);

	FS_MusicPlay(NULL);
	soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
	
	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;
	show_mouse(NULL);

	fade_out(10);
	clear(screen);
	
	return v;
}

