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

#include "server_protocol.h"
#include "global.h"
#include <string>
#include <vector>
#include "wind.h"

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
		draw_border(temp_bmp, 0, 0, m_width - 1, m_height - 1, xcom1_color(1));
		if (m_title != "") {
			textout_centre(temp_bmp, m_fnt, m_title.c_str(), 1 + (m_width - 5) / 2, 1, xcom1_color(1));
			line(temp_bmp, 0, text_height(m_fnt), m_width - 2, text_height(m_fnt), xcom1_color(1));
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
	USER_STATUS_OFFLINE        //!< user has disconnected from the server (to be removed from the list)
};

struct UserInfo
{
	std::string name;
	USER_STATUS status;

	UserInfo() : name("invalid"), status(USER_STATUS_OFFLINE) { }
	UserInfo(std::string name, USER_STATUS status) : name(name), status(status) { }
};

#define COLOR_YELLOW   makecol(255, 255, 0)
#define COLOR_GREEN    makecol(0, 255, 0)
#define COLOR_DARKGRAY xcom1_color(4)

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
		assert(status != USER_STATUS_OFFLINE);
		m_users.push_back(UserInfo(name, status));
		resize(-1, -1);
	}

	virtual void redraw_full(BITMAP *bmp, int x, int y)
	{
		m_x = x; m_y = y;
		if (m_height == 0 || m_width == 0) return;
		BITMAP *temp_bmp = create_bitmap(m_width, m_height);
		clear_to_color(temp_bmp, xcom1_color(15));

		for (unsigned int i = 0; i < m_users.size(); i++) {
			text_mode(-1);
			int color;
			switch (m_users[i].status) {
				case USER_STATUS_BUSY: color = COLOR_DARKGRAY; break;
				case USER_STATUS_CHALLENGE_IN: color = COLOR_GREEN; break;
				case USER_STATUS_CHALLENGE_OUT: color = COLOR_YELLOW; break;
				default: color = xcom1_color(2); break;
			}

			textout(temp_bmp, m_font, m_users[i].name.c_str(), 0, 
				i * text_height(m_font), color);
		}
		if (bmp == screen) scare_mouse_area(x, y, m_width, m_height);
		blit(temp_bmp, bmp, 0, 0, x, y, m_width, m_height);
		if (bmp == screen) unscare_mouse();
		destroy_bitmap(temp_bmp);
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

int connect_internet_server()
{
    if (!nlInit()) return -1;
    if (!nlSelectNetwork(NL_IP)) return -1;
    nlEnable(NL_SOCKET_STATS);

    ClientServerUfo *server = new ClientServerUfo();
    if (!server->connect("localhost", 2000))
    	return -1;

    char buf[64];
    server->login(std::string("user") + std::string(itoa(rand(), buf, 10)), "");

	lobby_init_mouse();

	ConsoleWindow *chat = new ConsoleWindow(SCREEN_W, SCREEN_H, g_small_font);
	WindowBorder *chat_border = new WindowBorder(chat, "ufo2000 internet server", large);
	chat_border->set_full_redraw();
	UsersList *users = new UsersList(large);
	WindowBorder *users_border = new WindowBorder(users, "users online", large);
	users_border->set_full_redraw();

//	Write greetings and the short help to the chat console
	chat->printf("You have just connected to ufo2000 internet server\n");
	chat->printf("There are two windows here: chat console in the left window\n");
	chat->printf("and the list of online players in the right\n");
	chat->printf("\n");
	chat->printf("white player name - available for chat\n");
	chat->printf(COLOR_YELLOW, "yellow player name - you have sent a challenge to this player\n");
	chat->printf(COLOR_GREEN, "green player name - you can accept a challenge from this player\n");
	chat->printf(COLOR_DARKGRAY, "darkgray player name - the player is busy playing with the others\n");
	chat->printf("\n");
	chat->printf("currently the followind commands are available for debugging:\n");
	chat->printf("  add [username]\n");
	chat->printf("  del [username]\n");
	chat->printf("  host [username]\n");
	chat->printf("  join [username]\n");
	chat->printf("  busy [username]\n");
	chat->printf("\n");

	chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
	users_border->resize(users_border->get_width(), SCREEN_H);

	int mouse_leftr = 0, mouse_rightr = 0;

	while (true)
	{
		NLulong id;
		std::string packet;
		if (server->recv_packet(id, packet))
		{
			switch (id)
			{
				case SRV_USER_ONLINE: users->update_user_info(packet.c_str(), USER_STATUS_READY); break;
				case SRV_MESSAGE: chat->printf("%s", packet.c_str()); break;
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
			if (name != "")
				chat->printf("%s clicked\n", name.c_str());
		}
		if ((mouse_b & 1) && (mouse_leftr)) {
			mouse_rightr = 0;
		}

		process_keyswitch();

		if (keypressed()) {
			int scancode; int keycode = ureadkey(&scancode);

			if (scancode == KEY_F10) {
				change_screen_mode();
				lobby_init_mouse();
			}
			if (scancode == KEY_ESC && askmenu("EXIT GAME")) break;

			if (chat->process_keyboard_input(keycode, scancode)) {
				server->send_packet(SRV_MESSAGE, chat->get_text());

				if (strncmp(chat->get_text(), "add ", 4) == 0)
					users->update_user_info(chat->get_text() + 4, USER_STATUS_READY);
				if (strncmp(chat->get_text(), "del ", 4) == 0)
					users->update_user_info(chat->get_text() + 4, USER_STATUS_OFFLINE);
				if (strncmp(chat->get_text(), "host ", 5) == 0)
					users->update_user_info(chat->get_text() + 5, USER_STATUS_CHALLENGE_OUT);
				if (strncmp(chat->get_text(), "join ", 5) == 0)
					users->update_user_info(chat->get_text() + 5, USER_STATUS_CHALLENGE_IN);
				if (strncmp(chat->get_text(), "busy ", 5) == 0)
					users->update_user_info(chat->get_text() + 5, USER_STATUS_BUSY);
				users_border->resize(-1, -1);
				users_border->resize(users_border->get_width(), SCREEN_H);
				chat_border->resize(SCREEN_W - users_border->get_width(), SCREEN_H);
				chat_border->set_full_redraw();
				users_border->set_full_redraw();
			}
		}
	}

	delete users;
	delete users_border;
	delete chat;
	delete chat_border;

    nlShutdown();

	return -1;
}
