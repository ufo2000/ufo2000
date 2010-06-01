/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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
#include "cell.h"
#include "scenario.h"

IMPLEMENT_PERSISTENCE(Cell, "Cell");

Cell::Cell( Position p )
    : m_position(p)
{
    m_soldier = NULL;
    m_soldier_aux = NULL;
    MOUSE = 0;
    m_place = new Place(0, 152, 20 * 10, 3 ,this);

    type[0] = 0; type[1] = 0; type[2] = 0; type[3] = 0;

    m_smog_time = 0;
    m_fire_time = 0;
    m_light = scenario->rules[0];
	m_visi = m_light;
    memset(visi, 0, sizeof(visi));
	islight = 0;
}

Cell::~Cell()
{
    delete m_place;
}

void Cell::set_soldier(Soldier *soldier) {
    if (soldier != NULL) {
        if (m_soldier == NULL) {
            m_soldier = soldier;
        }else {
            m_soldier_aux = soldier;
        }
    } else {
        if(m_soldier_aux == NULL){
            m_soldier = NULL;
        }else {
            m_soldier = m_soldier_aux;
            m_soldier_aux = NULL;
        }
    }
}

bool Cell::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);

    PersistWriteObject(archive, m_soldier);
    PersistWriteObject(archive, m_place);
        
    return true;
}

bool Cell::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);

    PersistReadObject(archive, m_soldier);
    PersistReadObject(archive, m_place);
        
    return true;
}
