#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <map>
#include <nl.h>

// Limit for average incoming traffic (average is calculated by HawkNL 
// for for past 8 seconds)
#define AVE_TRAFFIC_LIMIT 500
// Maximum number of authenticated users allowed on server
#define PLAYERS_COUNT_LIMIT 16
// Maximum number of connections
#define CONNECTIONS_COUNT_LIMIT 32
// Number of miliseconds for users to login (after this time the socket 
// will be closed)
#define LOGIN_TIME_LIMIT 10000
// The maximum length of user name
#define USERNAME_SIZE_LIMIT 16
// Maximum size of data packet
#define PACKET_SIZE_LIMIT 16384

inline long get_time_diff(const NLtime &x, const NLtime &y)
{
	return (y.seconds - x.seconds) * 1000 + (y.mseconds - x.mseconds);
}

/**
 * Client side client-server connection
 */
class ClientServer
{
	std::string m_stream;
    NLsocket    m_socket;
public:
	virtual ~ClientServer();
	bool connect(const std::string &host, int port);
	bool send_packet(NLulong id, const std::string &packet);
	int recv_packet(NLulong &id, std::string &packet);
	int wait_packet(NLulong &id, std::string &buffer);
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

    long            m_max_ave_traffic;
    NLtime          m_connection_time;

	void Run();

public:
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

	NLtime                                m_connection_time;
	long                                  m_traffic_in;
	long                                  m_traffic_out;
	long                                  m_http_traffic_in;
	long                                  m_http_traffic_out;
	bool                                  m_http;

	void HandleNewConnections();
	void HandleSocket(NLsocket socket);
public:
	void Run(NLsocket sock);
	virtual ~ServerDispatch() {}
//	A function that is called when a new client wants to connect
	virtual ServerClient *CreateServerClient(NLsocket socket);
	virtual void MakeHtmlReport(std::string &html_body);
};

#endif
