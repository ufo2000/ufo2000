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
#ifndef CONFIG_H
#define CONFIG_H

void loadini();
void saveini();

int setmodemplay();
int setnetplay();
int setsocketplay();
int sethotseatplay();
int setdplayplay();

void configure();
void aboutinfo();

int cfg_get_base_accuracy();
int cfg_get_screen_x_res();
int cfg_get_screen_y_res();
int cfg_get_min_color_depth();
const char *cfg_get_menu_music_file_name();
const char *cfg_get_setup_music_file_name();
const char *cfg_get_editor_music_file_name();
const char *cfg_get_combat_music_file_name();
const char *cfg_get_win_music_file_name();
const char *cfg_get_lose_music_file_name();
const char *cfg_get_net1_music_file_name();
const char *cfg_get_net2_music_file_name();
const char *cfg_get_loading_image_file_name();
const char *cfg_get_menu_image_file_name();
const char *cfg_get_endturn_image_file_name();
const char *cfg_get_win_image_file_name();
const char *cfg_get_lose_image_file_name();

FONT *cfg_get_console_font();
int cfg_get_music_volume();

const std::string &cfg_get_server_host();
const std::string &cfg_get_server_login();
const std::string &cfg_get_server_password();
const std::string &cfg_get_server_proxy();

#endif
