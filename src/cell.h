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
#ifndef CELL_H
#define CELL_H

#include "global.h"
#include "place.h"
#include "soldier.h"

//////////////////////////////////////////////////////////////////////////////
/// Cell of map                                                            ///
//////////////////////////////////////////////////////////////////////////////

class Cell: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Cell);
private:
	int m_smog_state;
	int m_smog_time;
 	int m_fire_state;
 	int m_fire_time;

	int m_pfval;      //path finder internal
	int MOUSE;
	char visi[3][3][3];
	Soldier *m_soldier;
	Place *m_place;
public:
	unsigned char type[4];

	Soldier *get_soldier() { return m_soldier; }
	Place *get_place() { return m_place; }
	void set_soldier(Soldier *soldier) { m_soldier = soldier; }
	bool soldier_here() { return m_soldier != NULL; }

	Cell();
	virtual ~Cell();

	void cycle_smoke();

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);

	friend class Map;
};

#endif
