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

#include <stdio.h>
#include "server_protocol.h"

#define MAX_USERNAME_SIZE 16

static std::string tostring(int x)
{
	char buffer[32];
	sprintf(buffer, "%d", x);
	return buffer;
}

void ServerDispatch::MakeHtmlReport(std::string &html_body)
{
	html_body = "<html><head></head><body>";
	html_body += "<table border=1>";
	html_body += "<tr><td>user name<td>bytes received<td>bytes sent";

//	Report other players status
	std::map<std::string, ServerClient *>::iterator it = m_clients_by_name.begin();
	while (it != m_clients_by_name.end()) {
		ServerClientUfo *client = dynamic_cast<ServerClientUfo *>(it->second);

		html_body += "<tr><td>";
		html_body += client->m_name;
		html_body += "<td>";
		html_body += tostring(client->m_traffic_out);
		html_body += "<td>";
		html_body += tostring(client->m_traffic_in);

		it++;
	}

	html_body += "</table>";
	html_body += "<br>";
	html_body += "bytes received = number of bytes received by user from server<br>";
	html_body += "bytes received = number of bytes sent by user to server<br>";
	html_body += "</body><html>";
}

ServerClient *ServerDispatch::CreateServerClient(NLsocket socket)
{
	return new ServerClientUfo(this, socket);
}

ServerClientUfo::~ServerClientUfo()
{
	if (m_name != "") {
	//	send information that the user is offline to al other users
		send_packet_all(SRV_USER_OFFLINE, m_name);

	//	remove this user from challenge lists of other players
		std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
		while (it != m_server->m_clients_by_name.end()) {
			ServerClientUfo *opponent = dynamic_cast<ServerClientUfo *>(it->second);
			opponent->m_challenged_opponents.erase(m_name);
			it++;
		}
	}
//	if there is opponent playing with this user, remove pointer to
//	self from opponent's data
	if (m_opponent != NULL) {
		m_opponent->m_opponent = NULL;
	}
}

bool ServerClientUfo::recv_packet(NLulong id, const std::string &packet)
{
	printf("received packet from socket %d {id=%d, data=%s}\n", (int)m_socket, (int)id, packet.c_str());

//	only SRV_LOGIN packet is accepted from not authenticated users
	if (m_name == "" && id != SRV_LOGIN) {
		m_error = true;
		return false;
	}

//	process incoming packet
	switch (id) {
		case SRV_LOGIN: {
			std::string login, password;
			bool colon_found = false;
			for (unsigned int i = 0; i < packet.size(); i++) {
				if (!colon_found && packet[i] == ':') {
					colon_found = true;
				} else if (!colon_found) {
					login.append(packet.substr(i, 1));
				} else {
					password.append(packet.substr(i, 1));
				}
			}

			printf("user login. login = '%s', password = '%s'\n", login.c_str(), password.c_str());

			if (login.size() > MAX_USERNAME_SIZE || m_server->m_clients_by_name.find(login) != m_server->m_clients_by_name.end()) {
				send_packet_back(SRV_FAIL, "");
				m_error = true;
				break;
			}

			m_name = login;
			send_packet_back(SRV_OK, "login ok");
	    // send user list to a newly created user
	    	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	    	while (it != m_server->m_clients_by_name.end()) {
	    		ServerClientUfo *opponent = dynamic_cast<ServerClientUfo *>(it->second);
	    		
				printf("send user online: %s\n", opponent->m_name.c_str());
				opponent->send_packet_back(SRV_USER_ONLINE, m_name);
				if (opponent->m_busy)
					send_packet_back(SRV_USER_BUSY, opponent->m_name);
				else
					send_packet_back(SRV_USER_ONLINE, opponent->m_name);
				it++;
	    	}
			m_server->m_clients_by_name[m_name] = this;
			break;
		}
		case SRV_CHALLENGE: {
        //	Check that the opponent is currently online
			std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.find(packet);
			if (it == m_server->m_clients_by_name.end()) {
				printf("Warning: opponent '%s' is offline\n", packet.c_str());
				send_packet_back(SRV_USER_OFFLINE, packet);
				break;
			}

			ServerClientUfo *opponent = dynamic_cast<ServerClientUfo *>(it->second);

        //	Check that the opponent is not busy now
			if (opponent->m_busy) {
				printf("Warning: opponent '%s' is busy\n", packet.c_str());
				send_packet_back(SRV_USER_BUSY, packet);
				break;
			}

        //	Try to find self in the opponent's challenge list
        	if (opponent->m_challenged_opponents.find(m_name) != opponent->m_challenged_opponents.end()) {
            //	opponent found in the challenge list
        		send_packet_all(SRV_USER_BUSY, m_name);
				opponent->send_packet_all(SRV_USER_BUSY, packet);
        		send_packet_back(SRV_GAME_START_JOIN, packet);
        		opponent->send_packet_back(SRV_GAME_START_HOST, m_name);
        		m_opponent = opponent;
        		opponent->m_opponent = this;
        		m_busy = true;
        		opponent->m_busy = true;
        		m_challenged_opponents.clear();
        		opponent->m_challenged_opponents.clear();
        	} else {
	        //	insert the opponent into challenge list
				m_challenged_opponents.insert(packet);
				opponent->send_packet_back(SRV_USER_CHALLENGE_IN, m_name);
				send_packet_back(SRV_USER_CHALLENGE_OUT, packet);
			}
			break;
		}
		case SRV_MESSAGE: {
	    // send message to all other logged in users
			send_packet_all(SRV_MESSAGE, m_name + ": " + packet);
			break;
	    }
		case SRV_GAME_PACKET: {
		// send packet to the opponent	
			if (m_opponent != NULL) {
				m_opponent->send_packet_back(SRV_GAME_PACKET, packet);
			} else {
				printf("Warning: game packet from '%s', no opponent to forward\n", m_name.c_str());
			}
			break;
		}
		case SRV_ENDGAME: {
			if (m_opponent) m_opponent->m_opponent = NULL;
			m_opponent = NULL;
			m_busy = false;

		//	Report other players status
	    	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	    	while (it != m_server->m_clients_by_name.end()) {
	    		ServerClientUfo *opponent = dynamic_cast<ServerClientUfo *>(it->second);
	    		if (opponent->m_name != m_name) {
					opponent->send_packet_back(SRV_USER_ONLINE, m_name);
					if (opponent->m_busy)
						send_packet_back(SRV_USER_BUSY, opponent->m_name);
					else
						send_packet_back(SRV_USER_ONLINE, opponent->m_name);
				}
				it++;
	    	}

			break;
		}
	}

	return true;
}

bool ClientServerUfo::login(const std::string &name, const std::string &pass)
{
	if (!send_packet(SRV_LOGIN, name + ":" + pass)) {
		printf("login result: fail connect\n");
		return false;
	}

	NLulong id; std::string buffer;
	if (!wait_packet(id, buffer)) {
		printf("server closed connection\n");
		return false;
	}

	printf("login result: %s\n", buffer.c_str());
    return id == SRV_OK;
};

bool ClientServerUfo::message(const std::string &text)
{
	return send_packet(SRV_MESSAGE, text);
}

bool ClientServerUfo::challenge(const std::string &user)
{
	if (!send_packet(SRV_CHALLENGE, user)) return false;
	return true;
}
