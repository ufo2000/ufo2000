#include "server_protocol.h"

ServerClient *ServerDispatch::CreateServerClient(NLsocket socket)
{
	return new ServerClientUfo(this, socket);
}

ServerClientUfo::~ServerClientUfo()
{
	if (m_name != "") {
		send_packet_all(SRV_USER_OFFLINE, m_name);
	}
}

bool ServerClientUfo::recv_packet(NLulong id, const std::string &packet)
{
	printf("received packet from socket %d {id=%d, data=%s}\n", (int)m_socket, (int)id, packet.c_str());

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
				send_packet_back(SRV_FAIL, "login fail (two users with the same login)");
				m_error = true;
				break;
			}

			m_name = login;
			send_packet_back(SRV_OK, "login ok");
			send_packet_all(SRV_USER_ONLINE, m_name);
	    // send user list to a newly created user
	    	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	    	while (it != m_server->m_clients_by_name.end()) {
				printf("send user online: %s\n", it->first.c_str());
				send_packet_back(SRV_USER_ONLINE, it->first);
				it++;
	    	}
			m_server->m_clients_by_name[m_name] = this;
			break;
		}
		case SRV_MESSAGE:
			send_packet_all(SRV_MESSAGE, m_name + ": " + packet);
			break;
	}

	return true;
}

bool ClientServerUfo::login(const std::string &name, const std::string &pass)
{
	NLulong id; std::string buffer;
	if (!send_packet(SRV_LOGIN, name + ":" + pass)) {
		printf("login result: fail connect\n");
		return false;
	}

    while (true) {
    	int res = recv_packet(id, buffer);
    	if (res < 0) {
			printf("server closed connection\n");
			return false;
    	}
    	if (res == 1) break;
        nlThreadYield();
    }
	printf("login result: %s\n", buffer.c_str());
    return id == SRV_OK;
};

bool ClientServerUfo::message(const std::string &text)
{
	return send_packet(SRV_MESSAGE, text);
}
