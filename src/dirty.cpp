/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

#include "stdafx.h"

#include "global.h"
#include "dirty.h"

static int rect_cmp(const void *p1, const void *p2)
{
	Rect *r1 = (Rect *)p1;
	Rect *r2 = (Rect *)p2;
	return (r1->y - r2->y);
}


DirtyList::DirtyList(int sz)
{
	size = sz;
	rect = new Rect[size];
	count = 0;
}

DirtyList::~DirtyList()
{
	delete []rect;
}

void DirtyList::add(int x, int y, int w, int h)
{
	rect[count].x = x;
	rect[count].y = y;
	rect[count].w = w;
	rect[count].h = h;
	count++;
	//if (count >= size) count = 0;
}

void DirtyList::sort()
{
	qsort(rect, count, sizeof(Rect), rect_cmp);
}

void DirtyList::clear()
{
	count = 0;
}

void DirtyList::apply(BITMAP *src, BITMAP *dst)
{
	for (int i = 0; i < count; i++) {
		blit(src, dst, rect[i].x, rect[i].y,
		     rect[i].x, rect[i].y,
		     rect[i].w, rect[i].h);
	}
}

void DirtyList::apply(BITMAP *src, BITMAP *dst, int src_x, int src_y)
{
	for (int i = 0; i < count; i++) {
		blit(src, dst, rect[i].x, rect[i].y,
		     rect[i].x + src_x, rect[i].y + src_y,
		     rect[i].w, rect[i].h);
	}
}

void DirtyList::copy(DirtyList *src)
{
	count = src->count;
	for (int i = 0; i < count; i++) {
		rect[i].x = src->rect[i].x;
		rect[i].y = src->rect[i].y;
		rect[i].w = src->rect[i].w;
		rect[i].h = src->rect[i].h;
	}
}

void DirtyList::add(DirtyList *src)
{
	for (int i = 0; i < src->count; i++) {
		rect[count + i].x = src->rect[i].x;
		rect[count + i].y = src->rect[i].y;
		rect[count + i].w = src->rect[i].w;
		rect[count + i].h = src->rect[i].h;
	}
	count += src->count;
}
