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
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "video.h"
//#include "word.h"
#include "../ufo2000.h"
#include "mainmenu.h"
#include "version.h"
#include "pfxopen.h"

#include "sound.h"

static int old_mouse_x = -1, old_mouse_y = -1;

static PALLETE menupal;
static BITMAP *menuback;


void initmainmenu()
{
	menuback = LOADBITMAP_ORIG("ufointro/pict2.lbm", menupal);
}

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

buttonState BS_DISABLED = { 5, { 183, 190, 197, 204, 211 }, 195, 85, NULL };
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

static int d_mainmenu_background_proc(int msg, DIALOG *d, int c) 
{
	if (msg == MSG_DRAW) {
		stretch_blit(menuback, screen, 0, 0, menuback->w, menuback->h, 0, 0, screen->w, screen->h);
		text_mode(-1);
		textprintf(screen, g_small_font, 0, 0, xcom1_menu_color(220), "UFO2000 %s", UFO_VERSION_STRING);
	}
	return D_O_K;
}

static int d_mainmenu_button_proc(int msg, DIALOG *d, int c) 
{
    buttonState *the_state = &BS_IDLE;
    
	switch (msg) {
		case MSG_DRAW:
			if (d->flags & D_DISABLED) {
                the_state = &BS_DISABLED;
                break;
			}
            if (d->flags & D_SELECTED) {
                the_state = &BS_SELECTED;
                break;
            }
            if ((d->flags & D_GOTFOCUS) || (d->flags & D_GOTMOUSE) ) {
                the_state = &BS_GOTFOCUS;
                break;
            }
			break;
		default:
            return d_button_proc(msg, d, c);
	}
    d_draw_baton(screen, d->x, d->y, d->w, d->h, the_state, (char *)d->dp);
	return D_O_K;
}

#define MENU_LEFT           (SCREEN_W - 640 + 341)
#define MENU_TOP            115
#define MENU_BTN_STEP       8
#define MENU_BTN_W          237
#define MENU_BTN_H          32

extern MIDI *g_menu_midi_music;

/** Initializes and runs button-based main menu. It is implemented as an Allegro gui dialog.
 */
int do_mainmenu()
{
	clear_keybuf();

    DIALOG the_dialog[MAINMENU_COUNT + 1];
    
    int i;
    
    for (i = 0; i < MAINMENU_COUNT; i++) {
        the_dialog[i].proc = d_mainmenu_button_proc;
        the_dialog[i].x = MENU_LEFT;
        the_dialog[i].y = MENU_TOP + (MAINMENU_COUNT - i - 1) * (MENU_BTN_STEP + MENU_BTN_H);
        the_dialog[i].w = MENU_BTN_W;
        the_dialog[i].h = MENU_BTN_H;
        the_dialog[i].fg = 0;
        the_dialog[i].bg = 255;
        the_dialog[i].flags = D_EXIT;
        the_dialog[i].key = 0;
        the_dialog[i].d1 = 0;
        the_dialog[i].d2 = 0;
        the_dialog[i].dp = NULL;
        the_dialog[i].dp2 = NULL;
        the_dialog[i].dp3 = NULL;
    }
    
    the_dialog[MAINMENU_QUIT].key = 27;
    the_dialog[MAINMENU_COUNT].proc = NULL;
    
    the_dialog[MAINMENU_BACKGROUND].proc = d_mainmenu_background_proc;

    the_dialog[MAINMENU_INTERNET].dp    = (void *) "connect to internet server";
    the_dialog[MAINMENU_TCPIP].dp       = (void *) "tcp/ip";
    the_dialog[MAINMENU_HOTSEAT].dp     = (void *) "start hotseat game";
    the_dialog[MAINMENU_EDITOR].dp      = (void *) "editor";
    the_dialog[MAINMENU_LOADGAME].dp    = (void *) "load saved game";
    the_dialog[MAINMENU_ABOUT].dp       = (void *) "about";
    the_dialog[MAINMENU_QUIT].dp        = (void *) "quit";
    
    BS_DISABLED.font    = large;
    BS_IDLE.font        = large;    
    BS_SELECTED.font    = large;
    BS_GOTFOCUS.font    = large;

	RGB *menupal = (RGB *)datafile[DAT_MENUPAL_BMP].dat;
	BITMAP *mouser2 = (BITMAP*)datafile[DAT_MOUSE2_BMP].dat;

	set_mouse_sprite(mouser2);
	set_palette(menupal);
	set_mouse_speed(1, 1);
	set_mouse_range(0, 0, screen->w - 1, screen->h - 1);
	
	if (old_mouse_x != -1)
		position_mouse(old_mouse_x, old_mouse_y);
	else
		position_mouse(550, 180);

	play_midi(g_menu_midi_music, 1);
	soundSystem::getInstance()->play(SS_WINDOW_OPEN_1);

	int v = do_dialog(the_dialog, -1);

	play_midi(NULL, 1);
	soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
	
	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;

	fade_out(10);
	clear(screen);
	
	return v;
}
