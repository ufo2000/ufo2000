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

#ifndef EDITOR_H
#define EDITOR_H

#include "global.h"
#include "platoon.h"

class Place;
class Units;
class Soldier;
class SPK;
class Item;

struct Buffer
{
	bool empty;
	MANDATA md;
	ITEMDATA id;
}; 

/** 
 * Squad editor class. It is used from the planner screen to load,
 * save or edit soldiers in a current squad.
 *
 * @ingroup planner
 */
class Editor
{ 
private:
	Place   *m_armoury;
	Soldier *man;
	Platoon *m_plt;

	SPK     *tac01;
	BITMAP  *b123, *b4, *b5;

	Item    *sel_item;
	Item    *dup_item;
	int     sel_item_place;

	Buffer buffer;

	int load_clip();
	bool handle_mouse_leftclick();

	void copy_soldier(Soldier *src);
	void paste_soldier(Soldier *dest);
	
	void change_equipment();

public:
	Editor();
	~Editor();

	void load();
	void save();
	void show();

	int set_man(char *name);

	void build_Units(Units &local) { m_plt->build_Units(local);	}
	void send_Units(Units &local) { m_plt->send_Units(local); }
	Platoon *platoon() { return m_plt; };

	void edit_soldier();
};

#endif
