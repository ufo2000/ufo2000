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
#include "stats.h"

IMPLEMENT_PERSISTENCE(StatEntry, "StatEntry");

bool StatEntry::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, next);
	PersistWriteObject(archive, prev);

	return true;
}

bool StatEntry::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, next);
	PersistReadObject(archive, prev);

	return true;
}

IMPLEMENT_PERSISTENCE(Statistics, "Statistics");

// Create as many StatEntry as we need.
Statistics::Statistics(int size)
{
	platoon_size = size;
	StatEntry *newentry = new StatEntry();
	last = first = newentry;
	for (int i = 1; i < size; i++)
	{
		newentry = new StatEntry();
		last->set_next(newentry);
		newentry->set_prev(last);
		last = newentry;
	}
	first->set_prev(NULL);
	last->set_next(NULL);
}

// Remove all the StatEntry.
Statistics::~Statistics()
{
	StatEntry *temp;
	for (StatEntry *current = first; current;)
	{
		temp = current->getnext();
		delete current;
		current = temp;
	}
}

// Find the matching StatEntry.
StatEntry *Statistics::get_stat_for_SID(int SID)
{
	for (StatEntry *current = first; current != last->getnext(); current = current->getnext())
		if (current->get_SID() == SID)
			return current;

	return NULL;
}

// Find the StatEntry with the highest kill total.
StatEntry *Statistics::get_most_kills()
{
	int total = -999;
	StatEntry *best = NULL;
	for (StatEntry *current = first; current; current = current->getnext())
	{
		if (current->get_kills() > total)
		{
			total = current->get_kills();
			best = current;
		}
	}

	return best;
}

// Find the StatEntry with the highest damage dealt.
StatEntry *Statistics::get_most_inflicted()
{
	int total = -999;
	StatEntry *best = NULL;
	for (StatEntry *current = first; current; current = current->getnext())
	{
		if (current->get_inflicted() > total)
		{
			total = current->get_inflicted();
			best = current;
		}
	}

	return best;
}

// Find the StatEntry with the lowest damage dealt.
StatEntry *Statistics::get_least_inflicted()
{
	int total = 9999;
	StatEntry *best = NULL;
	for (StatEntry *current = first; current; current = current->getnext())
	{
		if (current->get_inflicted() < total)
		{
			total = current->get_inflicted();
			best = current;
		}
	}

	return best;
}

// Find the StatEntry with the highest damage taken.
StatEntry *Statistics::get_most_taken()
{
	int total = -999;
	StatEntry *best = NULL;
	for (StatEntry *current = first; current; current = current->getnext())
	{
		if (current->get_taken() > total)
		{
			total = current->get_taken();
			best = current;
		}
	}

	return best;
}

// Sums up the kill count of all StatEntry.
int Statistics::total_kills()
{
	int total = 0;
	for (StatEntry *current = first; current; current = current->getnext())
		total += current->get_kills();

	return total;
}

// Finds out how many casualties were taken.
int Statistics::total_dead()
{
	int total = 0;
	for (StatEntry *current = first; current; current = current->getnext())
		total += current->is_dead();

	return total;
}

// Sums up the total amount of damage inflicted.
int Statistics::total_damage_inflicted()
{
	int total = 0;
	for (StatEntry *current = first; current; current = current->getnext())
		total += current->get_inflicted();

	return total;
}

// Sums up the total amount of damage taken.
int Statistics::total_damage_taken()
{
	int total = 0;
	for (StatEntry *current = first; current; current = current->getnext())
		total += current->get_taken();

	return total;
}

bool Statistics::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, first);
	PersistWriteObject(archive, last);

	return true;
}

bool Statistics::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, first);
	PersistReadObject(archive, last);

	return true;
}