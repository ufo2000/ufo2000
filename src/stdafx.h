#ifndef __STDAFX_H__
#define __STDAFX_H__

//Compile-time messages
//usage: #pragma chMSG("Hello")
#define chSTR2(x) #x
#define chSTR(x) chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef LINUX
#include <io.h>
#include <conio.h>
#endif

#include "jpgalleg/jpgalleg.h"
#include "fpasprite/fpasprite.h"

extern "C" {
#include "agup/agup.h"
}


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <time.h>
#include <signal.h>
#include <stdarg.h>

#include <allegro.h>
#include <allegro/internal/aintern.h>
#ifdef WIN32
#include <winalleg.h>
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef LINUX
#include <unistd.h>
inline long filelength(int handle) { struct stat s; fstat(handle, &s); return s.st_size; }
#endif


#include <fcntl.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <set>
#include <memory>

#include "fdlibm/fdlibm.h"
using namespace fdlibm;

// Provide some defines to override functions from "math.h" as "math.h"
// gets included automatically by "iostream" header file in linux for
// example, so this workaround is required
#define sin(x) fdlibm::sin(x)
#define cos(x) fdlibm::cos(x)
#define sqrt(x) fdlibm::sqrt(x)
#define acos(x) fdlibm::acos(x)
#define atan2(x, y) fdlibm::atan2(x, y)
#define floor(x) fdlibm::floor(x)
#define ceil(x) fdlibm::ceil(x)

#define REAL  double
#define PI    3.1415926535897932384626433832795  // should be accurate enough

#define STDBUFSIZE 128 // Default buffer size for string messages to output

#include <cstring>

#include <expat.h>

#include "sysworkarounds.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "LuaPlus/LuaPlusCD.h"


/**
 * Standard game errors are coded with these constants.
 */
enum GameErrorCodes {OK = 0, ERR_NO_TUS = -1, ERR_NO_ENERGY = -2,
    ERR_NO_AMMO = -3, ERR_NO_ITEM = -4, ERR_DISTANCE = -5, ERR_MINUS_NUM = -6};
    // ERR_MINUS_NUM should have the most negative value.

const int SQUAD_LIMIT = 20;

#endif //__STDAFX_H__
