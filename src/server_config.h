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

#include "sqlite/sqlite3_plus.h"

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#define DB_FILENAME "ufo2000.db"

extern int g_server_reload_config_flag;

extern unsigned long g_srv_tcp_port;
extern unsigned long g_srv_ave_traffic_limit;
extern unsigned long g_srv_players_count_limit;
extern unsigned long g_srv_connections_count_limit;
extern unsigned long g_srv_login_time_limit;
extern unsigned long g_srv_username_size_limit;
extern unsigned long g_srv_packet_size_limit;
extern unsigned long g_srv_keep_log_time;
extern unsigned long g_srv_daemonize;
extern sqlite3::connection db_conn;

void load_config();
void load_config(const std::string &pathname);
bool split_with_colon(const std::string &str, std::string &login, std::string &pass);
bool validate_ip(const std::string &ip);
void server_log(const char *fmt, ...);
void strip_server_log(double delta_time);

#endif
