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

#ifndef PFXOPEN_H
#define PFXOPEN_H

#include <stdio.h>
#include <allegro.h>

extern const char *origfiles_prefix;
extern const char *ownfiles_prefix;
extern const char *gametemp_prefix;
extern const char *runtemp_prefix;

int open_prefixed(const char *pathname, int flags, const char *pfx);
FILE *fopen_prefixed(const char *pathname, const char *flags, const char *pfx);
DATAFILE *loaddata_prefixed(const char *pathname, const char *pfx);
BITMAP *loadbitmap_prefixed(const char *filename, RGB *pal, const char *pfx);

#define OPEN_ORIG(fname, flags) open_prefixed( (fname), (flags), origfiles_prefix) 
#define OPEN_OWN(fname, flags) open_prefixed( (fname), (flags), ownfiles_prefix) 
#define OPEN_GTEMP(fname, flags) open_prefixed( (fname), (flags), gametemp_prefix) 
#define OPEN_RTEMP(fname, flags) open_prefixed( (fname), (flags), runtemp_prefix) 

#define FOPEN_ORIG(fname, flags) fopen_prefixed( (fname), (flags), origfiles_prefix) 
#define FOPEN_OWN(fname, flags) fopen_prefixed( (fname), (flags), ownfiles_prefix) 
#define FOPEN_GTEMP(fname, flags) fopen_prefixed( (fname), (flags), gametemp_prefix) 
#define FOPEN_RTEMP(fname, flags) fopen_prefixed( (fname), (flags), runtemp_prefix) 

#define LOADDATA_OWN(fname) loaddata_prefixed( (fname), ownfiles_prefix) 
#define LOADDATA_GTEMP(fname) loaddata_prefixed( (fname), gametemp_prefix) 
#define LOADDATA_RTEMP(fname) loaddata_prefixed( (fname), runtemp_prefix) 

#define LOADBITMAP_OWN(fname,pal) loadbitmap_prefixed( (fname), (pal), ownfiles_prefix);
#define LOADBITMAP_ORIG(fname,pal) loadbitmap_prefixed( (fname), (pal), origfiles_prefix);

#endif 
