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

#ifndef TEXT_H
#define TEXT_H

/**
 * Ufo2000: Text for messages and help-system
 */

//! Starting pages for the help-chapters
#define HELP_U2K_INDEX    0
 // "HELP_INDEX" already used in /MinGW/include/winuser.h
#define HELP_INTRO       10
#define HELP_BATTLESCAPE 20
#define HELP_MAPVIEW     40
#define HELP_MINDPROBE   50
#define HELP_SCANNER     60
#define HELP_MEDIKIT     70
#define HELP_PSI_AMP     80
#define HELP_BLASTER     90
#define HELP_INVENTORY  100
#define HELP_STATS      110
#define HELP_ENDGAME    130
#define HELP_NET        140

// First page of scenario-help follows directly
// after last page of help for mission-planner:
#define HELP_PLANNER    160
#define HELP_SCENARIO   HELP_PLANNER + 11

//! Write message into init-scripts.log
void lua_message( const std::string &str1 );

//! Interface to LUA-gettext()
const char *gettext(const char *str);

//! Translations for placenames
void Init_place_names();

//! Tooltips for the control-panel
const char *icontext( const int icon_nr );

//! Show tip-of-the-day
void showtip();

//! Simple help, using alert3().
void help( const int helppage );

#endif

