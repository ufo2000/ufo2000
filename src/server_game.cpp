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

#include "server_game.h"
#include "server_config.h"

int Server_Game_UFO::CreateGame(std::string playername1,std::string playername2)
{
    sqlite3::connection db_conn(DB_FILENAME);
    try {
        db_conn.executenonquery("begin transaction;");
        db_conn.executenonquery("update ufo2000_sequences set seq_val=seq_val+1 where name='ufo2000_games';");
        long int game_id = db_conn.executeint32("select seq_val from ufo2000_sequences where name='ufo2000_games';");
        db_conn.executenonquery("commit;");
        db_conn.executenonquery("\
            insert into ufo2000_games\
            (id, last_received_packed, is_finished, errors, result) \
            values \
            (%d, 0, 'N', NULL, 0);", game_id);
        db_conn.executenonquery("\
            insert into ufo2000_game_players\
            (game, player, last_sended_packet, position)\
            values\
            (%d, '%s', 0, 1);", game_id, playername1.c_str());
        db_conn.executenonquery("\
            insert into ufo2000_game_players\
            (game, player, last_sended_packet, position)\
            values\
            (%d, '%s', 0, 2);", game_id, playername2.c_str());
        return game_id;
    }
    catch(std::exception &ex) {
        server_log("Exception Occured: %s",ex.what());
        return 0;
    }
}


/*Server_Game_UFO* Server_Game_UFO::ActivatePlayer(int game_id,std::String playername,ServerClientUfo* player)
{
if (active_games.find(game_id) == active_games.end()) {
*active_games[game_id]=new Server_Game_UFO;
            unsigned long old = *config_variables[var];
else
new

}

PacketToServer
insert
PacketToClient
*/
