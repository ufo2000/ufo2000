/*
 *         __   _____    ______   ______   ___    ___
 *        /\ \ /\  _ `\ /\  ___\ /\  _  \ /\_ \  /\_ \
 *        \ \ \\ \ \L\ \\ \ \__/ \ \ \L\ \\//\ \ \//\ \      __     __
 *      __ \ \ \\ \  __| \ \ \  __\ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\
 *     /\ \_\/ / \ \ \/   \ \ \L\ \\ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \
 *     \ \____//  \ \_\    \ \____/ \ \_\ \_\/\____\/\____\ \____\ \____ \
 *      \/____/    \/_/     \/___/   \/_/\/_/\/____/\/____/\/____/\/___L\ \
 *                                                                  /\____/
 *                                                                  \_/__/
 *
 *      Version 2.2, by Angelo Mottola, 2000-2003.
 *
 *      Public header file.
 *
 *      See the readme.txt file for instructions on using this package in your
 *      own programs.
 */

#ifndef _JPGALLEG_H_
#define _JPGALLEG_H_

#include <allegro.h>

/* Subsampling mode */
#define JPG_SAMPLING_444			0
#define JPG_SAMPLING_422			1
#define JPG_SAMPLING_411			2

/* Force greyscale when saving */
#define JPG_GREYSCALE				0x10


/* Error codes */
#define JPG_ERROR_NONE				0
#define JPG_ERROR_READING_FILE			-1
#define JPG_ERROR_WRITING_FILE			-2
#define JPG_ERROR_INPUT_BUFFER_TOO_SMALL	-3
#define JPG_ERROR_OUTPUT_BUFFER_TOO_SMALL	-4
#define JPG_ERROR_HUFFMAN			-5
#define JPG_ERROR_NOT_JFIF			-6
#define JPG_ERROR_UNSUPPORTED_ENCODING		-7
#define JPG_ERROR_UNSUPPORTED_COLOR_SPACE	-8
#define JPG_ERROR_UNSUPPORTED_DATA_PRECISION	-9
#define JPG_ERROR_BAD_IMAGE			-10
#define JPG_ERROR_OUT_OF_MEMORY			-11


/* Datafile object type for JPG images */
#define DAT_JPEG				DAT_ID('J','P','E','G')


#ifdef __cplusplus
extern "C" {
#endif


extern int jpgalleg_init(void);

extern BITMAP *load_jpg(AL_CONST char *filename, RGB *palette);
extern BITMAP *load_memory_jpg(void *buffer, int size, RGB *palette);

extern int save_jpg(AL_CONST char *filename, BITMAP *image, AL_CONST RGB *palette);
extern int save_jpg_ex(AL_CONST char *filename, BITMAP *image, AL_CONST RGB *palette, int quality, int flags);
extern int save_memory_jpg(void *buffer, int *size, BITMAP *image, AL_CONST RGB *palette);
extern int save_memory_jpg_ex(void *buffer, int *size, BITMAP *image, AL_CONST RGB *palette, int quality, int flags);

extern int jpgalleg_error;


#ifdef __cplusplus
}
#endif

#endif
