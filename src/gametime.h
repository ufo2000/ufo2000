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
#ifndef __GAMETIME_H
#define __GAMETIME_H


#include <sys/time.h>
#include <time.h>


/**
 * A rate at which game time may be accelerated.
 */
typedef struct {

    char*  description;
    int    seconds;

} GameTimeRate;


/**
 * The seven rates of game time relative to real time are indexed by the type
 * GameTimeRateIndex
 */
extern  GameTimeRate  g_game_time_rate[7];


typedef enum {

    PAUSED,
    FIVE_SECONDS,
    ONE_MINUTE,
    FIVE_MINUTES,
    THIRTY_MINUTES,
    ONE_HOUR,
    ONE_DAY,

} GameTimeRateIndex;


/**
 * Stores the instant in time that is current within a game (as opposed to the
 * current time in the real world).
 */
class GameTime {

private:
    time_t             m_game_time;
    GameTimeRateIndex  m_rate;
    struct timeval     m_when_last_updated;

public:
    /** Constructs a new GameTime object */
    GameTime();

    /**
     * Invoked to indicate that some time has passed and that the game time
     * should advance before it is rendered.
     */
    void tick();

    /**
     * Returns the current game time in human readable form (month, hours,
     * seconds, etc.)
     */
    struct tm* human_readable();

    GameTimeRateIndex get_rate();

    void set_rate( GameTimeRateIndex new_rate );

};


#endif
