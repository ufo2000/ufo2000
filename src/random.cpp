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
// Uncomment the next line to get debug output for this class
// #define RANDOM_DEBUG
#include <memory.h>
#include "stdafx.h"
#include "global.h"
#include "random.h"
#ifdef RANDOM_DEBUG
#include "text.h"
#endif

IMPLEMENT_PERSISTENCE(Random, "Random");

/**
 * Reserve memory only, no initialization.
 */
Random::Random()
{
    m_iff = false;
}

/**
 * Initialize by calling init function.
 */
Random::Random(long init_num, long preiters)
{
    m_iff = false;
    init(init_num, preiters);
}

/**
 * An empty destructor.
 */
Random::~Random()
{
}

/**
 * Get a uniformly distributed integer in the range from 0 to MBIG - 1.
 */
long Random::get()
{
    next();
    return m_j;
}

/**
 * Get a uniformly distributed value in the range from 0 to 1.
 * Double should be precise enough even between different machines.
 */
double Random::getUniform()
{
    next();
    return (m_j * FAC);
}

/**
 * Get a uniformly distributed value in the range from a to b.
 * Double should be precise enough even between different machines.
 */
double Random::getUniform(double a, double b)
{
    ASSERT(a < b);
    next();
    double temp = m_j * FAC;
    temp = temp * (b - a) + a;
#ifdef RANDOM_DEBUG
    char tmp[128];
    sprintf(tmp, "double Random::getUniform(double a = %lf, double b = %lf) = %lf", a, b, temp);
    lua_message(tmp);
#endif
    return temp;
}

/**
 * Initialize random number generator with the init_num value.
 * Then make preiters iterations (0 if omitted).
 */
void Random::init (long init_num, long preiters)
{
#ifdef RANDOM_DEBUG
    char tmp[128];
    sprintf(tmp, "void Random::init(long init_num = %ld, long preiters = %ld)", init_num, preiters);
    lua_message(tmp);
#endif
    ASSERT(init_num >= 0);
    ASSERT(preiters >= 0);
    int i, j, k;
    m_j = MSEED - init_num;
    if (m_j < 0) m_j = -m_j;
    m_j %= MBIG;
    m_a[55] = m_j;
    m_k = 1;
    for (i = 1, j = 0; i <= 54; i++) {
        j += 21;
        if (j > 55) j -= 55;
        m_a[j] = m_k;
        m_k = m_j - m_k;
        if (m_k < 0) m_k += MBIG;
        m_j = m_a[j];
    }
    for (k = 1; k <= 4; k++)
        for (i = 1, j = 31; i <= 55; i++, j >= 55 ? j = 1 : j++) {
            m_a[i] -= m_a[j];
            if (m_a[i] < 0) m_a[i] += MBIG;
        }
    m_inext = 0;
    m_inextp = 31;
    m_iff = true;
    for (m_iters = 0; preiters; preiters--) next();
}

bool Random::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);
    return true;
}

bool Random::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);
    return true;
}
