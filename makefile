#
# HAVE_ defines.
#
# unix-like systems (tested on: RedHat 8, FreeBSD 4.6RC).

CONFIG= -DLINUX -DHAVE_VSNPRINTF -DHAVE_ARPA_INET_H 

# windoze (tested on: MinGW 2.0)

# CONFIG= -DWIN32 

# optimization (uncomment only one)
# release options:
#OPTCFLAGS=-O -mcpu=i686 -fomit-frame-pointer
#OPTLDFLAGS=-s
#ALLEGINC=${shell allegro-config --cflags}
#ALLEGLIB=${shell allegro-config --libs}

#debug options:
OPTCFLAGS=-ggdb -Wall -Werror
OPTLDFLAGS=-ggdb
ALLEGINC=${shell allegro-config --cflags debug}
ALLEGLIB=${shell allegro-config --libs --static debug}

# Any non-standard locations of libraries get here. (i.e. expat)
#EXTRAINC=-I/usr/local/include
#EXTRALIB=-L/usr/local/lib

CC = g++
LD = g++
CFLAGS = -funsigned-char -pipe $(CONFIG)
CFLAGS += $(OPTCFLAGS) $(ALLEGINC) $(EXTRAINC)
LIBS = $(OPTLDFLAGS) $(ALLEGLIB) $(EXTRALIB) -lexpat -ljpgal

SRCS = about.cpp bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       multiplay.cpp netsock.cpp packet.cpp pck.cpp place.cpp         \
       platoon.cpp soldier.cpp sound.cpp spk.cpp terrapck.cpp         \
       units.cpp video.cpp wind.cpp word.cpp crc32.cpp persist.cpp    \
       pfxopen.cpp minimap.cpp

OBJS = ${addprefix obj/,$(SRCS:.cpp=.o)}
DEPS = ${addprefix obj/,$(SRCS:.cpp=.d)}

NAME = ufo2000

all:  $(NAME)

obj/%.o: src/%.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) obj/*.o
	$(RM) obj/*.d
	$(RM) $(NAME)

-include $(DEPS)
