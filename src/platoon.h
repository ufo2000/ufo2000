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
#ifndef PLATOON_H
#define PLATOON_H

#include "soldier.h"

class Units;
class Statistics;

/**
 * Class for handling the squad controlled by one of the opposing sides
 */
class Platoon: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Platoon);
private:
	//! Pointer to current selected soldier in the squad
	Soldier *man;
	//! Squad identifier
	long     ID;
	//! Number of soldiers in the squad
	long     size;

	char m_visible[4][10 * 6][10 * 6];
	char m_seen[4][10 * 6][10 * 6];

	Statistics *m_stats;

	int m_visibility_changed;

public:
	Platoon() { memset(m_seen, 0, sizeof(m_seen)); }
	Platoon(int PID, int num);
	Platoon(int PID, PLAYERDATA *pd, DeployType dep_type);
	virtual ~Platoon();
	void destroy();

	Statistics *get_stats() {return m_stats;}

	void move(int ISLOCAL);
    void restore_moved();
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
	int check_for_hit(int z, int x, int y, Soldier* no_test = NULL);
	void apply_hit(int sniper, int z, int x, int y, int type, int hitdir);
	void set_visibility_changed() { m_visibility_changed = true; }
	void recalc_visibility();

	int nobullfly();
	int nomoves();

	int realsize();

	Soldier *captain()
	{
		if (man == NULL) return NULL;
		return man->is_active() ? man : man->next_active_soldier();
	}
	int num_of_men() { ASSERT(size == realsize()); return size; }
	int calc_platoon_cost();

	int check_reaction_fire(Soldier *target);
	
	void change_morale(int delta, bool send_to_remote);
	void check_morale();

	void save_MANDATA(const char *fn);
	void load_MANDATA(const char *fn);
	void save_ITEMDATA(const char *fn);
	void load_ITEMDATA(const char *fn);
	void save_FULLDATA(char *fn);
	void load_FULLDATA(char *fn);

	void build_Units(Units &local);
	void send_Units(Units &local);

	int eot_save(char *buf, int &buf_size);

	int is_seen(int lev, int col, int row) { return m_seen[lev][col][row]; }
	void set_seen(int lev, int col, int row, int value) { m_seen[lev][col][row] = value; }
	int is_visible(int lev, int col, int row) { return m_visible[lev][col][row]; }
	void set_visible(int lev, int col, int row, int value) { m_visible[lev][col][row] = value; }

	void sit_on_start();

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
