/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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


/* standard */
#include <string.h>

/* application */
#include "gametime.h"


#define  UPDATES_EACH_SECOND  5


GameTimeRate  g_game_time_rate[7] = {

    { "Paused",  0 },
    { "5 Secs",  5 },
    { "1 Min",   60 },
    { "5 Mins",  60*5 },
    { "30 Mins", 60*30 },
    { "1 Hour",  60*60 },
    { "1 Day",   60*60*24 },
};


long usecs_between( struct timeval* instant1, struct timeval* instant2 )
{
    return (instant2->tv_usec - instant1->tv_usec)
        + 1000000L * (instant2->tv_sec - instant1->tv_sec);
}


GameTime::GameTime() {

    m_game_time = 994239723;
    m_rate = PAUSED;
    gettimeofday( &m_when_last_updated, NULL );
}


void GameTime::tick() {

    struct timeval  now;
    long            time_since_updated;

    gettimeofday( &now, NULL );

    time_since_updated = usecs_between( &m_when_last_updated, &now );

    if ( (1000000 / UPDATES_EACH_SECOND) < time_since_updated )
    {
        m_game_time +=
            g_game_time_rate[m_rate].seconds * time_since_updated / 1000000;

        memcpy( &m_when_last_updated, &now, sizeof(struct timeval) );
    }
}


struct tm* GameTime::human_readable() {

    return gmtime( &m_game_time );
}


GameTimeRateIndex GameTime::get_rate() {

    return m_rate;
}


void GameTime::set_rate( GameTimeRateIndex new_rate ) {

    m_rate = new_rate;
}
