/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

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


/*#ifdef WIN32
char HOSTNAME[256] = "10.0.0.2";
char PORT[256] = "2000";
 
int initsocketgame() {return 0;}
void closesocketgame() {}
int packet_recv_socket(char *pkt) {return 0;}
void packet_send_socket(char *pkt) {}
void packet_send_socket(char *pkt, int size) {}
 
#else*/

#include "global.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#ifdef DJGPP
#include <lsck/lsck.h>
#endif
#include "wind.h"
#include "netsock.h"
#ifdef WIN32
#include <winsock2.h>
#define EWOULDBLOCK   WSAEWOULDBLOCK
#define SHUT_RDWR     SD_BOTH
#else
#include <unistd.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#endif

/*
extern Wind *info_win;
//extern int SEND;
extern int HOST;
*/

struct sockaddr_in in, peer_in;
char HOSTNAME[256] = "127.0.0.1";
//char HOSTNAME[256] = "10.0.0.2";
char PORT[256] = "2000";

#ifdef WIN32
static WSADATA wsaData;
static SOCKET sock, hostsock;
#else
static int sock, hostsock;
#endif

#ifndef HAVE_ARPA_INET_H
#define inet_ntoa _inet_ntoa
const char *_inet_ntoa(const struct in_addr in)
{
	static char tmpbuf[24];
	assert(sizeof(in_addr) == 4);
	char a[4];
	memcpy(a, &in, 4);
	sprintf(tmpbuf, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
	return tmpbuf;
}
#endif

int initsocketgame()
{
#ifdef WIN32
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) == SOCKET_ERROR) {
		info_win->printstr("can't start winsock\n");
		//WSACleanup();
		return 0;
	}
#endif
	if (HOST) {
		//printf ("Enter port number: ");
		//gets (PORT);
		memset (&in, 0, sizeof(struct sockaddr_in));
		hostsock = socket(AF_INET, SOCK_STREAM, 0);
		if (hostsock > 0) {
			info_win->printstr("socket created\n");
		} else {
			info_win->printstr("can't create socket\n");
			return 0;
		}

		int opt = 1;
		if (setsockopt(hostsock, SOL_SOCKET, SO_REUSEADDR, (char*) & opt, sizeof(opt))) {
			info_win->printstr("can't setsockopt SO_REUSEADDR\n");
			return 0;
		}

		in.sin_family = AF_INET;
		in.sin_addr.s_addr = INADDR_ANY;
		in.sin_port = htons (atoi(PORT));
		if (bind(hostsock, (struct sockaddr *) & in, sizeof(struct sockaddr_in)) == -1) {
			info_win->printstr("can't bind\n");
			return 0;
		}
		if (listen(hostsock, 2) == -1) {
			info_win->printstr("can't listen\n");
			return 0;
		}
#ifdef WIN32
		unsigned long arg = 1234;
		ioctlsocket(hostsock, FIONBIO, &arg);
#else
		fcntl(hostsock, F_SETFL, O_NONBLOCK);
#endif
		info_win->printstr("accept");

		int oldsec = 0;
		time_t ltime;
		struct tm *time_of_day;

#ifdef WIN32
		int len = sizeof(struct sockaddr_in);
#else
		size_t len = sizeof(struct sockaddr_in);
#endif
		while (1) {
			if (key[KEY_ESC]) {
				//readkey();
				info_win->printstr("\naccept canceled\n");
				return 0;
			}
			sock = accept(hostsock, (struct sockaddr *) & peer_in, &len);
			if (sock == -1) {
#ifdef WIN32
				if (WSAGetLastError() == EWOULDBLOCK) {
#else
				if ((errno == EWOULDBLOCK) || (errno == 0)) { //success == 0 !!!
#endif
					time(&ltime);
					time_of_day = localtime(&ltime);
					if (time_of_day->tm_sec != oldsec) {
						oldsec = time_of_day->tm_sec;
						info_win->printchr('.');
					}
				} else {
					int err = errno;
					char str[100];
					sprintf(str, "\naccept error %d %s\n", err, strerror(err));
					info_win->printstr(str);
					//info_win->printstr( strerror(errno) ); //verbose output of errno
					readkey();      //wait until key pressed
					info_win->printstr("\ncan't accept\n");
					return 0;
				}
			} else
				break;
		}
		char str[100];
		sprintf (str, "\nconnected %s\n", inet_ntoa(peer_in.sin_addr));
		info_win->printstr(str);

	}
	else {
		sock = socket(AF_INET, SOCK_STREAM, 0);

		if (sock > 0) {
			info_win->printstr("socket created\n");
		} else {
			info_win->printstr("can't create socket\n");
			return 0;
		}

		//printf ("Enter IP address: ");
		//gets (HOSTNAME);

		struct hostent *hent = gethostbyname(HOSTNAME);
		if (hent == NULL) {
			info_win->printstr("can't gethostbyname\n");
			return 0;
		}
		unsigned long int haddress = ((struct in_addr *) hent->h_addr)->s_addr;

		memset(&in, 0, sizeof(struct sockaddr_in));
		in.sin_family = AF_INET;
		in.sin_addr.s_addr = haddress;
		in.sin_port = htons(atoi(PORT));

		char str[100];
		sprintf(str, "connecting to %s\n", inet_ntoa(in.sin_addr));
		info_win->printstr(str);

		if (connect(sock, (struct sockaddr *) & in, sizeof(struct sockaddr_in)) == -1) {
			info_win->printstr("can't connect\n");
			return 0;
		}
		info_win->printstr("connected\n");
	}
	/*
	#ifdef WIN32
		unsigned long arg = 1234;
		ioctlsocket(sock, FIONBIO, &arg);
	#else
		fcntl(sock, F_SETFL, O_NONBLOCK);
	#endif
	*/

	/*int opt = 0x4000;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&opt, sizeof(opt)) != 0) {
		info_win->printstr("can't setsockopt\n");
	}

	opt = 0x4000;
	if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&opt, sizeof(opt)) != 0) {
		info_win->printstr("can't setsockopt\n");
	}*/

	//opt = 0;
	//int opt_len = sizeof(opt);
	//getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&opt, &opt_len);
	//printf("sock SO_SNDBUF = %d %d\n", opt, opt_len);
	return 1;
}

void closesocketgame() 
{
	shutdown(sock, SHUT_RDWR);
	if (HOST)
		shutdown(hostsock, SHUT_RDWR);
	close(sock);
	if (HOST)
		close(hostsock);
#ifdef WIN32
	WSACleanup();
#endif
}

char recvbuf[100000];
unsigned int recvbuf_size = 0;

//int packet_recv_socket(char *pkt) {return 0;}

int packet_recv_socket(char *pkt) 
{
	fd_set readfd, exceptfd;
	FD_ZERO(&readfd);
	FD_ZERO(&exceptfd);
	FD_SET(sock, &readfd);
	FD_SET(sock, &exceptfd);

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;
try_select:
	int sv = select(FD_SETSIZE, &readfd, NULL, &exceptfd, &tv);
	if (sv == -1) {
		if (errno == EINTR)
			goto try_select;
		int err = errno;
		char str[100];
		sprintf(str, "select error %d %s\n", err, strerror(err));
		if (info_win != NULL)
			info_win->printstr(str);
		// return 0; //err
	} else {
		if (FD_ISSET(sock, &exceptfd)) {
			return 0;      //err
		}
		if (FD_ISSET(sock, &readfd)) {
			int rnum = recv(sock, recvbuf + recvbuf_size, 100000 - recvbuf_size, 0);
			if (rnum > 0)
				recvbuf_size += rnum;
			//else
			//	return 0; //err
		}
	}

	int pkt_size;
	if (recvbuf_size >= sizeof(pkt_size)) {
		memcpy(&pkt_size, recvbuf, sizeof(pkt_size));
		//assert( (pkt_size > 0)&&(pkt_size < 0xFFFF) );

		if (recvbuf_size >= sizeof(pkt_size) + pkt_size) {
			memcpy(pkt, recvbuf + sizeof(pkt_size), pkt_size);
			recvbuf_size -= sizeof(pkt_size) + pkt_size;
			memmove(recvbuf, recvbuf + sizeof(pkt_size) + pkt_size, recvbuf_size);
			return pkt_size;
		}
	}
	return 0;
}

void packet_send_socket(char *pkt) 
{
	packet_send_socket(pkt, strlen(pkt) + 1);
}

char sendbuf[10000];

//void packet_send_socket(char *pkt, int size) {
//}
//#include <unistd.h>

void packet_send_socket(char *pkt, int size) 
{
	memcpy(sendbuf, &size, sizeof(size));
	memcpy(sendbuf + sizeof(size), pkt, size);
	size += sizeof(size);

	int snum = 0;
	fd_set writefd, exceptfd;
	struct timeval tv;

	while (snum < size) {
		FD_ZERO(&writefd);
		FD_ZERO(&exceptfd);
		FD_SET(sock, &writefd);
		FD_SET(sock, &exceptfd);

		tv.tv_sec = 0;
		tv.tv_usec = 100;
		int sv;
		//try_select:
		sv = select(FD_SETSIZE, NULL, &writefd, &exceptfd, &tv);
		//TEMP_FAILURE_RETRY( sv = select(1, NULL, &writefd, &exceptfd, &tv) );
		if (sv == -1) {
			//if (errno == EINTR)
			//	goto try_select;
			//info_win->printstr("!select");
			//readkey();
			int err = errno;
			char str[100];
			sprintf(str, "send select error %d %s\n", err, strerror(err));
			if (info_win != NULL)
				info_win->printstr(str);
			return ;      //err
		}

		if (FD_ISSET(sock, &exceptfd)) {
			if (info_win != NULL)
				info_win->printstr("exceptfd");
			//readkey();
			return ;      //err
		}

		if (FD_ISSET(sock, &writefd)) {
			int rv = send(sock, sendbuf + snum, size - snum, 0);
			//assert( rv>0 );
			if (rv > 0) {
				snum += rv;
			} else {
				if (info_win != NULL)
					info_win->printstr("rv < 0");
				//readkey();
				//	return; //err
			}
			//printf("<%d>", snum);
		}
	}
}

//#endif
