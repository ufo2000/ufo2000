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
#include "global.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "video.h"
#include "soldier.h"
#include "sound.h"
#include "map.h"
#include "multiplay.h"
#include "wind.h"
#include "explo.h"

SKIN_INFO g_skins[] = 
{
	{ "male",    S_XCOM_0,  0 },
	{ "female",  S_XCOM_0,  1 },
	{ "sectoid", S_SECTOID, 0 },
	{ "muton",   S_MUTON,   0 }
};

int g_skins_count = sizeof(g_skins) / sizeof(g_skins[0]);

/**
 * Function that gets index into g_skins array for given unit characteristics
 *
 * @todo move skins handling to a separate class, responsible for displaying
 *       unit image
 */
int get_skin_index(int SkinType, int fFemale)
{
	for (int i = 0; i < g_skins_count; i++) {
		if (g_skins[i].SkinType == SkinType && g_skins[i].fFemale == fFemale)
			return i;
	}
	return fFemale ? 1 : 0;
}

char *****Soldier::m_bof = NULL;
PCK **Soldier::m_pck = NULL;
SPK *Soldier::m_spk[4][2][4] = {{{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}}};

BITMAP *Soldier::m_unibord = NULL;

//dirs      0  1  2  3  4  5  6  7
int Soldier::dir2ofs[8] = {1, 1, 0, -1, -1, -1, 0, 1};
//y  x   -1  0  1
char Soldier::ofs2dir[3][3] = {{5, 6, 7},
                               {4, 8, 0},
                               {3, 2, 1}};

IMPLEMENT_PERSISTENCE(Soldier, "Soldier");

void Soldier::initpck()
{
	static char *skin_fname[] = {
        "handob.pck", "xcom_0.pck", "xcom_1.pck", "xcom_2.pck", "xcom_2.pck",
        "sectoid.pck", "muton.pck", "snakeman.pck", "ethereal.pck", "floater.pck",
        "celatid.pck", "silacoid.pck", "chrys.pck", "civm.pck", "civf.pck",
        "zombie.pck"
    };

	assert(sizeof(skin_fname) / sizeof(skin_fname[0]) >= SKIN_NUMBER);

	m_pck = new PCK *[SKIN_NUMBER];
	char fname[100];
	for (int n = 0; n < SKIN_NUMBER; n++) {
		sprintf(fname, "units/%s", skin_fname[n]);
		m_pck[n] = new PCK(fname);
	}

	for (int ar = 0; ar < 2; ar++)
		for (int se = 0; se < 2; se++)
			for (int ty = 0; ty < 4; ty++) {
				sprintf(fname, "ufograph/man_%d%c%d.spk", ar, se == 0 ? 'm' : 'f', ty);
				m_spk[ar][se][ty] = new SPK(fname);
			}

	m_spk[2][0][0] = new SPK("ufograph/man_2.spk");
	m_spk[3][0][0] = new SPK("ufograph/man_3.spk");

	SPK *unibord = new SPK("ufograph/unibord.pck");
	m_unibord = create_bitmap(320, 200); clear(m_unibord);
	unibord->show_pck(m_unibord, 0, 0);
	delete unibord;

	initbof();
}

char body_txt[] = \
	"  01234567 76543210\n"
	"0|                 0\n"
	"1|     111 000     1\n"
	"2|     122 000     2\n"
	"3|   12233 00000   3\n"
	"4|  122233 000000  4\n"
	"5| 121 233 000 000 5\n"
	"0| 12  233 000  00 0\n"
	"1|     123 000     1\n"
	"2|    122   000    2\n"
	"3|   122     000   3\n"
	"4|  122       000  4\n"
	"5|                 5\n"
	"  01234567 01234567\n"
	"\n"
	"  01234567 76543210\n"
	"0|                 0\n"
	"1|                 1\n"
	"2|                 2\n"
	"3|                 3\n"
	"4|     111 000     4\n"
	"5|     122 000     5\n"
	"0|   12233 00000   0\n"
	"1|  122233 000000  1\n"
	"2| 121 133 000 000 2\n"
	"3|     223 000     3\n"
	"4|    122   000    4\n"
	"5|                 5\n"
	"  01234567 01234567\n";

void Soldier::initbof()
{
	int s, d, i, j, k;

	m_bof = new char****[2];
	for (s = 0; s < 2; s++) {
		m_bof[s] = new char***[8];
		for (d = 0; d < 8; d++) {
			m_bof[s][d] = new char**[12];
			for (i = 0; i < 12; i++) {
				m_bof[s][d][i] = new char*[16];
				for (j = 0; j < 16; j++) {
					m_bof[s][d][i][j] = new char[16];
					for (k = 0; k < 16; k++)
						m_bof[s][d][i][j][k] = 0;
				}
			}
		}
	}

	int fl = strlen(body_txt);
	char *dat = new char[fl];
	memcpy(dat, body_txt, fl);

	char *beg, *str = dat;
	char body[12][16];

	for (s = 0; s < 2; s++) {
		memset(body, 0, sizeof(body));
		i = 0;
		while ((beg = (char *)memchr((void *)str, '|', fl)) != NULL) {
			for (int j = 0; j < 8; j++) {
				body[i][j] = beg[j + 1];
				body[i][15 - j] = beg[j + 1];
			}
			fl -= (beg - str);
			str = beg + 1;
			i++;
			if (i > 11)
				break;
		}

		d = 0;
		for (i = 0; i < 12; i++) {
			for (j = 0; j < 16; j++) {
				if (isdigit(body[i][j])) {
					int w = body[i][j] - '0';
					for (k = 0; k < w; k++) {
						m_bof[s][d][11 - i][7 - k][j] = 1;
						m_bof[s][d][11 - i][8 + k][j] = 1;
					}
				}
			}
		}

		for (i = 0; i < 12; i++) {
			for (d = 1; d < 8; d++) {
				REAL ang = d * PI / 4.0;
				REAL co = cos(ang);
				REAL si = sin(ang);
				for (j = 0; j < 16; j++)
					for (k = 0; k < 16; k++) {
						if (m_bof[s][0][i][j][k]) {
							REAL xx = (REAL)j - 7.5;
							REAL yy = (REAL)k - 7.5;
							int x = (int)ceil(xx * co - yy * si + 7.0);
							int y = (int)ceil(xx * si + yy * co + 7.0);
							if ((x < 16) && (y < 16) &&
							        (x >= 0) && (y >= 0))
								m_bof[s][d][i][x][y] = 1;
						}
					}
			}
		}
	}
	delete [] dat;
}

void Soldier::freepck()
{
	for (int n = 0; n < SKIN_NUMBER; n++)
		delete m_pck[n];
	delete [] m_pck;

	for (int ar = 0; ar < 2; ar++)
		for (int se = 0; se < 2; se++)
			for (int ty = 0; ty < 4; ty++)
				delete m_spk[ar][se][ty];

	delete m_spk[2][0][0];
	delete m_spk[3][0][0];

	int s, d, i, j;
	for (s = 0; s < 2; s++) {
		for (d = 0; d < 8; d++) {
			for (i = 0; i < 12; i++) {
				for (j = 0; j < 16; j++)
					delete [] m_bof[s][d][i][j];
				delete [] m_bof[s][d][i];
			}
			delete [] m_bof[s][d];
		}
		delete [] m_bof[s];
	}
	delete [] m_bof;
}

Soldier::Soldier(int _NID)
{
	NID = _NID;
	z = -1; x = -1; y = -1;
	dir = 0; phase = 0; state = STAND;
	m_next = NULL; m_prev = NULL;

	m_bullet = new Bullet;
	m_place[P_SHL_RIGHT] = new Place(16, 40, 2, 1);
	m_place[P_SHL_LEFT] = new Place(112, 40, 2, 1);
	m_place[P_ARM_RIGHT] = new Place(0, 64, 2, 3);
	m_place[P_ARM_LEFT] = new Place(128, 64, 2, 3);
	m_place[P_LEG_RIGHT] = new Place(0, 120, 2, 1);
	m_place[P_LEG_LEFT] = new Place(128, 120, 2, 1);
	m_place[P_BACK] = new Place(192, 40, 3, 3);
	m_place[P_BELT] = new Place(192, 104, 4, 2);
	m_place[P_MAP] = new Place(0, 152, 20 * 10, 3);      //as map
	m_p_map_allocated = true;

	curway = -1; waylen = 0;
	FIRE_num = 0;
	enemy_num = 0;
	seen_enemy_num = 0;
	MOVED = 0;

	memset(&md, 0, sizeof(md));
	memset(&id, 0, sizeof(id));
	memset(&ud, 0, sizeof(ud));
}


Soldier::Soldier(int _NID, int _z, int _x, int _y)
{
	NID = _NID; z = _z; x = _x; y = _y;
	dir = 0;      //!!face to center of map
	phase = 0; state = STAND;
	m_next = NULL; m_prev = NULL;

	m_bullet = new Bullet;
	m_place[P_SHL_RIGHT] = new Place(16, 40, 2, 1);
	m_place[P_SHL_LEFT] = new Place(112, 40, 2, 1);
	m_place[P_ARM_RIGHT] = new Place(0, 64, 2, 3);
	m_place[P_ARM_LEFT] = new Place(128, 64, 2, 3);
	m_place[P_LEG_RIGHT] = new Place(0, 120, 2, 1);
	m_place[P_LEG_LEFT] = new Place(128, 120, 2, 1);
	m_place[P_BACK] = new Place(192, 40, 3, 3);
	m_place[P_BELT] = new Place(192, 104, 4, 2);
	m_place[P_MAP] = map->place(z, x, y);      //!!
	m_p_map_allocated = false;

	curway = -1; waylen = 0;
	FIRE_num = 0;
	enemy_num = 0;
	seen_enemy_num = 0;
	MOVED = 0;
}


Soldier::Soldier(int _NID, int _z, int _x, int _y, MANDATA *mdat, ITEMDATA *idat)
{
	NID = _NID; z = _z; x = _x; y = _y;
	dir = 0;

	//face to center of map
	int dest_col = map->width * 10 / 2;
	int dest_row = map->height * 10 / 2;
	fixed ox = itofix(dest_col - x);
	fixed oy = itofix(dest_row - y);
	if ((!ox) && (!oy)) {
		dir = 0;
	} else {
		int ang = fixtoi(fatan2(oy, ox));
		if (ang < 0) ang = 256 + ang;
		ang = (ang + 16) % 256;
		dir = ang >> 5;
	}

	phase = 0; state = STAND;
	m_next = NULL; m_prev = NULL;

	m_bullet = new Bullet;
	m_place[P_SHL_RIGHT] = new Place(16, 40, 2, 1);
	m_place[P_SHL_LEFT] = new Place(112, 40, 2, 1);
	m_place[P_ARM_RIGHT] = new Place(0, 64, 2, 3);
	m_place[P_ARM_LEFT] = new Place(128, 64, 2, 3);
	m_place[P_LEG_RIGHT] = new Place(0, 120, 2, 1);
	m_place[P_LEG_LEFT] = new Place(128, 120, 2, 1);
	m_place[P_BACK] = new Place(192, 40, 3, 3);
	m_place[P_BELT] = new Place(192, 104, 4, 2);
	m_place[P_MAP] = map->place(z, x, y);      //!!
	m_p_map_allocated = false;

	curway = -1; waylen = 0;
	FIRE_num = 0;
	enemy_num = 0;
	seen_enemy_num = 0;
	MOVED = 0;

	memcpy(&md, mdat, sizeof(md));
	memcpy(&id, idat, sizeof(id));

	initialize();
}


Soldier::~Soldier()
{
	delete m_bullet;
	for (int i = 0; i < 8; i++) delete m_place[i];
	if (m_p_map_allocated) delete m_place[P_MAP];

	//closegame() and kill
	if (map != NULL && x != -1 && y != -1 && z != -1)
		map->set_man(z, x, y, NULL);
}

void Soldier::initialize()
{
	process_MANDATA();
	process_ITEMDATA();

	ud.CurTU = ud.MaxTU;
	ud.CurHealth = ud.MaxHealth;
	ud.CurEnergy = ud.MaxEnergy;
	ud.CurReactions = md.Reactions;
	ud.CurStrength = ud.MaxStrength;
	ud.Morale = 100;
	ud.CurFAccuracy = ud.MaxFA;
	ud.CurTAccuracy = ud.MaxTA;

	ud.CurFront = ud.MaxFront;
	ud.CurLeft = ud.MaxLeft;
	ud.CurRight = ud.MaxRight;
	ud.CurRear = ud.MaxRear;
	ud.CurUnder = ud.MaxUnder;
}

void Soldier::process_MANDATA()
{
	memset(&ud, 0, sizeof(ud));
	//!!!copy data from md to ud
	strcpy(ud.Name, md.Name);
	ud.MaxTU = md.TimeUnits;
	ud.MaxHealth = md.Health;
	ud.MaxStrength = md.Strength;
	ud.MaxEnergy = 100;

//	Verify that md.SkinType and md.fFemale have valid values
	int skin_index = get_skin_index(md.SkinType, md.fFemale);
	md.SkinType = g_skins[skin_index].SkinType;
	md.fFemale  = g_skins[skin_index].fFemale;

	/*ud.MaxFront = md.;
	ud.MaxLeft = md.;
	ud.MaxRight = md.;
	ud.MaxRear = md.;
	ud.MaxUnder = md.;*/
	ud.MaxFA = md.Firing;
	ud.MaxTA = md.Throwing;
	ud.Morale = 100;
	ud.HeadWound = 0;
	ud.TorsoWound = 0;
	ud.RArmWound = 0;
	ud.LArmWound = 0;
	ud.RLegWound = 0;
	ud.LLegWound = 0;

	int armour[5] = {12, 8, 8, 5, 2};
	ud.MaxFront = armour[0];
	ud.MaxLeft = armour[1];
	ud.MaxRight = armour[2];
	ud.MaxRear = armour[3];
	ud.MaxUnder = armour[4];

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ud.CurTU = ud.MaxTU;
	ud.CurHealth = ud.MaxHealth;
	ud.CurEnergy = ud.MaxEnergy;
	ud.CurReactions = md.Reactions;
	ud.CurStrength = ud.MaxStrength;
	ud.Morale = 100;
	ud.CurFAccuracy = ud.MaxFA;
	ud.CurTAccuracy = ud.MaxTA;

	ud.CurFront = ud.MaxFront;
	ud.CurLeft = ud.MaxLeft;
	ud.CurRight = ud.MaxRight;
	ud.CurRear = ud.MaxRear;
	ud.CurUnder = ud.MaxUnder;
}

void Soldier::process_ITEMDATA()
{
	destroy_all_items();

	Item * it = NULL;
	for (int i = 0; i < id.num; i++) {
		if (id.place[i] == 0xFF) //clip
		{
			assert(it != NULL);
			int v = it->loadclip(new Item(id.type[i]));
			assert(v);
		} else {
			Place *pp = place(id.place[i]);
			assert(pp != NULL);
			it = new Item(id.type[i]);
			int v = pp->put(it, id.x[i], id.y[i]);
			assert(v);
		}
	}
}

void Soldier::build_ITEMDATA()
{
	memset(&id, 0, sizeof(id));
	strcpy(id.name, md.Name);

	for (int i = 0; i < NUMBER_OF_PLACES; i++) {
		m_place[i]->build_ITEMDATA(i, &id);
	}
}

void Soldier::destroy_all_items()
{
	for (int i = 0; i < NUMBER_OF_PLACES; i++) {
		m_place[i]->destroy_all_items();
	}
}


int Soldier::calc_ammunition_cost()
{
	int p = md.TimeUnits +
	        md.Health +
	        md.Firing +
	        md.Throwing;

	return p;
}

void Soldier::build_items_stats(char *buf, int &len)
{
	for (int i = 0; i < NUMBER_OF_PLACES; i++) {
		m_place[i]->build_items_stats(buf, len);
	}
}


void Soldier::restore()
{
	seen_enemy_num = 0;
	MOVED = 0;
	ud.CurTU = ud.MaxTU;
	//ud.CurHealth = ud.MaxHealth;
	//ud.CurStrength = ud.MaxStrength;
	//ud.CurEnergy = ud.MaxEnergy;
	//ud.Morale = 100;
	//ud.CurFAccuracy = ud.MaxFA;
	//ud.CurTAccuracy = ud.MaxTA;
}

#define Y_SIT 5
#define MAPX 0
#define MAPY (-18)

void Soldier::draw()
{
	int head_frame = 32;
	if (md.fFemale) head_frame = 16 * 16 + 11;

	int gx = map->x + CELL_SCR_X * x + CELL_SCR_X * y;
	int gy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
	if ((z > 0) && map->isStairs(z - 1, x, y)) {
		gy += CELL_SCR_Z + map->mcd(z - 1, x, y, 3)->T_Level;
	} else {
		gy += map->mcd(z, x, y, 0)->T_Level;
		gy += map->mcd(z, x, y, 3)->T_Level;
	}


	if (state == DIE) {
		m_pck[md.SkinType]->showpck(264 + phase / 3, gx, gy);
		return ;
	}

	BITMAP *image;
	int arm1, arm2, army, handob_y = 0;
	int yofs;

	image = create_bitmap(32, 32);
	clear_to_color(image, xcom1_color(0));

	arm1 = 0; arm2 = 0; army = 5;
	if (dir < 4) {
		arm2 = 1;
	} else {
		arm1 = 1;
	}

	if (state == SIT) handob_y += 4;
	if (md.SkinType == S_SECTOID) handob_y += 6; // $$$

	if ((lhand_item() != NULL) || (rhand_item() != NULL)) {
		if (state != MARCH) {
			arm1 += 30;
			arm2 += 30;
		}
	}
	if ((dir < 1) || (dir > 5)) {
		if (rhand_item() != NULL)
			m_pck[S_HANDOB]->drawpck(dir + 8 * rhand_item()->data()->pHeld, image, handob_y);
		else if (lhand_item() != NULL)
			m_pck[S_HANDOB]->drawpck(dir + 8 * lhand_item()->data()->pHeld, image, handob_y);
	}

	switch (state) {
		case DIE: break;      //neverhap
		case SIT:
			m_pck[md.SkinType]->drawpck(dir + 8 * arm1, image, Y_SIT);
			m_pck[md.SkinType]->drawpck(dir + head_frame, image, Y_SIT + 1);      //head
			m_pck[md.SkinType]->drawpck(dir + 8 * 3, image, 2);
			m_pck[md.SkinType]->drawpck(dir + 8 * arm2, image, Y_SIT);
			break;
		case STAND:
			m_pck[md.SkinType]->drawpck(dir + 8 * arm1, image, 0);
			m_pck[md.SkinType]->drawpck(dir + head_frame, image, 0);      //head
			m_pck[md.SkinType]->drawpck(dir + 8 * 2, image, 0);
			m_pck[md.SkinType]->drawpck(dir + 8 * arm2, image, 0);
			break;
		case MARCH:
			if (phase % 4 == 0)
				yofs = -1;
			else
				if (phase % 2 == 1)
					yofs = 0;
				else
					yofs = 2;

			if ((lhand_item() != NULL) || (rhand_item() != NULL)) {
				arm1 += 30;
				arm2 += 30;
				m_pck[md.SkinType]->drawpck(dir + 8 * arm1, image, 0);
			} else {
				m_pck[md.SkinType]->drawpck(phase + (dir * 3 + 7 - 1 - arm2) * 8, image, yofs);
			}

			m_pck[md.SkinType]->drawpck(dir + head_frame, image, 0);      //head
			int yo = 0;
			if (phase % 4 == 0) yo = -1;

			m_pck[md.SkinType]->drawpck(phase + (dir * 3 + 7) * 8, image, yo);     //yofs);

			if ((lhand_item() != NULL) || (rhand_item() != NULL)) {
				m_pck[md.SkinType]->drawpck(dir + 8 * arm2, image, 0);
			} else {
				m_pck[md.SkinType]->drawpck(phase + (dir * 3 + 7 - 1 - arm1) * 8, image, yofs);
			}
			break;
	}

	if ((dir > 0) && (dir < 6)) {
		if (rhand_item() != NULL)
			m_pck[S_HANDOB]->drawpck(dir + 8 * rhand_item()->data()->pHeld, image, handob_y);
		else if (lhand_item() != NULL)
			m_pck[S_HANDOB]->drawpck(dir + 8 * lhand_item()->data()->pHeld, image, handob_y);
	}

	int ox, oy;
	if (dir < 3) ox = 1; else if ((dir == 3) || (dir == 7)) ox = 0; else ox = -1;
	if ((dir == 1) || (dir == 5)) {
		oy = 0; ox *= 2;
	} else
			if ((dir > 1) && (dir < 5)) oy = -1; else oy = 1;
	if ((dir == 3) || (dir == 7)) {
		ox = 0; oy *= 2;
	}

	//text_mode(-1);
	//textprintf(image, font, 0, 0, 1, "%d", phase);
	//textprintf(image, font, 0, 8, 1, "%d%d", ox, oy);
	//textprintf(image, font, 0, 0, 1, "%d", enemy_num);
	//textprintf(image, font, 0, 0, 1, "%d", seen_enemy_num);


	if (state == MARCH) {
		if (phase < 4)
			draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
		else
			draw_sprite(screen2, image, gx + (phase - 8) * 2 * ox, gy - (phase - 8) * oy);
	} else
		draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);

	destroy_bitmap(image);
}


void Soldier::draw_inventory()
{
	showspk();

	text_mode(-1);
	textout(screen2, large, md.Name, 0, 0, xcom1_color(66));
	textout(screen2, g_small_font, "TUS>", 250, 24, xcom1_color(66));
	textprintf(screen2, g_small_font, 271, 24, xcom1_color(18), "%d", ud.CurTU);

	for (int i = 0; i < 8; i++)
		m_place[i]->drawgrid(i);

	map->place(z, x, y)->drawgrid(P_MAP);
}


void Soldier::draw_unibord(int gx, int gy)
{
	//m_unibord->show_pck(screen2, 0, 0);
	//int gx = SCREEN2W/2 - 160;
	//int gy = SCREEN2H/2 - 100;
	draw_sprite(screen2, m_unibord, gx, gy);

	//BITMAP *ui = create_bitmap(320, 200);
	//draw_sprite(ui, m_unibord, 0, 0);

	//textout_centre(screen2, font, md.Name, gx+160, gy+7, 66);
	textout_centre(screen2, large, md.Name, gx + 160, gy + 4, xcom1_color(66));

	int fw = ud.HeadWound + ud.TorsoWound + ud.RArmWound +
	         ud.LArmWound + ud.RLegWound + ud.LLegWound;

	struct {
		char *str;
		int cur;
		int max;
		int col;
	}
	param[17] = {
	                {"TIME UNITS", ud.CurTU, ud.MaxTU, 68},
	                {"ENERGY", ud.CurEnergy, ud.MaxEnergy, 148},
	                {"HEALTH", ud.CurHealth, ud.MaxHealth, 36},
	                {"FATAL WOUNDS", fw, fw, 36},
	                {"BRAVERY", md.Bravery, md.Bravery, 196},
	                {"MORALE", ud.Morale, 100, 197},
	                {"REACTIONS", ud.CurReactions, md.Reactions, 20},
	                {"FIRING ACCURACY", ud.CurFAccuracy, ud.MaxFA, 132},
	                {"THROWING ACCURACY", ud.CurTAccuracy, ud.MaxTA, 100},
	                {"STRENGTH", ud.MaxStrength, md.Strength, 52},
	                {NULL, 0, 0, 0},
	                {NULL, 0, 0, 0},
	                {"FRONT ARMOUR", ud.CurFront, ud.MaxFront, 84},
	                {"LEFT ARMOUR", ud.CurLeft, ud.MaxLeft, 84},
	                {"RIGHT ARMOUR", ud.CurRight, ud.MaxRight, 84},
	                {"REAR ARMOUR", ud.CurRear, ud.MaxRear, 84},
	                {"UNDER ARMOUR", ud.CurUnder, ud.MaxUnder, 84}
	            };

	for (int i = 0; i < 17; i++) {
		if (param[i].str != NULL) {
			//textout(screen2, font, param[i].str, gx+8, gy+31+i*10, 50);
			textout(screen2, g_small_font, param[i].str, gx + 8, gy + 31 + i * 10, xcom1_color(50));
			//textprintf(ui, font, 150, 31+i*10, 146, "%d", param[i].val);
			//printsmall(gx+154, gy+32+i*10, 146, param[i].cur);
			textprintf(screen2, g_small_font, gx + 151, gy + 31 + i * 10, xcom1_color(146), "%d", param[i].cur);

			rect(screen2, gx + 170, gy + 32 + i * 10, gx + 170 + param[i].max, gy + 36 + i * 10, xcom1_color(param[i].col));
			if (param[i].cur)
				rectfill(screen2, gx + 170, gy + 33 + i * 10, gx + 170 + param[i].cur - 1, gy + 35 + i * 10, xcom1_color(param[i].col - 4));
		}
	}

	//draw_sprite(screen2, ui, gx, gy);
	//destroy_bitmap(ui);
	//readkey();
}


void Soldier::draw_selector(int select_y)
{
	if (!ismoving()) {
		int sx = map->x + CELL_SCR_X * x + CELL_SCR_X * y + 12;
		int sy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 29 - CELL_SCR_Z * z;
		draw_sprite(screen2, selector, sx, sy - select_y);
	}
}

void Soldier::draw_blue_selector()
{
	if ((FLAGS & F_SELECTENEMY) && map->visible(z, x, y)) {
		int sx = map->x + CELL_SCR_X * x + CELL_SCR_X * y + 12;
		int sy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 29 - CELL_SCR_Z * z;

		//	Draw blue triangle with its point at sx, sy and height 5
		sx += 3; sy += 10; int j;
		for (j = 0; j < 5; j++) {
			line(screen2, sx - j, sy - j, sx + j, sy - j, xcom1_color(256 - 48 + 3));
			putpixel(screen2, sx - j, sy - j, xcom1_color(15));
			putpixel(screen2, sx + j, sy - j, xcom1_color(15));
		}
		line(screen2, sx - j, sy - j, sx + j, sy - j, xcom1_color(15));
	}
}

#define ES_SIDE 15
void Soldier::draw_enemy_seen(int select_y)
{
	text_mode( -1);
	char num[2] = {0, 0};

	for (int i = 0; i < enemy_num; i++) {
		int x1, y1, x2, y2;
		x1 = SCREEN2W - 25;
		y1 = SCREEN2H - 23 - i * 20;

		x2 = x1 + ES_SIDE;
		y2 = y1 + ES_SIDE - 1;
		int color = 39;
		rectfill(screen2, x1, y1, x2, y2, xcom1_color(color));

		num[0] = i + '0';
		textout(screen2, font, num, x1 + 5, y1 + 4, xcom1_color(16));

		//	Draw numbers above seen enemies
		if (FLAGS & F_SELECTENEMY) {
			int sx = map->x + CELL_SCR_X * enemy_x[i] + CELL_SCR_X * enemy_y[i] + 12;
			int sy = map->y - (enemy_x[i] + 1) * CELL_SCR_Y + CELL_SCR_Y * enemy_y[i] - 29 - CELL_SCR_Z * enemy_z[i];

			//	Draw a number over enemy head
			if (i < 10)	textout(screen2, font, num, sx, sy - 2, xcom1_color(1));
		}
	}
}

int Soldier::center_enemy_seen()
{
	for (int i = 0; i < enemy_num; i++) {
		int x1, y1, x2, y2;
		x1 = SCREEN2W - 25;
		y1 = SCREEN2H - 23 - i * 20;

		x2 = x1 + ES_SIDE;
		y2 = y1 + ES_SIDE - 1;

		if (mouse_inside(x1, y1, x2, y2)) {
			map->center(enemy_z[i], enemy_x[i], enemy_y[i]);
			return 1;
		}
	}
	return 0;
}

void Soldier::turnto(int destdir)
{
	int a = destdir - dir;
	if (a != 0) {
		if (a > 0) {
			if (a <= 4) {
				dir++;
			} else {
				dir--;
			}
		} else {
			if (a > -4) {
				dir--;
			} else {
				dir++;
			}
		}
		if (dir < 0) dir = 7;
		if (dir > 7) dir = 0;
	}
}


int Soldier::ismoving()
{
	return ((state == MARCH) || (state == DIE) || (!m_bullet->ready()) ||
	        (curway != -1) || (waylen != 0));
}

void Soldier::calc_visible_cells()
{
	memset(m_visible_cells, 0, sizeof(m_visible_cells));

	int i, j;
	for (i = 0; i < enemy_num; i++) {
		int seen = 0;
		for (j = 0; j < seen_enemy_num; j++) {
			if ((seen_enemy_z[j] == enemy_z[i]) &&
			        (seen_enemy_x[j] == enemy_x[i]) &&
			        (seen_enemy_y[j] == enemy_y[i])) {
				seen = 1;
				break;
			}
		}
		if (!seen) {
			seen_enemy_z[seen_enemy_num] = enemy_z[i];
			seen_enemy_x[seen_enemy_num] = enemy_x[i];
			seen_enemy_y[seen_enemy_num] = enemy_y[i];
			seen_enemy_num++;
		}
	}

	if (is_marching()) {
		enemy_num = map->calc_visible_cells(z, x, y, dir, m_visible_cells, enemy_z, enemy_x, enemy_y);

		for (i = 0; i < enemy_num; i++) {
			int seen = 0;
			for (j = 0; j < seen_enemy_num; j++) {
				if ((seen_enemy_z[j] == enemy_z[i]) &&
				        (seen_enemy_x[j] == enemy_x[i]) &&
				        (seen_enemy_y[j] == enemy_y[i])) {
					seen = 1;
					break;
				}
			}
			if (!seen) {
				break_march();
				return ;
			}
		}
	} else {
		enemy_num = map->calc_visible_cells(z, x, y, dir, m_visible_cells, enemy_z, enemy_x, enemy_y);
	}
}

int Soldier::move(int ISLOCAL) {
	map->check_mine(z,x,y);
	if ((z > 0) && map->mcd(z, x, y, 0)->No_Floor) {
		if (!map->isStairs(z - 1, x, y)) {
			map->set_man(z, x, y, NULL);
			z--;
			if (this == sel_man) {
				//map->center(this);
				map->sel_lev = z;
			}
			if (ISLOCAL) {
				calc_visible_cells();
				VISIBILITY_CHANGED = 1;
			}
		}
	}
	if (map->isStairs(z, x, y)) {
		map->set_man(z, x, y, NULL);
		z++;
		//map->sel_lev = z;
		if (this == sel_man) {
			//map->center(this);
			map->sel_lev = z;
		}
		if (ISLOCAL) {
			calc_visible_cells();
			VISIBILITY_CHANGED = 1;
		}
	}

	map->set_man(z, x, y, this);      //preventor!!
	m_place[P_MAP] = map->place(z, x, y);

	if (state == DIE) {
		//textprintf(screen, font, 1, 100, 1, "phase=%d", phase);
		if (phase < 2 * 3)
			phase++;
		else
			return 0;
	}

	if (state == MARCH) {
		phase++;

		//1) time = time_of_src/2 + time_of_dest/2;
		//2) time = time_of_dest;

		if (phase == 4) {
			map->set_man(z, x, y, NULL);

			x += DIR_DELTA_X(dir);
			y += DIR_DELTA_Y(dir);

			map->set_man(z, x, y, this);
			m_place[P_MAP] = map->place(z, x, y);

			if (ISLOCAL) {
				calc_visible_cells();
				VISIBILITY_CHANGED = 1;
			}
		}

		if (phase >= 8) {
			phase = 0;
			spend_time(walktime( -1));

			curway++;
			if (curway >= waylen) {
				finish_march(ISLOCAL);
			} else {
				if (!havetime(walktime(way[curway])))
					finish_march(ISLOCAL);
				else
					dir = way[curway];
			}
		}
	}

	if ((state == STAND) || (state == SIT)) {
		if (curway != -1) {
			if (dir == way[curway]) {
				if (waylen == 0) {
					curway = -1;
				} else {
					state = MARCH;
					phase = 0;
				}
			} else {
				turnto(way[curway]);
			}

			if (ISLOCAL) {
				calc_visible_cells();
				VISIBILITY_CHANGED = 1;
			}
		} else {
			if (FIRE_num && m_bullet->ready()) {
				FIRE_num--;
				shoot(FIRE_z, FIRE_x, FIRE_y);
			}
		}
	}
	return 1;
}


void Soldier::wayto(int dest_lev, int dest_col, int dest_row)
{
	if ((dest_col != x) || (dest_row != y)) {

	//	!!! Hack to fix synchronization problems with stairs
	//	Should make normal pathfinding code to find paths across stairs
		dest_lev = z;

		curway = 1;
		waylen = map->pathfind(z, x, y, dest_lev, dest_col, dest_row, way);
		if (map->man(dest_lev, dest_col, dest_row) != NULL) {
			waylen--;
		}

		if ((waylen < 2) || (!havetime(walktime(way[1])))) {
			curway = -1;
			waylen = 0;
		} else {
			//net->send_move(NID, dest_lev, dest_col, dest_row);
		}
	}
}


void Soldier::finish_march(int ISLOCAL)
{
	state = STAND;
	curway = -1;
	waylen = 0;
	if (ISLOCAL)
		net->send_move(NID, z, x, y);     //!!!!!!!!!!!!!!!
}


void Soldier::break_march()
{
	//phase = 0;
	//curway = -1;
	waylen = curway - 1;
}


int Soldier::dirto(int src_col, int src_row, int dest_col, int dest_row)
{
	fixed ox = itofix(dest_col - src_col);
	fixed oy = itofix(dest_row - src_row);
	int ang = fixtoi(fatan2(oy, ox));
	if (ang < 0) ang = 256 + ang;
	ang = (ang + 16) % 256;
	return ang >> 5;
}


void Soldier::faceto(int dest_col, int dest_row)
{
	fixed ox = itofix(dest_col - x);
	fixed oy = itofix(dest_row - y);
	if ((!ox) && (!oy)) return ;
	int ang = fixtoi(fatan2(oy, ox));
	if (ang < 0) ang = 256 + ang;
	ang = (ang + 16) % 256;
	//text_mode(-1); textprintf(screen, font, 1, 1, 1, "ang = %d", ang); readkey();

	int nturns = (ang >> 5) - dir;
	if (nturns == 0)
		return ;

	if (nturns < 0) nturns = -nturns;
	if (nturns > 4)
		nturns = 8 - nturns;
	//text_mode(-1); textprintf(screen, font, 1, 1, 1, "nturns = %d", nturns); readkey();
	if (havetime(nturns)) {
		spend_time(nturns);
		net->send_face(NID, dest_col, dest_row);
		way[0] = ang >> 5; curway = 0; waylen = 0;
	} else {
		//info->printstr("___lack of time___\n");
	}
}


void Soldier::hit(int pierce)
{
	if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
	{
		ud.CurHealth = 0;
		if (state != DIE)
		{
			play(S_DIE);
			state = DIE;
			phase = 0;
		}
	}
	else {
		ud.CurHealth -= pierce;
		play(S_WOUND);
	}
}

void Soldier::explo_hit(int pierce) //silent
{
	damage_items(pierce);

	if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
	{
		ud.CurHealth = 0;
		if (state != DIE)
		{
			state = DIE;
			phase = 0;
		}
	}
	else
	{
		ud.CurHealth -= pierce;
	}
}


void Soldier::die()
{
	unlink();
	map->set_man(z, x, y, NULL);

	z = map->find_ground(z, x, y);
	for (int i = 0; i < 8; i++)
		m_place[i]->dropall(z, x, y);

	/////////type of corpse
	int ctype;
	if (md.SkinType == S_XCOM_0)
		ctype = CORPSE;
	else if (md.SkinType == S_XCOM_1)
		ctype = CORPSE_ARMOUR;
	else if ((md.SkinType == S_XCOM_2) || (md.SkinType == S_XCOM_3))
		ctype = CORPSE_POWER_SUIT;
	else
		ctype = Sectoid_Corpse;     //skin_type; //////////aliens!!!!!!!!!!!!!!

	map->place(z, x, y)->put(new Item(ctype));

	char s[100];
	sprintf(s, "%s killed.", md.Name);
	g_console->print(s, xcom1_color(132));
}


void Soldier::unlink()
{
	if (m_prev != NULL) m_prev->m_next = m_next;
	if (m_next != NULL) m_next->m_prev = m_prev;
	m_prev = NULL; m_next = NULL;
}

void Soldier::draw_deselect_times(int sel_item_place)
{
	for (int i = 0; i < 8; i++)
		m_place[i]->draw_deselect_time(i, calctime(sel_item_place, i));
	map->place(z, x, y)->draw_deselect_time(P_MAP, calctime(sel_item_place, P_MAP));
}

void Soldier::damage_items(int damage)
{
	int place_def[8] = { 20, 20, 10, 10, 30, 30, 50, 40 };

	for (int i = 0; i < 8; i++) {
		int def = damage * place_def[i] / 100;
		m_place[i]->damage_items(damage - def);
	}
}

//shl_right 0   //leg_left  5
//shl_left  1   //back     6
//arm_right 2   //belt     7
//arm_left  3   //map       8
//leg_right 4
int Soldier::calctime(int src, int dst)
{
	if (src == dst)
		return 0;

	assert((src >= 0) && (src < 9) &&
	       (dst >= 0) && (dst < 9));

	if (((src == 2) && (dst == 3)) || ((src == 3) && (dst == 2)))
		return 4;
	//  shl  arm  leg   bk bl map
	static int tohand[] = {3, 3, 0, 0, 4, 4, 8, 4, 8};
	static int fromhand[] = {10, 10, 0, 0, 8, 8, 14, 8, 2};

	return tohand[src] + fromhand[dst];
}


Item *Soldier::select_item(int &i)
{
	for (i = 0; i < NUMBER_OF_PLACES; i++) {
		Item *it = m_place[i]->mselect();
		if (it != NULL) {
			return it;
		}
	}
	return NULL;
}


int Soldier::deselect_item(Item *&it, int it_place, int &req_time)
{
	for (int i = 0; i < NUMBER_OF_PLACES; i++) {
		req_time = calctime(it_place, i);
		if (havetime(req_time)) {
			if (m_place[i]->mdeselect(it)) {
				spend_time(req_time);
				return i;
			}
		}
	}
	return -1;
}


Soldier *Soldier::nextman()
{
	if (m_next != NULL) return m_next;
	Soldier *s = this;
	while (s->m_prev != NULL) {
		s = s->m_prev;
	}
	return s;
}


Soldier *Soldier::prevman()
{
	if (m_prev != NULL) return m_prev;
	Soldier *s = this;
	while (s->m_next != NULL) {
		s = s->m_next;
	}
	return s;
}


Item *Soldier::item_under_mouse(int ipl)
{
	if ((ipl >= 0) && (ipl <= 7))
		return m_place[ipl]->item_under_mouse();
	return NULL;
}

int Soldier::haveitem(Item *it)
{
	for (int i = 0; i < 8; i++)
		if (m_place[i]->isthere(it))
			return 1;
	return 0;
}

Place *Soldier::find_item(Item *it, int &lev, int &col, int &row)
{
	for (int i = 0; i < 8; i++)
		if (m_place[i]->isthere(it)) {
			lev = z; col = x; row = y;
			return m_place[i];
		}
	return NULL;
}


int Soldier::place(Place *pl)
{
	for (int i = 0; i < NUMBER_OF_PLACES; i++)
		if (pl == m_place[i])
			return i;
	return -1;
}


int Soldier::open_door()
{
	if (havetime(6)) {
		if (map->open_door(z, x, y, dir)) {
			spend_time(6);
			net->send_open_door(NID);
			return 1;
		}
	}
	return 0;
}

int Soldier::change_pose()
{
	assert((state == SIT) || (state == STAND));

	if (state == SIT) {
		if (!havetime(8)) return 0;
		state = STAND;
		spend_time(8);
	} else {
		if (!havetime(4)) return 0;
		state = SIT;
		spend_time(4);
	}
	net->send_change_pose(NID);
	return 1;

}

int Soldier::prime_grenade(int iplace, int delay_time, int req_time)
{
	assert((iplace == P_ARM_RIGHT) || (iplace == P_ARM_LEFT));
	assert(delay_time > 0);

	if (havetime(req_time)) {
		Item * it = item(iplace);
		assert(it != NULL);

		elist->add(it, delay_time);

		spend_time(req_time);
		net->send_prime_grenade(NID, iplace, delay_time, req_time);
		return 1;
	}
	return 0;
}


int Soldier::unload_ammo(Item * it)
{
	if ((it == NULL) || (!it->haveclip()))
		return 0;
	if ((rhand_item() != NULL) || (lhand_item() != NULL))
		return 0;

	if (havetime(8)) {
		putitem(it, P_ARM_RIGHT);
		putitem(it->unload(), P_ARM_LEFT);

		spend_time(8);
		net->send_unload_ammo(NID);
		return 1;
	}
	return 0;
}


int Soldier::load_ammo(int iplace, Item * it)
{
	if (it == NULL)
		return 0;

	if (!havetime(15)) return 0;

	Item *gun = item(iplace);
	if (gun == NULL)
		return 0;
	if (!gun->loadclip(it))
		return 0;

	spend_time(15);
	net->send_load_ammo(NID, iplace);
	return 1;
}

void Soldier::spend_time(int tm)
{
	ud.CurTU -= tm;

	if (FLAGS & F_ENDLESS_TU) {
		if (ud.CurTU < 32) ud.CurTU = ud.MaxTU;
	}
}

int Soldier::havetime(int ntime) // !!! do check of energy & others
{
	return (ud.CurTU >= ntime);
}

int Soldier::walktime(int _dir)
{
	int dz = z, dx = x, dy = y;
	if (_dir != -1) {
		dx += DIR_DELTA_X(_dir);
		dy += DIR_DELTA_Y(_dir);
	}
	int time_of_dst = map->mcd(dz, dx, dy, 0)->TU_Walk;
	time_of_dst += map->mcd(dz, dx, dy, 3)->TU_Walk;

	return time_of_dst;
}


// ud.MaxTU  100%
//   x         ntime
int Soldier::required(int pertime)
{
	return (ud.MaxTU * pertime) / 100;
}


int Soldier::FAccuracy(int peraccur, int TWOHAND)
{
	int ac = (ud.CurFAccuracy * peraccur) / 100;
	ac -= (ac * (ud.MaxHealth - ud.CurHealth)) / ud.MaxHealth / 2;

	if (TWOHAND) {
		if ((rhand_item() != NULL) && (lhand_item() != NULL))
			ac -= ac / 3;
	}

	if (state == SIT) ac += ac / 20;

	return ac;
}

int Soldier::TAccuracy(int peraccur)
{
	int ac = (ud.CurTAccuracy * peraccur) / 100;
	ac = (ac * ud.CurHealth) / ud.MaxHealth;
	return ac;
}

void Soldier::apply_accuracy(REAL & fi, REAL & te)
{
	REAL TE_STEP = (PI / 8. / 100.0) / 2.0;
	REAL FI_STEP = (PI / 32. / 100.0) / 2.0;

	int randmax = 100 - target.accur;
	if (randmax <= 0) randmax = 1;

//	According to central limit theorem, the sum of many small random values
//	is normally distributed
	for (int i = 0; i < 16; i++) {
		REAL rand_te = (REAL)(rand() % randmax);
		REAL rand_fi = (REAL)(rand() % randmax);
		te += TE_STEP * rand_te - TE_STEP * (randmax - 1) / 2.0;
		fi += FI_STEP * rand_fi - FI_STEP * (randmax - 1) / 2.0;
	}
}


void Soldier::apply_throwing_accuracy(REAL &fi, REAL &te, int weight)
{
	REAL TE_STEP = (PI / 8 / 30.0);
	REAL FI_STEP = (PI / 32 / 30.0);

	int randmax = 100 - target.accur;
	if (randmax <= 0) randmax = 1;

	REAL rand_te = (REAL)(rand() % randmax);
	REAL rand_fi = (REAL)(rand() % randmax);
	te += TE_STEP * rand_te - TE_STEP * randmax / 2.0;
	fi += FI_STEP * rand_fi - FI_STEP * randmax / weight;
}


int Soldier::check_for_hit(int _z, int _x, int _y)
{
	int lev = _z / 12;
	int col = _x / 16;
	int row = _y / 16;

	if ((lev == z) && (col == x) && (row == y)) {
		//return 1;
		lev = _z % 12;
		col = _x % 16;
		row = _y % 16;
		int s = 0;
		if (state == SIT)
			s = 1;

		//lev -= map->mcd(z, x, y, 0)->T_Level; //!!!
		//lev -= map->mcd(z, x, y, 3)->T_Level;
		//if (lev < 0) lev = 0;

		if (m_bof[s][dir][lev][col][15 - row])
			return 1;
	}

	return 0;
}


void Soldier::apply_hit(int _z, int _x, int _y, int _wtype)
{
	if (check_for_hit(_z, _x, _y)) {
		hit(Item::obdata[_wtype].damage);
	}
}

void Soldier::precise_aiming()
{
	BITMAP *bmp = map->create_lof_bitmap(map->sel_lev, map->sel_col, map->sel_row);
	BITMAP *bmp_back = create_bitmap(bmp->w, bmp->h);
	int mx = mouse_x, my = mouse_y;
	blit(screen, bmp_back, mx, my, 0, 0, bmp->w, bmp->h);
	blit(bmp, screen, 0, 0, mx, my, bmp->w, bmp->h);
	int z = 8, x = 8, y = 8;

	int mouse_leftr = 0, mouse_rightr = 0;
	if (!(mouse_b & 1)) mouse_leftr = 1;
	if (!(mouse_b & 2)) mouse_rightr = 1;

	set_mouse_range(mx + 1, my + 1, mx + bmp->w - 1, my + bmp->h - 1);
	show_mouse(screen);
	text_mode(0);
	while (!keypressed()) {
		if (CHANGE) {
			int sx = (mouse_x - mx) / 20;
			int sy = 2 - (mouse_y - my) / 20;
			z = sx * 3 + sy;

			x = (mouse_y - my) % 20 - 3; if (x < 0) x = 0; if (x > 15) x = 15;
			y = (mouse_x - mx) % 20 - 3; if (y < 0) y = 0; if (y > 15) y = 15;

			//textprintf(screen, font, mx, my-10, 1, "z=%02d x=%02d y=%02d", z, x, y);
			//textprintf(screen, font, mx, my-10, 1, "sx=%d sy=%d", sx, sy);
			CHANGE = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			mouse_leftr = 0;

			break;
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			z = 8;
			x = 8;
			y = 6;
			break;
		}

		if (!(mouse_b & 1)) mouse_leftr = 1;
		if (!(mouse_b & 2)) mouse_rightr = 1;
	}

	FIRE_z = map->sel_lev * 12 + z;
	FIRE_x = map->sel_col * 16 + x;
	FIRE_y = map->sel_row * 16 + y;

	show_mouse(NULL);
	set_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
	blit(bmp_back, screen, 0, 0, mx, my, bmp->w, bmp->h);
	destroy_bitmap(bmp_back);
	destroy_bitmap(bmp);
}

void Soldier::try_shoot()
{
	// Moving soldier cannot shoot
	if (ismoving()) return ;

	if ((z == map->sel_lev) && (x == map->sel_col) && (y == map->sel_row)) return ;

	if (FIRE_num != 0) return ;

	// In melee you have to be near your target
	if (target.action == PUNCH) {
		if ((z != map->sel_lev) || (abs(x - map->sel_col) > 1) || (abs(y - map->sel_row) > 1))
			return ;
	}

	if (target.action == AUTOSHOT)
		FIRE_num = 3;
	else
		FIRE_num = 1;

	if ((key[KEY_LSHIFT]) && (target.action != THROW)) // && (target.action != PUNCH))
	{
		precise_aiming();
	} else {
		FIRE_z = map->sel_lev * 12 + 8;
		FIRE_x = map->sel_col * 16 + 8;
		FIRE_y = map->sel_row * 16 + 8;
	}

	// There are many reasons why you cannot shoot infinitely ;)
	if (!havetime(target.time * 2 * FIRE_num)) {
		TARGET = 0;
	}

	if (!target.item->is_laser() && !target.item->is_cold_weapon() &&
	        ((target.item->ammo == NULL) || (target.item->ammo->rounds < FIRE_num + 1))) {
		TARGET = 0;
	}

	if (target.action != SNAPSHOT && target.action != AIMEDSHOT &&
	        target.action != AUTOSHOT && target.action != PUNCH) {
		TARGET = 0;
	}

	// Face to target
	faceto(map->sel_col, map->sel_row);
}


void Soldier::shoot(int zd, int xd, int yd)
{
	assert(target.action != NONE);
	assert(target.item != NULL);

	int z0 = z * 12 + 8; if (state == SIT) z0 -= 4;
	int x0 = x * 16 + 8;
	int y0 = y * 16 + 8;

	if (target.action == THROW) {
		zd -= 8;
		REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));

		int ro_real = m_bullet->calc_throw(z0, x0, y0, zd, xd, yd);
		if (ro_real > 18 * 16) {
			TARGET = 1; FIRE_num = 0;
			return ;
		}

		REAL fi = acos((REAL)(zd - z0) / ro);
		REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
		REAL zA = sqrt(ro);
		//apply_accuracy(fi, te);
		apply_throwing_accuracy(fi, te, target.item->data()->weight);

		thru(z0, x0, y0, ro, fi, te, zA, target.place, target.time);
	} else if (target.action == AIMEDTHROW) {
		REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
		REAL fi = acos((REAL)(zd - z0) / ro);
		REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
		apply_accuracy(fi, te);

		aimedthrow(z0, x0, y0, fi, te, target.place, target.time);
	} else if (target.action == PUNCH) {
		REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
		REAL fi = acos((REAL)(zd - z0) / ro);
		REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
		apply_accuracy(fi, te);

		punch(z0, x0, y0, fi, te, target.place, target.time);
	} else {
		if (target.item->is_laser()) {
			REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
			REAL fi = acos((REAL)(zd - z0) / ro);
			REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
			apply_accuracy(fi, te);

			beam(z0, x0, y0, fi, te, target.place, target.time);
		} else {
			REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
			REAL fi = acos((REAL)(zd - z0) / ro);
			REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
			apply_accuracy(fi, te);

			fire(z0, x0, y0, fi, te, target.place, target.time);
		}
	}
}


int Soldier::punch(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;
	spend_time(req_time);

	Item *it = item(iplace);
	if (it == NULL)
		return 0;

	m_bullet->punch(z0, x0, y0, fi, te, it->type);
	net->send_punch(NID, z0, x0, y0, fi, te, iplace, req_time);
	return 1;
}


int Soldier::thru(int z0, int x0, int y0, REAL ro, REAL fi, REAL te, REAL zA, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;
	spend_time(req_time);

	Item *it = item(iplace);
	if (it == NULL)
		return 0;
	m_place[iplace]->set_item(NULL);

	m_bullet->thru(z0, x0, y0, ro, fi, te, zA, it);
	net->send_thru(NID, z0, x0, y0, ro, fi, te, zA, iplace, req_time);
	return 1;
}


int Soldier::aimedthrow(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;
	spend_time(req_time);

	Item *it = item(iplace);
	if (it == NULL)
		return 0;
	m_place[iplace]->set_item(NULL);

	m_bullet->aimedthrow(z0, x0, y0, fi, te, it);
	net->send_aimedthrow(NID, z0, x0, y0, fi, te, iplace, req_time);
	return 1;
}


int Soldier::beam(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;
	spend_time(req_time);

	Item *it = item(iplace);
	if (it == NULL)
		return 0;

	m_bullet->beam(z0, x0, y0, fi, te, it->type);
	net->send_beam(NID, z0, x0, y0, fi, te, iplace, req_time);
	return 1;
}


int Soldier::fire(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;
	spend_time(req_time);

	Item *it = item(iplace);
	if ((it == NULL) || (!it->haveclip()) || (it->roundsremain() < 1))
		return 0;

	m_bullet->fire(z0, x0, y0, fi, te, it->cliptype());
	it->shot();

	net->send_fire(NID, z0, x0, y0, fi, te, iplace, req_time);
	return 1;
}


void Soldier::draw_bullet_way()
{
	if (target.action == THROW)
		m_bullet->showthrow(z, x, y, map->sel_lev, map->sel_col, map->sel_row);
	else
		m_bullet->showline(z, x, y, map->sel_lev, map->sel_col, map->sel_row);
}


void Soldier::showspk()
{
	switch (md.SkinType) {
		case S_XCOM_0:
		case S_XCOM_1:
			m_spk[(md.SkinType - 1) % 3][md.fFemale][md.Appearance]->show(screen2, 0, 0);
			break;
		case S_XCOM_2:
		case S_XCOM_3:
			m_spk[(md.SkinType - 1) % 3][0][0]->show(screen2, 0, 0);
			break;
		default:
			bigobs->showpck(Item::obdata[md.SkinType].pInv, 67, 70);
	}
}


void Soldier::drawbar(int col1, int col2, int x2, int y2, int val, int valmax)
{
	hline(screen2, x2, y2, x2 + valmax + 1, col1);
	hline(screen2, x2, y2 + 1, x2 + val, col2); putpixel(screen2, x2 + valmax + 1, y2 + 1, col1);
	hline(screen2, x2, y2 + 2, x2 + valmax + 1, col1);
}


void Soldier::drawinfo(int x, int y)
{
	if (rhand_item() != NULL) {
		int it_width = rhand_item()->data()->width;
		int it_height = rhand_item()->data()->height;
		int dx = (2 - it_width) * 16 / 2;
		int dy = (3 - it_height) * 15 / 2;

		bigobs->showpck(rhand_item()->data()->pInv, x + 280 + dx, y + 10 + dy);
		if (rhand_item()->clip() != NULL)
			printsmall(x + 304, y + 47, xcom1_color(1), rhand_item()->roundsremain());
		if (rhand_item()->is_grenade()) {
			if (rhand_item()->delay_time())
				printsmall(x + 304, y + 47, xcom1_color(36), rhand_item()->delay_time() - 1);
		}
	}
	if (lhand_item() != NULL) {
		int it_width = lhand_item()->data()->width;
		int it_height = lhand_item()->data()->height;
		int dx = (2 - it_width) * 16 / 2;
		int dy = (3 - it_height) * 15 / 2;

		bigobs->showpck(lhand_item()->data()->pInv, x + 8 + dx, y + 10 + dy);
		if (lhand_item()->clip() != NULL)
			printsmall(x + 33, y + 47, xcom1_color(1), lhand_item()->roundsremain());
		if (lhand_item()->is_grenade()) {
			if (lhand_item()->delay_time())
				printsmall(x + 33, y + 47, xcom1_color(36), lhand_item()->delay_time() - 1);
		}
	}
	textout(screen2, g_small_font, md.Name, x + 134, y + 32, xcom1_color(130));


	drawbar(71, 64, 170 + x, 41 + y, ud.CurTU, ud.MaxTU);
	//textprintf(screen2, font, x+136, y+41, 64, "%d", ud.CurTU);
	printsmall(x + 136, y + 42, xcom1_color(64), ud.CurTU);

	drawbar(21, 16, 170 + x, 45 + y, ud.CurEnergy, ud.MaxEnergy);
	//textprintf(screen2, font, x+154, y+41, 16, "%d", ud.CurEnergy);
	printsmall(x + 154, y + 42, xcom1_color(16), ud.CurEnergy);

	drawbar(37, 32, 170 + x, 49 + y, ud.CurHealth, ud.MaxHealth);
	//textprintf(screen2, font, x+136, y+49, 32, "%d", ud.CurHealth);
	printsmall(x + 136, y + 50, xcom1_color(32), ud.CurHealth);

	drawbar(197, 192, 170 + x, 53 + y, ud.Morale, 100);
	//textprintf(screen2, font, x+154, y+49, 192, "%d", ud.Morale);
	printsmall(x + 154, y + 50, xcom1_color(192), ud.Morale);
}

/*
void Soldier::show_MANDATA(int gx, int gy, int gcol)
{
	text_mode( -1);
	textprintf(screen2, font, gx, gy, gcol,
	           "ArmorType=%d Name=%s",
	           md.ArmorType, md.Name);
	textprintf(screen2, font, gx, gy + 10, gcol,
	           "TimeUnits=%d Health=%d Stamina=%d",
	           md.TimeUnits, md.Health, md.Stamina);
	textprintf(screen2, font, gx, gy + 20, gcol,
	           "Strength=%d Reactions=%d Kills=%d",
	           md.Strength, md.Reactions, md.Kills);
	textprintf(screen2, font, gx, gy + 30, gcol,
	           "Firing=%d Throwing=%d Close=%d",
	           md.Firing, md.Throwing, md.Close);
}
*/
int Soldier::eot_save(char *txt)
{
	int len = 0;
	len += sprintf(txt + len,
		"\r\n%s:\r\nNID=%d z=%d x=%d y=%d dir=%d state=%d ud.MaxTU=%d ud.MaxHealth=%d ud.MaxEnergy=%d ud.CurFAccuracy=%d ud.CurTAccuracy=%d ud.CurTU=%d ud.CurHealth=%d ud.CurEnergy=%d ud.Morale=%d\r\n",
		md.Name, NID, z, x, y, dir, (int)state, ud.MaxTU, ud.MaxHealth, ud.MaxEnergy, ud.CurFAccuracy, ud.CurTAccuracy, ud.CurTU, ud.CurHealth, ud.CurEnergy, ud.Morale);
	for (int i = 0; i < 8; i++) {
		len += m_place[i]->eot_save(i, txt + len);
	}
	return len;
}


bool Soldier::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, m_next);
	PersistWriteObject(archive, m_prev);
	PersistWriteObject(archive, m_bullet);

	for (int i = 0; i < NUMBER_OF_PLACES; i++)
		PersistWriteObject(archive, m_place[i]);

	return true;
}

bool Soldier::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, m_next);
	PersistReadObject(archive, m_prev);
	PersistReadObject(archive, m_bullet);

	for (int i = 0; i < NUMBER_OF_PLACES; i++)
		PersistReadObject(archive, m_place[i]);

	return true;
}
