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

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

/** fork, exit in parent, return in child.
 * Must be called before doing anything else,
 * because it closes file descriptors left and right.
 */
void daemonize()
{
    int ern;
    int pid = fork();
    /* fork() failed ? */
    if (pid < 0)
    {
        ern = errno;
        server_log("Can not fork: %s\n", strerror(ern));
        exit(1);
    }
    /* Exit in parent. */
    if (pid > 0)
    {
        server_log("Parent: forked child %d, exiting.\n", pid);
        exit(0);
    }
    /* Become process group leader (If someone pathologically waits on us,
       this will tell them to go away)
     */
    setsid();

    /* Paranoidally get rid of all potentially open file descriptors.
       This is to prevent file descriptor leakage from parent process,
       close any opened files, etc, etc. May not play well with buffered I/O
       like fstreams. Hovewer, you have been warned to not do anything before
       calling this function.
     */
    int FD_PARANOIA_LEVEL = getdtablesize();
    for (int d=0; d < FD_PARANOIA_LEVEL; d++)
        close(d);

    /* Open standard file descriptors just in case something would want
       to write there. Particularly handy when launching other processes. */

    int stdout_d, stdin_d, stderr_d;
    const int unix_stdin_d  = STDIN_FILENO;
    const int unix_stdout_d = STDOUT_FILENO;
    const int unix_stderr_d = STDERR_FILENO;

    stdin_d = open("/dev/null", O_RDONLY);
    if (stdin_d < 0)
    {
        ern = errno;
        server_log("Can not read /dev/null: %s\n", strerror(ern));
        exit(1);
    }
    /* Rename opened for reading /dev/null to stdin */
    if (unix_stdin_d != stdin_d)
    {
        int rv = dup2(stdin_d, STDIN_FILENO);
        if (rv < 0)
        {
            ern = errno;
            server_log("Can not dup2 stdin_d: %s (%d, %d)\n", strerror(ern), stdin_d, STDIN_FILENO);
            exit(1);
        }
        close(stdin_d);
    }

    stdout_d = open("/dev/null", O_APPEND);
    if (stdout_d < 0)
    {
        ern = errno;
        server_log("Can not append to /dev/null: %s\n", strerror(ern));
        exit(1);
    }
    /* Rename opened for appending /dev/null to stdout */
    if (unix_stdout_d != stdout_d)
    {
        int rv = dup2(stdout_d, unix_stdout_d);
        if (rv < 0)
        {
            ern = errno;
            server_log("Can not dup2 stdout_d: %s\n", strerror(ern));
            exit(1);
        }
        close(stdout_d);
    }

    stderr_d = open("/dev/null", O_APPEND);
    if (stderr_d < 0)
    {
        ern = errno;
        server_log("Can not append to /dev/null: %s\n", strerror(ern));
        exit(1);
    }
    /* Rename opened for appending /dev/null to stdout */
    if (unix_stderr_d != stderr_d)
    {
        int rv = dup2(stderr_d, unix_stderr_d);
        if (rv < 0)
        {
            ern = errno;
            server_log("Can not dup2 stderr_d: %s\n", strerror(ern));
            exit(1);
        }
        close(stderr_d);
    }
}

#endif


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
#ifndef WIN32
	daemonize();
#endif

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
