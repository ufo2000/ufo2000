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

#include <stdio.h>
#include <nl.h>
#ifdef HAVE_DUMBOGG
#include <aldumb.h>
extern "C" {
#include "dumbogg/dumbogg.h"
}
#endif
#include "global.h"
#include "music.h"
#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t + 999) / 1000)
#else
#include <unistd.h>
#endif

static MIDI *allegro_midi = NULL;

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
		if (dp) al_poll_duh(dp);
		nlMutexUnlock(&mutex);
		usleep(10000);
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
	duh = dumb_load_xm(filename);
	if (!duh) duh = dumb_load_s3m(filename);
	if (!duh) duh = dumb_load_mod(filename);
	if (!duh) duh = dumb_load_it(filename);
	if (!duh) duh = dumb_load_ogg(filename, 1);
	if (duh) dp = al_start_duh(duh, 2, 0, 1.0f, 4096, 44100);
	nlMutexUnlock(&mutex);
#endif
	return true;
}
