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
#ifndef RANDOM_H
#define RANDOM_H
#include "stdafx.h"
#include "global.h"

/**
 * Based on Donald E. Knuth's algorithm using subtractive method.
 * See the "Numerical Recipes In C" book, chapter 7.1
 * (available online at www.nr.com) for details.
 * The implementation should be portable enough.
 */
class Random: public persist::BaseObject
{
    DECLARE_PERSISTENCE(Random);
private:
    static const long MBIG = 1000000000;
    static const long MSEED = 161803398;
    int m_inext, m_inextp;
    long m_a[56];
    long m_j, m_k, m_init, m_iters;
    REAL m_normalsaved;
    bool m_iff, m_normalready;
    void Random::next() {
        ASSERT (m_iff);
        if (++m_inext == 56) m_inext = 1;
        if (++m_inextp == 56) m_inextp = 1;
        m_j = m_a[m_inext] - m_a[m_inextp];
        if (m_j < 0) m_j += MBIG;
        m_a[m_inext] = m_j;
        m_iters++;
    }
public:
    long get();
    REAL getUniform();
    REAL getUniform(REAL a, REAL b);
    REAL getNormal();
    REAL getNormal(REAL varsqrt);
    void init(long init_num, long preiters = 0);
    void save(long &state_init, long &state_iters, bool &state_normalready, REAL &state_normalsaved) {
        ASSERT (m_iff);
        state_init = m_init;
        state_iters = m_iters;
        state_normalready = m_normalready;
        state_normalsaved = m_normalsaved;
    }
    void load(long state_init, long state_iters, bool state_normalready, REAL state_normalsaved) {
        init(state_init, state_iters);
        m_normalready = state_normalready;
        m_normalsaved = state_normalsaved;
    }
    Random();
    Random(long init_num, long preiters = 0);
    ~Random();

    virtual bool Write(persist::Engine &archive) const;
    virtual bool Read(persist::Engine &archive);
};
#endif
