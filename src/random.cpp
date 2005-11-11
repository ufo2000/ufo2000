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
/*
   Make RANDOM_DL an integer up to 3 to set the debug level
   and get debug output for this class.
   RANDOM_DL = 0 will produce no debug output.
*/
#define RANDOM_DL 0
#include "random.h"
#if RANDOM_DL > 0
#include "text.h"
#endif

#define FAC (REAL)(1.0 / MBIG)

IMPLEMENT_PERSISTENCE(Random, "Random");

/**
 * Reserve memory only, no initialization.
 */
Random::Random()
{
    m_iff = false;
    m_normalready = false;
}

/**
 * Initialize by calling init function.
 */
Random::Random(long init_num, long preiters)
{
    m_iff = false;
    m_normalready = false;
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
 */
REAL Random::getUniform()
{
    next();
    return (m_j * FAC);
}

/**
 * Get a uniformly distributed value in the range from a to b.
 */
REAL Random::getUniform(REAL a, REAL b)
{
    ASSERT(a < b);
    next();
    REAL result = m_j * FAC;
    result = result * (b - a) + a;
#if RANDOM_DL >= 3
    char debugstr[STDBUFSIZE];
    sprintf(debugstr, "REAL Random::getUniform(REAL a = %lf, REAL b = %lf) = %lf",
        (double) a, (double) b, (double) result);
    lua_message(debugstr);
#endif
    return result;
}

/**
 * Get a standard normally distributed value, that is, N(0, 1).
 * Based on "Numerical Recipes In C" book, chapter 7.2
 * (available online at www.nr.com).
 * In fact, generates pairs of independent random values.
 */
REAL Random::getNormal()
{
    REAL result;
    if (m_normalready) {m_normalready = false; result = m_normalsaved;}
    else {
        const REAL LOW = 1E-10;
        REAL v1, v2, rsq, fact;
        do { // (v1, v2) should be inside the unit circle.
            v1 = getUniform(-1.0, 1.0);
            v2 = getUniform(-1.0, 1.0);
            rsq = v1 * v1 + v2 * v2;
        } while (rsq >= 1.0 || rsq < LOW); // Don't make it too low to avoid precision errors.
        fact = sqrt (-2.0 * log (rsq) / rsq);
        m_normalsaved = v1 * fact;
        m_normalready = true;
        result = v2 * fact;
    }
#if RANDOM_DL >= 2
    char debugstr[STDBUFSIZE];
    sprintf(debugstr, "REAL Random::getNormal() = %lf",
        (double) result);
    lua_message(debugstr);
#endif
    return result;
}

/**
 * Get a normally distributed value with variance equal to square of varsqrt,
 * that is, N(0, varsqrt * varsqrt).
 */
REAL Random::getNormal(REAL varsqrt)
{
    ASSERT(varsqrt > 0);
    REAL result = getNormal() * varsqrt;
#if RANDOM_DL >= 1
    char debugstr[STDBUFSIZE];
    sprintf(debugstr, "REAL Random::getNormal(REAL varsqrt = %lf) = %lf",
        (double) varsqrt, (double) result);
    lua_message(debugstr);
#endif
    return result;
}

/**
 * Initialize random number generator with the init_num value.
 * Then make preiters iterations (0 if omitted).
 */
void Random::init(long init_num, long preiters)
{
#if RANDOM_DL >= 1
    char debugstr[STDBUFSIZE];
    sprintf(debugstr, "void Random::init(long init_num = %ld, long preiters = %ld)",
        init_num, preiters);
    lua_message(debugstr);
#endif
    ASSERT(init_num >= 0);
    ASSERT(preiters >= 0);
    m_init = init_num;
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
