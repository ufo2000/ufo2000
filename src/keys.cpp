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
#include "global.h"
#include <string.h>
#include "keys.h"

int USERUSKEY = 1;
int KEYMODE = 0;      //0 eng,  1 rus
int crel = 1;

void keyswitch()
{
	if (crel) {
		KEYMODE = 1 - KEYMODE;
		crel = 0;
		/*if (KEYMODE)
			info->writestr(76, -1, "rus");
		else
			info->writestr(76, -1, "eng");*/
	}
}

static char en[] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', '{', ']', '}', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', '\'', '\"', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '<', '.', '>', 0};
static char ru[] = {'©', 'Ê', '„', '™', '•', '≠', '£', 'Ë', 'È', 'ß', 'Â', 'ï', 'Í', 'ö', '‰', 'Î', '¢', '†', 'Ø', '‡', 'Æ', '´', '§', '¶', 'Ü', 'Ì', 'ù', 'Ô', 'Á', '·', '¨', '®', '‚', 'Ï', '°', 'Å', 'Ó', 'û', 0};

/*
 !"#$%&'()*+,-./0123456789:;<=>?
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
`abcdefghijklmnopqrstuvwxyz{|}~
ÄÅÇÉÑÖÜáàâäãåçéèêëíìîïñóòôöõúùûü
†°¢£§•¶ß®©™´¨≠ÆØ∞±≤≥¥µ∂∑∏π∫ªºΩæø
¿¡¬√ƒ≈∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ
‡·‚„‰ÂÊÁËÈÍÎÏÌÓÔÒ
*/

char keymaper(char c)
{
	char *ptr;
	if (KEYMODE) {
		ptr = strchr(en, c);
		if (ptr != NULL)
			return ru[ptr - en];
		else {
			if ((c >= 'A') && (c <= 'Z')) {
				c += 32;
				ptr = strchr(en, c);
				if (ptr != NULL) {
					char r = ru[ptr - en];
					if ((r >= '†') && (r <= 'Ø'))
						r -= 32;
					else
						r -= 80;
					return r;
				}
			}
		}
	}
	return c;
}
