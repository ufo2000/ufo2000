
              **************************************************
              **                UFO2000 0.4.0                 **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                English Manual                **
              **************************************************

                                  $Rev$

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2004  ufo2000 development team

The development of UFO2000 is continuing after about a year of inactivity. 
Now UFO2000 is back and is improved by the cooperative efforts of several
developers; the "ufo2000 development team" to be short. For information 
about who has contributed to UFO2000, read the AUTHORS file in UFO2000 
directory. The original author, Alexander Ivanov, is currently not 
involved in the development process, but he has already done 
a great job, many thanks to him! Without his excellent work this game 
would never have come into existence.

This game is free software (GPL). For more information about the 
license, read the file COPYING, located in your UFO2000 directory. 

Anyone can take part in UFO2000 development; any help would be 
appreciated. Please visit http://ufo2000.sourceforge.net/ web page.
There you can subscribe to the ufo2000 mailing list and submit your 
comments, ideas, bugreports and patches.

See ChangeLog file for full change listings.

===============
About
===============

UFO2000 is a multiplayer remake of the tactical part of Microprose's 
game, X-COM: UFO Defense (UFO: Enemy Unknown).

You need data files from the original X-COM to play UFO2000. The game will 
also work with data files from the X-COM demo, but you will only be able 
to play on 'City' map and use a limited set of units in this case. Having a 
full version of X-COM is highly recommended.

The game compiles and works in Windows, Linux and FreeBSD operating systems.
Probably the game will also work in other UNIX-like systems. If you managed 
to successfully compile and run UFO2000 in one of them, please let us know.

Detailed information about installing UFO2000 can be found in the INSTALL
file from UFO2000 directory.

=========
Main Menu
=========

After starting the game, you will see a number of available menu options:

   Connect to internet server

      The game can connect to the server running on the internet or on a 
      local network (to start the server on your computer, just run 
      'ufo2000-srv' executable included in ufo2000 distributive.) After 
      selecting this menu option, you will see "connect to server" dialog.
      You need to enter server address; lxnt.info is used by default, it
      is currently the official ufo2000 server running on the internet. If 
      you want to play on a local network, you need to start the ufo2000 
      server on one computer and enter its IP address when connecting
      to it from ufo2000 on another computer.
     The other two required fields are login and password. They are 
      used to identify the user to track some game statistics 
      (http://ufo2000.lxnt.info/results.php, currently.) Login 
      should be any name not used by other players and password should be 
      at least 6 characters long. After successful login or registration, 
      you will see the internet server chat screen.

   Start hotseat game

      Starts the game on a single computer (no network is required.) The 
      current player changes after each turn. After clicking this menu
      item, you will be moved directly to the mission planner screen.

   Load saved game

      Load previously saved game. Works only for hotseat games.

   Quit

      No comments ;)


====================
Internet Server Chat
====================

After connecting to the ufo2000 server, you will see a chat console and
a list of players connected to the server. The status of each player
is determined by the color of his name:

white player name  - that's your own name
gray player name   - available for chat
yellow player name - you have sent a challenge to this player
green player name  - you can accept a challenge from this player
red player name    - the player is busy playing with someone else

To challenge a player, just click on his name (it will turn yellow.)
If someone's name is green, that means he has challenged you. After
clicking on his name, the game will start and the chat console will be 
replaced by the mission planner screen.

===============
Mission Planner
===============

Here you can select soldiers, determine their starting locations, and 
edit their starting statistics/weapons by holding the left CTRL key 
and clicking on your chosen soldier (see Unit Editor section below.)

To place your units, press the left mouse button on his/her name and 
point to your chosen starting position on the map. Press again to 
confirm. To deselect the soldier click the right mouse button on his/her name.

Info about player units:
   1) "total men points=..." The sum of parameters (TU, Health,.. )
      for the selected units.
   2) Number of weapons of each kind for the selected units.

To generate a new map click "NEW", to load press on "LOAD". If you have 
adjusted your map size in the ufo2000.ini file, but still have your old 
map, press "NEW", or load the map you want.

After finishing your plans, click on your side's "SEND" button
to send your local playerdata to the other player. "SEND" will 
then become green. To start the game, players must click "START" after 
all "SEND" options are green. The game will only begin after all these 
options are green. Adjusting your players or changing the map will 
revert all words to red.

To return to the main menu, press ESC.


===============
Unit Editor
===============

You can access this from within the Unit Placement screen. Hold CTRL 
and click on the soldier you wish to edit.

Coming into this you can edit soldier parameters and loadout. Enabled 
parameters for soldiers are: Stamina, Strength, Time Units, Health, Firing, 
Reactions, and Throwing Accuracy. Other parameters are disabled because 
we don't know how they work in-game. If, after arranging your equipment, 
you wish to edit your stats again, click the chart on the right.

Each parameter is restricted to be >= 50 and <= 80. The sum of these 
parameters has a certain limit for each unit, so you can't set all 
the parameters to maximum. There are no restrictions for the kind and 
number of each weapon for your units, so during the Mission Planner it 
is suggested that you look at your enemy's weapon selection. You can save 
and load various unit/squad configurations with these keys:

F2          Save
F3          Load

Left CTRL   Shows item damage status (in editor -- max value), this
Left ALT    also works in the inventory screen during the game.


===============
Game Controls
===============

All unit actions, as in X-COM, are controlled with the mouse. You can
send and receive messages to/from the other player during the game. 
Just type your message out on the keyboard, then hit enter.

These keys are used for game control: 

F10         Fullscreen/Windowed mode

ESC         Exit to main menu

Cursors &   change size of map screen
Keypad +/-  

Right CTRL  Fast switching between two keymaps selected in primary_keyboard 
            and secondary_keyboard in ufo2000.ini file. 

Right CTRL  + 'b', 'd', 'e', 'p' or 'r' Switch between Belarusian, German,
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
F_SELECTENEMY = 1 # Set to 0 to disable blue markers above enemy soldiers

*1: Can be adjusted in-game, in the options menu.
*2: The format of the file "soldier.dat" is the same as the one in 
UFO1, so you can use any "soldier.dat" from that game. However, you 
must write the correct platoon_size in ufo2000.ini (e.g. number of 
soldiers in "soldier.dat").

--END OF FILE--

