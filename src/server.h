#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <map>
#include <nl.h>

/**
 * Client side client-server connection
 */
class ClientServer
{
	std::string m_stream;
    NLsocket    m_socket;
public:
	bool connect(const std::string &host, int port);
	bool send_packet(NLulong id, const std::string &packet);
	bool recv_packet(NLulong &id, std::string &packet);
};

/**
 * Server side client-server connection
 */
class ServerClient
{
protected:
	friend class    ServerDispatch;
	std::string     m_name;
	std::string     m_stream;
	NLsocket        m_socket;
	bool            m_error;
	ServerDispatch *m_server;
	NLsocket        m_socket_opponent;

	void Run();

public:
	ServerClient() { m_error = true; }
	ServerClient(ServerDispatch *server, NLsocket socket);
	virtual ~ServerClient();
//	This function sends a data packet to the specified recipient
	bool send_packet_back(NLulong id, const std::string &packet);
//	This function sends packet to all users
	bool send_packet_all(NLulong id, const std::string &packet);
//	This function sends a packet to the opponent of this user
	bool send_packet_opponent(NLulong id, const std::string &packet);

//	Function that handles received from this client packets
	virtual bool recv_packet(NLulong id, const std::string &packet) = 0;
};

#define MAX_CLIENTS 32
/**
 * Dispather of the packets
 */
class ServerDispatch
{
public:
	friend class ServerClient;
	std::map<NLsocket, ServerClient *>    m_clients_by_socket;
	std::map<std::string, ServerClient *> m_clients_by_name;
    NLint                                 m_group;
    NLsocket                              m_socket;
	void HandleNewConnections();
	void HandleSocket(NLsocket socket);
public:
	void Run(NLsocket sock);
	virtual ~ServerDispatch() {}
//	A function that is called when a new client wants to connect
	virtual ServerClient *CreateServerClient(NLsocket socket);
};

#endif
