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
#include "config.h"

SKIN_INFO g_skins[] =
{
	{ "male",    S_XCOM_0,  0, {  5,  5,  5,  5,  5} },
	{ "female",  S_XCOM_0,  1, {  5,  5,  5,  5,  5} },
	{ "armour_m",S_XCOM_1,  0, { 50, 40, 40, 30, 30} },
	{ "armour_f",S_XCOM_1,  1, { 50, 40, 40, 30, 30} },
	{ "power_m", S_XCOM_2,  0, {100, 80, 80, 70, 60} },
	{ "power_f", S_XCOM_2,  1, {100, 80, 80, 70, 60} },
	{ "fly_m",   S_XCOM_3,  0, {110, 90, 90, 80, 70} },
	{ "fly_f",   S_XCOM_3,  1, {110, 90, 90, 80, 70} },
	{ "sectoid", S_SECTOID, 0, {  4,  3,  3,  2,  2} },
	{ "muton",   S_MUTON,   0, { 20, 20, 20, 20, 10} }
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
SPK *Soldier::m_spk[6][2][4] = {{{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
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

	m_spk[4][0][0] = new SPK("ufo2000.dat#SECTOID_SPK");
	m_spk[5][0][0] = new SPK("ufo2000.dat#MUTON_SPK");

	SPK *unibord = new SPK("ufograph/unibord.pck");
	m_unibord = create_bitmap(320, 200); clear_to_color(m_unibord, xcom1_color(15));
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
	delete m_spk[4][0][0];
	delete m_spk[5][0][0];

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

Soldier::Soldier(Platoon *platoon, int _NID)
{
	NID = _NID;
	z = -1; x = -1; y = -1;
	dir = 0; phase = 0; m_state = STAND;
	m_next = NULL; m_prev = NULL;
	m_body = NULL;
	m_platoon = platoon;

	m_bullet = new Bullet(this);
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


Soldier::Soldier(Platoon *platoon, int _NID, int _z, int _x, int _y)
{
	NID = _NID; z = _z; x = _x; y = _y;
	dir = 0;      //!!face to center of map
	phase = 0; m_state = STAND;
	m_next = NULL; m_prev = NULL;
	m_body = NULL;
	m_platoon = platoon;

	m_bullet = new Bullet(this);
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


Soldier::Soldier(Platoon *platoon, int _NID, int _z, int _x, int _y, MANDATA *mdat, ITEMDATA *idat)
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

	phase = 0; m_state = STAND;
	m_next = NULL; m_prev = NULL;
	m_body = NULL;
	m_platoon = platoon;

	m_bullet = new Bullet(this);
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

	if (md.TimeUnits < 50) md.TimeUnits = 50;
	if (md.TimeUnits > 80) md.TimeUnits = 80;
	if (md.Health < 50) md.Health = 50;
	if (md.Health > 80) md.Health = 80;
	if (md.Firing < 50) md.Firing = 50;
	if (md.Firing > 80) md.Firing = 80;
	if (md.Throwing < 50) md.Throwing = 50;
	if (md.Throwing > 80) md.Throwing = 80;
	if (md.Stamina < 50) md.Stamina = 50;
	if (md.Stamina > 80) md.Stamina = 80;
	if (md.Strength < 25) md.Strength = 25;
	if (md.Strength > 40) md.Strength = 40;
	if (md.Reactions < 50) md.Reactions = 50;
	if (md.Reactions > 80) md.Reactions = 80;

	if (md.TimeUnits + md.Health + md.Firing + md.Throwing + md.Stamina + (md.Strength * 2) + md.Reactions > MAXPOINTS) {
		md.TimeUnits = 50;
		md.Health    = 50;
		md.Firing    = 50;
		md.Throwing  = 50;
		md.Stamina   = 50;
		md.Reactions = 50;
		md.Strength  = 25;
	}

	strcpy(ud.Name, md.Name);
	ud.MaxTU = md.TimeUnits;
	ud.MaxHealth = md.Health;
	ud.MaxStrength = md.Strength;
	ud.MaxEnergy = md.Stamina;

//	Verify that md.SkinType and md.fFemale have valid values
	int skin_index = get_skin_index(md.SkinType, md.fFemale);
	md.SkinType = g_skins[skin_index].SkinType;
	md.fFemale  = g_skins[skin_index].fFemale;

	ud.MaxFA = md.Firing;
	ud.MaxTA = md.Throwing;
	ud.Morale = 100;
	ud.HeadWound = 0;
	ud.TorsoWound = 0;
	ud.RArmWound = 0;
	ud.LArmWound = 0;
	ud.RLegWound = 0;
	ud.LLegWound = 0;

	// Set the armour values based on the skin.
	ud.MaxFront = g_skins[skin_index].armour_values[0];
	ud.MaxLeft = g_skins[skin_index].armour_values[1];
	ud.MaxRight = g_skins[skin_index].armour_values[2];
	ud.MaxRear = g_skins[skin_index].armour_values[3];
	ud.MaxUnder = g_skins[skin_index].armour_values[4];

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
	// TUs with stamina as they determine how far you go in a turn.
	// TUs twice as important because they get used up twice as fast.
	// Reactions is doubled because it means doing damage in enemy's turn!
	// Strength is two for one points wise, so double the value that's in there.
	// Accuracy averaged into one value.
	// The amount of armour we have matters, too!
	int p = (((md.TimeUnits * 2) + md.Stamina) / 2) +
	        md.Health +
			(md.Reactions * 2) +
	        ((md.Firing + md.Throwing) / 2) +
			(md.Strength * 2) +
			g_skins[get_skin_index(md.SkinType, md.fFemale)].armour_values[0];

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
	// Percent of TUs: the lesser of 100% or ((strength / weight) * 100%).
	ud.CurTU = (count_weight() > ud.CurStrength) ? (ud.MaxTU * ud.CurStrength / count_weight()) : ud.MaxTU;
	//ud.CurHealth = ud.MaxHealth;
	//ud.CurStrength = ud.MaxStrength;
	// Regain 33% of energy per turn. Should this be 43%?
	ud.CurEnergy = ((ud.CurEnergy + (ud.MaxEnergy / 3)) >= ud.MaxEnergy) ? ud.MaxEnergy : ud.CurEnergy + (ud.MaxEnergy / 3);
	//ud.Morale = 100;
	//ud.CurFAccuracy = ud.MaxFA;
	//ud.CurTAccuracy = ud.MaxTA;

	if (ud.CurStun > 0) // Do we have stun damage?
	{
		int i = 5; //(int)randval(1, 11);

		// Are we currently stunned?
		bool was_stunned = is_stunned();

        // Reduce stun damage
		if (ud.CurStun > i)
			ud.CurStun -= i;
		else
			ud.CurStun = 0;

		// This means we should wake up, so find the body!
		if (was_stunned && !is_stunned()) {
			int z0 = -1, x0 = -1, y0 = -1, found = 0;
			Place *target = m_body->get_place();
			for (z0 = 0; z0 < map->level; z0++) {
				for (x0 = 0; x0 < map->width*10; x0++) {
					for (y0 = 0; y0 < map->height*10; y0++) {
						if (target == map->place(z0, x0, y0))
						{
							found = 1;
							break;
						}
						if (found)
							break;
					}
					if (found)
						break;
				}
				if (found)
					break;
			}
			// Body found on map (not in someone's backback) and also
			// nobody is standing at this place.
			if (found && map->man(z0, x0, y0) == NULL) {
				z = z0;
				x = x0;
				y = y0;
				m_body->unlink();
				delete m_body;
				m_body = NULL;
				map->set_man(z0, x0, y0, this); // Get back into action.
				m_state = STAND;
				phase = 0;
			}
		}
	}
}

#define Y_SIT 5
#define MAPX 0
#define MAPY (-18)

void Soldier::draw()
{
	int head_frame = 32;

	// The female appearance of the power suit is the flying suit.
	// This icky-looking switch causes the proper suit to be displayed regardless of gender.
	switch (md.SkinType)
	{
		default:
			if (!md.fFemale) break;
		case S_XCOM_3:
			head_frame = 16 * 16 + 11;
		case S_XCOM_2:
			break;
	}

	int gx = map->x + CELL_SCR_X * x + CELL_SCR_X * y;
	int gy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
	if ((z > 0) && map->isStairs(z - 1, x, y)) {
		gy += CELL_SCR_Z + map->mcd(z - 1, x, y, 3)->T_Level;
	} else {
		gy += map->mcd(z, x, y, 0)->T_Level;
		gy += map->mcd(z, x, y, 3)->T_Level;
	}


	if (m_state == FALL) {
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

//	Aliens do not crouch
	State state = m_state;
	if (state == SIT && (md.SkinType == S_SECTOID || md.SkinType == S_MUTON))
		state = STAND;

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
		case FALL: case LIE: break;      //neverhap
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

			// special case for the health bar
			if (i == 2) // draw stun damage
				if (ud.CurStun > 0)
				{
					if (ud.CurStun < ud.CurHealth)
						rectfill(screen2, gx + 170, gy + 33 + i * 10, gx + 170 + ud.CurStun - 1, gy + 35 + i * 10, xcom1_color(80));
					else
						rectfill(screen2, gx + 170, gy + 33 + i * 10, gx + 170 + ud.CurHealth - 1, gy + 35 + i * 10, xcom1_color(80));
				}
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

		// Start at 1 instead of 0.
		num[0] = ((i + 1) % 10) + '0';
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
	return ((m_state == MARCH) || (m_state == FALL) || (!m_bullet->ready()) ||
	        (curway != -1) || (waylen != 0));
}

void Soldier::calc_visible_cells()
{
	m_platoon->set_visibility_changed();
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

	enemy_num = map->calc_visible_cells(this, z, x, y, dir, m_visible_cells, enemy_z, enemy_x, enemy_y);

	if (is_marching()) {
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
	}
}

int Soldier::move(int ISLOCAL)
{
	if (z == -1) return 0; // auto-return 0 if stunned

	if ((z > 0) && map->mcd(z, x, y, 0)->No_Floor) {
		if (!map->isStairs(z - 1, x, y)) {
			map->set_man(z, x, y, NULL);
			z--;
			if (this == sel_man) {
				//map->center(this);
				map->sel_lev = z;
			}
			calc_visible_cells();
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
		calc_visible_cells();
	}

	map->set_man(z, x, y, this);      //preventor!!
	m_place[P_MAP] = map->place(z, x, y);

	if (m_state == FALL) {
		//textprintf(screen, font, 1, 100, 1, "phase=%d", phase);
		if (phase < 2 * 3) {
			phase++;
		} else {
			phase = 0;
			m_state = LIE;
			return 0;
		}
	}

	if (m_state == MARCH) {
		
		if ((phase == 3 || phase == 7) && map->visible(z, x, y)) {
			// Make some step sounds (twice per movement from one cell to another)
			switch (md.SkinType) {
				case S_SECTOID:
					soundSystem::getInstance()->play(SS_STEP_SECTOID, 128);
					break;
				case S_MUTON:
					soundSystem::getInstance()->play(SS_STEP_MUTON, 128);
					break;
				default:
					soundSystem::getInstance()->play(SS_STEP_HUMAN, 128);
					break;
			}
		}
		phase++;

		if (phase == 4) {
			// We are axactly in the middle between map cells
			map->set_man(z, x, y, NULL);

			x += DIR_DELTA_X(dir);
			y += DIR_DELTA_Y(dir);

			// If we're moving along a diagonal, use 1.5 times the cost, as 
			// in the original game itself. Please note that walktime(-1) 
			// returns the time of a horizontal move, whereas walktime(dir)
			// factors in the diagonal move multiplier.
			if (DIR_DIAGONAL(dir))
				spend_time((walktime(-1) * 3 / 2), 1);
			else
				spend_time(walktime(-1), 1);

			map->set_man(z, x, y, this);
			m_place[P_MAP] = map->place(z, x, y);

			calc_visible_cells();
		}

		if (phase >= 8) {
			// We have just come to another map cell
			phase = 0;

			// Check for proximity grenades
			if (map->check_mine(z, x, y)) {
				finish_march(ISLOCAL);
				if (!is_active()) return 0;
			}

			// Check for reaction fire.
			if (ISLOCAL) {
				if (platoon_remote->check_reaction_fire(this)) {
					// In other words, if more than 0 shots were fired.
					finish_march(ISLOCAL);
				}
			}

			if (m_state == MARCH) {
				// We haven't stopped because of proximity mines and reaction 
				// fire, so continue marching until we reach final destination
				curway++;
				if (curway >= waylen) {
					finish_march(ISLOCAL);
				} else {
					if (!havetime(walktime(way[curway]), 1))
						finish_march(ISLOCAL);
					else
						dir = way[curway];
				}
			}
		}
	}

	if ((m_state == STAND) || (m_state == SIT)) {
		if (curway != -1) {
			if (dir == way[curway]) {
				if (waylen == 0) {
					curway = -1;
				} else {
					m_state = MARCH;
					phase = 0;
				}
			} else {
				turnto(way[curway]);
			}

			calc_visible_cells();
		} else {
			if (FIRE_num && m_bullet->ready()) {
				FIRE_num--;
				// The ISLOCAL bit is for reaction fire...
				shoot(FIRE_z, FIRE_x, FIRE_y, ISLOCAL);
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

		if ((waylen < 2) || (!havetime(walktime(way[1]), 1))) {
			curway = -1;
			waylen = 0;
		} else {
			//net->send_move(NID, dest_lev, dest_col, dest_row);
		}
	}
}


void Soldier::finish_march(int ISLOCAL)
{
	if (is_active())
		m_state = STAND;
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

static double randval(double min, double max)
{
	double std = (double)rand() / (double)(RAND_MAX - 1);
	return min + std * (max - min);
}

#define HITLOC_HEAD     0
#define HITLOC_TORSO    1
#define HITLOC_LEFTARM  2
#define HITLOC_RIGHTARM 3
#define HITLOC_LEFTLEG  4
#define HITLOC_RIGHTLEG 5

#define DAMAGEDIR_FRONT      0
#define DAMAGEDIR_FRONTLEFT  1
#define DAMAGEDIR_LEFT       2
#define DAMAGEDIR_REARLEFT   3
#define DAMAGEDIR_REAR       4
#define DAMAGEDIR_REARRIGHT  5
#define DAMAGEDIR_RIGHT      6
#define DAMAGEDIR_FRONTRIGHT 7
#define DAMAGEDIR_UNDER      8

// I can't believe that pierce is passed by reference here.
// But, how else am I going to do this?
int Soldier::do_armour_check(int &pierce, int damdir)
{
	int hitloc;
	int damagedir = damdir;
	int pierce1 = (pierce / 2), pierce2 = (pierce / 2);

	switch(damagedir)
	{
	case DAMAGEDIR_FRONT:
		if (ud.CurFront >= pierce)
		{
			ud.CurFront -= pierce; // Can't get through the armour.
			return -1;
		}

		pierce -=  ud.CurFront;
		ud.CurFront = 0;

		// Until we can get the random values transmitted over the net, we can't use 'em.
/*		i = (int)randval(0, 4);
		if (i >= 3)
			hitloc = HITLOC_HEAD;
		else */
			hitloc = HITLOC_TORSO;
		break;
	case DAMAGEDIR_FRONTLEFT:
		// Split this evenly between the two pieces of armour.
		if (ud.CurFront >= pierce1)
		{
			ud.CurFront -= pierce1;
			pierce1 = 0;
		}
		else
		{
			pierce1 -= ud.CurFront;
			ud.CurFront = 0;
		}
		if (ud.CurLeft >= pierce2)
		{
			ud.CurLeft -= pierce2;
			pierce2 = 0;
		}
		else
		{
			pierce2 -= ud.CurLeft;
			ud.CurLeft = 0;
		}
		if ((pierce1 + pierce2) == 0) return -1; // Can't get through the armor.

		// Pick a random location when the random values can be transmitted.
		hitloc = HITLOC_LEFTARM;
		break;
	case DAMAGEDIR_LEFT:
		if (ud.CurLeft >= pierce)
		{
			ud.CurLeft -= pierce; // Can't get through the armour.
			return -1;
		}

		pierce -=  ud.CurLeft;
		ud.CurLeft = 0;

/*		i = (int)randval(0, 4);
		switch(i)
		{
		default:
		case 0:
			hitloc = HITLOC_TORSO;
			break;
		case 1:
			hitloc = HITLOC_LEFTARM;
			break;
		case 2: */
			hitloc = HITLOC_LEFTLEG;
/*			break;
		case 3:
			hitloc = HITLOC_HEAD;
			break;
		} */
		break;
	case DAMAGEDIR_REARLEFT:
		// Split this evenly between the two pieces of armour.
		if (ud.CurRear >= pierce1)
		{
			ud.CurRear -= pierce1;
			pierce1 = 0;
		}
		else
		{
			pierce1 -= ud.CurRear;
			ud.CurRear = 0;
		}
		if (ud.CurLeft >= pierce2)
		{
			ud.CurLeft -= pierce2;
			pierce2 = 0;
		}
		else
		{
			pierce2 -= ud.CurLeft;
			ud.CurLeft = 0;
		}
		if ((pierce1 + pierce2) == 0) return -1; // Can't get through the armour.

		// Pick a random location when the random values can be transmitted.
		hitloc = HITLOC_LEFTARM;
		break;
	case DAMAGEDIR_REAR:
		if (ud.CurRear >= pierce)
		{
			ud.CurRear -= pierce; // Can't get through the armour.
			return -1;
		}

		pierce -=  ud.CurRear;
		ud.CurRear = 0;

		// Until we can get the random values transmitted over the net, we can't use 'em.
/*		i = (int)randval(0, 4);
		if (i >= 3)
			hitloc = HITLOC_HEAD;
		else */
			hitloc = HITLOC_TORSO;
		break;
	case DAMAGEDIR_REARRIGHT:
		// Split this evenly between the two pieces of armour.
		if (ud.CurRear >= pierce1)
		{
			ud.CurRear -= pierce1;
			pierce1 = 0;
		}
		else
		{
			pierce1 -= ud.CurRear;
			ud.CurRear = 0;
		}
		if (ud.CurRight >= pierce2)
		{
			ud.CurRight -= pierce2;
			pierce2 = 0;
		}
		else
		{
			pierce2 -= ud.CurRight;
			ud.CurLeft = 0;
		}
		if ((pierce1 + pierce2) == 0) return -1; // Can't get through the armuor.

		// Pick a random location when the random values can be transmitted.
		hitloc = HITLOC_RIGHTARM;
		break;
	case DAMAGEDIR_RIGHT:
		if (ud.CurRight >= pierce)
		{
			ud.CurRight -= pierce; // Can't get through the armour.
			return -1;
		}

		pierce -=  ud.CurRight;
		ud.CurRight = 0;

/*		i = (int)randval(0, 4);
		switch(i)
		{
		default:
		case 0:
			hitloc = HITLOC_TORSO;
			break;
		case 1:
			hitloc = HITLOC_RIGHTARM;
			break;
		case 2: */
			hitloc = HITLOC_RIGHTLEG;
/*			break;
		case 3:
			hitloc = HITLOC_HEAD;
			break;
		} */
		break;
	case DAMAGEDIR_FRONTRIGHT:
		// Split this evenly between the two pieces of armour.
		if (ud.CurFront >= pierce1)
		{
			ud.CurFront -= pierce1;
			pierce1 = 0;
		}
		else
		{
			pierce1 -= ud.CurFront;
			ud.CurFront = 0;
		}
		if (ud.CurRight >= pierce2)
		{
			ud.CurRight -= pierce2;
			pierce2 = 0;
		}
		else
		{
			pierce2 -= ud.CurRight;
			ud.CurLeft = 0;
		}
		if ((pierce1 + pierce2) == 0) return -1; // Can't get through the armuor.

		// Pick a random location when the random values can be transmitted.
		hitloc = HITLOC_RIGHTARM;
		break;
	case DAMAGEDIR_UNDER:
		// Special case.
		if (ud.CurUnder >= pierce)
		{
			ud.CurUnder -= pierce;
			return -1; // Nope.
		}

		pierce -= ud.CurUnder;
		ud.CurUnder = 0;

		hitloc = HITLOC_TORSO;
		break;
	default:
		hitloc = HITLOC_TORSO;
		break;
	}

	return hitloc;
}

void Soldier::apply_wound(int hitloc)
{
	// This SHOULD apply from 1-3 wounds, but random values can't be transmitted at the moment.
	switch(hitloc)
	{
	case HITLOC_HEAD:
		ud.HeadWound++;
		break;
	case HITLOC_TORSO:
	default:
		ud.TorsoWound++;
		break;
	case HITLOC_LEFTARM:
		ud.LArmWound++;
		break;
	case HITLOC_RIGHTARM:
		ud.RArmWound++;
		break;
	case HITLOC_LEFTLEG:
		ud.LLegWound++;
		break;
	case HITLOC_RIGHTLEG:
		ud.RLegWound++;
		break;
	}
	return;
}

void Soldier::hit(int sniper, int pierce, int type, int hitdir)
{
	int damagedir = (dir + (hitdir + 4)) % 8; // Becomes DAMAGEDIR_*, except DAMAGEDIR_UNDER...
	int hitloc;

	// Give credit to the sniper for inflicting damage if it's not stun damage.
	if (sniper && (type != DT_STUN))
	{
		StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
		if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
		if (stat) stat->inc_damage_inflicted(pierce);
	}
	
	// Record that we took damage.
	this->m_platoon->get_stats()->get_stat_for_SID(NID)->inc_damage_taken(pierce);

	if ((hitloc = do_armour_check(pierce, damagedir)) == -1) return; // Can't pierce the armour.

	if (type == DT_STUN) {
		ud.CurStun += pierce;
		if (is_stunned()) {
			m_state = FALL;
			phase = 0;
		}
		return;
	}

	if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
	{
		// Credit the sniper for getting a kill
		if (sniper)
		{
			StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
			if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
			if (stat) stat->inc_kills();
		}
		// Record that we died
		this->get_platoon()->get_stats()->get_stat_for_SID(NID)->set_dead(1);
		ud.CurHealth = 0;
		if (m_state != FALL)
		{
			switch (md.SkinType)
			{
			    case S_SECTOID:
					soundSystem::getInstance()->play(SS_SECTOID_DEATH);
					break;
			    case S_MUTON:
					soundSystem::getInstance()->play(SS_MUTON_DEATH);
					break;
			    default:
					if (md.fFemale == 1)
					    soundSystem::getInstance()->play(SS_FEMALE_DEATH);
					else
					    soundSystem::getInstance()->play(SS_MALE_DEATH);
			}
			m_state = FALL;
			phase = 0;
		}
	} else {
		apply_wound(hitloc);
		ud.CurHealth -= pierce;
		switch (md.SkinType)
		{
		    case S_SECTOID:
				soundSystem::getInstance()->play(SS_SECTOID_WOUND);
				break;
		    case S_MUTON:
				soundSystem::getInstance()->play(SS_MUTON_WOUND);
				break;
		    default:
				if (md.fFemale == 1)
				    soundSystem::getInstance()->play(SS_FEMALE_WOUND);
				else
				    soundSystem::getInstance()->play(SS_MALE_WOUND);
		}

		if (is_stunned())
		{
			m_state = FALL;
			phase = 0;
		}
	}
}

void Soldier::explo_hit(int sniper, int pierce, int type, int hitdir, int dist) //silent
{
	int damagedir = (dir + (hitdir + 4)) % 8; // Becomes DAMAGEDIR_*, except DAMAGEDIR_UNDER...
	int hitloc;

	// Give credit to the sniper for inflicting damage if it's not stun damage.
	if (sniper && (type != DT_STUN))
	{
		StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
		if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
		if (stat) stat->inc_damage_inflicted(pierce);
	}
	
	// Record that we took damage.
	this->m_platoon->get_stats()->get_stat_for_SID(NID)->inc_damage_taken(pierce);

	damage_items(pierce); // Items are OUTSIDE the armour, after all.

	// If minimal range, hit under armour. Otherwise, hit armour normally.
	if ((dist < 2) && ((hitloc = do_armour_check(pierce, DAMAGEDIR_UNDER)) == -1)) return;
	else if ((dist >= 2) && (hitloc = do_armour_check(pierce, damagedir)) == -1) return;

	if (type == DT_STUN) // Did we get stunned?
	{
		ud.CurStun += pierce;
		if (is_stunned())
		{
			m_state = FALL;
			phase = 0;
		}
		return;
	}

	if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
	{
		// Credit the sniper for getting a kill
		if (sniper)
		{
			StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
			if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
			if (stat) stat->inc_kills();
		}
		// Record that we died
		this->get_platoon()->get_stats()->get_stat_for_SID(NID)->set_dead(1);
		ud.CurHealth = 0;
		if (m_state != FALL)
		{
			switch(md.SkinType)
			{
			    case S_SECTOID:
					soundSystem::getInstance()->play(SS_SECTOID_DEATH);
					break;
			    case S_MUTON:
					soundSystem::getInstance()->play(SS_MUTON_DEATH);
					break;
			    default:
					if (md.fFemale == 1)
					    soundSystem::getInstance()->play(SS_FEMALE_DEATH);
					else
					    soundSystem::getInstance()->play(SS_MALE_DEATH);
			}
			m_state = FALL;
			phase = 0;
		}
	}
	else
	{
		apply_wound(hitloc);
		ud.CurHealth -= pierce;

		if (is_stunned())
		{
			m_state = FALL;
			phase = 0;
		}
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
	else if (md.SkinType == S_SECTOID)
		ctype = Sectoid_Corpse;
	else
		ctype = Muton_Corpse;

	map->place(z, x, y)->put(new Item(ctype));

	g_console->printf(xcom1_color(132), "%s killed.", md.Name);
}


void Soldier::stun()
{
	if (z == -1) return; // already stunned.
	map->set_man(z, x, y, NULL);

	z = map->find_ground(z, x, y);
	for (int i = 0; i < 8; i++)
		m_place[i]->dropall(z, x, y);

	/////////type of body
	int ctype;
	if (md.SkinType == S_XCOM_0)
		ctype = CORPSE;
	else if (md.SkinType == S_XCOM_1)
		ctype = CORPSE_ARMOUR;
	else if ((md.SkinType == S_XCOM_2) || (md.SkinType == S_XCOM_3))
		ctype = CORPSE_POWER_SUIT;
	else if (md.SkinType == S_SECTOID)
		ctype = Sectoid_Corpse;
	else
		ctype = Muton_Corpse;

	m_body = new Item(ctype);
	map->place(z, x, y)->put(m_body);

	x = -1;
	y = -1;
	z = -1;

	g_console->printf(xcom1_color(132), "%s stunned.", md.Name);
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
			soundSystem::getInstance()->play(SS_DOOR_CLICK);
			spend_time(6);
			net->send_open_door(NID);
			return 1;
		}
	}
	return 0;
}

int Soldier::change_pose()
{
	assert((m_state == SIT) || (m_state == STAND));

	if (m_state == SIT) {
		if (!havetime(8)) return 0;
		m_state = STAND;
		spend_time(8);
	} else {
		if (!havetime(4)) return 0;
		m_state = SIT;
		spend_time(4);
	}
	net->send_change_pose(NID);
	return 1;

}

int Soldier::prime_grenade(int iplace, int delay_time, int req_time)
{
	assert((iplace == P_ARM_RIGHT) || (iplace == P_ARM_LEFT));

	if (havetime(req_time)) {
		Item * it = item(iplace);
		assert(it != NULL);

		elist->add(this, it, delay_time);

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

/**
 * Function that decrements soldier time units and energy for some action.
 * The soldier must have enough time units and energy before calling 
 * this function,
 * 
 * @param tm          time required to perform an action
 * @param use_energy  flag which shows whether the action requires energy 
 *                    to perform
 */
void Soldier::spend_time(int tm, int use_energy)
{
	assert(ud.CurTU >= tm);
	ud.CurTU -= tm;
	if (use_energy) {
		assert(ud.CurEnergy >= (tm / 2));
		ud.CurEnergy -= (tm / 2);
	}

	if (FLAGS & F_ENDLESS_TU) {
		if (ud.CurTU < 32) ud.CurTU = ud.MaxTU;
		if (ud.CurEnergy < 16) ud.CurEnergy = ud.MaxEnergy;
	}
}

/**
 * Function that checks if the soldier has time units and energy required
 * to do something. For actions that require energy, 
 *
 * @param ntime       time required to perform an action
 * @param use_energy  flag which shows whether the action requires energy 
 *                    to perform
 */
int Soldier::havetime(int ntime, int use_energy)
{
	if (use_energy)
		return ((ud.CurTU >= ntime) && (ud.CurEnergy >= (ntime / 2)));

	return (ud.CurTU >= ntime);
}

/**
 * Function that returns the time needed to move from current location to 
 * specified direction.
 *
 * @param _dir  walk direction (-1 in the case when the time to get to the current 
 *              map location is needed)
 */
int Soldier::walktime(int _dir)
{
	int dz = z, dx = x, dy = y;
	if (_dir != -1) {
		dx += DIR_DELTA_X(_dir);
		dy += DIR_DELTA_Y(_dir);
	}
	int time_of_dst = map->mcd(dz, dx, dy, 0)->TU_Walk;
	time_of_dst += map->mcd(dz, dx, dy, 3)->TU_Walk;

	if (_dir != -1 && DIR_DIAGONAL(_dir))
		time_of_dst = time_of_dst * 3 / 2; // Diagonal move multiplier.
	// Only used with havetime(). Actual movement calls walktime(-1).

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
	int ac = ud.CurFAccuracy;
	ac -= (ac * (ud.MaxHealth - ud.CurHealth)) / ud.MaxHealth / 2;

	if (TWOHAND) {
		if ((rhand_item() != NULL) && (lhand_item() != NULL))
			ac -= ac / 3;
	}

	if (m_state == SIT) ac += ac / 20;

	double weapon_delta = 1. / (double)(peraccur * peraccur);
	double soldier_delta = 1. / (double)(ac * ac);

	return static_cast<int>(sqrt(2. / (weapon_delta + soldier_delta)));
}

int Soldier::TAccuracy(int peraccur)
{
	int ac = (ud.CurTAccuracy * peraccur) / 100;
	ac = (ac * ud.CurHealth) / ud.MaxHealth;
	return ac;
}

void Soldier::apply_accuracy(REAL & fi, REAL & te)
{
	REAL TE_STEP = (PI / 8. / (double)(cfg_get_base_accuracy()));
	REAL FI_STEP = (PI / 32. / (double)(cfg_get_base_accuracy()));

	double acc = 100. * 100. / (double)(target.accur * target.accur);

//	According to central limit theorem, the sum of many small random values
//	is normally distributed
	for (int i = 0; i < 16; i++) {
		te += TE_STEP * randval(-acc, +acc);
		fi += FI_STEP * randval(-acc, +acc);
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
		if (m_state == SIT)
			s = 1;

		//lev -= map->mcd(z, x, y, 0)->T_Level; //!!!
		//lev -= map->mcd(z, x, y, 3)->T_Level;
		//if (lev < 0) lev = 0;

		if (m_bof[s][dir][lev][col][15 - row])
			return 1;
	}

	return 0;
}


void Soldier::apply_hit(int sniper, int _z, int _x, int _y, int _wtype, int _hitdir)
{
	if (check_for_hit(_z, _x, _y)) {
		hit(sniper, Item::obdata[_wtype].damage, Item::obdata[_wtype].damageType, _hitdir);
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
	REACTION = 0; // Very important!

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

	// Perform some checks to determine if we can keep targeting mode after 
	// this shot. So we need to check if TWO shots can be made (one right now 
	// and another one after left clicking mouse in a targeting mode again)

	if (!havetime(target.time * 2 * FIRE_num)) {
		TARGET = 0;
	}

	if (!target.item->is_laser() && !target.item->is_cold_weapon() &&
	        ((target.item->m_ammo == NULL) || (target.item->m_ammo->m_rounds < FIRE_num + 1))) {
		TARGET = 0;
	}

	if (target.action != SNAPSHOT && target.action != AIMEDSHOT &&
	        target.action != AUTOSHOT && target.action != PUNCH) {
		TARGET = 0;
	}

	// Face to target
	faceto(map->sel_col, map->sel_row);
}

// Note the absence of map->sel_*, since the aim is to hit the target's position.
// So instead of using map->sel_*, we use the_target->*.
void Soldier::try_reaction_shot(Soldier *the_target)
{
	REACTION = 1; // Also very important - reaction shots don't provoke reaction shots.
 
	// Moving soldier cannot shoot
	if (ismoving()) return;

	if ((z == the_target->z) && (x == the_target->x) && (y == the_target->y)) return;

	if (FIRE_num != 0) return;

	// This shouldn't happen, but check anyways!
	if (target.action == PUNCH) {
		if ((z != the_target->z) || (abs(x - the_target->x) > 1) || (abs(y - the_target->y) > 1))
			return ;
	}

	if (target.action == AUTOSHOT)
		FIRE_num = 3;
	else
		FIRE_num = 1;

	FIRE_z = the_target->z * 12 + 8;
	FIRE_x = the_target->x * 16 + 8;
	FIRE_y = the_target->y * 16 + 8;

	// Check that we can fire at least one shot using current settings

	if (!havetime(target.time * FIRE_num)) {
		FIRE_num = 0;
		return;
	}

	if (!target.item->is_laser() && !target.item->is_cold_weapon() &&
	        ((target.item->m_ammo == NULL) || (target.item->m_ammo->m_rounds < FIRE_num))) {
		FIRE_num = 0;
		return;
	}

	if (target.action != SNAPSHOT && target.action != AIMEDSHOT &&
	        target.action != AUTOSHOT && target.action != PUNCH) {
		FIRE_num = 0;
		return;
	}

	// Face to target
	faceto(the_target->x, the_target->y);
	return;
}

void Soldier::shoot(int zd, int xd, int yd, int ISLOCAL)
{
	assert(target.action != NONE);
	assert(target.item != NULL);

	int z0 = z * 12 + 8; if (m_state == SIT) z0 -= 4;
	int x0 = x * 16 + 8;
	int y0 = y * 16 + 8;

	int chances = (target.time / 4); // How many chances at a reaction shot do we get? TUs / 4.

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
		apply_throwing_accuracy(fi, te, target.item->data()->weight);

		thru(z0, x0, y0, ro, fi, te, zA, target.place, target.time);
		// !!! Hack - reaction temporarily disabled to prevent grenades bug
		chances = 0;
	} else if (target.action == AIMEDTHROW) {
		REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
		REAL fi = acos((REAL)(zd - z0) / ro);
		REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
		apply_accuracy(fi, te);

		aimedthrow(z0, x0, y0, fi, te, target.place, target.time);
		// !!! Hack - reaction temporarily disabled to prevent grenades bug
		chances = 0;
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

	if (!REACTION)            // If this shot isn't a reaction shot,
		while (chances-- > 0) // then give a chance at a reaction shot for every four TUs used.
		{
			if (ISLOCAL) platoon_remote->check_reaction_fire(this);
			else platoon_local->check_reaction_fire(this);
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

	m_bullet->punch(z0, x0, y0, fi, te, it->m_type);
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

	m_bullet->beam(z0, x0, y0, fi, te, it->m_type);
	net->send_beam(NID, z0, x0, y0, fi, te, iplace, req_time);
	return 1;
}


int Soldier::fire(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time)
{
	if (!havetime(req_time))
		return 0;

	Item *it = item(iplace);
	if ((it == NULL) || (!it->haveclip()) || (it->roundsremain() < 1))
		return 0;

	spend_time(req_time);
	
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
	// Why did these have "% 4" before? They prevented the flying suit from displaying.
	switch (md.SkinType) {
		case S_XCOM_0:
		case S_XCOM_1:
			m_spk[(md.SkinType - 1) % 4][md.fFemale][md.Appearance]->show(screen2, 0, 0);
			break;
		case S_XCOM_2:
		case S_XCOM_3:
			m_spk[(md.SkinType - 1) % 4][0][0]->show(screen2, 0, 0);
			break;
		case S_SECTOID:
			m_spk[4][0][0]->show(screen2, 0, 0);
			break;
		case S_MUTON:
			m_spk[5][0][0]->show(screen2, 0, 0);
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
			if (rhand_item()->delay_time() > 0)
				printsmall(x + 304, y + 47, xcom1_color(36), rhand_item()->delay_time() - 1);
			if ((rhand_item()->m_type == PROXIMITY_GRENADE) && (rhand_item()->delay_time() < 0))
				textout(screen2, g_small_font, "*", x + 304, y + 43, xcom1_color(36));
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
			if (lhand_item()->delay_time() > 0)
				printsmall(x + 33, y + 47, xcom1_color(36), lhand_item()->delay_time() - 1);
			if ((lhand_item()->m_type == PROXIMITY_GRENADE) && (lhand_item()->delay_time() < 0))
				textout(screen2, g_small_font, "*", x + 32, y + 43, xcom1_color(36));
		}
	}
	textout(screen2, g_small_font, md.Name, x + 134, y + 32, xcom1_color(130));


	drawbar(xcom1_color(71), xcom1_color(64), 170 + x, 41 + y, ud.CurTU, ud.MaxTU);
	//textprintf(screen2, font, x+136, y+41, 64, "%d", ud.CurTU);
	printsmall(x + 136, y + 42, xcom1_color(64), ud.CurTU);

	drawbar(xcom1_color(21), xcom1_color(16), 170 + x, 45 + y, ud.CurEnergy, ud.MaxEnergy);
	//textprintf(screen2, font, x+154, y+41, 16, "%d", ud.CurEnergy);
	printsmall(x + 154, y + 42, xcom1_color(16), ud.CurEnergy);

	drawbar(xcom1_color(37), xcom1_color(32), 170 + x, 49 + y, ud.CurHealth, ud.MaxHealth);
	//textprintf(screen2, font, x+136, y+49, 32, "%d", ud.CurHealth);
	printsmall(x + 136, y + 50, xcom1_color(32), ud.CurHealth);

	if (ud.CurStun > 0) // draw stun bar
	{
		if (ud.CurStun < ud.CurHealth)
			hline(screen2, 170 + x, 50 + y, 170 + x + ud.CurStun, 80);
		else
			hline(screen2, 170 + x, 50 + y, 170 + x + ud.CurHealth, 80);
		putpixel(screen2, 170 + x + ud.MaxHealth + 1, 50 + y + 1, 37);
	}

	drawbar(xcom1_color(197), xcom1_color(192), 170 + x, 53 + y, ud.Morale, 100);
	//textprintf(screen2, font, x+154, y+49, 192, "%d", ud.Morale);
	printsmall(x + 154, y + 50, xcom1_color(192), ud.Morale);
}

#define AUTO   0
#define SNAP   1
#define AIMED  2

int Soldier::check_reaction_fire(Soldier *the_target)
{
	// Translate target's cell into a value used by m_visible_cells.
	int width_10 = 10 * map->width, height_10 = 10 * map->height,
		n = the_target->y + (the_target->x * height_10) + (the_target->z * width_10 * height_10);
	calc_visible_cells(); //!!
	if(m_visible_cells[n] != 0) // Can we see this cell?
	{
		// Ok, check for reaction fire.
		// Compare the reaction figures.
		float total_reactions = ud.CurReactions;
		float tu_ratio; 
		if (the_target->ud.CurTU > 0) tu_ratio = ud.CurTU / the_target->ud.CurTU;
		else tu_ratio = 999;

		assert(the_target->ud.CurReactions > 0); // Shouldn't happen, but...
		
		if (((float)ud.CurReactions / (float)the_target->ud.CurReactions) < total_reactions)
			total_reactions = ((float)ud.CurReactions / (float)the_target->ud.CurReactions);

		total_reactions /= 2;

		if (tu_ratio < 1) total_reactions *= tu_ratio;
		
		if (randval(0, 1) < total_reactions)
		{
			// We can make a reaction shot.
			// Try the weapon in right hand first
			if (do_reaction_fire(the_target, P_ARM_RIGHT, AIMED)) return 1;
			if (do_reaction_fire(the_target, P_ARM_RIGHT, SNAP)) return 1;
			if (do_reaction_fire(the_target, P_ARM_RIGHT, AUTO)) return 1;

			// No luck with right arm, go to left arm.
			if (do_reaction_fire(the_target, P_ARM_LEFT, AIMED)) return 1;
			if (do_reaction_fire(the_target, P_ARM_LEFT, SNAP)) return 1;
			if (do_reaction_fire(the_target, P_ARM_LEFT, AUTO)) return 1;

			// No luck whatsoever. Fuhgeddabouddit.
			return 0;
		}
		else return 0; // Can't react fast enough. No go.
	}
	return 0; // Can't see cell. Abort.
}

int Soldier::do_reaction_fire(Soldier *the_target, int place, int shot_type)
{
	Item *it = item(place);
	if (it == NULL) return 0; // no item in hand
	if (!it->data()->isGun && !it->is_laser()) return 0; // item is not a gun or laser
	if (it->data()->isGun && !it->haveclip()) return 0; // gun with no clip

	int tus;

	// Can this item make this type of shot?
	if (!it->data()->accuracy[shot_type]) return 0;

	// How many TUs do we use?
	tus = required(it->data()->time[shot_type]);
	if (shot_type == AUTO) tus = (tus + 2) / 3 * 3; // this was taken from firemenu

	if (tus <= ud.CurTU) {
		// We have enough time to make the shot. Set up target.
		target.accur = FAccuracy(it->data()->accuracy[shot_type], it->data()->twoHanded);
		target.time = tus;
		switch (shot_type) {
			case AIMED:
				target.action = AIMEDSHOT;
				break;
			case AUTO:
				target.action = AUTOSHOT;
				break;
			case SNAP:
				target.action = SNAPSHOT;
				break;
		}
		target.item = it;
		target.place = place;
		try_reaction_shot(the_target);
		if (FIRE_num > 0) // If FIRE_num is set, we're firing shots, so...
			return 1;
	}

	// Nope.
	return 0;
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
		md.Name, NID, z, x, y, dir, (int)m_state, ud.MaxTU, ud.MaxHealth, ud.MaxEnergy, ud.CurFAccuracy, ud.CurTAccuracy, ud.CurTU, ud.CurHealth, ud.CurEnergy, ud.Morale);
	for (int i = 0; i < 8; i++) {
		len += m_place[i]->eot_save(i, txt + len);
	}
	return len;
}

int Soldier::count_weight()
{
	int weight = 0;
	for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
		weight += m_place[i]->count_weight();

	return weight;
}

bool Soldier::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, m_next);
	PersistWriteObject(archive, m_prev);
	PersistWriteObject(archive, m_bullet);
	PersistWriteObject(archive, m_body);
	PersistWriteObject(archive, m_platoon);

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
	PersistReadObject(archive, m_body);
	PersistReadObject(archive, m_platoon);

	for (int i = 0; i < NUMBER_OF_PLACES; i++)
		PersistReadObject(archive, m_place[i]);

	return true;
}
