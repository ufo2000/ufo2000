#ifndef __SERVER_PROTOCOL_H__
#define __SERVER_PROTOCOL_H__

#include "server.h"

#define SRV_FAIL         0
#define SRV_OK           1
#define SRV_LOGIN        2
#define SRV_MESSAGE      3
#define SRV_USER_ONLINE  4
#define SRV_USER_OFFLINE 5
#define SRV_USER_BUSY    6

class ServerClientUfo: public ServerClient
{
public:
	ServerClientUfo(ServerDispatch *d, NLsocket s) : ServerClient(d, s) { }
	bool recv_packet(NLulong id, const std::string &packet);
};

class ClientServerUfo : public ClientServer
{
public:
	bool login(const std::string &name, const std::string &pass);
	bool message(const std::string &text);
};

#endif
