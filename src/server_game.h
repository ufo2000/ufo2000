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

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "server_protocol.h"

class Server_Game_UFO
{
public:
    //! Creating record about a new game between a chosen users.
    static long int CreateGame(std::string playername1,std::string playername2);
    //! User tries to play in his unfinished game. If game is active (the opponent
    //! is active), method returns pointer to existing object, otherwise
    //! method creates a new object.
    static void ActivatePlayer(int game_id,ServerClientUfo* player);
    //! The user is getting offline, if his opponent is offline, the game
    //! object can be deleted till a next user activation.
    static void DeactivatePlayer(ServerClientUfo* player);
    //! Packet from user
    void PacketToServer(ServerClientUfo* sender, int packet_type,const std::string &packet);
private:
    //! Pointers to clients, NULL if a client is not active
    ServerClientUfo *players[2];
    static std::map<long int, Server_Game_UFO *> active_games;
    int PacketToClient(ServerClientUfo* recipient, int packet_type, const std::string &packet);
    sqlite3::connection db_conn;
    long int game_id;
	Server_Game_UFO(long int _game_id) : db_conn(DB_FILENAME), game_id(_game_id) {
        players[0] = NULL;
        players[1] = NULL;
    }
};

#endif
