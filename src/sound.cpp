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
#include <assert.h>
#include <stdio.h>
#include <allegro.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef USE_JGMOD
#include <jgmod.h>
#endif
#include "sound.h"

#include "pfxopen.h"

#ifdef USE_JGMOD
static JGMOD *mod;
#endif

static SAMPLE **sample;
static int sample_num;
static unsigned char *raw;
static int raw_size;
int SOUND = 1;
int MODSOUND = 1;
int digvoices = 16;
int modvoices = 8;
char modname[128];
int digvol = 255;
int modvol = 125;

void playall();

void initsound()
{
	if (!SOUND) {
		MODSOUND = 0;
		return ;
	}

	if (SOUND) {
		reserve_voices(digvoices, -1);
	}
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
		//if (install_sound(DIGI_SB, MIDI_NONE, NULL) != 0) {
		printf("Error initialising sound system\n%s\n", allegro_error);
		//readkey();
		SOUND = 0;
		MODSOUND = 0;
		return ;
	}
#ifdef USE_JGMOD
	if (MODSOUND) {
		//printf("Digital sound driver: %s\n", digi_driver->name);
		if (install_mod(modvoices) < 0) { // call install_mod only after install_sound
			printf("Error setting digi voices");
			MODSOUND = 0;
			readkey();
		} else {
			mod = load_mod(modname);
			if (mod == NULL) {
				printf("Error reading module %s", modname);
				MODSOUND = 0;
				//*modname=0;
				//set_mod_volume(0);
				readkey();
			} else {
				set_mod_volume(modvol);
				play_mod(mod, TRUE);
				//printf("Module %s %d", modname, modvol);
			}
		}
	}
#endif
	set_volume(digvol, -1);

	// sound3 = sound1+sound1 !!!!!! //outdated joke
	int fh = OPEN_ORIG("sound/sound1.cat", O_RDONLY | O_BINARY);
	assert(fh != -1);
	raw_size = filelength(fh);
	raw = new unsigned char[raw_size];
	raw_size = read(fh, raw, raw_size);
	close(fh);

	sample_num = ((long*)raw)[0] / 8;
	//printf("sample_num=%d", sample_num);

	//  0  1  2  3  4  5  6  7
	// B8 01 00 00 06 3C 00 00
	// |  ofs	 | |  len	 |
#define SHITOFS 6
	sample = new SAMPLE * [sample_num];
	for (int i = 0; i < sample_num; i++) {
		int ofs = ((long*)raw + i * 2)[0];
		int len = ((long*)raw + i * 2 + 1)[0];
		//printf("\nofs=%x len=%x", ofs, len);
		//readkey();
		if (len > SHITOFS) {
			ofs += SHITOFS;
			len -= SHITOFS;
		}

		sample[i] = create_sample(8, 0, 11025, len);      //NULL if len=0
		if (sample[i] != NULL)
			for (int j = 0; j < len; j++)
				((unsigned char *)sample[i]->data)[j] = raw[ofs++] + 0x60;
	}
	//#define SHITOFS 6
	//	(char *)sample[20]->data = (char *)sample[20]->data + SHITOFS;
	//	sample[20]->len -= SHITOFS;


	//playall();
}

void closesound()
{
	for (int i = 0; i < sample_num; i++)
		if (sample[i] != NULL) 
			destroy_sample(sample[i]);
	delete [] sample;
	
	delete [] raw;

	if (SOUND) {
		//destroy_sample(the_sample);
		remove_sound();
	}
}

void play(int SNUM)
{
	if (SOUND) {
		if (SNUM < sample_num)
			if (sample[SNUM] != NULL)
				play_sample(sample[SNUM], 255, 128, 1000, 0);
	}
}

void play(int SNUM, int vol, int pan, int freq, int loop)
{
	if (SOUND) {
		if (SNUM < sample_num)
			if (sample[SNUM] != NULL)
				play_sample(sample[SNUM], vol, pan, freq, loop);
	}
}

void playall()
{
	if (SOUND) {
		for (int i = 0; i < sample_num; i++) {
			if (sample[i] != NULL) {
				do {
					play_sample(sample[i], 255, 128, 1000, 0);
					printf("\nsample %d", i);
					fflush(NULL);
				} while ((readkey() >> 8) == KEY_SPACE);
				stop_sample(sample[i]);
				if (key[KEY_ESC]) return ;
			}
		}
	}
}

void setmodvol(int v)
{
#ifdef USE_JGMOD
	set_mod_volume(v);
#endif
}

int getmodvol()
{
	if (!MODSOUND)
		return 0;
#ifdef USE_JGMOD
	return get_mod_volume();
#else
	return 0;
#endif
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
#ifdef USE_JGMOD
	if (is_mod_playing())
		stop_mod();
	if (mod != NULL) destroy_mod(mod);

	mod = load_mod(fname);
	if (mod == NULL) {
		alert("Error reading module:", fname, NULL, "shIT", NULL, 0, 0);
		//MODSOUND=0;
		return 0;
	}
	strcpy(modname, fname);
	play_mod(mod, TRUE);
	//printf("Module %s %d", modname, modvolume);
	return 1;
#else
	return 0;
#endif
}
