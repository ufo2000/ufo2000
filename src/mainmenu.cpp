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
#include <stdio.h>
#include <string.h>
#include <time.h>
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

//BITMAP *triton;

/*void cb() {
	clear(menuscr);
	blit(menuback, menuscr, 0, 0, 0, 0, menuback->w, menuback->h);
	stretch_blit(menuscr, screen, 0,0, 320, 200, 0,0, 640,400);
	//readkey();
}*/

void initmainmenu2()
{
	//triton = load_bitmap("triton.pcx", menupal);
	menuback = LOADBITMAP_ORIG("ufointro/pict2.lbm", menupal);
	menuscr = create_bitmap(320, 200);
	//set_display_switch_callback(SWITCH_OUT, cb);
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
	//winet = new Word("inet");
	winet = new Word(BWD_INET);
	//wipx = new Word("ipx");
	wipx = new Word(BWD_IPX);
	//wmodem = new Word("modem");
	wmodem = new Word(BWD_MODEM);
	//wdos = new Word("dos");
	wdos = new Word(BWD_DOS);
	//wabout = new Word("about");
	wabout = new Word(BWD_ABOUT);
	//wcfg = new Word("cfg");
	wcfg = new Word(BWD_CFG);
	/*wipx->savebwd();
	wmodem->savebwd();
	wdos->savebwd();
	wabout->savebwd();
	wcfg->savebwd();
	winet->savebwd();*/
}

static int d_baton_proc(int msg, DIALOG *d, int c)
{
	int state1, state2;
	int g;

	switch (msg) {
		case MSG_DRAW:
			if (d->flags & D_SELECTED) {
				g = 1;
				state1 = d->bg;
				state2 = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
			} else {
				g = 0;
				state1 = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
				state2 = d->bg;
			}

			if ((d->flags & D_GOTFOCUS) &&
			        (!(d->flags & D_SELECTED) || !(d->flags & D_EXIT))) {
				rectfill(screen, d->x + 1 + g, d->y + 1 + g, d->x + d->w - 2 + g, d->y + d->h - 2 + g, 33);
				rect(screen, d->x + g, d->y + g, d->x + d->w - 1 + g, d->y + d->h - 1 + g, state1);
				text_mode( -1);
				gui_textout(screen, (char *)d->dp, d->x + d->w / 2 + g, d->y + d->h / 2 - text_height(font) / 2 + g, state2, TRUE);
			} else {
				rectfill(screen, d->x + 1 + g, d->y + 1 + g, d->x + d->w - 2 + g, d->y + d->h - 2 + g, state2);
				rect(screen, d->x + g, d->y + g, d->x + d->w - 1 + g, d->y + d->h - 1 + g, state1);
				text_mode( -1);
				gui_textout(screen, (char *)d->dp, d->x + d->w / 2 + g, d->y + d->h / 2 - text_height(font) / 2 + g, state1, TRUE);
			}

			if (d->flags & D_SELECTED) {
				vline(screen, d->x, d->y, d->y + d->h - 1, d->bg);
				hline(screen, d->x, d->y, d->x + d->w - 1, d->bg);
			} else {
				vline(screen, d->x + d->w, d->y + 1, d->y + d->h - 1, d->fg);
				hline(screen, d->x + 1, d->y + d->h, d->x + d->w, d->fg);
			}
			break;
		default:
			return d_button_proc(msg, d, c);
	}
	return D_O_K;
}

/*
int d_triton_proc(int msg, DIALOG *d, int c) {
	int state1, state2;
	int g;
 
	switch (msg) {
		case MSG_DRAW:
			  if (d->flags & D_SELECTED) {
				  g = 1;
				  state1 = d->bg;
				  state2 = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
			  } else {
				  g = 0;
				  state1 = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
				  state2 = d->bg;
			  }
 
			  if ((d->flags & D_GOTFOCUS) &&
					(!(d->flags & D_SELECTED) || !(d->flags & D_EXIT)))
			  {
				  //rectfill(screen, d->x+1+g, d->y+1+g, d->x+d->w-2+g, d->y+d->h-2+g, 33);
blit(triton, screen, 0,0, d->x+1+g, d->y+1+g, triton->w, triton->h);
				  rect(screen, d->x+g, d->y+g, d->x+d->w-1+g, d->y+d->h-1+g, state1);
				  //text_mode(-1);
				  //gui_textout(screen, (char *)d->dp, d->x+d->w/2+g, d->y+d->h/2-text_height(font)/2+g, state2, TRUE);
			  } else {
				  rectfill(screen, d->x+1+g, d->y+1+g, d->x+d->w-2+g, d->y+d->h-2+g, state2);
				  rect(screen, d->x+g, d->y+g, d->x+d->w-1+g, d->y+d->h-1+g, state1);
				  text_mode(-1);
				  gui_textout(screen, (char *)d->dp, d->x+d->w/2+g, d->y+d->h/2-text_height(font)/2+g, state1, TRUE);
			  }
 
			  if (d->flags & D_SELECTED) {
				  vline(screen, d->x, d->y, d->y+d->h-1, d->bg);
				  hline(screen, d->x, d->y, d->x+d->w-1, d->bg);
			  } else {
				  vline(screen, d->x+d->w, d->y+1, d->y+d->h-1, d->fg);
				  hline(screen, d->x+1, d->y+d->h, d->x+d->w, d->fg);
			  }
			  break;
		default:
			  return d_button_proc(msg, d, c);
	}
	return D_O_K;
}
*/
#define DX  300 
//#define DY  100
#define FG  0
#define BG  255

#ifdef DJGPP
#include <dpmi.h>
static __dpmi_free_mem_info dpmi_free_mem;
#endif

int do_mainmenu2()
{
	
	static char dstr[7][100];      /////////////////////////////
#ifdef DJGPP
	int DY = 100;
#else
	int DY = 85;
#endif
	//	{ d_triton_proc,  DX+41,  DY+90, 237,  25,  FG,  BG,	 0,	D_EXIT,  0,	 0,	 (void *)dstr[2],	 NULL, NULL},

	DIALOG the_dialog[] = {
	//    (dialog proc) (x)      (y)       (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3)
		{ d_baton_proc, DX + 41, DY + 180, 237, 25, FG,  BG,  27,   D_EXIT, 0,   0,   (void *)dstr[0], NULL, NULL},
		{ d_baton_proc, DX + 41, DY + 150, 237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[1], NULL, NULL},
		{ d_baton_proc, DX + 41, DY + 120, 237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[2], NULL, NULL},
	    { d_baton_proc, DX + 41, DY + 90,  237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[3], NULL, NULL},
	    { d_baton_proc, DX + 41, DY + 60,  237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[4], NULL, NULL},
	    { d_baton_proc, DX + 41, DY + 30,  237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[5], NULL, NULL},
	    { d_baton_proc, DX + 41, DY,       237, 25, FG,  BG,  0,    D_EXIT, 0,   0,   (void *)dstr[6], NULL, NULL},
	    { NULL,         0,       0,        0,   0,  0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL}
	};
	
	

#ifdef DJGPP
	sprintf(dstr[6], "play by modem");
	sprintf(dstr[5], "play by ipx");
	sprintf(dstr[4], "play by tcp/ip");
	sprintf(dstr[3], "play by hotseat");
	sprintf(dstr[2], "editor");
	sprintf(dstr[1], "about info");
	sprintf(dstr[0], "quit to os");
#elif WIN32
	sprintf(dstr[MAINMENU_LOADGAME], "load saved game");
#    if defined(HAVE_DPLAY)
	sprintf(dstr[MAINMENU_DPLAY],    "DirectPlay");
#    else
	the_dialog[MAINMENU_DPLAY].proc = NULL;
#    endif
	sprintf(dstr[MAINMENU_TCPIP],    "tcp/ip");
	sprintf(dstr[MAINMENU_HOTSEAT],  "hotseat");
	sprintf(dstr[MAINMENU_EDITOR],   "editor");
	sprintf(dstr[MAINMENU_ABOUT],    "about");
	sprintf(dstr[MAINMENU_QUIT],     "quit");
#else
	the_dialog[MAINMENU_LOADGAME].proc = NULL;
	sprintf(dstr[MAINMENU_LOADGAME], "load saved game");
	sprintf(dstr[MAINMENU_TCPIP],    "tcp/ip"); 
	sprintf(dstr[MAINMENU_HOTSEAT],  "hotseat"); 
	sprintf(dstr[MAINMENU_EDITOR],   "editor"); 
	sprintf(dstr[MAINMENU_ABOUT],    "about"); 
	sprintf(dstr[MAINMENU_QUIT],     "quit"); 
#endif
	//BITMAP *mouser2 = load_bitmap("mouse2.bmp",menupal);
	RGB *menupal = (RGB *)datafile[DAT_MENUPAL].dat;
	BITMAP *mouser2 = (BITMAP*)datafile[DAT_MOUSE2].dat;
//	DATAFILE *dfp = find_datafile_object(datafile, "DAT_MENUPAL");
//	RGB *menupal = (RGB *)(dfp->dat);
//	dfp = find_datafile_object(datafile, "DAT_MOUSE2");
//	BITMAP *mouser2 = (BITMAP*)(dfp->dat);

	set_mouse_sprite(mouser2);
	set_palette(menupal);
	set_mouse_speed(1, 1);
	set_mouse_range(0, 0, 639, 399);
	
	if (old_mouse_x != -1)
		position_mouse(old_mouse_x, old_mouse_y);
	else
		position_mouse(550, 180);
	//position_mouse(320, 200);

	clear(menuscr);
	assert(menuback != NULL);
	assert(menuscr != NULL);
	blit(menuback, menuscr, 0, 0, 0, 0, menuback->w, menuback->h);
	stretch_blit(menuscr, screen, 0, 0, 320, 200, 0, 0, 640, 400);

	//gui_fg_color = 0;
	//gui_bg_color = 255;

#ifdef DJGPP
	__dpmi_get_free_memory_information(&dpmi_free_mem);
	//textprintf(BITMAP *bmp, FONT *f, int x, y, color, char *fmt, ...);
	text_mode( -1);
	textprintf(screen, font, 0, 0, 1, "heap: %ld", dpmi_free_mem.largest_available_free_block_in_bytes);
#endif
#ifdef WIN32
	text_mode( -1);
	textprintf(screen, font, 0, 0, 1, "UFO2000 %s", UFO_VERSION_STRING);
#endif

	int v = do_dialog(the_dialog, -1);
	
	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;

	fade_out(10);
	clear(screen);
	
	//set_mouse_sprite(NULL);
	//destroy_bitmap(mouser2);
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
