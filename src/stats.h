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

#ifndef STATS_H
#define STATS_H

#include "global.h"

/**
 * A class that holds all of the statistics for a single soldier
 */
class StatEntry: public persist::BaseObject
{ 
	DECLARE_PERSISTENCE(StatEntry);
private:
	int         SID;              //!< The soldier's ID
	char        name[26];         //!< The name of the soldier
	int         kills;            //!< The number of soldiers he's killed
	int         dead;             //!< Is he dead?
	int         damage_inflicted; //!< How much damage did he inflict?
	int         damage_taken;     //!< How much damage did he take?
	StatEntry  *next;             //!< The next statistics entry.
	StatEntry  *prev;             //!< The previous statistics entry.
public:
	StatEntry()
	{
		SID = 0;
		name[0] = '\0';
		kills = 0;
		dead = 0;
		damage_inflicted = 0;
		damage_taken = 0;
		next = NULL;
		prev = NULL;
	}
	virtual ~StatEntry() {}
	void        set_SID(int val) {SID = val;}
	void        set_name(const char *newname) {strcpy(name, newname);}
	void        inc_kills() {kills++;}
	void        set_dead(int val) {dead = val;}
	void        inc_damage_inflicted(int val) {damage_inflicted += val;}
	void        inc_damage_taken(int val) {damage_taken += val;}
	void        set_next(StatEntry *other) {next = other;}
	void        set_prev(StatEntry *other) {prev = other;}

	int         get_SID() {return SID;}
	const char *get_name() {return name;}
	int         get_kills() {return kills;}
	int         is_dead() {return dead;}
	int         get_inflicted() {return damage_inflicted;}
	int         get_taken() {return damage_taken;}

	StatEntry  *getnext() {return next;}
	StatEntry  *getprev() {return prev;}

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

/**
 * A class that holds all of the statistics for a platoon
 */
class Statistics: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Statistics);
private:
	int platoon_size;
	StatEntry *first;
	StatEntry *last;
public:
	Statistics(int size);
	virtual ~Statistics();

	StatEntry *getfirst() {return first;}
	StatEntry *getlast()  {return last;}

	StatEntry *get_stat_for_SID(int SID);
	StatEntry *get_most_kills();
	StatEntry *get_most_inflicted();
	StatEntry *get_least_inflicted();
	StatEntry *get_most_taken();

	int total_kills();
	int total_dead();
	int total_damage_inflicted();
	int total_damage_taken();

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif
