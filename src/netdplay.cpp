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
#include "global.h"
#if defined (WIN32) && defined(HAVE_DPLAY)

#include "wind.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <windowsx.h>
#include <allegro.h>
#define WIN32_LEAN_AND_MEAN
#include <winalleg.h>
#include <allegro/platform/aintwin.h>
#include <dplay.h>
#include "netdplay.h"

#define MAXPLAYERS 2

//0.1.5
// {3885ACC0-B7E7-11d4-8150-00C026EE24C6}
//static const GUID DPXCOM_GUID =
//{ 0x3885acc0, 0xb7e7, 0x11d4, { 0x81, 0x50, 0x0, 0xc0, 0x26, 0xee, 0x24, 0xc6 } };

//0.1.9, 0.2.9, 0.3.0
// {DAC8F882-F6E3-11d4-B367-00C026EE24C6}
//static const GUID DPXCOM_GUID =
//{ 0xdac8f882, 0xf6e3, 0x11d4, { 0xb3, 0x67, 0x0, 0xc0, 0x26, 0xee, 0x24, 0xc6 } };

//0.3.1
// {625201E1-344F-11d5-9EB2-00C026EE24C6}
static const GUID DPXCOM_GUID =
    {
        0x625201e1, 0x344f, 0x11d5, { 0x9e, 0xb2, 0x0, 0xc0, 0x26, 0xee, 0x24, 0xc6 }
    };


static LPDIRECTPLAY4A	lpDirectPlay4A;
static DPID dpidPlayer;
//HANDLE hPlayerEvent;


static BYTE Connection[10][1000];
static int Connection_num = 0;
static char Connection_name[10][1000];

static char *connbox_getter(int index, int *list_size)
{
	if (index < 0) {
		*list_size = Connection_num;
		return NULL;
	} else
		return Connection_name[index];
}


static DIALOG conn_dialog[] = {
                                  // (dialog proc)	  (x)	 (y)	 (w)  (h)	(fg)  (bg)  (key) (flags)		(d1)				 (d2)		 (dp)				 (dp2) (dp3)
                                  { d_box_proc, 110, 140, 420, 100, 0, 1, 0, D_EXIT, 0, 0, NULL, NULL, NULL },
                                  { d_list_proc, 120, 150, 400, 50, 0, 1, 0, D_EXIT, 0, 0, connbox_getter, NULL, NULL },
                                  {d_button_proc, 165, 210, 100, 20, 0, 1, 0, D_EXIT, 0, 0, (void *)"Select", NULL, NULL },
                                  {d_button_proc, 165 + 100 + 100, 210, 100, 20, 0, 1, 0, D_EXIT, 0, 0, (void *)"Cancel", NULL, NULL },
                                  { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
                              };


static GUID Session[10];
static int Session_num = 0;
static char Session_name[10][1000];

static char *sessbox_getter(int index, int *list_size)
{
	if (index < 0) {
		*list_size = Session_num;
		return NULL;
	} else
		return Session_name[index];
}


static DIALOG sess_dialog[] = {
                                  // (dialog proc)	  (x)	 (y)	 (w)	(h)	(fg)  (bg)  (key) (flags)	  (d1)				 (d2)		(dp)				  (dp2) (dp3)
                                  { d_box_proc, 110, 140, 420, 100, 0, 1, 0, D_EXIT, 0, 0, NULL, NULL, NULL },
                                  { d_list_proc, 120, 150, 400, 50, 0, 1, 0, 0, 0, 0, sessbox_getter, NULL, NULL },
                                  {d_button_proc, 150, 210, 100, 20, 0, 1, 0, D_EXIT, 0, 0, (void *)"Join", NULL, NULL },
                                  {d_button_proc, 150 + 115 + 115, 210, 100, 20, 0, 1, 0, D_EXIT, 0, 0, (void *)"Cancel", NULL, NULL },
                                  {d_button_proc, 150 + 115, 210, 100, 20, 0, 1, 0, D_EXIT, 0, 0, (void *)"Rescan", NULL, NULL },
                                  { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
                              };



static BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(
    LPCGUID lpguidSP,
    LPVOID	lpConnection,
    DWORD	dwConnectionSize,
    LPCDPNAME	lpName,
    DWORD	dwFlags,
    LPVOID	lpContext)
{

	//HWND			hWnd = (HWND) lpContext;
	//LRESULT		iIndex;
	//LPVOID		 lpConnectionBuffer;

	// store service provider name in combo box
	//iIndex = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_ADDSTRING, 0,
	//						 (LPARAM) lpName->lpszShortNameA);
	//printf("\t\t%s\n", lpName->lpszShortNameA);
	char oem_str[0x100];
	CharToOem(lpName->lpszShortNameA, oem_str);
	//printf("\tecc %d. %s\n", Connection_num, oem_str);
	char str[1000];
	sprintf(str, "ecc %d. %s\n", Connection_num, oem_str);
	//info_win->printstr(str);
	sprintf(Connection_name[Connection_num], "%s", oem_str);
	//strcpy(Connection_name[Connection_num], str);

	//if (iIndex == CB_ERR)
	// goto FAILURE;

	// make space for connection shortcut
	//lpConnectionBuffer = GlobalAllocPtr(GHND, dwConnectionSize);
	//if (lpConnectionBuffer == NULL)
	// goto FAILURE;

	// store pointer to connection shortcut in combo box
	//memcpy(lpConnectionBuffer, lpConnection, dwConnectionSize);
	//SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_SETITEMDATA, (WPARAM) iIndex,
	//							 (LPARAM) lpConnectionBuffer);
	//printf("\t\t_%s\n", lpConnectionBuffer);
	memcpy(Connection[Connection_num++], lpConnection, dwConnectionSize);

	//FAILURE:
	return (TRUE);
}


static BOOL FAR PASCAL DirectPlayEnumSessionsCallback(
    LPCDPSESSIONDESC2 lpSessionDesc,
    LPDWORD	lpdwTimeOut,
    DWORD	dwFlags,
    LPVOID	lpContext)
{
	//HWND		hWnd = (HWND) lpContext;
	//LPGUID	 lpGuid;
	//LONG		iIndex;

	// see if last session has been enumerated
	if (dwFlags & DPESC_TIMEDOUT)
		return (FALSE);

	// store session name in list
	//iIndex = SendDlgItemMessage( hWnd, IDC_SESSIONLIST, LB_ADDSTRING,
	//						 (WPARAM) 0, (LPARAM) lpSessionDesc->lpszSessionNameA);

	//if (iIndex == LB_ERR)
	// goto FAILURE;
	char oem_str[0x100];
	CharToOem(lpSessionDesc->lpszSessionNameA, oem_str);
	//printf("\tesc %d. %s\n", Session_num, oem_str);
	//char str[1000];
	//sprintf(str, "esc %d. %s\n", Session_num, oem_str);
	//info_win->printstr(str);
	sprintf(Session_name[Session_num], "%s", oem_str);


	// make space for session instance guid
	//lpGuid = (LPGUID) GlobalAllocPtr( GHND, sizeof(GUID) );
	//if (lpGuid == NULL)
	// goto FAILURE;

	// store pointer to guid in list
	//*lpGuid = lpSessionDesc->guidInstance;
	//SendDlgItemMessage( hWnd, IDC_SESSIONLIST, LB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpGuid);
	Session[Session_num++] = lpSessionDesc->guidInstance;
	//FAILURE:
	return (TRUE);
}


int initdplaygame()
{
	HRESULT hr;

	//allegro init it
	//hr = CoInitialize(NULL);
	//if (FAILED(hr))
	//	info_win->printstr("CoInitialize fail\n");
	//printf("\tCoInitialize fail\n");

	hr = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
	                      IID_IDirectPlay4A, (LPVOID*) & lpDirectPlay4A);
	if (FAILED(hr))
		info_win->printstr("CoCreateInstance fail\n");
	//printf("\tCoCreateInstance fail\n");


	lpDirectPlay4A->EnumConnections(&DPXCOM_GUID,
	                                DirectPlayEnumConnectionsCallback, NULL, 0);

	if (popup_dialog(conn_dialog, 0) == 3)
		return 0;
	int ind = conn_dialog[1].d1;

	hr = lpDirectPlay4A->InitializeConnection(Connection[ind], 0);
	if (FAILED(hr))
		info_win->printstr("InitializeConnection fail\n");
	//printf("\tInitializeConnection fail\n");

	info_win->redraw_full();
	DPSESSIONDESC2 sessionDesc;

	if (HOST) {

		ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
		sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
		sessionDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
		sessionDesc.guidApplication = DPXCOM_GUID;
		sessionDesc.dwMaxPlayers = MAXPLAYERS;
		sessionDesc.lpszSessionNameA = "UFO2000 Session";

		hr = lpDirectPlay4A->Open(&sessionDesc, DPOPEN_CREATE);
		if FAILED(hr)
			info_win->printstr("HOST Open fail\n");
		//printf("\tHOST Open fail\n");
		/*else {
			printf("\tHOST Open\n");
			getch();
		}*/

	} else {
rescan:
		info_win->redraw_full();
		ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
		sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
		sessionDesc.guidApplication = DPXCOM_GUID;

		hr = lpDirectPlay4A->EnumSessions(&sessionDesc, 0,
		                                  DirectPlayEnumSessionsCallback, NULL,
		                                  DPENUMSESSIONS_AVAILABLE);
		if (FAILED(hr))
			info_win->printstr("EnumSessions fail\n");
		//printf("\tEnumSessions fail\n");

		if (!Session_num)
			sess_dialog[2].flags = D_DISABLED;
		else
			sess_dialog[2].flags = D_EXIT;

		//int sn = getch();
		//int sn = readkey() & 0xff;
		//sn -= '0';
		//int sn = 0;
		info_win->redraw_full();
		int res = popup_dialog(sess_dialog, 0);
		info_win->redraw_full();
		if (res == 4) {
			Session_num = 0;
			goto rescan;

		}
		if (res != 2)
			return 0;

		int sn = sess_dialog[1].d1;


		ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
		sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
		sessionDesc.guidInstance = Session[sn];

		hr = lpDirectPlay4A->Open(&sessionDesc, DPOPEN_JOIN);
		if FAILED(hr)
			info_win->printstr("JOIN Open fail\n");
		//printf("\tJOIN Open fail\n");

	}

	DPNAME dpName;
	ZeroMemory(&dpName, sizeof(DPNAME));
	dpName.dwSize = sizeof(DPNAME);
	dpName.lpszShortNameA = "lpszPlayerName";
	dpName.lpszLongNameA = NULL;

	/*hPlayerEvent = CreateEvent(NULL,	  // no security
										FALSE,		// auto reset
										FALSE,		// initial event reset
										NULL);		// no name
	*/

	// create a player with this name
	//hr = lpDirectPlay4A->CreatePlayer(&dpidPlayer, &dpName,
	//					 hPlayerEvent, NULL, 0, 0);
	hr = lpDirectPlay4A->CreatePlayer(&dpidPlayer, &dpName,
	                                  NULL, NULL, 0, 0);
	if (FAILED(hr))
		info_win->printstr("CreatePlayer fail\n");
	//printf("\tCreatePlayer fail\n");

	//printf("\ninitdplaygame\n");
	info_win->printstr("\ninitdplaygame\n");
	info_win->redraw_full();
	return 1;
}

void closedplaygame()
{
	Connection_num = 0;
	Session_num = 0;
	if (lpDirectPlay4A) {
		if (dpidPlayer)
			lpDirectPlay4A->DestroyPlayer(dpidPlayer);
		lpDirectPlay4A->Close();
		lpDirectPlay4A->Release();
	}
	//if (hPlayerEvent)
	// CloseHandle(hPlayerEvent);

	//CoUninitialize();
	printf("\nclosedplaygame\n");
}

int packet_recv_dplay(char *pkt)
{
	DPID idFrom, idTo;
	DWORD dwDataSize = 1000;
	HRESULT hr = lpDirectPlay4A->Receive(&idFrom, &idTo, 0,
	                                     pkt, &dwDataSize);
	if (hr == DP_OK) {
		pkt[dwDataSize] = 0;

		if (idFrom == DPID_SYSMSG) {
			LPDPMSG_GENERIC sm = (LPDPMSG_GENERIC)pkt;
			if (info_win != NULL) {
				char str[1000];
				sprintf(str, "DPID_SYSMSG %d", sm->dwType);
				info_win->printstr(str);
				if (sm->dwType == DPSYS_CREATEPLAYERORGROUP)
					info_win->printstr("(remote join)\n");
			}

			//printf("\tDPID_SYSMSG %d\n", sm->dwType);
			return 0;
		}
		//return 1;
		return dwDataSize;
	}
	if (hr != DPERR_NOMESSAGES)
		printf("\tReceive fail\n");
	return 0;
}

void packet_send_dplay(char *pkt)
{
	HRESULT hr = lpDirectPlay4A->Send(dpidPlayer, DPID_ALLPLAYERS, 0,
	                                  pkt, strlen(pkt));
	if FAILED(hr)
		printf("\tSend fail\n");
}


void packet_send_dplay(char *pkt, int size)
{
	HRESULT hr = lpDirectPlay4A->Send(dpidPlayer, DPID_ALLPLAYERS, 0,
	                                  pkt, size);
	if FAILED(hr)
		printf("\tSend fail\n");
}

#else

int initdplaygame()
{
	return 0;
}
void closedplaygame()
{}
int packet_recv_dplay(char *pkt)
{
	return 0;
}
void packet_send_dplay(char *pkt)
{}
void packet_send_dplay(char *pkt, int size)
{}

#endif
