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
#include "bullet.h"
#include "video.h"
#include "sound.h"
#include "map.h"
#include "platoon.h"
#include "explo.h"
#include "multiplay.h"

#define PHASE 2

IMPLEMENT_PERSISTENCE(Bullet, "Bullet");

Bullet::Bullet(Soldier *man)
{
	state = READY;
	phase = 0;
	item  = NULL;
	owner = man->get_NID();
}

void Bullet::punch(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
	//play(S_PUNCH);
	state = ST_PUNCH;

	z0 = _z0; x0 = _x0; y0 = _y0;
	fi = _fi; te = _te;
	type = _type;

	for (i = 7; i < 24; i++) {
		z = (int)(z0 + i * cos(fi));
		x = (int)(x0 + i * cos(te) * sin(fi));
		y = (int)(y0 + i * sin(te) * sin(fi));

		if ((!map->inside(z, x, y)) ||
		        (!map->pass_lof_cell(z, x, y)))
			break;
		if (platoon_remote->check_for_hit(z, x, y) ||
		        platoon_local->check_for_hit(z, x, y)
		   )
			break;
	}

	x0 += (int)(8 * cos(te) * sin(fi));
	y0 += (int)(8 * sin(te) * sin(fi));
	z0 += (int)(8 * cos(fi));

	i = 0;
}

void Bullet::fire(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
    
//	soundSystem::getInstance()->play(SS_CV_RIFLE_SHOT);
	switch(_type)
	{
		case PISTOL_CLIP:
			soundSystem::getInstance()->play(SS_CV_PISTOL_SHOT);
			break;
		case RIFLE_CLIP:
		default:
			soundSystem::getInstance()->play(SS_CV_RIFLE_SHOT);
			break;
		case CANNON_AP_AMMO:
		case CANNON_HE_AMMO:
		case CANNON_I_AMMO:
 		        soundSystem::getInstance()->play(SS_CV_HEAVY_CANNON_SHOT);
			break;
		case AUTO_CANNON_AP_AMMO:
		case AUTO_CANNON_HE_AMMO:
		case AUTO_CANNON_I_AMMO:
 		        soundSystem::getInstance()->play(SS_CV_AUTO_CANNON_SHOT);
			break;
		case SMALL_ROCKET:
		case LARGE_ROCKET:
		case INCENDIARY_ROCKET:
 		        soundSystem::getInstance()->play(SS_RL_LAUNCH);
			break;
		case Plasma_Pistol_Clip:
			soundSystem::getInstance()->play(SS_PLASMA_PISTOL_SHOT);
			break;
		case Plasma_Rifle_Clip:
			soundSystem::getInstance()->play(SS_PLASMA_RIFLE_SHOT);
			break;
		case Heavy_Plasma_Clip:
			soundSystem::getInstance()->play(SS_PLASMA_HEAVY_SHOT);
			break;
		case STUN_MISSILE:
			soundSystem::getInstance()->play(SS_ALIEN_SMALL_LAUNCH);
			break;
	}
	state = FLY;

	z0 = _z0; x0 = _x0; y0 = _y0;
	fi = _fi; te = _te;
	type = _type;

	i = 7;
	move();
}

void Bullet::beam(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
//	soundSystem::getInstance()->play(SS_LASER_RIFLE_SHOT);
	switch(_type)
	{
		case LASER_PISTOL:
			soundSystem::getInstance()->play(SS_LASER_PISTOL_SHOT);
			break;			
		case LASER_GUN:
		default:
			soundSystem::getInstance()->play(SS_LASER_RIFLE_SHOT);
			break;
		case HEAVY_LASER:
			soundSystem::getInstance()->play(SS_LASER_HEAVY_SHOT);
			break;
	}
	state = BEAM;

	z0 = _z0; x0 = _x0; y0 = _y0;
	fi = _fi; te = _te;
	type = _type;

	for (i = 7; i < 100000; i++) {
		z = (int)(z0 + i * cos(fi));
		x = (int)(x0 + i * cos(te) * sin(fi));
		y = (int)(y0 + i * sin(te) * sin(fi));

		if ((!map->inside(z, x, y)) ||
		        (!map->pass_lof_cell(z, x, y)))
			break;
		if (platoon_remote->check_for_hit(z, x, y) ||
		        platoon_local->check_for_hit(z, x, y)
		   )
			break;
	}

	x0 += (int)(8 * cos(te) * sin(fi));
	y0 += (int)(8 * sin(te) * sin(fi));
	z0 += (int)(8 * cos(fi));

	i = 0;
}

void Bullet::thru(int _z0, int _x0, int _y0, REAL _ro, REAL _fi, REAL _te, REAL _zA, Item *_item)
{
	//play(S_THROW);
	state = THROWN;

	z0 = _z0; x0 = _x0; y0 = _y0;
	ro = _ro; fi = _fi; te = _te;
	zA = _zA; item = _item;

	i = 4;
	move();
}

void Bullet::aimedthrow(int _z0, int _x0, int _y0, REAL _fi, REAL _te, Item *_item)
{
	//play(S_AIMEDTHROW);
	state = ST_AIMEDTHROW;

	z0 = _z0; x0 = _x0; y0 = _y0;
	/*ro = _ro;*/ fi =      _fi;      te =      _te;
	item = _item;
	type = item->m_type;

	i = 7;
	move();
}

void Bullet::hitcell()
{
	if (z < 0) z = 0;
	else
		if (z >= map->level * 12) z = map->level * 12 - 1;

	if (x < 0) x = 0;
	else
		if (x >= map->width * 10 * 16) x = map->width * 10 * 16 - 1;

	if (y < 0) y = 0;
	else
		if (y >= map->height * 10 * 16) y = map->height * 10 * 16 - 1;

	lev = z / 12;      //if (lev >= map->level) lev = map->level - 1;
	col = x / 16;      //if (col >= map->width*10) col = map->width*10 - 1;
	row = y / 16;      //if (row >= map->height*10) row = map->height*10 - 1;
}


void Bullet::move()
{
	int j;
	REAL zK;

	switch (state) {
		case READY:
			break;
		case FLY:
			for (j = 0; j < 8; j++) {
				i++;

				x = (int)(x0 + i * cos(te) * sin(fi));
				y = (int)(y0 + i * sin(te) * sin(fi));
				z = (int)(z0 + i * cos(fi));
				//text_mode(0);
				//textprintf(screen, font, 0, SCREEN2H+20, 1, "(%f,%f,%f)", z, x, y);

				if ((!map->inside(z, x, y)) ||
				        (!map->pass_lof_cell(z, x, y)) ||
				        platoon_remote->check_for_hit(z, x, y) ||
				        platoon_local->check_for_hit(z, x, y)
				   ) {
					hitcell();
					state = HIT;
					switch(type)
					{
						case PISTOL_CLIP:
						case RIFLE_CLIP:
						default:
							soundSystem::getInstance()->play(SS_CV_BULLET_HIT);
							break;
						case CANNON_AP_AMMO:
						case AUTO_CANNON_AP_AMMO:
				 		    soundSystem::getInstance()->play(SS_CV_CANNON_AP_HIT);
							break;
						case CANNON_HE_AMMO:
						case AUTO_CANNON_HE_AMMO:
				 		    soundSystem::getInstance()->play(SS_CV_CANNON_HE_HIT);
							break;
						case CANNON_I_AMMO:
						case AUTO_CANNON_I_AMMO:
				 		    soundSystem::getInstance()->play(SS_CV_CANNON_IN_HIT);
							break;
						case SMALL_ROCKET:
			 		        soundSystem::getInstance()->play(SS_SMALL_ROCKET_HIT);
							break;
						case LARGE_ROCKET:
			 		        soundSystem::getInstance()->play(SS_HE_ROCKET_HIT);
							break;
						case INCENDIARY_ROCKET:
			 		        soundSystem::getInstance()->play(SS_IN_ROCKET_HIT);
							break;
						case Plasma_Pistol_Clip:
						case Plasma_Rifle_Clip:
						case Heavy_Plasma_Clip:
							soundSystem::getInstance()->play(SS_PLASMA_HIT);
							break;
						case STUN_MISSILE:
							soundSystem::getInstance()->play(SS_ALIEN_SMALL_HIT);
							break;
					}

					break;
				}
			}
			break;
		case BEAM:
			i++;
			if (i > 8) {
				hitcell();
				state = HIT;
				break;
			}
			break;
		case THROWN:
			zK = 4 * zA / ro / ro;
			for (j = 0; j < 3; j++) {
				i++;

				x = (int)(x0 + i * cos(te) * sin(fi));
				y = (int)(y0 + i * sin(te) * sin(fi));
				z = (int)(z0 + i * cos(fi) - zK * (i - ro / 2.0) * (i - ro / 2.0) + zA);

				if ((!map->inside(z, x, y)) ||
				        (!map->pass_lof_cell(z, x, y))) {
					i -= 1;      //prevent fall over wall
					x = (int)(x0 + i * cos(te) * sin(fi));
					y = (int)(y0 + i * sin(te) * sin(fi));
					z = (int)(z0 + i * cos(fi) - zK * (i - ro / 2.0) * (i - ro / 2.0) + zA);

					hitcell();      //lev = 0; //!!!!
					lev = map->find_ground(lev, col, row);
					map->place(lev, col, row)->put(item);
					if (platoon_local->belong(this))
						elist->check_for_detonation(0, item);
					item = NULL;
					state = READY;
					break;
				}
			}
			break;
		case ST_AIMEDTHROW:
			for (j = 0; j < 3; j++) {
				i++;

				x = (int)(x0 + i * cos(te) * sin(fi));
				y = (int)(y0 + i * sin(te) * sin(fi));
				z = (int)(z0 + i * cos(fi));

				if ((!map->inside(z, x, y)) || (i > 16 * 5) ||
				        (!map->pass_lof_cell(z, x, y)) ||
				        platoon_remote->check_for_hit(z, x, y) ||
				        platoon_local->check_for_hit(z, x, y)
				   ) {
					//i -= 1; //prevent fall over wall
					x = (int)(x0 + i * cos(te) * sin(fi));
					y = (int)(y0 + i * sin(te) * sin(fi));
					z = (int)(z0 + i * cos(fi));

					hitcell();      //lev = 0; //!!!!
					hitman();
					lev = map->find_ground(lev, col, row);
					map->place(lev, col, row)->put(item);
					item  = NULL;
					state = READY;
					break;
				}
			}
			break;
		case ST_PUNCH:
			i++;
			if (i > 8) {
				hitcell();
				state = HIT;
				break;
			}
			break;
		case HIT:
			if (phase == 0) {
				if (explodable()) {
					detonate();
					item  = NULL;
					state = READY;
					break;
				}
			}
			if (incendiary()) {
				detonate();
				item  = NULL;
				state = READY;
				break;
			}
			phase++;
			if (phase == 2 * PHASE)
				hitman();

			if (phase > 9 * PHASE) {
				phase = 0;
				item  = NULL;
				state = READY;
			}
	}
}

#define NORMAL_PROJECTILE 26
#define LASER_PROJECTILE  35
#define PLASMA_PROJECTILE 46

void draw_bullet_trace(int x0, int y0, int z0, int x, int y, int z, int length, int color)
{
	double dx = x - x0;
	double dy = y - y0;
	double dz = z - z0;

	double total_trace_length = sqrt(dx * dx + dy * dy + dz * dz);

	if (total_trace_length > length) {
		x0 = (int)(x - dx * length / total_trace_length);
		y0 = (int)(y - dy * length / total_trace_length);
		z0 = (int)(z - dz * length / total_trace_length);
	}

	int xg1 = map->x + x0 + y0;
	int yg1 = (int)(map->y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

	int xg2 = map->x + x + y;
	int yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

	for (int i = 0; i < length; i++) {
		double k = (double)i / (double)length;
		k = k * k;
		int x = (int)(xg2 + (xg1 - xg2) * k);
		int y = (int)(yg2 + (yg1 - yg2) * k);

		putpixel(screen2, x, y, color);
	}
}

void Bullet::draw()
{
	int xg, yg;
	int xg2, yg2;
	int xe, ye, ze;

	switch (state) {
		case READY:
			break;

		case FLY: {
			xg = map->x + x + y;
			yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			int len = 15, c = 16;

			switch (type) {
				case Plasma_Pistol_Clip: len = 15; c = 50; break;
				case Plasma_Rifle_Clip: len = 20; c = 51; break;
				case Heavy_Plasma_Clip: len = 30; c = 52; break;
			}

			draw_bullet_trace(x0, y0, z0, x, y, z, len, xcom1_color(c + 5));
			putpixel(screen2, xg, yg, xcom1_color(c));
			circle(screen2, xg, yg, 1, xcom1_color(c + 2));

			break;
		}
		case BEAM:
			xg = map->x + x0 + y0;
			yg = (int)(map->y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

			xg2 = map->x + x + y;
			yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			line(screen2, xg, yg, xg2, yg2, xcom1_color(143 + i));
			break;

		case THROWN:
			xg = map->x + x + y;
			yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			if ((xg > -32) && (xg < SCREEN2W) && (yg >= -34) && (yg < SCREEN2H)) {
				map->drawitem(item->data()->pMap, xg - 16, yg - 26);
				//circle(screen2, xg, yg, 1, 32);
			}

			xg2 = map->x + x + y;
			yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - 2);
			putpixel(screen2, xg2, yg2, 0);
			//circle(screen2, xg2, yg2, 1, 0);

			break;

		case ST_AIMEDTHROW:
			xg = map->x + x + y;
			yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			xe = (int)(x0 + (i - 7) * cos(te) * sin(fi));
			ye = (int)(y0 + (i - 7) * sin(te) * sin(fi));
			ze = (int)(z0 + (i - 7) * cos(fi));

			xg2 = map->x + xe + ye;
			yg2 = (int)(map->y - (xe + 1) / 2.0 + ye / 2.0 - ze * 2.0 - 2);

			line(screen2, xg, yg, xg2, yg2, xcom1_color(1));

			/*if ( (xg>-32)&&(xg<SCREEN2W)&&(yg>=-34)&&(yg<SCREEN2H) ) {
			 map->drawitem(item->data()->pMap, xg-16, yg-26);
			 //circle(screen2, xg, yg, 1, 32);
			}*/
			break;

		case ST_PUNCH:
			xg = map->x + x0 + y0;
			yg = (int)(map->y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

			//xg2 = map->x + xd + yd;
			//yg2 = map->y - (xd+1)/2 + yd/2 - zd*23.5/12.0;
			xg2 = map->x + x + y;
			yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			line(screen2, xg, yg, xg2, yg2, xcom1_color(143 + i));
			break;

		case HIT:
			xg = map->x + x + y;
			yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

			if ((xg > -32) && (xg < SCREEN2W) && (yg >= -34) && (yg < SCREEN2H)) {
				switch (type) {
					case LASER_PISTOL: case LASER_GUN: case HEAVY_LASER:
						Map::smoke->showpck(LASER_PROJECTILE + phase / PHASE, xg - 16, yg - 10);
						break;
					case Plasma_Pistol_Clip: case Plasma_Rifle_Clip: case Heavy_Plasma_Clip:
						Map::smoke->showpck(PLASMA_PROJECTILE + phase / PHASE, xg - 16, yg - 10);
						break;
					default:
						Map::smoke->showpck(NORMAL_PROJECTILE + phase / PHASE, xg - 16, yg - 10);
						break;
				}
			}
			break;
	}
}

static void dotted_line_proc(BITMAP *bmp, int x, int y, int color)
{
	static unsigned char counter = 0;
	counter--;
	if (counter % 5 == 0)
		putpixel(bmp, x, y, color);
}


void Bullet::showline(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
	if ((z_s == z_d) && (x_s == x_d) && (y_s == y_d))
		return ;

	int xd, yd, zd;
	int xg, yg;
	int xg2, yg2;

	x0 = x_s * 16 + 8; y0 = y_s * 16 + 8; z0 = z_s * 12 + 8;
	xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 8;

	REAL ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
	REAL fi = acos((REAL)(zd - z0) / ro);
	REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));

	int i;
	for (i = 7; i < 100000; i++) {
		zd = (int)(z0 + i * cos(fi));
		xd = (int)(x0 + i * cos(te) * sin(fi));
		yd = (int)(y0 + i * sin(te) * sin(fi));

		if ((!map->inside(zd, xd, yd)) ||
		        (!map->pass_lof_cell(zd, xd, yd)))
			break;
		if (platoon_remote->check_for_hit(zd, xd, yd) ||
		        platoon_local->check_for_hit(zd, xd, yd))
			break;
	}

	xg = map->x + x0 + y0;
	yg = (int)(map->y - (x0 + 1) / 2 + y0 / 2 - z0 * 2.0 - 2);

	xg2 = map->x + xd + yd;
	yg2 = (int)(map->y - (xd + 1) / 2 + yd / 2 - zd * 2.0 - 2);

	//line(screen2, xg, yg, xg2, yg2, 144);
	do_line(screen2, xg, yg, xg2, yg2, xcom1_color(144), dotted_line_proc);


	i = 1000;
	zd = (int)(z0 + i * cos(fi));
	xd = (int)(x0 + i * cos(te) * sin(fi));
	yd = (int)(y0 + i * sin(te) * sin(fi));

	xg = xg2;
	yg = yg2;

	xg2 = map->x + xd + yd;
	yg2 = (int)(map->y - (xd + 1) / 2 + yd / 2 - zd * 2.0 - 2);

	//line(screen2, xg, yg, xg2, yg2, 144);
	do_line(screen2, xg, yg, xg2, yg2, xcom1_color(150), dotted_line_proc);
}


void Bullet::showthrow(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
	int xd, yd, zd;
	int color = 50;

	x0 = x_s * 16 + 8; y0 = y_s * 16 + 8; z0 = z_s * 12 + 8;
	xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 0;

	ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
	//if (ro > 18.0*16)
	//	color = 33;

	fi = acos((zd - z0) / ro);
	te = atan2(yd - y0, xd - x0);

	REAL zA = sqrt(ro);
	REAL zK = 4 * zA / ro / ro;

	x = x0; y = y0; z = z0;
	i = 8;
	int throwable = 1;
	//while(i < ro) {
	while (z > 0) {
		if (i > 18.0 * 16)
			color = 33;       // red
		else
			color = 50;       // green

		x = (int)(x0 + i * cos(te) * sin(fi));
		y = (int)(y0 + i * sin(te) * sin(fi));
		//z = z0 + i * cos(fi) + sqrt(ro)*sin(i*3.1415926/ro);
		z = (int)(z0 + i * cos(fi) - zK * (i - ro / 2.0) * (i - ro / 2.0) + zA);

		if (throwable) {
			if ((!map->inside(z, x, y)) ||
			        (!map->pass_lof_cell(z, x, y)))
				throwable = 0;
			//break;
			if (platoon_remote->check_for_hit(z, x, y) ||
			        platoon_local->check_for_hit(z, x, y))
				throwable = 0;
			//break;
		}

		int xg = map->x + x + y;
		int yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

		if ((xg > -32) && (xg < SCREEN2W) && (yg >= -34) && (yg < SCREEN2H)) {
			if (throwable)
				circle(screen2, xg, yg, 1, color);
			else
				putpixel(screen2, xg, yg, color + 4);
		}
		i++;
	}
}


int Bullet::calc_throw(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
	int xd, yd, zd;
	//int color = 50;

	x0 = x_s; y0 = y_s; z0 = z_s;
	xd = x_d; yd = y_d; zd = z_d;

	ro = sqrt((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0));
	//if (ro > 18.0*16)
	//	color = 33;

	fi = acos((zd - z0) / ro);
	te = atan2(yd - y0, xd - x0);

	REAL zA = sqrt(ro);
	REAL zK = 4 * zA / ro / ro;

	x = x0; y = y0; z = z0;
	i = 8;

	//while(i < ro) {
	while (z > 0) {
		i++;
		//if (i > 18*16)
		// color = 33;  // red
		//else
		//	color = 50;  // green

		x = (int)(x0 + i * cos(te) * sin(fi));
		y = (int)(y0 + i * sin(te) * sin(fi));
		//z = z0 + i * cos(fi) + sqrt(ro)*sin(i*3.1415926/ro);
		z = (int)(z0 + i * cos(fi) - zK * (i - ro / 2.0) * (i - ro / 2.0) + zA);

		//int xg = map->x + x + y;
		//int yg = map->y - (x+1)/2.0 + y/2.0 - z*2.0 - 2;

		//if ( (xg>-32)&&(xg<SCREEN2W)&&(yg>=-34)&&(yg<SCREEN2H) ) {
		//	circle(screen2, xg, yg, 1, color);
		//}
	}
	return i;
}


int Bullet::explodable()
{
	if ((type == CANNON_HE_AMMO) ||
	        (type == AUTO_CANNON_HE_AMMO) ||
	        (type == SMALL_ROCKET) ||
	        (type == LARGE_ROCKET) ||
	        (type == INCENDIARY_ROCKET) ||
			(type == STUN_MISSILE))
		return 1;
	return 0;
}

int Bullet::incendiary() {
	if ((type == INCENDIARY_ROCKET)||
		(type == AUTO_CANNON_I_AMMO) ||
		(type == CANNON_I_AMMO))
		return 1;
	return 0;
}

void Bullet::detonate()
{
	int range = Item::explo_range(type);
	int damage = Item::obdata[type].damage;

	if (net->SEND) {
		net->SEND = 0;
		map->explode(owner, lev, col, row, type, range, damage);
		net->SEND = 1;
	} else
		map->explode(owner, lev, col, row, type, range, damage);
}

void Bullet::hitman()
{
	// Here, we figure out which direction the bullet is facing.
	// 567
	// 4 0
	// 321
	int hitdir;
	REAL theangle = te;

	while (theangle < 0)
		theangle += (2 * PI);
	while (theangle > (2 * PI))
		theangle -= (2 * PI);

	if (theangle < (PI / 8))
		hitdir = 0;
	else if (theangle < ((3 * PI) / 8))
		hitdir = 7;
	else if (theangle < ((5 * PI) / 8))
		hitdir = 6;
	else if (theangle < ((7 * PI) / 8))
		hitdir = 5;
	else if (theangle < ((9 * PI) / 8))
		hitdir = 4;
	else if (theangle < ((11 * PI) / 8))
		hitdir = 3;
	else if (theangle < ((13 * PI) / 8))
		hitdir = 2;
	else if (theangle < ((15 * PI) / 8))
		hitdir = 1;
	else
		hitdir = 0;


	map->apply_hit(z, x, y, type);
	platoon_remote->apply_hit(owner, z, x, y, type, hitdir);
	platoon_local->apply_hit(owner, z, x, y, type, hitdir);
}

bool Bullet::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, item);

	return true;
}

bool Bullet::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, item);

	return true;
}
