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
#ifndef KEYS_H
#define KEYS_H

void process_keyswitch();
void keyswitch(int keyswitch_lang);
int keymaper(int c);

#define k_left       75
#define k_right      77
#define k_up         72
#define k_down       80
#define k_pgdn       81
#define k_pgup       73
#define k_ins        82
#define k_esc        27
#define k_space      32
#define k_enter      13
#define k_bksp        8
#define k_tab         9
#define k_end      0x4F
#define k_kp_minus 0x8E
#define k_kp_plus  0x90


#define k_alt_enter 0x1c
#define k_alt_x     0x2d
#define k_alt_up    0x98
#define k_alt_down  0xa0
#define k_alt_left  0x9b
#define k_alt_right 0x9d

#define k_f1    0x3b
#define k_f2    0x3c
#define k_f3    0x3d
#define k_f4    0x3e
#define k_f5    0x3f
#define k_f6    0x40
#define k_f7    0x41
#define k_f8    0x42
#define k_f9    0x43
#define k_f10   0x44

#define k_0    0x30
#define k_1    0x31
#define k_2    0x32
#define k_3    0x33
#define k_4    0x34
#define k_5    0x35
#define k_6    0x36
#define k_7    0x37
#define k_8    0x38
#define k_9    0x39

#define s_left   0x4b
#define s_right  0x4d
#define sr_left  0xcb
#define sr_right 0xcd
#define s_up     0x48
#define s_down   0x50
#define sr_up    0xc8
#define sr_down  0xd0
#define s_esc    1
#define s_space  0x39
#define sr_space 0xb9
#define s_enter  0x1C
#define sr_enter (s_enter|0x80)

#define s_a      0x1E
#define sr_a     (s_a|0x80)
#define s_s      0x1F
#define sr_s     (s_s|0x80)
#define s_d      0x20
#define sr_d     (s_d|0x80)

#endif
