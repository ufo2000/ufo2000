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
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "video.h"
#include "dirty.h"
#include "about.h"
#include "pfxopen.h"

char About::aboutstr[100][80] = {
	"UFO 2000",
	"remake",
	"by Sanami",
	"Yakutsk",
	"Russia",
	"Asia",
	"Earth",
	"Sol system",
	"galaxy",
	"universe"
};

About::About()
{
	aboutstr_size = 10;
}


int About::load(char *fname)
{
	FILE * f = FOPEN_OWN(fname, "rt");
	if (f == NULL) return 0;
	aboutstr_size = 0;
	while (fscanf(f, "%[^\n]\n", aboutstr[aboutstr_size]) == 1) {
#ifdef LINUX
		int l = strlen(aboutstr[aboutstr_size]);
		if ((l > 0) && (aboutstr[aboutstr_size][l - 1] == '\r'))
			aboutstr[aboutstr_size][l - 1] = 0;
#endif
		aboutstr_size++;

	}
	fclose(f);
	return 1;
}

void About::transinfo()
{
	install_int_ex(drawit_timer, BPS_TO_TIMER(20));      //ticks each second

	RGB pal[PAL_SIZE];
	BITMAP *pict = load_bitmap(F("$(xcom)/ufointro/pict4.lbm"), pal);
	set_palette(pal);
	BITMAP *back = create_bitmap(SCREEN_W, SCREEN_H);
	stretch_blit(pict, back, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	destroy_bitmap(pict);
	BITMAP *scr = create_bitmap(SCREEN_W, SCREEN_H);

	blit(back, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	blit(back, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	BITMAP *textline = create_bitmap(strlen(aboutstr[0]) * 8, 8);
	clear_to_color(textline, 0);     // 208); //yellow
	text_mode( -1);
	textout(textline, font, aboutstr[0], 0, 0, xcom1_color(80));      //80 green

	BITMAP *roto = create_bitmap(350, 200);
	clear_to_color(roto, 0);     // 208); //yellow
	//rotate_scaled_sprite(roto, textline, 0, 0, itofix(0), ftofix(2.0);
	stretch_sprite(roto, textline, 0, 0, roto->w, roto->h);



	RGB_MAP rgb_table;
	create_rgb_table(&rgb_table, pal, NULL);
	RGB_MAP *rgb_map_old = rgb_map;
	rgb_map = &rgb_table;
	COLOR_MAP trans_table;

	dirty->clear();
	int tx = 20, ty = 50;
	int DONE = 0;
	int t = 0, step = 2;
	int cm = 1;
	int skip = 0;
	int lp = 0, rp = 0;


	while (!DONE) {
		if (CHANGE) {
			if (mouse_b & 1) {
				lp = 1;
			}
			if (mouse_b & 2) {
				rp = 1;
			}
			if (lp) {
				if (!(mouse_b & 1)) {
					//if (mouse_inside(0, 0, 100, 150)) {
					//	break;
					//}
					lp = 0;
					//break;
				}
			}
			if (rp) {
				if (!(mouse_b & 2)) {
					break;
				}
			}
			CHANGE = 0;
			//DRAWIT=1;
		}

		if (DRAWIT) {
			DRAWIT = 0;
			dirty->apply(back, scr);
			old_dirty->copy(dirty);
			dirty->clear();

			if (!skip) t += step;

			if ((t >= 15) || (t == 0)) {
				if (t >= 15) {
					skip++;
					if (skip < 10)
						goto cont;
					//continue;
					skip = 0;
				}

				step = -step;
				if (t == 0) {
					destroy_bitmap(textline);
					textline = create_bitmap(strlen(aboutstr[cm]) * 8, 8);
					clear(textline);
					textout(textline, font, aboutstr[cm], 0, 0, xcom1_color(80));      //80 green
					clear(roto);

					if (strlen(aboutstr[cm]) < 13)
						stretch_sprite(roto, textline, 0, 0, roto->w, roto->h);
					else
						stretch_sprite(roto, textline, 0, roto->h / 4, roto->w, roto->h / 2);

					cm++;
					if (cm >= aboutstr_size) cm = 0;
				}
			}
cont:
			create_trans_table(&trans_table, pal, t * 16, t * 16, t * 16, NULL);
			color_map = &trans_table;
			draw_trans_sprite(scr, roto, tx, ty);
			dirty->add(tx, ty, roto->w, roto->h);

			//sword->move();
			//sword->draw(scr);

			draw_sprite(scr, mouser2, mouse_x * 2, mouse_y * 2);
			dirty->add(mouse_x * 2, mouse_y * 2, mouser2->w, mouser2->h);

			old_dirty->add(dirty);
			old_dirty->sort();

			old_dirty->apply(scr, screen);
		}

		if (keypressed()) {
			int c = readkey();
			switch (c >> 8) {
				case KEY_ESC: DONE = 1; break;
			}
		}
	}

	rgb_map = rgb_map_old;
	destroy_bitmap(back);
	destroy_bitmap(scr);
	destroy_bitmap(textline);
	destroy_bitmap(roto);
	remove_int(drawit_timer);

}

void About::show()
{
	dirty = new DirtyList(1000);
	old_dirty = new DirtyList(1000);
	//sword = new Sword("HellO");

	load("about.txt");

	mouser2 = create_bitmap(mouser->w * 2, mouser->h * 2);
	stretch_blit(mouser, mouser2, 0, 0, mouser->w, mouser->h,
	             0, 0, mouser2->w, mouser2->h);
	position_mouse(160, 100);
	set_mouse_range(0, 0, 319, 199);

	set_mouse_speed(2, 2);
	transinfo();
	set_mouse_speed(1, 1);

	destroy_bitmap(mouser2);
	//delete(sword);
	delete(dirty);
	delete(old_dirty);
	fade_out(10);
	clear(screen);
}
