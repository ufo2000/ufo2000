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

#include "stdafx.h"

#include "global.h"
#include "text.h"

#include "font.h"
#include "icon.h"

/**
 * Call Message() from main.lua to write a message 
 * into the logfile init-scripts.log
 */
void lua_message( const std::string &str1 )
{
    // Todo: general time-class, to avoid calling this several times per second
    time_t now = time(NULL);
    struct tm * t = localtime(&now);
    char timebuf[128];
    strftime(timebuf, 128, "%Y-%m-%d %H:%M:%S", t);
#ifndef WIN32
    snprintf(timebuf + strlen(timebuf), sizeof(timebuf), " [%d]", getpid());
#endif

    char txt1[32], txt2[32];
    sprintf(txt1, "# %s : ", timebuf );
    sprintf(txt2, "." );

    lua_safe_dostring(L, (std::string("Message([[%s]], [[") + 
        txt1 + str1 + txt2 + std::string("]])")).c_str() );
};

/**
 * gettext-prototype:
 * 
 * For translating text to foreign languages.
 * Instead of the full gnu-gettext-package, a simple LUA-script is used.
 * Todo: investigate performance, cache translated messages ...
 */
const char *gettext( char *str1 )
// !! Note: lua-scripts can be called only after initmain() with lua_open() !!
{ 
    static std::string str2;
  //strcpy(str2, "" );
    str2 = "";

  //lua_message( std::string("gettext: '") + str1 + std::string("'") );
/*
    FILE *f1 = fopen( "gettext.log", "at");
    fprintf(f1, "#: '%s'\n", str1);
    fclose(f1);
*/
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "gettext");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (!lua_isfunction(L, -1))
        display_error_message( "Fatal: no 'gettext' function registered" );
    lua_pushstring(L, str1);
    lua_safe_call(L, 1, 1);
    str2 = lua_tostring(L, -1);
    lua_settop(L, stack_top);
/*
    FILE *f2 = fopen( "gettext.log", "at");
    fprintf(f2, "#= '%s'\n", str2.c_str() );
    fclose(f2);
*/
    if (str2 == "")    // if lua-script did not return a translation
        return str1;   // give back original string
    else
        return str2.c_str();
};

/**
 * Text for the tooltips of the buttons on the control-panel.
 *
 * The texts must be short, otherwise they will be cut off
 * at the right edge of the battlescape-window.
 */
const char *icontext( const int icon_nr )
{
    static std::string txt;

    switch (icon_nr) {
        //                            ....+....1....+....2....+....3
        case B_MAN_UP      : txt = _("Move up one level");   break;
        case B_MAN_DOWN    : txt = _("Move down one level"); break;

        case B_VIEW_UP     : txt = _("View up one level");   break;
        case B_VIEW_DOWN   : txt = _("View down one level"); break;

        case B_MAP         : txt = _("Show Map");            break;
        case B_CROUCH      : txt = _("Stand up / Crouch");   break;

        case B_INVENTORY   : txt = _("Inventory");           break;
        case B_CENTER_VIEW : txt = _("CenterView");          break;

        case B_NEXT_MAN    : txt = _("Jump to Next Man");    break;
        case B_NEXT_MAN_2  : txt = _("Done with this man");  break;

        case B_TOGGLE_ROOF : txt = _("Toggle Roof");         break;
        case B_OPTIONS     : txt = _("Options");             break;

        // when screen-width is set to minimum, "Exit Game" barely fits
        case B_DONE        : txt = _("Done with turn");      break;
        case B_EXIT        : txt = _("Exit Game");           break;

        case B_MAN_STATS   : txt = _("Show Stats of man");   break;
        case B_BARCHART    : txt = _("Switch Barchart");     break;

        case R_TIME_FREE + BUTTON_NUMBER : 
                             txt = _("Reserve Time: None");
                             break;
        case R_TIME_AIM  + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Aimed shot");
                             break;
        case R_TIME_SNAP + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Snapshot");
                             break;
        case R_TIME_AUTO + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Autoshot");
                             break;

        default            : txt = "icon ??"; 
    }
    return txt.c_str();
}

/**
 * Show a single help-page when KEY_F1 is pressed.
 * Each page of help can have only 3 lines of text.
 * The user can browse between pages with the prev & next-buttons,
 * the ok-button exits help.
 *
 * There must be a gap between the pages of different help-chapters.
 * The number 'startpage-1' of each help-chapter is reserved for a jump to the index-page.
 */
void help( const int helppage )
// Page    0   : Help-Index (Todo)
// Page    1..8: Reserved
// Pages  10+ 3: UFO2000-Introduction (extended ABOUT-Box)
// Pages  20+15: Help for Battlescape 
// Pages  40+ 1: Help for Mapviewer
// Pages  50+ 1: Help for Mindprobe (Todo)
// Pages  60+ 1: Help for Motion-Scanner (Todo)
// Pages  70+ 1: Help for Medi-kit (Todo)
// Pages  80+ 1: Help for PSI-Amp  (Todo)
// Pages  90+ 1: Help for Blaster-Launcher (Todo)
// Pages 100+ 5: Inventory Help
// Pages 110+ 9: Stats Help
// Pages 130+ 1: Endgame-Stats Help
// Pages 140+ 4: Network Help
//
// Pages 160+10: Help for Mission-Planner and Rules
// Pages 171++: Scenario-Help (Todo)
{
    int b1       =   0;		//<! button-number the user pressed
    int kp_0     =   0;		//<! Keypress: 0 : default
    int kp_index = 105;		//<! Keypress for indexpage: 105:"i"
    int kp_prev  = 112;		//<! Keypress for prev.page: 112:"p"
    int kp_ok    =  27;		//<! Keypress for ok=close :  13:ENTER / 27:ESC
    int kp_next  = 110;		//<! Keypress for nextpage : 110:"n"
    char index[16], prev[16], next[16], ok[16];
    char exit[32];

// Todo: Calculate keycode from "&I" in button-text
    sprintf(index, "%s", _("HELP-&INDEX") );
    sprintf(prev,  "%s", _("&PREV PAGE") );
    sprintf(next,  "%s", _("&NEXT PAGE") );
    sprintf(ok,    "%s", _("  CLOSE  ")  );

    sprintf(exit,  "%s", _("To leave, press ESC.") );

    rest(1); // Don't eat all CPU resources

    switch (helppage) {
        case HELP_U2K_INDEX + 0 :
            // Todo : Dialog for direct access to all help-chapters
            help( HELP_INTRO       );
            help( HELP_BATTLESCAPE );
            break;

        // UFO2000 - Introduction :
        case HELP_INTRO + 0 :
            b1 = alert3( "UFO2000 is a multiplayer game in the spirit of 'X-COM: UFO Defense'.",
                         "At the moment, only the tactical part of the game is playable,",
                         "e.g. two teams of soldiers and/or aliens fighting each other.",
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_INTRO + 1 :
            b1 = alert3( "UFO2000 needs data (graphics, sound, maps) from an original XCOM-game.",
                         "Files from the XCOM-Demo can be used if you don't own XCOM-1,",
                         "but this demo only has a few of the maps and aliens.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INTRO + 2 :
            b1 = alert3( "If you also have Terror-from-the-Deep (aka UFO-2) or the TFTD-demo,",
                         "some additional maps from this game can be used.",
                         "(But XCOM-1 is always required)",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INTRO + 3 : 
            b1 = alert3( "UFO2000 can be played hotseat (solo, or 2 players on 1 computer),",
                         "or via LAN or internet.  The offical server runs on lxnt.info:2001.",
                         "There is no AI implemented yet.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Battlescape:
        case HELP_BATTLESCAPE + 0 :
            b1 = alert3( "UFO2000 is controlled mainly with the mouse, and these keys:",
                         "F1: Help, F2+F3: Save+Load, F9: Keymap, F10: Window/Fullscreen",
                         "F12: Screenshot, Keypad+/-: adjust Music-Volume, ESC: Quit.",
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 1 :
            b1 = alert3( "The active soldier is highlighted by a yellow arrow above him.",
                         "Click on any of your soldiers to make him active.",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 2 :
            b1 = alert3( "To move the active soldier, left-click on the map.",
                         "Right-click turns him towards the cursor, opens doors,",
                         "and stops movement (e.g. in case of a mis-click).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 3 :
            b1 = alert3( "To fire a gun, click on the weapon in the left or right hand,",
                         "select the fire-mode (aimed, snapshot, autoshot), then click",
                         "on the target.  Use the up&&down-buttons to aim at other levels.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 4 :
            b1 = alert3( "Grenades should be primed before they are thrown.",
                         "The delay is the number of rounds before they go off.",
                         "Zero delay means they explode at the end of your turn.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 5 :
            b1 = alert3( "While you hold a grenade in your hand, it does not countdown,",
                         "but a high-explosive does !",
                         "Primed proxy-grenades get hot when thrown or put on the ground.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 6 :
            //            ....+....1....+....2....+....3....+....4....+....5....+....6
            b1 = alert3( "The control-panel at the bottom of the screen:",
                         "The boxes at the left and right show the items in hand, e.g. weapons.",
                         "Between them are some buttons and a small status-display.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 7 :
            b1 = alert3( "Control-panel:  Buttons in column 1:",
                         "Climb up && down a level.",
                         "This works only when standing on a gravlift.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 8 :
            b1 = alert3( "Control-panel:  Buttons in column 2 (Ladder-symbol):",
                         "Change level up && down: Look (and aim) at other levels.",
                         "Maps have 4 levels.  Keyboard-shortcut: PgUp && PgDn-Keys.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 9 :
            b1 = alert3( "Control-panel:  Buttons in column 3:",
                         "Map-Display: Invoke the map-viewer for a small overhead-view.",
                         "Stand up / Kneel.  Kneeling improves aiming (10%).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 10 :
            b1 = alert3( "Control-panel:  Buttons in column 4:",
                         "Inventory: access your belt, backpack etc., and items on the ground.",
                         "Center view: scroll map-display to center around the current man.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 11 :
            b1 = alert3( "Control-panel:  Buttons in column 5:",
                         "Switch to next man, who still has TUs left.      ",
                         "Switch to next man, and mark current man as done.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 12 :
            b1 = alert3( "Control-panel:  Buttons in column 6:",
                         "Show/hide other levels (e.g. show roof of buildings etc.)",
                         "Config-Dialog: adjust movement speed, scrolling speed etc.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 13 :
            b1 = alert3( "Control-panel:  Buttons in column 7:",
                         "End your turn.",
                         "Quit: abort mission (same as ESC).",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 14 :
            b1 = alert3( "Control-panel:  green&&red buttons at the lower left:",
                         "Green: Reserve no time, move freely.                   ",
                         "Red:   Reserve time for aimed shot, snapshot, autofire.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 15 :
            b1 = alert3( "Control-panel:  big brown button in the center",
                         "(shows turn-number):",
                         "Invoke statistics-display for current man.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mapviewer:
        case HELP_MAPVIEW - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MAPVIEW + 0 :
            b1 = alert3( "The map-viewer shows a small overhead-view of the map.",
                         "To look at other levels, use the up&&down-buttons at the left.",
                         "To leave, click on the red OK-button, or press ESC.",
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MAPVIEW + 1 :
            b1 = alert3( "A click on the map-area centers the view to this spot.",
                         "",
                         "Unexplored areas are shown as black.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_MAPVIEW + 2 : 
            b1 = alert3( "Yellow dots: your men.             ",
                         "Red dots   : enemy soldiers.       ",
                         "Blue dots  : map-tiles with damage.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mindprobe:
        case HELP_MINDPROBE - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MINDPROBE + 0 :
            b1 = alert3( "With a mindprobe you can look at the stats of enemy soldiers.",
                         "(Todo)",
                         exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MINDPROBE + 1 : 
            b1 = alert3( "Mindprobe:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Motion-Scanner:
        case HELP_SCANNER - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_SCANNER + 0 :
            b1 = alert3( "A Motion-Scanner indicates motion during the last enemy turn.",
                         "(Todo)",
                         exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_SCANNER + 1 : 
            b1 = alert3( "Motion-Scanner:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Medi-kit:
        case HELP_MEDIKIT - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MEDIKIT + 0 :
            b1 = alert3( "A Medi-kit can heal wounded soldiers.",
                         "(Todo)",
                         exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MEDIKIT + 1 : 
            b1 = alert3( "Medi-kit:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for PSI-amp :
        case HELP_PSI_AMP - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_PSI_AMP + 0 :
            b1 = alert3( "To use a Psi-Amp, a soldiers needs high psi-strength.",
                         "(Todo)",
                         exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_PSI_AMP + 1 : 
            b1 = alert3( "Psi-amp",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Blaster-Launcher:
        case HELP_BLASTER - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_BLASTER + 0 :
            b1 = alert3( "The Blaster-Launcher is an advanced alien rocket-launcher.",
                         "(Todo)",
                         exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_BLASTER + 1 : 
            b1 = alert3( "Blaster-Launcher:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Inventory:
        case HELP_INVENTORY - 1 :
            help( HELP_U2K_INDEX );
            break;
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
        case HELP_INVENTORY + 5 : 
            // Todo: Flying Armor
            b1 = alert3( "The armor cannot be changed during a battle.",
                         "Flying armor does not work yet (it only has stronger armor).",
                         "Click on the picture of the soldier to get to his stats.",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Stats-Display:
        case HELP_STATS - 1 :
            help( HELP_U2K_INDEX );
            break;
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
                         "Stun: shown as an overlay to health. When the stun-value",
                         "reaches the value of health, the soldier gets unconscious.",
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
        case HELP_STATS + 6 :  // Todo: Knife
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
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Endgame-Stats:
        case HELP_ENDGAME - 1 :
            help( HELP_U2K_INDEX );
            break;
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
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Network - Connect to Server / Chat:
        case HELP_NET - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_NET + 0 :
            b1 = alert3( "Connect to an UFO2000-Server on your LAN (e.g. 192.168.1.2:2000)",
                         "or on the Internet (e.g. the offical server lxnt.info:2001).",
                         exit,
                         NULL, ok, next, kp_0, kp_ok, kp_next);
            break;
        case HELP_NET + 1 :
            b1 = alert3( "To get a valid login-name at the official server,",
                         "register at http://ufo2000.lxnt.info/admin.php",
                         "Registration is free, and no personal data is required.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 2 :
            b1 = alert3( "To chat, just type your message, and press ENTER.",
                         "To challenge another player, or accept his/her challenge,",
                         "click on his/her name in the player-list at the right.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 3 :
            b1 = alert3( "Colors of the playernames:",
                         "White: You.                             ",
                         "Gray : Someone else, available for chat.",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 4 :
            b1 = alert3( "Red   : He/she is busy, playing with someone else.",
                         "Yellow: You have sent a challenge to him/her.     ",
                         "Green : You can accept a challenge from him/her.  ",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mission-Planner:
        case HELP_PLANNER - 1 :
            help( HELP_U2K_INDEX );
            break;
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
        case HELP_PLANNER + 2 :  // Todo: access to Load,Save,Edit from mission-planner
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
        case HELP_PLANNER + 7 :  // ?? explain turn vs. half-turn
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
            b1 = alert3( "Gamerules - Map Exploration Level at start of game:",
                         "0: map not explored, 1: deployment-area explored, 2: whole map explored.",
                         "Unexplored (not yet seen) parts of the map are shown as black.",
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

