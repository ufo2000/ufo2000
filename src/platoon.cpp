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
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "platoon.h"
#include "video.h"
#include "multiplay.h"
#include "map.h"

IMPLEMENT_PERSISTENCE(Platoon, "Platoon");

int VISIBILITY_CHANGED = 1;

Platoon::Platoon(int PID, int num)
{
	ID = PID;
	size = num;
	memset(m_seen, 0, sizeof(m_seen));

	Soldier *s1 = NULL, *s2;
	for (int i = 0; i < size; i++) {
		s2 = new Soldier(i);
		if (s1 != NULL) {
			s1->set_next(s2);
			s2->set_prev(s1);
		} else {
			man = s2;
		}
		s1 = s2;
	}
}

Platoon::Platoon(int PID, PLAYERDATA * pd)
{
	ID = PID;
	size = pd->size;
	memset(m_seen, 0, sizeof(m_seen));

	Soldier *s1 = NULL, *s2;
	for (int i = 0; i < size; i++) {
		s2 = new Soldier(i, pd->lev[i], pd->col[i], pd->row[i], &pd->md[i], &pd->id[i]);
		if (s1 != NULL) {
			s1->set_next(s2);
			s2->set_prev(s1);
		} else {
			man = s2;
		}
		s1 = s2;
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
}


int RECALC_VISIBILITY = 1;

void Platoon::move(int ISLOCAL)
{
	Soldier *ss = man;
	while (ss != NULL) {
		if (ss->move(ISLOCAL) == 0) { //dead
			if (ss == man)
				man = man->nextman();      //!!ret this if no other
			if (ss == man) {
				//if (ISLOCAL)
				// net->send_message("\n...\n...\nall soldier are dead\n...\n...\n");
				man = NULL;
			}
			if (ss == sel_man) sel_man = NULL;
			Soldier *s = ss;
			ss = ss->next();
			s->die();
			size--;
			delete s;
			RECALC_VISIBILITY = 1;
		} else {
			ss = ss->next();
		}
	}
	if (RECALC_VISIBILITY && ISLOCAL) {
		ss = man;
		while (ss != NULL) {
			ss->calc_visible_cells();
			ss = ss->next();
		}
		VISIBILITY_CHANGED = 1;
		RECALC_VISIBILITY = 0;
	}

	if (ISLOCAL && VISIBILITY_CHANGED) {
		char visible_cells[4 * 6 * 10 * 6 * 10];
		memset(visible_cells, 0, sizeof(visible_cells));
		ss = man;
		while (ss != NULL) {
			for (int i = 0; i < 4 * 6*10 * 6*10; i++) {
				visible_cells[i] |= ss->m_visible_cells[i];
			}
			ss = ss->next();
		}
		map->update_visible_cells(visible_cells);
		//VISIBILITY_CHANGED = 0;
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
		ss = ss->next();
		if (ss == NULL)
			break;
	}
	return ss;
}


Soldier *Platoon::next_not_moved_man(Soldier *sel_man)
{
	//sel_man->MOVED = 1; //in icon done
	Soldier *ss = sel_man->next();
	while (ss != NULL) {
		if (!ss->MOVED)
			return ss;
		ss = ss->next();
	}

	ss = man;
	while ((ss != NULL) && (ss != sel_man)) {
		if (!ss->MOVED)
			return ss;
		ss = ss->next();
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


int Platoon::freeNID()
{
	int i = 0;

	Soldier *ss = man;
	while (ss != NULL) {
		if (ss->NID == i) {
			i++;
			ss = man;
			continue;
		}
		ss = ss->next();
	}
	return i;
}


void Platoon::del(int SID)
{
	Soldier *ss = man;

	while (ss != NULL) {
		if (ss->NID == SID) {
			if (ss == man)
				man = ss->next();
			ss->unlink();
			delete(ss);
			return ;
		}
		ss = ss->next();
	}
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


int Platoon::check_for_hit(int z, int x, int y)
{
	Soldier *ss = man;

	int v = 0;
	while (ss != NULL) {
		v |= ss->check_for_hit(z, x, y);
		ss = ss->next();
	}
	return v;
}


void Platoon::apply_hit(int z, int x, int y, int type)
{
	Soldier *ss = man;

	while (ss != NULL) {
		ss->apply_hit(z, x, y, type);
		ss = ss->next();
	}
}


void Platoon::save_FULLDATA(char *fn)
{
	int fh = open(fn, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	assert(fh != -1);
	Soldier *ss = man;
	while (ss != NULL) {
		write(fh, &ss->md, sizeof(ss->md));
		ss->build_ITEMDATA();
		write(fh, &ss->id, sizeof(ss->id));
		ss = ss->next();
	}
	close(fh);
}

void Platoon::load_FULLDATA(char *fn)
{
	char *buf;
	unsigned int buf_size;

	int fh = open(fn, O_RDONLY | O_BINARY);
	assert(fh != -1);
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


void Platoon::save_MANDATA(char *fn)
{
	int fh = open(fn, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	assert(fh != -1);
	Soldier *ss = man;
	while (ss != NULL) {
		write(fh, &ss->md, sizeof(ss->md));
		ss = ss->next();
	}
	close(fh);
}


void Platoon::load_MANDATA(char *fn)
{
	char *buf;
	unsigned int buf_size;

	int fh = open(fn, O_RDONLY | O_BINARY);
	assert(fh != -1);
	buf_size = filelength(fh);
	buf = new char[buf_size];
	buf_size = read(fh, buf, buf_size);
	close(fh);

	Soldier *ss = man;
	unsigned int ofs = 0;
	while (ss != NULL) {
		if (ofs + sizeof(ss->md) > buf_size)
			break;
		memcpy(&ss->md, buf + ofs, sizeof(ss->md));
		ss->process_MANDATA();
		ofs += sizeof(ss->md);
		ss = ss->next();
	}
	delete []buf;
}


void Platoon::save_ITEMDATA(char *fn)
{
	int fh = open(fn, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	assert(fh != -1);
	Soldier *ss = man;
	while (ss != NULL) {
		ss->build_ITEMDATA();
		write(fh, &ss->id, sizeof(ss->id));
		ss = ss->next();
	}
	close(fh);
}


void Platoon::load_ITEMDATA(char *fn)
{
	char *buf;
	unsigned int buf_size;

	int fh = open(fn, O_RDONLY | O_BINARY);
	assert(fh != -1);
	buf_size = filelength(fh);
	buf = new char[buf_size];
	buf_size = read(fh, buf, buf_size);
	close(fh);

	Soldier *ss = man;
	unsigned int ofs = 0;
	while (ss != NULL) {
		if (ofs + sizeof(ss->id) > buf_size)
			break;
		memcpy(&ss->id, buf + ofs, sizeof(ss->id));
		ss->process_ITEMDATA();
		ofs += sizeof(ss->id);
		ss = ss->next();
	}
	delete []buf;
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


void Platoon::send_Units(Units &u)
{
	/*int num=0;
	for(int i=0; i<u.size; i++) {
		Soldier *ss = findman(u.name[i]);
		if (ss != NULL) {
			net->send_unit_data(u.size, num, u.lev[i], u.col[i], u.row[i],
									  &ss->ud, &ss->id);
			num++;
		}
	}*/
	net->send_unit_data_size(0);
	int num = 0;
	for (int i = 0; i < u.size; i++) {
		if (u.x[i] == 0)
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

	return true;
}

bool Platoon::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, man);

	return true;
}
