/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

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

#include "stdafx.h"

#include "global.h"
#include "video.h"
#include "icon.h"
#include "sound.h"
#include "explo.h"
#include "map.h"
#include "multiplay.h"
#include "platoon.h"
#include "soldier.h"
#include "wind.h"
#include "config.h"
#include "scenario.h"
#include "colors.h"
#include "text.h"

//! firemenu foreground color
#define _FG COLOR_WHITE
//! firemenu background color
#define _BG COLOR_BLACK1

// not used / debug
void cprintf(char *str)
{
	text_mode(0); textprintf(screen, font, 1, 1, COLOR_WHITE, "%s", str);
}

/**
 * Read definitions for control-panel (icon-area) on battlescape
 * from file init-scripts/standard-icons.lua,
 * then display icons on screen.
 */
Icon::Icon()
{
	int i;
	int j;

    //names in .lua file - do not translate !
	item[I_LEFT].name            = "LeftItem";
	item[I_RIGHT].name           = "RightItem";
	
	button[B_MAN_UP].name        = "ManUp";
	button[B_MAN_DOWN].name      = "ManDown";
	button[B_VIEW_UP].name       = "ViewUp";
	button[B_VIEW_DOWN].name     = "ViewDown";
	button[B_MAP].name           = "Map";
	button[B_CROUCH].name        = "Crouch";
	button[B_INVENTORY].name     = "Inventory";
	button[B_CENTER_VIEW].name   = "CenterView";
	button[B_NEXT_MAN].name      = "NextMan";
	button[B_NEXT_MAN_2].name    = "NextMan2";
	button[B_TOGGLE_ROOF].name   = "ToggleRoof";
	button[B_OPTIONS].name       = "Options";
	button[B_DONE].name          = "Done";
	button[B_EXIT].name          = "Exit";

	button[B_MAN_STATS].name     = "ManStats"; 
	button[B_BARCHART].name      = "BarChart";

	text[T_TURN_NUMBER].name     = "TurnNumber";
	text[T_MAN_NAME].name        = "ManName";
	
	attribute[A_TIME_UNITS].name = "TimeUnits";
	attribute[A_ENERGY].name     = "Energy";
	attribute[A_HEALTH].name     = "Health";
	attribute[A_MORALE].name     = "Morale";
		
	reserve[R_TIME_FREE].name    = "ResTimeFree";
	reserve[R_TIME_AIM].name     = "ResTimeAim";
	reserve[R_TIME_SNAP].name    = "ResTimeSnap";
	reserve[R_TIME_AUTO].name    = "ResTimeAuto";

	int nc[4];
	const char *nd;
	bool custom_icons = false;
	
	int stack_top = lua_gettop(L);
	
	if (exists(F("$(ufo2000)/init-scripts/icons.lua")))
		lua_safe_dofile(L, F("$(ufo2000)/init-scripts/icons.lua"));
	else
		lua_safe_dofile(L, F("$(ufo2000)/init-scripts/standard-icons.lua"));	
	
	//image
	lua_pushstring(L, "Image");
	lua_gettable(L, -2);
	custom_icons = lua_isstring(L, -1);
    if (custom_icons)
	   filename = (std::string)lua_tostring(L, -1);
	lua_pop(L, 1);

	//items
	for (i = 0; i < ITEM_NUMBER; i++) {
   		lua_pushstring(L, item[i].name);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));

		lua_pushstring(L, "Button");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 4; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		
		item[i].button.set_coords(nc[0], nc[1], nc[2], nc[3]);     
	
		lua_pop(L, 1);
	    
		lua_pushstring(L, "Image");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 2; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		item[i].ImageX = nc[0];
		item[i].ImageY = nc[1];
	
		lua_pop(L, 1);
	
		lua_pushstring(L, "DigitsCoords");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 2; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		item[i].DigitsX = nc[0];
		item[i].DigitsY = nc[1];
	
		lua_pop(L, 1);
		
		lua_pushstring(L, "DigitsRoundsColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		item[i].DigitsRoundsColor = nc[0];
				
		lua_pushstring(L, "DigitsPrimeColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		item[i].DigitsPrimeColor = nc[0];
		
		lua_pop(L, 1);
	}
	
	//buttons
	for (i = 0; i < BUTTON_NUMBER; i++) {
		lua_pushstring(L, button[i].name);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (int j = 1; j <= 4; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		button[i].set_coords(nc[0], nc[1], nc[2], nc[3]);
	
		lua_pop(L, 1);
	}
	
	//texts
	for (i = 0; i < TEXT_NUMBER; i++) {
		lua_pushstring(L, text[i].name);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		lua_pushstring(L, "Coords");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 2; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		text[i].x = nc[0];
		text[i].y = nc[1];
		
		lua_pop(L, 1);
	
		lua_pushstring(L, "Color");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		text[i].color = nc[0];
		
		lua_pushstring(L, "Font");
		lua_gettable(L, -2);
		ASSERT(lua_isstring(L, -1));
		nd = lua_tostring(L, -1);
		lua_pop(L, 1);
	
		if (nd == (std::string)"small")
			text[i].font = g_small_font;
		else if (nd == (std::string)"normal")
			text[i].font = font;
		else if (nd == (std::string)"large")
			text[i].font = large;
		else 
			ASSERT(false);
	
		lua_pop(L, 1);
	}
	
	//attributes	
	for (i = 0; i < ATTRIBUTE_NUMBER; i++) {
		lua_pushstring(L, attribute[i].name);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
		
		lua_pushstring(L, "BarZeroCoords");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 2; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		attribute[i].BarX = nc[0];
		attribute[i].BarY = nc[1];
	
		lua_pop(L, 1);
	
		lua_pushstring(L, "BarDirection");
		lua_gettable(L, -2);
		ASSERT(lua_isstring(L, -1));
		nd = lua_tostring(L, -1);
		lua_pop(L, 1);
	
		if (nd == (std::string)"horizontal")
			attribute[i].BarDirection = dir_hor;
		else if (nd == (std::string)"vertical")
			attribute[i].BarDirection = dir_vert;
		else 
			ASSERT(false);
		
		lua_pushstring(L, "FColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		attribute[i].FColor = nc[0];
	
		lua_pushstring(L, "BColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		attribute[i].BColor = nc[0];
	
		lua_pushstring(L, "DigitsCoords");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 2; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		attribute[i].DigitsX = nc[0];
		attribute[i].DigitsY = nc[1];
	
		lua_pop(L, 1);
		
		lua_pushstring(L, "DigitsColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		attribute[i].DigitsColor = nc[0];
		
		lua_pop(L, 1);
	}

	//stun bar color	
	lua_pushstring(L, "StunColor");
	lua_gettable(L, -2);
	ASSERT(lua_isnumber(L, -1));
	nc[0] = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	//reserve time buttons
		for (i = 0; i < RESERVE_NUMBER; i++) {
		lua_pushstring(L, reserve[i].name);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		lua_pushstring(L, "Button");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 4; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		reserve[i].button.set_coords(nc[0], nc[1], nc[2], nc[3]);
		
		lua_pop(L, 1);
		
		lua_pushstring(L, "BorderCoords");
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
	
		for (j = 1; j <= 4; j++) {
			lua_pushnumber(L, j);
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1));
			nc[j - 1] = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	
		reserve[i].BorderX1 = nc[0];
		reserve[i].BorderY1 = nc[1];
		reserve[i].BorderX2 = nc[2];
		reserve[i].BorderY2 = nc[3];
		
		lua_pop(L, 1);
	
		lua_pushstring(L, "BorderColor");
		lua_gettable(L, -2);
		ASSERT(lua_isnumber(L, -1));
		nc[0] = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		reserve[i].BorderColor = nc[0];
		
		lua_pop(L, 1);
	}
	
	stun_color = nc[0];
			
	lua_settop(L, stack_top);

	tac00 = new SPK("$(xcom)/ufograph/tac00.scr");                     
	
	if (custom_icons) {
		BITMAP *custom_image;
		custom_image = load_bitmap(F(filename.c_str()), NULL);
		ASSERT(custom_image);
		width  = custom_image->w;
		height = custom_image->h;
		
		iconsbmp = create_bitmap(width, height);
		blit(custom_image, iconsbmp, 0, 0, 0, 0, iconsbmp->w, iconsbmp->h);
		destroy_bitmap(custom_image);
	} else {
		SPK *spk = new SPK("$(xcom)/ufograph/icons.pck");
		BITMAP *image = create_bitmap(320, 200); clear(image);
		spk->show(image, 0, 0);
		delete(spk);
		
		width  = 320;
		height =  57;

        iconsbmp = create_bitmap(width, height);
		blit(image, iconsbmp, 0, 144, 0, 0, iconsbmp->w, iconsbmp->h); 
		destroy_bitmap(image);
	}
		
	x = (SCREEN2W - width) / 2;
	y = SCREEN2H - height;
}

/**
 * Cleanup: icon-bitmap and graphic-resource
 */
Icon::~Icon()
{
	destroy_bitmap(iconsbmp);
	delete(tac00);
}

/**
 * Draw plain control-panel / icon-area, 
 * then add infos about currently active soldier
 */
void Icon::draw()
{
	//blit(iconsbmp, screen2, 0, 0, x, y, iconsbmp->w, iconsbmp->h);
	draw_sprite(screen2, iconsbmp, x, y);
	info();
}

bool firemenu_dialog_proc_exit = 0;

int firemenu_dialog_proc(int msg, DIALOG * d, int c)
{
	if (mouse_b & 2) {
		while (mouse_b & 2) yield_timeslice();
		firemenu_dialog_proc_exit = 1;
	return D_CLOSE;
	}
	return d_button_proc(msg, d, c);
}

/**
 * Menu to select firing-mode (aimed, snapshot, auto) for weapon in hand, 
 * or throwing
 */
void Icon::firemenu(int iplace)
{
	TARGET = 0;

	if (sel_man == NULL)
		return ;
	if (sel_man->ismoving())
		return ;

	Item *it = sel_man->item(iplace);
	if (it == NULL)
		return ;

	static char dstr[5][100];

	static DIALOG the_dialog[] = {
		//         dialog proc,  x,   y,   w,  h,  fg,  bg, key,  flags, d1, d2,              dp,  dp2,  dp3  
		{ firemenu_dialog_proc, 41, 150, 237, 25, _FG, _BG,   0, D_EXIT,  0,  0, (void *)dstr[0], NULL, NULL},
		{ firemenu_dialog_proc, 41, 120, 237, 25, _FG, _BG,   0, D_EXIT,  0,  0, (void *)dstr[1], NULL, NULL},
		{ firemenu_dialog_proc, 41,  90, 237, 25, _FG, _BG,   0, D_EXIT,  0,  0, (void *)dstr[2], NULL, NULL},
		{ firemenu_dialog_proc, 41,  60, 237, 25, _FG, _BG,   0, D_EXIT,  0,  0, (void *)dstr[3], NULL, NULL},
		{ firemenu_dialog_proc, 41,  30, 237, 25, _FG, _BG,   0, D_EXIT,  0,  0, (void *)dstr[4], NULL, NULL},
		{ d_yield_proc,          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
		{                 NULL,  0,   0,   0,  0, _FG, _BG,   0,      0,  0,  0,            NULL, NULL, NULL}
	};

	for (int d = 0; d < 5; d++) {
		the_dialog[d].x = (SCREEN2W - 237) / 2;
		the_dialog[d].y = y + 6 - d * 30;
	}

	int waccur[5], wtime[5];
	Action waction[5];

	if (it != NULL && scenario->can_use(sel_man, it)) {
		int i = 0;
        sprintf(dstr[i], _("CANCEL") );
		the_dialog[i].proc = firemenu_dialog_proc;
		i++;

		waccur[i] = sel_man->TAccuracy(100); // !!! no such parameter in obdata.dat
		wtime[i]  = sel_man->required(25);
		if (sel_man->havetime(wtime[i])) {
            sprintf(dstr[i], _("THROW       ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
			the_dialog[i].proc = firemenu_dialog_proc;
			waction[i] = THROW;
			i++;
		}
        
		if (it->is_cold_weapon()) {
			waccur[i] = 100;
			wtime[i]  = sel_man->required(25);
			if (sel_man->havetime(wtime[i])) {
				// More stun rod hack.
				if (it->is_stun_rod())
                    sprintf(dstr[i], _("STUN        ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
                else
                    sprintf(dstr[i], _("PUNCH       ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
				the_dialog[i].proc = firemenu_dialog_proc;
				waction[i] = PUNCH;
				i++;
			}
			if (it->is_knife()) {
				waccur[i] = sel_man->TAccuracy(it->obdata_accuracy(ATHROW));
				wtime[i] = sel_man->required(50);
				if (sel_man->havetime(wtime[i])) {
                    sprintf(dstr[i], _("AIMED THROW ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
					the_dialog[i].proc = firemenu_dialog_proc;
					waction[i] = AIMEDTHROW;
					i++;
				}
			}
		} else
			if (it->is_grenade()) {
				wtime[i] = sel_man->required(24);
				if (sel_man->havetime(wtime[i])) {
					if (it->is_explo()) {
						if (it->delay_time() == 0) {
                            sprintf(dstr[i], _("PRIME EXPLOSIVE     TUs>%02d"), wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = PRIME;
							i++;
						}
					} else {
						if (it->delay_time() == 0) {
							sprintf(dstr[i], _("PRIME GRENADE       TUs>%02d"), wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = PRIME;
							i++;
						}
					}
				}
			} else {
				if ((it->obdata_isGun() && it->haveclip()) || it->is_laser()) {
					if (it->obdata_accuracy(AUTO)) {
						waccur[i] = sel_man->FAccuracy(it->obdata_accuracy(AUTO), it->obdata_twoHanded());
						wtime[i] = sel_man->required(it->obdata_time(AUTO));
						if (sel_man->havetime((wtime[i] + 2) / 3 * 3)) {
                            sprintf(dstr[i], _("AUTO SHOT   ACC>%02d%% TUs>%02d"), waccur[i], (wtime[i] + 2) / 3 * 3);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = AUTOSHOT;
							wtime[i] = (wtime[i] + 2) / 3;      // per 3
							i++;
						}
					}
					if (it->obdata_accuracy(SNAP)) {
						waccur[i] = sel_man->FAccuracy(it->obdata_accuracy(SNAP), it->obdata_twoHanded());
						wtime[i] = sel_man->required(it->obdata_time(SNAP));
						if (sel_man->havetime(wtime[i])) {
                            sprintf(dstr[i], _("SNAP SHOT   ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = SNAPSHOT;
							i++;
						}
					}
					if (it->obdata_accuracy(AIMED)) {
						waccur[i] = sel_man->FAccuracy(it->obdata_accuracy(AIMED), it->obdata_twoHanded());
						wtime[i] = sel_man->required(it->obdata_time(AIMED));
						if (sel_man->havetime(wtime[i])) {
                            sprintf(dstr[i], _("AIMED SHOT  ACC>%02d%% TUs>%02d"), waccur[i], wtime[i]);
							the_dialog[i].proc = firemenu_dialog_proc;
							waction[i] = AIMEDSHOT;
							i++;
						}
					}
				}
			} 
			
		if (i > 1) {                  
			the_dialog[i].proc = NULL;
			//set_mouse_sprite(mouser);
			firemenu_dialog_proc_exit = 0;
			int sel = do_dialog(the_dialog, -1);
			//if (sel == i) return;
			if (firemenu_dialog_proc_exit || sel == 0) return ;

            // Todo: Fire-commands DROP & USE

			if (waction[sel] != PRIME) {
				target.accur  = waccur[sel];
				target.time   = wtime[sel];
				target.action = waction[sel];
				target.item   = it;
				target.place  = iplace;
				TARGET = 1;
			} else {
				target.time = wtime[sel];

				if (it->m_type == PROXIMITY_GRENADE) {
					sel_man->prime_grenade(iplace, -1, target.time);
				} else {
					int delay_time = doprime(it);
					if (delay_time > 0)
						sel_man->prime_grenade(iplace, delay_time, target.time);
				}
			}
		}
	}
}

/**
 * Dialog to prime a grenade: select delay
 */
int Icon::doprime(Item *it)
{
	int DX = (SCREEN2W - 98) / 2;
	int DY = y - 119;
	int DS = 18;

	static DIALOG dPrime[] = {
		//  dialog proc, x                , y                ,  w,  h,  fg,  bg, key,  flags, d1, d2, dp              ,  dp2,  dp3
        { d_button_proc, DX               , DY - 17          , 98, 15, _FG, _BG,   0, D_EXIT,  0,  0, (void *)_("Cancel"), NULL, NULL},
        { d_button_proc, DX               , DY               , DS, DS, _FG, _BG,  48, D_EXIT,  0,  0, (void *)"&0"     , NULL, NULL},
        { d_button_proc, DX + (DS + 2) * 1, DY               , DS, DS, _FG, _BG,  49, D_EXIT,  0,  0, (void *)"&1"     , NULL, NULL},
        { d_button_proc, DX + (DS + 2) * 2, DY               , DS, DS, _FG, _BG,  50, D_EXIT,  0,  0, (void *)"&2"     , NULL, NULL},
        { d_button_proc, DX + (DS + 2) * 3, DY               , DS, DS, _FG, _BG,  51, D_EXIT,  0,  0, (void *)"&3"     , NULL, NULL},
        { d_button_proc, DX + (DS + 2) * 4, DY               , DS, DS, _FG, _BG,  52, D_EXIT,  0,  0, (void *)"&4"     , NULL, NULL},
		{ d_button_proc, DX               , DY + (DS + 2) * 1, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"5"     , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 1, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"6"     , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 1, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"7"     , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 1, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"8"     , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 1, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"9"     , NULL, NULL},
		{ d_button_proc, DX               , DY + (DS + 2) * 2, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"10"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 2, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"11"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 2, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"12"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 2, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"13"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 2, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"14"    , NULL, NULL},
		{ d_button_proc, DX               , DY + (DS + 2) * 3, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"15"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 3, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"16"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 3, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"17"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 3, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"18"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 3, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"19"    , NULL, NULL},
		{ d_button_proc, DX               , DY + (DS + 2) * 4, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"20"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 1, DY + (DS + 2) * 4, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"21"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 2, DY + (DS + 2) * 4, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"22"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 3, DY + (DS + 2) * 4, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"23"    , NULL, NULL},
		{ d_button_proc, DX + (DS + 2) * 4, DY + (DS + 2) * 4, DS, DS, _FG, _BG,   0, D_EXIT,  0,  0, (void *)"24"    , NULL, NULL},
		{ d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
		{          NULL, 0                , 0                ,  0,  0,   0,   0,   0,      0,  0,  0, NULL            , NULL, NULL}
	};

	return do_dialog(dPrime, -1);
}

/**
 * Find out which button of the control-panel is under the mouse-cursor.
 */
int Icon::identify(int mx, int my)
{
    int icon_nr = -1;
    mx -= x; my -= y;
	int i;

    for (i = B_MAN_UP; i <= BUTTON_NUMBER; i++) { 
        if (button[ i ].is_inside(mx, my)) { 
            icon_nr = i; 
            return icon_nr;
        } 
    } 
    // Reserve-time-buttons are a separate class:
    for (i = R_TIME_FREE; i <= RESERVE_NUMBER; i++) { 
        if (reserve[ i ].button.is_inside(mx, my)) {
            icon_nr = i + BUTTON_NUMBER; 
            return icon_nr;
        } 
    } 
    return -1;
}

/**
 * If a button in the control-panel / icon-area was clicked, 
 * execute its command
 */
void Icon::execute(int mx, int my)
{
	mx -= x; my -= y;
    if (item[I_LEFT].button.is_inside(mx, my)) {
		if (MODE != WATCH)
			firemenu(P_ARM_LEFT);
	} else
	if (item[I_RIGHT].button.is_inside(mx, my)) {
		if (MODE != WATCH)
			firemenu(P_ARM_RIGHT);
	} else
	if (button[B_MAN_UP].is_inside(mx, my)) {
		if (MODE != WATCH) {
			// Todo: Flying armor
			if (sel_man->use_elevator(+1))
				map->center(sel_man);
		}
	} else
	if (button[B_MAN_DOWN].is_inside(mx, my)) {
		if (MODE != WATCH) {
			// Todo: Flying armor
			if (sel_man->use_elevator(-1))
				map->center(sel_man);
		}
	} else
	if (button[B_VIEW_UP].is_inside(mx, my)) {
		if (map->sel_lev < map->level - 1)
			map->sel_lev++;
	} else
	if (button[B_VIEW_DOWN].is_inside(mx, my)) {
		if (map->sel_lev > 0)
			map->sel_lev--;
//
// Buttons for reserving time:
//
	} else
	if (reserve[R_TIME_FREE].button.is_inside(mx, my)) {
		if(sel_man)
			sel_man->set_reserve_type(RESERVE_FREE); 
		soundSystem::getInstance()->play(SS_BUTTON_PUSH_1); 
	} else
	if (reserve[R_TIME_AIM].button.is_inside(mx, my)) {
		if(sel_man)		
			sel_man->set_reserve_type(RESERVE_AIM);
		soundSystem::getInstance()->play(SS_BUTTON_PUSH_2); 
	} else
	if (reserve[R_TIME_SNAP].button.is_inside(mx, my)) {
		if(sel_man)
			sel_man->set_reserve_type(RESERVE_SNAP);
		soundSystem::getInstance()->play(SS_BUTTON_PUSH_1); 
	} else
	if (reserve[R_TIME_AUTO].button.is_inside(mx, my)) {
		if(sel_man)
			sel_man->set_reserve_type(RESERVE_AUTO);
		soundSystem::getInstance()->play(SS_BUTTON_PUSH_2);  
	} else
	if (button[B_MAP].is_inside(mx, my)) {
		if (MODE != WATCH)
			MODE = MAP2D;
	} else
	if (button[B_CROUCH].is_inside(mx, my)) {
		if (MODE != WATCH) {
			if ((sel_man != NULL) && (!sel_man->ismoving())) {
				sel_man->change_pose();
			}
		}
	} else
	if (button[B_MAN_STATS].is_inside(mx, my)) {
		if (MODE != WATCH)
			MODE = UNIT_INFO;
//
// Test: Toggle Stats-BarChart between normal & alternate version (Todo)
//
	} else
	if (button[B_BARCHART].is_inside(mx, my)) {
		g_console->printf(COLOR_SYS_OK, "%s", "BarChart");
		soundSystem::getInstance()->play(SS_BUTTON_PUSH_1); 
//
	} else
	if (button[B_INVENTORY].is_inside(mx, my)) {
		if (MODE != WATCH) {
			TARGET = 0;
			if ((sel_man != NULL) && (!sel_man->ismoving())) {
				MODE = MAN;
			}
		}
	} else
	if (button[B_CENTER_VIEW].is_inside(mx, my)) {
		if (sel_man != NULL) {
			map->center(sel_man);
		}
	} else
	if (button[B_NEXT_MAN].is_inside(mx, my)) {	
		TARGET = 0;
		if (sel_man == NULL) {
			sel_man = platoon_local->captain();
			if (sel_man != NULL)
				map->center(sel_man);
		} else if (!sel_man->ismoving()) {
			Soldier *s = sel_man;
			sel_man = platoon_local->next_not_moved_man(sel_man);
			if (s != sel_man)
				map->center(sel_man);
		}
	} else
	if (button[B_NEXT_MAN_2].is_inside(mx, my)) {
		TARGET = 0;
		if (sel_man == NULL) {
			sel_man = platoon_local->captain();
			if (sel_man != NULL)
				map->center(sel_man);
		} else if (!sel_man->ismoving()) {
			sel_man->MOVED = 1;
			Soldier *s = sel_man;
			sel_man = platoon_local->next_not_moved_man(sel_man);
			if (s != sel_man)
				map->center(sel_man);
		}
	} else
	if (button[B_TOGGLE_ROOF].is_inside(mx, my)) {	
		if (FLAGS & F_SHOWLEVELS)
			FLAGS &= ~F_SHOWLEVELS;
		else
			FLAGS |= F_SHOWLEVELS;
	} else
	if (button[B_OPTIONS].is_inside(mx, my)) {
		if (MODE != WATCH)
			configure();
	} else
	if (button[B_DONE].is_inside(mx, my)) {
		if (MODE != WATCH) {
			TARGET = 0;
			if (nomoves())
				send_turn();
		}
	} else
	if (button[B_EXIT].is_inside(mx, my)) {
		simulate_keypress(KEY_ESC << 8);
	}
}

/**
 * Draw infos about current soldier into control-panel / icon-area: 
 * items in hands, name, stats with bargraphs.
 * Show Turn-number where XCOM had the rank-picture.
 */
void Icon::info()
{
	text_mode(-1);
	if (sel_man != NULL) {
		sel_man->drawinfo(x, y);
	}
	
	draw_text(T_TURN_NUMBER, (turn / 2) + 1, "%02d");
		
	if(sel_man)
		switch(sel_man->m_ReserveTimeMode) {
		case RESERVE_FREE:
			reserve[R_TIME_FREE].Draw(x, y);  
			break;
		
		case RESERVE_AIM:
			reserve[R_TIME_AIM].Draw(x, y);
			break;
		
		case RESERVE_SNAP:
			reserve[R_TIME_SNAP].Draw(x, y);
			break;
		
		case RESERVE_AUTO:
			reserve[R_TIME_AUTO].Draw(x, y);
			break;
		}
	else reserve[R_TIME_FREE].Draw(x, y);
}

/**
 * Test if mouse is inside an icon-button
 */
int Icon::inside(int mx, int my)
{
	if ((mx >= x) && (mx <= x + width) && (my >= y) && (my <= y + height))
		return 1;
	else
		return 0;
}

/**
 * Show End-of-turn - Picture
 */ 
void Icon::show_eot()
{
	BITMAP *eot_back = load_back_image(cfg_get_endturn_image_file_name());
	stretch_blit(eot_back, screen, 0, 0, eot_back->w, eot_back->h, 0, 0, SCREEN_W, SCREEN_H);
	destroy_bitmap(eot_back);
}

/**
 * Draw the stun-bar inside the health-bar
 */
void Icon::draw_stun_bar(int x, int y, int val, int maxval)
{
	if (attribute[A_HEALTH].BarDirection == dir_hor) {
		hline(screen2, x + attribute[A_HEALTH].BarX, y + attribute[A_HEALTH].BarY + 1, x + attribute[A_HEALTH].BarX + val, xcom1_color(stun_color)); 
		putpixel(screen2, x + attribute[A_HEALTH].BarX + maxval + 1, y + attribute[A_HEALTH].BarY + 1, xcom1_color(attribute[A_HEALTH].BColor));
	} else {
		vline(screen2, x + attribute[A_HEALTH].BarX + 1, y + attribute[A_HEALTH].BarY, y + attribute[A_HEALTH].BarY - val, xcom1_color(stun_color)); 
		putpixel(screen2, x + attribute[A_HEALTH].BarX + 1, y + attribute[A_HEALTH].BarY + maxval + 1, xcom1_color(attribute[A_HEALTH].BColor));
	}
}

/**
 * Draw value and stats-bar for an attribute (e.g. TU, energy, health, morale)
 */
void Icon::draw_attribute(int attr, int val, int maxval)
{
	attribute[attr].Draw(x, y, val, maxval);
}

/**
 * Draw text into icon-area, e.g. Turn-number
 */
void Icon::draw_text(int txt, char *val)
{
	text[txt].Draw(x, y, val);
}

void Icon::draw_text(int txt, int val, char *format)
{
	text[txt].Draw(x, y, val, format);
}

/**
 * Draw item in hand, with additional info: 
 * available ammo in weapon, delay for primed grenade
 */
void Icon::draw_item(int itm, Item *it, int rounds, int prime, bool primed)
{
	item[itm].Draw(x, y, it);
	if (rounds != -1)
		item[itm].DrawDigits(x, y, rounds, dig_round);
	else if (prime != -1)
		item[itm].DrawDigits(x, y, prime, dig_count);
	else if (primed)
		item[itm].DrawPrimed(x, y);
}

