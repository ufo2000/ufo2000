/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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
#undef map
#include "server_protocol.h"
#include <string>
#include <vector>
#include <memory>
#include "wind.h"
#include "config.h"
#include "sound.h"
#include "music.h"
#include "multiplay.h"

#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t + 999) / 1000)
#else
#include <unistd.h>
#endif

#define BORDER_COLOR xcom1_color(4)
#define TITLE_COLOR  xcom1_color(2)

void draw_border(BITMAP *bmp, int x, int y, int w, int h, int color)
{
	line(bmp, x + 1, y + 0, x + w - 2, y + 0, color);
	line(bmp, x + 1, y + h - 2 + 1, x + w - 2, y + h - 2 + 1, color);
	line(bmp, x + 0, y + 1, x + 0, y + h - 2, color);
	line(bmp, x + w - 2 + 1, y + 1, x + w - 2 + 1, y + h - 2, color);
}

class WindowBorder: public VisualObject
{
	int           m_width;
	int           m_height;
	std::string   m_title;
	FONT         *m_fnt;
	VisualObject *m_chield;
	int           m_title_height;

public:
	WindowBorder(VisualObject *chield, const std::string &title = "", FONT *fnt = g_small_font)
		: m_title(title), m_fnt(fnt), m_chield(chield)
	{
		resize(-1, -1);
	}

	virtual void redraw_full(BITMAP *bmp, int x, int y)
	{
		BITMAP *temp_bmp = create_bitmap(m_width, m_height);
		clear_to_color(temp_bmp, xcom1_color(15));
		draw_border(temp_bmp, 0, 0, m_width - 1, m_height - 1, BORDER_COLOR);
		if (m_title != "") {
			textout_centre(temp_bmp, m_fnt, m_title.c_str(), 1 + (m_width - 5) / 2, 1, TITLE_COLOR);
			line(temp_bmp, 0, text_height(m_fnt), m_width - 2, text_height(m_fnt), BORDER_COLOR);
		}
		m_chield->redraw_full(temp_bmp, 2, 2 + m_title_height);
		if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
		blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
		if (bmp == screen) unscare_mouse();
		destroy_bitmap(temp_bmp);
	}
	virtual void redraw_fast(BITMAP *bmp, int x, int y)
	{
		m_chield->redraw(bmp, x + 2, y + 2 + m_title_height);
	}
	virtual bool resize(int width, int height)
	{
		if (width == -1 && height == -1) {
			m_width  = m_chield->get_width() + 5;
			if (m_width < text_length(m_fnt, m_title.c_str()) + 5)
				m_width = text_length(m_fnt, m_title.c_str()) + 5;
			m_height = m_chield->get_height() + 5;
			if (m_title != "") {
				m_title_height = text_height(m_fnt) + 1;
				m_height += m_title_height;
			}
			return true;
		}

		m_chield->resize(width - 5, height - ((m_title == "") ? 5 : 5 + text_height(m_fnt) + 1));

		m_width = width;
		m_height = height;
		return true;
	}
	virtual int get_width() const { return m_width; }
	virtual int get_height() const { return m_height; }
};

/**
 * Other users status on the server from the client's point of view
 */
enum USER_STATUS
{
	USER_STATUS_READY = 0,     //!< user is available in chat
	USER_STATUS_BUSY,          //!< user is playing with someone (not you)
	USER_STATUS_CHALLENGE_IN,  //!< you have received a challenge from this user
	USER_STATUS_CHALLENGE_OUT, //!< you have sent a challenge to this user
	USER_STATUS_OFFLINE,       //!< user has disconnected from the server (to be removed from the list)
	USER_STATUS_SELF           //!< own name in the list of connected players
};

struct UserInfo
{
	std::string name;
	USER_STATUS status;

	UserInfo() : name("invalid"), status(USER_STATUS_OFFLINE) { }
	UserInfo(std::string name, USER_STATUS status) : name(name), status(status) { }
};

#define COLOR_YELLOW   makecol(255, 255, 0)
#define COLOR_GREEN    xcom1_color(50)
#define COLOR_GRAY     xcom1_color(3)
#define COLOR_RED      xcom1_color(32)

class UsersList: public VisualObject
{
	std::vector<UserInfo> m_users;
	FONT *m_font;
	bool m_need_redraw;
	int  m_x;
	int  m_y;
	int  m_width;
	int  m_height;

public:
	UsersList(FONT *font = g_small_font) : m_font(font), m_need_redraw(false)
	{
		resize(-1, -1);
	}

	void update_user_info(const std::string &name, USER_STATUS status)
	{
		for (unsigned int i = 0; i < m_users.size(); i++)
			if (m_users[i].name == name)
			{
				if (status == USER_STATUS_OFFLINE)
					m_users.erase(m_users.begin() + i, m_users.begin() +  i + 1);
				else
					m_users[i].status = status;
				resize(-1, -1);
				return;
			}
		if (status != USER_STATUS_OFFLINE)
			m_users.push_back(UserInfo(name, status));
		resize(-1, -1);
	}

	void remove_all_users()
	{
		m_users.clear();
	}

	virtual void redraw_full(BITMAP *bmp, int x, int y)
	{
		m_x = x; m_y = y; // Hack - only has sence when bmp is screen
		if (m_height == 0 || m_width == 0) return;
		BITMAP *temp_bmp = create_bitmap(m_width, m_height);
		clear_to_color(temp_bmp, xcom1_color(15));

		for (unsigned int i = 0; i < m_users.size(); i++) {
			text_mode(-1);
			int color;
			switch (m_users[i].status) {
				case USER_STATUS_BUSY: color = COLOR_RED; break;
				case USER_STATUS_CHALLENGE_IN: color = COLOR_GREEN; break;
				case USER_STATUS_CHALLENGE_OUT: color = COLOR_YELLOW; break;
				case USER_STATUS_SELF: color = xcom1_color(1); break;
				default: color = COLOR_GRAY; break;
			}

			textout(temp_bmp, m_font, m_users[i].name.c_str(), 0, 
				i * text_height(m_font), color);
		}
		if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
		blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
		if (bmp == screen) unscare_mouse();
		destroy_bitmap(temp_bmp);
	}

	virtual void redraw_fast(BITMAP *bmp, int x, int y)
	{
		m_x = x; m_y = y; // Hack - only has sence when bmp is screen
	}

	virtual int get_width() const { return m_width; }
	virtual int get_height() const { return m_height; }

	virtual bool resize(int width, int height)
	{
		if (width != -1 || height != -1) return false;
		m_width = 0;
		m_height = text_height(m_font) * m_users.size();
    	for (unsigned int i = 0; i < m_users.size(); i++)
    	{
    		int width = text_length(m_font, m_users[i].name.c_str());
    		if (width > m_width) m_width = width;
		}
		set_full_redraw();
		return true;
	}

	std::string mouse_click(int mx, int my)
	{
		if (mx < m_x || mx >= m_x + m_width) return "";
		if (my < m_y || my >= m_y + m_height) return "";
		return m_users[(my - m_y) / text_height(m_font)].name;
	}

	USER_STATUS get_user_status(const std::string &name)
	{
    	for (unsigned int i = 0; i < m_users.size(); i++) {
    		if (m_users[i].name == name)
    			return m_users[i].status;
    	}
    	return USER_STATUS_OFFLINE;
	}
};

void lobby_init_mouse()
{
	BITMAP *mouser = (BITMAP *)datafile[DAT_MOUSE_BMP].dat;
	set_mouse_sprite(mouser);
	set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);
	set_mouse_speed(1, 1);
	set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
	show_mouse(screen);
	gui_fg_color = xcom1_color(15);
	gui_bg_color = xcom1_color(1);
}

//////////////////////////////////////////////////////////////////////////////

class initHawkNL
{
public:
	initHawkNL()
	{ 
		if (!nlInit() || !nlSelectNetwork(NL_IP)) {
			display_error_message("HawkNL library failed to init");
			exit(1);
		}
	    nlEnable(NL_SOCKET_STATS);
	}
	~initHawkNL()
	{
	    nlShutdown();
	}
};

//! Automatically init HawkNL on start and close on exit
static initHawkNL HawkNL;

//////////////////////////////////////////////////////////////////////////////

static int chat_msg_color(const std::string &msg)
{
	int sum = 0;
	for (unsigned int i = 0; i < msg.size(); i++) {
		unsigned char ch = (unsigned char)msg[i];
		sum += ch;
		if (ch == ':') break;
	}
	int colors[] = { 1, 2, 3, 4, 6, 7, 8, 9, 12 };
	return xcom1_color(colors[sum % (sizeof(colors) / sizeof(colors[0]))] * 16);
}

static bool asklogin()
{
	static char login_buffer[1024];
	static char password_buffer[1024];
	static char host_buffer[1024];

	static DIALOG login_dialog[] = {
		//(dialog proc)		 (x)  (y) (w) (h) (fg) (bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
		{ d_shadow_box_proc, 0,   0,  280, 80, 0,  1, 0, 0, 0, 0, NULL, NULL, NULL },
		{ d_rtext_proc,      10,  10, 70,  10, 0,  1, 0, 0, 0, 0, (void *)"Server:", NULL, NULL },
		{ d_edit_proc,       80,  10, 192, 10, 0,  1, 0, 0, 22, 0, (void *)host_buffer, NULL, NULL },
		{ d_rtext_proc,      10,  25, 70,  10, 0,  1, 0, 0, 0, 0, (void *)"Login:", NULL, NULL },
		{ d_edit_proc,       80,  25, 192, 10, 0,  1, 0, 0, 22, 0, (void *)login_buffer, NULL, NULL },
		{ d_rtext_proc,      10,  40, 70,  10, 0,  1, 0, 0, 0, 0, (void *)"Password:", NULL, NULL },
		{ d_edit_proc,       80,  40, 192, 10, 0,  1, 0, 0, 22, 0, (void *)password_buffer, NULL, NULL },
		{ d_button_proc,     140, 55, 60,  18, 0,  1, 13, D_EXIT, 0, 0, (void *)"OK", NULL, NULL },
		{ d_button_proc,     210, 55, 60,  18, 0,  1, 27, D_EXIT, 0, 0, (void *)"Cancel", NULL, NULL },
		{ NULL }
	};

	strcpy(host_buffer, g_server_host.c_str());
	strcpy(login_buffer, g_server_login.c_str());
	strcpy(password_buffer, g_server_password.c_str());

	centre_dialog(login_dialog);
	set_dialog_color(login_dialog, xcom1_color(15), xcom1_color(1));

	if (popup_dialog(login_dialog, 2) == 7) {
		g_server_host = host_buffer;
		g_server_login = login_buffer;
		g_server_password = password_buffer;
		return true;
	}
	return false;
}

const char *get_os_type_string()
{
	switch (os_type) {
		case OSTYPE_WIN3    : return "Windows 3.1 or earlier";
		case OSTYPE_WIN95   : return "Windows 95";
		case OSTYPE_WIN98   : return "Windows 98";
		case OSTYPE_WINME   : return "Windows ME";
		case OSTYPE_WINNT   : return "Windows NT";
		case OSTYPE_WIN2000 : return "Windows 2000";
		case OSTYPE_WINXP   : return "Windows XP";
		case OSTYPE_OS2     : return "OS/2";
		case OSTYPE_WARP    : return "OS/2 Warp 3";
		case OSTYPE_DOSEMU  : return "Linux DOSEMU";
		case OSTYPE_OPENDOS : return "Caldera OpenDOS";
		case OSTYPE_LINUX   : return "Linux";
		case OSTYPE_SUNOS   : return "SunOS/Solaris";
		case OSTYPE_FREEBSD : return "FreeBSD";
		case OSTYPE_NETBSD  : return "NetBSD";
		case OSTYPE_IRIX    : return "IRIX";
		case OSTYPE_QNX     : return "QNX";
		case OSTYPE_UNIX    : return "Unknown Unix variant";
		case OSTYPE_BEOS    : return "BeOS";
		case OSTYPE_MACOS   : return "MacOS";
		case OSTYPE_UNKNOWN : return "unknown";
	}
	return "unknown";
}

int connect_internet_server()
{
	if ((rand() % 2) == 1)
		FS_MusicPlay(F(cfg_get_net2_music_file_name()));
	else
		FS_MusicPlay(F(cfg_get_net1_music_file_name()));
	lobby_init_mouse();

	if (!g_server_autologin || g_server_login == "anonymous")
		if (!asklogin())
			return -1;

    std::auto_ptr<ClientServerUfo> server(new ClientServerUfo());
    std::string error_message;
    if (!server->connect(cfg_get_server_host(), cfg_get_server_proxy(), error_message)) {
		alert(" ", error_message.c_str(), " ", "    OK    ", NULL, 1, 0);
		g_server_autologin = 0;
    	return -1;
	}

    if (!server->login(cfg_get_server_login(), cfg_get_server_password(), error_message)) {
		alert(" ", error_message.c_str(), " ", "    OK    ", NULL, 1, 0);
		g_server_autologin = 0;
		return -1;
	}

	g_server_autologin = 1; // Remember successful login

	std::auto_ptr<ConsoleWindow> chat(new ConsoleWindow(SCREEN_W, SCREEN_H, cfg_get_console_font()));
	std::auto_ptr<WindowBorder> chat_border(new WindowBorder(chat.get(), 
		std::string("ufo2000 internet server (") + cfg_get_server_host() + std::string(")"), large));
	chat_border->set_full_redraw();
	std::auto_ptr<UsersList> users(new UsersList(large));
	users->update_user_info(g_server_login, USER_STATUS_SELF);
	std::auto_ptr<WindowBorder> users_border(new WindowBorder(users.get(), "users online", large));
	users_border->set_full_redraw();

	// Write greetings and the short help to the chat console
	chat->printf("You have just connected to ufo2000 internet server\n");
	chat->printf("There are two windows here: chat console in the left window\n");
	chat->printf("and the list of online players in the right\n");
	chat->printf("\n");
	chat->printf(xcom1_color(1), "white player name - that's your own name\n");
	chat->printf(COLOR_GRAY, "gray player name - available for chat\n");
	chat->printf(COLOR_YELLOW, "yellow player name - you have sent a challenge to this player\n");
	chat->printf(COLOR_GREEN, "green player name - you can accept a challenge from this player\n");
	chat->printf(COLOR_RED, "red player name - the player is busy playing with someone else\n");
	chat->printf("\n");
	chat->printf("You can left click on player names to select them as your opponents\n");
	chat->printf("\n");

	chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
	users_border->resize(users_border->get_width(), SCREEN_H);

	int mouse_leftr = 0, mouse_rightr = 0;

	net->gametype = GAME_TYPE_INTERNET_SERVER;
	net->m_internet_server = server.get();
	net->send_debug_message("system:%s", get_os_type_string());

	while (true)
	{
		NLulong id;
		std::string packet;
		int res = server->recv_packet(id, packet);
		if (res == -1) {
			alert(" ", "  Connecton lost  ", " ", "    OK    ", NULL, 1, 0);
			return -1;
		}
		if (res != 0)
		{
			switch (id)
			{
				case SRV_USER_ONLINE: users->update_user_info(packet, USER_STATUS_READY); break;
				case SRV_USER_OFFLINE: users->update_user_info(packet, USER_STATUS_OFFLINE); break;
				case SRV_USER_CHALLENGE_IN:
					soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
					users->update_user_info(packet, USER_STATUS_CHALLENGE_IN);
					break;
				case SRV_USER_CHALLENGE_OUT: users->update_user_info(packet, USER_STATUS_CHALLENGE_OUT); break;
				case SRV_USER_BUSY: users->update_user_info(packet, USER_STATUS_BUSY); break;
				case SRV_MESSAGE: chat->printf(chat_msg_color(packet), "%s", packet.c_str()); break;
				case SRV_GAME_START_HOST:
					show_mouse(NULL);
					alert(" ", "  Game should start as host now ", " ", "    OK    ", NULL, 1, 0);
					FS_MusicPlay(NULL);
		            HOST = 1;

		            if (initgame()) {
		                gameloop();
		                closegame();
		            }

					users->remove_all_users();
					users->update_user_info(g_server_login, USER_STATUS_SELF);

					server->send_packet(SRV_ENDGAME, "");
					if ((rand() % 2) == 1)
						FS_MusicPlay(F(cfg_get_net2_music_file_name()));
					else
						FS_MusicPlay(F(cfg_get_net1_music_file_name()));
					lobby_init_mouse();
					break;
				case SRV_GAME_START_JOIN:
					show_mouse(NULL);
					alert(" ", "  Game should start as client now ", " ", "    OK    ", NULL, 1, 0);
					FS_MusicPlay(NULL);
		            HOST = 0;

		            if (initgame()) {
		                gameloop();
		                closegame();
		            }

					users->remove_all_users();
					users->update_user_info(g_server_login, USER_STATUS_SELF);

					server->send_packet(SRV_ENDGAME, "");
					if ((rand() % 2) == 1)
						FS_MusicPlay(F(cfg_get_net2_music_file_name()));
					else
						FS_MusicPlay(F(cfg_get_net1_music_file_name()));
					lobby_init_mouse();
					break;
			}

			users_border->resize(-1, -1);
			users_border->resize(users_border->get_width(), SCREEN_H);
			chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
			chat_border->set_full_redraw();
			users_border->set_full_redraw();
		}

		chat_border->redraw(screen, 0, 0);
		users_border->redraw(screen, SCREEN_W - users_border->get_width(), 0);

		if (!(mouse_b & 1)) mouse_leftr = 1;
		if (!(mouse_b & 2)) mouse_rightr = 1;

		if ((mouse_b & 1) && (mouse_leftr)) {
			mouse_leftr = 0;
			std::string name = users->mouse_click(mouse_x, mouse_y);
			if (name != "") {
				switch (users->get_user_status(name)) {
					case USER_STATUS_READY:
						if (server->challenge(name))
							users->update_user_info(name, USER_STATUS_CHALLENGE_OUT);
						break;
					case USER_STATUS_CHALLENGE_IN:
						server->challenge(name);
						break;
					default:
						soundSystem::getInstance()->play(SS_BUTTON_PUSH_1);
						break;
				}
				users_border->resize(-1, -1);
				users_border->resize(users_border->get_width(), SCREEN_H);
				chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
				chat_border->set_full_redraw();
				users_border->set_full_redraw();
			}
		}
		if ((mouse_b & 1) && (mouse_leftr)) {
			mouse_rightr = 0;
		}

		process_keyswitch();

		if (keypressed()) {
			int scancode; int keycode = ureadkey(&scancode);

			switch (scancode) {
				case KEY_PLUS_PAD:
					FS_IncMusicVolume();
					break;
				case KEY_MINUS_PAD:
					FS_DecMusicVolume();
					break;
				case KEY_F9:
					keyswitch(0);
					break;
				case KEY_F10:
					change_screen_mode();
					lobby_init_mouse();
					break;
				case KEY_ESC:
					if (askmenu("DISCONNECT FROM SERVER"))
						return -1;
					break;
				default:
					if (chat->process_keyboard_input(keycode, scancode)) {
						server->send_packet(SRV_MESSAGE, chat->get_text());
						users_border->resize(-1, -1);
						users_border->resize(users_border->get_width(), SCREEN_H);
						chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
						chat_border->set_full_redraw();
						users_border->set_full_redraw();
					}
			}
		}
		// Do not load cpu so heavy
		usleep(10000);
	}

	show_mouse(NULL);
	FS_MusicPlay(NULL);
	return -1;
}
