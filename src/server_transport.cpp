/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

This file is partially based on clientserver example from HawkNL library
Copyright (C) 2000-2002  Phil Frisbie, Jr. (phil@hawksoft.com)

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

#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t + 999) / 1000)
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#include "server.h"
#include "server_protocol.h"
#include "server_config.h"

/**
 * $$$ Hack - to workaround HawkNL bug - sizeof(NLulong) == 8 on 64-bit systems
 * We use NLuint as 32-bit type here
 */
static NLuint nlSwapi(NLuint x)
{
    int tmp = 1;
    if (*(char *)&tmp == 1)
    {
        // swap is needed
        return (NLuint)(((((NLuint)x) & 0x000000ff) << 24) | ((((NLuint)x) & 0x0000ff00) << 8) | 
            ((((NLuint)x) & 0x00ff0000) >> 8) | ((((NLuint)x) & 0xff000000) >> 24));
    } else {
        // swap is not needed
        return x;
    }
}

#ifdef ENABLE_UFO2K_SERVER

ServerClient::ServerClient(ServerDispatch *server, NLsocket socket)
    :m_socket(socket), m_server(server)
{
    m_error = false;
    nlTime(&m_connection_time);
    m_max_ave_traffic = 0;
    m_http = false;
    m_server->m_clients_by_socket[m_socket] = this;
}

ServerClient::~ServerClient()
{
    if (m_http) {
        m_server->m_http_traffic_in  += nlGetSocketStat(m_socket, NL_BYTES_RECEIVED);
        m_server->m_http_traffic_out += nlGetSocketStat(m_socket, NL_BYTES_SENT);
    } else {
        m_server->m_traffic_in  += nlGetSocketStat(m_socket, NL_BYTES_RECEIVED);
        m_server->m_traffic_out += nlGetSocketStat(m_socket, NL_BYTES_SENT);
    }
    m_server->m_clients_by_socket.erase(m_socket);
    if (m_name != "") m_server->m_clients_by_name.erase(m_name);
    nlGroupDeleteSocket(m_server->m_group, m_socket);
    nlClose(m_socket);

    server_log("connection closed (name='%s', max_ave_traffic=%d, ip=%s)\n", 
        m_name.c_str(), m_max_ave_traffic, m_ip.c_str());
}

#endif

#define PACKET_HEADER_SIZE 8

static int stream_to_packet(std::string &stream, NLuint &id, std::string &packet)
{
    if (stream.size() < PACKET_HEADER_SIZE) return 0;

    const char *p = stream.data();

    NLuint size = nlSwapi(*(NLuint *)p);
    if (size > PACKET_SIZE_LIMIT) return -1;

//  if the packet is still not completely transmitted, exit from this cycle
    if (stream.size() < PACKET_HEADER_SIZE + size) return 0;

    id = nlSwapi(*(NLuint *)(p + 4));
    packet = std::string(p + PACKET_HEADER_SIZE, size);
    stream.erase(stream.begin(), stream.begin() + PACKET_HEADER_SIZE + size);
    return 1;
}

static bool packet_to_stream(std::string &stream, NLuint id, const std::string &packet)
{
    NLuint size = nlSwapi(packet.size());
    id = nlSwapi(id);
    
    stream += std::string((const char *)&size, sizeof(size));
    stream += std::string((const char *)&id, sizeof(id));
    stream += packet;

    return true;
}

static bool stream_to_socket(NLsocket socket, std::string &stream)
{
#ifdef HAVE_HAWKNL
    NLint size_written = nlWrite(socket, stream.data(), stream.size());

    if (size_written == NL_INVALID && nlGetError() != NL_CON_PENDING)
        return false;
    if (size_written != NL_INVALID)
        stream.erase(stream.begin(), stream.begin() + size_written);
#else
    ssize_t size_written = send(socket, stream.data(), stream.size(), 0);
    if (size_written == -1 && errno != EAGAIN && errno != EINTR)
        return false;
    if (size_written != -1)
        stream.erase(stream.begin(), stream.begin() + size_written);
#endif
    return true;    
}

#ifdef ENABLE_UFO2K_SERVER

void ServerDispatch::HandleSocket(NLsocket socket)
{
    ServerClient *client = m_clients_by_socket[socket];
    std::string &stream = client->m_stream;

    NLuint id;
    std::string packet;
    int err;

    if (client->m_name.empty() && stream.size() >= 3 && stream[0] == 'G' && 
        stream[1] == 'E' && stream[2] == 'T') {
    //  HTTP request        
        client->m_http = true;
        std::string http_reply;
        http_reply += "HTTP/1.1 200 OK\r\n";
        http_reply += "Content-Type: text/html;charset=utf-8\r\n";
        http_reply += "Connection: close\r\n\r\n";
        std::string html_body;
        MakeHtmlReport(html_body);
        http_reply += html_body;
        client->m_stream_out += http_reply;
        server_log("http request from %s\n", client->m_ip.c_str());
        return;
    }

    while ((err = stream_to_packet(stream, id, packet)) == 1)
        client->recv_packet(id, packet);

    if (err < 0 || client->m_error)
        delete client;
}

void ServerDispatch::HandleNewConnections()
{
//  Check timeouts for authentication
    NLtime now;
    nlTime(&now);

    std::map<NLsocket, ServerClient *>::iterator it = m_clients_by_socket.begin();
    while (it != m_clients_by_socket.end()) {
        ServerClient *client = it->second; it++;
        if (!client->m_name.empty()) continue;
        if (get_time_diff(client->m_connection_time, now) > (long)LOGIN_TIME_LIMIT) {
            server_log("connection timeout ip=%s\n", client->m_ip.c_str());
            delete client;
        }
    }

//  Check limit for the number of connections
    if (m_clients_by_socket.size() >= CONNECTIONS_COUNT_LIMIT) return;

//  check for a new client
    NLsocket newsock = nlAcceptConnection(m_socket);
    if (newsock == NL_INVALID) {
        if (nlGetError() != NL_NO_PENDING)
            server_log("Warning (nlGetError() != NL_NO_PENDING in ServerDispatch::HandleNewConnections)");
        return;
    }

    NLaddress addr;
    nlGetRemoteAddr(newsock, &addr);

    NLbyte string[NL_MAX_STRING_LENGTH];
    
    if (!validate_ip(nlAddrToString(&addr, string))) {
        server_log("rejected connection from %s, address found in blacklist\n",
            nlAddrToString(&addr, string));
        nlClose(newsock);
        return;
    } else {
        server_log("accepted connection from %s\n", nlAddrToString(&addr, string));
    }

    nlGroupAddSocket(m_group, newsock);
    ServerClient *client = CreateServerClient(newsock);
    if (client == NULL) {
        server_log("Error (client == NULL in ServerDispatch::HandleNewConnections)");
        exit(1);
    }
    client->m_ip = nlAddrToString(&addr, string);
}

void ServerDispatch::Run(NLsocket sock)
{
    static std::vector<NLsocket> s;

    nlTime(&m_connection_time);
    m_traffic_in       = 0;
    m_traffic_out      = 0;
    m_http_traffic_in  = 0;
    m_http_traffic_out = 0;
    m_socket           = sock;

    m_group = nlGroupCreate();

    while (1) {

        if (g_server_reload_config_flag) {
            load_config();
            g_server_reload_config_flag = 0;
        }

        HandleNewConnections();
        
    //  Check for incoming messages
        if (s.size() < CONNECTIONS_COUNT_LIMIT) s.resize(CONNECTIONS_COUNT_LIMIT);
        NLint count = nlPollGroup(m_group, NL_READ_STATUS, &s[0], CONNECTIONS_COUNT_LIMIT, 0);
        if (count == NL_INVALID) {
            server_log("Warning (count == NL_INVALID in ServerDispatch::Run)");
            continue;
        }

    //  Loop through the clients and read the packets
        for (NLint i = 0; i < count; i++) {
            ServerClient *client = m_clients_by_socket[s[i]];
            int readlen;
            NLbyte buffer[128];

            while ((readlen = nlRead(s[i], buffer, sizeof(buffer))) > 0) {
                client->m_stream.append(buffer, readlen);
            //  Check for traffic limit
                unsigned long ave_traffic = nlGetSocketStat(s[i], NL_AVE_BYTES_RECEIVED);
                if (ave_traffic > client->m_max_ave_traffic)
                    client->m_max_ave_traffic = ave_traffic;
            }

            if (readlen == NL_INVALID) {
                NLenum err = nlGetError();
                if (err == NL_SOCK_DISCONNECT || err == NL_MESSAGE_END) {
                    client->m_error = true;
                } else {
                    server_log("socket read error %d: user '%s' from %s\n",
                        err, client->m_name.c_str(), client->m_ip.c_str());
                }
            }

            HandleSocket(s[i]);
        }

    //  Loop through the clients and write the packets
        std::map<NLsocket, ServerClient *>::iterator it = m_clients_by_socket.begin();
        while (it != m_clients_by_socket.end()) {
            ServerClient *client = it->second; it++;
            if (client->m_stream_out.empty()) continue;

            NLint size_written = nlWrite(client->m_socket,
                client->m_stream_out.data(), client->m_stream_out.size());

            if (size_written != NL_INVALID) {
                client->m_stream_out.erase(client->m_stream_out.begin(), 
                    client->m_stream_out.begin() + size_written);
            } else {
                NLenum err = nlGetError();
                server_log("socket write error %d: user '%s' from %s\n",
                    err, client->m_name.c_str(), client->m_ip.c_str());
            }

            if (client->m_http && client->m_stream_out.empty())
                delete client;
        }

        usleep(50000);
    }
}

bool ServerClient::send_packet_back(NLuint id, const std::string &packet)
{
    packet_to_stream(m_stream_out, id, packet);
    return stream_to_socket(m_socket, m_stream_out);
}

/**
 * Sent packet to all other successfully logged in players
 */
bool ServerClient::send_packet_all(NLuint id, const std::string &packet)
{
    std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
    while (it != m_server->m_clients_by_name.end()) {
        if (it->first != "" && it->first != m_name)
            it->second->send_packet_back(id, packet);
        it++;
    }
    return true;
}

#endif

/****************************************************************************/

bool string_to_base64(const char *src, std::string &dst)
{
    static unsigned char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int a = 1, b = 1;

    while (true) {
        unsigned char buffer[3];
        unsigned char temp[4];

        int x;

        for (x = 0; x < 3 && *src != 0; x++)
            buffer[x] = (unsigned char)(*src++);

        if (x == 0) return true;

        if (x == 1) a=0;
        if (x == 2) b=0;
        temp[0] = alphabet[buffer[0] >> 2];
        temp[1] = alphabet[((buffer[0] & 3) << 4) | (buffer[1] >> 4) * a];
        temp[2] = x > 1 ? alphabet[((buffer[1] & 0xF) << 2) | (buffer[2] >> 6) * b] : '=';
        temp[3] = x > 2 ? alphabet[buffer[2] & 0x3F] : '=';

        dst.append(std::string((const char *)temp, 4));
    }
    return true;
}

bool ClientServer::connect(
    const std::string &ufo2000_server,
    const std::string &http_proxy,
    const std::string &http_proxy_login,
    std::string &error_message)
{
#ifdef HAVE_HAWKNL
    m_socket = NL_INVALID;
    NLaddress addr;

    std::string host = ufo2000_server;
    if (host.find(':') == std::string::npos) host.append(":2000");

    std::string proxy = http_proxy;
    if (proxy == "disabled") proxy = "";
    if (proxy == "auto") proxy = getenv("http_proxy") ? getenv("http_proxy") : "";

    if (!proxy.empty()) {
        error_message = "Failed to connect HTTP proxy";
        if (nlGetAddrFromName(proxy.c_str(), &addr) != NL_TRUE)
            return false;
    } else {
        error_message = "Failed to connect UFO2000 server";
        if (nlGetAddrFromName(host.c_str(), &addr) != NL_TRUE)
            return false;
    }

    m_socket = nlOpen(0, NL_RELIABLE);
    if (m_socket == NL_INVALID) return false;

    if (nlConnect(m_socket, &addr) == NL_FALSE) return false;

    if (!proxy.empty()) {
        int retry;
        char tmp[512];

        if (http_proxy_login.empty()) {
            sprintf(tmp, 
                "CONNECT %s HTTP/1.1\r\n\r\n", 
                host.c_str());
        } else {
            std::string login_base64;
            string_to_base64(http_proxy_login.c_str(), login_base64);
            sprintf(tmp, 
                "CONNECT %s HTTP/1.1\r\n"
                "Proxy-Authorization: Basic %s\r\n\r\n",
                host.c_str(),
                login_base64.c_str());
        }

        std::string request = tmp;
        std::string reply;

        bool fail_flag = true;
        for (retry = 0; retry < 100; retry++) {
            if (!stream_to_socket(m_socket, request)) return false;
            if (request.empty()) { fail_flag = false; break; }
            usleep(50000);
        }

        if (fail_flag) return false;

        fail_flag = true;
        for (retry = 0; retry < 100; retry++) {
            int readlen = nlRead(m_socket, tmp, sizeof(tmp));
            if (readlen > 0) {
                reply += std::string(tmp, tmp + readlen);
                std::string::size_type end_of_hdr = reply.find("\r\n\r\n");
                if (end_of_hdr != std::string::npos) {
                    std::string::size_type end_of_first_line = reply.find("\r\n");
                    reply.resize(end_of_first_line);
                    fail_flag = false;
                    break;
                }
            }
            usleep(50000);
        }

        if (fail_flag) return false;

        int http_code = 0;
        if (sscanf(reply.c_str(), "%s %d", tmp, &http_code) != 2) {
            error_message = "Invalid or no reply from HTTP proxy";
            return false;
        }

        if (http_code != 200) {
            sprintf(tmp, "HTTP proxy returned error %d", http_code);
            error_message = tmp;
            return false;
        }
    }
#else
    m_socket = -1;
    std::string proxy = http_proxy;
    if (proxy == "disabled") proxy = "";
    if (!proxy.empty()) {
        error_message = "Proxy is not supported for BSD sockets network code";
        return false;
    }
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    std::string host_str, port_str;

    if (!split_with_colon(ufo2000_server, host_str, port_str)) {
        host_str = ufo2000_server;
        port_str = "2000";
    }

    struct hostent *host = gethostbyname(host_str.c_str());
    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port_str.c_str()));

    while (::connect (m_socket, (struct sockaddr *)&sin, sizeof(sin)) == -1 && errno != EISCONN) {
        if (errno != EINTR)  {
            error_message = strerror(errno);
            close(m_socket);
            m_socket = -1;
            return false;
        }
    }

    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
#endif
    return true;
}

ClientServer::~ClientServer()
{
#ifdef HAVE_HAWKNL
    if (m_socket != NL_INVALID)
        nlClose(m_socket);
#else
    if (m_socket != -1)
        close(m_socket);
#endif
}

bool ClientServer::send_packet(NLuint id, const std::string &packet)
{
    packet_to_stream(m_stream_out, id, packet);
    return stream_to_socket(m_socket, m_stream_out);
}

bool ClientServer::send_packet(NLuint id, const std::map<std::string, std::string> &packet)
{
    std::string buffer;
    encode_stringmap(packet, buffer);
    packet_to_stream(m_stream_out, id | SRV_ADVANCED_PACKET_MASK, buffer);
    return stream_to_socket(m_socket, m_stream_out);
}

bool ClientServer::send_delayed_packet()
{
    return stream_to_socket(m_socket, m_stream_out);
}

bool ClientServer::flush_sent_packets()
{
    while (!m_stream_out.empty()) {
        if (!stream_to_socket(m_socket, m_stream_out))
            return false;
    }
    return true;
}

int ClientServer::recv_packet(NLuint &id, std::string &packet)
{
#ifdef HAVE_HAWKNL
    int readlen;
    NLbyte buffer[128];

    unsigned long stream_size_before = m_stream.size();

    while ((readlen = nlRead(m_socket, buffer, sizeof(buffer))) > 0)
        m_stream.append(buffer, readlen);

    if (stream_size_before == m_stream.size() && readlen == NL_INVALID)
    {
        NLenum err = nlGetError();
        if (err == NL_SOCK_DISCONNECT || err == NL_MESSAGE_END)
            return -1;
    }
#else
    int readlen;
    char buffer[128];

    unsigned long stream_size_before = m_stream.size();

    while ((readlen = recv(m_socket, buffer, sizeof(buffer), 0)) > 0)
        m_stream.append(buffer, readlen);

    if (stream_size_before == m_stream.size() && readlen == -1)
    {
        if (errno != EAGAIN && errno != EINTR)
            return -1;
    }
#endif

    int result = stream_to_packet(m_stream, id, packet);
    if (result < 0) return result;

    if (result && (id == SRV_KEEP_ALIVE)) {
        // send the same packet back to the server to confirm that 
        // we are still online, the rest of client code does not 
        // even need to know that we have received this packet
        send_packet(id, packet);
        return 0;
    }

    return result;
}

int ClientServer::wait_packet(NLuint &id, std::string &buffer)
{
    for (int retry = 0; retry < 100; retry++) {
        if (!flush_sent_packets()) return -1;
        int res = recv_packet(id, buffer);
        if (res != 0) return res;
        usleep(50000);
    }
    return -1;
}

inline static unsigned char decode_unsigned_char(const unsigned char *& p, const unsigned char *limit)
{
    if (p < limit)
        return *p++;
    else
        return 0;
}

inline static void encode_unsigned_char(unsigned char value, std::string &buffer)
{
    buffer.push_back(value);
}

static unsigned int decode_unsigned_int(const unsigned char *& p, const unsigned char *limit)
{
    int  bitcount;
    unsigned int Value;
    unsigned char b;

    for (Value = 0, bitcount = 0; bitcount <= 32 - 7; bitcount += 7) {
        b = decode_unsigned_char(p, limit);
        if (b & 0x80) { Value |= (b & 0x7F) << bitcount; break; }
        Value |= b << bitcount;
    }

    return Value;
}

static void encode_unsigned_int(unsigned int nValue, std::string &buffer)
{
    unsigned char b;

    while (true) {
        b = (unsigned char)(nValue & 0x7F);
        nValue >>= 7;
        if (nValue == 0) { encode_unsigned_char((unsigned char)(b | 0x80), buffer); break; }
        encode_unsigned_char(b, buffer);
    }
}

int encode_stringmap(const std::map<std::string, std::string> &info, std::string &buffer)
{
    buffer = "";
    encode_unsigned_int(info.size(), buffer);

    std::map<std::string, std::string>::const_iterator it = info.begin();
    while (it != info.end()) {
        encode_unsigned_int(it->first.size(), buffer);
        encode_unsigned_int(it->second.size(), buffer);
        for (int i = 0; i < (int)it->first.size(); i++)
            encode_unsigned_char(it->first[i], buffer);
        for (int i = 0; i < (int)it->second.size(); i++)
            encode_unsigned_char(it->second[i], buffer);
        it++;
    }

    return buffer.size();
}

bool decode_stringmap(std::map<std::string, std::string> &info, const std::string &buffer)
{
    const unsigned char *p = (const unsigned char *)buffer.data();
    const unsigned char *limit = (const unsigned char *)buffer.data() + buffer.size();
    unsigned int num = decode_unsigned_int(p, limit);

    while (num-- && p < limit) {
        unsigned int keysize = decode_unsigned_int(p, limit);
        unsigned int valsize = decode_unsigned_int(p, limit);
        std::string key;
        std::string val;
        for (unsigned int i = 0; i < keysize && p < limit; i++)
            key.push_back(decode_unsigned_char(p, limit));
        for (unsigned int i = 0; i < valsize && p < limit; i++)
            val.push_back(decode_unsigned_char(p, limit));
        info[key] = val;
    }
    return true;
}
