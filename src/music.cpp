/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

#ifdef HAVE_DUMBOGG
#include <aldumb.h>
extern "C" {
#include "dumbogg/dumbogg.h"
}
#endif
#include "global.h"
#include "music.h"

static MIDI *allegro_midi = NULL;
static int music_volume = 255;

#ifdef HAVE_DUMBOGG
static DUH *duh = NULL;
static AL_DUH_PLAYER *dp = NULL;
static NLthreadID tid = 0;
static int thread_exit_flag = 0;
static NLmutex mutex;

void *ThreadFunc(void *data)
{
	while (!thread_exit_flag) {
		nlMutexLock(&mutex);
		if (dp) {
			if (al_poll_duh(dp)) {
				al_stop_duh(dp);
				dp = al_start_duh(duh, 2, 0, 1.0f, 4096, 44100);
				DUH_SIGRENDERER *sr = al_duh_get_sigrenderer(dp);
				DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(sr);
				dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
				dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
			}
		}
		nlMutexUnlock(&mutex);
		rest(10);
	}
	nlMutexDestroy(&mutex);
	return NULL;
}
#endif

bool FS_MusicInit()
{
#ifdef HAVE_DUMBOGG
	dumb_register_stdfiles();
	nlMutexInit(&mutex);
	tid = nlThreadCreate(ThreadFunc, NULL, NL_FALSE);
#endif
	return true;
}

void FS_MusicClose()
{
#ifdef HAVE_DUMBOGG
	nlMutexLock(&mutex);
	if (dp) {
		al_stop_duh(dp);
		dp = NULL;
	}

	if (duh) {
		unload_duh(duh);
		duh = NULL;
	}
	nlMutexUnlock(&mutex);
	thread_exit_flag = 1;
	dumb_exit();
#endif
}

bool FS_MusicPlay(const char *filename)
{
#ifdef HAVE_DUMBOGG
	nlMutexLock(&mutex);
	if (dp) {
		al_stop_duh(dp);
		dp = NULL;
	}
	if (duh) {
		unload_duh(duh);
		duh = NULL;
	}
	nlMutexUnlock(&mutex);
#endif

	play_midi(NULL, 0);
	if (allegro_midi) {
		destroy_midi(allegro_midi);
		allegro_midi = NULL;
	}

	if (filename == NULL)
		return true;

	allegro_midi = load_midi(filename);
	if (allegro_midi) {
		play_midi(allegro_midi, 1);
		return true;
	}

#ifdef HAVE_DUMBOGG
	nlMutexLock(&mutex);
	if (dp) {
		al_stop_duh(dp);
		dp = NULL;
	}
	if (duh) {
		unload_duh(duh);
		duh = NULL;
	}
	duh = dumb_load_ogg(filename, 1);
	if (!duh) duh = dumb_load_xm(filename);
	if (!duh) duh = dumb_load_s3m(filename);
	if (!duh) duh = dumb_load_mod(filename);
	if (!duh) duh = dumb_load_it(filename);
	if (duh) dp = al_start_duh(duh, 2, 0, (float)music_volume / 255.0, 4096, 44100);

	DUH_SIGRENDERER *sr = al_duh_get_sigrenderer(dp);
	DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(sr);
	dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
	dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);

	nlMutexUnlock(&mutex);
#endif
	return true;
}

int FS_GetMusicVolume()
{
	return music_volume;
}

int FS_IncMusicVolume()
{
	FS_SetMusicVolume(music_volume + 16);
	return music_volume;
}

int FS_DecMusicVolume()
{
	FS_SetMusicVolume(music_volume - 16);
	return music_volume;
}

void FS_SetMusicVolume(int volume)
{
	if (volume < 0) volume = 0;
	if (volume > 255) volume = 255;
#ifdef HAVE_DUMBOGG
	nlMutexLock(&mutex);
	music_volume = volume;
	if (dp) al_duh_set_volume(dp, (float)music_volume / 255.0);
	nlMutexUnlock(&mutex);
#endif
	set_volume(-1, volume);
}
