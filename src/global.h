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
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

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
	archive.WriteBinary((const uint8 *)&object + 4, sizeof(object) - 4);
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
	archive.ReadBinary((uint8 *)&object + 4, sizeof(object) - 4);
}

#ifdef FIXED_POINT_MATH

#define REAL  fix
#define PI    128

inline fix floor(fix x)
{
	fix t; t.v = fixfloor(x.v); return t;
}
inline fix ceil(fix x)
{
	fix t; t.v = fixceil(x.v); return t;
}
#else

#include <math.h>
#define REAL  float
#ifndef PI
#define PI    3.141592654
#endif

#endif

#include "explo.h"

//////////////////////////////////////////////////////////////////////////////
//  Data types from original UFO:EU                                         //
//////////////////////////////////////////////////////////////////////////////

struct GEODATA
{
	unsigned short x_size;      //!< Distance e/w in tens of tiles
	unsigned short y_size;      //!< Distance n/s in tens of tiles
	unsigned short z_size;      //!< Height of map (tiles)
	unsigned short ship[2];     //!< The types of each of the two ships
	unsigned short terrain;     //!< Terrain set; refers to the MCD combo sets ---  A JUNGLE, FOREST, etc...
	unsigned char mapdata[36];  //!< The actual map data - refers to the number at the end of map name; i.e. urban12 would be number
	unsigned char use_rmp[36];  //!< Tells it if it should use the RMPs of each square(if no, there will be no units in that square)
	unsigned char ship1_y;      //!< These coords are given in tiles
	unsigned char ship1_x;
	unsigned char ship2_y;
	unsigned char ship2_x;
	unsigned char load_game;    //!< If this is on, then the game is supposed to use the data from the GAME_xx directory
};

struct MANDATA
{
	short          Rank;        //!< Rank  == -1 --> Not used
	short          Base;        //!< Base  == -1 --> Transfer in progress
	short          Craft;       //!< Craft == -1 --> Not on any craft
	short int      OldCraft;
	unsigned short Missions;
	unsigned short Kills;
	unsigned short Recovery;    //!< The number of days before their injuries are gone.
	unsigned short DeathCost;   //!< The number of points you lose when they die.
	char Name[26];              //!< There are actually 26 bytes allocated for this, but only the first 23 are used.  The names can be up to 22 bytes.
	unsigned char TimeUnits;
	unsigned char Health;
	unsigned char Stamina;
	unsigned char Reactions;
	unsigned char Strength;
	unsigned char Firing;
	unsigned char Throwing;
	unsigned char Close;        //!< Close combat accuracy
	unsigned char PsiStrength;
	unsigned char PsiSkill;
	unsigned char Bravery;      //!< Bravery = ( 11 - x ) * 10
	unsigned char TimeUnitsImp;
	unsigned char HealthImp;
	unsigned char StaminaImp;
	unsigned char ReactionsImp;
	unsigned char StrengthImp;
	unsigned char FiringImp;
	unsigned char ThrowingImp;
	unsigned char CloseImp;
	unsigned char BraveryImp;   //!< * 10
	unsigned char SkinType;     //!< 0=none
	unsigned char PsiImprove;   //!< The psionic improvement over the course of the last month
	unsigned char fPsiTraining; //!< 0=not in training  1=in psi training
	unsigned char Promoted;     //!< After a combat, this is set to 1 if they were promoted, 0 if not.
	unsigned char fFemale;
	unsigned char Appearance;
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
	unsigned char CurHealth;
	unsigned char CurStun;      //!< The stun level: if this is higher than CurHealth, they're stunned.
	unsigned char CurEnergy;
	unsigned char CurReactions;
	unsigned char CurStrength;
	unsigned char CurFront;
	unsigned char CurLeft;
	unsigned char CurRight;
	unsigned char CurRear;
	unsigned char CurUnder;
	unsigned char CurFAccuracy; //!< Firing Accuracy
	unsigned char CurTAccuracy; //!< Throwing Accuracy
	unsigned char MaxTU;
	unsigned char MaxHealth;
	unsigned char MaxEnergy;
	unsigned char MaxStrength;
	unsigned char MaxFront;
	unsigned char MaxLeft;
	unsigned char MaxRight;
	unsigned char MaxRear;
	unsigned char MaxUnder;
	unsigned char MaxFA;        // ?
	unsigned char MaxTA;        // ?
	unsigned char u37;
	unsigned char PsiSkill;
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
	char Name[25];              //!< The unit name!!
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
	char name[26];
	int num;
	char type[100];
	char place[100];
	char x[100];
	char y[100];
};

struct PLAYERDATA
{
	int size;
	MANDATA md[20];
	ITEMDATA id[20];
	int lev[20], col[20], row[20];
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

enum Mode { MAP2D, MAP3D, MAN, WATCH, UNIT_INFO, PLANNER };

extern volatile int CHANGE;
extern Map *map;
extern TerrainSet *terrain_set;
extern std::set<int> g_net_allowed_terrains;
extern Platoon *platoon_local, *platoon_remote;
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
extern PCK *bigobs;
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
