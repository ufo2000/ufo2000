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
#include "global.h"
#include <assert.h>
#include <stdio.h>
#include <allegro.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sound.h"
#include "wind.h"
#include "config.h"

#include "pfxopen.h"

int SOUND = 1;
int MODSOUND = 1;
int digvoices = 8;
int modvoices = 8;
char modname[128];
int digvol = 255;
int modvol = 125;

std::vector<SAMPLE *>  g_samples;
MIDI                  *g_midi_music;

void playall();

/**
 * Function that loads .CAT file
 *  0  1  2  3  4  5  6  7
 * B8 01 00 00 06 3C 00 00
 * |  ofs	 | |  len	 |
 *
 * @param samples  vector that stores all the loaded sound samples
 * @param filename path to .CAT file to be loaded
 */
void load_cat_file(std::vector<SAMPLE *> & samples, const char *filename)
{
	int fh = OPEN_ORIG(filename, O_RDONLY | O_BINARY);
	if (fh == -1) return;

	int raw_size = filelength(fh);
	unsigned char *raw = new unsigned char[raw_size];
	raw_size = read(fh, raw, raw_size);
	close(fh);

	int sample_num = ((long*)raw)[0] / 8;

	for (int i = 0; i < sample_num; i++) {
		int ofs = ((long*)raw + i * 2)[0];
		int len = ((long*)raw + i * 2 + 1)[0];

		SAMPLE *sample = create_sample(8, 0, 11025, len);
		assert(sample != NULL);
		for (int j = 0; j < len; j++)
			((unsigned char *)sample->data)[j] = raw[ofs++];
		samples.push_back(sample);
	}

	delete [] raw;
}

void initsound()
{
	if (!SOUND) {
		MODSOUND = 0;
		return ;
	}

	if (SOUND) {
		reserve_voices(digvoices, -1);
	}

	if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
		printf("Error initialising sound system\n%s\n", allegro_error);
		SOUND = 0;
		MODSOUND = 0;
		return ;
	}

	load_cat_file(g_samples, "sound/sample.cat");
	load_cat_file(g_samples, "sound/sample2.cat");
	load_cat_file(g_samples, "sound/sample3.cat");

	g_midi_music = load_midi(cfg_get_music_file_name());
}

void closesound()
{
	for (int i = 0; i < g_samples.size(); i++)
		destroy_sample(g_samples[i]);
	
	if (SOUND) {
		remove_sound();
	}
}

void play(int snum, int vol)
{
	if (!SOUND || snum < 0 || snum >= g_samples.size()) return;
	play_sample(g_samples[snum], vol, 128, 1000, 0);
}

void play(int snum, int vol, int pan, int freq, int loop)
{
	if (!SOUND || snum < 0 || snum >= g_samples.size()) return;
	play_sample(g_samples[snum], vol, pan, freq, loop);
}

void playall()
{
	if (!SOUND) return;
	
	for (int i = 0; i < g_samples.size(); i++) {
			do {
				play_sample(g_samples[i], 255, 128, 1000, 0);
				g_console->printf("sample %d", i);
				g_console->redraw(screen, 0, SCREEN2H);
			} while ((readkey() >> 8) == KEY_SPACE);
			stop_sample(g_samples[i]);
			if (key[KEY_ESC]) return ;
	}
}

void setmodvol(int v)
{
}

int getmodvol()
{
	if (!MODSOUND)
		return 0;
	return 0;
}

void setdigvol(int v)
{
	set_volume(v, -1);
	digvol = v;
}

int getdigvol()
{
	if (!SOUND)
		return 0;
	return digvol;
}

int modplay(char *fname)
{
	return 0;
}
