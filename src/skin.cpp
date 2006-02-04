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

#include "skin.h"
#include "soldier.h"
#include "pck.h"
#include "spk.h"
#include "video.h"
#include "map.h"

SKIN_INFO g_skins[] =
{
//    name,        SkinType,    fFemale, fFlying, armour_values, cost
    { "male",      S_XCOM_0,    0, 0, { 12,  8,  8,  5,  2}, 70 },
    { "female",    S_XCOM_0,    1, 0, { 12,  8,  8,  5,  2}, 70 },
    { "armour_m",  S_XCOM_1,    0, 0, { 50, 40, 40, 30, 30}, 380 },
    { "armour_f",  S_XCOM_1,    1, 0, { 50, 40, 40, 30, 30}, 380 },
    { "power_m",   S_XCOM_2,    0, 0, {100, 80, 80, 70, 60}, 780 },
    { "power_f",   S_XCOM_2,    1, 0, {100, 80, 80, 70, 60}, 780 },
    { "fly_m",     S_XCOM_3,    0, 1, {110, 90, 90, 80, 70}, 1030 },
    { "fly_f",     S_XCOM_3,    1, 1, {110, 90, 90, 80, 70}, 1030 },
    { "sectoid",   S_SECTOID,   0, 0, {  4,  3,  3,  2,  2}, 28 },
    { "muton",     S_MUTON,     0, 0, { 70, 60, 60, 50, 50}, 580 },
    { "snakeman",  S_SNAKEMAN,  0, 0, { 40, 30, 30, 20, 20}, 280 }, //LAWYER:  Added Snakeman
    { "ethereal",  S_ETHEREAL,  0, 1, { 60, 50, 50, 40, 40}, 630 }, //LAWYER:  Added Ethereal
    { "floater",   S_FLOATER,   0, 1, { 20, 15, 15, 10, 10}, 290 }, //LAWYER:  Added Floater
    { "chameleon", S_CHAMELEON, 0, 0, { 70, 60, 60, 50, 50}, 580 },
};

int g_skins_count = sizeof(g_skins) / sizeof(g_skins[0]);

int get_skin_index(int SkinType, int fFemale)
{
    for (int i = 0; i < g_skins_count; i++) {
        if (g_skins[i].SkinType == SkinType && g_skins[i].fFemale == fFemale)
            return i;
    }
    return fFemale ? 1 : 0;
}

char *****Skin::m_bof = NULL;
PCK **Skin::m_pck = NULL;
PCK *Skin::m_add1 = NULL;
BITMAP *Skin::m_image = NULL;
SPK *Skin::m_spk[9][2][4] = {{{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
                                {{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}}};

IMPLEMENT_PERSISTENCE(Skin, "Skin");

Skin::Skin(Soldier *soldier, int skin_type, int female_flag)
{
    m_soldier = soldier;
    skin_info = g_skins[get_skin_index(skin_type, female_flag)];
}

void Skin::initpck()
{
    static char *skin_fname[] = {
        "handob.pck", "xcom_0.pck", "xcom_1.pck", "xcom_2.pck", "xcom_2.pck",
        "sectoid.pck", "muton.pck", "snakeman.pck", "ethereal.pck", "floater.pck",
        "celatid.pck", "silacoid.pck", "chrys.pck", "civm.pck", "civf.pck",
        "zombie.pck"
    };

    ASSERT(sizeof(skin_fname) / sizeof(skin_fname[0]) >= SKIN_NUMBER);

    m_pck = new PCK *[SKIN_NUMBER];
    char fname[100];
    for (int n = 0; n < SKIN_NUMBER; n++) {
        m_pck[n] = NULL;
        sprintf(fname, "$(xcom)/units/%s", skin_fname[n]);
        int fh = open(F(fname), O_RDONLY | O_BINARY);
        if (fh != -1) {
            close(fh);
            m_pck[n] = new PCK(fname, false, 32, 40);
        }
    }

    if (FLAGS & F_CONVERT_XCOM_DATA) {
        for (int n = SKIN_NUMBER; n < (int)(sizeof(skin_fname) / sizeof(skin_fname[0])); n++) {
            sprintf(fname, "$(xcom)/units/%s", skin_fname[n]);
            PCK *tmp = new PCK(fname, false, 32, 40);
            delete tmp;
        }
    }

    m_add1 = new PCK("$(ufo2000)/newunits/add1.pck", false, 32, 40);

    for (int ar = 0; ar < 2; ar++)
        for (int se = 0; se < 2; se++)
            for (int ty = 0; ty < 4; ty++) {
                sprintf(fname, "$(xcom)/ufograph/man_%d%c%d.spk", ar, se == 0 ? 'm' : 'f', ty);
                m_spk[ar][se][ty] = new SPK(fname);
            }

    m_spk[2][0][0] = new SPK("$(xcom)/ufograph/man_2.spk");
    m_spk[3][0][0] = new SPK("$(xcom)/ufograph/man_3.spk");

    m_spk[4][0][0] = new SPK("$(ufo2000)/newunits/sectoid.spk");
    m_spk[5][0][0] = new SPK("$(ufo2000)/newunits/muton.spk");
    m_spk[6][0][0] = new SPK("$(ufo2000)/newunits/snakeman.spk"); //LAWYER:  Snakeman backpack view.  Needs an SPK
    m_spk[7][0][0] = new SPK("$(ufo2000)/newunits/ethereal.spk"); //LAWYER:  Ethereal backpack view.  Needs an SPK
    m_spk[8][0][0] = new SPK("$(ufo2000)/newunits/floater.spk"); //LAWYER:  Floater backpack view.  Needs an SPK    
    
    m_image = create_bitmap(32, 40);
    
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

void Skin::initbof()
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

void Skin::freepck()
{
    for (int n = 0; n < SKIN_NUMBER; n++)
        delete m_pck[n];
    delete [] m_pck;
    delete m_add1;

    for (int ar = 0; ar < 2; ar++)
        for (int se = 0; se < 2; se++)
            for (int ty = 0; ty < 4; ty++)
                delete m_spk[ar][se][ty];

    delete m_spk[2][0][0];
    delete m_spk[3][0][0];
    delete m_spk[4][0][0];
    delete m_spk[5][0][0];
    delete m_spk[6][0][0]; //LAWYER:  Neatness purposes, i think
    delete m_spk[7][0][0];
    delete m_spk[8][0][0];
    
    destroy_bitmap(m_image);
    
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

void Skin::update(int skin_type, int female_flag)
{
    skin_info = g_skins[get_skin_index(skin_type, female_flag)];
}

void Skin::next_human()
{
    m_soldier->md.SkinType++;
    
    if(m_soldier->md.SkinType > S_XCOM_3)
        m_soldier->md.SkinType = S_XCOM_0;
}

void Skin::next_alien()
{
    m_soldier->md.SkinType++;

    if(m_soldier->md.SkinType < S_SECTOID)
        m_soldier->md.SkinType = S_SECTOID;

    if(m_soldier->md.SkinType > S_FLOATER) //LAWYER:  Shifted by one.
        m_soldier->md.SkinType = S_SECTOID;
}

void Skin::draw_head(int Appearance, int head_frame, int dir, BITMAP *image, int delta)
{
    // we don't have a picture for black complection, so change it to brunette
    // when displaying head
    if (Appearance == 3) Appearance = 2;

    if (skin_info.SkinType == S_XCOM_0 && (Appearance == 1 || Appearance == 2)) {
        m_add1->drawpck(dir + (Appearance - 1) * 8 + skin_info.fFemale * 16, image, delta);
    } else if (skin_info.SkinType == S_XCOM_1 && (Appearance == 1 || Appearance == 2)) {
        m_add1->drawpck(dir + 32 + (Appearance - 1) * 8 + skin_info.fFemale * 16, image, delta);
    } else {
        m_pck[skin_info.SkinType]->drawpck(dir + head_frame, image, delta);      //head
    }
}

#define Y_SIT 5
#define MAPX 0
#define MAPY (-18)

class UnitSprite
{
    BITMAP *m_bmp;
public:
    UnitSprite(BITMAP *bmp) { m_bmp = bmp; }
    void draw(int x, int y, ALPHA_SPRITE *spr) { draw_alpha_sprite(m_bmp, spr, x, y); }
};

/**
 * Call for user made unit rendering function implemented in lua
 */
void Skin::draw_lua()
{
    State state = m_soldier->state();
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z;
    int dir = m_soldier->dir, phase = m_soldier->phase, is_flying = m_soldier->is_flying();
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item();

    ALPHA_SPRITE *weapon_hold = NULL;
    ALPHA_SPRITE *weapon_aim  = NULL;
    if (rhand_item != NULL) {
        weapon_hold = rhand_item->obdata_pHeld(dir);
        weapon_aim = rhand_item->obdata_pHeld((dir + 2) % 8);
    } else if (lhand_item != NULL) {
        weapon_hold = lhand_item->obdata_pHeld(dir);
        weapon_aim = lhand_item->obdata_pHeld((dir + 2) % 8);
    }

    int gx = g_map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = g_map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();

    BITMAP *image = m_image;
    clear_to_color(m_image, xcom1_color(0));
    UnitSprite sprite(m_image);

    int stack_top = lua_gettop(L);

    LUA_REGISTER_CLASS(L, UnitSprite);
    LUA_REGISTER_CLASS_METHOD(L, UnitSprite, draw);

    lua_pushstring(L, "UnitsTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1));
    lua_pushnumber(L, m_soldier->md.Appearance);
    lua_gettable(L, -2);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_pushnumber(L, 0);
        lua_gettable(L, -2);
    }
    ASSERT(lua_istable(L, -1));
    lua_pushstring(L, "pMap");
    lua_gettable(L, -2);
    ASSERT(lua_isfunction(L, -1));

    LPCD::Push(L, &sprite);
    lua_pushnumber(L, dir);
    lua_pushnumber(L, (state == MARCH || state == FALL) ? phase + 1 : phase);
    lua_newtable(L);
    if (weapon_hold) {
        lua_pushstring(L, "gun");
        LPCD::Push(L, weapon_hold);
        lua_settable(L, -3);
        lua_pushstring(L, "hand_object_image");
        LPCD::Push(L, weapon_hold);
        lua_settable(L, -3);
        if (weapon_aim) {
            lua_pushstring(L, "aimed_hand_object_image");
            LPCD::Push(L, weapon_aim);
            lua_settable(L, -3);
        }
    }
    if (state == FALL) {
        lua_pushstring(L, "collapse");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
        lua_pushstring(L, "is_falling");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if (state == SIT) {
        lua_pushstring(L, "crouch");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
        lua_pushstring(L, "is_crouching");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if (is_flying) {
        lua_pushstring(L, "is_flying");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    lua_pushstring(L, "appearance");
    lua_pushnumber(L, m_soldier->md.Appearance);
    lua_settable(L, -3);

    lua_safe_call(L, 4, 0);
    lua_settop(L, stack_top);

    int ox, oy;
    if (dir < 3) ox = 1; else if ((dir == 3) || (dir == 7)) ox = 0; else ox = -1;
    if ((dir == 1) || (dir == 5)) {
        oy = 0; ox *= 2;
    } else
            if ((dir > 1) && (dir < 5)) oy = -1; else oy = 1;
    if ((dir == 3) || (dir == 7)) {
        ox = 0; oy *= 2;
    }

    if (state == MARCH) {
        if (phase < 4)
            draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy - 6);
        else
            draw_sprite(screen2, image, gx + (phase - 8) * 2 * ox, gy - (phase - 8) * oy - 6);
    } else {
        draw_sprite(screen2, image, gx, gy - 6);
    }
}

void Skin::draw_common() //LAWYER:  I'm going to attempt to explain this as I go through it, as it's a bit of a mess.
{
    //-----Initialisation-----
    State state = m_soldier->state(); //LAWYER:  Current state, such as crouch, or flight?
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z; //LAWYER:  Map positions?
    int dir = m_soldier->dir, phase = m_soldier->phase, is_flying = m_soldier->is_flying(); //Direction, animation phase, flying state?
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item(); //Items in hand
    
    int head_frame = 32; //LAWYER:  Likely has something to do with the start of the head position in an anim sequence
    //-----End Initialisation-----
    
    // The female appearance of the power suit is the flying suit.
    // This icky-looking switch causes the proper suit to be displayed regardless of gender.
    switch (skin_info.SkinType)
    {
        default:
            if (!skin_info.fFemale) break;
        case S_XCOM_3:
            head_frame = 16 * 16 + 11;
        case S_XCOM_2:
            break;
    }
    //LAWYER:  Placement in the map, I'd assume
    int gx = g_map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = g_map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();
    
    //LAWYER:  If we're falling, skip any calculations for building the character and draw the death sequence
    if (state == FALL) {
        m_pck[skin_info.SkinType]->showpck(264 + phase / 3, gx, gy);
        return ;
    }
    
    int arm1, arm2, army, handob_y = 0;
    int yofs;

    BITMAP *image = m_image;
    clear_to_color(image, xcom1_color(0));

    arm1 = 0; arm2 = 0; army = 5;
    if (dir < 4) {
        arm2 = 1;
    } else {
        arm1 = 1;
    }

    if (state == SIT) handob_y += 4;
    if (skin_info.SkinType == S_SECTOID) handob_y += 6; // $$$

    if ((lhand_item != NULL) || (rhand_item != NULL)) {
        if (state != MARCH) {
            arm1 += 30;
            arm2 += 30;
        }
    }
    if ((dir < 1) || (dir > 5)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, handob_y);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, handob_y);
    }

    switch (state) {
        case FALL: case LIE: break;      //neverhap
        case SIT:
            m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, Y_SIT);
//          m_pck[md.SkinType]->drawpck(dir + head_frame, image, Y_SIT + 1);      //head
            draw_head(m_soldier->md.Appearance, head_frame, dir, image, Y_SIT + 1);
            if (skin_info.SkinType == S_SECTOID)
                m_add1->drawpck(dir + 16 * 4 + 8, image, 2);
            else if (skin_info.SkinType == S_MUTON)
                m_add1->drawpck(dir + 16 * 4 + 0, image, 2);
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * 3, image, 2);
            m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, Y_SIT);
            break;
        case STAND:
            m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, 0);
//          m_pck[md.SkinType]->drawpck(dir + head_frame, image, 0);      //head
            draw_head(m_soldier->md.Appearance, head_frame, dir, image, 0);
            if (is_flying && m_pck[skin_info.SkinType]->m_imgnum >= 275)
                m_pck[skin_info.SkinType]->drawpck(275 + dir, image, 0);
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * 2, image, 0);
            m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, 0);
            break;
        case MARCH:
            if (phase % 4 == 0)
                yofs = -1;
            else
                if (phase % 2 == 1)
                    yofs = 0;
                else
                    yofs = 2;

            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                arm1 += 30;
                arm2 += 30;
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, 0);
            } else {
                m_pck[skin_info.SkinType]->drawpck(phase + (dir * 3 + 7 - 1 - arm2) * 8, image, yofs);
            }

//          m_pck[md.SkinType]->drawpck(dir + head_frame, image, 0);      //head
            draw_head(m_soldier->md.Appearance, head_frame, dir, image, 0);
            int yo = 0;
            if (phase % 4 == 0) yo = -1;

            if (is_flying && m_pck[skin_info.SkinType]->m_imgnum >= 275)
                m_pck[skin_info.SkinType]->drawpck(275 + dir, image, 0);
            else
                m_pck[skin_info.SkinType]->drawpck(phase + (dir * 3 + 7) * 8, image, yo);     //yofs);

            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, 0);
            } else {
                m_pck[skin_info.SkinType]->drawpck(phase + (dir * 3 + 7 - 1 - arm1) * 8, image, yofs);
            }
            break;
    }

    if ((dir > 0) && (dir < 6)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, handob_y);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, handob_y);
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
}

void Skin::draw()
{
    if (skin_info.SkinType == S_CHAMELEON) {
        draw_lua();
        return;
    }

    if (m_pck[skin_info.SkinType] == NULL) {
        // HACK: no original x-com sprite available, just draw what we have
        draw_lua();
        return;
    }

    switch (skin_info.SkinType) {
        case S_SNAKEMAN:
            draw_snakeman();
            break;
        case S_ETHEREAL:
            draw_ethereal();
            break;
        case S_FLOATER:
            draw_floater();
            break;
        default:
            draw_common();
    }
}


bool Skin::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);

    PersistWriteObject(archive, m_soldier);

    return true;
}

bool Skin::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);

    PersistReadObject(archive, m_soldier);

    return true;
}

/**
 * LAWYER: Because Snakemen have a weird number of frames, as compared to the 
 * default ones, it needs its own function.
 */
void Skin::draw_snakeman()
{
    //-----Initialisation-----
    State state = m_soldier->state(); //LAWYER:  Current state, such as crouch, or flight?
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z; //LAWYER:  Map positions?
    int dir = m_soldier->dir, phase = m_soldier->phase, is_flying = m_soldier->is_flying(); //Direction, animation phase, flying state?
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item(); //Items in hand, pointers to them
    
    int head_frame = 24; //LAWYER:  The snakeman head animations start at frame 24.
    //-----End Initialisation-----
    

    //LAWYER:  Placement in the map, I'd assume
    int gx = g_map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = g_map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();
    
    //LAWYER:  If we're falling, skip any calculations for building the character and draw the death sequence
    if (state == FALL) {
        m_pck[skin_info.SkinType]->showpck(96 + phase / 3, gx, gy); //LAWYER:  96 is the start of the death frames for Snakemen
        return ;
    }
    
    int arm1, arm2, army, handob_y = 0;
    int yofs;

    BITMAP *image = m_image;
    clear_to_color(image, xcom1_color(0));

    arm1 = 0; arm2 = 0; army = 5;
    if (dir < 4) {
        arm2 = 1;
    } else {
        arm1 = 1;
    }
    

    

 //   if (state == SIT) handob_y += 4; //Move the handobject down by four in a crouch sequence
//    if (skin_info.SkinType == S_SECTOID) handob_y += 6; // $$$

//LAWYER;  The position of the hand items
    if ((lhand_item != NULL) || (rhand_item != NULL)) {
        if (state != MARCH) {
            arm1 += 13;
            arm2 += 13; //LAWYER:  The Snakeman weapon animations are at sequence 13, I think
        }
    }
    //LAWYER:  If we're facing away from the camera, draw the weapons first.
    if ((dir < 1) || (dir > 5)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, handob_y);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, handob_y);
    }

    switch (state) {
        case FALL: case LIE: break;      //neverhap
        case SIT: //LAWYER:  Can Snakemen even crouch??
        case STAND: //LAWYER:  Motionless
            if ((lhand_item != NULL) || (rhand_item != NULL))
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm1) + 3, image, 0); //LAWYER: Arm #1, with weapon
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, 0); //LAWYER: Arm #1, without weapon
            
            m_pck[skin_info.SkinType]->drawpck(dir + head_frame, image, 0);      //LAWYER:  As there's only one possible head option with the snakeman, we can use the redundant method of drawing head images

            m_pck[skin_info.SkinType]->drawpck(dir + 8 * 2, image, 0); //LAWYER;  Legs, I assume.  Same frame numbers as a standard sprite
            if ((lhand_item != NULL) || (rhand_item != NULL))
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm2) + 3, image, 0); //LAWYER; The other arm, with weapon
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, 0); //LAWYER: Arm #2, without weapon
                
            break;
        case MARCH: //LAWYER:  Walking.  This can probably be cut right down, because Aliens don't pump their arms when walking.  Here's where it gets crashy!
        
        yofs = 0;
        //LAWYER:  Calculation for offset ofarms and head, I assume
        switch (phase) //LAWYER:  I know this is completely ugly, but there's no pattern to the yofs on a snakeman, so this is the only way I could figure it out
        {
        case 0:
            yofs = 3;
            break;
        case 1:
            yofs = 3;
            break;
        case 2:
            yofs = 2;
            break;
        case 3:
            yofs = 2;
            break;
        case 4:
            yofs = 1;
            break;
        case 5:
            yofs = 1;
            break;
        case 6:
            yofs = 2;
            break;
        case 7:
            yofs = 3;
            break;
        }

            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                arm1 += 13;
                arm2 += 13;
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm1) + 3, image, yofs);
            } else {
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, yofs); //LAWYER: Arm #1, without weapon
            }
            m_pck[skin_info.SkinType]->drawpck(32 + phase + (dir * 8), image, 0);      //Tail
        m_pck[skin_info.SkinType]->drawpck(dir + head_frame, image, yofs);      //LAWYER:  As there's only one possible head option with the snakeman, we can use the redundant method of drawing head images
        int yo = 0; //LAWYER:  Y offset for legs
//            if (phase % 4 == 0) yo = -1;
            

 //               m_pck[skin_info.SkinType]->drawpck(phase + (dir * 3 + 7) * 8, image, yo);     //yofs); //LAWYER:  Moving legs

            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm2) + 3, image, yofs);
            } else {
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, yofs); //LAWYER: Arm #1, without weapon
            }
            break;
    }
    //LAWYER:  If facing the camera, draw weapons last
    if ((dir > 0) && (dir < 6)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, handob_y);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, handob_y);
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

    //LAWYER:  I'm at a loss as to what this part does, but it looks important.
    if (state == MARCH) {
        if (phase < 4)
            draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
        else
            draw_sprite(screen2, image, gx + (phase - 8) * 2 * ox, gy - (phase - 8) * oy);
    } else
        draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
}

/**
 * LAWYER: Ethereals are made of one sprite, so can skip all of the sprite building tech
 */
void Skin::draw_ethereal() 
{

    State state = m_soldier->state();
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z;
    int dir = m_soldier->dir, phase = m_soldier->phase, is_flying = m_soldier->is_flying();
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item();
    
    int gx = g_map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = g_map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();

    
    int arm1, arm2, army, handob_y = 0;
    int yofs;

    BITMAP *image = m_image;
    clear_to_color(image, xcom1_color(0));

    arm1 = 0; arm2 = 0; army = 5;
    if (dir < 4) {
        arm2 = 1;
    } else {
        arm1 = 1;
    }


    //LAWYER:  If we're falling, skip any calculations for building the character and draw the death sequence
    if (state == FALL) {
        m_pck[skin_info.SkinType]->showpck(8 * 9 + (phase / 3), gx, gy); //LAWYER: Ported from the Ethereal LUA file
        return;
    }
    
    //LAWYER:  If we're facing away from the camera, draw the weapons first.
    if ((dir < 1) || (dir > 5)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, 0);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, 0);
    }

    switch (state) {
        case FALL: case LIE: break;      //neverhap
        case SIT:
        
        case STAND: //LAWYER:  Motionless
                m_pck[skin_info.SkinType]->drawpck(dir, image, 0);//LAWYER:  More magic porting            
                break;
        case MARCH: //LAWYER:  Walking. 
                if (!is_flying) //LAWYER:  So the Ethereals don't kick their legs as they float
                    m_pck[skin_info.SkinType]->drawpck(8 + dir * 8 + phase, image, 0);      //LAWYER:  Ported again
                else
                    m_pck[skin_info.SkinType]->drawpck(dir, image, 0);
                break;
    }
    //LAWYER:  If facing the camera, draw weapons last
    if ((dir > 0) && (dir < 6)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, 0);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, 0);
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

    //LAWYER:  I'm at a loss as to what this part does, but it looks important.
    if (state == MARCH) {
        if (phase < 4)
            draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
        else
            draw_sprite(screen2, image, gx + (phase - 8) * 2 * ox, gy - (phase - 8) * oy);
    } else
        draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
}

/**
 * LAWYER:  Floaters don't have legs, so require less building
 */
void Skin::draw_floater() //LAWYER:  The Floater.  Note that due to animation oddities, the arm1 and arm2 variables are swapped in the drawing of arms without weapons
{
    #define FLOATER_ARM 2 //LAWYER:  Position of floater arms, made a DEFINE for ease of editing
    
    State state = m_soldier->state(); //LAWYER:  Current state, such as crouch, or standing, walking, or dying.  Floaters don't have a crouch animation
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z; //LAWYER:  Map positions
    int dir = m_soldier->dir, phase = m_soldier->phase, is_flying = m_soldier->is_flying(); //Direction, animation phase, flying state
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item(); //Items in hand, pointers to them
    
    //LAWYER:  Placement in the map, to do with drawing the object
    int gx = g_map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = g_map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();
    
    //LAWYER:  If in the "Death" state, skip everything else and play it.
    if (state == FALL) {
        m_pck[skin_info.SkinType]->showpck(64 + phase / 3, gx, gy);
        return ;
    }
    
    int arm1, arm2, army, handob_y = 0;

    BITMAP *image = m_image;
    clear_to_color(image, xcom1_color(0)); //LAWYER:  This sets the colour pallet for the object.  As it's a UFO creature, it gets XCOM1

    arm1 = 0; arm2 = 0; army = 5;
    if (dir < 4) { //LAWYER:  Adjust the choice of which arm is drawn first based on direction
        arm2 = 1;
    } else {
        arm1 = 1;
    }

//LAWYER;  The position of the hand items
    if ((lhand_item != NULL) || (rhand_item != NULL)) {
        if (state != MARCH) {
            arm1 += 8;
            arm2 += 8; //LAWYER:  The start of the hand animations when holding items
        }
    }
    //LAWYER:  If we're facing away from the camera, draw the weapons first.
    if ((dir < 1) || (dir > 5)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, 0);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, 0);
    }

    switch (state) {
        case FALL: case LIE: break;      //neverhap
        case SIT: //LAWYER:  Floaters can't crouch, they have no knees D:
        
        case STAND: //LAWYER:  Motionless
        
        //LAWYER:  Due to the odd way Floaters are handled, the torso must be drawn at different steps
        //The craziness works as so:
        // 0, 6, 7 - Body needs to be drawn last.
        // 3 - body needs to be drawn first 
        
            if (dir == 3) //IF we're facing downards, draw the body first and all of the arms on top of it
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * 2, image, 0);
        
            if ((lhand_item != NULL) || (rhand_item != NULL))
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm1) + 3, image, FLOATER_ARM); //LAWYER: Arm behind body, with weapon
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, 0); //LAWYER: Arm behind body, without weapon
            
            if (dir == 1 || dir == 2 || dir == 4 || dir == 5)
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * 2, image, 0); //LAWYER:  Normal body drawing, between the arms
                
            if ((lhand_item != NULL) || (rhand_item != NULL))
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm2) + 3, image, FLOATER_ARM); //LAWYER; Arm in front of body, with weapon
            else
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, 0); //LAWYER: Arm in front of body, without weapon

            if (dir == 0 || dir == 6 || dir == 7)
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * 2, image, 0); //LAWYER:  Facing away from camera, draw body last               
            break;
            
        case MARCH: //LAWYER:  Walking
            
            if (dir == 3)
                m_pck[skin_info.SkinType]->drawpck(24 + (phase / 5) + (dir * 5), image, 0); //Due to an odd number of walk animations, floaters need a special formula
                
            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                arm1 += 8;
                arm2 += 8;
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm1) + 3, image, FLOATER_ARM); //With weapon
            } else {
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm2, image, 0); //LAWYER: Arm #1, without weapon
            }
            
            if (dir == 1 || dir == 2 || dir == 4 || dir == 5)
            {
                    m_pck[skin_info.SkinType]->drawpck(24 + (phase / 5) + (dir * 5), image, 0);
            }   

            if ((lhand_item != NULL) || (rhand_item != NULL)) {
                m_pck[skin_info.SkinType]->drawpck((dir + 8 * arm2) + 3, image, FLOATER_ARM);
            } else {
                m_pck[skin_info.SkinType]->drawpck(dir + 8 * arm1, image, 0); //LAWYER: Arm #1, without weapon
            }
            
            if (dir == 0 || dir == 6 || dir == 7)
            {
                    m_pck[skin_info.SkinType]->drawpck(24 + (phase / 5) + (dir * 5), image, 0);
            }
                
            break;
    }
    
    //LAWYER:  If facing the camera, draw weapons last
    if ((dir > 0) && (dir < 6)) {
        if (rhand_item != NULL)
            PCK::showpck(image, rhand_item->obdata_pHeld(dir), 0, 0);
        else if (lhand_item != NULL)
            PCK::showpck(image, lhand_item->obdata_pHeld(dir), 0, 0);
    }

    //LAWYER:  I'm at a loss as to the purpose of this, but it looks important
    int ox, oy;
    if (dir < 3) ox = 1; else if ((dir == 3) || (dir == 7)) ox = 0; else ox = -1;
    if ((dir == 1) || (dir == 5)) {
        oy = 0; ox *= 2;
    } else
            if ((dir > 1) && (dir < 5)) oy = -1; else oy = 1;
    if ((dir == 3) || (dir == 7)) {
        ox = 0; oy *= 2;
    }

    if (state == MARCH) {
        if (phase < 4)
            draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
        else
            draw_sprite(screen2, image, gx + (phase - 8) * 2 * ox, gy - (phase - 8) * oy);
    } else
        draw_sprite(screen2, image, gx + phase * 2 * ox, gy - phase * oy);
}
