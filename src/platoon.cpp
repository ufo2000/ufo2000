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
	
	for (int i = 0; i < 4; i++)
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

	m_visibility_changed = 1;
}

Platoon::Platoon(int PID, PLAYERDATA * pd, DeployType dep_type)
{
	StatEntry *current;
	ID = PID;
	size = pd->size;
	memset(m_seen, 0, sizeof(m_seen));
	
	for (int i = 0; i < 4; i++)
        for (int j = 0; j < 10 * 6; j++)
            for (int k = 0; k < 10 * 6; k++)
                m_seen_item_index[i][j][k] = -1;

	m_stats = new Statistics(size);
	current = m_stats->getfirst();

	Soldier *s1 = NULL, *s2;
	int i;
	for (i = 0; i < size; i++) {
		s2 = new Soldier(this, i+PID, pd->lev[i], pd->col[i], pd->row[i], &pd->md[i], &pd->id[i], dep_type);
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
	}
	m_visibility_changed = 1;
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

void Platoon::recalc_visibility()
{
	if (!m_visibility_changed) return;

	memset(m_visible, 0, sizeof(m_visible));

	Soldier *ss = man;
	while (ss != NULL) {
		if (ss->is_active()) {
			ss->calc_visible_cells();
			int n = 0, k, i, j, width_10 = 10 * map->width, height_10 = 10 * map->height;
			for (k = 0; k < map->level; k++)
				for (i = 0; i < width_10; i++)
					for (j = 0; j < height_10; j++) {
						m_visible[k][i][j] |= ss->m_visible_cells[n++];
						m_seen[k][i][j] |= m_visible[k][i][j];

                        if (m_visible[k][i][j]) {
                            if (map->place(k, i, j)->top_item())
                                m_seen_item_index[k][i][j] = map->place(k, i, j)->top_item()->itemtype();
                            else
                                m_seen_item_index[k][i][j] = -1;
                        }
					}
		}
		ss = ss->next();
	}

	m_visibility_changed = 0;
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
				s->die();
				size--;
				delete s;
				platoon_local->set_visibility_changed();
				platoon_remote->set_visibility_changed();
			} else if (s->x != -1) {
				s->stun();
				platoon_local->set_visibility_changed();
				platoon_remote->set_visibility_changed();
			}
			
		} else {
			ss = ss->next();
		}
	}

	recalc_visibility();
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


// See also: Soldier::draw_blue_selector()
void Platoon::draw_blue_selectors()
{
	Soldier *ss = man;
	while (ss != NULL) {
		ss->draw_blue_selector();
		ss = ss->next();
	}
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

int Platoon::check_reaction_fire(Soldier *target)
{
	std::vector<Soldier *> soldiers;
	Soldier *ss = man;
	while (ss != NULL) {
		if (ss->is_active()) soldiers.push_back(ss);
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

void Platoon::change_morale(int delta, bool send_to_remote)
{
	Soldier *ss = man;

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
		//if (rand() % 100 < 100 - 2 * ss->ud.Morale) {
		if (false) {
			int action = rand() % 2;
			ss->panic(action);
			net->send_panic(ss->get_NID(), action);
			panicked = true;
		}
		ss = ss->next();
	}
	
	if (!panicked)				//balance issue - maybe should be disabled
		change_morale(5, true);		
}

/**
 * Save full team information (soldiers, their stats, equipment) into a binary file
 */
void Platoon::save_FULLDATA(const char *fn)
{
	int fh = open(F(fn), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	if (fh == -1) {
		alert(" ", "Can't create specified file", " ", "    OK    ", NULL, 1, 0);
		return;		
	}
	Soldier *ss = man;
	while (ss != NULL) {
		write(fh, &ss->md, sizeof(ss->md));
		ss->build_ITEMDATA();
		write(fh, &ss->id, sizeof(ss->id));
		ss = ss->next();
	}
	close(fh);
}

void Platoon::save_to_string(std::string &str)
{
	str.clear();

	Soldier *ss = man;
	while (ss != NULL) {
		char tmp[512];
		sprintf(tmp, "[%d] = {\n", ss->get_NID());
		str += tmp;
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
	char *buf;
	unsigned int buf_size;

	int fh = open(F(fn), O_RDONLY | O_BINARY);
	if (fh == -1) {
		alert(" ", "Can't open specified file", " ", "    OK    ", NULL, 1, 0);
		return;		
	}

	buf_size = filelength(fh);
	buf = new char[buf_size];
	buf_size = read(fh, buf, buf_size);
	close(fh);

	Soldier * ss = man;
	unsigned int ofs = 0;
	while (ss != NULL) {
		if (ofs + sizeof(ss->md) + sizeof(ss->id) > buf_size)
			break;

		memcpy(&ss->md, buf + ofs, sizeof(ss->md));
		ss->process_MANDATA();
		ofs += sizeof(ss->md);

		memcpy(&ss->id, buf + ofs, sizeof(ss->id));
		ss->process_ITEMDATA();
		ofs += sizeof(ss->id);

		ss = ss->next();
	}
	delete [] buf;
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
