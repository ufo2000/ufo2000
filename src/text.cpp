/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/**
 * Ufo2000: Text for messages and help-system
 */

// Todo: collect all text used in Ufo2000

#include "global.h"
#include "text.h"

#include "font.h"
#include "icon.h"

/**
 * Text for the tooltips of the buttons on the control-panel.
 *
 * The texts must be short, otherwise they will be cut off
 * at the right edge of the battlescape-window.
 */
char *icontext( const int icon_nr )
{
    switch (icon_nr) {
        //                           ....+....1....+....2....+....3
        case B_MAN_UP      : return "Move up one level";
		case B_MAN_DOWN    : return "Move down one level";

        case B_VIEW_UP     : return "View up one level";
        case B_VIEW_DOWN   : return "View down one level";

		case B_MAP         : return "Show Map";
		case B_CROUCH      : return "Stand up / Crouch";

		case B_INVENTORY   : return "Inventory";
		case B_CENTER_VIEW : return "CenterView";

		case B_NEXT_MAN    : return "Jump to Next Man";
		case B_NEXT_MAN_2  : return "Done with this man";

		case B_TOGGLE_ROOF : return "Toggle Roof";
		case B_OPTIONS     : return "Options";

        // when screen-width is set to minimum, "Exit Game" barely fits
		case B_DONE        : return "Done with turn";
		case B_EXIT        : return "Exit Game";

        case B_MAN_STATS   : return "Show Stats of man";
        case B_BARCHART    : return "Switch Barchart";

        case R_TIME_FREE + BUTTON_NUMBER : 
                             return "Reserve Time: None";
        case R_TIME_AIM  + BUTTON_NUMBER : 
                             return "Reserve Time: Aimed shot";
        case R_TIME_SNAP + BUTTON_NUMBER : 
                             return "Reserve Time: Snapshot";
        case R_TIME_AUTO + BUTTON_NUMBER : 
                             return "Reserve Time: Autoshot";

		default            : return "icon ??";
    }
}

/**
 * Show a single help-page when KEY_F1 is pressed.
 * Each page of help can have only have 3 lines of text.
 * The user can browse between pages with the prev & next-buttons,
 * the ok-button exits help.
 *
 * There must be a gap between the pages of different help-chapters.
 */
void help( const int helppage )
// Pages     0: Index (Todo)
// Pages  1-15: Battlescape Help
// Pages 20-25: Inventory Help
// Pages 30-39: Stats Help
// Pages 50-51: Endgame-Stats Help
// Pages 60-63: Network Help
//
// Pages 90-100: Help for Mission-Planner and Rules
// Pages 101++: Scenario-Help (Todo)
{
    int b1      =   0;		//<! button-number the user pressed
    int kp_0    =   0;		//<! Keypress: 0 : default
    int kp_prev = 112;		//<! Keypress for prev.page: 112:"p"
    int kp_ok   =  27;		//<! Keypress for ok=close :  13:ENTER / 27:ESC
    int kp_next = 110;		//<! Keypress for nextpage : 110:"n"
    char prev[16], next[16], ok[16];
    char exit[32];

    sprintf(prev, "%s", "&PREV PAGE");
    sprintf(next, "%s", "&NEXT PAGE");
    sprintf(ok  , "%s", "  CLOSE  ");

    sprintf(exit, "%s", "To leave, press ESC.");

    rest(1); // Don't eat all CPU resources

    switch (helppage) {
        // Battlescape Help :
        case HELP_BATTLESCAPE + 0 :
            b1 = alert3( "UFO2000 is controlled mainly with the mouse, and these keys:",
                         "F1: Help, F2+F3: Save+Load, F9: Keymap, F10: Window/Fullscreen",
                         "F12: Screenshot, Keypad+/-: adjust Music-Volume, ESC: Quit.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;

        case HELP_BATTLESCAPE + 1 :
            b1 = alert3( "To move the active soldier, left-click on the map.",
                         "Right-click turns him towards the cursor, opens doors,",
                         "and stops movement (e.g. in case of a mis-click).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 2 :
            b1 = alert3( "To fire a gun, click on the weapon in the left or right hand,",
                         "select the fire-mode (aimed, snapshot, autoshoot),",
                         "then click on the target.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 3 :
            b1 = alert3( "Grenades should be primed before they are thrown.",
                         "The delay is the number of rounds before they go off.",
                         "Zero delay means they explode at the end of your turn.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 4 :
            b1 = alert3( "While you hold a grenade in your hand, it does not countdown,",
                         "but a high-explosive does !",
                         "Primed proxy-grenades get hot when thrown or put on the ground.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 5 :
            //            ....+....1....+....2....+....3....+....4....+....5....+....6
            b1 = alert3( "The control-panel at the bottom of the screen:",
                         "The boxes at the left and right show the items in hand, e.g. weapons.",
                         "Between them are some buttons and a small status-display.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 6 :
            b1 = alert3( "Control-panel:  Buttons in column 1:",
                         "Climb up && down a level.",
                         "This works only when standing on a gravlift.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 7 :
            b1 = alert3( "Control-panel:  Buttons in column 2:",
                         "(Ladder-symbol) Change level up && down: Show other levels.",
                         "Also can be done with PgUp, PgDn-Keys. The map has 4 levels.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 8 :
            b1 = alert3( "Control-panel:  Buttons in column 3:",
                         "Map-Display: Invoke the map-viewer.",
                         "Stand up / Kneel.  Kneeling improves aiming.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 9 :
            b1 = alert3( "Control-panel:  Buttons in column 4:",
                         "Inventory: access your belt, backpack etc., and items on the ground.",
                         "Center view: scroll map-display to center around the current man.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 10 :
            b1 = alert3( "Control-panel:  Buttons in column 5:",
                         "Switch to next man, who still has TUs left.      ",
                         "Switch to next man, and mark current man as done.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 11 :
            b1 = alert3( "Control-panel:  Buttons in column 6:",
                         "Show/hide other levels (e.g. show roof of buildings etc.)",
                         "Config-Dialog: adjust movement speed, scrolling speed etc.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 12 :
            b1 = alert3( "Control-panel:  Buttons in column 7:",
                         "End your turn.",
                         "Quit: abort mission (same as ESC).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 13 :
            b1 = alert3( "Control-panel:  green&&red buttons at the lower left:",
                         "Green: Reserve no time, move freely.                   ",
                         "Red:   Reserve time for aimed shot, snapshot, autofire.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 14 :
            b1 = alert3( "Control-panel:  big brown button in the center",
                         "(shows turn-number):",
                         "Invoke statistics-display for current man.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Inventory:
        case HELP_INVENTORY + 0 :
            b1 = alert3( "Inventory: here you can manage the weapons",
                         "and equipment of your soldier.",
                         "To leave, press the OK-button, or goto the next/previous man.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 1 : // Todo: save+load single soldiers
            b1 = alert3( "In the mission-planner, you can use F2 && F3",
                         "to save && load a team of soldiers.",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 2 :
            b1 = alert3( "Use the mouse to move items between the slots",
                         "for hands, belt, backpack etc.",
                         "Note: during battle, each move costs time.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 3 :
            b1 = alert3( "To unload a weapon, move it over the clip-button",
                         "(top right, below the arrow-buttons) and click there.",
                         "You must have your second hand free.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 4 :
            b1 = alert3( "To load an empty weapon, hold it in one hand,",
                         "and move ammo of the matching type into the weapon-slot.",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 5 : // Todo: Flying Armor
            b1 = alert3( "The armor cannot be changed during a battle.",
                         "Flying armor does not work yet (it only has stronger armor).",
                         "Click on the picture of the soldier to get to his stats.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);
            break;

        // Help for Stats-Display:
        case HELP_STATS + 0 :
            b1 = alert3( "Stats-Display: the current attributes of the soldier.",
                         "",
                         "To leave, press ESC or click a mousebutton.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_STATS + 1 :
            b1 = alert3( "Stats:",
                         "Time-Units: every action costs time.",
                         "Energy: some actions also cost energy (e.g. walking).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 2 :
            b1 = alert3( "Stats:",
                         "Health: when this reaches zero, the soldier dies.",
                         "Fatal Wounds: amount of health-loss each round.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 3 :
            b1 = alert3( "Stats:",
                         "Stun: shown as an overlay to Health. When the stun-value",
                         "reaches the value of health, the soldier is stunned.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 4 :
            b1 = alert3( "Stats:",
                         "Bravery: (not yet implemented - same for all man)",
                         "Morale: current morale. Goes down when own men die.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 5 :
            b1 = alert3( "Stats:",
                         "Reactions: chance for firing at an enemy during their round.",
                         "This needs time for at least a snapshot.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 6 :
            b1 = alert3( "Stats:",
                         "Firing accuracy: for pistols and guns.",
                         "Throwing accuracy: for grenades.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 7 :
            b1 = alert3( "Stats:",
                         "Strength: this determines how much the soldier can carry.",
                         "When overloaded, he gets less TU each round.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 8 : // Todo: Psi-Amp
            b1 = alert3( "Stats:",
                         "PSI-Strength: (not yet implemented)", "",
                         //"PSI-Skill:",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 9 :
            b1 = alert3( "Stats:",
                         "Armor: protection-value on all sides of the soldier.",
                         "Under-armor protects against nearby explosions.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);
            break;
        // Help for Endgame-Stats:
        case HELP_ENDGAME + 0 :
            b1 = alert3( "Endgame-Statistics: ",
                         "",
                         "A summary of kills and damage for both teams.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_ENDGAME + 1 :
            b1 = alert3( "Endgame-Stats:",
                         "You can stay here to chat in private with your partner.",
                         exit,
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);
            break;

        // Help for Network - Connect to Server / Chat:
        case HELP_NET + 0 :
            b1 = alert3( "Connect to an UFO2000-Server on your LAN (e.g. 192.168.1.2:2000)",
                         "or on the Internet (e.g. the offical server lxnt.info:2001).",
                         exit,
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_NET + 1 :
            b1 = alert3( "To chat, just type your message, and press ENTER.",
                         "To challenge another player, or accept his challenge,",
                         "click on his name in the player-list at the right.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 2 :
            b1 = alert3( "Colors of the playernames:",
                         "White: You.                             ",
                         "Gray : Someone else, available for chat.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 3 :
            b1 = alert3( "Red   : He/she is busy, playing with someone else.",
                         "Yellow: You have sent a challenge to him/her.     ",
                         "Green : You can accept a challenge from him/her.  ",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);
            break;

        // Help for Mission-Planner:
        case HELP_PLANNER + 0 :
            b1 = alert3( "Mission-Planner: here you can select a map and a scenario,",
                         "set the game-rules, and select soldiers for a mission.",
                         "When finished, press SEND, then START. To leave, press ESC.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 1 :
            b1 = alert3( "To edit one of your soldiers, CTRL-click on his name.",
                         "Left-click to place a soldier on the map (your side only),",
                         "right-click to remove him.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 2 :
            b1 = alert3( "SAVE && LOAD for a team can be found on the soldier-edit-screen.",
                         "",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 3 :
            b1 = alert3( "To change the scenario, click on the text below 'MATCH SETTINGS'.",
                         "To generate a new map, click on the terrain-name,",
                         "then click on LOAD, NEW or select a new size (4*4, 5*5, 6*6).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 4 :
            b1 = alert3( "Click on 'Game rules' for fine-tuning the settings:",
                         "Explosives, Pointlimit, Turnlimit, Timelimit, FOW, Items on Ground.",
                         "You should negotiate them via chat with your partner.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 5 :
            b1 = alert3( "Gamerules - Explosives:",
                         "What kind of grenades and rockets are allowed.",
                         "(0=no explosives .. 3=everything allowed)",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 6 :
            b1 = alert3( "Gamerules - Pointlimit:",
                         "How many points you can spend on your soldiers,",
                         "their attributes, armor, weapons and equipment.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 7 :
            b1 = alert3( "Gamerules - Turnlimit:",
                         "How many turns the match will last.",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 8 :
            b1 = alert3( "Gamerules - Timelimit:",
                         "How many seconds each turn lasts.",
                         "This is for fast, blitz-type games.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 9 :
            b1 = alert3( "Gamerules - Map explored:",
                         "Settings: all map explored / fog of war.",
                         "E.g. all parts of the map that you have not yet seen are black.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 10 :
            b1 = alert3( "Gamerules - Ground",
                         "If this is off, you cannot leave the editor while items",
                         "are on the ground (e.g. heavy rockets to take on the mission).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
                         // kp_next: continue with scenario-help
            break;

        // Scenario-Help:
        case HELP_SCENARIO + 0 :
            b1 = alert3( "There are several scenario-types available:",
                         "Deathmatch, Escape, Control, Sabotage, Capture ...",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
                         // kp_prev: continue with help for mission-planner
            break;
        case HELP_SCENARIO + 1 :
            b1 = alert3( "Deathmatch - the most basic scenario:",
                         "To win, you just have to survive,",
                         "and kill or stun everybody else.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        // TODO - Scenario-developers, please help yourself !!!
        case HELP_SCENARIO + 2 :
            b1 = alert3( "(to be continued)",
                         "",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;


        default:
            // no help page: just exit
            break;
    }
    if (b1 == 1)
        help( helppage - 1 );  // Previous help-page
    if (b1 == 3)
        help( helppage + 1 );  // Next help-page
}
END_OF_FUNCTION(help);

