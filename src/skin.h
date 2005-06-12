/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2004  ufo2000 development team

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

#ifndef SKIN_H
#define SKIN_H

#include "pck.h"
#include "spk.h"

#define S_HANDOB          0
#define S_XCOM_0          1
#define S_XCOM_1          2
#define S_XCOM_2          3
#define S_XCOM_3          4
#define S_SECTOID         5
#define S_MUTON           6
#define SKIN_NUMBER       7
// not yet implemented:
#define S_SNAKEMAN        7
#define S_CHRYSSALID      8

struct SKIN_INFO {
	const char *Name;
	int         SkinType;
	int         fFemale;
	int         fFlying;
	int         armour_values[5];
    int         cost;
};

extern SKIN_INFO g_skins[];
extern int g_skins_count;

int get_skin_index(int skin_type, int female_flag);

class Skin: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Skin);
public:
    static char *****m_bof;
    static PCK **m_pck;
    static PCK *m_add1;
    static BITMAP *m_image;
    static SPK *m_spk[6][2][4];
private:
    SKIN_INFO skin_info;
    
    Soldier *m_soldier;
    
    void draw_head(int Appearance, int head_frame, int dir, BITMAP *image, int delta);
    void draw_common();
    
    static void initbof();
public:
    Skin(Soldier *soldier, int skin_type, int female_flag);
    
    static void initpck();
	static void freepck();
	
	void update(int skin_type, int female_flag);
	void next_human();
	void next_alien();

    int get_type() { return skin_info.SkinType; }
	int get_fFemale() { return skin_info.fFemale; }
	int get_fFlying() { return skin_info.fFlying; }
	int get_armour_value(int side) { return skin_info.armour_values[side]; }
	
	static int get_armour_cost(int skin_type, int female_flag) { return g_skins[get_skin_index(skin_type, female_flag)].cost; }
	int get_armour_cost() { return get_armour_cost(skin_info.SkinType, skin_info.fFemale); }
	
	bool check_for_hit(int sit, int dir, int lev, int col, int row)
    {
        return m_bof[sit][dir][lev][col][row];
    }
	
	void draw();
	
	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
