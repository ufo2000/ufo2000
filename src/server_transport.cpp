/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
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

#include "server.h"
#include "server_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nl.h>

#include <string>
#include <vector>
#include <map>

#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t + 999) / 1000)
#else
#include <unistd.h>
#endif

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

#define PACKET_HEADER_SIZE 8

static int stream_to_packet(std::string &stream, NLulong &id, std::string &packet)
{
	if (stream.size() < PACKET_HEADER_SIZE) return 0;

	const char *p = stream.data();

	NLulong size = nlSwapl(*(NLulong *)p);
	if (size > PACKET_SIZE_LIMIT) return -1;

//	if the packet is still not completely transmitted, exit from this cycle
	if (stream.size() < PACKET_HEADER_SIZE + size) return 0;

	id = nlSwapl(*(NLulong *)(p + 4));
	packet = std::string(p + PACKET_HEADER_SIZE, size);
	stream.erase(stream.begin(), stream.begin() + PACKET_HEADER_SIZE + size);
	return 1;
}

static bool packet_to_stream(std::string &stream, NLulong id, const std::string &packet)
{
	NLulong size = nlSwapl(packet.size());
	id = nlSwapl(id);
	
	stream += std::string((const char *)&size, sizeof(size));
	stream += std::string((const char *)&id, sizeof(id));
	stream += packet;

	return true;
}

static bool stream_to_socket(NLsocket socket, std::string &stream)
{
	NLint size_written = nlWrite(socket, stream.data(), stream.size());

	if (size_written == NL_INVALID && nlGetError() != NL_CON_PENDING)
		return false;

	if (size_written != NL_INVALID)
		stream.erase(stream.begin(), stream.begin() + size_written);

	return true;	
}

void ServerDispatch::HandleSocket(NLsocket socket)
{
	ServerClient *client = m_clients_by_socket[socket];
	std::string &stream = client->m_stream;

    NLulong id;
    std::string packet;
    int err;

    if (client->m_name.empty() && stream.size() >= 3 && stream[0] == 'G' && 
    	stream[1] == 'E' && stream[2] == 'T') {
	//	HTTP request    	
		client->m_http = true;
		std::string http_reply;
		http_reply += "HTTP/1.0 200 OK\n";
		http_reply += "Content-Type: text/html;charset=utf-8\n\n";
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
//	Check timeouts for authentication
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

//	Check limit for the number of connections
	if (m_clients_by_socket.size() >= CONNECTIONS_COUNT_LIMIT) return;

//	check for a new client
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

    time_t last_log_strip_time = time(NULL);
	strip_server_log(g_srv_keep_log_time * 24 * 3600);

    while (1) {

    //	Strip server log every 8 hours
    	if (difftime(time(NULL), last_log_strip_time) > 8 * 3600) {
    		last_log_strip_time = time(NULL);
			strip_server_log(g_srv_keep_log_time * 24 * 3600);
    	}

    	if (g_server_reload_config_flag) {
    		load_config();
			g_server_reload_config_flag = 0;
    	}

    	HandleNewConnections();
        
    //	Check for incoming messages
        if (s.size() < CONNECTIONS_COUNT_LIMIT) s.resize(CONNECTIONS_COUNT_LIMIT);
        NLint count = nlPollGroup(m_group, NL_READ_STATUS, &s[0], CONNECTIONS_COUNT_LIMIT, 0);
        if (count == NL_INVALID) {
			server_log("Warning (count == NL_INVALID in ServerDispatch::Run)");
        	continue;
        }

	//	Loop through the clients and read the packets
        for (NLint i = 0; i < count; i++) {
			ServerClient *client = m_clients_by_socket[s[i]];
			int readlen;
			NLbyte buffer[128];

			while ((readlen = nlRead(s[i], buffer, sizeof(buffer))) > 0) {
				client->m_stream.append(buffer, readlen);
            //	Check for traffic limit
				unsigned long ave_traffic = nlGetSocketStat(s[i], NL_AVE_BYTES_RECEIVED);
				if (ave_traffic > client->m_max_ave_traffic)
					client->m_max_ave_traffic = ave_traffic;
				if (ave_traffic > AVE_TRAFFIC_LIMIT) {
					server_log("flooder detected, connection terminated: user '%s' from %s\n",
						client->m_name.c_str(), client->m_ip.c_str());
					client->m_error = true;
					break;
				}
			}

			if (readlen == NL_INVALID) {
				NLenum err = nlGetError();
				if (err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
					client->m_error = true;
            }

            HandleSocket(s[i]);
        }

    //	Loop through the clients and write the packets
    	std::map<NLsocket, ServerClient *>::iterator it = m_clients_by_socket.begin();
		while (it != m_clients_by_socket.end()) {
			ServerClient *client = it->second; it++;
			if (client->m_stream_out.empty()) continue;

			NLint size_written = nlWrite(client->m_socket,
				client->m_stream_out.data(), client->m_stream_out.size());

			if (size_written != NL_INVALID) {
				client->m_stream_out.erase(client->m_stream_out.begin(), 
					client->m_stream_out.begin() + size_written);
			}

			if (client->m_http && client->m_stream_out.empty())
				delete client;
		}

        usleep(50000);
    }
}

bool ServerClient::send_packet_back(NLulong id, const std::string &packet)
{
	packet_to_stream(m_stream_out, id, packet);
	return stream_to_socket(m_socket, m_stream_out);
}

/**
 * Sent packet to all other successfully logged in players
 */
bool ServerClient::send_packet_all(NLulong id, const std::string &packet)
{
	std::map<std::string, ServerClient *>::iterator it = m_server->m_clients_by_name.begin();
	while (it != m_server->m_clients_by_name.end()) {
		if (it->first != "" && it->first != m_name)
			it->second->send_packet_back(id, packet);
		it++;
	}
	return true;
}

/****************************************************************************/

bool ClientServer::connect(const std::string &host, int port)
{
	m_socket = NL_INVALID;

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
	if (m_socket != NL_INVALID)
		nlClose(m_socket);
}

bool ClientServer::send_packet(NLulong id, const std::string &packet)
{
	packet_to_stream(m_stream_out, id, packet);
	return stream_to_socket(m_socket, m_stream_out);
}

bool ClientServer::send_delayed_packet()
{
	return stream_to_socket(m_socket, m_stream_out);
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

	return stream_to_packet(m_stream, id, packet) == 1;
}

int ClientServer::wait_packet(NLulong &id, std::string &buffer)
{
    while (true) {
    	if (!send_delayed_packet()) return -1;
    	int res = recv_packet(id, buffer);
		if (res != 0) return res;
        usleep(1);
    }
}
