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

/**
 * Main program:  Ufo2000-client
 */


#include "stdafx.h"

#include "global.h"

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
#include "video.h"
#include "keys.h"
#include "crc32.h"
#include "music.h"
#include "scenario.h"
#include "colors.h"
#include "text.h"
#include "random.h"
#include "stats.h"

#include "sysworkarounds.h"

//#define DEBUG
#define MSCROLL 10
#define BACKCOLOR COLOR_BLACK1

Target target;
int HOST, DONE, TARGET, turn;
Mode MODE;                      //!< Display-Mode
ConsoleWindow *g_console;

int g_time_limit;               //!< Limit of time for a single turn in seconds
volatile int g_time_left;       //!< Current counter for time left for this turn
int last_time_left;             //!< Time of last screen-update for g_time_left
int g_p2_start_sit=0;           //!< If player 2 starts sitting - 0 by default
int g_tie;                      //   Flags denoting which players accepted draw
int g_random_init[2];           //   For initializing Random

int GameErrorColour[-ERR_MINUS_NUM];
const char GameErrorMessage[-ERR_MINUS_NUM][STDBUFSIZE] = {
    "Success.",
    "Not Enough Time Units!",
    "Not Enough Energy!",
    "Out Of Ammo!",
    "No Item To Use!",
    "Out Of Range!"
};

//ReserveTime_Mode ReserveTimeMode;     // TODO: should be platoon- or soldier-specific

/**
 * Update display of time remaining (for games with time-limit),
 * plus Warning-sounds when time is running out.
 */
// see also: show_time_left() in minimap.h
void show_time_left()
{
    int time_left = g_time_left;

    // Todo: check if minimap is visible, so show time only once on screen:
    textprintf(screen2, font, 0, 0, COLOR_WHITE, _("Time left: %d"), time_left);

    if (last_time_left == time_left)    // Play sounds only once per second
        return;

    // Possible sounds: SS_WINDOW_OPEN_1, SS_WINDOW_OPEN_2, SS_CLIP_LOAD, SS_ITEM_PUT
    if ((time_left == 10))  
        soundSystem::getInstance()->play(SS_WINDOW_OPEN_2); 
    if ((time_left <=  5))
        soundSystem::getInstance()->play(SS_ITEM_PUT); 

    last_time_left = time_left;
}

Net *net;
Map *map;
Scenario *scenario;
TerrainSet *terrain_set;
Icon *icon;
Inventory *inventory;
Editor *editor;
Platoon *p1, *p2;
Platoon *platoon_local, *platoon_remote;
Soldier *sel_man = NULL;
Explosive *elist;
Random *cur_random;

volatile unsigned int ANIMATION = 0;
volatile int CHANGE = 1;
volatile int MOVEIT = 0;
volatile int FLYIT = 0;
volatile int NOTICE = 1;
volatile int MAPSCROLL = 1;
volatile int REPLAYIT = 0;
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

void timer_replay()
{
	REPLAYIT++;
}
END_OF_FUNCTION(timer_replay);

int speed_unit      = 15;
int speed_bullet    = 30;
int speed_mapscroll = 30;
int mapscroll       = 10;
int replaydelay		= 2;

void install_timers(int _speed_unit, int _speed_bullet, int _speed_mapscroll)
{
    install_int_ex(timer_handler, BPS_TO_TIMER(_speed_unit));     //ticks each second
    install_int_ex(timer_handler2, BPS_TO_TIMER(_speed_bullet * 2));     //ticks each second
    install_int_ex(timer_handler4, BPS_TO_TIMER(_speed_mapscroll));     //ticks each second
    install_int_ex(timer_1s, BPS_TO_TIMER(1));     //ticks each second
    install_int_ex(timer_replay, BPS_TO_TIMER(1));
}

void uninstall_timers()
{
    remove_int(timer_handler4);
    remove_int(timer_1s);
    remove_int(timer_handler2);
    remove_int(timer_handler);
    remove_int(timer_replay);
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

/**
 * Generate a pseudo-random number based on local time.
 * Use for initializing some more randomizing number generator.
 */
int getsomerand()
{
    time_t now = time(NULL);
    struct tm * time_now = localtime(&now);
    return (time_now->tm_sec * 60 + time_now->tm_min); // shuffled on purpose, to make randomness better
}

/**
 * Initialize synchronized random number generator
 */
void initrand()
{
    Mode old_MODE = MODE;
    MODE = WATCH;
    g_random_init[0] = getsomerand();
    g_random_init[1] = -1;
    net->send_initrand(g_random_init[0]);
    do {
        rest(1);
        net->check();
    } while (g_random_init[1] == -1);
    cur_random->init(g_random_init[HOST] * 3600 + g_random_init[!HOST]);
    MODE = old_MODE;
}

/**
 * Initialize game state
 */ 
//simple all of new - rem about rand in sol's constructors
//can diff on remote comps
void restartgame()
{
    win  = 0;
    loss = 0;

    map = new Map(mapdata);
    p1  = new Platoon(1000, &pd1, scenario->deploy_type[0]);
    p2  = new Platoon(2000, &pd2, scenario->deploy_type[1]);
    cur_random = new Random;

    bool map_saved = Map::save_GEODATA("$(home)/cur_map.lua", &mapdata);
    ASSERT(map_saved);

    int fh = open(F("$(home)/cur_p1.dat"), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
    ASSERT(fh != -1);
    write(fh, &pd1, sizeof(pd1));
    close(fh);

    fh = open(F("$(home)/cur_p2.dat"), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
    ASSERT(fh != -1);
    write(fh, &pd2, sizeof(pd2));
    close(fh);

    elist = new Explosive();
    elist->reset();
    if (HOST) {
        platoon_local  = p1;
        platoon_remote = p2;
        MODE = MAP3D;
    } else {
        platoon_local  = p2;
        platoon_remote = p1;
        MODE = WATCH;
    }

    // Initial statistics get set properly here (strength effect on TU, mostly).
    platoon_local->restore();
    platoon_remote->restore();

    //deal with initial sit for p2
    if(!HOST || (net->gametype == GAME_TYPE_HOTSEAT))
    {
        if (FLAGS & F_SECONDSIT) p2->sit_on_start();
    }
    else {  
        if(g_p2_start_sit) p2->sit_on_start(); //!HOTSEAT HOST - recieves g_p2_start_sit in planner (connect.cpp)
    }

    //sel_man = NULL;
    sel_man = platoon_local->captain();
    if (sel_man != NULL) map->center(sel_man);
    DONE = 0; TARGET = 0; turn = 0;
}

/**
 * Initialize video, timers etc. before game
 */ 
int initgame()
{
    FS_MusicPlay(F(cfg_get_setup_music_file_name()));
    if (!net->init())
    {
        FS_MusicPlay(NULL);
        return 0;
    }
    FS_MusicPlay(NULL);

    install_timers(speed_unit, speed_bullet, speed_mapscroll);
    //mouse_callback = mouser_proc;

    reset_video();
    restartgame();
    g_tie = 0; // Clear tie flags for the new game.
    initrand();
    //clear_to_color(screen, 58); //!!!!!
    
    // Set/Clear some scenario specific variables and effects:
    scenario->start();

    return 1;
}

/**
 * Cleanup after game
 */ 
void closegame()
{
    delete cur_random;
    cur_random = NULL;
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
    lua_message( "Done : closegame" );
}

int print_y = 0;
Wind *print_win = NULL;

std::string indent(const std::string &str)
{
	bool newline = true;
	std::string tmp;
	for (int i = 0; i < (int)str.size(); i++) {
		if (newline) {
			tmp += "\t";
			newline = false;
		}
		if (str[i] == '\n') {
			newline = true;
		}
		tmp += str[i];
	}
	return tmp;
}

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
                print_win->printstr(curline.c_str(), COLOR_WHITE);
            } else {
                curline.append("\n");
                text_mode( -1);
                textout(screen, font, curline.c_str(), 0, print_y, COLOR_WHITE);
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
 * Fatal errors: display error message, exit program
 */
void display_error_message(const std::string &error_text)
{
#ifdef WIN32
    // show errormessage in windows-messagebox:
    MessageBox(NULL, error_text.c_str(), "UFO2000 Error!", MB_OK);  // don't translate !
#else
    fprintf(stderr, "\n%s\n", error_text.c_str());
#endif
    lua_message( std::string("!! Error: ") + error_text.c_str() );
    exit(1);
}

int file_select_mr(const char *message, char *path, const char *ext)
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    return file_select_ex(message, path, ext, 512, SCREEN_W / 2, SCREEN_H / 2);
}

static int assert_handler(const char *msg)
{
    if (net) net->send_debug_message("assert:%s", msg);
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

/**
 * Resolve file name and get absolute path (it can have one of the following 
 * prefixes: '$(xcom)', '$(tftd)', '$(ufo2000)', '$(home)')
 */
const char *F(const char *fileid)
{
    static std::string fname;
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "GetDataFileName");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (!lua_isfunction(L, -1))
        display_error_message( "Fatal: no 'GetDataFileName' function registered"); // don't translate !
    lua_pushstring(L, fileid);
    lua_safe_call(L, 1, 1);
    if (!lua_isstring(L, -1))
        display_error_message(std::string("Can't find file name by ") + fileid + " identifier");
    fname = lua_tostring(L, -1);
    lua_settop(L, stack_top);

    return fname.c_str();
}

std::string lua_escape_string(const std::string &str)
{
    std::string out = "";
    for (int i = 0; i < (int)str.size(); i++) {
        switch (str[i]) {
            case '\\': out += "\\\\"; break;
            case '\'': out += "\\\'"; break;
            case '\"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            default: out.append(&str[i], 1); break;
        }
    }
    return out;
}

void find_lua_files_callback(const char *filename, int attrib, int param)
{
    lua_safe_dofile(L, filename, "plugins_sandbox");
    // $$$ Fixme: lua_dofile sets errno variable in some mysterious way,
    // so allegro for_each_file function stops searching files if we do not 
    // reset this back to 0
    *allegro_errno = 0; 
}

/**
 * lua_call replacement with error message showing on errors
 */
int lua_safe_call(lua_State *L, int narg, int nret)
{
    int status;
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushliteral(L, "_TRACEBACK");
    lua_rawget(L, LUA_GLOBALSINDEX);  /* get traceback function */
    lua_insert(L, base);  /* put it under chunk and args */
    status = lua_pcall(L, narg, nret, base);
    lua_remove(L, base);  /* remove traceback function */
    if (status) {
        display_error_message(lua_tostring(L, -1));
    }
    return status;
}

/**
 * lua_dofile replacement with error message showing on errors
 */
int lua_safe_dofile(lua_State *L, const char *name, const char *env_name)
{
    int status = luaL_loadfile(L, name);
    if (status) display_error_message(lua_tostring(L, -1));
    if (env_name != NULL) {
        lua_pushstring(L, env_name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        ASSERT(lua_istable(L, -1));
        lua_setfenv(L, -2);
    }
    status = lua_safe_call(L, 0, LUA_MULTRET);
    return status;
}

/**
 * lua_dobuffer replacement with error message showing on errors
 */
int lua_safe_dobuffer(lua_State *L, const char *buff, size_t size, const char *name)
{
    int status = luaL_loadbuffer(L, buff, size, name);
    if (status) display_error_message(lua_tostring(L, -1));
    status = lua_safe_call(L, 0, LUA_MULTRET);
    return status;
}

/**
 * lua_dostring replacement with error message showing on errors
 */
int lua_safe_dostring(lua_State *L, const char *str)
{
    return lua_safe_dobuffer(L, str, strlen(str), str);
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
    luaopen_base(L);
    luaopen_string(L);
    luaopen_io(L);
    luaopen_math(L);
    luaopen_table(L);
    luaopen_debug(L);
	
	// Lua API for accessing C++ objects
    LUA_REGISTER_CLASS(L, Platoon);
    LUA_REGISTER_CLASS_METHOD(L, Platoon, findnum);
	
    LUA_REGISTER_CLASS(L, Soldier);
    LUA_REGISTER_CLASS_METHOD(L, Soldier, reset_stats);
    LUA_REGISTER_CLASS_METHOD(L, Soldier, set_attribute);
    LUA_REGISTER_CLASS_METHOD(L, Soldier, set_name);
    LUA_REGISTER_CLASS_METHOD(L, Soldier, set_skin_info);
    LUA_REGISTER_CLASS_METHOD(L, Soldier, find_place);

    LUA_REGISTER_CLASS(L, Place);
    LUA_REGISTER_CLASS_METHOD(L, Place, add_item);
    LUA_REGISTER_CLASS_METHOD(L, Place, destroy_all_items);
	
    LUA_REGISTER_FUNCTION(L, pck_image);
    
#ifdef LINUX
    // Do not silently exit on broken network connection
    signal(SIGPIPE, SIG_IGN);
#endif

#ifndef DATA_DIR
    // Set current directory to the place where ufo2000 executable was started from
    // if no data directory location was specified
    char ufo2000_dir[512];
    get_executable_name(ufo2000_dir, sizeof(ufo2000_dir));
    char *p = get_filename(ufo2000_dir);
    ASSERT(p > ufo2000_dir);
    *(p - 1) = '\0';
#ifdef WIN32
    // Convert '\\' to '/' even in Windows to keep consistency
    // ('/' is used as path separator everywhere), DOS is dead,
    // so this will not cause any problems
    p = ufo2000_dir;
    while (*p != '\0') { if (*p == '\\') *p = '/'; p++; }
#endif  
    
    chdir(ufo2000_dir);
    
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

    // Initialize lua environment
    lua_safe_dofile(L, DATA_DIR "/init-scripts/main.lua");

    // Load standard and custom maps
    lua_safe_dofile(L, DATA_DIR "/init-scripts/standard-maps.lua", "plugins_sandbox");
    for_each_file(DATA_DIR "/newmaps/*.lua", FA_RDONLY | FA_ARCH, find_lua_files_callback, 0);

    FLAGS = 0;
    push_config_state();
    set_config_file(F("$(home)/ufo2000.ini"));
    
    install_keyboard();

    // initialize language settings
    lua_pushstring(L, "SetLanguage");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, get_config_string("System", "language", "en"));
        lua_safe_call(L, 1, 0);
    } else {
        lua_pop(L, 1);
    }
    
    if (get_config_int("Flags", "F_CLEARSEEN", 0)) FLAGS |= F_CLEARSEEN;      // clear seen every time
    if (get_config_int("Flags", "F_SHOWROUTE", 0)) FLAGS |= F_SHOWROUTE;      // show pathfinder matrix
    if (get_config_int("Flags", "F_SHOWLOFCELL", 0)) FLAGS |= F_SHOWLOFCELL;  // show cell's LOF & BOF
    if (get_config_int("Flags", "F_SHOWLEVELS", 0)) FLAGS |= F_SHOWLEVELS;    // show all level
    if (get_config_int("Flags", "F_FASTSTART", 0)) FLAGS |= F_FASTSTART;      // skip
    if (get_config_int("Flags", "F_FULLSCREEN",  0)) FLAGS |= F_FULLSCREEN;   // start in fullscreen mode
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
    if (get_config_int("Flags", "F_TOOLTIPS",    0)) FLAGS |= F_TOOLTIPS;     // Enable display of tooltips for the control-panel
    if (get_config_int("Flags", "F_ENDTURNSND", 1)) FLAGS |= F_ENDTURNSND;    // sound signal at the end of turn
    if (get_config_int("Flags", "F_SECONDSIT", 1)) FLAGS |= F_SECONDSIT;      // second player starts in SIT position
    if (get_config_int("Flags", "F_REACTINFO", 0)) FLAGS |= F_REACTINFO;      // show debug info on reaction fire

    const AGUP_THEME *gui_theme = NULL;

    gui_theme = agup_load_bitmap_theme(F(get_config_string("General", "gui_theme", "")), NULL);
    if (!gui_theme) gui_theme = agup_theme_by_name(get_config_string("General", "gui_theme", "BeOS"));

    if (argc > 1) {
        g_server_login = argv[1];
        g_server_password = argc > 2 ? argv[2] : "";
    } else {
        g_server_login = get_config_string("Server", "login", "anonymous");
        g_server_password = get_config_string("Server", "password", "");
    }

    loadini();
    pop_config_state();

    datafile = load_datafile("#");   // contains palette-info, and graphics (mouse-pointer...)
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
    print_win = new Wind(text_back, 15, 300, 625, 390, COLOR_BLACK2);

    /* to use the init console as an ostream -very handy. */
    consoleBuf consbuf(FLAGS & F_LOGTOSTDOUT);
    std::ostream console(&consbuf);

    console<<"allegro_init"<<std::endl;

    console<<"agup_init"<<std::endl;
    if (gui_theme == NULL) gui_theme = abeos_theme;
    agup_init(gui_theme);
    gui_shadow_box_proc = d_agup_shadow_box_proc;
    gui_ctext_proc = d_agup_ctext_proc;
    gui_button_proc = d_agup_button_proc;
    gui_edit_proc = d_agup_edit_proc;
    gui_list_proc = d_agup_list_proc;
    gui_text_list_proc = d_agup_text_list_proc;

    lua_safe_dofile(L, DATA_DIR "/init-scripts/standard-items.lua");
    lua_safe_dofile(L, DATA_DIR "/init-scripts/standard-equipment.lua");
    lua_safe_dofile(L, DATA_DIR "/init-scripts/standard-soldiersetup.lua");

    console<<"install_timer"<<std::endl;
    install_timer();
    console<<"install_mouse"<<std::endl;
    install_mouse();

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

    FS_MusicInit();
    FS_SetMusicVolume(cfg_get_music_volume());

    LOCK_VARIABLE(CHANGE); LOCK_FUNCTION(mouser_proc);
    LOCK_VARIABLE(MOVEIT); LOCK_VARIABLE(ANIMATION); LOCK_FUNCTION(timer_handler);
    LOCK_VARIABLE(FLYIT); LOCK_FUNCTION(timer_handler2);
    LOCK_VARIABLE(NOTICE); LOCK_VARIABLE(g_time_left); LOCK_FUNCTION(timer_1s);
    LOCK_VARIABLE(MAPSCROLL); LOCK_FUNCTION(timer_handler4);
    LOCK_VARIABLE(REPLAYIT); LOCK_FUNCTION(timer_replay);

    LOCK_FUNCTION(keyboard_proc);

    console<<"initpck units"<<std::endl;
    Soldier::initpck();

    console<<"initpck terrain"<<std::endl;
    Map::initpck();

    console<<"new console window"<<std::endl;
    g_console = new ConsoleWindow(SCREEN_W, SCREEN_H - SCREEN2H);

    console<<"new icon"<<std::endl;
    icon = new Icon();

    console<<"new inventory"<<std::endl;
    inventory = new Inventory();

    console<<"new editor"<<std::endl;
    editor = new Editor();

    console<<"new net"<<std::endl;
    net = new Net();
    
    console<<"new terrain_set"<<std::endl;
    terrain_set = new TerrainSet();
    
    console<<"new scenario"<<std::endl;
    scenario = new Scenario(SC_DEATHMATCH);

    console<<"init_place_names"<<std::endl;
    Init_place_names();

    mouse_callback = mouser_proc;
    //keyboard_callback = keyboard_proc;

    fade_out(FADE_SPEED);
    clear(screen);

    if (!exists(F("$(home)/cur_map.lua")) || 
            !Map::load_GEODATA("$(home)/cur_map.lua", &mapdata) || 
            !Map::valid_GEODATA(&mapdata)) {
            
        Map::new_GEODATA(&mapdata); 
    }

    // Error codes are negative.
    GameErrorColour[-OK]             = COLOR_SYS_OK;
    GameErrorColour[-ERR_NO_TUS]     = COLOR_ORANGE;
    GameErrorColour[-ERR_NO_ENERGY]  = COLOR_ORANGE;
    GameErrorColour[-ERR_NO_AMMO]    = COLOR_ORANGE;
    GameErrorColour[-ERR_NO_ITEM]    = COLOR_SYS_FAIL;
    GameErrorColour[-ERR_DISTANCE]   = COLOR_YELLOW;

    delete print_win;
}

void closemain()
{
    saveini();

    delete terrain_set;
    delete net;
    delete editor;
    delete inventory;
    delete icon;
    delete g_console;
    net = NULL;

    free_pck_cache();

    Map::freepck();
    Soldier::freepck();

    FS_MusicClose();

    soundSystem::getInstance()->shutdown();
    closevideo();

    agup_shutdown();
    allegro_exit();
    
    lua_close(L);

    std::cout<<"\nUFO2000 "
             <<g_version_id.c_str()
             <<"\nCompiled with "
             <<allegro_id << " on "
             <<__TIME__<<" "
             <<__DATE__<<"\n"
             <<"\nCopyright (C) 2000-2001  Alexander Ivanov aka Sanami"
             <<"\nCopyright (C) 2002-2004  ufo2000 development team"
             <<"\n\n"
             <<"http://ufo2000.sourceforge.net/\n"
             <<"http://ufo2000.lxnt.info/\n\n";
}


#undef map
/**
 * Buffer that stores gamestate information indexed by gamestate crc.
 * It is used for debugging network synchronization bugs when gamestate
 * becomes different on local and remote computers.
 */
static std::map<int, std::string> g_eot_save;
#define map g_map

int build_crc()
{
    char buf[200000]; memset(buf, 0, sizeof(buf));
    int buf_size = 0;
	
    p1->eot_save(buf, buf_size);
    p2->eot_save(buf, buf_size);
    map->eot_save(buf, buf_size);

    int crc = crc16(buf);
    g_eot_save[crc] = std::string(buf, buf_size);

    return crc;
}

/**
 * Check that no moves are performed on the map, 
 * so that it is safe to end turn or do something similar
 */
bool nomoves()
{
    return platoon_local->nomoves() && platoon_remote->nomoves();
}

/**
 * Function that saves game state information to a text file
 */
static void dump_gamestate_on_crc_error(int crc)
{
    if (g_eot_save.find(crc) == g_eot_save.end()) return;
    char filename[128];
    sprintf(filename, "$(home)/eot_save_%d.txt", crc);
    int fh = open(F(filename), O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
    if (fh != -1) {
        write(fh, g_version_id.data(), g_version_id.size());
        write(fh, g_eot_save[crc].data(), g_eot_save[crc].size());
        close(fh);
    }
}

/**
 * Function that calculates current game state crc 
 * and compares it with the value received from the remote computer (crc argument)
 */
void check_crc(int crc)
{
    int bcrc = build_crc();
    if (crc != bcrc) {
        g_console->printf(COLOR_SYS_FAIL, _("wrong wholeCRC") );
        g_console->printf(COLOR_SYS_INFO, "crc=%d, bcrc=%d", crc, bcrc);
        net->send_debug_message("crc error");
        battle_report( "# %s: crc=%d, bcrc=%d\n", _("wrong wholeCRC"), crc, bcrc );
        
        dump_gamestate_on_crc_error(crc);
        dump_gamestate_on_crc_error(bcrc);
    }
    
    g_eot_save.empty();
}

/**
 * Function that is called to make all the necessary changes to the gamestate
 * when changing active player (passing turn)
 */
void switch_turn()
{
    CONFIRM_REQUESTED = 0; // ???

    turn++;
    map->step();

//  Still did not test where this code would be better to put
    switch (scenario->check_conditions()) {
        case 1:
        loss = 1;
        break;
        
        case 2:
        win = 1;
        break;
        
        case 3:
        win = loss = 1;
        break;
    }      
    
    elist->step(0);
}

/**
 * This function is called when player wants to pass turn to the other player
 */
void send_turn()
{
    ASSERT(MODE != WATCH);
    platoon_local->restore_moved();
    switch_turn();
    
    int crc = build_crc();
    net->send_endturn(crc);

    battle_report("# %s: %d\n", _("Turn end"), turn );
    g_console->printf(COLOR_VIOLET00, "%s", _("Turn end") );
    if(FLAGS & F_ENDTURNSND)
        soundSystem::getInstance()->play(SS_BUTTON_PUSH_2);

    platoon_remote->restore();
    platoon_remote->set_visibility_changed();
    platoon_remote->recalc_visibility();

    if (net->gametype == GAME_TYPE_HOTSEAT) {
        if (win || loss) {
        //  !!! Hack - to prevent unnecessery replay while in endgame screen
            closehotseatgame();         
            return;
        }
    
    //  Load the game state for the start of enemy turn and switch to WATCH mode
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
        platoon_local->recalc_visibility();

        MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
        alert(" ", _("  NEXT TURN  "), " ", 
                   _("    OK    "), NULL, 1, 0);
    }

    g_time_left = 0;
    MODE = WATCH;
}

/**
 * This function is called when we receive turn from the other player
 */
void recv_turn(int crc)
{
    // In replay mode pass of the turn is simple
    if (net->gametype == GAME_TYPE_REPLAY) {
        switch_turn();
        platoon_local->restore();
		
        Platoon *pt = platoon_local;
        platoon_local = platoon_remote;
        platoon_remote = pt;
		
        return;
    }

    ASSERT(MODE == WATCH);
    switch_turn();
    
    check_crc(crc);

    platoon_local->restore();
    platoon_local->set_visibility_changed();
    platoon_local->recalc_visibility();

    if (net->gametype == GAME_TYPE_HOTSEAT) {
        savegame(F("$(home)/ufo2000.tmp"));
        map->m_minimap_area->set_full_redraw();
    }

    platoon_local->check_morale();

    g_time_left = g_time_limit;
    last_time_left = -1;
    MODE = MAP3D;

    g_console->printf(
        COLOR_VIOLET00,             // COLOR_SYS_INFO1
        _("Next turn. local = %d, remote = %d soldiers"),
        platoon_local->num_of_men(),
        platoon_remote->num_of_men());
    if(FLAGS & F_ENDTURNSND)
        soundSystem::getInstance()->play(SS_BUTTON_PUSH_2); 

    battle_report("# %s: %d\n", _("Next turn"), turn );
}

int GAMELOOP = 0;

#define STAT_PANEL_W 200

void draw_stats()
{ 
    if (screen->w - SCREEN2W < STAT_PANEL_W) return;
    
    BITMAP *stat_panel = create_bitmap(STAT_PANEL_W, 200);
    clear_to_color(stat_panel, BACKCOLOR);
    int i = 0;
    Soldier *man;
    textprintf(stat_panel, g_small_font, 10, 0, COLOR_GREEN, "Local  %d", platoon_local->num_of_men());
    textprintf(stat_panel, g_small_font, 100, 0, COLOR_RED, "Remote %d", platoon_remote->num_of_men());
    for (i = 0; i < SQUAD_LIMIT; i++) {
        man = platoon_local->findnum(i);
        if (man != NULL)
            man->draw_stats(stat_panel, 10, 10 * (i + 1), (man == sel_man));
    }
    blit(stat_panel, screen, 0, 0, screen->w - STAT_PANEL_W, 250, STAT_PANEL_W, 200);
    destroy_bitmap(stat_panel);
}

/**
 * Redraw battlescape and minimap on the screen
 */
void build_screen(int & select_y)
{
    int icon_nr = -9;
    clear_to_color(screen2, BACKCOLOR);

    map->set_sel(mouse_x, mouse_y);
    map->draw();

    if(sel_man && !sel_man->ismoving() && (MODE == MAP3D || MODE == WATCH)) {
        if(key[KEY_LCONTROL])
            map->draw_path_from(sel_man);
        else if(key[KEY_ALT])
            sel_man->draw_bullet_way();
    }

    p1->bulldraw();
    p2->bulldraw();

    platoon_remote->draw_blue_selectors();

    if (sel_man != NULL) {
        // Todo: adjust select_y for elevation of current tile (e.g. stairs)
        sel_man->draw_selector(select_y);
        sel_man->draw_enemy_seen(select_y);
    }

	if (net->gametype != GAME_TYPE_REPLAY)
        icon->draw();
    else {
        char buf[10];
        if (replaydelay != -1)
            sprintf(buf, "< %d >", 9 - replaydelay);
        else
            sprintf(buf, "< P >");
        rect(screen2, 0, 10, text_length(font, buf) + 2, text_height(font) + 12, COLOR_GRAY01);
        rectfill(screen2, 1, 11, text_length(font, buf) + 1, text_height(font) + 11, COLOR_GRAY15);
        textout(screen2, font, buf, 2, 12, COLOR_GRAY01);
    }

    if (g_time_left > 0) 
        show_time_left();

    draw_stats();

    if (MODE == WATCH)
        textprintf(screen2, font, 0, 0, COLOR_WHITE, _("WATCH") );

    if (FLAGS & F_TOOLTIPS && net->gametype != GAME_TYPE_REPLAY) {
        // Tooltips for the buttons of the control-panel:
        if (icon->inside(mouse_x, mouse_y)) {
            icon_nr = icon->identify(mouse_x, mouse_y);
            if (icon_nr >= 0 ) {
                int prev_tm = text_mode(0);
                textprintf(screen2, font,  mouse_x+7, mouse_y-2,
                            COLOR_WHITE, "%s", icontext(icon_nr) );
                text_mode(prev_tm);
            }
        }
    }
            
    if (MODE == MAP2D) {
        map->draw2d();
    } else if (MODE == MAN) {
        if (sel_man != NULL) {
            inventory->draw(SCREEN2W / 2 - 160, SCREEN2H / 2 - 100);
        } else {
            MODE = MAP3D;
        }			
	} else if (MODE == UNIT_INFO) {
        if (sel_man != NULL)
            sel_man->draw_unibord(SCREEN2W / 2 - 160, SCREEN2H / 2 - 100);
        else
            MODE = MAP3D;
    }
    
    draw_sprite(screen2, mouser, mouse_x, mouse_y);
    blit(screen2, screen, 0, 0, 0, 0, screen2->w, screen2->h);
    map->svga2d();      // Minimap

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

/**
 * Opens stream for replay file and saves game position to it
 */
void savereplay(const char *filename)
{   
	//When engine will read packets from replay file it will expect packets are send
	//from remote platoon to local platoon. So if local player goes first we have to
	//swap platoons in replay.
    if(HOST) {
        Platoon *pt = platoon_local;
        platoon_local = platoon_remote;
        platoon_remote = pt;
    }

    net->m_replay_file = new std::fstream(filename, std::ios::binary | std::ios::out);
    savegame_stream(*net->m_replay_file);
    
    //Restoring platoons for current game
    if(HOST) {
        Platoon *pt = platoon_local;
        platoon_local = platoon_remote;
        platoon_remote = pt;
    }
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
    PersistWriteObject(archive, cur_random);
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

/**
 * Opens stream for replay file and loads game position from it
 */
bool loadreplay(const char *filename)
{
    net->m_replay_file = new std::fstream(filename, std::ios::binary | std::ios::in);
    if (!net->m_replay_file->is_open()) return false;
    return loadgame_stream(*net->m_replay_file);
}

bool loadgame_stream(std::iostream &stream)
{
    char sign[64], buff[64];
    sprintf(sign, "ufo2000 %s (%s %s)\n", UFO_VERSION_STRING, __DATE__, __TIME__);
    stream.read(buff, strlen(sign) + 1);
    if (strcmp(sign, buff) != 0) {
      // Developers: comment this out, to re-use saved game after new compile
        return false;  // "version of savegame not compatible"
    }

    persist::Engine archive(stream, persist::Engine::modeRead);

    PersistReadBinary(archive, &turn, sizeof(turn));
    PersistReadBinary(archive, &MODE, sizeof(MODE));
    PersistReadBinary(archive, &pd1,  sizeof(pd1));
    PersistReadBinary(archive, &pd2,  sizeof(pd2));
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
    PersistReadObject(archive, cur_random);

    TARGET = 0;

    return true;
}

/**
 * Set colors for list of soldier-names,
 * e.g. in endgame-stats.
 */
int name_color( int player, int nr, int dead )
// Note: colors are choosen to be readable with and without the background-image
{
    static int alive = 0;
    int c1;

    if (dead)
        return COLOR_DARKGRAY08;

    alive ++;
    if (nr == 1)
       alive = 0;

    if (player == 0)
        c1 =  16;  // COLOR_ORANGE00
    else
        c1 =  48;  // COLOR_GREEN00

    return xcom_color(c1 + alive);
}
/**
 * Set colors for displaying number of kills
 */
int kills_color( int kills )
{
    if (kills == 0)
        return COLOR_GRAY02;
    else if (kills <= 1)
        return COLOR_RED00;
    else if (kills <= 2)
        return COLOR_RED03;
    else if (kills <= 3)
        return COLOR_RED07;
    else if (kills <= 4)
        return COLOR_RED10;

    return COLOR_RED12;
}
/**
 * Set colors for displaying amount of damage done
 */
int damage_color( int damage )
{
  //int c1 = 192;  // COLOR_VIOLET00
  //int c1 = 224;  // COLOR_GRAYBLUE00
    int c1 = 208;  // COLOR_SKYBLUE00

    if (damage == 0)
        return COLOR_GRAY02;
    else if (damage <= 50)
        return xcom1_color(c1 + 0);
    else if (damage <= 100)
        return xcom1_color(c1 + 2);
    else if (damage <= 200)
        return xcom1_color(c1 + 5);
    else if (damage <= 300)
        return xcom1_color(c1 + 8);

    return xcom1_color(c1 + 10);
}

/**
 * Note: These stats have several quirks, e.g.
 * kills and damage to own men contribute to your 'success',
 * damage from explosions might be accounted several times,
 * stun-damage is not accounted at all,
 * a scout who gets no kills himself is hardly a coward, etc.
 * @brief  Display combat-statistics after a game
 * @sa     Soldier::explo_hit()
 */
void endgame_stats()
{
    lua_message( "Enter: endgame_stats" );
    net->send_debug_message("result:%s", (win == loss) ? ("draw") : (win ? "victory" : "defeat"));
    
    BITMAP *back;
    BITMAP *scr = create_bitmap(320, 200);
    BITMAP *newscr = create_bitmap(SCREEN_W, SCREEN_H);
    clear(scr);
    clear(newscr);
    char player1[64];
    char player2[64];
    char winner[64];
    char txt[64];

    StatEntry *temp;
    Platoon *ptemp;

    if ((net->gametype == GAME_TYPE_HOTSEAT) && !(turn % 2)) //turn % 2 != 0 - wrong
    {
        ptemp = platoon_remote;
        platoon_remote = platoon_local; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        platoon_local = ptemp;
    }

    int local_kills      = platoon_local ->get_stats()->total_kills();
    int remote_kills     = platoon_remote->get_stats()->total_kills();
    int local_dead       = platoon_local ->get_stats()->total_dead();
    int remote_dead      = platoon_remote->get_stats()->total_dead();
    int local_inflicted  = platoon_local ->get_stats()->total_damage_inflicted();
    int remote_inflicted = platoon_remote->get_stats()->total_damage_inflicted();
    int local_taken      = platoon_local ->get_stats()->total_damage_taken();
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
    if (temp->get_inflicted() > devastating->get_inflicted()) //why mvp?
    {
        devastating_remote = 1;
        devastating = temp;
    }

    StatEntry *coward = platoon_local->get_stats()->get_least_inflicted();
    temp = platoon_remote->get_stats()->get_least_inflicted();
    if (temp->get_inflicted() < coward->get_inflicted()) //why mvp?
    {
        coward_remote = 1; //why 0?
        coward = temp;
    }
    else 
        if(temp->get_inflicted() == coward->get_inflicted())
        {
            // (turn%2!=0) means p1 wins and now p1=platon_local. lets make loser most coward
            coward_remote = (turn%2)?1:0;
            coward = (turn%2)?temp:coward;
        }

    if ((net->gametype == GAME_TYPE_HOTSEAT) && !(turn % 2))
    {
        ptemp = platoon_remote;
        platoon_remote = platoon_local; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        platoon_local = ptemp;
    }

    fade_out(10);
    clear(screen);

    reset_video();

    DONE = 0;

    BITMAP *back_win  = load_back_image(cfg_get_win_image_file_name());
    BITMAP *back_lose = load_back_image(cfg_get_lose_image_file_name());

    if (net->gametype == GAME_TYPE_HOTSEAT)
    {
       if (win == loss) {
            FS_MusicPlay(F(cfg_get_lose_music_file_name()));
            back = back_lose;
            strcpy(winner, _("DRAW!"));
        } else {
            FS_MusicPlay(F(cfg_get_win_music_file_name()));
            back = back_win;
            if (win) {
                if (turn % 2 == 0) strcpy(winner, _("PLAYER 2 WINS!") );
                else               strcpy(winner, _("PLAYER 1 WINS!") );
            } else {
                if (turn % 2 == 0) strcpy(winner, _("PLAYER 1 WINS!") );
                else               strcpy(winner, _("PLAYER 2 WINS!") );
            }
        }
    } else {
        // Only go to the top case if it's an exclusive win.
        if ((win && (!loss))) {
            FS_MusicPlay(F(cfg_get_win_music_file_name()));
            back = back_win;
            strcpy(winner, _("YOU WIN!") );
        } else {
            FS_MusicPlay(F(cfg_get_lose_music_file_name()));
            back = back_lose;
            if (!win) strcpy(winner, _("YOU LOSE!") );
            else      strcpy(winner, _("DRAW!") );
        }
    }

    // Todo: other background-image, or make this one darker
    stretch_blit(back, newscr, 0, 0, back->w, back->h, 0, 0, SCREEN_W, SCREEN_H);

    // avoid open/closing logfile with many battle_report() - statements:
    FILE *f_br = fopen( F("$(home)/battlereport.txt"), "at");
    fprintf(f_br, "\n# %s:\n\n", _("Summary") );
    fprintf(f_br, "%s \n\n", winner);

    textprintf_centre(newscr, large, 320, 12-4, COLOR_RED00, "%s", winner);

    int x1 =  16, x2 = 336;
    int y1 =  38-8, y2 =  56-8, h = 8, w = 8;
    if (net->gametype == GAME_TYPE_HOTSEAT)
    {
        //textprintf(newscr, g_small_font,   8, 60, COLOR_WHITE, "Player 1");
        //textprintf(newscr, g_small_font, 328, 60, COLOR_WHITE, "Player 2");
        strcpy(player1, _("Player 1") );
        strcpy(player2, _("Player 2") );
      //textprintf(newscr, large,  x1, y1, COLOR_GREEN, _("Player 1") );
      //textprintf(newscr, large,  x2, y1, COLOR_GREEN, _("Player 2") );
    } else {
        // Todo: Use login-name of other player
        strcpy(player1, _("Your Platoon") );
        strcpy(player2, _("Remote Platoon") );
      //textprintf(newscr, large,  x1, y1, COLOR_GREEN, _("Your Platoon") );
      //textprintf(newscr, large,  x2, y1, COLOR_GREEN, _("Remote Platoon") );
    }
    textprintf(newscr, large,  x1, y1, COLOR_GREEN, player1 );
    textprintf(newscr, large,  x2, y1, COLOR_GREEN, player2 );

    strcpy(txt, _("Total Kills:") );
    textprintf(newscr, g_small_font,  x1+ 0, y2+0*h, COLOR_RED03,  "%s",  txt );
    textprintf(newscr, g_small_font,  x2+ 0, y2+0*h, COLOR_YELLOW, "%s",  txt);
    textprintf(newscr, g_small_font,  x1+68, y2+0*h, COLOR_RED03,  "%2d", local_kills);
    textprintf(newscr, g_small_font,  x2+68, y2+0*h, COLOR_YELLOW, "%2d", remote_kills);
    fprintf(f_br, "%-30s: %14s %14s\n", _("Player"), player1, player2);
    fprintf(f_br, "%-30s  %14d %14d\n", txt, local_kills, remote_kills);

    strcpy(txt, _("Death:") );
    textprintf(newscr, g_small_font,  x1+ 0, y2+1*h, COLOR_YELLOW, "%s",  txt);
    textprintf(newscr, g_small_font,  x2+ 0, y2+1*h, COLOR_RED03,  "%s",  txt);
    textprintf(newscr, g_small_font,  x1+68, y2+1*h, COLOR_YELLOW, "%2d", local_dead);
    textprintf(newscr, g_small_font,  x2+68, y2+1*h, COLOR_RED03,  "%2d", remote_dead);
    fprintf(f_br, "%-30s  %14d %14d\n", txt, local_dead, remote_dead);

    strcpy(txt, _("Total Damage Inflicted:") );
    textprintf(newscr, g_small_font,  x1+  0, y2+2*h, COLOR_BLUE,   "%s",  txt);
    textprintf(newscr, g_small_font,  x2+  0, y2+2*h, COLOR_GRAY,   "%s",  txt);
    textprintf(newscr, g_small_font,  x1+104, y2+2*h, COLOR_BLUE,   "%6d", local_inflicted);
    textprintf(newscr, g_small_font,  x2+104, y2+2*h, COLOR_GRAY,   "%6d", remote_inflicted);
    fprintf(f_br, "%-30s  %14d %14d\n", txt, local_inflicted, remote_inflicted);

    strcpy(txt, _("Total Damage Taken:") );
    textprintf(newscr, g_small_font,  x1+  0, y2+3*h, COLOR_GRAY,   "%s",  txt);
    textprintf(newscr, g_small_font,  x2+  0, y2+3*h, COLOR_BLUE,   "%s",  txt);
    textprintf(newscr, g_small_font,  x1+104, y2+3*h, COLOR_GRAY,   "%6d", local_taken);
    textprintf(newscr, g_small_font,  x2+104, y2+3*h, COLOR_BLUE,   "%6d", remote_taken);
    fprintf(f_br, "%-30s  %14d %14d\n", txt, local_taken, remote_taken);

    // Useless ranking - not logged to battlereport...

    //textprintf_centre(newscr, large,        320, 108, COLOR_GOLD, "Most Valuable Soldier:");
    //textprintf_centre(newscr, g_small_font, 320, 124, COLOR_GOLD, "%s (%s, %d kills)",
    textprintf_centre(newscr, large,        104, 100, COLOR_GOLD, _("Most Valuable Soldier:"));
    textprintf_centre(newscr, g_small_font, 110, 116, COLOR_GOLD, _("%s (%s, %d kills)"),
        mvp->get_name(),
        (mvp_remote) ? ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 2") : _("Remote") ) : ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 1") : _("Local") ),
        mvp->get_kills());

    //textprintf_centre(newscr, large,        320, 140, COLOR_MAGENTA, "Most Devastating Soldier:");
    //textprintf_centre(newscr, g_small_font, 320, 156, COLOR_MAGENTA, "%s (%s, %d damage inflicted)",
    textprintf_centre(newscr, large,        300, 100-10, COLOR_MAGENTA, _("Most Devastating Soldier:") );
    textprintf_centre(newscr, g_small_font, 300, 116-10, COLOR_MAGENTA, _("%s (%s, %d damage inflicted)"),
        devastating->get_name(),
        (devastating_remote) ? ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 2") : _("Remote")) : ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 1") : _("Local") ),
        devastating->get_inflicted());

    //textprintf_centre(newscr, large,        320, 172, COLOR_ROSE, "Most Cowardly Soldier:");
    //textprintf_centre(newscr, g_small_font, 320, 188, COLOR_ROSE, "%s (%s, %d damage inflicted)",
    textprintf_centre(newscr, large,        516, 100, COLOR_ROSE, _("Most Cowardly Soldier:") );
    textprintf_centre(newscr, g_small_font, 504, 116, COLOR_ROSE, _("%s (%s, %d damage inflicted)"),
        coward->get_name(),
        (coward_remote) ? ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 2") : _("Remote") ) : ((net->gametype == GAME_TYPE_HOTSEAT) ? _("Player 1") : _("Local") ),
        coward->get_inflicted());

    // Table of soldier-names for both players, with kills and damage:
  //int x1 =  20, x2 = 320, y1 = 138;
        x1 =  20; x2 = 320; y1 = 134;
        h  =  12, w  =   8;
    int x  =   0, y  =   0;
    int dead = 0, kills = 0, damage = 0;

    // "18s 9s 6s" vs. "22s 5d 6d" to give some room for translations:
    textprintf(newscr, font, x1,    y1, COLOR_WHITE, 
               "%-18s %9s %6s", _("Name"), _("Kills"), _("Damage") );
    textprintf(newscr, font, x1+x2, y1, COLOR_WHITE, 
               "%-18s %9s %6s", _("Name"), _("Kills"), _("Damage") );

    fprintf(f_br, "\n# %s:\n", _("Details") );
    fprintf(f_br, "\n%-20s:  %-18s %9s %6s\n", _("Player"),
            _("Name"), _("Kills"), _("Damage") );

    for (int pl = 0; pl < 2; pl++) {
        fprintf(f_br, "---\n");
        if (pl == 0) {
            temp = platoon_local ->get_stats()->getfirst();
            strcpy(txt, player2 ); // ??
        } else {
            temp = platoon_remote->get_stats()->getfirst();
            strcpy(txt, player1 );  // ??
        }

        for (int nr = 1; nr <= 15 ; nr++) { // screen-space for 15 soldiers per side
            x = x1   + pl * x2;
            y = y1+4 + nr * h;
            if (temp != NULL) {
                dead   = temp->is_dead();
                kills  = temp->get_kills();
                damage = temp->get_inflicted();

                textprintf(newscr, font, x,      y, name_color(pl, nr, dead), "%-22s", temp->get_name());
                textprintf(newscr, font, x+22*w, y, kills_color(kills  ), "%5d", kills  );
                textprintf(newscr, font, x+28*w, y, damage_color(damage), "%6d", damage );
                if (dead)
                    fprintf(f_br, "%-20s: -%-22s %5d %6d\n", txt,
                            temp->get_name(), kills, damage);
                else
                    fprintf(f_br, "%-20s:  %-22s %5d %6d\n", txt,
                            temp->get_name(), kills, damage);

                temp = temp->getnext();
            }
        }
    }

    fprintf(f_br, "# %s #\n\n", _("End") );
    fclose(f_br);  // Battlereport

    g_console->set_full_redraw();

    MODE = MAP3D; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    CHANGE = 1;

    g_console->printf(COLOR_SYS_PROMPT, "%s\n\n", _("You can chat here.  Press ESC when finished.") );
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
            int vol;
            int scancode;
            int keycode = ureadkey(&scancode);

            switch (scancode) {
                case KEY_ESC:
                    if (askmenu( _("EXIT TO MAIN MENU") ))
                        DONE = 1;
                    break;
                case KEY_UP:
                    resize_screen2(0, -10);
                    break;
                case KEY_DOWN:
                    resize_screen2(0, 10);
                    break;
                case KEY_ASTERISK:   // ?? ToDo: Sound+Music on/off
                    //soundSystem::getInstance()->play(SS_WINDOW_OPEN_2);
                    FS_MusicPlay(NULL);
                    g_console->printf(COLOR_SYS_FAIL, _("Music OFF") );
                    break;
                case KEY_F1:
                    help( HELP_ENDGAME );
                    break;
                case KEY_F9:
                    keyswitch(0);
                    //g_console->printf(COLOR_SYS_OK, "%s", "Keyboard changed");
                    // ?? Todo: other feedback to user ??
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
    FS_MusicPlay(NULL);
}

#ifdef WIN32

#include "exchndl/exchndl.h"

static LPTOP_LEVEL_EXCEPTION_FILTER prevExceptionFilter = NULL;

static LONG WINAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
#ifdef __MINGW32__
    char *exception_report = GenerateExceptionReport(pExceptionInfo);
#else
    char *exception_report = "?";
#endif
    if (net) {
        net->send_debug_message("crash:%s", exception_report);
        net->flush();
    }

    FILE *f = fopen(F("$(home)/ufo2000-crash.html"), "at");
    fprintf(f, "<H1>%s</H1>\n", "Ufo2000-Crashreport");
    fprintf(f, "%s\n", exception_report);
    fprintf(f, "<HR>\n" );
    fclose(f);

    if (prevExceptionFilter)
        return prevExceptionFilter(pExceptionInfo);
    else
        return EXCEPTION_CONTINUE_SEARCH;
}
#endif

/**
 * @brief  Show the reason why the requested action could not be performed.
 */
void report_game_error(int chk)
{
    // TODO: The error sound should be toggled by some specific flag.
    // TODO: Even message displaying should be toggled since it may become
    //       too annoying, for example, when trying to throw a grenade
    //       as far as the soldier can.
    chk = -chk; // Error codes are negative.
    if(FLAGS & F_ENDTURNSND)
        soundSystem::getInstance()->play(SS_BUTTON_PUSH_2);
    g_console->printf(GameErrorColour[chk], GameErrorMessage[chk]);
}

/**
 * @brief  Shift the active map level up by 1.
 */
void view_level_up()
{
    if (map->sel_lev < map->level - 1) {
        map->sel_lev++;
        position_mouse(mouse_x, mouse_y - CELL_SCR_Z);
    }
}

/**
 * @brief  Shift the active map level down by 1.
 */
void view_level_down()
{
    if (map->sel_lev > 0) {
        map->sel_lev--;
        position_mouse(mouse_x, mouse_y + CELL_SCR_Z);
    }
}

/**
 * Main loop of the tactical part of the game
 */
void gameloop()
{
    // If it's not replay mode, this code start to write information into replay file
    if (net->gametype != GAME_TYPE_REPLAY)
        savereplay(F("$(home)/replay.tmp"));

    int select_y = 0;
    int mouse_leftr = 1, mouse_rightr = 1;
    int old_mouse_z = mouse_z; // mouse wheel status on the previous cycle
    int color1;
    int b1 = 0, k, who;
    char buf[STDBUFSIZE];

    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    resize_screen2(0, 0);
    MouseRange *temp_mouse_range_ptr;
    lua_message( "Start: gameloop" );
    if ((rand() % 2) == 1)
        FS_MusicPlay(F(cfg_get_combat2_music_file_name()));
    else
        FS_MusicPlay(F(cfg_get_combat1_music_file_name()));

    clear_keybuf();
    GAMELOOP = 1;

    g_console->printf( COLOR_SYS_HEADER, _("Welcome to the battlescape of UFO2000 !") );
    g_console->printf( COLOR_SYS_INFO1,  _("Press F1 for help.") );  // see KEY_F1
    color1 = 0;
    battle_report( "*\n* %s: %s\n*\n\n", _("Battlereport"), datetime() );

    platoon_local->set_visibility_changed();
    platoon_local->recalc_visibility();
    platoon_remote->set_visibility_changed();
    platoon_remote->recalc_visibility();

    if (MODE != WATCH) {
        g_time_left = g_time_limit;
        last_time_left  = -1;
    } else {
        g_time_left = 0;
    }

    if (net->gametype == GAME_TYPE_HOTSEAT)
        savegame(F("$(home)/ufo2000.tmp"));

    while (!DONE) {

        rest(1); // Don't eat all CPU resources

        if (MODE != WATCH && g_time_left == 0) {
            TARGET = 0;
            if (nomoves()) {
                send_turn();
            }
        }

        g_console->redraw(screen, 0, SCREEN2H);

		if (net->gametype == GAME_TYPE_REPLAY) {
			if (REPLAYIT >= replaydelay) {
				if (replaydelay != - 1)
					net->check();
				REPLAYIT = 0;
			}
        } else {
        	net->check();
        }

    if (g_tie == 3) // Check if both players accepted the draw.
    {
        win = loss = 1;
        DONE = 1;
    }

        if (win || loss) break;

        if (NOTICE) {
            if (NOTICEdemon) {
                net->send_notice();
//              g_console->printf(COLOR_SYS_INFO, "%d", NOTICEremote);
            }
            NOTICEremote++;
            NOTICE = 0;
        }

        if (CHANGE) {
            FLYIT  = 0;
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
            if (sel_man == NULL)  // Get cursor back to normal mode in case
                TARGET = 0;       // the current soldier died while targetting.

            select_y = (select_y + 1) % 3;
            CHANGE = 1;

            MOVEIT--;
        }

        if (CHANGE) {
            build_screen(select_y);
//          g_console->printf(COLOR_SYS_INFO, "*"); // temp - for debugging
            CHANGE = 0;
        }

        if (MAPSCROLL) {
            if ((MODE == MAP3D) || (MODE == WATCH)) {
                if (map->scroll(mouse_x, mouse_y)) CHANGE = 1;
            }
            MAPSCROLL = 0;
        }

        if ((mouse_b & 1) && (mouse_leftr)) { //left mouseclick
            mouse_leftr = 0;
            switch (MODE) {
                case UNIT_INFO:
                    MODE = MAP3D;
                    break;
                case WATCH:
                	if (net->gametype != GAME_TYPE_REPLAY) {
                    	if (icon->inside(mouse_x, mouse_y)) {
                        	icon->execute(mouse_x, mouse_y);
                        	break;
                    	}
                    } else {
                    	if (mouse_inside(2, 12, 10, 20)) {
                    			if (replaydelay != -1)
                    				replaydelay++;
                    			if (replaydelay > 8)
                    				replaydelay = -1;
                    	}
                    	if (mouse_inside(18, 12, 26, 20))
                    		replaydelay = -1;
                    	if (mouse_inside(34, 12, 42, 20)) {
                    			if (replaydelay != -1)
                    				replaydelay--;
                    			else
                    				replaydelay = 8;
                    			if (replaydelay < 0)
                    				replaydelay = 0;
                    	}
                    }
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

                    // Handle buttons of the control panel
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
                            if (!sel_man->ismoving() && sel_man->standup()) {
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

        if ((mouse_b & 2) && (mouse_rightr)) { //right mouseclick
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
                    if (icon->inside(mouse_x, mouse_y))
                        break;

                    if (TARGET)
                        TARGET = 0;
                    else {
                        if (sel_man != NULL) {
                            // Wait for opponent to finish reaction fire
                            // (avoid dodging reaction fire bullets).
                            if (!platoon_remote->nomoves()) break;

                            if (!sel_man->ismoving()) {
                                sel_man->faceto(map->sel_col, map->sel_row);

                                //if (sel_man->curway == -1)
                                //  sel_man->open_door();
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

        // Handle mouse wheel events
        if (mouse_z != old_mouse_z) {
            // Mouse wheel state has changed
            // Scroll viewport level up and down
            if(mouse_z > old_mouse_z)
                view_level_up();
            else
                view_level_down();
            old_mouse_z = mouse_z;
        }

        // update mouse buttons' states
        if (!(mouse_b & 1)) {
            mouse_leftr = 1;
//          CHANGE = 1;
        }

        if (!(mouse_b & 2)) {
            mouse_rightr = 1;
//          CHANGE = 1;
        }

        process_keyswitch();

        if (keypressed()) {
            int scancode;
            int keycode = ureadkey(&scancode);
            int vol;

            switch (scancode) {
                case KEY_PGUP:
                    view_level_up();
                    break;
                case KEY_PGDN:
                    view_level_down();
                    break;
                case KEY_TAB:   //next soldier
                    TARGET = 0;
                    if (sel_man == NULL) {
                        sel_man = platoon_local->captain();
                        if (sel_man != NULL)
                            map->center(sel_man);
                    } else if (!sel_man->ismoving()) {
                        Soldier *s = sel_man;
                        sel_man = platoon_local->next_not_moved_man(sel_man);
                        if (s != sel_man)
                            map->center(sel_man);
                    }
                    break;
                case KEY_ASTERISK:   // ?? ToDo: Sound+Music on/off
                    //soundSystem::getInstance()->play(SS_WINDOW_OPEN_2);
                    FS_MusicPlay(NULL);
                    g_console->printf(COLOR_SYS_FAIL, _("Music OFF") );
                    break;
                case KEY_LEFT:
                    if (!key[KEY_LSHIFT])
                        map->move(mapscroll, 0);
                    else
                        //resize_screen2(-10, 0);
                        if (SCREEN2W == screen->w) {
                            int wth = map->m_minimap_area->get_minimap_width();
                            wth += wth >= STAT_PANEL_W ? 0 : STAT_PANEL_W - wth;
                            resize_screen2(-(wth + 10), 0);
                        }
                    break;
                case KEY_UP:
                    if (!key[KEY_LSHIFT])
                        map->move(0, mapscroll);
                    else
                        resize_screen2(0, -10);
                    break;
                case KEY_RIGHT:
                    if (!key[KEY_LSHIFT])
                        map->move(-mapscroll, 0);
                    else
                        //resize_screen2(10, 0);
                        if (SCREEN2W < screen->w)
                            resize_screen2(screen->w - SCREEN2W, 0);
                    break;
                case KEY_DOWN:
                    if (!key[KEY_LSHIFT])
                        map->move(0, -mapscroll);
                    else
                        resize_screen2(0, 10);
                    break;
                case KEY_F1:
                    switch (MODE) {
                        case UNIT_INFO:
                            help( HELP_STATS );
                            break;
                        case MAP2D:
                            help( HELP_MAPVIEW );
                            break;
                        case MAN:
                            help( HELP_INVENTORY );
                            break;
                        default:
                            help( HELP_BATTLESCAPE );
                    }
                    break;
                case KEY_F2:
                    if (askmenu( _("SAVE GAME") )) {
                        savegame(F("$(home)/ufo2000.sav"));
                        // Todo: test if save was successful
                        g_console->printf(COLOR_SYS_OK, _("Game saved") );

                        battle_report( "# %s\n", _("Game saved") );
                    }
                    break;
                case KEY_F3:
                    if (askmenu( _("LOAD GAME") )) {
                        if (!loadgame(F("$(home)/ufo2000.sav"))) {
                            battle_report( "# %s: %s\n", _("LOAD GAME"), _("failed") );
                            temp_mouse_range_ptr = new MouseRange(0, 0, SCREEN_W - 1, SCREEN_H - 1);
                            alert( _("Saved game not found"), "", "", _("OK"), NULL, 0, 0);
                            delete temp_mouse_range_ptr;
                        } else {
                            battle_report( "# %s: %s\n", _("LOAD GAME"), _("success") );
                        }
                        inithotseatgame();
                        if (net->gametype == GAME_TYPE_HOTSEAT)
                            savegame(F("$(home)/ufo2000.tmp"));
                    }
                    break;
                case KEY_F5: 
                    if (FLAGS & F_TOOLTIPS) { 
                        FLAGS &= ~F_TOOLTIPS;
                        g_console->printf(COLOR_SYS_FAIL, _("Tooltips OFF") );
                    } else {
                        FLAGS |= F_TOOLTIPS;
                        g_console->printf(COLOR_SYS_OK,   _("Tooltips ON.") );
                    } 
                    soundSystem::getInstance()->play(SS_BUTTON_PUSH_1); 
                    break;
                case KEY_F9:
                    keyswitch(0);
                    // g_console->printf(COLOR_SYS_OK, "%s", "Keyboard changed");
                    // Todo: other form of feedback ??
                    break;
                case KEY_F10:
                    change_screen_mode();
                    break;
                case KEY_F11:
                    if (NOTICEdemon) {
                        if (askmenu( _("STOP NOTIFY") ))
                            NOTICEdemon = 0;
                    } else {
                        if (askmenu( _("START NOTIFY") )) {
                            NOTICEdemon = 1;
                            NOTICEremote = 0;
                        }
                    }
                    break;
                case KEY_F12:
                    if (askmenu( _("SCREEN-SNAPSHOT") )) {
                        savescreen();
                    }
                    break;
                case KEY_ESC:
                    if (MODE == MAN) {
                        inventory->close();
                    } else if (MODE == UNIT_INFO || MODE == MAP2D) {
                        MODE = MAP3D;
                    } else {
                        temp_mouse_range_ptr = new MouseRange(0, 0, SCREEN_W - 1, SCREEN_H - 1);
                        who = (p1 == platoon_local);
	                    k = (1 << who);
	                    if (net->gametype != GAME_TYPE_REPLAY) {
    	                    b1 = alert3("", _("ABORT MISSION ?"), "", _("YES=RESIGN"),
        	                    (g_tie & k) ? _("RECALL DRAW OFFER") : 
            	                (g_tie & (k ^ 3)) ? _("ACCEPT DRAW OFFER") :
                	            _("OFFER DRAW"), _("NO=CONTINUE"), 0, 0, 1);
                	    } else {
                	    	b1 = askmenu(_("EXIT FROM REPLAY?"));
                	    }
                   	    delete temp_mouse_range_ptr;
                       	if (b1 == 1) {
                           	DONE = 1;
                            battle_report( "# %s\n", _("Game aborted") );
   	                    } else if (b1 == 2) {
       	                    g_tie ^= k;
           	                net->send_tie(who);
               	            if (g_tie == 3) {
                   	            sprintf(buf, "%s", _("You: Draw offer accepted"));
                       	    } else if (g_tie & k) {
                           	    sprintf(buf, "%s", _("You: Draw offered"));
                            } else {
   	                            sprintf(buf, "%s", _("You: Draw offer recalled"));
       	                    }
           	                g_console->printf(COLOR_SYS_PROMPT, buf);
               	            battle_report("# %s\n", buf);
                   	    }
                    }
                    break;
                default:
                    if (g_console->process_keyboard_input(keycode, scancode))
                        net->send_message((char *)g_console->get_text());
            }
            CHANGE = 1;
        }
    }

    FS_MusicPlay(NULL);

    GAMELOOP = 0;

    if (win || loss)
    {
        net->m_replay_file->close();
    
        if (net->gametype != GAME_TYPE_REPLAY && askmenu(_("Save replay?"))) {
            std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
                _("Save replay (*.replay file)"), F("$(home)"), "replay", true);
                
            if (!filename.empty()) {
                if (exists(filename.c_str()))
                    if (remove(filename.c_str()) != 0) {
                    	g_console->printf(_("Unable to delete existing file %s!"), filename.c_str());
                    	g_console->printf("%s (%d)", strerror(errno), errno);
                    	filename += "_";
                    }
                
                if (rename(F("$(home)/replay.tmp"), filename.c_str()) == 0) {
                    g_console->printf(_("Replay saved as %s"), filename.c_str());
                } else {
                    g_console->printf(_("Unable to save %s!"), filename.c_str());
                    g_console->printf("%s (%d)", strerror(errno), errno);
                }
            }
        }

        if (exists(F("$(home)/replay.tmp"))) {
            if (remove(F("$(home)/replay.tmp")) != 0) {
                g_console->printf(_("Unable to delete temporary file %s!"), F("$(home)/replay.tmp"));
                g_console->printf("%s (%d)", strerror(errno), errno);
            }
        }
    
        endgame_stats();
    }

    fade_out(10);

    net->send_quit();

    clear(screen);

    // Closes replay file (input or output according to mode, play or replay)
    delete net->m_replay_file;
    net->m_replay_file = NULL;
}

void faststart()
{
    HOST = sethotseatplay();

    //initgame();
    install_timers(speed_unit, speed_bullet, speed_mapscroll);

    reset_video();

    bool map_loaded = Map::load_GEODATA("$(home)/cur_map.lua", &mapdata);
    ASSERT(map_loaded);

    int fh = open(F("$(home)/cur_p1.dat"), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    read(fh, &pd1, sizeof(pd1));
    close(fh);

    fh = open(F("$(home)/cur_p2.dat"), O_RDONLY | O_BINARY);
    ASSERT(fh != -1);
    read(fh, &pd2, sizeof(pd2));
    close(fh);

    map = new Map(mapdata);
    elist = new Explosive();
    p1 = new Platoon(1111, &pd1, scenario->deploy_type[0]);
    p2 = new Platoon(2222, &pd2, scenario->deploy_type[1]);
    cur_random = new Random;

    elist->reset();
	
    platoon_local  = p1;
    platoon_remote = p2;
    MODE = MAP3D;
	
	// synchronize available equipment with ourselves :)
	lua_pushstring(L, "SyncEquipmentInfo");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "QueryEquipmentInfo");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_safe_call(L, 0, 1);
	lua_safe_call(L, 1, 0);
    
	lua_safe_dostring(L, "SetEquipment('Standard')");

    sel_man = platoon_local->captain();
    if (sel_man != NULL) map->center(sel_man);
    DONE = 0; TARGET = 0; turn = 0;

    clear_to_color(screen, 58);      //!!!!!
    initrand();
    gameloop();
    closegame();
}

void start_loadgame()
{
/* It fixes existing problem with crash when game is loaded after some other 
game have been played before. I don't know how to fix it in other way.*/
    win = 0; loss = 0; 

    HOST = sethotseatplay();

    install_timers(speed_unit, speed_bullet, speed_mapscroll);

    reset_video();

    // Todo: message for "version of savegame not compatible"
    if (!loadgame(F("$(home)/ufo2000.sav")))
    {
        alert( "", _("Saved game not available"), "", _("OK"), NULL, 0, 0);
        return;
    }
    battle_report( "# %s: %d\n", _("LOAD GAME"), turn );

    inithotseatgame();

    map->center(sel_man);
    DONE = 0;

    clear_to_color(screen, COLOR_GREEN10);
    gameloop();
    closegame();
}

/**
 * Dirty function which starts replay of the game
 */
void start_loadreplay()
{
/* It fixes existing problem with crash when game is loaded after some other 
game have been played before. I don't know how to fix it in other way.*/
    win = 0; loss = 0; 

    HOST = 0;
    net->gametype = GAME_TYPE_REPLAY;

    install_timers(speed_unit, speed_bullet, speed_mapscroll);

    reset_video();
    clear_to_color(screen, COLOR_BLACK1);

    char path[1000]; *path = 0;
    
    std::string filename = gui_file_select(SCREEN_W / 2, SCREEN_H / 2, 
        _("Load replay (*.replay files)"), F("$(home)"), "replay");
        
    if (filename.empty()) {
        alert( "", _("No saved replays found!"), "", _("OK"), NULL, 0, 0);
        return;
    }        
    
    if (!loadreplay(filename.c_str())) {
        alert( "", _("Replay is invalid!"), _("(Probably it was saved by incompatible version)."), _("OK"), NULL, 0, 0);
        return;
    }
    
    battle_report( "# %s: %d\n", _("START REPLAY"), turn );

    inithotseatgame();

    DONE = 0;

    MODE = WATCH;
    sel_man = NULL;

    gameloop();
    closegame();
}


/**
 * Test / Debug
 */
//! Test: Draw rectangles in all colors as background, 
//! labeled with white numbers, 
//! output to screen or save as bitmap-file.
void color_chart1()
{
    //int h = 32, w = 32;   // 32*16=512
    int h = 22, w = 38;
    int x =  0, y =  0, color = 0;
    BITMAP *xx = create_bitmap(16*w, 16*h);  // 24*16,38*16 = 384,608
    clear(xx);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            color = i * 16 + j;
            x     = j * w;
            y     = i * h;
            rectfill( xx, x, y, x + w-1, y + h-1, xcom_color(color) );
            //textprintf_centre_ex(xx, g_small_font,
            //    j * 32 + 16, i * 32 + 16,
            //    xcom_color(1), -1, "%d", i * 16 + j); 
            //textprintf_centre(xx, g_small_font, x + w/2, y + h/2,
            //    xcom_color(1), "%d", color );
        printsmall_x( xx, x + w/2, y + h/2, xcom_color(1), color );
        }
    }
    //save_bitmap("xcom_palette.bmp", xx, (RGB *)datafile[DAT_GAMEPAL_BMP].dat);
    //blit(xx, screen, 0, 0, 0, 0, SCREEN_W, SCREEN2H);
    blit( xx, screen, 0, 0, 0, 0, 16*w, 16*h );
    destroy_bitmap(xx); 
}

//! Test: Draw text in all colors on black background
void color_chart2()
{
    int color = 0, x = 0, y = 0, h = 22, w = 38;
    BITMAP *xx = create_bitmap(16*w, 16*h);  // 22*16,38*16=352,608
    clear(xx);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            color = i * 16 + j;
            x     = j * w;
            y     = i * h;
            //rectfill(xx, j*w, i*h, j*w + w-1, i*h + h-1, xcom_color(color) );
            //textprintf_centre(xx, g_small_font, 
            //    j*w + w/2, i*h + h/2,
            //    xcom_color(color), "%d", color );
        printsmall_x( xx, x + w/2, y + h/2, xcom_color(color), color );
        }
    }
    //save_bitmap("xcom_pal_text.bmp", xx, (RGB *)datafile[DAT_GAMEPAL_BMP].dat);
    //save_pcx(filename, scr, (RGB *)datafile[DAT_GAMEPAL_BMP].dat);
    blit( xx, screen, 0, 0, 0, 0, 16*w, 16*h );
    destroy_bitmap(xx); 
}

//! Test: Call test-routines
void test1()
{
    int DONE = 0, test = 0;

    textprintf(screen, font, 1, 31*12, COLOR_SYS_PROMPT, 
        "%s", "Colortest - press SPACE to switch, ESC to quit"); 

    while (!DONE) { 
        if (keypressed()) {
            int scancode;
            ureadkey(&scancode);

            switch (scancode) {
                case KEY_ESC:
                        //if (askmenu("Test ok"))
                        DONE = 1;
                    break;
                //case KEY_UP:
                //    resize_screen2(0, -10);
                //    break;
                default:
                    if (test == 0 ) {
                        color_chart1(); test++;
                    } else {
                        color_chart2(); test=0;
                    } 
            }
        }
    }
}

std::string g_version_id;

/**
 * Main UFO2000 - client-program
 */
int main(int argc, char *argv[])
{
    char version_id[128];
    if (strcmp(UFO_SVNVERSION, "unknown") == 0 || strcmp(UFO_SVNVERSION, "exported") == 0 || strcmp(UFO_SVNVERSION, "") == 0) {
        sprintf(version_id, "%s (revision >=%d)", UFO_VERSION_STRING, UFO_REVISION_NUMBER);
    } else {
        sprintf(version_id, "%s.%s", UFO_VERSION_STRING, UFO_SVNVERSION);
    }
    g_version_id = version_id;

    initmain(argc, argv);
    MouseRange *temp_mouse_range_ptr = new MouseRange(0, 0, SCREEN_W - 1, SCREEN_H - 1);

#ifdef WIN32
    prevExceptionFilter = SetUnhandledExceptionFilter(TopLevelExceptionFilter);
#endif

    lua_message( std::string("UFO2000 Version: ") + version_id );
    if (FLAGS & F_FASTSTART) {
        faststart();
    } else if (argc >= 3) {
        // skybuck: connect directly to game server if 4 command line arguments
        // actually just 3. (the 4th is the executable file path+name in argv[0]

        // try to auto login ;)
        g_server_autologin = 1;

        // skybuck: ufo2000 will automatically use argv[1] and argv[2] as
        // login name and password
        // only thing remaining to be done is send a auto challenge etc...
        // or simple start the game with the player...
        // but just logging in and skipping everything would be already good enough :D
        connect_internet_server();
    } else {
        // skybuck: otherwise just start/show main menu
        int mm = 2, h = -1;
        int b1 = 0;
        while ((mm = do_mainmenu()) != MAINMENU_QUIT) {
            h = -1;
            switch (mm) {
                case MAINMENU_ABOUT:
                    // $$$ TODO: show the about-box with the title-picture as background, 
                    // not on black screen
                    set_palette((RGB *)datafile[DAT_MENUPAL_BMP].dat);  // yellow mouse-cursor
                    char about1[128];
                    sprintf(about1, _("UFO2000 v%s.%s - a free and opensource multiplayer game"),
                                    UFO_VERSION_STRING, UFO_SVNVERSION);
                    b1 = alert(about1,
                          _("inspired by 'X-COM: UFO Defense', see http://ufo2000.sf.net."),
                          "(c) 2000-2001 A.Ivanov, (c) 2002-2004 ufo2000 development team",
                          _(" &MORE "), _(" OK "), 109, 0);    // 109: 'm'
                    if ( b1 == 1 ) 
                        help( HELP_INTRO );
                    break;
                case MAINMENU_TIP_OF_DAY:
                    set_palette((RGB *)datafile[DAT_MENUPAL_BMP].dat);  // yellow mouse-cursor
                    showtip();
                    break;
                case MAINMENU_EDITOR:
                    FS_MusicPlay(F(cfg_get_editor_music_file_name()));
//                  editor->do_mapedit();
                    set_palette((RGB *)datafile[DAT_GAMEPAL_BMP].dat);
                    gui_fg_color = COLOR_BLACK1;
                    gui_bg_color = COLOR_WHITE; 
                    alert(" ", "Map editor is currently disabled", " ", "    OK    ", NULL, 1, 0);
                    FS_MusicPlay(NULL);
                    continue;
                case MAINMENU_HOTSEAT:
                    h = sethotseatplay();
                    break;
                case MAINMENU_INTERNET:
                    h = connect_internet_server();
                    break;
                case MAINMENU_LOADGAME:
                    start_loadgame();
                    break;
                case MAINMENU_SHOW_REPLAY:
                    start_loadreplay();
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

    delete temp_mouse_range_ptr;

#ifdef WIN32
    SetUnhandledExceptionFilter(prevExceptionFilter);
#endif

    closemain();
    return 0;
}
END_OF_MAIN();
