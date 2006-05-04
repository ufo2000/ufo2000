/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2006  ufo2000 development team

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
/*
 * -- Most GUI control locations : April 16 2006 -- 
 * Game_init procedure : main.cpp ~line 830
 */
#ifndef SKIN_GUI_H
#define SKIN_GUI_H

#include "global.h"
#include "sprite.h"

/*class SkinFeature;*/
/**
 * SkinFeature object. Reaches for properties of a screen feature defined in standard-gui.lua
 * Used to get GUI related properties such as window placement, fonts, images, button sizes, etc.
 */ 
class SkinFeature
{
private:
    const char *m_feature;  
    const char *m_fscreen; 
    ALPHA_SPRITE *feat_get_bitmap(const char *bitmap_name);
    int feat_get_color_int(const char *property_name, int index);
    int feat_get_placement_int(const char *property_name, int index);
    bool feat_placement_exists(const char *property_name);
public:
    SkinFeature();
    SkinFeature(const char *feature_name, const char *screen_name);
    ~SkinFeature();
    /* Return backgrounds, normal and mouse-over.
    @todo: Consider bg colors. */
    BITMAP *bg_norm();
    BITMAP *bg_over();
    /* The following functions return placement properties (bounding box) */
    int get_x1();
    int get_y1();
    int get_x2();
    int get_y2();
    int get_width();
    int get_height();
    /* These functions do the same but consider the padding correction. (content) */
    int get_pd_x1();
    int get_pd_y1();
    int get_pd_x2();
    int get_pd_y2();
    int get_pd_width();
    int get_pd_height();
    /* Returns padding (defined as distance of content boundaries from bounding box boundaries) */
    int get_padding();
    /* Returns the font to be used on that feature. */
    //  FONT *get_font() { return m_font; };
    //int normal_text_color() { return 0; }; 
    //int active_text_color() { return 0; }; 
    /*  - Other properties */
    const char *screen() { return m_fscreen; };
    const char *name() { return m_feature; };
};
 
/*class SkinInterface;*/
/**
 * SkinInterface object. Reaches for properties defined in standard-gui.lua
 * Used to get GUI related properties such as window placement, fonts, images, button sizes, etc.
 * -- globals used --
 * SCREEN_H, SCREEN_W
 * -- Usage --
 * gui = new SkinInterface();
 * gui->function( params );
 * gui->Feature( feature name )->function( params );
 */ 
class SkinInterface
{
private:
    friend class SkinFeature;

    ALPHA_SPRITE *m_background;
    const char *m_screen;  
    /*  Returns values from the screen itself (Screen) */
    ALPHA_SPRITE *get_bitmap(const char *bitmap_name);
public:
    SkinInterface();
    SkinInterface(const char *screen_name);
    ~SkinInterface();
    /*  - Feature subclass */
    SkinFeature *Feature(const char *feature_name);    
    /*  - Background related properties */
    BITMAP *background();
    /*  - Other properties */
    const char *name() { return m_screen; };
};

#endif
