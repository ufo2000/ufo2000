##############################################################################
# Compiling ufo2000: make {debug=1} {xmingw=1} {dumbogg=1}                   #
#                                                                            #
# Define xmingw=1 when compiling win32 binary with Mingw gcc crosscompiler   #
# Define debug=1 when you want to build debug version of ufo2000             #
#                                                                            #
# Just typing 'make' builds the release version of ufo2000 for *nix          #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# Type 'make server' to build ufo2000 server                                 #
#                                                                            #
# The game depends on Allegro (4.0.x), Expat, HawkNL and Lua libraries,      #
# so you need to install them before running make                            #
#                                                                            #
# DUMB and Ogg Vorbis are optional (they allow to play music in XM, S3M,     #
# MOD, IT and OGG formats). Use dumbogg=1 in make command line to build      #
# ufo2000 with these libraries.                                              #
#                                                                            #
# When compiling the game with Mingw (either native or a crosscompiler), it  #
# is possible to use a set of precompiled libraries. Just download zip from  #
# http://ufo2000.lxnt.info/files/mingw-libs.zip and extract it into ufo2000  #
# sources directory.                                                         #
#                                                                            #
# Also it is highly recommended but not necessery to have subversion         #
# client installed (it is required if you want to make 'source-zip' and      #
# 'source-bz2' targets)                                                      #
##############################################################################

UFO_SVNVERSION := ${shell svnversion .}
UFO_VERSION := ${shell awk 'BEGIN {FS="\""} /UFO_VERSION_STRING/ { print $$2; }' src/version.h}
ifneq ($(UFO_VERSION),)
ifneq ($(UFO_SVNVERSION),)
	DISTNAME := ufo2000-$(UFO_VERSION).$(UFO_SVNVERSION)
else
	DISTNAME := ufo2000-$(UFO_VERSION)
endif
else
	DISTNAME := ufo2000
endif

CX = g++
CC = gcc
CFLAGS = -funsigned-char -Wall -Wno-deprecated-declarations -DDEBUGMODE
OBJDIR = obj
NAME = ufo2000
SERVER_NAME = ufo2000-srv

ifneq ($(UFO_SVNVERSION),)
	CFLAGS += -DUFO_SVNVERSION=\"$(UFO_SVNVERSION)\"
endif

ifdef DATA_DIR
	CFLAGS += -DDATA_DIR=\"$(DATA_DIR)\"
endif

ifndef OPTFLAGS
	OPTFLAGS = -O2 -pipe
endif

ifdef WINDIR
	win32 = 1
endif

ifdef xmingw
    CX = i386-mingw32msvc-g++
    CC = i386-mingw32msvc-gcc
    win32 = 1
endif

VPATH = src src/jpgalleg src/dumbogg src/exchndl src/agup

SRCS = bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp           \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       multiplay.cpp packet.cpp pck.cpp place.cpp                     \
       platoon.cpp soldier.cpp sound.cpp spk.cpp terrapck.cpp         \
       units.cpp video.cpp wind.cpp crc32.cpp persist.cpp             \
       jpgalleg.c decode.c encode.c io.c minimap.cpp about.cpp        \
       stats.cpp server_protocol.cpp server_transport.cpp             \
       server_gui.cpp server_config.cpp music.cpp scenario.cpp        \
       aalg.c aase.c abeos.c abitmap.c agtk.c agup.c ans.c            \
       aphoton.c awin95.c

SRCS_SERVER = server_main.cpp server_protocol.cpp \
       server_transport.cpp server_config.cpp

ifdef debug
	CFLAGS += -g
	OBJDIR := ${addsuffix -debug,$(OBJDIR)}
	NAME := ${addsuffix -debug,$(NAME)}
	SERVER_NAME := ${addsuffix -debug,$(SERVER_NAME)}
else
	CFLAGS += $(OPTFLAGS)
endif

LIBS = -lexpat -llua -llualib

ifdef dumbogg
	LIBS += -lvorbisfile -lvorbis -logg -laldmb -ldumb
	SRCS += dumbogg.c
	CFLAGS += -DHAVE_DUMBOGG
endif

ifdef win32
	OBJDIR := ${addsuffix -win32,$(OBJDIR)}
	NAME := ${addsuffix .exe,$(NAME)}
	SERVER_NAME := ${addsuffix .exe,$(SERVER_NAME)}
	CFLAGS += -DWIN32 -DALLEGRO_STATICLINK -I mingw-libs/include -L mingw-libs/lib
	LIBS += -lNL_s -lalleg_s -lws2_32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 \
	        -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -lbfd -liberty
	SERVER_LIBS = -lNL_s -lws2_32
	SRCS += exchndl.c
else
	CFLAGS += -DLINUX
	INCLUDES = ${shell allegro-config --cflags}
	CFLAGS += $(INCLUDES)
ifdef static	
	LIBS := -static $(LIBS) -lNL ${shell allegro-config --libs}
	SERVER_LIBS = -static -lNL -pthread
else
	LIBS += -lNL -pthread ${shell allegro-config --libs}
	SERVER_LIBS = -lNL -pthread
endif
endif

OBJS := $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))
DEPS = $(OBJS:.o=.d)

OBJS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.o))
DEPS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.d))

ifdef win32
	OBJS := $(OBJS) Seccast.o 
ifndef debug
	SUBSYSTEM := -Wl,--subsystem=windows -e _WinMainCRTStartup
endif
endif

##############################################################################

all: $(OBJDIR) $(NAME)

server: $(OBJDIR) $(SERVER_NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CX) -MMD $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CX) $(CFLAGS) -o $@ $^ $(LIBS) $(SUBSYSTEM)

$(SERVER_NAME): $(OBJS_SERVER)
	$(CX) $(CFLAGS) -o $@ $^ $(SERVER_LIBS)

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.d
	$(RM) $(NAME)

binary-gz: all server
# create linux binary distributive
	svn delete --force $(DISTNAME)
	svn export . $(DISTNAME)
	rm -R $(DISTNAME)/src
	rm -R $(DISTNAME)/datfile
	rm -R $(DISTNAME)/doxygen
	rm $(DISTNAME)/makefile* $(DISTNAME)/Seccast*
	rm $(DISTNAME)/*.rc $(DISTNAME)/*.h
	cp ufo2000 ufo2000-srv $(DISTNAME)
	tar -czf $(DISTNAME).tar.gz $(DISTNAME)
	svn delete --force $(DISTNAME)

win32-installer: all server
# create windows installer using NSIS
	svn delete --force $(DISTNAME)
	svn export . $(DISTNAME)
	rm $(DISTNAME)/makefile* $(DISTNAME)/Seccast*
	rm $(DISTNAME)/*.rc $(DISTNAME)/*.h
	cp ufo2000.exe ufo2000-srv.exe $(DISTNAME)
	sed 's,GAME_VERSION "installer",GAME_VERSION "$(UFO_VERSION).$(UFO_SVNVERSION)",g' < ufo2000.nsi > $(DISTNAME)/ufo2000.nsi
	makensis $(DISTNAME)/ufo2000.nsi
	cp $(DISTNAME)/$(DISTNAME).exe $(DISTNAME).exe
	svn delete --force $(DISTNAME)

win32-beta-installer: all server
# create windows beta installer using NSIS
	svn delete --force $(DISTNAME)
	svn export . $(DISTNAME)
	rm $(DISTNAME)/makefile* $(DISTNAME)/Seccast*
	rm $(DISTNAME)/*.rc $(DISTNAME)/*.h
	cp ufo2000.exe ufo2000-srv.exe $(DISTNAME)
	sed 's,host = lxnt.info,host = lxnt.info:2001,g' < ufo2000.default.ini > $(DISTNAME)/ufo2000.default.ini
	sed 's,GAME_NAME "UFO2000",GAME_NAME "UFO2000 Beta",g' < ufo2000.nsi > $(DISTNAME)/ufo2000.nsi.1
	sed 's,GAME_VERSION "installer",GAME_VERSION "$(UFO_VERSION).$(UFO_SVNVERSION)-beta",g' < $(DISTNAME)/ufo2000.nsi.1 > $(DISTNAME)/ufo2000.nsi
	makensis $(DISTNAME)/ufo2000.nsi
	cp $(DISTNAME)/$(DISTNAME)-beta.exe $(DISTNAME)-beta.exe
	svn delete --force $(DISTNAME)

source-bz2: 
# create tar.bz2 archive with ufo2000 sources (on *nix systems)
	-$(RM) $(DISTNAME)-src.tar.bz2
	svn delete --force $(DISTNAME)
	svn export . $(DISTNAME)
	sed 's,unknown,$(UFO_SVNVERSION),g' < src/version.h > $(DISTNAME)/src/version.h
	tar -cjf $(DISTNAME)-src.tar.bz2 $(DISTNAME)
	svn delete --force $(DISTNAME)

-include $(DEPS)
-include $(DEPS_SERVER)
