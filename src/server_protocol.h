#ifndef __SERVER_PROTOCOL_H__
#define __SERVER_PROTOCOL_H__

#include <set>
#include "server.h"

// server replies
#define SRV_FAIL               0
#define SRV_OK                 1

// commands
#define SRV_LOGIN              2
#define SRV_MESSAGE            3
#define SRV_CHALLENGE          4
#define SRV_GAME_START_HOST    5
#define SRV_GAME_START_JOIN    6

// user status update notifications
#define SRV_USER_ONLINE        7
#define SRV_USER_OFFLINE       8
#define SRV_USER_CHALLENGE_IN  9
#define SRV_USER_CHALLENGE_OUT 10
#define SRV_USER_BUSY          11

class ServerClientUfo: public ServerClient
{
	std::set<std::string> m_challenged_opponents;
public:
	ServerClientUfo(ServerDispatch *d, NLsocket s) : ServerClient(d, s) { }
	virtual ~ServerClientUfo();
	bool recv_packet(NLulong id, const std::string &packet);
};

class ClientServerUfo : public ClientServer
{
public:
	bool login(const std::string &name, const std::string &pass);
	bool message(const std::string &text);
	bool challenge(const std::string &user);
};

#endif
