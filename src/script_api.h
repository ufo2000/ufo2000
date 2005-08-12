/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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
#ifndef SCRIPT_API_H
#define SCRIPT_API_H

#include <vector>

/**
 * @file script_api.h
 * @brief Bindings for calling lua functions from C++ and vice versa
 *
 * This file contains all the "black magic" involved in using lua stack 
 * based api and makes other parts of the game simplier
 */

/**
 * Gets the name of the currently selected equipment set
 */
const char *get_current_equipment_name();

/**
 * Tries to set new equipment
 */
bool set_current_equipment_name(const char *equipment_name);

/**
 * Gets the list of allowed equipment sets, returns currently active set
 */
int query_equipment_sets(std::vector<std::string> &eqsets);

#endif
