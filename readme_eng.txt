
              **************************************************
              **                UFO2000 0.3.XX                **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                English Manual                **
              **************************************************

                                  $Rev$

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2003  ufo2000 development team

The development of UFO2000 is continued after about a year of inactivity. 
Now UFO2000 is back and is improved by cooperative efforts of several
developers, "ufo2000 development team" to be short. For the information 
about who has contributed to UFO2000, read AUTHORS file in UFO2000 
directory. The original author, Alexander Ivanov, is currently not 
involved in the development process anymore, but he already has done 
a great work, many thanks to him! Without his excellent work this game 
would never come to existence.

This game is free software (GPL). For more information about the 
license, read the file COPYING, located in your UFO2000 directory. 

Anyone can take part in UFO2000 development. Any help would be 
appreciated. Please visit http://ufo2000.sourceforge.net/ web page.
There you can subscribe to ufo2000 mailing list and submit your 
comments, ideas, bugreports and patches.

See ChangeLog file for full change listings.

This file was revised and added to by Jeremy Walker, aka Kuroshi X.

===============
About
===============

UFO2000 is a multiplayable remake of the tactical part of Microprose's 
game, X-COM: UFO Defense (UFO: Enemy Unknown).

It requires X-COM game files from the folders:
   GEODATA GEOGRAPH MAPS SOUND TERRAIN UFOGRAPH UFOINTRO UNITS

The Windows version can be compiled either with Microsoft Visual C++ 6.0 or 
free Mingw GCC compiler.

The game also compiles and works in Linux and FreeBSD operating system.
Probably the game will also work in other UNIX like systems. If you managed 
to successfully compile and run UFO2000 in one of them, please let us know.

More detailed information about compiling UFO2000 can be found in INSTALL
file from UFO2000 directory.

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

Right CTRL + 'b', 'd', 'e', 'p' or 'r' Switch between Belarusian, German,
        English, Polish and Russian keymaps (other languages can be
        selected in ufo2000.ini file)

Left CTRL   Show route of selected man to the cursor cell with TU cost.
 		In targeting mode it shows the trajectory of your shots and
 		thrown items. In the inventory screen, it will show item
 		damage value. (ALT alternative)

Left SHIFT  Allows precise aiming if held down until after firing with
 		the left mouse button.

ENTER       send message
BKSP        remove last character in message

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
sound=yes            #yes - sound fx on, no - off
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
