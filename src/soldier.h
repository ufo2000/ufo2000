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
#ifndef SOLDIER_H
#define SOLDIER_H
#include "pck.h"
#include "spk.h"
#include "bullet.h"
#include "item.h"
#include "place.h"

#define P_SHL_RIGHT       0
#define P_SHL_LEFT        1
#define P_ARM_RIGHT       2
#define P_ARM_LEFT        3
#define P_LEG_RIGHT       4
#define P_LEG_LEFT        5
#define P_BACK            6
#define P_BELT            7
#define P_MAP             8
#define NUMBER_OF_PLACES  9
#define P_ARMOURY         9

#define S_HANDOB          0
#define S_XCOM_0          1
#define S_XCOM_1          2
#define S_XCOM_2          3
#define S_XCOM_3          4
#define S_SECTOID         5
#define S_MUTON           6
#define SKIN_NUMBER       7

struct SKIN_INFO { 
	const char *Name; 
	int         SkinType; 
	int         fFemale; 
};

extern SKIN_INFO g_skins[];
extern int g_skins_count;

int get_skin_index(int skin_type, int female_flag);

enum State { SIT = 0, STAND, MARCH, DIE };

//////////////////////////////////////////////////////////////////////////////
/// A class which describes unit with all his equipment, health status and ///
/// position on map                                                        ///
//////////////////////////////////////////////////////////////////////////////

class Soldier: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Soldier);
private:
	friend class Map;
	friend class Platoon;
	friend class Inventory;
	friend class Icon;
	friend class Editor;
public:
	MANDATA md;
	ITEMDATA id;
private:
	UNITDATA ud;

	static char *****m_bof;
	static PCK **m_pck;
	static SPK *m_spk[6][2][4];
	static BITMAP *m_unibord;
	static int dir2ofs[8];
	static char ofs2dir[3][3];

	Soldier *m_next, *m_prev;
	Bullet *m_bullet;
	Place  *m_place[NUMBER_OF_PLACES]; //8 - for internal editor use
	bool m_p_map_allocated;

	int NID;
	int dir, phase;
	State m_state;
	int z, x, y;

	// This flag is set after soldier has moved. Needed for correct work of
	// switch to next soldier button
	int MOVED;

	// The number of shots to be done and aiming point. Soldier cannot be saved
	// before all shots have been processed
	int FIRE_num;
	int FIRE_z, FIRE_x, FIRE_y;

	// Information for pathfinding It does not need to be saved and can be built
	// by calling wayto function. If waylen != 0 this soldier is currently moving
	// and cannot be saved
	char way[100];
	int curway, waylen;

	// Information about seen enemies. It does not need to be saved and can be
	// restored by calling calc_visible_cells? function
	char m_visible_cells[4 * 6 * 10 * 6 * 10];      // do sizeof
	int enemy_num;
	int enemy_z[100], enemy_x[100], enemy_y[100];
	int seen_enemy_num;
	int seen_enemy_z[100], seen_enemy_x[100], seen_enemy_y[100];

	static void initbof();

public:

	static void initpck();
	static void freepck();

	Soldier() { }
	Soldier(int _NID);
	Soldier(int _NID, int _z, int _x, int _y);
	Soldier(int _NID, int _z, int _x, int _y, MANDATA *sdat, ITEMDATA *idat);
	virtual ~Soldier();

	void initialize();

	void process_MANDATA();
	void show_MANDATA(int gx, int gy, int gcol);
	void process_ITEMDATA();
	void build_ITEMDATA();
	void build_items_stats(char *buf, int &len);


	void restore();
	int move(int ISLOCAL);
	void calc_visible_cells();

	void draw();
	void draw_inventory();
	void draw_unibord(int gx, int gy);
	void draw_selector(int select_y);
	void draw_blue_selector();
	void showspk();
	void drawbar(int col1, int col2, int x2, int y2, int val, int valmax);
	void drawinfo(int x, int y);
	void draw_bullet_way();
	void draw_enemy_seen(int select_y);
	int center_enemy_seen();

	void turnto(int destdir);
	void wayto(int dest_lev, int dest_col, int dest_row);
	void finish_march(int ISLOCAL);
	void break_march();
	void faceto(int dest_col, int dest_row);
	int dirto(int src_col, int src_row, int dest_col, int dest_row);
	int open_door();
	int change_pose();
	int prime_grenade(int iplace, int delay_time, int req_time);
	int unload_ammo(Item *it);
	int load_ammo(int iplace, Item *it);
	void die();

	void apply_accuracy(REAL &fi, REAL &te);
	void apply_throwing_accuracy(REAL &fi, REAL &te, int weight);
	int required(int pertime);
	int FAccuracy(int peraccur, int TWOHAND);
	int TAccuracy(int peraccur);

	void precise_aiming();
	void try_shoot();
	void shoot(int tlev, int tcol, int trow);
	int thru(int z0, int x0, int y0, REAL ro, REAL fi, REAL te, REAL zA, int iplace, int req_time);
	int beam(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int iplace, int req_time);
	int fire(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int iplace, int req_time);
	int punch(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time);
	int aimedthrow(int z0, int x0, int y0, REAL fi, REAL te, int iplace, int req_time);

	int check_for_hit(int _z, int _x, int _y);
	void apply_hit(int _z, int _x, int _y, int _type);
	void hit(int pierce);
	void explo_hit(int pierce);      //silent

	int ismoving();
	int is_marching() { return (m_state == MARCH); }
	int havetime(int ntime);
	void spend_time(int tm);
	int walktime(int _dir);

	void unlink();
	Soldier *nextman();
	Soldier *prevman();
	Soldier *next() { return m_next; }
	Soldier *prev() { return m_prev; }
	void set_next(Soldier *s) { m_next = s; }
	void set_prev(Soldier *s) { m_prev = s; }
	Bullet *bullet() { return m_bullet; }
/*
	inline void create_map_place()
	{
		m_place[P_MAP] = new Place(0, 152, 20 * 10, 3);
	} //as map
	inline void destroy_map_place()
	{
		delete m_place[P_MAP];
	}
*/
	int eot_save(char *txt);

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int calc_ammunition_cost();
	Item *item_under_mouse(int ipl);
	int calctime(int src, int dst);
	Item *select_item(int &pn);
	int deselect_item(Item *&it, int it_place, int &req_time);
	Place *find_item(Item *it, int &lev, int &col, int &row);
	int haveitem(Item *it);

	Item *lhand_item() { return m_place[P_ARM_LEFT]->item(); }
	Item *rhand_item() { return m_place[P_ARM_RIGHT]->item(); }

	Item *item(int ip)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip]->item();
	}

	Item *item(int ip, int ix, int iy)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip]->item(ix, iy);
	}

	Item *getitem(int ip, int ix, int iy)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip]->get(ix, iy);
	}

	int putitem(Item *it, int ip, int ix, int iy)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip]->put(it, ix, iy);
	}

	int putitem(Item *it, int ip)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip]->put(it);
	}

	int place(Place *place);
	Place *place(int ip)
	{
		assert((ip >= 0) && (ip < NUMBER_OF_PLACES));
		return m_place[ip];
	}

	void destroy_all_items();
	void draw_deselect_times(int sel_item_place);
	void damage_items(int damage);

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
