/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

This file is partially based on clientserver example from HawkNL library
Copyright (C) 2000-2002  Phil Frisbie, Jr. (phil@hawksoft.com)

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

#include <stdio.h>
#include <nl.h>
#include <signal.h>
#include "server.h"
#include "server_config.h"

void printErrorExit(void)
{
	NLenum err = nlGetError();
    
	if (err == NL_SYSTEM_ERROR)
		server_log("System error: %s\n", nlGetSystemErrorStr(nlGetSystemError()));
	else
		server_log("HawkNL error: %s\n", nlGetErrorStr(err));

	nlShutdown();
	exit(1);
}

void reload_config(int s)
{
	g_server_reload_config_flag = 1;
}

int main()
{
	NLsocket serversock;
	NLenum   type = NL_IP;/* default network type */

    server_log("server started\n");
	load_config();

	if (!nlInit()) printErrorExit();

	printf("nlGetString(NL_VERSION) = %s\n", nlGetString(NL_VERSION));

    if (!nlSelectNetwork(type)) printErrorExit();

    nlEnable(NL_SOCKET_STATS);
    serversock = nlOpen(g_srv_tcp_port, NL_RELIABLE);
    
    if (serversock == NL_INVALID) printErrorExit();
    
    if (!nlListen(serversock))       /* let's listen on this socket */
    {
        nlClose(serversock);
        printErrorExit();
    }

#ifndef WIN32
	signal(SIGHUP, reload_config);
#endif

	ServerDispatch *server = new ServerDispatch();
	server->Run(serversock);
	delete server;

    nlShutdown();
    return 0;
}
