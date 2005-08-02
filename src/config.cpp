/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2004  ufo2000 development team

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
#include "spk.h"
#include "video.h"
#include "config.h"
#include "multiplay.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"

extern void install_timers(int _speed_unit, int _speed_bullet, int _speed_mapscroll);
extern void uninstall_timers();
extern int speed_unit;
extern int speed_bullet;
extern int speed_mapscroll;
extern int mapscroll;

static char *gen = "General";
static char *edit = "Editor";
static char *serv = "Server";
static char *flag = "Flags";

static int base_accuracy = 120;
static int preferred_screen_xres = 640;
static int preferred_screen_yres = 400;
static int min_color_depth = 8;
static std::string menu_midi_file_name;
static std::string setup_midi_file_name;
static std::string editor_midi_file_name;
static std::string combat1_midi_file_name;
static std::string combat2_midi_file_name;
static std::string win_midi_file_name;
static std::string lose_midi_file_name;
static std::string net1_midi_file_name;
static std::string net2_midi_file_name;

static std::string loading_image_file_name;
static std::string menu_image_file_name;
static std::string endturn_image_file_name;
static std::string win_image_file_name;
static std::string lose_image_file_name;

std::string g_setup_f5;
std::string g_setup_f6;
std::string g_setup_f7;
std::string g_setup_f8;

std::string g_server_host;
std::string g_server_login;
std::string g_server_password;
std::string g_server_proxy;
std::string g_server_proxy_login;
int         g_server_autologin;

static std::string console_font_file;
static int music_volume;

const char *cfg_get_console_font_file()
{
    return console_font_file.c_str();
}

int cfg_get_music_volume()                    { return music_volume; } 
int cfg_get_base_accuracy()                   { return base_accuracy; } 
int cfg_get_screen_x_res()                    { return preferred_screen_xres; } 
int cfg_get_screen_y_res()                    { return preferred_screen_yres; } 
int cfg_get_min_color_depth()                 { return min_color_depth; } 

const std::string &cfg_get_server_host()      { return g_server_host; } 
const std::string &cfg_get_server_login()     { return g_server_login; } 
const std::string &cfg_get_server_password()  { return g_server_password; } 
const std::string &cfg_get_server_proxy()     { return g_server_proxy; }
const std::string &cfg_get_server_proxy_login() { return g_server_proxy_login; }

const char *cfg_get_menu_music_file_name()    { return menu_midi_file_name.c_str(); } 
const char *cfg_get_setup_music_file_name()   { return setup_midi_file_name.c_str(); } 
const char *cfg_get_editor_music_file_name()  { return editor_midi_file_name.c_str(); } 
const char *cfg_get_combat1_music_file_name() { return combat1_midi_file_name.c_str(); } 
const char *cfg_get_combat2_music_file_name() { return combat2_midi_file_name.c_str(); } 
const char *cfg_get_win_music_file_name()     { return win_midi_file_name.c_str(); } 
const char *cfg_get_lose_music_file_name()    { return lose_midi_file_name.c_str(); } 
const char *cfg_get_net1_music_file_name()    { return net1_midi_file_name.c_str(); } 
const char *cfg_get_net2_music_file_name()    { return net2_midi_file_name.c_str(); } 

const char *cfg_get_loading_image_file_name() { return loading_image_file_name.c_str(); }
const char *cfg_get_menu_image_file_name()    { return menu_image_file_name.c_str(); }
const char *cfg_get_endturn_image_file_name() { return endturn_image_file_name.c_str(); }
const char *cfg_get_win_image_file_name()     { return win_image_file_name.c_str(); }
const char *cfg_get_lose_image_file_name()    { return lose_image_file_name.c_str(); }

const std::string &cfg_get_setup_f5()         { return g_setup_f5; }
const std::string &cfg_get_setup_f6()         { return g_setup_f6; }
const std::string &cfg_get_setup_f7()         { return g_setup_f7; }
const std::string &cfg_get_setup_f8()         { return g_setup_f8; }

void loadini()
{
	set_config_file(F("$(home)/ufo2000.ini"));

    SCREEN2W             = get_config_int(gen,       "width",          430);
    SCREEN2H             = get_config_int(gen,       "height",         280);

    MAP_WIDTH            = get_config_int(gen,       "map_width",        4);
    MAP_HEIGHT           = get_config_int(gen,       "map_height",       4);
    speed_unit           = get_config_int(gen,       "speed_unit",      30);
    speed_bullet         = get_config_int(gen,       "speed_bullet",    30);
    speed_mapscroll      = get_config_int(gen,       "speed_mapscroll", 30);
    mapscroll            = get_config_int(gen,       "mapscroll",       10);
    mouse_sens           = get_config_int(gen,       "mouse_sens",      14);

    set_console_font_size(get_config_int(gen, "console_font_size", 9));

    local_platoon_size   = get_config_int(edit,      "platoon_size",     1);

    g_setup_f5           = get_config_string(edit,   "quick_setup_f5", "Rifleman");
    g_setup_f6           = get_config_string(edit,   "quick_setup_f6", "Scout");
    g_setup_f7           = get_config_string(edit,   "quick_setup_f7", "HeavyWeapons");
    g_setup_f8           = get_config_string(edit,   "quick_setup_f8", "Marksman");

    g_time_limit         = get_config_int(gen,       "time_limit", -1);

// base_accuracy         = get_config_int(gen,       "accuracy",        75);
    preferred_screen_xres  = get_config_int(gen,     "screen_x_res",   640);
    preferred_screen_yres  = get_config_int(gen,     "screen_y_res",   400);
    min_color_depth        = get_config_int(gen,     "color_depth",      8);
    menu_midi_file_name    = get_config_string(gen,  "menu_music",    "$(xcom)/sound/gmstory.mid");
    setup_midi_file_name   = get_config_string(gen,  "setup_music",   "$(xcom)/sound/gmenbase.mid");
    editor_midi_file_name  = get_config_string(gen,  "editor_music",  "$(xcom)/sound/gmdefend.mid");
    combat1_midi_file_name = get_config_string(gen,  "combat_music1", "$(xcom)/sound/gmtactic.mid");
    combat2_midi_file_name = get_config_string(gen,  "combat_music2", "$(xcom)/sound/gmtactic.mid");
    win_midi_file_name     = get_config_string(gen,  "win_music",     "$(xcom)/sound/gmwin.mid");
    lose_midi_file_name    = get_config_string(gen,  "lose_music",    "$(xcom)/sound/gmlose.mid");
    net1_midi_file_name    = get_config_string(gen,  "net_music1",    "$(xcom)/sound/gmgeo1.mid");
    net2_midi_file_name    = get_config_string(gen,  "net_music2",    "$(xcom)/sound/gmgeo2.mid");

    loading_image_file_name = get_config_string(gen, "loading_image", "$(ufo2000)/arts/text_back.jpg");
    menu_image_file_name    = get_config_string(gen, "menu_image",    "$(xcom)/ufointro/pict2.lbm");
    endturn_image_file_name = get_config_string(gen, "endturn_image", "$(xcom)/ufograph/tac00.scr");
    win_image_file_name     = get_config_string(gen, "win_image",     "$(xcom)/geograph/back01.scr");
    lose_image_file_name    = get_config_string(gen, "lose_image",    "$(xcom)/geograph/back02.scr");

    console_font_file       = get_config_string(gen,  "console_font_file", "$(ufo2000)/fonts/DejaVuSansMono-Roman.ttf");
    music_volume            = get_config_int(gen,     "music_volume", 255);

    g_server_host           = get_config_string(serv, "host", "127.0.0.1");
    g_server_proxy          = get_config_string(serv, "http_proxy", "auto");
    g_server_autologin      = get_config_int(serv,    "autologin", 0);
}

void saveini()
{
    set_config_file(F("$(home)/ufo2000.ini"));

    set_config_int(gen,     "width",           SCREEN2W);
    set_config_int(gen,     "height",          SCREEN2H);

    set_config_int(gen,     "speed_unit",      speed_unit);
    set_config_int(gen,     "speed_bullet",    speed_bullet);
    set_config_int(gen,     "speed_mapscroll", speed_mapscroll);
    set_config_int(gen,     "mapscroll",       mapscroll);
    set_config_int(gen,     "mouse_sens",      mouse_sens);

    set_config_string(gen,  "console_font_file", console_font_file.c_str());
    set_config_int(gen,     "console_font_size", get_console_font_size());               
    
    set_config_int(gen,		"music_volume", music_volume);
    
    set_config_int(edit,    "platoon_size",    local_platoon_size);

    set_config_string(edit, "quick_setup_f5",  g_setup_f5.c_str() );
    set_config_string(edit, "quick_setup_f6",  g_setup_f6.c_str() );
    set_config_string(edit, "quick_setup_f7",  g_setup_f7.c_str() );
    set_config_string(edit, "quick_setup_f8",  g_setup_f8.c_str() );

    set_config_string(serv, "host",            g_server_host.c_str());
    set_config_string(serv, "login",           g_server_login.c_str());
    set_config_string(serv, "password",        g_server_password.c_str());
    set_config_int(serv,    "autologin",       g_server_autologin);
    
    set_config_int(flag,	"F_ENDTURNSND",	   FLAGS & F_ENDTURNSND ? 1 : 0);
    set_config_int(flag,	"F_SECONDSIT",	   FLAGS & F_SECONDSIT ? 1 : 0);
    set_config_int(flag,	"F_TOOLTIPS",	   FLAGS & F_TOOLTIPS ? 1 : 0);
}

int sethotseatplay()
{
	net->gametype = GAME_TYPE_HOTSEAT;
	return 1;
}

void set_language(const char *lang)
{
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "SetLanguage");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, lang);
        lua_safe_call(L, 1, 0);
    }
    lua_settop(L, stack_top);

    translation_cache.clear();
}

#define FG 0
#define BG 1

// Order of GUI-elements in config_dlg:
#define SPEED_UNIT       2
#define SPEED_BULLET     3
#define SPEED_MAPSCROLL  4
#define MAPSCROLL        5
#define VOLUME			 6
#define FONT_SIZE		 7
#define MOUSE_SENS       8
#define FLAG_ETS         9
#define FLAG_SS          10
#define FLAG_TT          11
#define LANG             12
#define OK_BUTTON        13
#define MAX_VALUE        99
                                                 
static DIALOG *config_dlg = NULL;                                                 
                                                 
static int d_slider_pro2(int msg, DIALOG *d, int c)
{
	char s[100];
	int v;
	v = d_slider_proc(msg, d, c);
	switch (msg) {
		case MSG_DRAW:
			sprintf(s, "%2d", d->d2);
			text_mode(d->bg);
			gui_textout(screen, s, d->x - 18, d->y + 4, d->fg, 0);
			break;    
		case MSG_CLICK:
        case MSG_CHAR:
            if (d != &config_dlg[VOLUME])
            {
            	if (d == &config_dlg[FONT_SIZE]) {
                	if (d->d2 < 9) d->d2 = 9;
                } else {
                	if (d->d2 < 1) d->d2 = 1;
                }
            }

			scare_mouse();
            SEND_MESSAGE(d, MSG_DRAW, 0);
            unscare_mouse();
            break;
		default:
			break;
	}
	return v;
}                                                   

// Todo: automatic detection of available languages
std::vector<std::string> language_codes;
std::vector<std::string> language_names;

inline void add_lng(std::string code, std::string name)
{
    language_codes.push_back(code);
    language_names.push_back(name);
}

void init_languages()
{
    language_codes.clear(); language_names.clear();
    add_lng("be", "Byelorussian");
    add_lng("en", "English");
    //add_lng("fr", "French");  // actually no translation, though ufo2000-fr.po is present
    add_lng("de", "German");
    add_lng("ru", "Russian");
    add_lng("es", "Spanish");
}

static int lang_change_button_proc(int msg, DIALOG *d, int c)
{
    int result = d_button_proc(msg, d, c);
    if (result == D_CLOSE) {
        if (language_names.size() > 1)
            d->d1 = gui_select_from_list(180, 120, "Language:", language_names, d->d1);

        char *temp = new char[strlen(language_codes[d->d1].c_str())];
        strcpy(temp, language_codes[d->d1].c_str());

        d->w = text_length(font, temp) + 6;
        d->dp = (void *)temp;

        return D_REDRAW;
    }
    return result;
}

/**
 * Animation speed and other settings dialog available from battlescape
 */
void configure()
{
    set_config_file(F("$(home)/ufo2000.ini"));
    init_languages();
    char *temp = new char[strlen(get_config_string("System", "language", "en"))];
    strcpy(temp, get_config_string("System", "language", "en"));

    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    DIALOG config_dlg[] = {
        //                   x    y   w    h   fg  bg key flags d1 d2  dp   dp2   dp3
        { d_shadow_box_proc, 0,   0, 550, 224, FG, BG, 0, 0,    0, 0, NULL, NULL, NULL },
        { d_text_proc,       8,   8, 534,  16, FG, BG, 0, 0, 0, 0, (void *)_("Options"), NULL, NULL },
        { d_slider_pro2,    24,  40, 136,  16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
        { d_slider_pro2,    24,  64, 136,  16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
        { d_slider_pro2,    24,  88, 136,  16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
        { d_slider_pro2,    24, 112, 136,  16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
        { d_slider_pro2,	24, 136, 136,  16, FG, BG, 0, 0, MAX_VALUE, 4, NULL, NULL, NULL },
        { d_slider_pro2,	24, 160, 136,  16, FG, BG, 0, 0, 24, 4, NULL, NULL, NULL },
        { d_slider_pro2,    24, 184, 136,  16, FG, BG, 0, 0, 16, 4, NULL, NULL, NULL },
        { d_check_proc,	   340,  40, 192,  16, FG, BG, 0, 0, 1, 0, (void *)_("end turn sound"), NULL, NULL },
        { d_check_proc,	   340,  64, 192,  16, FG, BG, 0, 0, 1, 0, (void *)_("start sitting if second"), NULL, NULL },
        { d_check_proc,	   340,  88, 192,  16, FG, BG, 0, 0, 1, 0, (void *)_("icon panel tooltips"), NULL, NULL },
        { lang_change_button_proc, 340, 132, 100, 16, FG, BG, 0, D_EXIT, -1, 0, (void *)temp, NULL, NULL },
        { d_button_proc,   400, 200,  64,  16, FG, BG, 0, D_EXIT, 0, 0, (void *)_("OK"), NULL, NULL },
        { d_button_proc,   472, 200,  64,  16, FG, BG, 0, D_EXIT | D_GOTFOCUS, 0, 0, (void *)_("Cancel"), NULL, NULL },
        { d_text_proc,     176,  44,  88,  16, FG, BG, 0, 0, 0, 0, (void *)_("movement speed"), NULL, NULL },
        { d_text_proc,     176,  68, 104,  16, FG, BG, 0, 0, 0, 0, (void *)_("fire speed"), NULL, NULL },
        { d_text_proc,     176,  92, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("scroll speed"), NULL, NULL },
        { d_text_proc,     176, 116, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("mapscroll points"), NULL, NULL },
        { d_text_proc,	   176, 140, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("music volume"), NULL, NULL },
        { d_text_proc,	   176, 164, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("console font size"), NULL, NULL },
        { d_text_proc,	   176, 188, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("mouse sensitivity"), NULL, NULL },
        { d_text_proc,	   372, 137, 128,  16, FG, BG, 0, 0, 0, 0, (void *)_("language"), NULL, NULL },
        { d_yield_proc,      0,   0,   0,   0,  0,  0, 0, 0, 0, 0, NULL, NULL, NULL},
        { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
    };
    
    ::config_dlg = config_dlg;

    // Start-values for sliders:
    config_dlg[SPEED_UNIT].d2      = speed_unit;
    config_dlg[SPEED_BULLET].d2    = speed_bullet;
    config_dlg[SPEED_MAPSCROLL].d2 = speed_mapscroll;
    config_dlg[MAPSCROLL].d2       = mapscroll;
    config_dlg[VOLUME].d2		   = music_volume * MAX_VALUE / 255;
    config_dlg[FONT_SIZE].d2	   = get_console_font_size();
    config_dlg[MOUSE_SENS].d2      = mouse_sens;
    if (FLAGS & F_ENDTURNSND) config_dlg[FLAG_ETS].flags = D_SELECTED;
    if (FLAGS & F_SECONDSIT) config_dlg[FLAG_SS].flags = D_SELECTED;
    if (FLAGS & F_TOOLTIPS) config_dlg[FLAG_TT].flags = D_SELECTED;
    config_dlg[LANG].w             = text_length(font, temp) + 6;

	centre_dialog(config_dlg);
	set_dialog_color(config_dlg, COLOR_BLACK1, COLOR_WHITE);

	if (popup_dialog(config_dlg, -1) == OK_BUTTON) {
		speed_unit = config_dlg[SPEED_UNIT].d2;
		speed_bullet = config_dlg[SPEED_BULLET].d2;
		speed_mapscroll = config_dlg[SPEED_MAPSCROLL].d2;
		mapscroll = config_dlg[MAPSCROLL].d2;
		music_volume = config_dlg[VOLUME].d2 * 255 / MAX_VALUE;
		set_console_font_size(config_dlg[FONT_SIZE].d2);
		mouse_sens = config_dlg[MOUSE_SENS].d2;
		set_mouse_sens(mouse_sens);
		
		if (config_dlg[FLAG_ETS].flags == D_SELECTED) FLAGS |= F_ENDTURNSND;
		else FLAGS &= ~F_ENDTURNSND;
			
		if (config_dlg[FLAG_SS].flags == D_SELECTED) FLAGS |= F_SECONDSIT;
		else FLAGS &= ~F_SECONDSIT;
			
		if (config_dlg[FLAG_TT].flags == D_SELECTED) FLAGS |= F_TOOLTIPS;
		else FLAGS &= ~F_TOOLTIPS;

        if (config_dlg[LANG].d1 != -1) {
            std::string lang = language_codes[config_dlg[LANG].d1];
            set_config_string("System", "language", lang.c_str());
            set_language(lang.c_str());
        }

		uninstall_timers();
		install_timers(speed_unit, speed_bullet, speed_mapscroll);
	}
}
