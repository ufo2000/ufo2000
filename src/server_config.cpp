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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <nl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include "server_config.h"

static std::map<std::string, unsigned long *> config_variables;

static unsigned long init_server_variable(const std::string name, unsigned long *val, unsigned long defval)
{
	config_variables[name] = val;
	return defval;
}

#define SERVER_CONFIG_VARIABLE(a, d) \
	unsigned long g_srv_##a = init_server_variable(#a, &g_srv_##a, d)

SERVER_CONFIG_VARIABLE(tcp_port,                2000);
// Limit for average incoming traffic (average is calculated by HawkNL 
// for for past 8 seconds)
SERVER_CONFIG_VARIABLE(ave_traffic_limit,       2000);
// Maximum number of authenticated users allowed on server
SERVER_CONFIG_VARIABLE(players_count_limit,     16);
// Maximum number of connections (including non authenticated and 
// http requests)
SERVER_CONFIG_VARIABLE(connections_count_limit, 32);
// Number of miliseconds for users to login (after this time the socket 
// will be closed)
SERVER_CONFIG_VARIABLE(login_time_limit,        10000);
// The maximum length of user name
SERVER_CONFIG_VARIABLE(username_size_limit,     16);
// Maximum size of data packet
SERVER_CONFIG_VARIABLE(packet_size_limit,       16384);

struct ip_info
{
	NLulong ip;
	NLulong mask;
	ip_info(): ip(0), mask(0xFFFFFFFF) { }
	ip_info(NLulong _ip, NLulong _mask): ip(_ip), mask(_mask) { }
};

static std::vector<ip_info>  accept_ip;
static std::vector<ip_info>  reject_ip;
static std::map<std::string, std::string> accept_user;

/**
 * Decode string into IP-address and mask
 */
static bool decode_ip(const char *buffer, NLulong &ip, NLulong &mask)
{
	unsigned int p1, p2, p3, p4, x;
	if (sscanf(buffer, "%u.%u.%u.%u /%u", &p1, &p2, &p3, &p4, &x) == 5)	{
		if (p1 > 255 || p2 > 255 || p3 > 255 || p4 > 255) return false;
		ip   = (p1 << 24) | (p2 << 16) | (p3 << 8) | p4;
		mask = 0;
		for (int i = 31; i >= 0 && x > 0; i--, x--) mask |= 1 << i;
		return true;
	} else if (sscanf(buffer, "%u.%u.%u.%u", &p1, &p2, &p3, &p4) == 4) {
		if (p1 > 255 || p2 > 255 || p3 > 255 || p4 > 255) return false;
		ip   = (p1 << 24) | (p2 << 16) | (p3 << 8) | p4;
		mask = 0xFFFFFFFF;
		return true;
	}
	return false;
}

static bool check_ip_match(NLulong ip, const ip_info &info)
{
	return (ip & info.mask) == (info.ip & info.mask);
}

/**
 * Parses text line and extracts variable name + value from it
 */
static bool get_variable(
	const std::string & str, 
	std::string       & var, 
	std::string       & val, 
	char                comment_char, 
	std::string       & comment_string)
{
    std::string::size_type comment_pos = str.find(comment_char);
    if (comment_pos == std::string::npos) {
    	comment_pos = str.size();
	    if (&comment_string != NULL) comment_string = "";
    } else {
	    if (&comment_string != NULL) {
	    	comment_string = str.substr(
	    		comment_pos, str.find_last_not_of(" \t\n\r") - comment_pos + 1);
	    }
	}

    if (comment_pos == 0) return false;

    std::string tmp = str.substr(0, str.find_last_not_of(" \t\n\r", comment_pos - 1) + 1);

    std::string::size_type eq = tmp.find('=');
    if (eq == std::string::npos) return false;
    std::string::size_type start = tmp.find_first_not_of(" \t\n\r");
    if (start == eq) return false;
    std::string::size_type end = tmp.find_last_not_of(" \t\n\r", eq - 1);

    var = tmp.substr(start, end - start + 1);

    std::string::size_type val_start = tmp.find_first_not_of(" \t\n\r", eq + 1);
    if (val_start == std::string::npos) val_start = eq + 1;

    val = tmp.substr(val_start);

    return true;
}

void load_config()
{
	char buffer[512];
	FILE *f = fopen("ufo2000-srv.conf", "rt");
	if (f == NULL) {
		printf("Error: can't open config file\n");
		return;
	}

	reject_ip.clear();
	accept_ip.clear();
	accept_user.clear();

	while (fgets(buffer, 511, f)) {
		std::string var, val, comment;
		if (!get_variable(buffer, var, val, '#', comment)) continue;

		if (var == "accept_user") {
			std::string login, password;
			if (!split_loginpass(val, login, password)) {
				server_log("invalid user login format in config: %s\n", val.c_str());
			} else {
				server_log("config accept_user = '%s'\n", val.c_str());
				accept_user.insert(std::pair<std::string, std::string>(login, password));
			}
		} else if (var == "reject_ip") {
			NLulong ip, mask;
			if (!decode_ip(val.c_str(), ip, mask)) {
				server_log("invalid ip address format in config: %s\n", val.c_str());
			} else {
				server_log("config reject_ip = '%s' (0x%08X 0x%08X)\n", val.c_str(), (int)ip, (int)mask);
				reject_ip.push_back(ip_info(ip, mask));
			}
		} else if (var == "accept_ip") {
			NLulong ip, mask;
			if (!decode_ip(val.c_str(), ip, mask)) {
				server_log("invalid ip address format in config: %s\n", val.c_str());
			} else {
				server_log("config accept_ip = '%s' (0x%08X 0x%08X)\n", val.c_str(), (int)ip, (int)mask);
				accept_ip.push_back(ip_info(ip, mask));
			}
		} else if (config_variables.find(var) != config_variables.end()) {
			unsigned long old = *config_variables[var];
			*config_variables[var] = atoi(val.c_str());
			if (old != *config_variables[var])
				server_log("config variable '%s' changed to %d\n", var.c_str(), *config_variables[var]);
		} else {
			server_log("unrecognized variable in config: %s\n", var.c_str());
		}
	}
	fclose(f);
}

/**
 * @return  0 - not registered\n
 *          1 - password valid\n
 *         -1 - password invalid
 */
int validate_user(const std::string &username, const std::string &password)
{
	if (accept_user.find(username) == accept_user.end()) return 0;
	return accept_user[username] == password ? 1 : -1;
}

bool validate_ip(const std::string &ip_string)
{
	NLulong ip = 0;
	unsigned int p1, p2, p3, p4;
	if (sscanf(ip_string.c_str(), "%u.%u.%u.%u", &p1, &p2, &p3, &p4) != 4)
		return false;
	ip = (p1 << 24) | (p2 << 16) | (p3 << 8) | p4;

	unsigned int i;
	for (i = 0; i < accept_ip.size(); i++)
		if (check_ip_match(ip, accept_ip[i])) return true;
	for (i = 0; i < reject_ip.size(); i++)
		if (check_ip_match(ip, reject_ip[i])) return false;
	return true;
}



/**
 * Add a message to log file
 */ 
void server_log(const char *fmt, ...)
{
	time_t now = time(NULL);
	struct tm * t = localtime(&now);
	char timebuf[1000];
	strftime(timebuf, 1000, "%d/%m/%Y %H:%M:%S", t);

	va_list arglist;
	va_start(arglist, fmt);

	FILE *flog = fopen("ufo2000-srv.log", "at");
	assert(flog != NULL);
	fprintf(flog, "%s ", timebuf);
	vfprintf(flog, fmt, arglist);
	fclose(flog);

	va_end(arglist);
}

bool split_loginpass(const std::string &str, std::string &login, std::string &password)
{
	bool colon_found = false;
	for (unsigned int i = 0; i < str.size(); i++) {
		if (!colon_found && str[i] == ':') {
			colon_found = true;
		} else if (!colon_found) {
			login.append(str.substr(i, 1));
		} else {
			password.append(str.substr(i, 1));
		}
	}
	return colon_found;
}

bool add_user(const std::string &login, const std::string &password)
{
	if (!accept_user.insert(std::pair<std::string, std::string>(login, password)).second)
		return false;

	FILE *f = fopen("ufo2000-srv.conf", "at");
	fprintf(f, "\naccept_user = %s:%s", login.c_str(), password.c_str());
	fclose(f);
	return true;
}

int g_server_reload_config_flag = 0;
