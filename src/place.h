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
#ifndef PLACE_H
#define PLACE_H
#include "global.h"
#include "item.h"

/**
 * Items container. Simulates items stacked on the ground and also 
 * unit equipment held in different places and body parts (hands, belt, 
 * backpack, etc.) 
 *
 * @ingroup planner
 * @ingroup battlescape
 */
class Place: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Place);
private:
	friend class Item;

	int gx, gy;

	int width, height;
	int viscol;

	Item *m_item;

	void set(int x, int y, int w, int h);

public:
	Place(int x, int y, int w, int h);
	virtual ~Place();

	void draw(int gx, int gy);
	void drawgrid(int PLACE_NUM);

	Item *mselect();
	int mdeselect(Item *it);

	int put(Item *it);
	int put(Item *it, int xx, int yy);
	Item *get(int ax, int ay);
	int destroy(Item *it);

	int isfree(int xx, int yy);
	int ishand();
	int isfit(Item *it, int xx, int yy);
	void dropall(int lev, int col, int row);
	int outside_belt(int x, int y);
	Item *item_under_mouse();
	int isthere(Item *it);

	bool add_item(int x, int y, const char *item_name);

	void save_to_file(const char *filename, const char *prefix);
	void save_to_string(std::string &str);

	int save_items(char *fs, int _z, int _x, int _y, char *txt);
	int eot_save(int ip, char *txt);

	int count_weight();
	int has_forbidden_equipment();

	void scroll_left();
	void scroll_right();

	Item *item(int ix, int iy);
	Item *item() { return m_item; }
	Item *top_item();
	void set_item(Item *it) { m_item = it; }

	void build_ITEMDATA(int ip, ITEMDATA *id);
	void build_items_stats(char *buf, int &len);

	void destroy_all_items();
	void damage_items(int dam);
	bool check_mine();
	void draw_deselect_time(int PLACE_NUM, int time);

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
