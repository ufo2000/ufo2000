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
#ifndef SOUND_H
#define SOUND_H

void initsound();
void closesound();
void play(int SNUM);
void play(int SNUM, int vol, int pan, int freq, int loop);
void setmodvol(int v);
int getmodvol();
void setdigvol(int v);
int getdigvol();
int modplay(char *modname);

#define S_SCREAM   0
#define S_EMPTY    1
#define S_FIRE     4      //from rifle
#define S_DIE      48     //soldier last scream
#define S_WOUND    7      //wounded scream
#define S_HIT      2      //bullet hit
#define S_EXPLODE  5      //explosive
#define S_MESSAGE  20     //come from remote
#define S_LASER    18     //fire from laser guns 
//#define S_THROW

#endif
