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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <fstream>
#include "version.h"
#include "pck.h"
#include "explo.h"
#include "soldier.h"
#include "platoon.h"
#include "map.h"
#include "icon.h"
#include "inventory.h"
#include "sound.h"
#include "multiplay.h"
#include "wind.h"
#include "config.h"
#include "mainmenu.h"
#include "editor.h"
#include "about.h"
#include "video.h"
#include "keys.h"
#include "crc32.h"

#include "sysworkarounds.h"


//#define DEBUG
#define MSCROLL 10
#define BACKCOLOR xcom1_color(15)

Target target;
int HOST, DONE, TARGET, turn;
Mode MODE;
ConsoleWindow *g_console;

//! Limit of time for a single turn in seconds
int g_time_limit;
//! Current counter for time left for this turn
volatile int g_time_left;

Net *net;
Map *map;
TerrainSet *terrain_set;
Icon *icon;
Inventory *inventory;
About *about;
Editor *editor;
Platoon *p1, *p2;
Platoon *platoon_local, *platoon_remote;
Soldier *sel_man = NULL;
Explosive *elist;

volatile unsigned int ANIMATION = 0;
volatile int CHANGE = 1;
volatile int MOVEIT = 0;
volatile int FLYIT = 0;
volatile int NOTICE = 1;
volatile int MAPSCROLL = 1;
int NOTICEremote = 0;
int NOTICEdemon = 0;

void mouser_proc(int flags)
{
	CHANGE = 1;
}
END_OF_FUNCTION(mouser_proc);

void timer_handler()
{
	ANIMATION++;
	MOVEIT++;
}
END_OF_FUNCTION(timer_handler);

void timer_handler2()
{
	FLYIT++;
}
END_OF_FUNCTION(timer_handler2);

void timer_1s()
{
	if (g_time_left > 0) g_time_left--;
	NOTICE++;
}
END_OF_FUNCTION(timer_1s);

void timer_handler4()
{
	MAPSCROLL++;
}
END_OF_FUNCTION(timer_handler4);

int speed_unit = 15;
int speed_bullet = 30;
int speed_mapscroll = 30;
int mapscroll = 10;

void install_timers(int _speed_unit, int _speed_bullet, int _speed_mapscroll)
{
	install_int_ex(timer_handler, BPS_TO_TIMER(_speed_unit));     //ticks each second
	install_int_ex(timer_handler2, BPS_TO_TIMER(_speed_bullet * 2));     //ticks each second
	install_int_ex(timer_handler4, BPS_TO_TIMER(_speed_mapscroll));     //ticks each second
	install_int_ex(timer_1s, BPS_TO_TIMER(1));     //ticks each second
}

void uninstall_timers()
{
	remove_int(timer_handler4);
	remove_int(timer_1s);
	remove_int(timer_handler2);
	remove_int(timer_handler);
}

int keyboard_proc(int key)
{
	if (key >> 8 == KEY_F10) {
		change_screen_mode();
		return 0;
	}
	return key;
}
END_OF_FUNCTION(keyboard_proc);

GEODATA mapdata;
PLAYERDATA pd1, pd2;
PLAYERDATA *pd_local, *pd_remote;
int local_platoon_size;

// For endgame.
int win;
int loss;

//simple all of new - rem about rand in sol's constructors
//can diff on remote comps
void restartgame()
{
	win = 0;
	loss = 0;
	//map = new Map(4,4);
	//map->load("floor0.map", Map::cultivat);
	//map = new Map("GEODATA.DAT");
	map = new Map(mapdata);
	p1 = new Platoon(1000, &pd1);
	p2 = new Platoon(2000, &pd2);

	int fh = open(F("$(home)/cur_map.dat"), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
	ASSERT(fh != -1);
	write(fh, &mapdata, sizeof(mapdata));
	close(fh);

	fh = open(F("$(home)/cur_p1.dat"), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
	ASSERT(fh != -1);
	write(fh, &pd1, sizeof(pd1));
	close(fh);

	fh = open(F("$(home)/cur_p2.dat"), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
	ASSERT(fh != -1);
	write(fh, &pd2, sizeof(pd2));
	close(fh);

	//srand(0); //workaround!!!
	//p1 = new Platoon(1111, P1S, P1Z, P1X, P1Y, 0);
	//p2 = new Platoon(2222, P2S, P2Z, P2X, P2Y, 4);
	//srand(time(NULL));

	/*map->place(0, P1X, P1Y+2)->put(new Item(HIGH_EXPLOSIVE));
	map->place(0, P2X, P2Y+2)->put(new Item(HIGH_EXPLOSIVE));

	map->place(0, P1X, P1Y+2)->put(new Item(LASER_PISTOL));
	map->place(0, P2X, P2Y+2)->put(new Item(LASER_PISTOL));

	map->place(0, P1X, P1Y+2)->put(new Item(LASER_GUN));
	map->place(0, P2X, P2Y+2)->put(new Item(LASER_GUN));*/

	elist = new Explosive();
	elist->reset();
	if (HOST) {
		platoon_local = p1;
		platoon_remote = p2;
		MODE = MAP3D;
	} else {
		platoon_local = p2;
		platoon_remote = p1;
		MODE = WATCH;
	}

	// Initial statistics get set properly here (strength effect on TU, mostly).
	platoon_local->restore();
	platoon_remote->restore();

	//sel_man = NULL;
	sel_man = platoon_local->captain();
	if (sel_man != NULL) map->center(sel_man);
	DONE = 0; TARGET = 0; turn = 0;
}

int initgame()
{
	play_midi(g_setup_midi_music, 1);
	if (!net->init())
	{
		play_midi(NULL, 0);
		return 0;
	}
	play_midi(NULL, 0);

	install_timers(speed_unit, speed_bullet, speed_mapscroll);
	//mouse_callback = mouser_proc;

	reset_video();
	restartgame();
	resize_screen2(0, 0);
	//clear_to_color(screen, 58); //!!!!!

	return 1;
}

void closegame()
{
	delete p1;
	p1 = NULL;
	delete p2;
	p2 = NULL;
	delete map;
	map = NULL;
	delete elist;
	elist = NULL;

	uninstall_timers();
	net->close();
}

int print_y = 0;
Wind *print_win = NULL;

class consoleBuf : public std::streambuf {
    std::string curline;
    bool doCout;

protected:
    virtual int overflow(int c) {
        if (doCout)
            if (c == 10)
                std::cout<<std::endl;
            else
                std::cout<<(static_cast<char>(c));
        if (c == 10) {
            if (print_win != NULL) {
                curline.append("\r\n");
                print_win->printstr(curline.c_str(), xcom1_color(1));
            } else {
                curline.append("\n");
                text_mode( -1);
                textout(screen, font, curline.c_str(), 0, print_y, xcom1_color(1));
                print_y += 10;
            }
            curline.assign("");
        } else {
            curline += static_cast<char>(c);
        }

        return c;
    }
public:
    consoleBuf(bool dco) {
        curline.assign("");
        doCout = dco;
    }
};


#define FADE_SPEED 20


/**
 * Display error message
 */
void display_error_message(const std::string &error_text)
{
#ifdef WIN32
	MessageBox(NULL, error_text.c_str(), "UFO2000 Error!", MB_OK);
#else
	fprintf(stderr, "\n%s\n", error_text.c_str());
#endif
	exit(1);
}

static int assert_handler(const char *msg)
{
	net->send_debug_message("assert:%s", msg);
	display_error_message(msg);
	return 0;
}

lua_State *L;

static int lua_UpdateCrc32(lua_State *L)
{
	int n = lua_gettop(L);
	if (n != 2 || !lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
		lua_pushstring(L, "incorrect arguments to function `UpdateCrc32'");
		lua_error(L);
    }

    unsigned long result = update_crc32(
    	(unsigned long)lua_tonumber(L, 1), lua_tostring(L, 2), lua_strlen(L, 2));

    lua_pushnumber(L, result);
    return 1;
}

static int lua_ALERT(lua_State *L)
{
	int n = lua_gettop(L);
	if (n != 1 || !lua_isstring(L, 1)) {
		display_error_message("Lua unknown error");
		return 0;
	}
	display_error_message(lua_tostring(L, 1));
	return 0;
}

const char *F(const char *fileid)
{
	static std::string fname;
	int stack_top = lua_gettop(L);
	lua_pushstring(L, "GetDataFileName");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (!lua_isfunction(L, -1))
		display_error_message("Fatal: no 'GetDataFileName' function registered");
	lua_pushstring(L, fileid);
	lua_call(L, 1, 1);
	if (!lua_isstring(L, -1))
		display_error_message(std::string("Can't find file name by ") + fileid + " identifier");
	fname = lua_tostring(L, -1);
	lua_settop(L, stack_top);

	return fname.c_str();
}

void initmain(int argc, char *argv[])
{
	register_assert_handler(assert_handler);
	srand(time(NULL));
	set_uformat(U_UTF8);
	allegro_init();
	register_bitmap_file_type("jpg", load_jpg, NULL);
	set_color_conversion(COLORCONV_TOTAL | COLORCONV_DITHER);

	L = lua_open();
	lua_register(L, "UpdateCrc32", lua_UpdateCrc32);
	lua_register(L, "_ALERT", lua_ALERT);
	luaopen_base(L);
	luaopen_string(L);
	luaopen_io(L);

#ifdef WIN32
	// Win32-version has all data files in a single directory where it was installed
	char ufo2000_dir[MAX_PATH];
	GetModuleFileName(NULL, ufo2000_dir, sizeof(ufo2000_dir));
	if (strrchr(ufo2000_dir, '\\')) *strrchr(ufo2000_dir, '\\') = '\0';
	char *p = ufo2000_dir;
	while (*p != '\0') { if (*p == '\\') *p = '/'; p++; }
	SetCurrentDirectory(ufo2000_dir);
	lua_pushstring(L, "ufo2000_dir");
	lua_pushstring(L, ufo2000_dir);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "home_dir");
	lua_pushstring(L, ufo2000_dir);
	lua_settable(L, LUA_GLOBALSINDEX);
#endif

#ifdef DATA_DIR
	// A directory for ufo2000 data files was specified at compile time
	lua_pushstring(L, "ufo2000_dir");
	lua_pushstring(L, DATA_DIR);
	lua_settable(L, LUA_GLOBALSINDEX);
	
	char *env_home = getenv("HOME");
	if (env_home) {
		std::string home_dir = std::string(env_home) + "/.ufo2000";
#ifdef LINUX
		mkdir(home_dir.c_str(), 0755);
#else
		mkdir(home_dir.c_str());
#endif
		lua_pushstring(L, "home_dir");
		lua_pushstring(L, home_dir.c_str());
		lua_settable(L, LUA_GLOBALSINDEX);
	}
#else
#define DATA_DIR "."
#endif

	lua_dofile(L, DATA_DIR "/init-scripts/main.lua");
	lua_dofile(L, DATA_DIR "/init-scripts/filecheck.lua");
	lua_dofile(L, DATA_DIR "/init-scripts/standard-maps.lua");
	lua_dofile(L, DATA_DIR "/init-scripts/user-maps.lua");

    FLAGS = 0;
	push_config_state();
	set_config_file(F("$(home)/ufo2000.ini"));
	if (get_config_int("Flags", "F_CLEARSEEN", 0)) FLAGS |= F_CLEARSEEN;      // clear seen every time
	if (get_config_int("Flags", "F_SHOWROUTE", 0)) FLAGS |= F_SHOWROUTE;      // show pathfinder matrix
	if (get_config_int("Flags", "F_SHOWLOFCELL", 0)) FLAGS |= F_SHOWLOFCELL;  // show cell's LOF & BOF
	if (get_config_int("Flags", "F_SHOWLEVELS", 0)) FLAGS |= F_SHOWLEVELS;    // show all level
	if (get_config_int("Flags", "F_FASTSTART", 0)) FLAGS |= F_FASTSTART;      // skip
	if (get_config_int("Flags", "F_FULLSCREEN", 0)) FLAGS |= F_FULLSCREEN;    // start fullscreen mode
	if (get_config_int("Flags", "F_RAWMESSAGES", 0)) FLAGS |= F_RAWMESSAGES;  // show raw net packets
	if (get_config_int("Flags", "F_SEL_ANY_MAN", 0)) FLAGS |= F_SEL_ANY_MAN;  // allow select any man
	if (get_config_int("Flags", "F_SWITCHVIDEO", 1)) FLAGS |= F_SWITCHVIDEO;  // allow switch full/window screen mode
	if (get_config_int("Flags", "F_PLANNERDBG", 0)) FLAGS |= F_PLANNERDBG;    // mission planner debug mode
	if (get_config_int("Flags", "F_ENDLESS_TU", 0)) FLAGS |= F_ENDLESS_TU;    // endless soldier's time units
	if (get_config_int("Flags", "F_SAFEVIDEO", 1)) FLAGS |= F_SAFEVIDEO;      // enable if you experience bugs with video
	if (get_config_int("Flags", "F_SELECTENEMY", 1)) FLAGS |= F_SELECTENEMY;  // draw blue arrows and numbers above seen enemies
	if (get_config_int("Flags", "F_FILECHECK", 1)) FLAGS |= F_FILECHECK;      // check for datafiles integrity
	if (get_config_int("Flags", "F_LARGEFONT", 0)) FLAGS |= F_LARGEFONT;      // use big ufo font for dialogs, console and stuff.
    if (get_config_int("Flags", "F_SMALLFONT", 0)) FLAGS |= F_SMALLFONT;      // no, use small font instead.
    if (get_config_int("Flags", "F_SOUNDCHECK", 0)) FLAGS |= F_SOUNDCHECK;    // perform soundtest.
    if (get_config_int("Flags", "F_LOGTOSTDOUT", 0)) FLAGS |= F_LOGTOSTDOUT;  // Copy all init console output to stdout.
    if (get_config_int("Flags", "F_DEBUGDUMPS", 0)) FLAGS |= F_DEBUGDUMPS;    // Produce a lot of files with the information which can help in debugging

	if (argc > 1) {
		g_server_login = argv[1];
		g_server_password = argc > 2 ? argv[2] : "";
	} else {
		g_server_login = get_config_string("Server", "login", "anonymous");
		g_server_password = get_config_string("Server", "password", "");
	}

	loadini();
	pop_config_state();

	datafile = load_datafile("#");
	if (datafile == NULL) {
		datafile = load_datafile(F("$(ufo2000)/ufo2000.dat"));
		if (datafile == NULL) {
			allegro_exit();
			fprintf(stderr, "Error loading datafile!\n\n");
			exit(1);
		}
	}

	set_window_title("UFO2000");
	set_window_close_button(0);

	set_video_mode();
	set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);

	BITMAP *text_back = load_back_image(cfg_get_loading_image_file_name());
	stretch_blit(text_back, screen, 0, 0, text_back->w, text_back->h, 0, 0, SCREEN_W, SCREEN_H);
	print_win = new Wind(text_back, 15, 300, 625, 390, xcom1_color(255));

    /* to use the init console as an ostream -very handy. */
    consoleBuf consbuf(FLAGS & F_LOGTOSTDOUT);
    std::ostream console(&consbuf);

	console<<"allegro_init"<<std::endl;

	console<<"install_timer"<<std::endl;
	install_timer();
	console<<"install_mouse"<<std::endl;
	install_mouse();
	console<<"install_keyboard"<<std::endl;
	install_keyboard();
    {
        bool VERBOSE_SOUNDCHECK = false;
        
        console<<"Initializing sound..."<<std::endl;
        std::string xml;
        soundSystem *ss = soundSystem::getInstance();
        std::ifstream ifs_xml(F("$(ufo2000)/soundmap.xml"));
        if (ifs_xml) {
            ISTREAM_TO_STRING(ifs_xml, xml);

            if (FLAGS & F_SOUNDCHECK) {
                if (0 == ss->initialize(xml, &console, VERBOSE_SOUNDCHECK)) {
                    console<<"  Soundcheck in progress..."<<std::endl;
                    ss->playLoadedSamples(&console);
                } else {
                    console<<"  soundSystem initialization failed."<<std::endl;
                }
            } else {
                if ( 0 > ss->initialize(xml, &console, VERBOSE_SOUNDCHECK))
                    console<<"  Failed."<<std::endl;
            }
        } else {
            console<<"  Error reading soundmap.xml"<<std::endl;
        }
    }
	console<<"initvideo"<<std::endl;
	initvideo();

	LOCK_VARIABLE(CHANGE); LOCK_FUNCTION(mouser_proc);
	LOCK_VARIABLE(MOVEIT); LOCK_VARIABLE(ANIMATION); LOCK_FUNCTION(timer_handler);
	LOCK_VARIABLE(FLYIT); LOCK_FUNCTION(timer_handler2);
	LOCK_VARIABLE(NOTICE); LOCK_VARIABLE(g_time_left); LOCK_FUNCTION(timer_1s);
	LOCK_VARIABLE(MAPSCROLL); LOCK_FUNCTION(timer_handler4);

	LOCK_FUNCTION(keyboard_proc);

	console<<"initpck units"<<std::endl;
	Soldier::initpck();

	console<<"initpck terrain"<<std::endl;
	Map::initpck();

	console<<"init obdata"<<std::endl;
	Item::initobdata();

	console<<"init bigobs"<<std::endl;
	Item::initbigobs();

	console<<"new console window"<<std::endl;
	g_console = new ConsoleWindow(SCREEN_W, SCREEN_H - SCREEN2H, cfg_get_console_font());

	console<<"new icon"<<std::endl;
	icon = new Icon((SCREEN2W - 320) / 2, SCREEN2H - 56);

	console<<"new inventory"<<std::endl;
	inventory = new Inventory();

	console<<"new about"<<std::endl;
	about = new About();

	console<<"new editor"<<std::endl;
	editor = new Editor();

	console<<"new net"<<std::endl;
	net = new Net();

	console<<"new terrain_set"<<std::endl;
	terrain_set = new TerrainSet();

	mouse_callback = mouser_proc;
	//keyboard_callback = keyboard_proc;

	fade_out(FADE_SPEED);
	clear(screen);

	memset(&mapdata, 0, sizeof(mapdata));
	int fh = open(F("$(home)/geodata.dat"), O_RDONLY | O_BINARY);
	if (fh != -1) {
		read(fh, &mapdata, sizeof(mapdata));
		close(fh);
	}
	if (fh == -1 || !Map::valid_GEODATA(&mapdata))
		Map::new_GEODATA(&mapdata);

	delete print_win;
}

void closemain()
{
	saveini();
	/*	for(int i=0;i<obdata_num;i++) {
			delete []obdata[i];
		}
		delete []obdata;*/
	//closenet();

	delete terrain_set;
	delete net;
	delete editor;
	delete about;
	delete inventory;
	delete icon;
	delete g_console;

	Item::freebigobs();
	Map::freepck();
	Soldier::freepck();

	soundSystem::getInstance()->shutdown();
	closevideo();

	allegro_exit();
	
	lua_close(L);

	char revision[64];
	if (strcmp(UFO_SVNVERSION, "unknown") == 0 || strcmp(UFO_SVNVERSION, "exported") == 0 || strcmp(UFO_SVNVERSION, "") == 0) {
		sprintf(revision, "%s (revision >=%d)", UFO_VERSION_STRING, UFO_REVISION_NUMBER);
	} else {
		sprintf(revision, "%s.%s", UFO_VERSION_STRING, UFO_SVNVERSION);
	}

	std::cout<<"\nUFO2000 "
             <<revision
             <<"\nCompiled with "
             <<allegro_id << " on "
             <<__TIME__<<" "
             <<__DATE__<<"\n"
             <<"\nCopyright (C) 2000-2001  Alexander Ivanov aka Sanami"
             <<"\nCopyright (C) 2002-2003  ufo2000 development team"
             <<"\n\n"
             <<"http://ufo2000.sourceforge.net/\n"
             <<"http://ufo2000.lxnt.info/\n\n";
}

int build_crc()
{
	char buf[200000]; memset(buf, 0, sizeof(buf));
	int buf_size = 0;

	p1->eot_save(buf, buf_size);
	p2->eot_save(buf, buf_size);
	map->eot_save(buf, buf_size);

	int crc = crc16(buf);

	if (FLAGS & F_DEBUGDUMPS) {
		char filename[128];
		sprintf(filename, "$(home)/eot_save_%d.txt", crc);
		int fh = open(F(filename), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
		if (fh != -1) {
			buf_size = write(fh, buf, buf_size);
			close(fh);
		}
	}

	return crc;
}

/**
 * Check that no moves are performed on the map, so that it is save to end turn or
 * do something similar
 */
bool nomoves()
{
	return platoon_local->nomoves() && platoon_remote->nomoves();
}

/**
 * Function that calculates current gamestate crc and compares it with the 
 * passed in argument value
 */
void check_crc(int crc)
{
	int bcrc = build_crc();
	if (crc != bcrc) {
		g_console->printf("%s", "wrong wholeCRC");
		g_console->printf("crc=%d, bcrc=%d", crc, bcrc);
		net->send_debug_message("crc error");
	}
}

/**
 * Function that is called to make all the necessery changes to the gamestate
 * when changing active player (passing turn)
 */
void switch_turn()
{
	CONFIRM_REQUESTED = 0; // ???

	turn++;
	map->step();

//	Still did not test where this code would be better to put

	if (platoon_remote->captain() == NULL) // Win?
		win = 1;

	if (platoon_local->captain() == NULL) // Loss?
		loss = 1;
}

/**
 * This function is called when player wants to pass turn to the other player
 */
void send_turn()
{
	ASSERT(MODE != WATCH);
	switch_turn();
	elist->step(-1);
	int crc = build_crc();
	net->send_endturn(crc);

	platoon_remote->restore();

	if (net->gametype == HOTSEAT) {
		if (win || loss) {
		//	!!! Hack - to prevent unnecessery replay while in endgame screen
			closehotseatgame();			
			return;
		}
    
    //	Load the game state for the start of enemy turn and switch to WATCH mode
		icon->show_eot();
		loadgame(F("$(home)/ufo2000.tmp"));

		map->m_minimap_area->set_full_redraw();
		g_console->set_full_redraw();

		Platoon *pt = platoon_local;
		platoon_local = platoon_remote;
		platoon_remote = pt;

		sel_man = platoon_local->captain();
		if (sel_man != NULL) map->center(sel_man);

		platoon_local->set_visibility_changed();

		set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
		alert(" ", "  NEXT TURN  ", " ", "    OK    ", NULL, 1, 0);
	}

	g_time_left = 0;
	MODE = WATCH;
}

/**
 * This function is called when we receive turn from the other player
 */
void recv_turn(int crc)
{
	ASSERT(MODE == WATCH);
	switch_turn();
	elist->step(0);
	check_crc(crc);

	platoon_local->restore();
	platoon_local->set_visibility_changed();

	if (net->gametype == HOTSEAT) {
		savegame(F("$(home)/ufo2000.tmp"));
		map->m_minimap_area->set_full_redraw();
	}

	g_time_left = g_time_limit;
	MODE = MAP3D;

	g_console->printf(
		xcom1_color(192),
		"Next turn. local = %d, remote = %d soldiers",
		platoon_local->num_of_men(),
		platoon_remote->num_of_men());
}

int GAMELOOP = 0;

/** Redraw field of view and minimap on the screen
*/
void build_screen(int & select_y)
{
	clear_to_color(screen2, BACKCOLOR);

	switch (MODE) {
		case UNIT_INFO:
			if (sel_man != NULL)
				sel_man->draw_unibord(SCREEN2W / 2 - 160, SCREEN2H / 2 - 100);
			else
				MODE = MAP3D;
			break;
		case MAP2D:
			map->draw2d();
			break;
		case WATCH:
		case MAP3D:
			map->set_sel(mouse_x, mouse_y);
			map->draw();

			if (key[KEY_LCONTROL] && sel_man != NULL && !sel_man->ismoving()) {
				if (TARGET)
					sel_man->draw_bullet_way();
				else
					map->draw_path_from(sel_man);
			}

			p1->bulldraw();
			p2->bulldraw();

			platoon_remote->draw_blue_selectors();

			if (sel_man != NULL) {
				sel_man->draw_selector(select_y);
				sel_man->draw_enemy_seen(select_y);
			}

			icon->draw();

			if (MODE == WATCH)
				textprintf(screen2, font, 0, 0, xcom1_color(1), "%s", "WATCH");
			break;
		case MAN:
			if (sel_man != NULL) {
				inventory->draw();
			} else {
				MODE = MAP3D;
				//map->cell[map->sel_col][map->sel_row].place.put(icon->sel_item);
				//icon->sel_item = NULL;
			}
			break;
		default:
			ASSERT(false);
	}

	draw_sprite(screen2, mouser, mouse_x, mouse_y);
	blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
	map->svga2d();

	if (FLAGS & F_SHOWLOFCELL) {
		map->show_lof_cell();
	}
}

/**
 * Save game state to "ufo2000.sav" file
 */
void savegame(const char *filename)
{
	std::fstream f(filename, std::ios::binary | std::ios::out);
	savegame_stream(f);
}

void savegame_stream(std::iostream &stream)
{
	char sign[64];
	sprintf(sign, "ufo2000 %s (%s %s)\n", UFO_VERSION_STRING, __DATE__, __TIME__);
	stream.write(sign, strlen(sign) + 1);

	persist::Engine archive(stream, persist::Engine::modeWrite);

	PersistWriteBinary(archive, &turn, sizeof(turn));
	PersistWriteBinary(archive, &MODE, sizeof(MODE));
	PersistWriteBinary(archive, &pd1, sizeof(pd1));
	PersistWriteBinary(archive, &pd2, sizeof(pd2));
	PersistWriteBinary(archive, &mapdata, sizeof(mapdata));

	PersistWriteObject(archive, p1);
	PersistWriteObject(archive, p2);
	PersistWriteObject(archive, platoon_local);
	PersistWriteObject(archive, platoon_remote);
	PersistWriteObject(archive, map);
	PersistWriteObject(archive, sel_man);
	PersistWriteObject(archive, elist);
}

/**
 * Load game state from a savefile
 *
 * @param filename name of the file in which the game state is stored
 * @return         true on success, false on failure
 */
bool loadgame(const char *filename)
{
	std::fstream f(filename, std::ios::binary | std::ios::in);
	if (!f.is_open()) return false;

	return loadgame_stream(f);
}

bool loadgame_stream(std::iostream &stream)
{
	char sign[64], buff[64];
	sprintf(sign, "ufo2000 %s (%s %s)\n", UFO_VERSION_STRING, __DATE__, __TIME__);
	stream.read(buff, strlen(sign) + 1);
	if (strcmp(sign, buff) != 0) return false;

	persist::Engine archive(stream, persist::Engine::modeRead);

	PersistReadBinary(archive, &turn, sizeof(turn));
	PersistReadBinary(archive, &MODE, sizeof(MODE));
	PersistReadBinary(archive, &pd1, sizeof(pd1));
	PersistReadBinary(archive, &pd2, sizeof(pd2));
	PersistReadBinary(archive, &mapdata, sizeof(mapdata));

	delete p1;
	delete p2;
	delete map;
	delete elist;

	PersistReadObject(archive, p1);
	PersistReadObject(archive, p2);
	PersistReadObject(archive, platoon_local);
	PersistReadObject(archive, platoon_remote);
	PersistReadObject(archive, map);
	PersistReadObject(archive, sel_man);
	PersistReadObject(archive, elist);

	TARGET = 0;

	return true;
}

void endgame_stats()
{
	net->send_debug_message("result:%s", (win == loss) ? ("draw") : (win ? "victory" : "defeat"));

	BITMAP *back;
	BITMAP *scr = create_bitmap(320, 200);
	BITMAP *newscr = create_bitmap(SCREEN_W, SCREEN_H);
	clear(scr);
	clear(newscr);
	char winner[64];

	StatEntry *temp;
	Platoon *ptemp;

	if ((net->gametype == HOTSEAT) && (turn % 2 != 0))
	{
		ptemp = platoon_remote;
		platoon_remote = platoon_local; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		platoon_local = ptemp;
	}

	int local_kills      = platoon_local->get_stats()->total_kills();
	int remote_kills     = platoon_remote->get_stats()->total_kills();
	int local_dead       = platoon_local->get_stats()->total_dead();
	int remote_dead      = platoon_remote->get_stats()->total_dead();
	int local_inflicted  = platoon_local->get_stats()->total_damage_inflicted();
	int remote_inflicted = platoon_remote->get_stats()->total_damage_inflicted();
	int local_taken      = platoon_local->get_stats()->total_damage_taken();
	int remote_taken     = platoon_remote->get_stats()->total_damage_taken();

	int mvp_remote = 0, devastating_remote = 0, coward_remote = 0;
	StatEntry *mvp = platoon_local->get_stats()->get_most_kills();
	temp = platoon_remote->get_stats()->get_most_kills();
	if (temp->get_kills() > mvp->get_kills())
	{
		mvp_remote = 1;
		mvp = temp;
	}

	StatEntry *devastating = platoon_local->get_stats()->get_most_inflicted();
	temp = platoon_remote->get_stats()->get_most_inflicted();
	if (temp->get_inflicted() > mvp->get_inflicted())
	{
		devastating_remote = 1;
		devastating = temp;
	}

	StatEntry *coward = platoon_local->get_stats()->get_least_inflicted();
	temp = platoon_remote->get_stats()->get_least_inflicted();
	if (temp->get_inflicted() < mvp->get_inflicted())
	{
		coward_remote = 0;
		coward = temp;
	}

	if ((net->gametype == HOTSEAT) && (turn % 2 != 0))
	{
		ptemp = platoon_remote;
		platoon_remote = platoon_local; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		platoon_local = ptemp;
	}


	fade_out(10);
	clear(screen);

	reset_video();

	DONE = 0;

	BITMAP *back_win = load_back_image(cfg_get_win_image_file_name());
	BITMAP *back_lose = load_back_image(cfg_get_lose_image_file_name());

	if (net->gametype == HOTSEAT)
 	{
		if (win == loss)
 		{
			play_midi(g_lose_midi_music, 1);
			back = back_lose;
			strcpy(winner, "DRAW!");
 		}
 		else
 		{
			play_midi(g_win_midi_music, 1);
			back = back_win;
 			if (win)
 			{
				if (turn % 2 == 0) strcpy(winner, "PLAYER 2 WINS!");
				else strcpy(winner, "PLAYER 1 WINS!");
 			}
 			else
 			{
				if (turn % 2 == 0) strcpy(winner, "PLAYER 1 WINS!");
				else strcpy(winner, "PLAYER 2 WINS!");
 			}
 		}
 	}
 	else
 	{
		// Only go to the top case if it's an exclusive win.
		if ((win && (!loss)))
		{
			play_midi(g_win_midi_music, 1);
			back = back_win;
			strcpy(winner, "YOU WIN!");
		}
		else
		{
			play_midi(g_lose_midi_music, 1);
			back = back_lose;
			if (!win) strcpy(winner, "YOU LOSE!");
			else strcpy(winner, "DRAW!");
		}
 	}

	stretch_blit(back, newscr, 0, 0, back->w, back->h, 0, 0, SCREEN_W, SCREEN_H);

	textprintf_centre(newscr, large, 320, 24, xcom1_color(1), "%s", winner);

	if (net->gametype == HOTSEAT)
	{
		textprintf(newscr, g_small_font,   8, 60, xcom1_color(1), "Player 1");
		textprintf(newscr, g_small_font, 328, 60, xcom1_color(1), "Player 2");
	}
	else
	{
		textprintf(newscr, g_small_font,   8, 60, xcom1_color(1), "Your Platoon");
		textprintf(newscr, g_small_font, 328, 60, xcom1_color(1), "Remote Platoon");
	}
	textprintf(newscr, g_small_font,  16, 68, xcom1_color(1), "Total Kills: %d", local_kills);
	textprintf(newscr, g_small_font, 336, 68, xcom1_color(1), "Total Kills: %d", remote_kills);
	textprintf(newscr, g_small_font,  16, 76, xcom1_color(1), "Deaths: %d", local_dead);
	textprintf(newscr, g_small_font, 336, 76, xcom1_color(1), "Deaths: %d", remote_dead);
	textprintf(newscr, g_small_font,  16, 84, xcom1_color(1), "Total Damage Inflicted: %d", local_inflicted);
	textprintf(newscr, g_small_font, 336, 84, xcom1_color(1), "Total Damage Inflicted: %d", remote_inflicted);
	textprintf(newscr, g_small_font,  16, 92, xcom1_color(1), "Total Damage Taken: %d", local_taken);
	textprintf(newscr, g_small_font, 336, 92, xcom1_color(1), "Total Damage Taken: %d", remote_taken);

	textprintf_centre(newscr, large, 320, 108, xcom1_color(1), "Most Valuable Soldier:");
	textprintf_centre(newscr, g_small_font, 320, 124, xcom1_color(1), "%s (%s, %d kills)",
		mvp->get_name(), 
		(mvp_remote) ? ((net->gametype == HOTSEAT) ? "Player 2" : "Remote") : ((net->gametype == HOTSEAT) ? "Player 1" : "Local"),
		mvp->get_kills());

	textprintf_centre(newscr, large, 320, 140, xcom1_color(1), "Most Devastating Soldier:");
	textprintf_centre(newscr, g_small_font, 320, 156, xcom1_color(1), "%s (%s, %d damage inflicted)",
		devastating->get_name(), 
		(devastating_remote) ? ((net->gametype == HOTSEAT) ? "Player 2" : "Remote") : ((net->gametype == HOTSEAT) ? "Player 1" : "Local"),
		devastating->get_inflicted());

	textprintf_centre(newscr, large, 320, 172, xcom1_color(1), "Most Cowardly Soldier:");
	textprintf_centre(newscr, g_small_font, 320, 188, xcom1_color(1), "%s (%s, %d damage inflicted)",
		coward->get_name(), 
		(coward_remote) ? ((net->gametype == HOTSEAT) ? "Player 2" : "Remote") : ((net->gametype == HOTSEAT) ? "Player 1" : "Local"),
		coward->get_inflicted());

	g_console->set_full_redraw();

	MODE = MAP3D; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	CHANGE = 1;

	while (!DONE)
	{
		net->check();

		if (CHANGE) {
			blit(newscr, screen, 0, 0, 0, 0, SCREEN_W, SCREEN2H);
			CHANGE = 0;
		}

		g_console->redraw(screen, 0, SCREEN2H);

		process_keyswitch();

		if (keypressed()) {
			int scancode;
			int keycode = ureadkey(&scancode);

			switch (scancode) {
				case KEY_ESC:
					if (askmenu("EXIT GAME"))
						DONE = 1;
					break;
				case KEY_UP:
					resize_screen2(0, -10);
					break;
				case KEY_DOWN:
					resize_screen2(0, 10);
					break;
				case KEY_F9:
					keyswitch(0);
					break;
				case KEY_F10:
					change_screen_mode();
					break;
				default:
					if (g_console->process_keyboard_input(keycode, scancode))
						net->send_message((char *)g_console->get_text());
			}
			CHANGE = 1;
		}
	}
	play_midi(NULL, 0);
}

#ifdef WIN32
static LPTOP_LEVEL_EXCEPTION_FILTER prevExceptionFilter = NULL;

static LONG WINAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	net->send_debug_message("crash");

	if (prevExceptionFilter)
		return prevExceptionFilter(pExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}
#endif

/**
 * Main loop of the tactical part of the game
 */
void gameloop()
{
#ifdef WIN32
	prevExceptionFilter = SetUnhandledExceptionFilter(TopLevelExceptionFilter);
#endif

	int mouse_leftr = 1, mouse_rightr = 1, select_y = 0;

	play_midi(g_combat_midi_music, 1);

	clear_keybuf();
	GAMELOOP = 1;

	platoon_local->set_visibility_changed();
	platoon_remote->set_visibility_changed();

	if (MODE != WATCH) {
		g_time_left = g_time_limit;
	} else {
		g_time_left = 0;
	}

	if (net->gametype == HOTSEAT)
		savegame(F("$(home)/ufo2000.tmp"));

	while (!DONE) {

		if (MODE != WATCH && g_time_left == 0) {
			TARGET = 0;
			if (nomoves()) {
				send_turn();
			}
		}

		g_console->redraw(screen, 0, SCREEN2H);

		net->check();

		if (win || loss) break;

		if (NOTICE) {
			if (NOTICEdemon) {
				net->send_notice();
//				g_console->printf("%d", NOTICEremote);
			}
			NOTICEremote++;
			NOTICE = 0;
		}

		if (CHANGE) {
			FLYIT = 0;
			MOVEIT = 0;
		}

		while (FLYIT > 0) {
			platoon_local->bullmove();     //!!!! bull of dead?
			platoon_remote->bullmove();

			FLYIT--;
		}

		while (MOVEIT > 0) {
			if (FLAGS & F_CLEARSEEN)
				map->clearseen();

			map->smoker();
			platoon_remote->move(0);
			platoon_local->move(1);      //!!sel_man may die

			select_y = (select_y + 1) % 3;
			CHANGE = 1;

			MOVEIT--;
		}

		if (CHANGE) {
			build_screen(select_y);
			CHANGE = 0;
		}

		if (MAPSCROLL) {
			if ((MODE == MAP3D) || (MODE == WATCH)) {
				if (map->scroll(mouse_x, mouse_y)) CHANGE = 1;
			}
			MAPSCROLL = 0;
		}

		if ((mouse_b & 1) && (mouse_leftr)) { //left
			mouse_leftr = 0;
			switch (MODE) {
				case UNIT_INFO:
					MODE = MAP3D;
					break;
				case WATCH:
					break;
				case MAP2D:
					if (map->center2d(mouse_x, mouse_y))
						MODE = MAP3D;
					break;
				case MAP3D:
					// Center to one of seen enemies if appropriate bar with a digit 
					// in the right bottom corner was clicked
					if (sel_man != NULL && sel_man->center_enemy_seen()) break;

                    // Wait for opponent to finish reaction fire (avoid dodging 
                    // reaction fire bullets)
					if (!platoon_remote->nomoves()) break;

                    // Handle buttons the control panel
					if (icon->inside(mouse_x, mouse_y)) {
						icon->execute(mouse_x, mouse_y);
						break;
					}

                    // Try to shoot if currently in targeting mode
					if (TARGET) {
						if (sel_man != NULL) sel_man->try_shoot();
						break;
					} 
					
					if (sel_man != NULL) {
						Soldier *ssman = map->sel_man();
						if (ssman == NULL) {
							if (!sel_man->ismoving()) {
								sel_man->wayto(map->sel_lev, map->sel_col, map->sel_row);
							}
						} else {
							if (!platoon_local->belong(ssman)) {
								if (!sel_man->ismoving()) {
									sel_man->wayto(map->sel_lev, map->sel_col, map->sel_row);
								}
							}

							if (!sel_man->ismoving()) {
								if (platoon_local->belong(ssman))
									sel_man = ssman;
							}
							if (FLAGS & F_SEL_ANY_MAN)
								sel_man = ssman;
						}
					} else {
						Soldier *ss = map->sel_man();
						if ((ss != NULL) && (platoon_local->belong(ss)))
							sel_man = ss;
						//net_send("_sel_man");
						if (FLAGS & F_SEL_ANY_MAN) {
							if (ss != NULL)
								sel_man = ss;
						}
					}
					break;
				case MAN:
					inventory->execute();
					break;
				default:
					ASSERT(false);
			}
		}

		if ((mouse_b & 2) && (mouse_rightr)) { //right
			mouse_rightr = 0;
			switch (MODE) {
				case UNIT_INFO:
					MODE = MAP3D;
					break;
				case WATCH:
					break;
				case MAP2D:
					map->center2d(mouse_x, mouse_y);
					MODE = MAP3D;
					break;
				case MAP3D:
					if (TARGET)
						TARGET = 0;
					else {
						if (sel_man != NULL) {
							if (!sel_man->ismoving()) {
								sel_man->faceto(map->sel_col, map->sel_row);

								//if (sel_man->curway == -1)
								//	sel_man->open_door();
								if (!sel_man->ismoving())
									sel_man->open_door();
							} else if (sel_man->is_marching()) {
								//sel_man->finish_march();
								sel_man->break_march();
								//sel_man->waylen = sel_man->curway - 1;
							}
						}
					}
					break;
				case MAN:
					inventory->close();
					break;
				default:
					ASSERT(false);
			}
		}

		if (!(mouse_b & 1)) {
			mouse_leftr = 1;
			//			CHANGE = 1;
		}

		if (!(mouse_b & 2)) {
			mouse_rightr = 1;
			//			CHANGE = 1;
		}

		process_keyswitch();

		if (keypressed()) {
			int scancode;
			int keycode = ureadkey(&scancode);

			switch (scancode) {
				case KEY_PGUP:
					if (map->sel_lev < map->level - 1) {
						map->sel_lev++;
						//mouse_y -= 24;
						position_mouse(mouse_x, mouse_y - 24);
					}
					break;
				case KEY_PGDN:
					if (map->sel_lev > 0) {
						map->sel_lev--;
						//mouse_y += 24;
						position_mouse(mouse_x, mouse_y + 24);
					}
					break;
				case KEY_PLUS_PAD:
					resize_screen2(10, 10);
					break;
				case KEY_MINUS_PAD:
					resize_screen2(-10, -10);
					break;
				case KEY_LEFT:
					resize_screen2(-10, 0);
					//map->move(MSCROLL*2,0);
					break;
				case KEY_UP:
					resize_screen2(0, -10);
					//map->move(0,MSCROLL*2);
					break;
				case KEY_RIGHT:
					resize_screen2(10, 0);
					//map->move(-MSCROLL*2,0);
					break;
				case KEY_DOWN:
					resize_screen2(0, 10);
					//map->move(0,-MSCROLL*2);
					break;
				case KEY_F1:
					if (FLAGS & F_RAWMESSAGES) {
						FLAGS &= ~F_RAWMESSAGES;
					} else {
						FLAGS |= F_RAWMESSAGES;
					}
					break;
				case KEY_F2:
					if (askmenu("SAVE GAME"))
						savegame(F("$(home)/ufo2000.sav"));
					break;
				case KEY_F3:
					if (askmenu("LOAD GAME")) {
						if (!loadgame(F("$(home)/ufo2000.sav"))) {
							alert("saved game not found", "", "", "OK", NULL, 0, 0);
						}
						inithotseatgame();
						if (net->gametype == HOTSEAT)
							savegame(F("$(home)/ufo2000.tmp"));
					}
					break;
				case KEY_F5:
					if (askmenu("RESTART GAME")) {
						//restartgame();
						net->send_restart();
						//position_mouse(160, 100);
					}
					break;
				case KEY_F9:
					keyswitch(0);
					break;
				case KEY_F10:
					change_screen_mode();
					break;
				case KEY_F11:
					if (NOTICEdemon) {
						if (askmenu("STOP NOTIFY"))
							NOTICEdemon = 0;
					} else {
						if (askmenu("START NOTIFY")) {
							NOTICEdemon = 1;
							NOTICEremote = 0;
						}
					}
					break;
				case KEY_F12:
					if (askmenu("SNAPSHOT")) {
						savescreen();
					}
					break;
				case KEY_ESC:
					if (askmenu("EXIT GAME"))
						DONE = 1;
					break;
				default:
					if (g_console->process_keyboard_input(keycode, scancode))
						net->send_message((char *)g_console->get_text());
			}
			CHANGE = 1;
		}
	}

	play_midi(NULL, 0);

	GAMELOOP = 0;

	if (win || loss)
	{
		endgame_stats();
	}

	fade_out(10);

	net->send_quit();

	clear(screen);
#ifdef WIN32
	SetUnhandledExceptionFilter(prevExceptionFilter);
#endif
}

void faststart()
{
	HOST = sethotseatplay();

	//initgame();
	install_timers(speed_unit, speed_bullet, speed_mapscroll);

	reset_video();

	//restartgame();

	int fh = open(F("$(home)/cur_map.dat"), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	read(fh, &mapdata, sizeof(mapdata));
	close(fh);

	fh = open(F("$(home)/cur_p1.dat"), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	read(fh, &pd1, sizeof(pd1));
	close(fh);

	fh = open(F("$(home)/cur_p2.dat"), O_RDONLY | O_BINARY);
	ASSERT(fh != -1);
	read(fh, &pd2, sizeof(pd2));
	close(fh);

	map = new Map(mapdata);
	elist = new Explosive();
	p1 = new Platoon(1111, &pd1);
	p2 = new Platoon(2222, &pd2);

	//map->place(0, 0, 0)->put(new Item(KASTET));
	//map->place(0, 0, 0)->put(new Item(KNIFE));

	elist->reset();
	if (HOST) {
		platoon_local = p1;
		platoon_remote = p2;
		MODE = MAP3D;
	} else {
		platoon_local = p2;
		platoon_remote = p1;
		MODE = WATCH;
	}

	sel_man = platoon_local->captain();
	if (sel_man != NULL) map->center(sel_man);
	DONE = 0; TARGET = 0; turn = 0;

	resize_screen2(0, 0);
	clear_to_color(screen, 58);      //!!!!!
	gameloop();
	closegame();
}

void start_loadgame()
{
	HOST = sethotseatplay();

	install_timers(speed_unit, speed_bullet, speed_mapscroll);

	reset_video();

   	if (!loadgame(F("$(home)/ufo2000.sav")))
   	{
   		alert("saved game not found", "", "", "OK", NULL, 0, 0);
   		return;
   	}
	inithotseatgame();

	map->center(sel_man);
	DONE = 0;

	resize_screen2(0, 0);
	clear_to_color(screen, 58);      //!!!!!
	gameloop();
	closegame();
}

int main(int argc, char *argv[])
{
	initmain(argc, argv);

	if (FLAGS & F_FASTSTART) {
		faststart();
	} else {
        int mm = 2, h = -1;
        while ((mm = do_mainmenu()) != MAINMENU_QUIT) {
            h = -1;
            switch (mm) {
                case MAINMENU_ABOUT:
                    about->show();
                    continue;
                case MAINMENU_EDITOR:
					play_midi(g_editor_midi_music, 1);
//                  editor->do_mapedit();
					set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);
					gui_fg_color = xcom1_color(15);
					gui_bg_color = xcom1_color(1);

					alert(" ", "Map editor is currently disabled", " ", "    OK    ", NULL, 1, 0);
					play_midi(NULL, 0);
                    continue;
                case MAINMENU_HOTSEAT:
                    h = sethotseatplay();
                    break;
/*
                case MAINMENU_TCPIP:
                    h = setsocketplay();
                    break;
*/
                case MAINMENU_INTERNET:
                    h = connect_internet_server();
                    break;
                case MAINMENU_LOADGAME:
                    start_loadgame();
                    break;
                default:
                    continue;
            }
            if (h == -1)
                continue;
            HOST = h;

            if (initgame()) {
                gameloop();
                closegame();
            }
        }
    }

	closemain();
	return 0;
}
END_OF_MAIN();
