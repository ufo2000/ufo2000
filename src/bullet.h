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
#ifndef BULLET_H
#define BULLET_H

#include "global.h"
#include "item.h"

class Bullet: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Bullet);
private:
	int z0, x0, y0;
	REAL ro, fi, te;      // spher coord	 ro_dist	fi_zang	te_xang

	int z, x, y;
	int i;

	REAL zA;     	// max ampl for thrown
	Item *item;      // thrown item
	int lev, col, row;      // thrown item dst

	enum State {READY, FLY, BEAM, THROWN, HIT, ST_PUNCH, ST_AIMEDTHROW} state;
	int type;      //bullet type
	int phase;
public:

	Bullet();

	void fire(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type);
	void beam(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type);
	void thru(int _z0, int _x0, int _y0, REAL _ro, REAL _fi, REAL _te, REAL _zA, Item *_item);
	void aimedthrow(int _z0, int _x0, int _y0, REAL _fi, REAL _te, Item *_item);
	void punch(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type);

	void move();
	void draw();

	void hitcell();
	void hitman();

	void detonate();
	int explodable();
	int incendiary();


	void showline(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d);
	void showthrow(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d);
	int calc_throw(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d);

	inline int ready()
	{
		if (state == READY)
			return 1;
		return 0;
	}

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
