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
#ifndef MAINMENU_H
#define MAINMENU_H

#ifdef DJGPP
#error DJGPP is now not supported
#endif

/**
 * Main menu items (shown in reverse order)
 */
enum MAINMENU_ITEMS
{
	MAINMENU_BACKGROUND = 0,
	MAINMENU_QUIT,
	MAINMENU_LOADGAME,
	MAINMENU_HOTSEAT,
    MAINMENU_INTERNET,
	MAINMENU_COUNT,
	// Currently disabled menu items come next
	MAINMENU_ABOUT, // has problems with color depth > 8
	MAINMENU_TCPIP, // has problems with color depth > 8, also uses old network code
	MAINMENU_EDITOR // does not support all terrains
};

void initmainmenu();
int do_mainmenu();

#endif
