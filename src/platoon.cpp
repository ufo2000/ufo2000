/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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
#include "platoon.h"
#include "units.h"
#include "video.h"
#include "multiplay.h"
#include "map.h"
#include "stats.h"

IMPLEMENT_PERSISTENCE(Platoon, "Platoon");

Platoon::Platoon(int PID, int num)
{
    StatEntry *current;
    ID = PID;
        
    size = num;
    memset(m_seen, 0, sizeof(m_seen));
    //Number of levels should be set relative to map max number of levels
    for (int i = 0; i < MAP_LEVEL_LIMIT; i++)
        for (int j = 0; j < 10 * 6; j++)
            for (int k = 0; k < 10 * 6; k++)
                m_seen_item_index[i][j][k] = -1;

    m_stats = new Statistics(size);
    current = m_stats->getfirst();

    Soldier *s1 = NULL, *s2;
    for (int i = 0; i < size; i++) {
        s2 = new Soldier(this, i+PID);
        current->set_name("Soldier"); // This will NEVER be seen.
        current->set_SID(i+PID);
        current = current->getnext();
        if (s1 != NULL) {
            s1->set_next(s2);
            s2->set_prev(s1);
        } else {
            man = s2;
        }
        s1 = s2;
    }
}

Platoon::Platoon(int PID, PLAYERDATA * pd, DeployType dep_type)
{
    StatEntry *current;
    ID = PID;
    size = pd->size;
    memset(m_seen, 0, sizeof(m_seen));
    
    for (int i = 0; i < map->level; i++)
        for (int j = 0; j < 10 * 6; j++)
            for (int k = 0; k < 10 * 6; k++)
                m_seen_item_index[i][j][k] = -1;

    m_stats = new Statistics(size);
    current = m_stats->getfirst();

    Soldier *s1 = NULL, *s2;
    int i;
    int32 vision_mask = 1;
    for (i = 0; i < size; i++) {
        s2 = new Soldier(this, i+PID, pd->lev[i], pd->col[i], pd->row[i], &pd->md[i], &pd->id[i], dep_type,vision_mask);
        current->set_name(pd->md[i].Name);
        current->set_SID(i+PID);
        current = current->getnext();
        if (s1 != NULL) {
            s1->set_next(s2);
            s2->set_prev(s1);
        } else {
            man = s2;
        }
        s1 = s2;
        vision_mask = vision_mask << 1;
    }
}


Platoon::~Platoon()
{
    destroy();
}


void Platoon::destroy()
{
    Soldier *s = man;
    while (s != NULL) {
        Soldier *tmp = s->next();
        delete s;
        s = tmp;
    }

    delete m_stats;
}

void Platoon::move(int ISLOCAL)
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (ss->move(ISLOCAL) == 0) { //dead, or stunned
            if (ss->is_dead()) // dead. New captain for platoon needed.
            {
                if (ss == man)
                    man = man->nextman();      //!!ret this if no other
                if (ss == man) {
                    man = NULL;
                }
            }
            if (ss == sel_man) sel_man = NULL;
            Soldier *s = ss;
            ss = ss->next();
            if (s->is_dead()) {
                map->clear_vision_matrix(s);
                s->die();
                size--;
                            
                if (this == platoon_local)
                    platoon_remote->soldier_moved(s);
                else
                    platoon_local->soldier_moved(s);
                            
                delete s;
            } else if (s->x != -1) {
                map->clear_vision_matrix(s);
                s->stun();
                if (this == platoon_local)
                    platoon_remote->soldier_moved(s);
                else
                    platoon_local->soldier_moved(s);
            }
            
        } else {
            ss = ss->next();
        }
    }

}

void Platoon::bullmove()
{
    Soldier *ss = man;
    while (ss != NULL) {
        ss->bullet()->move();
        ss = ss->next();
    }
}


void Platoon::bulldraw()
{
    Soldier *ss;

    ss = man;
    while (ss != NULL) {
        ss->bullet()->draw();
        ss = ss->next();
    }
}

/**
 * Restore minimal amount of parameters - even for Watch mode
 */
void Platoon::restore_moved()
{
    Soldier *ss = man;
    while (ss != NULL) {
        ss->restore_moved();
        ss = ss->next();
    }
}

/**
 * Restore full amount of parameters - at the beginning of the turn
 */
void Platoon::restore()
{
    Soldier *ss = man;
    while (ss != NULL) {
        ss->restore();
        ss = ss->next();
    }
}


Soldier *Platoon::findman(int NID)
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (ss->NID == NID)
            break;
        ss = ss->next();
    }
    return ss;
}

/**
 * Loop through all soldiers in platoon and force a vision update
 * Used only at game start.
 */
void Platoon::initialize_vision_matrix()
{
    memset(m_vision_matrix, 0, map->size() * sizeof(int32));
    Soldier *ss = man;
    while (ss != NULL) {
        if ( ss->is_active() )
            map->update_vision_matrix(ss);
        ss = ss->next();
    }
}

int Platoon::is_seen(int lev, int col, int row) 
{
    return m_seen[lev][col][row] || net->gametype == GAME_TYPE_REPLAY;
}

int Platoon::is_visible(int lev, int col, int row) 
{
    return m_vision_matrix[Position(lev,col,row).index()] || (net->gametype == GAME_TYPE_REPLAY);
}

/**
 * When a soldier moves into a new voxel, tell all of the enemy
 * soldiers who can see that voxel that he is there.  This avoids
 * having to unneccesarily recalculate those soldier's visibility.
 */ 
void Platoon::soldier_moved(Soldier* mover)
{
    Soldier *ss = man;
    int32 index = Position(mover->z, mover->x, mover->y).index();
    int32 vision_mask = mover->get_vision_mask();
    while (ss != NULL) {
        if ( ss->is_active()) {
            if(mover->is_active() && m_vision_matrix[index] & ss->get_vision_mask())
                ss->set_visible_enemies(ss->get_visible_enemies() | vision_mask);
            else
                ss->set_visible_enemies(ss->get_visible_enemies() & ~vision_mask);
        }
        ss = ss->next();
    }
}

/**
 * Loop through all soldiers and append their visible enemies
 * to the platoon.
 */
int32 Platoon::update_visible_enemies()
{
    m_visible_enemies = 0;
    Soldier *ss = man;
    while (ss != NULL) {
        if ( ss->is_active() )
            m_visible_enemies |= ss->get_visible_enemies();
        ss = ss->next();    
    }

    return m_visible_enemies;
}

/**
 * Queue all enemy soldiers who can see the target.
 * Then check if each soldier can take a shot at target.
 */
int Platoon::check_reaction_fire(Soldier *target)
{
    // Reaction fire isn't calculated in replay mode
    if (net->gametype == GAME_TYPE_REPLAY || g_game_receiving)
        return 0;

    std::vector<Soldier *> soldiers;
    Soldier *ss = man;
    int32 n = Position(target->z, target->x, target->y).index();
    while (ss != NULL) {
        if ( ss->is_active() ) {
            // Can we see this cell?
            if( get_vision_matrix()[n] & ss->get_vision_mask() ){
                soldiers.push_back(ss);
            }
        }
        ss = ss->next();
    }

    while (!soldiers.empty()) {
        int index = rand() % soldiers.size();
        if (soldiers[index]->check_reaction_fire(target))
            return 1;
        soldiers.erase(soldiers.begin() + index);
    }
    
    return 0;
}

/**
 * Create appropriate bar with a digit in the right bottom corner 
 * for enemies that are seen.  
 */
#define ES_SIDE 15
void Platoon::draw_enemy_indicators(bool draw_indicators, bool draw_markers) 
{
    if (m_visible_enemies == 0 )
        return;
    
    text_mode( -1);
    char num[2] = {0, 0};
    int32 counter = 0;
        
    Soldier* s = platoon_remote->man;
    while (s != NULL && counter < 10){
        if (m_visible_enemies & s->get_vision_mask()){
            
            // Start at 1 instead of 0.
            num[0] = ((counter + 1) % 10) + '0';

            if (draw_indicators) {
                int x1, y1, x2, y2;
                x1 = SCREEN2W - ES_SIDE - 3;
                y1 = SCREEN2H - 23 - counter * 20;
        
                x2 = x1 + ES_SIDE;
                y2 = y1 + ES_SIDE - 1;
                
                // Draw it bright red if the selected soldier can see 
                // the enemy, otherwise make the indicator dull red.
                if (sel_man != NULL)
                    rectfill(screen2, x1, y1, x2, y2, (sel_man->get_visible_enemies() & s->get_vision_mask()) ? COLOR_RED07 : COLOR_RED15);
        
                textout(screen2, font, num, x1 + 5, y1 + 4, COLOR_GOLD);
            }

            counter++;
            
            // Draw numbers above seen enemies
            if (draw_markers) {
                int sx = g_map->x + CELL_SCR_X * s->x + CELL_SCR_X * s->y + 12;
                int sy = g_map->y - (s->x + 1) * CELL_SCR_Y + CELL_SCR_Y * s->y - 29 - CELL_SCR_Z * s->z;

                // Draw a number over enemy head
                textout(screen2, font, num, sx, sy - 2, COLOR_WHITE);

                // Draw blue triangle with its point at sx, sy and height 5
                sx += 3; sy += 10; int j;
                for (j = 0; j < 5; j++) {
                    line(screen2,     sx - j, sy - j, sx + j, sy - j, COLOR_SKYBLUE03);
                    putpixel(screen2, sx - j, sy - j, COLOR_BLACK1);
                    putpixel(screen2, sx + j, sy - j, COLOR_BLACK1);
                }
                line(screen2, sx - j, sy - j, sx + j, sy - j, COLOR_BLACK1);
            }
        }
        s = s->next();
    }
}

/**
 * Process mouseclick on numbered buttons for seen enemy soldier,
 * center map on selected soldier.
 */
int Platoon::center_enemy_seen()
{
    int counter = 0; // Number displayed inside visible enemy indicator
    Soldier* s = platoon_remote->man;
    while (s != NULL) {
        if (m_visible_enemies & s->get_vision_mask()) {
            int x1, y1, x2, y2;
            x1 = SCREEN2W - ES_SIDE - 3;
            y1 = SCREEN2H - 23 - counter * 20;
            
            x2 = x1 + ES_SIDE;
            y2 = y1 + ES_SIDE - 1;

            counter++;        
            if (mouse_inside(x1, y1, x2, y2)) {
                map->center(s);
                return 1;
            }
        }
        s = s->next();
    }
    return 0;
}


Soldier *Platoon::findman(char *sn)
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (strcmp(ss->md.Name, sn) == 0)
            break;
        ss = ss->next();
    }
    return ss;
}


Soldier *Platoon::findnum(int N)
{
    Soldier *ss = man;
    for (int i = 1; i <= N; i++) {
        if (ss == NULL)
            break;
        ss = ss->next();
    }
    return ss;
}

/**
 * Returns pointer to the next soldier who is not still marked as moved.
 * When reaching end of soldiers list, the search is wrapped around to the 
 * first soldier
 */
Soldier *Platoon::next_not_moved_man(Soldier *sel_man)
{
    if (sel_man == NULL || !sel_man->is_active()) sel_man = captain();
    if (sel_man == NULL) return NULL;

    Soldier *ss = sel_man->next_active_soldier();
    while (ss != NULL) {
        if (!ss->MOVED) return ss;
        ss = ss->next_active_soldier();
    }

    ss = captain();
    while (ss != sel_man) {
        if (!ss->MOVED) return ss;
        ss = ss->next_active_soldier();
    }

    return sel_man;
}


int Platoon::nobullfly()
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (!ss->bullet()->ready())
            return 0;
        ss = ss->next();
    }
    return 1;
}


int Platoon::nomoves()
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (ss->ismoving())
            return 0;
        ss = ss->next();
    }
    if (nobullfly())
        return 1;
    return 0;
}


int Platoon::belong(Soldier *some)
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (ss == some)
            return 1;
        ss = ss->next();
    }
    return 0;
}


int Platoon::belong(Bullet *some)
{
    Soldier *ss = man;
    while (ss != NULL) {
        if (ss->bullet() == some)
            return 1;
        ss = ss->next();
    }
    return 0;
}


int Platoon::realsize()
{
    int n = 0;
    Soldier *ss = man;
    while (ss != NULL) {
        n++;
        ss = ss->next();
    }
    size = n;
    return n;
}


int Platoon::calc_platoon_cost()
{
    int n = 0;
    Soldier *ss = man;
    
    while (ss != NULL) {
        n += ss->calc_full_ammunition_cost();
        ss = ss->next();
    }

    return n;
}


Place *Platoon::find_item(Item *it, int &lev, int &col, int &row)
{
    Soldier *ss = man;

    while (ss != NULL) {
        Place *ip = ss->find_item(it, lev, col, row);
        if (ip != NULL)
            return ip;
        ss = ss->next();
    }
    return NULL;
}

int Platoon::find_place_coords(Place *pl, int &lev, int &col, int &row)
{
    Soldier *ss = man;
    
    while (ss != NULL) {
        int pf = ss->find_place_coords(pl, lev, col, row);
        if (pf)
            return 1;
        ss = ss->next();
    }
    return 0;
}


int Platoon::check_for_hit(int z, int x, int y, Soldier* no_test)
{
    Soldier *ss = man;

    int v = 0;
    while (ss != NULL) {
        if(no_test != ss)
            v |= ss->check_for_hit(z, x, y);
        ss = ss->next();
    }
    return v;
}


void Platoon::apply_hit(int sniper, int z, int x, int y, int type, int hitdir)
{
    Soldier *ss = man;

    while (ss != NULL) {
        ss->apply_hit(sniper, z, x, y, type, hitdir);
        ss = ss->next();
    }
}

int Platoon::dist_to_nearest(Soldier *some)
{
    int res = -1, cur;
    Soldier *ss = man;
    
    while (ss != NULL) {
        if (ss != some) {
            cur = (int)sqrt((some->x - ss->x) * (some->x - ss->x) +
                            (some->y - ss->y) * (some->y - ss->y));
            if ((res == -1) || (cur < res))
                res = cur;
        }

        ss = ss->next();
    }
    
    return res;
}

void Platoon::change_morale(int delta, bool send_to_remote)
{
    Soldier *ss = man;

    if (! g_game_receiving || !send_to_remote)
    while (ss != NULL) {
        ss->change_morale(delta);   
        ss = ss->next();
    }
    
    if (send_to_remote)
        net->send_morale_change(delta);
}

void Platoon::check_morale()
{
    bool panicked = false;
    Soldier *ss = man;
    
    while (ss != NULL) {
        if (rand() % 100 < 100 - 2 * ss->ud.Morale) {
            int action = rand() % 2;
            ss->panic(action);
            net->send_panic(ss->get_NID(), action);
            panicked = true;
        }
        ss = ss->next();
    }
    
    if (!panicked)
        change_morale(5, true);     
}

/**
 * Save full team information (soldiers, their stats, equipment) into a binary file
 */
void Platoon::save_FULLDATA(const char *fn)
{
    std::string str;
    save_to_string(str);
    FILE *f = fopen(F(fn), "wt");
    if (f == NULL) {
        alert(" ", "Can't create specified file", " ", "    OK    ", NULL, 1, 0);
        return;     
    }
    std::string x = "return {\n" + indent(str) + "}\n";
    fprintf(f, "%s", x.c_str());
    fclose(f);

    // !!! The next code is a hack added to workaround a problem 
    // when soldiers start the battle unarmed (because of some weird
    // code in send/receive team data protocol implementation)
    Soldier *ss = man;
    while (ss != NULL) {
        ss->build_ITEMDATA();
        ss = ss->next();
    }
}

void Platoon::save_to_string(std::string &str)
{
    str.clear();

    Soldier *ss = man;
    while (ss != NULL) {
        str += "{\n";
        std::string soldier_str;
        ss->save_to_string(soldier_str);
        str += indent(soldier_str);
        ss = ss->next();
        str += "},\n";
    }
}

/**
 * Load full team information (soldiers, their stats, equipment) from a binary file
 */
void Platoon::load_FULLDATA(const char *fn)
{
    lua_pushstring(L, "LoadSquad");
    lua_gettable(L, LUA_GLOBALSINDEX);
    
    // load team information as a table on the top of lua stack
    int stack_top = lua_gettop(L);
    lua_safe_dofile(L, F(fn), "restricted_sandbox");
    lua_settop(L, stack_top + 1);
    LUA_PUSH_OBJECT_POINTER(L, this);
    lua_safe_call(L, 2, 0);

    // !!! The next code is a hack added to workaround a problem 
    // when soldiers start the battle unarmed (because of some weird
    // code in send/receive team data protocol implementation)
    Soldier *ss = man;
    while (ss != NULL) {
        ss->build_ITEMDATA();
        ss = ss->next();
    }
}


void Platoon::build_Units(Units &u)
{
    Soldier * ss = man;
    //u.reset();
    int num = 0;
    while (ss != NULL) {
        //strcpy(u.name[u.size], ss->md.Name);
        //u.cost[u.size] = ss->calc_ammunition_cost();
        //u.size++;
        u.add(num, ss->md.Name, ss->calc_ammunition_cost());
        net->send_add_unit(num, ss->md.Name, ss->calc_ammunition_cost());
        num++;
        ss = ss->next();
    }
}

/**
 * Send information about our team to the remote player
 */
void Platoon::send_Units(Units &u)
{
    net->send_unit_data_size(0);
    int i;
    int num = 0;
    for (i = 0; i < u.size; i++) {
        if (!u.is_selected(i))
            continue;
        Soldier *ss = findman(u.name[i]);
        if (ss != NULL) {
            pd_local->lev[num] = u.lev[i];
            pd_local->col[num] = u.col[i];
            pd_local->row[num] = u.row[i];
            memcpy(&pd_local->md[num], &ss->md, sizeof(MANDATA));
            memcpy(&pd_local->id[num], &ss->id, sizeof(ITEMDATA));
            net->send_unit_data(num, u.lev[i], u.col[i], u.row[i],
                                &ss->md, &ss->id);
            num++;
        }
    }
    pd_local->size = num;
    net->send_unit_data_size(num);
}

/**
 * Output information about team in a text buffer.
 * It is used for synchronization bugs detection.
 */
int Platoon::eot_save(char *buf, int &buf_size)
{
    buf_size += sprintf(buf + buf_size, "\r\nplatoon:\r\nID=%ld size=%d\r\n", ID, realsize());

    Soldier *ss = man;
    while (ss != NULL) {
            buf_size += ss->eot_save(buf + buf_size);
        ss = ss->next();
    }

    return buf_size;
}

bool Platoon::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);

    PersistWriteObject(archive, man);

    PersistWriteObject(archive, m_stats);

    return true;
}

bool Platoon::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);

    PersistReadObject(archive, man);

    PersistReadObject(archive, m_stats);

    return true;
}

void Platoon::sit_on_start()
{
    Soldier *ss = man;
    while (ss != NULL) {
            ss->set_start_sit();
        ss = ss->next();
    }
}
