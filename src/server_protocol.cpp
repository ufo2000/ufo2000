#include "server_protocol.h"

ServerClient *ServerDispatch::CreateServerClient(NLsocket socket)
{
	return new ServerClientUfo(this, socket);
}

bool ServerClientUfo::recv_packet(NLulong id, const std::string &packet)
{
	printf("received packet from socket %d {id=%d, data=%s}\n", m_socket, id, packet.c_str());

	switch (id) {
		case SRV_LOGIN: {
			m_name = packet;
			send_packet_back(SRV_OK, "login ok");
			send_packet_all(SRV_USER_ONLINE, m_name);
	    // send user list to a newly created user
	    	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	    	while (it != m_server->m_clients_by_name.end())
	    	{
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
	if (!send_packet(SRV_LOGIN, name + ":" + pass))
	{
		printf("login result: fail connect\n");
		return false;
	}
    while (!recv_packet(id, buffer))
        nlThreadYield();
	printf("login result: %s\n", buffer.c_str());
    return id == SRV_OK;
};

bool ClientServerUfo::message(const std::string &text)
{
	return send_packet(SRV_MESSAGE, text);
}
