CC = g++
LD = g++
CFLAGS = -funsigned-char -Wall
CFLAGS += -pipe -DLINUX
CFLAGS += -O -mcpu=i686 -s -fomit-frame-pointer
#CFLAGS += -ggdb
INCLUDES = ${shell allegro-config --cflags}
CFLAGS += $(INCLUDES)
LIBS = $(addprefix -l,$(LIBRARIES)) ${shell allegro-config --libs }

SRCS = about.cpp bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       multiplay.cpp netdplay.cpp netipx.cpp netmdm.cpp netsock.cpp   \
       packet.cpp pck.cpp place.cpp platoon.cpp soldier.cpp sound.cpp \
       spk.cpp terrapck.cpp units.cpp video.cpp wind.cpp word.cpp     \
       crc32.cpp persist.cpp jpeg.cpp pfxopen.cpp

OBJS = $(addprefix obj/,$(SRCS:.cpp=.o))
DEPS = $(addprefix obj/,$(SRCS:.cpp=.d))

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
