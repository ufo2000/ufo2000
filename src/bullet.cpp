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

#include "stdafx.h"

#include "global.h"
#include "bullet.h"
#include "video.h"
#include "sound.h"
#include "map.h"
#include "platoon.h"
#include "explo.h"
#include "multiplay.h"
#include "colors.h"
#include "script_api.h"

#define PHASE 2

std::vector<ALPHA_SPRITE *> Bullet::hit_bullet, Bullet::hit_laser, Bullet::hit_plasma, Bullet::hit_punch;

IMPLEMENT_PERSISTENCE(Bullet, "Bullet");

Bullet::Bullet(Soldier *man)
{
    state = READY;
    phase = 0;
    item  = NULL;
    owner = man->get_NID();
    affected = new std::vector<int>;
    if (hit_bullet.empty())
        hit_bullet = lua_table_image_vector("hit_bullet");
    if (hit_laser.empty())
        hit_laser = lua_table_image_vector("hit_laser");
    if (hit_plasma.empty())
        hit_plasma = lua_table_image_vector("hit_plasma");
    if (hit_punch.empty())
        hit_punch = lua_table_image_vector("hit_punch");
}

Bullet::~Bullet()
{
    delete affected;
}

/**
 * Called when a Hand to hand weapon is used.
 */
void Bullet::punch(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
    //play(S_PUNCH);
    state = ST_PUNCH;

    z0 = _z0; x0 = _x0; y0 = _y0;
    fi = _fi; te = _te;
    type = _type;

    for (i = 3; i < 24; i++) {
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
/*
    x0 += (int)(8 * cos(te) * sin(fi));
    y0 += (int)(8 * sin(te) * sin(fi));
    z0 += (int)(8 * cos(fi));
*/
    i = 0;
}
/**
 * Called when a projectile weapon is fired.
 */
void Bullet::fire(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
    state = FLY;
    affected->clear();

    z0 = _z0; x0 = _x0; y0 = _y0;
    fi = _fi; te = _te;
    type = _type;

    i = 3;
    move();
}
/**
 * Called when a beam weapon is fired.
 */
void Bullet::beam(int _z0, int _x0, int _y0, REAL _fi, REAL _te, int _type)
{
    state = BEAM;
    affected->clear();

    z0 = _z0; x0 = _x0; y0 = _y0;
    fi = _fi; te = _te;
    type = _type;
    
    int cx = -1, cy = -1, cz = -1;
    bool new_check = false;

    for (i = 3; i < 100000; i++) {
        z = (int)(z0 + i * cos(fi));
        x = (int)(x0 + i * cos(te) * sin(fi));
        y = (int)(y0 + i * sin(te) * sin(fi));
        
        if (cz != z / 12) { cz = z / 12; new_check = true; }
        if (cx != x / 16) { cx = x / 16; new_check = true; }
        if (cy != y / 16) { cy = y / 16; new_check = true; }
        
        if (new_check)
            affect_morale(cz, cx, cy);

        if ((!map->inside(z, x, y)) ||
                (!map->pass_lof_cell(z, x, y)))
            break;
        if (platoon_remote->check_for_hit(z, x, y) ||
                platoon_local->check_for_hit(z, x, y)
           )
            break;
    }

/*
    x0 += (int)(8 * cos(te) * sin(fi));
    y0 += (int)(8 * sin(te) * sin(fi));
    z0 += (int)(8 * cos(fi));
*/
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
    type = item->itemtype();

    i = 3;
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

/**
 * Includes most routines that define where a bullet or projectile is located during a specified frame.
 */
void Bullet::move()
{
    int j;
    REAL zK;
    static int cx, cy, cz;
    bool new_check = false;

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
                
                if (cz != z / 12) { cz = z / 12; new_check = true; }
                if (cx != x / 16) { cx = x / 16; new_check = true; }
                if (cy != y / 16) { cy = y / 16; new_check = true; }

                if (new_check)
                    affect_morale(cz, cx, cy);

                if ((!map->inside(z, x, y)) ||
                        (!map->pass_lof_cell(z, x, y)) ||
                        platoon_remote->check_for_hit(z, x, y) ||
                        platoon_local->check_for_hit(z, x, y)
                   ) {
                    explodes = map->inside(z, x, y);
                    hitcell();
                    state = HIT;
                    
                    soundSystem::getInstance()->play(Item::obdata_get_sound(type));
/*
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
*/
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
                if (explodable() && explodes) {
                    detonate();
                    item  = NULL;
                    state = READY;
                    break;
                }
            }
            if (incendiary() && explodes) {
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

/**
 * Bullet trail animation
 */
void Bullet::draw_bullet_trace(int length, int color)
{
    double dx = x - x0;
    double dy = y - y0;
    double dz = z - z0;

    double total_trace_length = sqrt(dx * dx + dy * dy + dz * dz);

    int trace_x0 = x0, trace_y0 = y0, trace_z0 = z0;

    if (total_trace_length > length) {
        trace_x0 = (int)(x - dx * length / total_trace_length);
        trace_y0 = (int)(y - dy * length / total_trace_length);
        trace_z0 = (int)(z - dz * length / total_trace_length);
    }

    int xg1 = map->x + trace_x0 + trace_y0;
    int yg1 = (int)(map->y - (trace_x0 + 1) / 2.0 + trace_y0 / 2.0 - trace_z0 * 2.0 - 2);

    int xg2 = map->x + x + y;
    int yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

    for (int i = 0; i < length; i++) {
        double k = (double)i / (double)length;
        k = k * k;
        int x = (int)(xg2 + (xg1 - xg2) * k);
        int y = (int)(yg2 + (yg1 - yg2) * k);

        putpixel(screen2, x, y, color);
    }

    for (int trail_frame_index = 0; ; trail_frame_index++) {
        ALPHA_SPRITE *trail_frame = Item::obdata_get_bitmap(type, "trailAnim", trail_frame_index + 1);
        if (!trail_frame) break;

        double u = (double)trail_frame_index / 2.8; // bitmap trail is longer than normal trail
        int x3 = (int)(xg2 + (xg1 - xg2) * u);
        int y3 = (int)(yg2 + (yg1 - yg2) * u);
        draw_alpha_sprite(screen2, trail_frame, x3 - 16, y3 - 16);
    }
}
/**
 * Bullet animation, includes all bullet states and hit animations
 */
void Bullet::draw()
{
    int xg, yg;
    double xt, yt, zt;
    int xg2, yg2;
    int xe, ye, ze;

    switch (state) {
        case READY:
            break;

        case FLY: {
            if (!platoon_local->is_visible(z / 12, x / 16, y / 16)) return;
            
            xg = map->x + x + y;
            yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

            int len = 15, c = 16;

            if (Item::can_set_color(type)) {
                len = Item::obdata_trailLength(type);
                draw_bullet_trace(len, Item::get_color(type, 2));
                putpixel(screen2, xg, yg, Item::get_color(type, 0));
                circle(screen2, xg, yg, 1, Item::get_color(type, 1));
            } else {
                if (Item::obdata_damageType(type) == DT_PLAS) {
                    len = 30;
                    c = 52;
                }

                draw_bullet_trace(len, xcom1_color(c + 5));
                putpixel(screen2, xg, yg, xcom1_color(c));
                circle(screen2, xg, yg, 1, xcom1_color(c + 2));
            }
            break;
        }
        case BEAM:
            /*xg = map->x + x0 + y0;
            yg = (int)(map->y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

            xg2 = map->x + x + y;
            yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

            line(screen2, xg, yg, xg2, yg2, xcom1_color(143 + i));*/
            for (int j = 3; j < 100000; j++) {
                zt = z0 + j * cos(fi);
                xt = x0 + j * cos(te) * sin(fi);
                yt = y0 + j * sin(te) * sin(fi);
                
                if ((!map->inside((int)zt, (int)xt, (int)yt)) ||
                    (!map->pass_lof_cell((int)zt, (int)xt, (int)yt)))
                    break;
                if (platoon_remote->check_for_hit((int)zt, (int)xt, (int)yt) ||
                    platoon_local->check_for_hit((int)zt, (int)xt, (int)yt))
                    break;

                if (!platoon_local->is_visible((int)(zt) / 12, (int)(xt) / 16, (int)(yt) / 16))
                    continue;
                
                xg = (int)(map->x + xt + yt);
                yg = (int)(map->y - (xt + 1) / 2.0 + yt / 2.0 - zt * 2.0 - 2);
                
                if (Item::can_set_color(type)) {
                    //the following fades the beam to black
                    //i is the beam animation state and goes from 0 to 9, see Beam() and Move()
                    int blended_color = 0;
                    int it_col = Item::get_color(type, 1);
                    blended_color = makecol(getr(it_col) - i * getr(it_col) / 10, getg(it_col) - i * getg(it_col) / 10, getb(it_col) - i * getb(it_col) / 10);
                    putpixel(screen2, xg, yg, blended_color);
                }else {
                    putpixel(screen2, xg, yg, xcom1_color(143 + i));
                }
            }
            break;

        case THROWN:
            if (!platoon_local->is_visible(z / 12, x / 16, y / 16)) return;

            xg = map->x + x + y;
            yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

            if ((xg > -32) && (xg < SCREEN2W) && (yg >= -34) && (yg < SCREEN2H)) {
                map->drawitem(item->obdata_pMap(), xg - 16, yg - 26);
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

            line(screen2, xg, yg, xg2, yg2, COLOR_WHITE);

            /*if ( (xg>-32)&&(xg<SCREEN2W)&&(yg>=-34)&&(yg<SCREEN2H) ) {
             map->drawitem(item->data()->pMap, xg-16, yg-26);
             //circle(screen2, xg, yg, 1, 32);
            }*/
            break;

        case ST_PUNCH:
            if (!platoon_local->is_visible(z0 / 12, x0 / 16, y0 / 16)) return;
        
            xg = map->x + x0 + y0;
            yg = (int)(map->y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

            //xg2 = map->x + xd + yd;
            //yg2 = map->y - (xd+1)/2 + yd/2 - zd*23.5/12.0;
            xg2 = map->x + x + y;
            yg2 = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

            line(screen2, xg, yg, xg2, yg2, xcom1_color(143 + i));
            break;

        case HIT:
            if (!platoon_local->is_visible(z / 12, x / 16, y / 16)) return;
        
            xg = map->x + x + y;
            yg = (int)(map->y - (x + 1) / 2.0 + y / 2.0 - z * 2.0 - 2);

            if ((xg > -32) && (xg < SCREEN2W) && (yg >= -34) && (yg < SCREEN2H)) {

                ALPHA_SPRITE *hit_frame = Item::obdata_get_bitmap(type, "hitAnim", 1 + (phase / PHASE));
                if (hit_frame) {
                    draw_alpha_sprite(screen2, hit_frame, xg - 15, yg - 16);
                }else if (!Item::obdata_get_bitmap(type, "hitAnim", 1)) {
                    ALPHA_SPRITE *default_frame = NULL;
                    int frame_id = (phase / PHASE);
                    if (Item::obdata_isHandToHand(type)) {
                        if(frame_id < (int)hit_punch.size()) {
                            default_frame = hit_punch[frame_id];
                        }
                    }else {
                        switch (Item::obdata_damageType(type)) {
                            case DT_LAS:
                                if(frame_id < (int)hit_laser.size()) {
                                    default_frame = hit_laser[frame_id];
                                }
                                break;
                            case DT_PLAS:
                                if(frame_id < (int)hit_plasma.size()) {
                                    default_frame = hit_plasma[frame_id];
                                }
                                break;
                            default:
                                if(frame_id < (int)hit_bullet.size()) {
                                    default_frame = hit_bullet[frame_id];
                                }
                                break;
                        }
                    }
                    if (default_frame) {
                        draw_alpha_sprite(screen2, default_frame, xg - 15, yg - 16);
                    }
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

    sel_man->calc_bullet_start(x_s, y_s, z_s, &x0, &y0, &z0); //start point as made in soldier::shoot
    xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 8;  //target in the center

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));

    int i;
    for (i = 3; i < 100000; i++) {
        zd = (int)(z0 + i * cos(fi));
        xd = (int)(x0 + i * cos(te) * sin(fi));
        yd = (int)(y0 + i * sin(te) * sin(fi));

        // hit obstacle or fly out of map borders
        if ((!map->inside(zd, xd, yd)) || (!map->pass_lof_cell(zd, xd, yd)))
            break;

        // hit enemy soldier, only visible soldiers are taken into account in 
        // order to prevent cheating
        if (platoon_remote->check_for_hit(zd, xd, yd) && platoon_local->is_visible(zd / 12, xd / 16, yd / 16))
            break;
        
        // hit local soldier (with a workaround preventing hit check for selected soldier)
        if (platoon_local->check_for_hit(zd, xd, yd, sel_man)) 
            break;
    }

    xg = map->x + x0 + y0;
    yg = (int)(map->y - (x0 + 1) / 2 + y0 / 2 - z0 * 2.0 - 2);

    xg2 = map->x + xd + yd;
    yg2 = (int)(map->y - (xd + 1) / 2 + yd / 2 - zd * 2.0 - 2);

    do_line(screen2, xg, yg, xg2, yg2, COLOR_YELLOW, dotted_line_proc);


    i = 1000;
    zd = (int)(z0 + i * cos(fi));
    xd = (int)(x0 + i * cos(te) * sin(fi));
    yd = (int)(y0 + i * sin(te) * sin(fi));

    xg = xg2;
    yg = yg2;

    xg2 = map->x + xd + yd;
    yg2 = (int)(map->y - (xd + 1) / 2 + yd / 2 - zd * 2.0 - 2);

    //line(screen2, xg, yg, xg2, yg2, 144);
    do_line(screen2, xg, yg, xg2, yg2, COLOR_BROWN, dotted_line_proc);
}


void Bullet::showthrow(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
    int xd, yd, zd;
    int color = 50;

    x0 = x_s * 16 + 8; y0 = y_s * 16 + 8; z0 = z_s * 12 + 8;
    xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 0;

    ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));

    fi = acos((zd - z0) / ro);
    te = atan2((double)(yd - y0), (double)(xd - x0));

    REAL zA = sqrt(ro);
    REAL zK = 4 * zA / ro / ro;

    x = x0; y = y0; z = z0;
    i = 8;
    int throwable = 1;

    while (z > 0) {
        if (i > 18.0 * 16)
            color = 33;       // red
        else
            color = 50;       // green

        x = (int)(x0 + i * cos(te) * sin(fi));
        y = (int)(y0 + i * sin(te) * sin(fi));
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

    x0 = x_s; y0 = y_s; z0 = z_s;
    xd = x_d; yd = y_d; zd = z_d;

    ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));

    fi = acos((double)((zd - z0) / ro));
    te = atan2((double)(yd - y0), (double)(xd - x0));

    REAL zA = sqrt(ro);
    REAL zK = 4 * zA / ro / ro;

    x = x0; y = y0; z = z0;
    i = 8;

    while (z > 0) {
        i++;

        x = (int)(x0 + i * cos(te) * sin(fi));
        y = (int)(y0 + i * sin(te) * sin(fi));
        z = (int)(z0 + i * cos(fi) - zK * (i - ro / 2.0) * (i - ro / 2.0) + zA);
    }
    return i;
}


int Bullet::explodable()
{
    if (Item::obdata_damageType(type) == DT_HE)
        return 1;
    if (Item::obdata_damageType(type) == DT_STUN && !Item::obdata_isHandToHand(type))
        return 1;
    return 0;
}

int Bullet::incendiary()
{
    if (Item::obdata_damageType(type) == DT_INC)
        return 1;
    return 0;
}

void Bullet::detonate()
{
    map->explode(owner, z, x, y, type);
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

void Bullet::affect_morale(int cz, int cx, int cy)
{
    int sNID;
    bool cont;
    std::vector<int>::iterator itr;

    for (int l = cz - 1; l <= cz + 1; l++) {
        for (int c = cx - 1; c <= cx + 1; c++) {
            for (int r = cy - 1; r <= cy + 1; r++) {
                if (!map->cell_inside(l, c, r)) continue;
                if (!map->man(l, c, r)) continue;

                if (map->man(l, c, r)->get_platoon()->findman(owner)) continue;
                sNID = map->man(l, c, r)->get_NID();
                if (sNID == owner) continue;

                cont = false;
                itr = affected->begin();
                while (itr != affected->end()) {
                    if (sNID == *itr) {
                        cont = true;
                        break;
                    }
                    itr++;
                }
                if (cont) continue;

                map->man(l, c, r)->change_morale(-2);
                affected->push_back(sNID);
            }
        }
    }
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
    
    affected = new std::vector<int>;

    return true;
}
