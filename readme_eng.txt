
              **************************************************
              **                UFO2000 0.3.XX                **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                English Manual                **
              **************************************************

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2003  ufo2000 development team

                    This file was last changed in $Rev$

The development of UFO2000 is continued after about a year of
inactivity. Now UFO2000 is back and supported by a group called the
"UFO2000 Development Team". The original author, Alexander Ivanov, has 
neither time nor desire for doing all the work alone. It is very 
difficult for a single person to maintain such a project. In addition, 
the game is completely free (GPL) and nobody earns a single cent from 
it. For information, read the file COPYING, located in your UFO2000 
folder.

The versions from 0.3.20 onward do not have a single author. In fact, 
there are several people who have contributed their pieces of code to 
this version (the original author, Alexander Ivanov, is among them).

Anyone can take part in UFO2000 development. Any help would be 
appreciated. Please visit http://sourceforge.net/projects/ufo2000/ and 
submit your comments, ideas, bugreports and patches there.

Primary tasks for this version branch (0.3.xx) are:
- fix all the known bugs.
- cleanup and comment the sources.

ufo2000 development team members:
- Alexander Ivanov aka 'sanami'
- Serge Semashko aka 'ssvb'
- Alexander Sabourenkov aka 'lxnt'
- Roman Vil'chenko aka 'tavy'

See CHANGELOG for full change listings.

This file was revised and added to by Jeremy Walker, aka Kuroshi X.

===============
About
===============

UFO2000 is a multiplayable remake of the tactical part of Microprose's 
game, X-COM: UFO Defense (UFO: Enemy Unknown).

It requires X-COM game files from the folders:
   GEODATA GEOGRAPH MAPS SOUND TERRAIN UFOGRAPH UFOINTRO UNITS

The Windows version is compiled with VC6 and requires DirectX 6 or 
above. There are also djgpp and linux versions.


===============
Map Editor
===============

Use the right mouse button to select the tile you wish to replace, and 
a selection menu will come up. Then select 10x10 tile type you want.

F2          Save
F3          Load
ESC         Finish


===============
Connect Chat
===============

After starting the server, but before you move into the mission planner, 
this is the screen you will be in. One player must start as the Server, 
and another as the Client, before you can begin.

F4          Redraw screen
F10         Start game
ESC         Cancel               


===============
Mission Planner
===============

The next screen after the Connect Chat is the Mission Planner.
Here you can select soldiers, determine their starting locations, and 
edit their starting statistics/weapons by holding the left CTRL key 
then clicking on your chosen soldier (see Unit Editor section below).

To place your units press the left mouse button on his/her name then 
point to your chosen starting position on the map. Press again to 
confirm. To deselect the soldier click the right mouse button on his/her name.

Info about player units:
   1) "total men points=..." The sum of params (TU, Health,.. )
      for the selected units.
   2) Number of weapons of each kind for the selected units.

To generate a new map click "NEW", to load press on "LOAD". If you have 
adjusted your map size in the ufo2000.ini file, but still have your old 
map, press "NEW", or load the map you want.

After finishing your plans click on your side's (server/client) "SEND" 
word to send your local playerdata to the other player. "SEND" then 
will become green. To start the game, players must click "START" after 
all "SEND" options are green. The game will only begin after all these 
options are green. Adjusting your players or changing the map will 
revert all words to red.

For a hotseat game, you can press F10 to "START" (your "SEND" must 
still be green, though).

To return to main menu, press ESC.


===============
Unit Editor
===============

You can access this from within the Unit Placement screen. Hold CTRL 
and click on the soldier you wish to edit.

Coming into this you can edit soldier parameters and loadout. Enabled 
parameters for soldiers are: Time Units, Health, Firing, and Throwing 
Accuracy. Other parameters, such as Strength, are disabled because we 
don't know how they work in-game. If, after arranging your equipment, 
you wish to edit your stats again, click the chart on the right.

Each parameter is restricted to be >= 50 and <= 80. The sum of these 
four parameters must be <= 240 for each unit. There are no restrictions 
for the kind and number of each weapon for your units, so during the 
Mission Planner it is suggested that you look at your enemy's weapon 
selection.

F2          Save
F3          Load

Left CTRL   Shows item damage status (in editor -- max value), this
Left ALT    also works in the inventory screen during the game.


===============
Game Controls
===============

All unit actions, as in UFO1, are controlled with the mouse. You can
send and receive messages to/from the other player during the game, 
just type your message out on the keyboard, then hit enter.

These keys are used for game control: 

F1          On/Off  Raw packets show
F4          Redraw screen
F5          Restart
F10         Fullscreen/Windowed mode
ESC         Exit to main menu
ALT-X       Quick exit to OS

Cursors &   change size of map screen
Keypad +/-  

Right CTRL  Switch between rus/lat if enabled in INI.

Left CTRL   Show route of selected man to the cursor cell with TU cost.
 		In targeting mode it shows the trajectory of your shots and
 		thrown items. In the inventory screen, it will show item
 		damage value. (ALT alternative)

Left SHIFT  Allows precise aiming if held down until after firing with
 		the left mouse button.

ENTER       send message
BKSP        remove last character in message

F11   Notifying mode on/off used for checking if connect is ok. 
      Sends "NOTE" packet each second to remote computer and receive
 	the responce. If such packet do not come through, the counter of
 	seconds activated starting from last packet received and shown in
 	top right corner of Info window. If it is necessary to fulfill
 	ceaseless modem command in terminal, e.g. "atdp123456", better
 	turn the notifying mode off in advance.

F12         save the screen to "snapshot.pcx" file 


===============
INI file
("ufo2000.ini")
===============

(Astericks denote notes listed at the bottom of the section. e.g. '*1')

[General]
port=2000            #IP Port (not for Windows version)
server=10.0.0.2      #Server's Default IP address (not for Windows version)
width=500            #Battle Screen width
height=280           #Battle Screen height
map_width=4          #   "NEW" map size
map_height=4         #   map_width*map_height <= 36 !
russian=yes          #no disable CTRL rus/lat,  yes - enable
bloodymenu=no        #no - no bloody menu, yes - yes!
sound=yes            #yes - sound fx on, no - off
comport=2            #   modem COM port (not for windows version)
                     #   1 - COM1, 2 - COM2, 3 - COM3, ...
digvoices=16         # ^2  (2, 4, 8, ...)  number of voices for fx
modules=no           #   not used
modvoices=8          #      "
modname=templsun.xm  #      "
modvolume=125        #      "
speed_unit = 30      #Sets unit speed. *1
speed_bullet = 30    #Same as above, but for game projectiles.
speed_mapscroll = 30 #Same as above, but for map scrolling.
mapscroll = 10       #

[Editor]
platoon_size=8       #number of soldiers in "soldier.dat" file *2
last_map_name = c:\ufo2000\geoscape.dat  #last map name
last_unit_name = team_ru.units           #last unit name

[Flags]
F_FULLSCREEN = 0  # Set to 1 to start the game in fullscreen mode
F_FILECHECK = 1   # Set to 0 to disable data files integrity check on start
F_SELECTENEMY = 1 # Set to 0 to disable blue markers above enemy soldiers

*1: Can be adjusted in-game, in the options menu.
*2: The format of the file "soldier.dat" is the same as the one in 
UFO1, so you can use any "soldier.dat" from that game. However, you 
must write the correct platoon_size in ufo2000.ini (e.g. number of 
soldiers in "soldier.dat").

--END OF FILE--