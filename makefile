##############################################################################
# Compiling ufo2000: make {win32=1} {debug=1}                                #
#                                                                            #
# Define win32=1 when compiling with Mingw gcc compiler for windows          #
# Define xmingw=1 when compiling win32 binary with Mingw gcc crosscompiler   #
# Define debug=1 when you want to build debug version of ufo2000             #
#                                                                            #
# Just typing 'make' builds the release version of ufo2000 for *nix          #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# The game depends on Allegro (4.0.x), Expat and HawkNL libraries, so you    #
# need to install them before running make                                   #
#                                                                            #
# Also it is highly recommended but not necessery to have subversion         #
# client installed                                                           #
##############################################################################

ifdef VERSION
	DISTNAME := ufo2000-$(VERSION)
else
	DISTNAME := ufo2000-r${shell svnversion .}
endif

CC = g++
LD = g++
CFLAGS = -funsigned-char -Wall
CFLAGS += -pipe
OBJDIR = obj
NAME = ufo2000
SERVER_NAME = ufo2000-srv

ifdef DATA_DIR
	CFLAGS += -DDATA_DIR=\"$(DATA_DIR)\"
endif

ifndef OPTFLAGS
	OPTFLAGS = -O2
endif

ifdef xmingw
    CC = i386-mingw32msvc-g++
    LD = i386-mingw32msvc-g++
    win32 = 1
endif

VPATH = src src/jpgalleg

SRCS = bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp           \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       multiplay.cpp netsock.cpp packet.cpp pck.cpp place.cpp         \
       platoon.cpp soldier.cpp sound.cpp spk.cpp terrapck.cpp         \
       units.cpp video.cpp wind.cpp word.cpp crc32.cpp persist.cpp    \
       jpeg.cpp pfxopen.cpp minimap.cpp about.cpp stats.cpp           \
       server_protocol.cpp server_transport.cpp server_gui.cpp        \
       server_config.cpp

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

ifdef win32
	OBJDIR := ${addsuffix -win32,$(OBJDIR)}
	NAME := ${addsuffix .exe,$(NAME)}
	SERVER_NAME := ${addsuffix .exe,$(SERVER_NAME)}
	CFLAGS += -DWIN32
	LIBS = -lexpat -llua -llualib -lNL -lalleg -lws2_32
else
	CFLAGS += -DLINUX
	INCLUDES = ${shell allegro-config --cflags}
	CFLAGS += $(INCLUDES)
	LIBS += -lexpat -llua -llualib -lNL ${shell allegro-config --libs}
endif

OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.d))
OBJS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.o))
DEPS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.d))

ifdef win32
	OBJS := $(OBJS) Seccast.o 
ifndef debug
	SUBSYSTEM := -Wl,--subsystem=windows -e _WinMainCRTStartup
	CFLAGS += -s
endif
endif

##############################################################################

all: $(OBJDIR) $(NAME)

server: $(OBJDIR) $(SERVER_NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS) $(SUBSYSTEM)

$(SERVER_NAME): $(OBJS_SERVER)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.d
	$(RM) $(NAME)

source-zip: 
# create zip archive with ufo2000 sources, requires 7-zip archiver
	-$(RM) $(DISTNAME)-src.zip
	svn export . $(DISTNAME)
	7z a -tzip -r -mx $(DISTNAME)-src.zip "$(DISTNAME)/*"
	svn delete --force $(DISTNAME)

source-bz2: 
# create tar.bz2 archive with ufo2000 sources (on *nix systems)
	-$(RM) $(DISTNAME)-src.tar.bz2
	svn export . $(DISTNAME)
	tar -cjf $(DISTNAME)-src.tar.bz2 $(DISTNAME)
	svn delete --force $(DISTNAME)
	cp ufo2000.ebuild $(DISTNAME).ebuild

-include $(DEPS)
-include $(DEPS_SERVER)
