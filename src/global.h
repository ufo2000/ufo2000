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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef LINUX
#include <io.h>
#include <conio.h>
#endif

#ifdef LINUX
#include <sys/stat.h>
inline long filelength(int handle) { struct stat s; fstat(handle, &s); return s.st_size; }
#endif

#include <assert.h>
#include <allegro.h>
#ifdef __MINGW32__
#include <winalleg.h>
#endif

#include "jpgalleg/jpgalleg.h"

extern "C" {
#include "agup/agup.h"
}

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

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "LuaPlus/LuaPlusCD.h"

#define LUA_REGISTER_CLASS(L, classname) \
	lua_pushstring(L, #classname); \
	lua_newtable(L); \
	lua_pushstring(L, "__index"); \
	lua_pushvalue(L, -2); \
	lua_settable(L, -3); \
	lua_settable(L, LUA_GLOBALSINDEX); \
	
#define LUA_REGISTER_CLASS_METHOD(L, classname, methodname) \
	lua_pushstring(L, #classname); \
	lua_gettable(L, LUA_GLOBALSINDEX); \
	lua_pushstring(L, #methodname); \
	lua_pushobjectdirectclosure(L, (classname*)0, &classname::methodname, 0); \
	lua_settable(L, -3); \
	lua_pop(L, 1);

#define LUA_PUSH_OBJECT_POINTER(L, classname, objectptr) \
	lua_pushstring(L, #classname); \
	lua_gettable(L, LUA_GLOBALSINDEX); \
	lua_boxpointer(L, objectptr); \
	lua_pushvalue(L, -2); \
	lua_setmetatable(L, -2); \
	lua_remove(L, -2);

#define LUA_REGISTER_FUNCTION(L, functionname) \
	lua_pushstring(L, #functionname); \
	lua_pushdirectclosure(L, functionname, 0); \
	lua_settable(L, LUA_GLOBALSINDEX); \

int lua_safe_call(lua_State *L, int narg, int nret);
int lua_safe_dofile(lua_State *L, const char *name);
int lua_safe_dobuffer(lua_State *L, const char *buff, size_t size, const char *name);
int lua_safe_dostring(lua_State *L, const char *str);

#include <nl.h>

// We rely on HawkNL in defining data types of proper system independent size
typedef NLbyte int8;
typedef NLshort int16;
typedef NLlong int32;
typedef NLubyte uint8;
typedef NLushort uint16;
typedef NLulong uint32;

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
#define map ufo2000_map

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

#include <math.h>
#define REAL  float
#ifndef PI
#define PI    3.141592654
#endif

#include "explo.h"

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

struct MANDATA
{
	int16         Rank;         //!< Rank  == -1 --> Not used
	int16         Base;         //!< Base  == -1 --> Transfer in progress
	int16         Craft;        //!< Craft == -1 --> Not on any craft
	int16         OldCraft;
	uint16        Missions;     //!< Number of missions soldier was on
	uint16        Kills;        //!< Number of kills soldier scored in his life
	uint16        Recovery;     //!< The number of days before their injuries are gone.
	uint16        DeathCost;    //!< The number of points you lose when they die.
#define MAN_NAME_LEN 22
	char          Name[26];     //!< There are actually 26 bytes allocated for this, but only the first 23 are used.  The names can be up to 22 bytes.
	unsigned char TimeUnits;    //!< TU each turn for actions
	unsigned char Health;       //!< Hitpoints: when down to 0, soldier dies
	unsigned char Stamina;      //!< Actions like walking consume TU as well as energy
	unsigned char Reactions;    //!< Gives chances for reaction-fire
	unsigned char Strength;     //!< How much the soldier can carry without strain
	unsigned char Firing;       //!< Accuracy at firing weapons
	unsigned char Throwing;     //!< Accuracy at throwing (Grenades etc.)
	unsigned char Close;        //!< Close-combat accuracy
	unsigned char PsiStrength;  //!< (Psi-combat not yet implemented)
	unsigned char PsiSkill;     //!< (Psi-combat not yet implemented)
	unsigned char Bravery;      //!< Bravery = ( 11 - x ) * 10
	unsigned char TimeUnitsImp; //!< Improvement in TU after a battle
	unsigned char HealthImp;    //!< Improvement in Health after a battle
	unsigned char StaminaImp;   //!< Improvement in Stamina after a battle
	unsigned char ReactionsImp; //!< Improvement in Reactions after a battle
	unsigned char StrengthImp;  //!< Improvement in Strength after a battle
	unsigned char FiringImp;    //!< Improvement in FiringAcc after a battle
	unsigned char ThrowingImp;  //!< Improvement in ThrowingAcc after a battle
	unsigned char CloseImp;     //!< Improvement in CloseCombat after a battle
	unsigned char BraveryImp;   //!< * 10
	unsigned char SkinType;     //!< 0=none
	unsigned char PsiImprove;   //!< The psionic improvement over the course of the last month
	unsigned char fPsiTraining; //!< 0=not in training  1=in psi training
	unsigned char Promoted;     //!< After a combat, this is set to 1 if they were promoted, 0 if not.
	unsigned char fFemale;      //
	unsigned char Appearance;   //
};

struct UNITDATA
{
	unsigned char Picture;      //!< The picture type used on the tactical view
	unsigned char BigPicture;   //!< The armour type they're wearing on the inventory screen.
	unsigned char u3;
	unsigned char u4;
	unsigned char u5;
	unsigned char u6;
	unsigned char u7;
	unsigned char u8;
	unsigned char u9;
	unsigned char u10;
	unsigned char Facing;       //!< The direction the unit is facing:  00=North, 01=North East, and so on up to 7.
	unsigned char u12;
	unsigned char CurTU;        //!< The number of TUs the unit has currently
	unsigned char CurHealth;    //!< Current Health
	unsigned char CurStun;      //!< The stun level: if this is higher than CurHealth, they're stunned.
	unsigned char CurEnergy;    //!< Current Energy
	unsigned char CurReactions; //!< Current Reaction
	unsigned char CurStrength;  //!< Current Strength
	unsigned char CurFront;     //!< Current armor on the front
	unsigned char CurLeft;      //!< Current armor at the left side
	unsigned char CurRight;     //!< Current armor at the right side
	unsigned char CurRear;      //!< Current armor on the rear
	unsigned char CurUnder;     //!< Current armor at the bottom
	unsigned char CurFAccuracy; //!< current Firing Accuracy (might go down when wounded)
	unsigned char CurTAccuracy; //!< current Throwing Accuracy
	unsigned char MaxTU;        //!< The maximum number of TUs the unit has
	unsigned char MaxHealth;    //!< Maximum Health
	unsigned char MaxEnergy;    //!< Maximum Energy
	unsigned char MaxStrength;  //!< Maximum Strength
	unsigned char MaxFront;     //!< Maximum armor on the front
	unsigned char MaxLeft;      //!< Maximum armor at the left side
	unsigned char MaxRight;     //!< Maximum armor at the right side
	unsigned char MaxRear;      //!< Maximum armor on the rear
	unsigned char MaxUnder;     //!< Maximum armor at the bottom
	unsigned char MaxFA;        // ?
	unsigned char MaxTA;        // ?
	unsigned char u37;
	unsigned char PsiSkill;     //!< 
	unsigned char ItemDie;      //!< The item type which is created when the unit dies
	unsigned char u40;
	unsigned char SoldierNo;    //!< The entry # in soldier.dat which this unit equals.  FF means they were created for this fight (they're alien, tank, or civilian).
	unsigned char u41;
	unsigned char Rank;         //!< The soldier rank for humans/aliens or, on tanks, the body type (0=tank 1=hovertank)
	unsigned char ARank;        //!< The alien rank, for research purposes (if you stun them)
	unsigned char u45;
	unsigned char EnergyUse;    //!< If it's set to 255 no energy is used, if it's 1 then half as much energy as TUs is used, and if it's anything else then as much energy as TUs is used.
	unsigned char Backpack;     //!< In humans: 0=normal backpack 1=large backpack  In tanks: 'head' type--0=cannon, 1=rocket, 2=laser, and so on.
	unsigned char MIAScore;     //!< The number of points lost if they're Missing in Action
	unsigned char u49;
	unsigned char UnitBottom;   //!< The bottom (in pixels above ground, I think) of the unit.  Used for collisions with bullets AND in drawing the picture (they are shifted up this much).
	unsigned char u51;
	unsigned char UnitHeight;   //!< The number of pixels tall (I think) the unit is, for same purpose as UnitBottom
	unsigned char u53;
	unsigned char ItemLeft;     //!< The item type in the left hand--used only on the tactical screen (not used in the inventory screen)
	unsigned char ItemRight;    //!< The item type in the right hand
	unsigned char u56;
	unsigned char u57;
	unsigned char PsiStrength;
	unsigned char Morale;
	unsigned char u60;
	unsigned char u61;
	unsigned char u62;
	unsigned char u63;
	unsigned char HeadWound;    //!< Number of fatal wounds in the head.
	unsigned char TorsoWound;
	unsigned char RArmWound;    //!< Right arm wounds
	unsigned char LArmWound;
	unsigned char RLegWound;
	unsigned char LLegWound;
	unsigned char u70;
	unsigned char u71;
	unsigned char u72;
	unsigned char u73;
	unsigned char u74;
	unsigned char u75;
	unsigned char u76;
	unsigned char u77;
	unsigned char Missions;     //!< Number of missions so far this mission ... !!?
	unsigned char Kills;        //!< Number of kills so far this mission
	unsigned char u80;
	unsigned char u81;
	unsigned char u82;
	unsigned char u83;
	unsigned char u84;
	unsigned char u85;
	unsigned char u86;
	char          Name[25];     //!< The unit name!!
	unsigned char u112;
	unsigned char u113;
	unsigned char u114;
	unsigned char u115;
	unsigned char Gender;       //!< used in the inventory picture
	unsigned char SkinColor;    //!< the 'Appearance' stat from SOLDIER.DAT
	unsigned char AttachedWep;
	unsigned char AttachedAmmo; //!< The number of rounds left in the AttachedWep
	unsigned char Flags;        //!< if (Flags&&2) the unit can fly, otherwise it can't.
	unsigned char u118;
	unsigned char u119;         //!< skin_type;  armour_type
	unsigned char u120;         //!< MOVED; // already moved this turn
};

struct ITEMDATA
{
	char  name[26];
	int32 num;
	char  type[100];
	char  place[100];
	char  x[100];
	char  y[100];
};

struct PLAYERDATA
{
	int32    size;
	MANDATA  md[20];
	ITEMDATA id[20];
	int32    lev[20], col[20], row[20];
};

//////////////////////////////////////////////////////////////////////////////
//  Definitions of all global variables from the game. There are a lot of   //
//  unneeded global variables that should be removed in the future.         //
//////////////////////////////////////////////////////////////////////////////

class Map;
class TerrainSet;
class Platoon;
class Net;
class Wind;
class PCK;
class Soldier;
class Scenario;

//! Display-Modes
enum Mode { MAP2D, MAP3D, MAN, WATCH, UNIT_INFO, PLANNER };

//! possible Stati for reserve-time - buttons
// extern int ReserveTimeMode;
// #define RESERVE_FREE 0
// #define RESERVE_AIM  1
// #define RESERVE_SNAP 2
// #define RESERVE_AUTO 3
extern enum ReserveTime_Mode { RESERVE_FREE, RESERVE_AIM, RESERVE_SNAP, RESERVE_AUTO } ReserveTimeMode;
// TODO: ReserveTime should be platoon- or soldier-specific

extern volatile int CHANGE;
extern Map *map;
extern Scenario *scenario;
extern TerrainSet *terrain_set;
extern std::set<std::string> g_net_allowed_terrains;
extern Platoon *platoon_local, *platoon_remote, *p1, *p2;
extern Explosive *elist;
extern Net *net;
extern int BLOODYMENU;
extern int SOUND;
extern int MODSOUND;
extern int digvoices;
extern int modvoices;
extern int modvol;
extern char modname[128];
extern int HOST;
extern char HOSTNAME[256];
extern char PORT[256];
extern int MAP_WIDTH, MAP_HEIGHT;

extern int local_platoon_size;
extern char last_map_name[1000];
extern char last_unit_name[1000];

extern Editor *editor;

extern int HOST;
extern GEODATA mapdata;
extern PLAYERDATA pd1, pd2;
extern PLAYERDATA *pd_local, *pd_remote;
extern Target target;
extern int TARGET;
extern Soldier *sel_man;
extern Mode MODE;
extern int turn;
extern int CONFIRM_REQUESTED;
extern DATAFILE *datafile;
extern int NOTICEremote;
extern Wind *info_win;
extern Icon *icon;
extern int g_time_limit;

extern volatile int g_time_left;

extern volatile int g_switch_in_counter;
class ConsoleStatusLine;
extern ConsoleStatusLine *g_status;
class ConsoleWindow;
extern ConsoleWindow *g_console;

bool loadgame(const char *filename);
bool loadgame_stream(std::iostream &stream);
void savegame(const char *filename);
void savegame_stream(std::iostream &stream);

void check_crc(int crc);
void recv_turn(int crc);
void send_turn();

bool nomoves();
int connect_internet_server();
void display_error_message(const std::string &error_text);

extern std::string g_version_id;
extern std::string g_server_host;
extern std::string g_server_login;
extern std::string g_server_password;
extern int         g_server_autologin;

int initgame();
void gameloop();
void closegame();

extern lua_State *L;

const char *F(const char *fileid);

#define CELL_SCR_X 16
#define CELL_SCR_Y 8
#define CELL_SCR_Z 24

#include "../ufo2000.h"

#endif
