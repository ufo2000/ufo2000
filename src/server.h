#ifndef __SERVER_H__
#define __SERVER_H__

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>
#include <set>
#include <nl.h>

// Limit for average incoming traffic (average is calculated by HawkNL 
// for the past 8 seconds)
#define AVE_TRAFFIC_LIMIT g_srv_ave_traffic_limit
// Maximum number of authenticated users allowed on server
#define PLAYERS_COUNT_LIMIT g_srv_players_count_limit
// Maximum number of connections
#define CONNECTIONS_COUNT_LIMIT g_srv_connections_count_limit
// Number of miliseconds for users to login (after this time the socket 
// will be closed)
#define LOGIN_TIME_LIMIT g_srv_login_time_limit
// The maximum length of user name
#define USERNAME_SIZE_LIMIT g_srv_username_size_limit
// Maximum size of data packet
#define PACKET_SIZE_LIMIT g_srv_packet_size_limit

inline long get_time_diff(const NLtime &x, const NLtime &y)
{
    return (y.seconds - x.seconds) * 1000 + (y.mseconds - x.mseconds);
}

/**
 * Client side client-server connection
 *
 * @ingroup network
 */
class ClientServer
{
    std::string m_stream;
    std::string m_stream_out;
    NLsocket    m_socket;
public:
    virtual ~ClientServer();
    bool ClientServer::connect(
        const std::string &host,
        const std::string &proxy,
        std::string &error_message);
    bool send_packet(NLulong id, const std::string &packet);
    bool send_delayed_packet();
    bool flush_sent_packets();
    int recv_packet(NLulong &id, std::string &packet);
    int wait_packet(NLulong &id, std::string &buffer);
};

/**
 * Server side client-server connection
 *
 * @ingroup network
 */
class ServerClient
{
protected:
    friend class    ServerDispatch;
    friend class    Server_Game_UFO;
    std::string     m_name;
    std::string     m_stream;
    std::string     m_stream_out;
    NLsocket        m_socket;
    bool            m_error;
    ServerDispatch *m_server;

    unsigned long   m_max_ave_traffic;
    NLtime          m_connection_time;

    std::string     m_ip;
    bool            m_http;

    void Run();

public:
    ServerClient(ServerDispatch *server, NLsocket socket);
    virtual ~ServerClient();
//  This function sends a data packet to the specified recipient
    bool send_packet_back(NLulong id, const std::string &packet);
//  This function sends a packet to all users
    bool send_packet_all(NLulong id, const std::string &packet);
//  This function sends a packet to the opponent of this user
    bool send_packet_opponent(NLulong id, const std::string &packet);

//  Function that handles packets received from this client 
    virtual bool recv_packet(NLulong id, const std::string &packet) = 0;
};

/**
 * Dispatcher of the packets
 *
 * @ingroup network
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
    unsigned long                         m_traffic_in;
    unsigned long                         m_traffic_out;
    unsigned long                         m_http_traffic_in;
    unsigned long                         m_http_traffic_out;

    void HandleNewConnections();
    void HandleSocket(NLsocket socket);
public:
    void Run(NLsocket sock);
    virtual ~ServerDispatch() {}
//  A function that is called when a new client wants to connect
    virtual ServerClient *CreateServerClient(NLsocket socket);
    virtual void MakeHtmlReport(std::string &html_body);
};

#endif
