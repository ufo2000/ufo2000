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
#include "server_config.h"

static std::string time_to_string(long t)
{
	char buffer[32];
	long hours   = t / (3600 * 1000); t -= hours * (3600 * 1000);
	long minutes = t / (60 * 1000); t -= minutes * (60 * 1000);
	long seconds = t / 1000;
	sprintf(buffer, "%02ld:%02ld:%02ld", hours, minutes, seconds);
	return buffer;
}

static std::string num_to_string(int x)
{
	char buffer[32];
	sprintf(buffer, "%d", x);
	return buffer;
}

void ServerDispatch::MakeHtmlReport(std::string &html_body)
{
	NLtime now;
	nlTime(&now);

	long traffic_in_cur = 0;
	long traffic_out_cur = 0;

	html_body = "<html><head></head><body>";
	html_body += "<table border=1>";
	html_body += "<tr><td>address<td>user name<td>bytes from<td>bytes to<td>max average traffic<td>time online";

//	Report other players status
	std::map<std::string, ServerClient *>::iterator it = m_clients_by_name.begin();
	while (it != m_clients_by_name.end()) {
		ServerClientUfo *client = dynamic_cast<ServerClientUfo *>(it->second);

		html_body += "<tr><td>";
		html_body += client->m_ip;
		html_body += "<td>";
		html_body += client->m_name;
		html_body += "<td>";
		html_body += num_to_string(nlGetSocketStat(client->m_socket, NL_BYTES_RECEIVED));
		traffic_in_cur += nlGetSocketStat(client->m_socket, NL_BYTES_RECEIVED);
		html_body += "<td>";
		html_body += num_to_string(nlGetSocketStat(client->m_socket, NL_BYTES_SENT));
		traffic_out_cur += nlGetSocketStat(client->m_socket, NL_BYTES_SENT);
		html_body += "<td>";
		html_body += num_to_string(client->m_max_ave_traffic);
		html_body += "<td>";
		html_body += time_to_string(get_time_diff(client->m_connection_time, now));

		it++;
	}

	html_body += "</table>";
	html_body += "<br>";
	html_body += "game server uptime = ";
	html_body += time_to_string(get_time_diff(m_connection_time, now)) + "<br>";
	html_body += "total incoming game traffic = ";
	html_body += num_to_string(m_traffic_in + traffic_in_cur) + "<br>";
	html_body += "total outcoming game traffic = ";
	html_body += num_to_string(m_traffic_out + traffic_out_cur) + "<br>";
	html_body += "total incoming HTTP traffic = ";
	html_body += num_to_string(m_http_traffic_in) + "<br>";
	html_body += "total outcoming HTTP traffic = ";
	html_body += num_to_string(m_http_traffic_out) + "<br>";
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
			split_loginpass(packet, login, password);

			server_log("user login (name = '%s', pwd = '%s' ip = %s)\n", 
				login.c_str(), password.c_str(), m_ip.c_str());

			if (login.size() > USERNAME_SIZE_LIMIT) {
		    	server_log("login failed: user name is too long\n");
				send_packet_back(SRV_FAIL, "User name is too long");
				m_error = true;
				break;
			}

			int validate_user_result = validate_user(login, password);
		    if (validate_user_result < 0) {
		    	server_log("login failed: invalid password\n");
				send_packet_back(SRV_FAIL, "Login failed (invalid password)");
				m_error = true;
				break;
			} else if (validate_user_result == 0) {
				if (password.size() >= 6 && add_user(login, password)) {
					server_log("successful registration: %s:%s\n", login.c_str(), password.c_str());
		    	} else {
					server_log("login failed: password is too short\n");
					send_packet_back(SRV_FAIL, "Registration failed (password is too short)");
					m_error = true;
					break;
				}
		    }

			if (m_server->m_clients_by_name.find(login) != m_server->m_clients_by_name.end()) {
		    	server_log("login failed: already online\n");
				send_packet_back(SRV_FAIL, "User with this name is already online");
				m_error = true;
				break;
			}

		//	check clients number limit
			if (m_server->m_clients_by_name.size() >= PLAYERS_COUNT_LIMIT) {
		    	server_log("login failed: too many players\n");
				send_packet_back(SRV_FAIL, "Too many players on server");
				m_error = true;
				break;
			}

	    	server_log("login ok\n");

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

            	server_log("game start: '%s' vs '%s'\n", m_name.c_str(), packet.c_str());

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

bool ClientServerUfo::login(const std::string &name, const std::string &pass,
	std::string &error_message)
{
	if (!send_packet(SRV_LOGIN, name + ":" + pass)) {
		error_message = "Failed to connect";
		return false;
	}

	NLulong id;
	if (!wait_packet(id, error_message)) {
		error_message = "Failed to connect";
		return false;
	}

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
