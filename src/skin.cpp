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
    { "male",    S_XCOM_0,  0, { 12,  8,  8,  5,  2} },
    { "female",  S_XCOM_0,  1, { 12,  8,  8,  5,  2} },
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
SPK *Skin::m_spk[6][2][4] = {{{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL}},
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
        sprintf(fname, "$(xcom)/units/%s", skin_fname[n]);
        m_pck[n] = new PCK(fname, false, 32, 40);
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
    
    m_image = create_bitmap(32, 32);
    
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

    if(m_soldier->md.SkinType > S_MUTON)
        m_soldier->md.SkinType = S_SECTOID;
}

int Skin::get_armour_cost(int skin_type, int female_flag)
{
    int p = 0;

    for (int i = 0; i < 5; i++)
        p += 2 * g_skins[get_skin_index(skin_type, female_flag)].armour_values[i];
        
    return p;
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

void Skin::draw_common()
{
    State state = m_soldier->state();
    int x = m_soldier->x, y = m_soldier->y, z = m_soldier->z;
    int dir = m_soldier->dir, phase = m_soldier->phase;
    Item *lhand_item = m_soldier->lhand_item(), *rhand_item = m_soldier->rhand_item();
    
    int head_frame = 32;

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

    int gx = map->x + CELL_SCR_X * x + CELL_SCR_X * y;
    int gy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - 18 - z * CELL_SCR_Z;
    gy += m_soldier->calc_z();

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
    //for different races (NIY)
    switch(skin_info.SkinType) {
        default:
            draw_common();
            break;
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
