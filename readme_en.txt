
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
   1) "total men points=..." Total points cost for selected units.
   2) Number of weapons of each kind for the selected units.

"Match settings" box:
   1)Name of scenario selected:
       Click to select scenario to use and scenario options.
   2)Name of map selected:
       Click to select terrain and size of the map and to either generate new
       or load existing map.
   3)"Game rules:"
       Click to set match rules, such as turns limit, time limit, etc.

After finishing your plans, click on your side's "SEND" button
to send your local playerdata to the other player. 
If your selected men meet all requirments for "SEND" will become green. 
Else you'll see a message about the error in chat console.
To start the game, players must click "START" after 
all "SEND" options are green. The game will only begin after all these 
options are green. Adjusting your players, changing the map, scenario
or match rules will revert all words to red.

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
the parameters to maximum. You can save and load various unit/squad 
configurations with these keys:

F2          Save
F3          Load


===============
Game Controls
===============

All unit actions, as in X-COM, are controlled with the mouse. UFO2000 uses
the same controls as the original games with only minor differences:
* right mouse click can be used to stop movement of a soldier
* soldiers can't automatically open doors, so you need to use right mouse 
  click behind the door to open it (just like in TFTD)

You can send and receive messages to/from the other player during the game. 
Just type your message out on the keyboard, then hit enter.

These keys are used for game control: 

F10         Switch Fullscreen/Windowed mode

ESC         Exit to main menu

Cursors &   change size of map screen
Keypad +/-  

Right CTRL  Fast switching between two keymaps selected in primary_keyboard 
F9          and secondary_keyboard in ufo2000.ini file. 

Right CTRL  + 'b', 'd', 'e', 'p' or 'r' Switch between Belarusian, German,
            English, Polish and Russian keymaps (other languages can be
            selected in ufo2000.ini file)

Left CTRL   Show route of selected man to the cursor cell with TU cost.
        In targeting mode it shows the trajectory of your shots and
        thrown items. In the inventory screen, it will show item
        damage value. (ALT alternative)

Left SHIFT  Allows precise aiming if held down until after firing with
        the left mouse button.

Left SHIFT  + cursor keys Scroll the map window.

ENTER       send message
BKSP        remove last character in message

F12         save the screen to "snapshot.pcx" file 

--END OF FILE--
