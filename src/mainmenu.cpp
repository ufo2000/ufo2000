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
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "video.h"
#include "word.h"
#include "../ufo2000.h"
#include "mainmenu.h"
#include "version.h"
#include "pfxopen.h"

int BLOODYMENU = 1;
static int old_mouse_x = -1, old_mouse_y = -1;

static RGB_MAP rgb_table;
static COLOR_MAP light_table;
static PALLETE menupal;
static BITMAP *menuback, *menuscr, *spotlight;
static Word *wdos, *wabout, *wcfg;
static Word *wmodem, *wipx, *wempty, *winet;
static Word *now;


void initmainmenu2()
{
	menuback = LOADBITMAP_ORIG("ufointro/pict2.lbm", menupal);
	menuscr = create_bitmap(320, 200);
}

void initmainmenu()
{
    if (!BLOODYMENU) {
        initmainmenu2();
        return ;
    }
    
    menuback = LOADBITMAP_ORIG("ufointro/pict5.lbm", menupal);
    create_rgb_table(&rgb_table, menupal, NULL);
    rgb_map = &rgb_table;
    create_light_table(&light_table, menupal, 0, 0, 0, NULL);
    
    menuscr = create_bitmap(320, 200);
    spotlight = create_bitmap(128, 128); clear(spotlight);
    for (int i = 0; i < 256; i++)
        circlefill(spotlight, 64, 64, 64 - i / 4, i);
    
    wempty = new Word();
    winet = new Word(BWD_INET);
    wipx = new Word(BWD_IPX);
    wmodem = new Word(BWD_MODEM);
    wdos = new Word(BWD_DOS);
    wabout = new Word(BWD_ABOUT);
    wcfg = new Word(BWD_CFG);
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
        if (state->border_colors[i] > 0)
            rect(bmp, x+i, y+i, x + w - i, y + h - i, state->border_colors[i]);
    }
    
    in_x = x + state->border_width; in_y = y + state->border_width; 
    in_h = h - 2 * state->border_width; in_w = w - 2 * state->border_width; 
    if (state->background_color > 0) {
        rectfill(bmp, in_x, in_y, in_x + in_w, in_y + in_h, state->background_color);
    }
    
    text_x = in_x + in_w/2;
    text_y = in_y + in_h/2 - text_height(state->font)/2;
    
    tmode = text_mode(-1);
    textout_centre(bmp, state->font, text, text_x, text_y, state->font_color);
    text_mode(tmode);
}

static int d_baton_proc(int msg, DIALOG *d, int c) {
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

#define MENU_LEFT           341
#define MENU_TOP            115
#define MENU_BTN_STEP       8
#define MENU_BTN_W          237
#define MENU_BTN_H          32

/** Initializes and runs button-based main menu. It is implemented as an Allegro gui dialog.
 */
int do_mainmenu2()
{

    DIALOG the_dialog[MAINMENU_COUNT + 1];
    
    int i;
    
    for (i=0; i<MAINMENU_COUNT; i++) {
        the_dialog[i].proc = d_baton_proc;
        the_dialog[i].x = MENU_LEFT;
        the_dialog[i].y = MENU_TOP + (MAINMENU_COUNT - i - 1) * ( MENU_BTN_STEP + MENU_BTN_H );
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
    
    the_dialog[MAINMENU_TCPIP].dp       = (void *) "tcp/ip";
    the_dialog[MAINMENU_HOTSEAT].dp     = (void *) "hotseat";
    the_dialog[MAINMENU_EDITOR].dp      = (void *) "editor";
    the_dialog[MAINMENU_LOADGAME].dp    = (void *) "load saved game";
    the_dialog[MAINMENU_ABOUT].dp       = (void *) "about";
    the_dialog[MAINMENU_QUIT].dp        = (void *) "quit";

    
#if defined(HAVE_DPLAY)
    the_dialog[MAINMENU_DPLAY].dp = (void *) "DirectPlay";
#endif
    
	the_dialog[MAINMENU_LOADGAME].proc = NULL;

    BS_DISABLED.font    = large;
    BS_IDLE.font        = large;    
    BS_SELECTED.font    = large;
    BS_GOTFOCUS.font    = large;

	RGB *menupal = (RGB *)datafile[DAT_MENUPAL].dat;
	BITMAP *mouser2 = (BITMAP*)datafile[DAT_MOUSE2].dat;

	set_mouse_sprite(mouser2);
	set_palette(menupal);
	set_mouse_speed(1, 1);
	set_mouse_range(0, 0, 639, 399);
	
	if (old_mouse_x != -1)
		position_mouse(old_mouse_x, old_mouse_y);
	else
		position_mouse(550, 180);

	clear(menuscr);


	blit(menuback, menuscr, 0, 0, 0, 0, menuback->w, menuback->h);
	stretch_blit(menuscr, screen, 0, 0, 320, 200, 0, 0, 640, 400);

	text_mode(-1);

    textprintf(screen, small, 0, 0, 220, "UFO2000 %s", UFO_VERSION_STRING);

	int v = do_dialog(the_dialog, -1);
	
	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;

	fade_out(10);
	clear(screen);
	
	return v;
}


int do_mainmenu()
{
	clear_keybuf();

	if (!BLOODYMENU) {
		return do_mainmenu2();
	}

	//install_keyboard();
	install_int_ex(drawit_timer, BPS_TO_TIMER(20));      //ticks each second

	//set_mouse_range(0, 0, 639, 399);

	if (old_mouse_x != -1)
		position_mouse(old_mouse_x, old_mouse_y);
	else
		position_mouse(160, 100);     //(170,135);

	now = wempty;

	set_palette(menupal);
	//fade_in(menupal, 10);
	//create_light_table(&light_table, menupal, 0, 0, 0, NULL);

	color_map = &light_table;

	int mouse_rightr = 1, SHOWMODE = 1, DONE = 0, mouse_leftr = 1, lpress = 0;
	int rx = 0, ry = 0;
	//int bloodpos=0;
	while (!DONE) {
		if (DRAWIT) {
			if (mouse_inside(144, 43, 190, 90)) {
				//if (mouse_inside(117, 36, 205, 130)) {
				if (now != wipx) {
					now = wipx;
					now->hide();
					//wmodem->dropblood();
				}
			} else if (mouse_inside(210, 83, 270, 144)) {
				//} else if (mouse_inside(207, 80, 273, 144)) {
				if (now != wmodem) {
					now = wmodem;
					now->hide();
					//wipx->dropblood();
				}
			} else if (mouse_inside(147, 111, 190, 175)) {
				if (now != wdos) {
					now = wdos;
					now->hide();
				}
			} else if (mouse_inside(232, 162, 300, 200)) {
				if (now != wabout) {
					now = wabout;
					now->hide();
				}
			} else if (mouse_inside(282, 75, 318, 114)) {
				if (now != wcfg) {
					now = wcfg;
					now->hide();
				}
			} else if (mouse_inside(104, 102, 137, 166)) {
				if (now != winet) {
					now = winet;
					now->hide();
				}

			} else {
				//now = wempty;
			}

			now->move();

			wipx->moveblood();
			wmodem->moveblood();
			wdos->moveblood();
			wabout->moveblood();
			//wempty->moveblood();
			wcfg->moveblood();
			winet->moveblood();
			if (now != wempty)
				//if (now->isunhide())
				if (rand() % 10 == 0) {
					now->addblood();
				}
			DRAWIT = 0;
			rx = rand() % 3 - 1;
			ry = rand() % 3 - 1;
		}

		int x = mouse_x - 64 + rx;     // rand()%3;
		int y = mouse_y - 64 + ry;     // rand()%3;

		clear(menuscr);
		blit(menuback, menuscr, x, y, x, y, 128, 128);
		//blit(back, menuscr, 0, 0, 0, 0, 320, 200);
		now->draw(menuscr, x, y);

		if (SHOWMODE) {
			wipx->drawblood(menuscr, x, y);
			wmodem->drawblood(menuscr, x, y);
			wdos->drawblood(menuscr, x, y);
			wabout->drawblood(menuscr, x, y);
			//wempty->drawblood(menuscr, x, y);
			wcfg->drawblood(menuscr, x, y);
			winet->drawblood(menuscr, x, y);

			if (lpress > 0) {
				lpress++;
				now->unhide();
				//if (lpress%2) now->draw(menuscr, x, y, 0);
				//else now->draw(menuscr, x, y, 255);
				if (lpress % 3 == 0) now->draw(menuscr, x, y, 0);
				else if (lpress % 3 == 1) now->draw(menuscr, x, y, 135);
				else now->draw(menuscr, x, y, 255);
				PALETTE output;
				fade_interpolate(menupal, black_palette, output, lpress * 5, 0, 255);
				set_palette(output);
				//create_light_table(&light_table, output, 0, 0, 0, NULL);
			}
			if (lpress > 10) {
				break;
			}
		}

		if (mouse_b & 1) { // left
			mouse_leftr = 0;
			if (!SHOWMODE)
				now->add(mouse_x, mouse_y);
			if ((now != wempty) && (lpress == 0)) {
				if (mouse_inside(144, 43, 190, 90) ||
				        mouse_inside(210, 83, 270, 144) ||
				        mouse_inside(147, 111, 190, 175) ||
				        mouse_inside(282, 75, 318, 114) ||
				        mouse_inside(232, 162, 300, 200) ||
				        mouse_inside(104, 102, 137, 166)) {
					old_mouse_x = mouse_x;
					old_mouse_y = mouse_y;
					lpress = 1;
				}
			}


			/*			else {
							if (lpress<5) {
								lpress++;
								now->unhide();
								if (lpress%2)//==1)
									now->draw(menuscr, x, y, 0);
								else //if (lpress==2)
									now->draw(menuscr, x, y, 255);
								//else if (lpress==5)
								//	break;
							} else {
								if (now != wempty)
									break;
								//blit(back, menuscr, x, y, x, y, 128, 128);
							}
						}*/
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			//SHOWMODE=1-SHOWMODE;
			if (!SHOWMODE) {
				now->unhide();
			} //else { now->hide(); }
			//now->hide();
		}
		if (!(mouse_b & 1)) {
			mouse_leftr = 1;
			//lpress=0;
		}
		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
		}

		draw_trans_sprite(menuscr, spotlight, x, y);
		if (!SHOWMODE)
			putpixel(menuscr, mouse_x, mouse_y, 255);

		//blit(s, screen, 0, 0, 0, 0, 320, 200);
		stretch_blit(menuscr, screen, 0, 0, 320, 200, 0, 0, 640, 400);
		//stretch_blit(menuback, screen, 0,0, 320, 200, 0,0, 640,400);

		if (keypressed()) {
			//char str[100];
			int c = readkey();

			switch (c >> 8) {
				case KEY_F1:
					now->clean();
					break;
				case KEY_F2:
					now->save();
					//wipx->save("ipx");
					//wmodem->save("modem");
					//wdos->save("dos");
					//wabout->save("about");
					break;
				case KEY_F3:
					now->load();
					//wipx->load("ipx");
					//wmodem->load("modem");
					//wdos->load("dos");
					//wabout->load("about");
					//now->unhide();
					break;
				case KEY_ESC:
					DONE = 1;
					now = wdos;
					break;
				case KEY_SPACE:
					now->addblood();
					//now->addblood(bloodpos);
					//sprintf (str, "%d", bloodpos);
					//textout(screen, font, str, 0, 0, 255);
					//bloodpos++;
					//readkey();
					break;
			}
		}

	}

	remove_int(drawit_timer);
	//remove_keyboard();
	//vsync();

	//fade_out(11);

	clear(screen);
	now->dropblood();


#ifdef WIN32
	if ((now == wipx) || (now == wmodem) || (now == winet))
		return 3;
#endif
	if (now == wdos)
		return 0;
	else if (now == wabout)
		return 1;
	else if (now == wcfg)
		return 2;
	else if (now == wipx)
		return 3;
	else if (now == wmodem)
		return 4;
	else if (now == winet)
		return 5;
	return 6;
}
