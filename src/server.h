#ifndef __SERVER_H__
#define __SERVER_H__

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>
#include <set>
#include <nl.h>

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
#ifdef ENABLE_UFO2K_SERVER
#define PACKET_SIZE_LIMIT g_srv_packet_size_limit
#else
#define PACKET_SIZE_LIMIT 65536
#endif

//! Encode std::map<std::string, std::string> into a single string
int encode_stringmap(const std::map<std::string, std::string> &info, std::string &buffer);
//! Decode std::map<std::string, std::string> from a string
bool decode_stringmap(std::map<std::string, std::string> &info, const std::string &buffer);

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
    bool connect(
        const std::string &host,
        const std::string &proxy,
        const std::string &proxy_login,
        std::string &error_message);
    bool send_packet(NLuint id, const std::string &packet);
    bool send_packet(NLuint id, const std::map<std::string, std::string> &packet);
    bool send_delayed_packet();
    bool flush_sent_packets();
    int recv_packet(NLuint &id, std::string &packet);
    int wait_packet(NLuint &id, std::string &buffer);
};

#ifdef ENABLE_UFO2K_SERVER

inline double get_time_diff(const NLtime &x, const NLtime &y)
{
    return (double)(y.seconds - x.seconds) * 1000.0 + (double)(y.mseconds - x.mseconds);
}

class ServerDispatch;

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
    bool send_packet_back(NLuint id, const std::string &packet);
//  This function sends a packet to all users
    bool send_packet_all(NLuint id, const std::string &packet);
//  This function sends a packet to the opponent of this user
    bool send_packet_opponent(NLuint id, const std::string &packet);

//  Function that handles packets received from this client 
    virtual bool recv_packet(NLuint id, const std::string &packet) = 0;
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

#endif
