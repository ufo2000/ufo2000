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
#ifndef WORD_H
#define WORD_H
#include <allegro.h>

class Blood
{
private:
	enum State {FLOW, DROP, FALL} state;
	int poscur, posold;
	int dir;
	int x, y;
public:
	Blood *prev, *next;

	Blood(int pos, int dire);
	~Blood();
	int isfall();
	void setdrop();
	void move(int *x, int *y, int sz);
	void draw(BITMAP *scr, int sx, int sy);
};


class Word
{
private:
	int x[1000];
	int y[1000];
	int size, vis;
	Blood *blood;
	char *fname;
public:
	Word();
	Word(char *fname);
	Word(int datname);
	~Word();

	int isunhide();
	void hide();
	void unhide();
	void clean();
	void move();
	void draw(BITMAP *scr, int sx, int sy);
	void draw(BITMAP *scr, int sx, int sy, int col);

	void add(int mx, int my);
	int load();
	void save();
	void savebwd();

	int getdir(int ps);
	void addblood();
	void addblood(int &pos);
	void delblood(Blood *db);
	void moveblood();
	void drawblood(BITMAP *scr, int sx, int sy);
	void dropblood();
};

#endif
