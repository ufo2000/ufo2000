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

#include <nl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include "server_config.h"

struct ip_info
{
	NLulong ip;
	NLulong mask;
	ip_info(): ip(0), mask(0xFFFFFFFF) { }
	ip_info(NLulong _ip, NLulong _mask): ip(_ip), mask(_mask) { }
};

static std::vector<ip_info>  accept_ip;
static std::vector<ip_info>  reject_ip;
static std::set<std::string> accept_user;

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
			server_log("config accept_user = '%s'\n", val.c_str());
			accept_user.insert(val);
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
		} else {
			server_log("unrecognized variable in config: %s\n", var.c_str());
		}
	}
	fclose(f);
}

bool validate_user(const std::string &username, const std::string &password)
{
	return accept_user.find(username + ":" + password) != accept_user.end();
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

int g_server_reload_config_flag = 0;
