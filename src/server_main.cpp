/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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
#include <stdlib.h>
#include <nl.h>
#include <signal.h>
#include "server.h"
#include "server_config.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME "ufo2000-srv.conf"
#endif

#ifndef CONFIG_FILE_LOCATION
#define CONFIG_FILE_LOCATION "/etc/"
#endif

#ifndef PID_FILE_PATHNAME
#define PID_FILE_PATHNAME "./ufo2000-srv.pid"
#endif

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

/** fork, exit in parent, return in child.
 * Must be called before doing anything else,
 * because it closes file descriptors left and right.
 */
static void daemonize()
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
        server_log("Parent: forked child pid %d, exiting.\n", pid);
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

static void write_pid_file()
{
    FILE *fp = fopen(PID_FILE_PATHNAME, "w");
    if (fp)
    {
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }
}

static void remove_pid_file()
{
    unlink(PID_FILE_PATHNAME);
}

/** signal handler for SIGHUP */
static void reload_config(int s)
{
	g_server_reload_config_flag = 1;
    server_log("Reloading configuration.\n");
}

/** signal handler for unexpected signals, like SIGPIPE */
static void unexpected_signal(int s)
{
    server_log("Received unexpected signal %d, aborting.\n", s);
    remove_pid_file();
    exit(1);
}

/** signal handler for SIGTERM and SIGINT : attempt to exit cleanly.
We probably should try to send abort packets to current clients.
At the very least we remove the pid file to signal that we
exited cleanly  (not crashed, that is)
*/
static void clean_shutdown(int s)
{
    server_log("Got signal %d, shutting down.\n", s);
	remove_pid_file();
    exit(0);
}

static void set_signal_handlers()
{
    signal(SIGHUP, reload_config);
    signal(SIGTERM, clean_shutdown);
    signal(SIGINT, clean_shutdown);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, unexpected_signal);
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

/** test if we can read and write a file with stdio functions */
static bool test_file_rw(const std::string *pn, std::string *err)
{
    FILE *fp;
    int the_errno;

    /* 0. Test if file exists by trying to open for reading. */
    fp = fopen(pn->c_str(), "r");
    if (fp == NULL)
    {
        the_errno = errno;
        err->assign(strerror(the_errno));
        return false;
    }
    fclose(fp);

    /* 1. Test if file is writable by trying to open for appending. */
    fp = fopen(pn->c_str(), "a");
    if (fp == NULL)
    {
        the_errno = errno;
        err->assign(strerror(the_errno));
        return false;
    }
    fclose(fp);

    return true;
}

static std::string *find_config_file(int argc, char *argv[])
{
    std::string *pn = new std::string;
    std::string *pn_e = new std::string;
    /* 0. Check parameters */

    if (argc == 2)
    {
        pn->assign(argv[1]);
        if (test_file_rw(pn, pn_e))
        {  /* FIXME: we don't really need write access here */
            pn->assign(argv[1]);
            return pn;
        }
        else
        {
            fprintf(stderr, "No access to '%s': %s.\n",
                    pn->c_str(), pn_e->c_str());
            exit(1);
        }
    }

    /* 1. look in hardcoded location */
    std::string *hpn = new std::string(CONFIG_FILE_LOCATION);
    std::string *hpn_e = new std::string;
    hpn->append(CONFIG_FILE_NAME);
    if (test_file_rw(hpn, hpn_e))
        return hpn;

    /* 2. look in current directory */
    pn->assign("./");
    pn->append(CONFIG_FILE_NAME);
    if (test_file_rw(pn, pn_e))
        return pn;

    fprintf(stderr, "Can not access neither\n'%s': %s\n"
            " nor\n'%s': %s\n, aborting.\n",
            hpn->c_str(), hpn_e->c_str(), pn->c_str(), pn_e->c_str() );

    exit(1);
}

// Create or upgrade the database, if it's not existed or outdated.
void prepare_db()
{
    static const char *update_db[] = {
        "CREATE TABLE ufo2000_users (name text primary key, password text, last_login date, victories integer default 0, draws integer default 0, defeats integer default 0, elo_score double default 1500);",

        "CREATE TABLE ufo2000_sequences (name text primary key,seq_val integer);",

        "CREATE TABLE ufo2000_games (id integer primary key,  last_received_packed integer, is_finished text, errors text, result integer);",

        "CREATE TABLE ufo2000_game_players(game integer,  player text, last_sended_packet integer, position integer, primary key(game, player));",

        "insert or ignore into ufo2000_sequences values ('ufo2000_games',0);",

        "CREATE TABLE ufo2000_game_packets(game integer, id integer, sender integer, command text, packet_type integer, primary key(game,id));",

        "CREATE TRIGGER ufo2000_game_insert insert ON ufo2000_games BEGIN "
        "   delete from ufo2000_game_packets where game<new.id-100; "
        "   delete from ufo2000_debug_log where game<new.id-100; "
        "END;",

        "alter table ufo2000_games add column client_version text;",

        "alter table ufo2000_game_packets add column time real;",

        "create table ufo2000_user_sessions (id integer primary key, user text, realm text, system text, version text, begin real, end real, unique (user, begin));",

        "alter table ufo2000_user_sessions add column realm text;",

        "alter table ufo2000_user_sessions add column system text;",

        "alter table ufo2000_user_sessions add column version text;",

        "alter table ufo2000_game_packets add column session integer;",

        "insert or ignore into ufo2000_sequences values ('ufo2000_user_sessions', 0);",

        "create table ufo2000_debug_packets (game integer, id integer, sender integer, time real, param text, value text, session integer, primary key (game, id));",

        "create table ufo2000_debug_log (game integer, session integer, sender integer, id integer, time real, type integer, value text);",

        "create index ufo2000_debug_log_indx on ufo2000_debug_log (game, session, sender, id, type);",

        "create index ufo2000_debug_packets_indx on ufo2000_debug_packets (session, game, param);"
    };

    for (int i = 0; i < (int)(sizeof(update_db) / sizeof(update_db[0])); i++) {
        try {
            db_conn.executenonquery(update_db[i]);
        }
        catch(std::exception &ex) {
            server_log("Error executing '%s'\n", update_db[i]);
            LOG_EXCEPTION(ex.what());
        }
    }
    db_conn.executenonquery("begin transaction;");
}

int main(int argc, char *argv[])
{
#ifdef WIN32
	// Win32-version has all data files in a single directory where it was installed
	char ufo2000_dir[MAX_PATH];
	GetModuleFileName(NULL, ufo2000_dir, sizeof(ufo2000_dir));
	if (strrchr(ufo2000_dir, '\\')) *strrchr(ufo2000_dir, '\\') = '\0';
	SetCurrentDirectory(ufo2000_dir);
#endif

	NLsocket serversock;
	NLenum   type = NL_IP;/* default network type */

    std::string *cfg_pathname = find_config_file(argc, argv);

    load_config(*cfg_pathname);

#ifndef WIN32
    if (g_srv_daemonize)
        daemonize();
    write_pid_file();
#endif

    if (!nlInit()) printErrorExit();
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
    set_signal_handlers();
#endif
    prepare_db();

	ServerDispatch *server = new ServerDispatch();
    server_log("server started\n");
	server->Run(serversock);
	delete server;

    nlShutdown();
#ifndef WIN32
    remove_pid_file();
#endif
    return 0;
}
