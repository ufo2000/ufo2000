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

#include "stdafx.h"

#include "global.h"
#include "video.h"
#include "place.h"
#include "map.h"
#include "colors.h"
#include "text.h"

IMPLEMENT_PERSISTENCE(Place, "Place");

Place::Place(int x, int y, int w, int h)
{
	viscol = 0;
	m_item = NULL;
	set(x, y, w, h);
}

Place::~Place()
{
	Item *t1 = m_item, *t2 = NULL;
	while (t1 != NULL) {
		t2 = t1->m_next;
		delete t1;
		t1 = t2;
	}
}

void Place::set(int x, int y, int w, int h)
{
	gx = x; gy = y;
	width = w; height = h;
}

/**
 * Scroll items on the ground-grid left
 */
// Todo: repack items
// ?? maybe scroll several columns at once
void Place::scroll_left()
{
	if (viscol > 0) viscol--;
}

/**
 * Scroll items on the ground-grid right
 */
void Place::scroll_right()
{
	if (viscol < width - 1) viscol++;
}


Item *Place::item_under_mouse()
{
	int xx = (mouse_x - gx) / 16;
	int yy = (mouse_y - gy) / 15;

	if ((xx >= 0) && (xx < width) && (yy >= 0) && (yy < height)) {
		Item *t = m_item;
		while (t != NULL) {
			if (t->inside(xx, yy))
				return t;
			t = t->m_next;
		}
	}
	return NULL;
}

Item *Place::item(int ix, int iy)
{
	Item *t = m_item;
	while (t != NULL) {
		if ((ix == t->m_x) && (iy == t->m_y)) {
			return t;
		}
		t = t->m_next;
	}
	return NULL;
}

Item *Place::top_item()
{
    if (m_item == NULL)
		return NULL;

	Item *t = m_item, *gt = m_item;

	while (t != NULL) {
		if (t->obdata_importance() > gt->obdata_importance()) {
			gt = t;
		}
		t = t->m_next;
	}

    return gt;
}

int Place::isfree(int xx, int yy)
{
	if (!outside_belt(xx, yy))
		if ((xx >= 0) && (xx < width) && (yy >= 0) && (yy < height)) {
			Item * t = m_item;
			while (t != NULL) {
				if (t->inside(xx, yy))
					return 0;
				t = t->m_next;
				//if (t==m_item) return 0;
			}
			return 1;
		}
	return 0;
}

int Place::ishand()
{
	if ((width == 2) && (height == 3))
		return 1;
	return 0;
}

/**
 * Test if item fits into a place like hand, belt, backpack etc.
 * Return 1 if it fits, 0 otherwise
 */
int Place::isfit(Item * it, int xx, int yy)
{
	if (ishand() && (m_item != NULL))
		return 0;

	for (int i = xx; i < xx + it->obdata_width(); i++)
		for (int j = yy; j < yy + it->obdata_height(); j++)
			if (!isfree(i, j)) return 0;
	return 1;
}

/**
 * Put item in place like hand, belt, backpack etc.
 */
int Place::put(Item *it, int xx, int yy)
{
    ASSERT(it->m_place == NULL);

	if (isfree(xx, yy) && isfit(it, xx, yy)) {
		if (m_item != NULL) m_item->m_prev = it;
		it->m_next = m_item; it->m_prev = NULL; it->m_place = this;
		it->setpos(xx, yy);
		m_item = it;
		return 1;
	}
	return 0;
}

int Place::put(Item * it)
{
	it->unlink();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (put(it, x, y))
				return 1;
		}
	}
	return 0;
}

Item * Place::get(int xx, int yy)
{
	Item * t;
	t = m_item;
	while (t != NULL) {
		if (ishand() || t->inside(xx, yy)) {
			t->unlink();
			return t;
		}
		t = t->m_next;
	}
	return NULL;
}

int Place::destroy(Item *it)
{
	Item * t;
	t = m_item;
	while (t != NULL) {
		if (t == it) {
			t->unlink();
			delete it;
			return 1;
		}
		t = t->m_next;
	}
	return 0;
}

/**
 * Select item with mouse
 */ 
Item *Place::mselect()
{
	if ((mouse_x > gx) && (mouse_x < gx + width * 16))
		if ((mouse_y > gy) && (mouse_y < gy + height * 15))
			return get((mouse_x -gx) / 16 + viscol, (mouse_y - gy) / 15);
	return NULL;
}

/**
 * De-select item with mouse
 */ 
int Place::mdeselect(Item *it)
{
	if ((mouse_x > gx) && (mouse_x < gx + width * 16))
		if ((mouse_y > gy) && (mouse_y < gy + height * 15)) {
			int x2 = (mouse_x - gx - (it->obdata_width() - 1) * 8) / 16 + viscol;
			int y2 = (mouse_y - gy - (it->obdata_height() - 1) * 8) / 15;
			//text_mode(0);
			//textprintf(screen, font, 1, 150, 1, "x=%d y=%d", x2, y2);

			if (isfree(x2, y2) && isfit(it, x2, y2)) {
				put(it, x2, y2);
				return 1;
			}
		}
	return 0;
}

int Place::outside_belt(int x, int y)
{
	if ((width == 4) && (height == 2))
		if ((y == 1) && ((x == 1) || (x == 2)))
			return 1;
	return 0;
}

/**
 * Draw item at a place on the battlemap.
 * If there are several items, draw the item with the highest importance.
 */
void Place::draw(int gx, int gy)
{
	if (m_item == NULL)
		return ;

	Item *t = m_item, *gt = m_item;

	while (t != NULL) {
		if (t->obdata_importance() > gt->obdata_importance()) {
			gt = t;
		}
		t = t->m_next;
	}

	map->drawitem(gt->obdata_pMap(), gx, gy);
}

// moved to text.cpp :
/*
static char *place_name[11] = {
    ("RIGHT SHOULDER"),
    ("LEFT SHOULDER"),
    ("RIGHT HAND"),
    ("LEFT HAND"),
    ("RIGHT LEG"),
    ("LEFT LEG"),
    ("BACK PACK"),
    ("BELT"),
    ("GROUND"),
    ("ARMOURY"),
    ("COMMON POOL")
};
*/

/**
 * Draw inventory-grid for belt, backpack, armory etc.
 */
void Place::drawgrid(int PLACE_NUM)
{
	ASSERT((PLACE_NUM >= 0) && (PLACE_NUM <= NUMBER_OF_PLACES));

    if (PLACE_NUM == P_ARMOURY)
        textout(screen2, large,        place_name[PLACE_NUM], gx, gy + 1 - text_height(large), COLOR_LT_OLIVE);
	else
        textout(screen2, g_small_font, place_name[PLACE_NUM], gx, gy + 1 - text_height(g_small_font), COLOR_LT_OLIVE);

	if (!ishand()) {
		int dx = 0, dy = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (!outside_belt(j, i))
					rect(screen2, gx + dx, gy + dy, gx + dx + 16, gy + dy + 15, COLOR_GRAY08);      //square
				dx += 16;
				if (j == 19) break;      // for map cell!!!!!!!!!!!!!!!!!!!
			}
			dx = 0;
			dy += 15;
		}
	} else {
		rect(screen2, gx, gy, gx + width * 16, gy + height * 15, COLOR_GRAY08);
	}

	// Draw items in grid:
	Item *t = m_item;
	while (t != NULL) {
		if ((t->m_x >= viscol - 1) && (t->m_x < viscol + 20)) {

			if (!is_item_allowed(t->itemtype())) {
				int x = gx + (t->m_x - viscol) * 16;
				int y = gy + t->m_y * 15;
				int sx = t->obdata_width() * 16;
				int sy = t->obdata_height() * 15;
				if (ishand()) {
					x = gx; 
					y = gy;
					sx = width * 16;
					sy = height * 15;
				}
				rectfill(screen2, x + 1, y + 1, x + sx - 1, y + sy - 1, COLOR_RED06);
			}

			int x = gx + (t->m_x - viscol) * 16;
			int y = gy + t->m_y * 15 + 5;

			if (ishand()) {
				int it_width = t->obdata_width();
				int it_height = t->obdata_height();
				x = gx + (width - it_width) * 16 / 2;
				y = gy + (height - it_height) * 15 / 2 + 5;
			}

			PCK::showpck(t->obdata_pInv(), x, y);  // Picture of item 

			if (ishand()) {	// Inventory-view: display ammo-rounds & grenade-delay 
				if (t->clip() != NULL) {   // see also: Soldier::drawinfo()
				    printsmall(gx + 23, gy + 39, COLOR_WHITE, t->roundsremain() );
				    textout(screen2, g_small_font, t->get_damage_name(), gx + 3, gy + 36, COLOR_GREEN);
				} 
				if (t->obdata_isAmmo() ) {   // Test
				    printsmall(gx + 23, gy + 39, COLOR_WHITE, t->m_rounds );
				} 
				if (t->is_grenade() ) {  // see also: icon.h : DrawPrimed 
				    if (t->delay_time() > 0)
					textprintf(screen2, g_small_font, gx+23, gy+36, COLOR_RED, "%d", t->delay_time() - 1);
				    else if (t->itemtype() == PROXIMITY_GRENADE && t->delay_time() < 0)
					textout(screen2, g_small_font, "*", gx + 23, gy + 36, COLOR_RED); 
				}
			}

			if (key[KEY_LCONTROL]) {
				t->draw_health(1, x + 1, y - 4);
			}
			if (key[KEY_ALT]) {
				t->draw_health(0, x + 1, y - 4);
			}
		}
		t = t->m_next;
	}
}

void Place::dropall(int lev, int col, int row)
{
	Item *t = m_item;
	while (t != NULL) {
		//text_mode(0);
		//textprintf(screen, font, 1, 150, 1, "x=%d y=%d t=%2x p=%4s n=%1s   ", t->x, t->y, t->type, t->prev, t->next);
		Item *tn = t->m_next;
		map->place(lev, col, row)->put(t);
		//textprintf(screen, font, 1, 160, 1, "put t=%2x", t->type);
		//textprintf(screen, font, 1, 170, 1, "x=%d y=%d t=%2x p=%4s n=%1s   ", t->x, t->y, t->type, t->prev, t->next);
		t = tn;
	}
	m_item = NULL;
}

int Place::isthere(Item *it)
{
	Item *t;
	t = m_item;
	while (t != NULL) {
		if (t == it) {
			ASSERT(it->m_place == this);
			return 1;
		}
		t = t->m_next;
	}
	ASSERT(it->m_place != this);
	return 0;
}

void Place::save_to_file(const char *fn, const char *prefix)
{
	FILE *fh = fopen(F(fn), "wt");
	ASSERT(fh != NULL);

	Item *it = m_item;
	while (it != NULL) {
		fprintf(fh, "%s:add_item(%d, %d, \"%s\")\n", prefix, it->m_x, it->m_y, it->name().c_str());
		it = it->m_next;
	}
	fclose(fh);
}

bool Place::add_item(int x, int y, const char *item_name)
{
	int stack_top = lua_gettop(L);
    // Enter 'ItemsTable' table
	lua_pushstring(L, "ItemsTable");
	lua_gettable(L, LUA_GLOBALSINDEX);
	ASSERT(lua_istable(L, -1)); 
    // Enter [item_name] table
	lua_pushstring(L, item_name);
	lua_gettable(L, -2);
	if (!lua_isnil(L, -1)) {
		lua_pushstring(L, "index");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		Item *it = new Item((int)lua_tonumber(L, -1));
		// Trying to put item here
		if (put(it, x, y)) {
			lua_settop(L, stack_top);
			return true;
		}
		// Maybe it is ammo for already added weapon?
		Item *weapon = get(x, y);
		if (weapon) {
			bool loaded = weapon->loadclip(it);
			put(weapon, x, y);
            if (loaded) {
                lua_settop(L, stack_top);
                return true;
            }
		}
        // Nowhere to put it, giving up
        delete it;
	}
	lua_settop(L, stack_top);
	return false;
}

void Place::build_ITEMDATA(int ip, ITEMDATA * id) //don't save clip rounds
{
	Item * it = m_item;
	while (it != NULL)
	{
		ASSERT(id->num < 100);
		id->place[id->num] = ip;
		id->type[id->num] = it->m_type;
		id->x[id->num] = it->m_x;
		id->y[id->num] = it->m_y;
		id->num++;
		if (it->haveclip()) {
			id->place[id->num] = 0xFF;
			id->type[id->num] = it->cliptype();
			id->num++;
		}
		it = it->m_next;
	}
}

void Place::save_to_string(std::string &str)
{
	str.clear();

    // search for the last item in the list (it was added first)
	Item *it = m_item;
	while (it != NULL && it->m_next) it = it->m_next;
	
    // save items list in correct order (items added first are listed in the beginning of list)
	while (it != NULL) {
		char line[512];
		if (!it->haveclip()) {
			sprintf(line, "{%d, %d, \"%s\"},\n", it->m_x, it->m_y, it->name().c_str());
		} else {
			sprintf(line, "{%d, %d, \"%s\", \"%s\"},\n", it->m_x, it->m_y, it->name().c_str(), 
				Item::obdata_name(it->cliptype()).c_str());
		}
		str += line;
		it = it->m_prev;
	}
}

void Place::build_items_stats(char *buf, int &len)
{
	Item *it = m_item;
	while (it != NULL) {
		buf[len++] = it->m_type;
		if (it->haveclip()) {
			buf[len++] = it->cliptype();
		}
		it = it->m_next;
	}
}


int Place::save_items(char *fs, int _z, int _x, int _y, char *txt)
{
	int len = 0;
	Item *t = m_item;

	char format[1000];
	sprintf(format, "%s type=%%d x=%%d y=%%d rounds=%%d\r\n", fs);

	while (t != NULL) {
		len += sprintf(txt + len, format, _z, _x, _y,
		               t->m_type, t->m_x, t->m_y, t->m_rounds);

		if (t->haveclip()) {
			len += sprintf(txt + len, format, _z, _x, _y,
			               t->cliptype(), -1, -1, t->roundsremain());
		}
		t = t->m_next;
	}
	return len;
}

/**
 * End-of-turn - Save
 */ 
int Place::eot_save(int ip, char *txt)
{
	int len = 0;
	Item *t = m_item;

	while (t != NULL) {
		len += sprintf(txt + len, "ip=%d type=%d x=%d y=%d rounds=%d\r\n",
		               ip, t->m_type, t->m_x, t->m_y, t->m_rounds);
		if (t->haveclip()) //////////////////////
		{
			len += sprintf(txt + len, "ip=%d type=%d x=%d y=%d rounds=%d\r\n",
			               -1, t->cliptype(), -1, -1, t->roundsremain());
		}
		t = t->m_next;
	}
	return len;
}

/**
 * Calculate and return weight for all the equipment stored at this place
 */
int Place::count_weight()
{
	Item *t = m_item;
	int weight = 0;

	while (t != NULL)
	{
		weight += t->obdata_weight();
		if (t->haveclip())
			weight += t->clip()->obdata_weight();
		t = t->m_next;
	}
	return weight;
}

/**
 * Check if any banned equipment (not in allowed equipment set) 
 * is stored at this place
 */
int Place::has_forbidden_equipment()
{
	Item *t = m_item;

	while (t != NULL)
	{
		if (!is_item_allowed(t->itemtype()))
			return 1;
		t = t->m_next;
	}
	return 0;
}

void Place::destroy_all_items()
{
	Item *t, *t2;
	t = m_item;
	while (t != NULL) {
		t2 = t->m_next;
		delete t;
		t = t2;
	}
	m_item = NULL;
}

#include "explo.h"

void Place::damage_items(int dam)
{
	/*Item *it;
	it = m_item;
	while(it != NULL) {
		if (it->damage(dam)) { //destroyed
			Item *t2;
			t2 = it->next;
			if (m_item == it)
				m_item = t2;

			it->unlink();
			elist.remove(it);
			delete it; // explodable

			it = t2;
			continue;			
		}
		it = it->next;
	}*/
}

/**
 * Explodes proximity mine if this place has one
 */
bool Place::check_mine() 
{
	Item *it = m_item;

	while (it != NULL) {
		if ((it->is_grenade()) && (it->m_type == PROXIMITY_GRENADE) && (it->delay_time() < 0)) {
			elist->check_for_detonation(1, it);
			return true;
		}
		it = it->m_next;
	}
	return false;
}

/**
 * Show TUs needed to move an item to a place like hand, belt, backpack etc.
 */
void Place::draw_deselect_time(int PLACE_NUM, int time)
{
	int color = COLOR_WHITE;
	if (time) {
		//place_name[PLACE_NUM]
		//textout(screen2, small, time, gx, gy-8, 66);

		//if (havetime(time) != OK) { color = COLOR_GRAY; }  // ??
		printsmall(gx + 1 + text_length(g_small_font, place_name[PLACE_NUM]), gy - 6, color, time);
	}
}

bool Place::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, m_item);

	return true;
}

bool Place::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, m_item);

	return true;
}
