
              **************************************************
              **                UFO2000 0.4.2                 **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                English Manual                **
              **************************************************

                                  $Rev: 646$

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

The game compiles and works in Windows, Linux and FreeBSD operating systems.
Probably the game will also work in other UNIX-like systems. If you managed 
to successfully compile and run UFO2000 in one of them, please let us know.

Detailed information about installing UFO2000 can be found in the INSTALL
file from UFO2000 directory.

===============
***Important***
===============

You need data files from the original X-COM: UFO Defense to play UFO2000. 
The game will also work with data files from the X-COM demo (which can be 
downloaded), but you will only be able to play on 'City' map and use a 
limited set of units in this case. Having a full version of X-COM: UFO Defense 
is highly recommended in order to be able to play in all the original terrains.
Having only TFTD is not sufficient: you will still need UFO, either the full 
game or the demo. 

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

Information about player units:
   1) "Total points=X (of Y)": X is the total points cost of selected units,
       while Y stands for the total points cost of all units in the squad.
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


===========
Unit Editor
===========

You can access Unit Edotir from the Mission Planner. Hold CTRL and click
on the soldier you wish to edit. You can alternatively press F4.

Here you can change both parameters and equipment of your soldiers.

To change your soldier's equipment, select an item from the armoury box
below with the left mouse button, and then put that item to soldier's
backpack, hands, belt, or pockets. When you have finished giving your
soldiers a particular piece of equipment, put it back to the armoury box
or simply click the right mouse button. To deselect some item already
given to your soldiers, click it with the right mouse button, or select
it by the left mouse button and put to some free place in the armoury box.

To change your soldier's parameters, click the chart on the right.
Currently available parameters are: Time Units, Stamina, Health, Reactions,
Firing Accuracy, Throwing Accuracy, and Strength. You can also change the
name of your soldier, his or her race, armor and appearance (the two latter
properties can only be changed for the human race).

Each parameter is restricted to be >= 50 and <= 80 except Strength which
can have a value between 25 and 40. The sum of these parameters (with
Strength doubled) has a certain limit for each unit, so you can't set
all the parameters to maximum. Currently the points limit is 420, giving
an average of 60 points per parameter with the exception for Strength
which gets only 30 points because of its double cost.

You can save and load various unit/squad configurations with these keys:

F2          Save
F3          Load

Use arrow buttons at the top of the screen or arrow keys to cycle between
soldiers. You can also use the TAB key to advance to the next soldier.

To return to the Mission Planner, press ESC or click OK button at the top
of the screen.

=============
Game Controls
=============

All unit actions, as in X-COM, are controlled with the mouse. UFO2000 uses
the same controls as the original games with only minor differences:
* right mouse click can be used to stop movement of a soldier
* soldiers can't automatically open doors, so you need to use right mouse 
  click behind the door to open it (just like in TFTD)
* mouse wheel scrolls map levels up and down just like the ladder buttons
  on the control panel or the PgUp/PgDn keys

You can send and receive messages to/from the other player during the game. 
Just type your message out on the keyboard, then hit enter.

The following keys are used to control the game:

F10         Switch Fullscreen/Windowed mode

ESC         Exit to main menu

Arrows      Scroll the map window

PgUp/PgDn   Switch between map levels

Right CTRL  Fast switching between two keymaps selected as primary_keyboard
 +F9        and secondary_keyboard in ufo2000.ini file.

Right CTRL  + 'b', 'd', 'e', 'p' or 'r' Switch between Belarusian, German,
            English, Polish and Russian keymaps (other languages can be
            selected in ufo2000.ini file).

Left CTRL   Shows the route of the selected soldier to the cursor cell
            with number of TUs left at each cell of the path.
            In the inventory screen, it will show item damage value
            as a damage bar.

Left ALT    Shows the trajectory of your shots assuming the accuracy of the
            shot is ideal.
            When THROW for an object in hand is selected, it alternatively
            shows the trajectory of an ideal throw.
            In the inventory screen, it will show item damage value
            in digits.

Left SHIFT  Allows precise aiming if held down while ordering a soldier to
            shoot with the left mouse button.

Left SHIFT  Change size of map screen.
 +Arrows

ENTER       Send message.

BKSP        Remove last character in the message.

F12         Save the screen to "snapshot_n.pcx" file where n is the number
            of the snapshot.

Other keys such as Latin letters, numbers, etc. can be used to write
a message to your opponent.
--END OF FILE--
