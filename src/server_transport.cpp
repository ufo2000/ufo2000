#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nl.h>

#include <string>
#include <vector>
#include <map>

#include "server.h"

#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t + 999) / 1000)
#else
#include <unistd.h>
#endif

ServerClient::ServerClient(ServerDispatch *server, NLsocket socket)
	:m_socket(socket), m_server(server)
{
	printf("%p\n", server);
	m_error = false;
	m_server->m_clients_by_socket[m_socket] = this;
}

ServerClient::~ServerClient()
{
	m_server->m_clients_by_socket.erase(m_socket);
	if (m_name != "") m_server->m_clients_by_name.erase(m_name);
	nlGroupDeleteSocket(m_server->m_group, m_socket);
	nlClose(m_socket);
}

#define PACKET_HEADER_SIZE 8
#define PACKET_SIZE_LIMIT  16384

static int decode_packet(std::string &stream, NLulong &id, std::string &packet)
{
	if (stream.size() < PACKET_HEADER_SIZE) return 0;

	const char *p = stream.data();

	NLulong size = nlSwapl(*(NLulong *)p);
	if (size > 16384) return -1;

//	if the packet is still not completely transmitted, exit from this cycle
	if (stream.size() < PACKET_HEADER_SIZE + size) return 0;

	id = nlSwapl(*(NLulong *)(p + 4));
	packet = std::string(p + PACKET_HEADER_SIZE, size);
	stream.erase(stream.begin(), stream.begin() + PACKET_HEADER_SIZE + size);
	return 1;
}

static bool send_to_socket(NLsocket socket, const char *buffer, NLint size)
{
	while (size > 0)
	{
		NLint size_written = nlWrite(socket, buffer, size);
		if (size_written == NL_INVALID)
		{
	        if (nlGetError() != NL_CON_PENDING) return false;
            usleep(1);
            continue;
		}
		buffer += size_written;
		size -= size_written;
	}
	return true;
}

static bool send_packet(NLsocket socket, NLulong id, const std::string &packet)
{
	struct packet_header { NLulong size, id; };
	packet_header header = { nlSwapl(packet.size()), nlSwapl(id) };
	if (!send_to_socket(socket, (const char *)&header, sizeof(header))) return false;
	if (!send_to_socket(socket, packet.data(), packet.size())) return false;
	return true;	
}

void ServerDispatch::HandleSocket(NLsocket socket)
{
	ServerClient *client = m_clients_by_socket[socket];
	std::string &stream = client->m_stream;

    NLulong id;
    std::string packet;
    int err;

	while ((err = decode_packet(stream, id, packet)) == 1)
		client->recv_packet(id, packet);

	if (err < 0 || client->m_error)
	{
	    printf("SERVER: socket %d closed\n", (int)socket);
        delete client;
	}
}

void ServerDispatch::HandleNewConnections()
{
/* check for a new client */
    NLsocket newsock = nlAcceptConnection(m_socket);
    if (newsock == NL_INVALID)
    {
    	assert(nlGetError() == NL_NO_PENDING);
    	return;
    }

    NLaddress addr;
    nlGetRemoteAddr(newsock, &addr);
    nlGroupAddSocket(m_group, newsock);

    NLbyte string[NL_MAX_STRING_LENGTH];
    
    printf("SERVER: Client %d connected from %s on socket %d\n", 
    	m_clients_by_socket.size(), nlAddrToString(&addr, string), (int)newsock);

    ServerClient *client = CreateServerClient(newsock);
    assert(client != NULL);
}

void ServerDispatch::Run(NLsocket sock)
{
	m_socket = sock;

    m_group = nlGroupCreate();

    while (1)
    {
    	HandleNewConnections();
        
    /* check for incoming messages */
        NLsocket s[MAX_CLIENTS];
        NLint count = nlPollGroup(m_group, NL_READ_STATUS, s, MAX_CLIENTS, 0);
        assert(count != NL_INVALID);

    /* loop through the clients and read the packets */
        for (NLint i = 0; i < count; i++)
        {
            int readlen;
	        NLbyte buffer[128];

            while ((readlen = nlRead(s[i], buffer, sizeof(buffer))) > 0)
            	m_clients_by_socket[s[i]]->m_stream.append(buffer, readlen);

            if (readlen == NL_INVALID)
            {
                NLenum err = nlGetError();
                if (err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
                	m_clients_by_socket[s[i]]->m_error = true;
            }

            HandleSocket(s[i]);
        }

        usleep(1);
    }
}

bool ServerClient::send_packet_back(NLulong id, const std::string &packet)
{
	return ::send_packet(m_socket, id, packet);
}

/**
 * Sent packet to all other successfully logged in players
 */
bool ServerClient::send_packet_all(NLulong id, const std::string &packet)
{
	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	while (it != m_server->m_clients_by_name.end()) {
		if (it->first != "" && it->first != m_name)
			if (!it->second->send_packet_back(id, packet)) 
				return false;
		it++;
	}
	return true;
}

/****************************************************************************/

bool ClientServer::connect(const std::string &host, int port)
{
    NLaddress addr;
    if (nlGetAddrFromName(host.c_str(), &addr) != NL_TRUE)
    	return false;
    if (nlSetAddrPort(&addr, port) != NL_TRUE)
    	return false;

    m_socket = nlOpen(0, NL_RELIABLE);
    if (m_socket == NL_INVALID) return false;

    if (nlConnect(m_socket, &addr) == NL_FALSE) return false;

    return true;
}

ClientServer::~ClientServer()
{
	nlClose(m_socket);
}

bool ClientServer::send_packet(NLulong id, const std::string &packet)
{
	return ::send_packet(m_socket, id, packet);
}

int ClientServer::recv_packet(NLulong &id, std::string &packet)
{
    int readlen;
    NLbyte buffer[128];

    unsigned long stream_size_before = m_stream.size();

    while ((readlen = nlRead(m_socket, buffer, sizeof(buffer))) > 0)
    	m_stream.append(buffer, readlen);

    if (stream_size_before == m_stream.size() && readlen == NL_INVALID)
    {
        NLenum err = nlGetError();
        if (err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
        	return -1;
    }

	return decode_packet(m_stream, id, packet) == 1;
}

int ClientServer::wait_packet(NLulong &id, std::string buffer)
{
    while (true) {
    	int res = recv_packet(id, buffer);
		if (res != 0) return res;
        usleep(1);
    }
}
