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
#include <string.h>
#include <allegro.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "word.h"
#include "pfxopen.h"

/*
extern DATAFILE *datafile;
*/

Blood::Blood(int pos, int dire)
{
	poscur = pos;
	posold = pos;
	dir = dire;
	/*if (rand()%2) ////////
		dir = 1;
	else
		dir = -1;*/


	x = -1; y = -1;      //prevent 0,0 falling shit
	prev = NULL; next = NULL;
	state = FLOW;
}

Blood::~Blood()
{}

int Blood::isfall()
{
	if (state == FALL)
		return 1;
	return 0;
}

void Blood::setdrop()
{
	state = DROP;
}

void Blood::move(int *wx, int *wy, int sz)
{
	int ox, oy;
	int fx, fy;
	switch (state) {
		case FLOW:
aga:
			posold = poscur;
			poscur += dir;
			if (poscur >= sz) {
				poscur = sz - 1;
				state = DROP;
				return ;
			}
			if (poscur < 0) {
				poscur = 0;
				state = DROP;
				return ;
			}

			x = wx[poscur];
			y = wy[poscur];
			ox = wx[posold];
			oy = wy[posold];

			if ((y < oy) || (y > oy + 1)) {
				x = ox; y = oy;
				state = DROP;
			} else if (y == oy) {
				if ((poscur < sz - 1) && (poscur > 1)) {
					fx = wx[poscur + dir];
					fy = wy[poscur + dir];
					if (fy > y)
						goto aga;
				}
			} else if (rand() % 10 == 0) state = DROP;

			break;
		case DROP:
			y++;
			if (y >= 200)
				state = FALL;
			break;
		case FALL:
			break;
	}
}

void Blood::draw(BITMAP *scr, int sx, int sy)
{
	switch (state) {
		case FLOW:
			if ((x > sx) && (x < sx + 127) && (y > sy) && (y < sy + 127))
				circle(scr, x, y, 1, 135);
		case DROP:
			if ((x > sx) && (x < sx + 127) && (y > sy) && (y < sy + 127)) {
				putpixel(scr, x, y, 135);
				putpixel(scr, x, y + 1, 135);
				//putpixel(scr, x, y+2, 135);
				//if (rand()%2)
				//	putpixel(scr, x+1, y+1, 135);
				//else
				//	putpixel(scr, x-1, y+1, 135);
				//putpixel(scr, x+1, y+2, 135);
			}
			break;
		case FALL:
			break;
	}
}

Word::Word()
{
	size = 0, vis = 0;
	blood = NULL;
	fname = NULL;
}

Word::Word(char *fname)
{
	size = 0, vis = 0;
	blood = NULL;
	this->fname = strdup(fname);
	load();
}

Word::Word(int datname)
{
	size = 0, vis = 0;
	blood = NULL;
	fname = NULL;

	int *xy = (int *)datafile[datname].dat;
	size = xy[0];
	for (int i = 0; i < size; i++) {
		x[i] = xy[2 + i * 2];
		y[i] = xy[2 + i * 2 + 1];
	}
}

Word::~Word()
{
	while (blood != NULL) {
		delblood(blood);
	}
}

void Word::hide()
{
	vis = 0;
}

int Word::isunhide()
{
	if (vis == size)
		return 1;
	else
		return 0;
}

void Word::unhide()
{
	vis = size;
}

void Word::clean()
{
	size = 0, vis = 0;
	blood = NULL;
}

void Word::move()
{
	vis++;
	if (vis > size)
		vis = size;
	//moveblood();
}

void Word::draw(BITMAP *scr, int sx, int sy)
{
	for (int i = 0; i < vis; i++) {
		if ((x[i] > sx) && (x[i] < sx + 128) &&
		        (y[i] > sy) && (y[i] < sy + 128))
			putpixel(scr, x[i], y[i], 135);
	}
	//drawblood(scr, sx, sy);
}

void Word::draw(BITMAP *scr, int sx, int sy, int col)
{
	for (int i = 0; i < vis; i++) {
		if ((x[i] > sx) && (x[i] < sx + 128) &&
		        (y[i] > sy) && (y[i] < sy + 128))
			putpixel(scr, x[i], y[i], col);
	}
	//drawblood(scr, sx, sy);
}

void Word::add(int mx, int my)
{
	if ((x[size - 1] != mx) || (y[size - 1] != my)) {
		x[size] = mx;
		y[size] = my;
		size++;
	}
}

int Word::load()
{
	clean();
	FILE *f = FOPEN_OWN(fname, "rt");
	if (f == NULL) {
		return 0;
	}
	while (fscanf(f, "%d %d\n", &x[size], &y[size]) == 2) size++;
	fclose(f);
	return size;
}

void Word::save()
{
	FILE * f = FOPEN_OWN(fname, "wt");
	for (int i = 0; i < size; i++)
		fprintf(f, "%d %d\n", x[i], y[i]);
	fclose(f);
}

void Word::savebwd()
{
	char *bfname = new char[strlen(fname) + 10];
	strcpy(bfname, fname);
	strcat(bfname, ".bwd");

	int *xy = new int[size * 2 + 1];
	xy[0] = size;
	xy[1] = -1;
	for (int i = 1; i < size; i++) {
		xy[i * 2] = x[i];
		xy[i * 2 + 1] = y[i];
	}
	int fh = OPEN_OWN(bfname, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY);
	assert(fh != -1);
	write(fh, xy, (size * 2 + 1) * sizeof(int));
	close(fh);
	delete []xy;
	delete []bfname;
}

int Word::getdir(int ps)
{ //, int sz, int *x, int *y) {
	int i;
	int nx, ny, ox, oy;
	int dir = 1;
	int step = 10;
	if (ps > size - step) step = size - ps;

	i = ps;
	ox = x[i]; oy = y[i];
	while (--step) {
		i += dir;
		nx = x[i]; ny = y[i];
		if (ny == oy) continue;
		if (ny == oy + 1) return dir;
		if (ny == oy - 1) return -dir;
		break;
	}

	dir = -1;
	step = 10;
	if (ps < step) step = ps;

	i = ps;
	ox = x[i]; oy = y[i];
	while (--step) {
		i += dir;
		nx = x[i]; ny = y[i];
		if (ny == oy) continue;
		if (ny == oy + 1) return dir;
		if (ny == oy - 1) return -dir;
		break;
	}


	if (rand() % 2)
		return 1;
	return -1;
}

void Word::addblood(int &pos)
{
	if (pos == size) pos = 0;
	Blood *b = new Blood(pos, getdir(pos));     //, size, x, y) );
	b->move(x, y, vis);      //or will draw at 0,0

	if (blood == NULL) {
		blood = b;
	} else {
		blood->prev = b;
		b->next = blood;
		blood = b;
	}
}

void Word::addblood()
{
	int pos = 0;

	if (vis < 5) return ;

	pos = rand() % (size - 2); if (pos >= vis) return ;
	//pos = rand()%(vis-2);

	Blood *b = new Blood(pos, getdir(pos));     //, size, x, y) );
	b->move(x, y, vis);      //or will draw at 0,0
	//b->move(x, y, size);

	if (blood == NULL) {
		blood = b;
	} else {
		blood->prev = b;
		b->next = blood;
		blood = b;
		/*		Blood *bl = blood;
				while (bl->next != NULL) {
					bl = bl->next;
				}
				bl->next = b;
				b->prev = bl;*/
	}
}

void Word::delblood(Blood *db)
{
	if (blood == db)
		blood = db->next;
	if (db->prev != NULL) db->prev->next = db->next;
	if (db->next != NULL) db->next->prev = db->prev;

	delete(db);
}

void Word::moveblood()
{
	Blood * b = blood;
	while (b != NULL) {
		b->move(x, y, size);
		if (b->isfall()) {
			//if (blood == b) {
			//	blood = NULL;
			//} else {
			Blood * bb = b->next;
			delblood(b);
			b = bb;
			//}
		} else
			b = b->next;
	}
}

void Word::dropblood()
{
	Blood * b = blood;
	while (b != NULL) {
		b->setdrop();
		b = b->next;
	}
}

void Word::drawblood(BITMAP *scr, int sx, int sy)
{
	Blood * b = blood;
	while (b != NULL) {
		b->draw(scr, sx, sy);
		b = b->next;
	}
}
