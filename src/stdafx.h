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

extern "C" {
#include "agup/agup.h"
}


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <time.h>
#include <nl.h>
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

#include <math.h>
#define REAL  float
#ifndef PI
#define PI    3.141592654
#endif


#include <cstring>


#include <expat.h>

#include "sysworkarounds.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "LuaPlus/LuaPlusCD.h"


#ifdef WIN32
#define usleep(t) Sleep((t + 999) / 1000)
#endif




#endif //__STDAFX_H__


