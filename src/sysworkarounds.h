/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

/*
 * This file is to contain various ugly workarounds
 * for bugs and misfeatures in system libraries
 */

#if !defined(__SYSWORKAROUNDS_H__)
#define __SYSWORKAROUNDS_H__

/* compile usable GCC version */
#if defined(__GNUC__)
#if !defined __GNUC_PATCHLEVEL__
#define __GNUC_PATCHLEVEL__ 0
#endif
#define GCC_VERSION (__GNUC__ * 10000 \
                   + __GNUC_MINOR__ * 100 \
                   + __GNUC_PATCHLEVEL__)
#else
/* non-GNU safety */
#define GCC_VERSION 0
#endif

#if (GCC_VERSION > 30200)

/* libstdc++ from pre-3.2.1 GCC has this operator horribly broken. */
#define ISTREAM_TO_STRING(input_stream, output_string)         \
    {                                                          \
        std::stringstream __temporary_stringstream;            \
        __temporary_stringstream<<input_stream.rdbuf();        \
        output_string = __temporary_stringstream.str();        \
    }

#else
/* so here's a dumb workaround
 * (but we should really consider deprecating pre-3.2.1 GCCs) */

#define ISTREAM_TO_STRING(input_stream, output_string)              \
    {                                                               \
        input_stream.seekg(0, std::ios::end);                       \
        int __temporary_buf_len = input_stream.tellg();             \
        char *__temporary_buf = new char[__temporary_buf_len];      \
        input_stream.seekg(0, std::ios::beg);                       \
        input_stream.read(__temporary_buf, __temporary_buf_len);    \
        output_string.assign(__temporary_buf, __temporary_buf_len); \
        delete [] __temporary_buf;                                  \
    }

#endif


#endif /* __SYSWORKAROUNDS_H__ */
