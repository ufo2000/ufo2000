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
#ifndef ICON_H
#define ICON_H
#include <allegro.h>
#include "spk.h"
#include "item.h"

class Icon
{
private:
	int    x, y;

	SPK    *tac00;
	BITMAP *iconsbmp;
public:
	Icon(int x2, int y2);
	~Icon();

	void draw();
	int inside(int mx, int my);
	void execute(int mx, int my);
	void info();
	void drawbar(int col1, int col2, int x2, int y2, int val, int valmax);
	void show_eot();

	//void firemenu(Item *it);
	void firemenu(int iplace);
	int doprime(Item *it);

	inline void setxy(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
};

#endif
