##############################################################################
# makefile for ufo2000                                                       #
#                                                                            #
# Compiling ufo2000: make {debug=1} {xmingw=1} {no_ttf=1} {no_dumbogg=1}     #
#                                                                            #
# Define xmingw=1 when compiling win32 binary with Mingw gcc crosscompiler   #
# Define debug=1 when you want to build debug version of ufo2000             #
#                                                                            #
# Just typing 'make' builds the release version of ufo2000 for *nix          #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# Type 'make server' to build ufo2000 server                                 #
#                                                                            #
# The game depends on Allegro (4.0.x), Expat and HawkNL libraries, so you    #
# need to install them before running make                                   #
#                                                                            #
# DUMB and Ogg Vorbis are optional (they allow to play music in XM, S3M,     #
# MOD, IT and OGG formats). Use no_dumbogg=1 in make command line to build   #
# ufo2000 without these libraries.                                           #
#                                                                            #
# FreeType2 library is optional too, its support can be disabled with        #
# no_ttf=1 make command line option.                                         #
#                                                                            #
# When compiling the game with Mingw (either native or a crosscompiler), it  #
# is possible to use a set of precompiled libraries. Just download archive   #
# http://ufo2000.lxnt.info/files/mingw-libs.zip and extract it into ufo2000  #
# sources directory.                                                         #
#                                                                            #
# Also it is highly recommended but not necessery to have subversion         #
# command line client installed (it is required if you want to make          #
# 'win32-installer', 'source-zip' and 'source-bz2' targets)                  #
##############################################################################

UFO_SVNVERSION := ${shell svnversion .}
UFO_VERSION := ${shell awk 'BEGIN {FS=sprintf("%c",34)} /UFO_VERSION_STRING/ { print $$2; }' src/version.h}

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
CFLAGS = -funsigned-char -Wall -Wno-deprecated-declarations -I src/lua -DDEBUGMODE
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

ifdef profile
	CFLAGS += -pg
endif

ifdef valgrind
	debug = 1
endif

VPATH = src src/jpgalleg src/dumbogg src/exchndl src/agup src/lua src/jinete src/glyphkeeper

SRCS_LUA = lapi.c lauxlib.c lbaselib.c lcode.c ldblib.c ldebug.c      \
           ldo.c ldump.c lfunc.c lgc.c liolib.c llex.c lmathlib.c     \
           lmem.c loadlib.c lobject.c lopcodes.c lparser.c            \
           lstate.c lstring.c lstrlib.c ltable.c ltablib.c            \
           ltests.c ltm.c lundump.c lvm.c lzio.c

SRCS = bullet.cpp cell.cpp config.cpp connect.cpp crc32.cpp dirty.cpp \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       minimap.cpp mouse.cpp multiplay.cpp music.cpp packet.cpp       \
       pck.cpp persist.cpp place.cpp platoon.cpp random.cpp           \
       scenario.cpp server_config.cpp server_gui.cpp                  \
       server_protocol.cpp server_transport.cpp skin.cpp soldier.cpp  \
       sound.cpp spk.cpp stats.cpp terrapck.cpp text.cpp units.cpp    \
       video.cpp wind.cpp                                             \
                                                                      \
       $(SRCS_LUA)                                                    \
                                                                      \
       aalg.c aase.c abeos.c abitmap.c agtk.c agup.c ans.c            \
       aphoton.c awin95.c decode.c encode.c io.c jpgalleg.c

SRCS_SERVER = server_config.cpp server_main.cpp server_protocol.cpp   \
              server_transport.cpp $(SRCS_LUA)

ifdef debug
	CFLAGS += -g
ifdef valgrind
	CFLAGS += -O1
endif
	OBJDIR := ${addsuffix -debug,$(OBJDIR)}
	NAME := ${addsuffix -debug,$(NAME)}
	SERVER_NAME := ${addsuffix -debug,$(SERVER_NAME)}
else
	CFLAGS += $(OPTFLAGS)
endif

LIBS = -lexpat

ifndef no_ttf
ifdef win32
	LIBS += -lfreetype
else
	CFLAGS += ${shell freetype-config --cflags}
	LIBS += ${shell freetype-config --libs}
endif
	CFLAGS += -DHAVE_FREETYPE -DGLYPH_TARGET=GLYPH_TARGET_ALLEGRO -DGK_NO_LEGACY
	SRCS += glyph.c ji_font.c
endif

ifndef no_dumbogg
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

OBJS_SERVER := $(SRCS_SERVER:.cpp=.o)
OBJS_SERVER := $(OBJS_SERVER:.c=.o)
OBJS_SERVER := $(addprefix $(OBJDIR)/,$(OBJS_SERVER))
DEPS_SERVER = $(OBJS_SERVER:.o=.d)

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

# Update the translations of game messages to different languages 
# using gettext tools - see manual at
# http://www.gnu.org/software/gettext/manual/gettext.html
lng-de: $(SRCS)
	xgettext -o translations/ufo2000.pot --keyword=_ $^
	msgmerge --update translations/ufo2000-de.po translations/ufo2000.pot

lng-es: $(SRCS)
	xgettext -o translations/ufo2000.pot --keyword=_ $^
	msgmerge --update translations/ufo2000-es.po translations/ufo2000.pot

lng-fr: $(SRCS)
	xgettext -o translations/ufo2000.pot --keyword=_ $^
	msgmerge --update translations/ufo2000-fr.po translations/ufo2000.pot

lng-ru: $(SRCS)
	xgettext -o translations/ufo2000.pot --keyword=_ $^
	msgmerge --update translations/ufo2000-ru.po translations/ufo2000.pot

lng-be: $(SRCS)
	xgettext -o translations/ufo2000.pot --keyword=_ $^
	msgmerge --update translations/ufo2000-be.po translations/ufo2000.pot
	
lng-all: lng-de lng-es lng-fr lng-ru lng-be

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
	svn export --native-eol "LF" . $(DISTNAME)
	sed 's,unknown,$(UFO_SVNVERSION),g' < src/version.h > $(DISTNAME)/src/version.h
	tar -cjf $(DISTNAME)-src.tar.bz2 $(DISTNAME)
	svn delete --force $(DISTNAME)

docs:
	sed 's,%REVISION%,$(UFO_VERSION).$(UFO_SVNVERSION),g' < doxygen/doxy-en.conf > doxy-en.conf
	doxygen doxy-en.conf

-include $(DEPS)
-include $(DEPS_SERVER)

#.
