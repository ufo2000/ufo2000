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
#ifndef DIRTY_H
#define DIRTY_H

struct Rect
{
	int x, y;
	int w, h;
};

class DirtyList
{
	Rect *rect;
	int size;
	int count;

public:
	DirtyList(int sz);
	~DirtyList();

	void add(int x, int y, int w, int h);
	void sort();
	void clear();
	void apply(BITMAP *src, BITMAP *dst);
	void apply(BITMAP *src, BITMAP *dst, int src_x, int src_y);

	void copy(DirtyList *src);
	void add(DirtyList *src);
};


#endif
