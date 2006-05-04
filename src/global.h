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

#ifndef GLOBAL_H
#define GLOBAL_H

#define d_box_proc d_agup_box_proc
#define d_shadow_box_proc d_agup_shadow_box_proc
#define d_button_proc d_agup_button_proc
#define d_push_proc d_agup_push_proc
#define d_check_proc d_agup_check_proc
#define d_radio_proc d_agup_radio_proc
#define d_icon_proc d_agup_icon_proc
#define d_edit_proc d_agup_edit_proc
#define d_list_proc d_agup_list_proc
#define d_text_list_proc d_agup_text_list_proc
#define d_textbox_proc d_agup_textbox_proc
#define d_slider_proc d_agup_slider_proc
#define d_menu_proc d_agup_menu_proc
#define d_window_proc d_agup_window_proc
#define d_text_proc d_agup_text_proc
#define d_ctext_proc d_agup_ctext_proc
#define d_rtext_proc d_agup_rtext_proc
#define d_clear_proc d_agup_clear_proc

#define LUA_REGISTER_CLASS(L, classname) \
    { \
        classname *dummy; \
        lua_pushstring(L, typeid(dummy).name()); \
        lua_newtable(L); \
        lua_pushstring(L, "__index"); \
        lua_pushvalue(L, -2); \
        lua_settable(L, -3); \
        lua_settable(L, LUA_GLOBALSINDEX); \
    }
    
#define LUA_REGISTER_CLASS_METHOD(L, classname, methodname) \
    { \
        classname *dummy; \
        lua_pushstring(L, typeid(dummy).name()); \
        lua_gettable(L, LUA_GLOBALSINDEX); \
        lua_pushstring(L, #methodname); \
        lua_pushobjectdirectclosure(L, (classname*)0, &classname::methodname, 0); \
        lua_settable(L, -3); \
        lua_pop(L, 1); \
    }

#define LUA_PUSH_OBJECT_POINTER(L, objectptr) \
    { \
        lua_pushstring(L, typeid(objectptr).name()); \
        lua_gettable(L, LUA_GLOBALSINDEX); \
        lua_boxpointer(L, objectptr); \
        lua_pushvalue(L, -2); \
        lua_setmetatable(L, -2); \
        lua_remove(L, -2); \
    }

#define LUA_REGISTER_FUNCTION(L, functionname) \
    lua_pushstring(L, #functionname); \
    lua_pushdirectclosure(L, functionname, 0); \
    lua_settable(L, LUA_GLOBALSINDEX); \

int lua_safe_call(lua_State *L, int narg, int nret);
int lua_safe_dofile(lua_State *L, const char *name, const char *env_name = NULL);
int lua_safe_dobuffer(lua_State *L, const char *buff, size_t size, const char *name);
int lua_safe_dostring(lua_State *L, const char *str);

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;

#if UINT_MAX == 0xFFFFFFFFUL
typedef signed int int32;
typedef unsigned int uint32;
#elif ULONG_MAX == 0xFFFFFFFFUL
typedef signed long int32;
typedef unsigned long uint32;
#else
#error Can not define int32 type
#endif

inline uint16 intel_uint16(uint16 x)
{
#ifdef ALLEGRO_BIG_ENDIAN
    return (uint16)(((((uint16)x) & 0x00ff) << 8) | ((((uint16)x) & 0xff00) >> 8));
#else
    return x;
#endif
}

inline uint32 intel_uint32(uint32 x)
{
#ifdef ALLEGRO_BIG_ENDIAN
    return (uint32)(((((uint32)x) & 0x000000ff) << 24) | ((((uint32)x) & 0x0000ff00) << 8) | 
        ((((uint32)x) & 0x00ff0000) >> 8) | ((((uint32)x) & 0xff000000) >> 24));
#else
    return x;
#endif
}

inline int16 intel_int16(int16 x) { return (int16)intel_uint16((uint16)x); }
inline int32 intel_int32(int32 x) { return (int32)intel_uint32((uint32)x); }

#include "persist.h"
#define map g_map

#include <set>

inline void PersistWriteBinary(persist::Engine &archive, const void *data, int size)
{
    archive.WriteBinary((const uint8 *)data, size);
}

inline void PersistReadBinary(persist::Engine &archive, void *data, int size)
{
    archive.ReadBinary((uint8 *)data, size);
}

template<class TYPE> void PersistWriteObject(persist::Engine &archive, TYPE &object)
{
    archive.Write(static_cast<persist::BaseObject *>(object));
}

template<class TYPE> void PersistReadObject(persist::Engine &archive, TYPE &object)
{
    persist::BaseObject *ptr;
    archive.Read(ptr);
    object = static_cast<TYPE>(ptr);
}

/**
 * Function that saves object data (as a piece of binary data)
 * 
 * @param archive persistence support object
 * @param object  object to be saved
 *
 * @todo now uses i386 only and also compiler specific skip of VTBL pointer, 
 * needs to be fixed in the future
 */
template<class TYPE> void PersistWriteBinary(persist::Engine &archive, TYPE &object)
{
    ASSERT(static_cast<const persist::BaseObject *>(&object));
    archive.WriteBinary((const uint8 *)&object + sizeof(void *), sizeof(object) - sizeof(void *));
}

/**
 * Function that restores object data (as a piece of binary data)
 * 
 * @param archive persistence support object
 * @param object  object to be restored
 *
 * @todo now uses i386 only and also compiler specific skip of VTBL pointer, 
 * needs to be fixed in the future
 */
template<class TYPE> void PersistReadBinary(persist::Engine &archive, TYPE &object)
{
    ASSERT(static_cast<persist::BaseObject *>(&object));
    archive.ReadBinary((uint8 *)&object + sizeof(void *), sizeof(object) - sizeof(void *));
}

//////////////////////////////////////////////////////////////////////////////
//  Data types from original UFO:EU                                         //
//////////////////////////////////////////////////////////////////////////////

struct GEODATA
{
    int    terrain;      //!< Terrain set index
    int    x_size;       //!< Distance e/w in tens of tiles
    int    y_size;       //!< Distance n/s in tens of tiles
    int    z_size;       //!< Height of map (tiles)
    int    load_game;    //!< Flag which indicates that the map should be reloaded
    uint8  mapdata[36];  //!< The actual map data - refers to the number at the end of map name; i.e. urban12 would be number
};

#pragma pack(1)
struct MANDATA
{
#define MAN_NAME_LEN 22
    char          Name[26];     //!< There are actually 26 bytes allocated for this, but only the first 23 are used.  The names can be up to 22 bytes.
    unsigned char TimeUnits;    //!< TU each turn for actions
    unsigned char Health;       //!< Hitpoints: when down to 0, soldier dies
    unsigned char Stamina;      //!< Actions like walking consume TU as well as energy
    unsigned char Reactions;    //!< Gives chances for reaction-fire
    unsigned char Strength;     //!< How much the soldier can carry without strain
    unsigned char Firing;       //!< Accuracy at firing weapons
    unsigned char Throwing;     //!< Accuracy at throwing (Grenades etc.)
    unsigned char Bravery;      //!< Bravery
    unsigned char SkinType;
    unsigned char fFemale;
    unsigned char Appearance;
};
#pragma pack()

#pragma pack(1)
struct ITEMDATA
{
    uint8  num;
    uint32 item_type[100];
    uint8  place[100];
    uint8  x[100];
    uint8  y[100];
};
#pragma pack()

#pragma pack(1)
struct PLAYERDATA
{
    uint8    size;
    MANDATA  md[20];
    ITEMDATA id[20];
    uint8    lev[20], col[20], row[20];
};
#pragma pack()

//////////////////////////////////////////////////////////////////////////////
//  Definitions of all global variables from the game. There are a lot of   //
//  unneeded global variables that should be removed in the future.         //
//////////////////////////////////////////////////////////////////////////////

class Map;
class TerrainSet;
class Platoon;
class Editor;
struct Target;
class Icon;
class Net;
class Wind;
class PCK;
class Position;
class Soldier;
class Scenario;
class Explosive;
class Random;
class Cell;

//! Display-Modes
enum Mode { MAP2D, MAP3D, MAN, WATCH, UNIT_INFO, PLANNER };

enum Action { NONE, THROW, PRIME, SNAPSHOT, AIMEDSHOT, AUTOSHOT, PUNCH, AIMEDTHROW };

enum DeployType {DEP_LEFT, DEP_RIGHT, DEP_CENTER, DEP_SURROUND, DEP_ALL};

//! possible Stati for reserve-time - buttons
// extern int ReserveTimeMode;
 #define RESERVE_FREE 0
 #define RESERVE_AIM  1
 #define RESERVE_SNAP 2
 #define RESERVE_AUTO 3
// TODO: ReserveTime should be platoon- or soldier-specific

extern volatile int CHANGE;
extern Map *map;
extern Scenario *scenario;
extern TerrainSet *terrain_set;
extern std::set<std::string> g_net_allowed_terrains;
extern Platoon *platoon_local, *platoon_remote, *p1, *p2;
extern Explosive *elist;
extern Random *cur_random;
extern Net *net;
extern int BLOODYMENU;
extern int SOUND;
extern int MODSOUND;
extern int digvoices;
extern int modvoices;
extern int modvol;
extern int mouse_sens;
extern char modname[128];
extern int HOST;
extern char HOSTNAME[256];
extern char PORT[256];
extern int MAP_WIDTH, MAP_HEIGHT;

extern int local_platoon_size;
extern char last_unit_name[1000];

// gettext-translation deactivated / active:
//#define _(String) (String)
#define _(String) ufo2k_gettext(String)

extern const char *place_name[11];

extern Editor *editor;

extern int HOST;
extern GEODATA mapdata;
extern PLAYERDATA pd1, pd2;
extern PLAYERDATA *pd_local, *pd_remote;
extern Target target;
extern int TARGET;
extern Soldier *sel_man;
extern Mode MODE;
extern int g_pause;
extern int g_game_receiving;
extern int g_fast_forward;
// id of a last received or sended packet to use in the debug info.
extern int g_current_packet_num;
//position of player who send currently processed packet to use in the debug info.
extern int g_current_packet_pos;

extern int debug_save_state_sender;
extern int debug_save_state_id;

extern int turn;
extern int CONFIRM_REQUESTED;
extern DATAFILE *datafile;
extern int NOTICEremote;
extern Wind *info_win;
extern Icon *icon;
extern int g_time_limit;
extern int g_p2_start_sit;

extern volatile int g_time_left;

extern volatile int g_switch_in_counter;
class ConsoleStatusLine;
extern ConsoleStatusLine *g_status;
class ConsoleWindow;
extern ConsoleWindow *g_console;

bool loadgame(const char *filename, int compress = 0);
bool loadgame_stream(std::istream &stream);
void savegame(const char *filename, int compress = 0);
void savegame_stream(std::ostream &stream);

void check_crc(int crc);
void recv_turn(int crc, const std::string &data);
void send_turn();

bool nomoves();
int connect_internet_server();
void display_error_message(const std::string &error_text, bool do_not_terminate = false);
int file_select_mr(const char *message, char *path, const char *ext);
void report_game_error(int chk);

extern std::string g_version_id;
extern std::string g_server_host;
extern std::string g_server_login;
extern std::string g_server_password;
extern std::string g_server_proxy_login;
extern int         g_server_autologin;

extern std::string g_default_weaponset;

int initgame();
void gameloop();
void closegame();

void geoscape();

extern lua_State *L;

const char *F(const char *fileid);
std::string lua_escape_string(const std::string &str);
std::string indent(const std::string &);
bool check_filename_case_consistency(const char *filename);

#define CELL_SCR_X 16
#define CELL_SCR_Y 8
#define CELL_SCR_Z 24

#include "../ufo2000.h"

/**
 * @defgroup planner Mission planner
 * Soldiers management, map and game rules selection, many other
 * battle preparation things
 */

/**
 * @defgroup network Network support
 */

/**
 * @defgroup gui User interface
 */

/**
 * @defgroup battlescape Battlescape 
 */
 
#endif
