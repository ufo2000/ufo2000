#include "server_protocol.h"

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

			if (m_server->m_clients_by_name.find(login) != m_server->m_clients_by_name.end()) {
				send_packet_back(SRV_FAIL, "login failed (two users with the same login)");
				m_error = true;
				break;
			}

			m_name = login;
			send_packet_back(SRV_OK, "login ok");
	    // send user list to a newly created user
	    	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	    	while (it != m_server->m_clients_by_name.end()) {
				printf("send user online: %s\n", it->first.c_str());
				it->second->send_packet_back(SRV_USER_ONLINE, m_name);
				send_packet_back(SRV_USER_ONLINE, it->first);
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

        //	Try to find self in the opponent's challenge list
        	if (opponent->m_challenged_opponents.find(m_name) != opponent->m_challenged_opponents.end()) {
        		send_packet_all(SRV_USER_BUSY, m_name);
				opponent->send_packet_all(SRV_USER_BUSY, packet);
        		send_packet_back(SRV_GAME_START_JOIN, packet);
        		opponent->send_packet_back(SRV_GAME_START_HOST, m_name);
        		m_opponent = opponent;
        		opponent->m_opponent = this;
        		break;
        	}

        //	Insert the opponent into challenge list
			m_challenged_opponents.insert(packet);
			opponent->send_packet_back(SRV_USER_CHALLENGE_IN, m_name);
			send_packet_back(SRV_USER_CHALLENGE_OUT, packet);
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
