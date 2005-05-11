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

#ifndef __SERVER_PROTOCOL_H__
#define __SERVER_PROTOCOL_H__

#include "server.h"

// server replies
#define SRV_FAIL               0
#define SRV_OK                 1

// commands
#define SRV_LOGIN              2
#define SRV_MESSAGE            3
#define SRV_CHALLENGE          4
#define SRV_GAME_START_HOST    5
#define SRV_GAME_START_JOIN    6

// user status update notifications
#define SRV_USER_ONLINE        7
#define SRV_USER_OFFLINE       8
#define SRV_USER_CHALLENGE_IN  9
#define SRV_USER_CHALLENGE_OUT 10
#define SRV_USER_BUSY          11

#define SRV_GAME_PACKET        12
#define SRV_ENDGAME            13

#define SRV_DEBUG_MESSAGE      14

#define SRV_KEEP_ALIVE         15
#define SRV_DECLINE_CHALLENGE  16

#define SRV_GAME_CONTINUE_REQUEST  17
#define SRV_GAME_RECOVERY_START    18
#define SRV_GAME_REPLAY_REQUEST  19

class Server_Game_UFO;

class ServerClientUfo: public ServerClient
{
	std::set<std::string>  m_challenged_opponents;
	bool                   m_busy;
public:
	static int             m_games_started;
	static std::string     m_last_user_name;
	static NLtime          m_last_user_disconnect_time;
	
	ServerClientUfo(ServerDispatch *d, NLsocket s)
		: ServerClient(d, s), m_busy(false), game(NULL) { }
	virtual ~ServerClientUfo();
	bool recv_packet(NLulong id, const std::string &packet);

    // TODO - return right opponent.
	bool is_in_server_chat() { return !m_busy; }
	Server_Game_UFO* game;
	long int session_id;
	int position;

private:
    bool add_user(const std::string &username, const std::string &password);
    int validate_user(const std::string &username, const std::string &password);
};

class ClientServerUfo: public ClientServer
{
public:
	bool login(const std::string &name, const std::string &pass,
		std::string &error_message);
	bool message(const std::string &text);
	bool challenge(const std::string &user);
	bool decline_challenge(const std::string &user);
	//! Send request to server to resume unfinished game
	bool resume_game();
	bool resume_game_debug(std::string game_id);
};

#endif
