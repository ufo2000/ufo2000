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
#ifndef PLATOON_H
#define PLATOON_H
#include "soldier.h"
#include "units.h"
#include <vector>

class Platoon: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Platoon);
private:
	Soldier *man;
	long     ID;
	long     size;

	char m_visible[4][10 * 6][10 * 6];
	char m_seen[4][10 * 6][10 * 6];

public:
	Platoon() { memset(m_seen, 0, sizeof(m_seen)); }
	Platoon(int PID, int num);
	Platoon(int PID, PLAYERDATA *pd);
	virtual ~Platoon();
	void destroy();

	void move(int ISLOCAL);
	void restore();
	void bullmove();
	void bulldraw();
	void draw_blue_selectors();

	int belong(Soldier *some);
	int belong(Bullet *some);
	Soldier *findman(int SID);
	Soldier *findman(char *sname);
	Soldier *findnum(int N);
	Soldier *next_not_moved_man(Soldier *sel_man);
	Place *find_item(Item *it, int &lev, int &col, int &row);
	int check_for_hit(int z, int x, int y);
	void apply_hit(int z, int x, int y, int type, int hitdir);

	int nobullfly();
	int nomoves();

	int realsize();
	int freeNID();
	void del(int SID);

	Soldier *captain() { return man; }
	int num_of_men() { return size; }

	int check_reaction_fire(Soldier *target);

	void save_MANDATA(char *fn);
	void load_MANDATA(char *fn);
	void save_ITEMDATA(char *fn);
	void load_ITEMDATA(char *fn);
	void save_FULLDATA(char *fn);
	void load_FULLDATA(char *fn);

	void build_Units(Units &local);
	void send_Units(Units &local);

	int eot_save(char *buf, int &buf_size);

	int is_seen(int lev, int col, int row) { return m_seen[lev][col][row]; }
	void set_seen(int lev, int col, int row, int value) { m_seen[lev][col][row] = value; }
	int is_visible(int lev, int col, int row) { return m_visible[lev][col][row]; }
	void set_visible(int lev, int col, int row, int value) { m_visible[lev][col][row] = value; }

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
