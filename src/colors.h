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
 * Color-constants
 */

#ifndef COLORS_H
#define COLORS_H

#include "global.h"

//! The color-palette of XCOM is organized in 'stripes' of 16 colors.
//! In each stripe, the brightness decreases from bright to dark.

// 0 : Special case:
#define COLOR_TRANSPARENT     xcom1_color(  0)

// 1-15 : GRAY: White, 8% Gray .. 97% Gray, Black:
#define COLOR_GRAY01          xcom1_color(  1)
#define COLOR_GRAY02          xcom1_color(  2)
#define COLOR_GRAY03          xcom1_color(  3)
#define COLOR_GRAY04          xcom1_color(  4)
#define COLOR_GRAY05          xcom1_color(  5)
#define COLOR_GRAY06          xcom1_color(  6)
#define COLOR_GRAY07          xcom1_color(  7)
#define COLOR_GRAY08          xcom1_color(  8)
#define COLOR_GRAY09          xcom1_color(  9)
#define COLOR_GRAY10          xcom1_color( 10)
#define COLOR_GRAY11          xcom1_color( 11)
#define COLOR_GRAY12          xcom1_color( 12)
#define COLOR_GRAY13          xcom1_color( 13)
#define COLOR_GRAY14          xcom1_color( 14)
#define COLOR_GRAY15          xcom1_color( 15)

// 16-31 : Gold, Orange, DarkOrange .. OrangeRed, SaddleBrown, Maroon :
#define COLOR_ORANGE00        xcom1_color( 16)
#define COLOR_ORANGE01        xcom1_color( 17)
#define COLOR_ORANGE02        xcom1_color( 18)
#define COLOR_ORANGE03        xcom1_color( 19)
#define COLOR_ORANGE04        xcom1_color( 20)
#define COLOR_ORANGE05        xcom1_color( 21)
#define COLOR_ORANGE10        xcom1_color( 26)
#define COLOR_ORANGE15        xcom1_color( 31)

// 32-47 : RED: LightCoral, IndianRed .. FireBrick .. Maroon
#define COLOR_RED00           xcom1_color( 32)
#define COLOR_RED01           xcom1_color( 33)
#define COLOR_RED02           xcom1_color( 34)
#define COLOR_RED03           xcom1_color( 35)
#define COLOR_RED04           xcom1_color( 36)
#define COLOR_RED05           xcom1_color( 37)
#define COLOR_RED06           xcom1_color( 38)
#define COLOR_RED07           xcom1_color( 39)
#define COLOR_RED08           xcom1_color( 40)
#define COLOR_RED09           xcom1_color( 41)
#define COLOR_RED10           xcom1_color( 42)
#define COLOR_RED11           xcom1_color( 43)
#define COLOR_RED12           xcom1_color( 44)
#define COLOR_RED13           xcom1_color( 45)
#define COLOR_RED14           xcom1_color( 46)
#define COLOR_RED15           xcom1_color( 47)

// 48-63 : GREEN: LightGreen .. MediumSeaGreen .. ForestGreen .. SeaGreen
#define COLOR_GREEN00         xcom1_color( 48)
#define COLOR_GREEN01         xcom1_color( 49)
#define COLOR_GREEN02         xcom1_color( 50)
#define COLOR_GREEN03         xcom1_color( 51)
#define COLOR_GREEN04         xcom1_color( 52)
#define COLOR_GREEN05         xcom1_color( 53)
#define COLOR_GREEN06         xcom1_color( 54)
#define COLOR_GREEN07         xcom1_color( 55)
#define COLOR_GREEN08         xcom1_color( 56)
#define COLOR_GREEN09         xcom1_color( 57)
#define COLOR_GREEN10         xcom1_color( 58)
#define COLOR_GREEN11         xcom1_color( 59)
#define COLOR_GREEN12         xcom1_color( 60)
#define COLOR_GREEN13         xcom1_color( 61)
#define COLOR_GREEN14         xcom1_color( 62)
#define COLOR_GREEN15         xcom1_color( 63)

// 64-79 : GREEN-OLIVE: Khaki, OliveDrab .. ForestGreen .. DarkGreen
#define COLOR_OLIVE00         xcom1_color( 64)
#define COLOR_OLIVE01         xcom1_color( 65)
#define COLOR_OLIVE02         xcom1_color( 66)
#define COLOR_OLIVE03         xcom1_color( 67)
#define COLOR_OLIVE04         xcom1_color( 68)
#define COLOR_OLIVE05         xcom1_color( 69)
#define COLOR_OLIVE06         xcom1_color( 70)
#define COLOR_OLIVE07         xcom1_color( 71)
#define COLOR_OLIVE10         xcom1_color( 74)
#define COLOR_OLIVE11         xcom1_color( 75)
#define COLOR_OLIVE15         xcom1_color( 79)

// 80-95 : RED_GRAY: 1% Gray, 10% Gray .. 87% Gray, 90% Gray
#define COLOR_RED_GRAY00      xcom1_color( 80)
#define COLOR_RED_GRAY07      xcom1_color( 87)
#define COLOR_RED_GRAY10      xcom1_color( 90)

// 96-111 : RED_BROWN: Goldenrod, Peru .. Chocolate .. SaddleBrown .. Maroon
#define COLOR_RED_BROWN00     xcom1_color( 96)
#define COLOR_RED_BROWN01     xcom1_color( 97)
#define COLOR_RED_BROWN02     xcom1_color( 98)
#define COLOR_RED_BROWN03     xcom1_color( 99)
#define COLOR_RED_BROWN04     xcom1_color(100)
#define COLOR_RED_BROWN05     xcom1_color(101)
#define COLOR_RED_BROWN10     xcom1_color(106)

// 112-127 : STEELBLUE: LightSteelBlue, LightSlateGray .. SlateGray,
//                      SteelBlue, DarkSlateBlue .. MidnightBlue
#define COLOR_STEELBLUE00     xcom1_color(112)
#define COLOR_STEELBLUE08     xcom1_color(120)
#define COLOR_STEELBLUE10     xcom1_color(122)

// 128-143 : BLUE: LightSteelBlue, LightSkyBlue, SteelBlue,
//                 DarkSlateBlue, MidnightBlue
#define COLOR_BLUE00          xcom1_color(128)
#define COLOR_BLUE01          xcom1_color(129)
#define COLOR_BLUE02          xcom1_color(130)
#define COLOR_BLUE03          xcom1_color(131)
#define COLOR_BLUE04          xcom1_color(132)
#define COLOR_BLUE10          xcom1_color(138)
#define COLOR_BLUE15          xcom1_color(143)

// 144-159 : YELLOW: Khaki .. DarkKhaki .. Peru .. SaddleBrown, Maroon
#define COLOR_YELLOW00        xcom1_color(144)
#define COLOR_YELLOW01        xcom1_color(145)
#define COLOR_YELLOW02        xcom1_color(146)
#define COLOR_YELLOW03        xcom1_color(147)
#define COLOR_YELLOW04        xcom1_color(148)
#define COLOR_YELLOW05        xcom1_color(149)
#define COLOR_YELLOW06        xcom1_color(150)
#define COLOR_YELLOW10        xcom1_color(154)

// 160-175 : BROWN: DarkKhaki, Peru, Sienna, SaddleBrown, Brown, 100% Gray
#define COLOR_BROWN00         xcom1_color(160)
#define COLOR_BROWN04         xcom1_color(164)
#define COLOR_BROWN10         xcom1_color(170)

// 176-191 : ROSE: MistyRose .. LightPink, RosyBrown .. PaleVioletRed ..
//                 DarkSlateBlue .. MidnightBlue
#define COLOR_ROSE00          xcom1_color(176)
#define COLOR_ROSE01          xcom1_color(177)
#define COLOR_ROSE02          xcom1_color(178)
#define COLOR_ROSE03          xcom1_color(179)
#define COLOR_ROSE04          xcom1_color(180)
#define COLOR_ROSE06          xcom1_color(182)
#define COLOR_ROSE10          xcom1_color(186)

// 192-207 : VIOLETT: Thistle, Plum .. MediumPurple .. MediumOrchid ..
//                    DarkOrchid, MediumVioletRed, MidnightBlue
#define COLOR_VIOLET00        xcom1_color(192)
#define COLOR_VIOLET05        xcom1_color(197)
#define COLOR_VIOLET07        xcom1_color(199)
#define COLOR_VIOLET10        xcom1_color(202)

// 208-223 : SKYBLUE: LightSkyBlue, SteelBlue, DodgerBlue .. RoyalBlue,
//                    DarkSlateBlue, MidnightBlue .. Navy .. Indigo
#define COLOR_SKYBLUE00       xcom1_color(208)
#define COLOR_SKYBLUE03       xcom1_color(211)

// 224-239 : BLUEGRAY: Lavender .. LightSteelBlue .. LightSlateGray,
//                     SlateGray, DarkSlateBlue, DarkSlateBlue .. 84% Gray
#define COLOR_GRAYBLUE00      xcom1_color(224)
#define COLOR_GRAYBLUE04      xcom1_color(228)
#define COLOR_GRAYBLUE10      xcom1_color(234)

// 240-255 : DARKGRAY: 38% Gray, 44% Gray, 48% Gray, 52% Gray .. 
//                     96% Gray, Black (again)
#define COLOR_DARKGRAY00      xcom1_color(240)
#define COLOR_DARKGRAY01      xcom1_color(241)
#define COLOR_DARKGRAY02      xcom1_color(242)
#define COLOR_DARKGRAY03      xcom1_color(243)
#define COLOR_DARKGRAY04      xcom1_color(244)
#define COLOR_DARKGRAY05      xcom1_color(245)
#define COLOR_DARKGRAY06      xcom1_color(246)
#define COLOR_DARKGRAY07      xcom1_color(247)
#define COLOR_DARKGRAY08      xcom1_color(248)
#define COLOR_DARKGRAY09      xcom1_color(249)
#define COLOR_DARKGRAY10      xcom1_color(250)
#define COLOR_DARKGRAY11      xcom1_color(251)
#define COLOR_DARKGRAY12      xcom1_color(252)
#define COLOR_DARKGRAY13      xcom1_color(253)
#define COLOR_DARKGRAY14      xcom1_color(254)
#define COLOR_DARKGRAY15      xcom1_color(255)


// Shortcuts & best looking & most often used colors:

#define COLOR_WHITE        COLOR_GRAY01
#define COLOR_LT_GRAY      COLOR_GRAY02
#define COLOR_GRAY         COLOR_GRAY03
#define COLOR_DK_GRAY      COLOR_GRAY10

#define COLOR_BLACK1       COLOR_GRAY15
#define COLOR_BLACK2       COLOR_DARKGRAY15
#define COLOR_WHITE1       COLOR_RED_GRAY00

#define COLOR_LT_RED       COLOR_RED00
#define COLOR_RED          COLOR_RED03
#define COLOR_DK_RED       COLOR_RED08

#define COLOR_ROSE         COLOR_ROSE04

#define COLOR_GOLD         COLOR_ORANGE00
#define COLOR_ORANGE       COLOR_ORANGE02
#define COLOR_YELLOW       COLOR_YELLOW00
#define COLOR_BROWN        COLOR_YELLOW06

#define COLOR_GREEN        COLOR_GREEN02
#define COLOR_DK_GREEN     COLOR_GREEN10

#define COLOR_LT_OLIVE     COLOR_OLIVE02
#define COLOR_OLIVE        COLOR_OLIVE05

#define COLOR_LT_BLUE      COLOR_BLUE02
#define COLOR_BLUE         COLOR_BLUE04
#define COLOR_DK_BLUE      COLOR_STEELBLUE08

#define COLOR_MAGENTA      COLOR_TRANSPARENT

// Colors for system-messages:
#define COLOR_SYS_HEADER   COLOR_ORANGE
#define COLOR_SYS_ERROR    COLOR_RED
#define COLOR_SYS_WARNING  COLOR_MAGENTA
#define COLOR_SYS_INFO     COLOR_DARKGRAY01
#define COLOR_SYS_INFO1    COLOR_WHITE
#define COLOR_SYS_INFO2    COLOR_GRAY04
#define COLOR_SYS_OK       COLOR_GREEN04
#define COLOR_SYS_FAIL     COLOR_RED02
#define COLOR_SYS_PROMPT   COLOR_MAGENTA
#define COLOR_SYS_DEBUG    COLOR_BLUE

// Examples of Usage:
//  	print_win->printstr(curline.c_str(), COLOR_WHITE);
//  	textout(screen, font, curline.c_str(), 0, print_y, COLOR_WHITE);
//	print_win = new Wind(text_back, 15, 300, 625, 390, COLOR_BLACK1);
//	textprintf(screen2, font, 0, 0, COLOR_WHITE, "%s", "WATCH");
//	textprintf(newscr, g_small_font,   8, 60, COLOR_WHITE, "Player 1");
//	g_console->printf(COLOR_BLUE, "%s killed.", md.Name);
//	g_console->printf(COLOR_SYS_PROMPT, "%s\n\n", "Press ESC to continue");

// See also:
// * rgb.txt from X11
// * main.cpp : color_chart1(), color_chart2()

// Some free color-picker utilities:
// * Pixie     - www.nattyware.com
// * Color Cop - www.datastic.com/tools/colorcop
// * WhatColor - www.hikarun.com/e

#endif
