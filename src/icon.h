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
#ifndef ICON_H
#define ICON_H

#include "font.h"
#include "spk.h"
#include "item.h"
#include "pck.h"

#define B_MAN_UP            0
#define B_MAN_DOWN          1
#define B_VIEW_UP           2
#define B_VIEW_DOWN         3
#define B_MAP               4
#define B_CROUCH            5
#define B_INVENTORY         6
#define B_CENTER_VIEW       7
#define B_NEXT_MAN          8
#define B_NEXT_MAN_2        9
#define B_TOGGLE_ROOF      10
#define B_OPTIONS          11
#define B_DONE             12
#define B_EXIT             13
#define B_MAN_STATS        14
#define B_BARCHART         15
#define BUTTON_NUMBER      16

#define A_TIME_UNITS        0
#define A_ENERGY            1
#define A_HEALTH            2
#define A_MORALE            3
#define ATTRIBUTE_NUMBER    4

#define I_LEFT              0
#define I_RIGHT             1
#define ITEM_NUMBER         2

#define T_TURN_NUMBER       0
#define T_MAN_NAME          1
#define TEXT_NUMBER         2

#define R_TIME_FREE         0
#define R_TIME_AIM          1
#define R_TIME_SNAP         2
#define R_TIME_AUTO         3
#define RESERVE_NUMBER      4

enum BarDir {dir_hor, dir_vert};
enum ItemDigs {dig_round, dig_count};

//icon elements
class IconButton
{
private:
	int x1, y1, x2, y2;
public:
    const char *name;

	void set_coords(int _x1, int _y1, int _x2, int _y2)
	{
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	};
	
    /**
     * Test if coordinates (of mousepointer) are inside the area of the button.
     */
    bool is_inside(int x, int y)
	{
		if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2))
			return true;
		else
			return false;
	};
	
	void Draw(BITMAP *dest, BITMAP *src)
	{
        if (src != NULL)
            blit(src, dest, x1, y1, x1, y1, x2 - x1, y2 - y1);
    };
};

class IconItem
{
public:
	IconButton button;
	int ImageX,  ImageY;
	int DigitsX, DigitsY;
	int DigitsRoundsColor, DigitsPrimeColor;
	
	const char *name;
	
    /**
     * Draw item (weapon) inside one of the hand-boxes of the control-panel.
     */
    void Draw(BITMAP *dest, Item *it)
	{
		int dx = (2 - it->obdata_width())  * 16 / 2;
		int dy = (3 - it->obdata_height()) * 15 / 2;

		PCK::showpck(dest, it->obdata_pInv(), ImageX + dx,  ImageY + dy);
	};
     
	void Draw(BITMAP *dest, BITMAP *src, Item *it)
	{
        button.Draw(dest, src);
        Draw(dest, it);
	};
	
	void DrawDigits(BITMAP *dest, int val, ItemDigs type)
	{
		if (type == dig_round)
			printsmall_x(dest, DigitsX, DigitsY, xcom1_color(DigitsRoundsColor), val);
		else
			printsmall_x(dest, DigitsX, DigitsY, xcom1_color(DigitsPrimeColor), val);
	};	
	
	void DrawPrimed(BITMAP *dest)
	{
		textout(dest, g_small_font, "*", DigitsX, DigitsY - 3, xcom1_color(DigitsPrimeColor));
	};
};

class IconAttribute
{
public:
	int BarX, BarY;
	BarDir BarDirection;
	int FColor, BColor;
	int DigitsX, DigitsY;
	int DigitsColor;    
	
	const char *name;
	
    /**
     * Draw value and barchart for attributes, e.g. TU, health etc. in the control-panel.
     */
	void Draw(BITMAP *dest, int val, int valmax)
	{
		if (BarDirection == dir_hor) {
			hline(dest,    BarX,              BarY,     BarX + valmax + 1, xcom1_color(BColor));
			hline(dest,    BarX,              BarY + 1, BarX + val, xcom1_color(FColor));
			putpixel(dest, BarX + valmax + 1, BarY + 1, xcom1_color(BColor));
			hline(dest,    BarX,              BarY + 2, BarX + valmax + 1, xcom1_color(BColor));
		} else {
			vline(dest,    BarX,     BarY,              BarY - valmax - 1, xcom1_color(BColor));
			vline(dest,    BarX + 1, BarY,              BarY - val, xcom1_color(FColor));
			putpixel(dest, BarX + 1, BarY - valmax - 1, xcom1_color(BColor));
			vline(dest,    BarX + 2, BarY,              BarY - valmax - 1, xcom1_color(BColor));
		}
		printsmall_x(dest, DigitsX, DigitsY, xcom1_color(DigitsColor), val);
	};
};

class IconText
{
public:
	int x, y;
	int color;
	FONT *font;
	
	const char *name;
	
	void Draw(BITMAP *dest, char *val)
	{
		textout(dest, font, val, x, y, xcom1_color(color));
	};
	
	void Draw(BITMAP *dest, int val, char *format)
	{
		textprintf(dest, font, x, y, xcom1_color(color), format, val);
	};
};

/**
 * Icons for reserve-time - buttons, with border to show active state
 */
class IconReserve
{
public:
	IconButton button;
	int BorderX1, BorderY1, BorderX2, BorderY2;
	int BorderColor;
	
	const char *name;
	
	void Draw(BITMAP *dest)
	{
		rect(dest, BorderX1, BorderY1, BorderX2, BorderY2, xcom1_color(BorderColor));
	}
};

/**
 * Control panel with buttons used by player to manage his squad in battlescape. 
 * Contains 'quit', 'next soldier', 'inventory', 'sit/stand' and other controls.
 *
 * @ingroup gui
 * @ingroup battlescape
 */
class Icon
{
private:
	int    x, y;
	int    width, height;

	SPK    *tac00;
	BITMAP *iconsbmp, *highlbmp, *clearbmp;
	
	std::string filename, highl_filename;
	int trans_level;
	
	IconItem item[ITEM_NUMBER];
	IconButton button[BUTTON_NUMBER];
	IconText text[TEXT_NUMBER];
	IconAttribute attribute[ATTRIBUTE_NUMBER];
	IconReserve reserve[RESERVE_NUMBER];
	
	int stun_color; 
public:
	Icon();
	~Icon();

	void draw();
	int inside(int mx, int my);
    int identify(int mx, int my);
	void execute(int mx, int my);
	void info();
	void drawbar(int col1, int col2, int x2, int y2, int val, int valmax);
	void show_eot();

	//void firemenu(Item *it);
	void firemenu(int iplace);
	int doprime(Item *it);

    //! Set position of control-panel on the screen: bottom, centered
	inline void setxy()
	{
		x = (SCREEN2W - width) / 2;
		y = SCREEN2H - height;
	}                        
	
	void draw_item(int itm, Item *it, int rounds, int prime, bool primed);
	void draw_text(int txt, char *val);
	void draw_text(int txt, int val, char *format);
	void draw_attribute(int attr, int val, int maxval);
	
	void draw_stun_bar(int x, int y, int val, int maxval);
};                      

#endif
