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

#ifdef HAVE_FMOD
#include <fmod.h>
#endif
#include <stdio.h>
#include "global.h"
#include "music.h"

bool FS_MusicInit()
{
#ifdef HAVE_FMOD
	if (FSOUND_Init(44100, 32, 0)) {
		FS_MusicSetVolume(255);
		return true;
	}
	return false;
#else
	return true;
#endif
}

void FS_MusicClose()
{
#ifdef HAVE_FMOD
	FSOUND_Close();
#endif
}

void FS_MusicSetVolume(int volume)
{
#ifdef HAVE_FMOD
	FSOUND_SetVolume(FSOUND_ALL, volume);
#endif
}

#ifdef HAVE_FMOD
static FMUSIC_MODULE *music_module = NULL;
static FSOUND_STREAM *sound_stream = NULL;
#else
static MIDI *allegro_midi = NULL;
#endif

/**
 * Start playing soundtrack in a loop
 */
bool FS_MusicPlay(const char *filename)
{
#ifdef HAVE_FMOD
	if (music_module) {
		FMUSIC_FreeSong(music_module);
		music_module = NULL;
	}

	if (sound_stream) {
		FSOUND_Stream_Close(sound_stream);
		sound_stream = NULL;
	}

	if (filename == NULL) return true;

	music_module = FMUSIC_LoadSong(filename);
	if (music_module) {
		FMUSIC_PlaySong(music_module);
	} else {
		sound_stream = FSOUND_Stream_Open(filename, FSOUND_LOOP_NORMAL, 0, 0);
		FSOUND_Stream_Play(FSOUND_FREE, sound_stream);
	}
#else
	play_midi(NULL, 0);

	if (allegro_midi) {
		destroy_midi(allegro_midi);
		allegro_midi = NULL;
	}

	if (filename == NULL) return true;

	allegro_midi = load_midi(filename);
	play_midi(allegro_midi, 1);
#endif
	return true;
}
