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
 *      Public library functions.
 *
 *      See the readme.txt file for instructions on using this package in your
 *      own programs.
 *
 */


#include "internal.h"


const unsigned char _jpeg_zigzag_scan[64] = {
	 0, 1, 5, 6,14,15,27,28,
	 2, 4, 7,13,16,26,29,42,
	 3, 8,12,17,25,30,41,43,
	 9,11,18,24,31,40,44,53,
	10,19,23,32,39,45,52,54,
	20,22,33,38,46,51,55,60,
	21,34,37,47,50,56,59,61,
	35,36,48,49,57,58,62,63
};

int jpgalleg_error = JPG_ERROR_NONE;


/* load_datafile_jpg:
 *  Hook function for loading a JPEG object from a datafile. Returns the
 *  decoded JPG into a BITMAP or NULL on error.
 */
static void *
load_datafile_jpg(PACKFILE *f, long size)
{
	BITMAP *bmp;
	char *buffer;
	
	buffer = (char *)malloc(size);
	if (!buffer)
		return NULL;
	pack_fread(buffer, size, f);
	bmp = load_memory_jpg(buffer, size, NULL);
	free(buffer);
	
	return (void *)bmp;
}


/* destroy_datafile_jpg:
 *  Hook function for freeing memory of JPEG objects in a loaded datafile.
 */
static void
destroy_datafile_jpg(void *data)
{
	if (data)
		destroy_bitmap((BITMAP *)data);
}


/* jpgalleg_init:
 *  Initializes JPGalleg by registering the file format with the Allegro image
 *  handling and datafile subsystems.
 */
int
jpgalleg_init(void)
{
	register_datafile_object(DAT_JPEG, load_datafile_jpg, destroy_datafile_jpg);
	register_bitmap_file_type("jpg", load_jpg, save_jpg);
	jpgalleg_error = JPG_ERROR_NONE;
	
	return 0;
}


/* load_jpg:
 *  Loads a JPG image from a file into a BITMAP.
 */
BITMAP *
load_jpg(AL_CONST char *filename, RGB *palette)
{
	PACKFILE *f;
	BITMAP *bmp;
	
	f = pack_fopen(filename, F_READ);
	if (!f)
		return NULL;
	_jpeg_init_file_io(f);
	
	bmp = _jpeg_decode(palette);
	
	pack_fclose(f);
	return bmp;
}


/* load_memory_jpg:
 *  Loads a JPG image from a memory buffer into a BITMAP.
 */
BITMAP *
load_memory_jpg(void *buffer, int size, RGB *palette)
{
	BITMAP *bmp;
	
	_jpeg_init_memory_io(buffer, size);
	
	bmp = _jpeg_decode(palette);

	return bmp;
}


/* save_jpg:
 *  Saves specified BITMAP into a JPG file with quality 75 and no subsampling.
 */
int
save_jpg(AL_CONST char *filename, BITMAP *bmp, AL_CONST RGB *palette)
{
	return save_jpg_ex(filename, bmp, palette, DEFAULT_QUALITY, DEFAULT_FLAGS);
}


/* save_jpg_ex:
 *  Saves a BITMAP into a JPG file using given quality and subsampling mode.
 */
int
save_jpg_ex(AL_CONST char *filename, BITMAP *bmp, AL_CONST RGB *palette, int quality, int flags)
{
	PACKFILE *f;
	PALETTE pal;
	int result;
	
	if (!palette)
		palette = pal;
	
	f = pack_fopen(filename, F_WRITE);
	if (!f)
		return -1;
	_jpeg_init_file_io(f);
	
	result = _jpeg_encode(bmp, palette, quality, flags);
	
	pack_fclose(f);
	return result;
}


/* save_memory_jpg:
 *  Saves a BITMAP in JPG format and stores it into a memory buffer. The JPG
 *  is saved with quality 75 and no subsampling.
 */
int
save_memory_jpg(void *buffer, int *size, BITMAP *bmp, AL_CONST RGB *palette)
{
	return save_memory_jpg_ex(buffer, size, bmp, palette, DEFAULT_QUALITY, DEFAULT_FLAGS);
}


/* save_memory_jpg_ex:
 *  Saves a BITMAP in JPG format using given quality and subsampling settings
 *  and stores it into a memory buffer.
 */
int
save_memory_jpg_ex(void *buffer, int *size, BITMAP *bmp, AL_CONST RGB *palette, int quality, int flags)
{
	int result;
	
	if (!buffer)
		return -1;
	_jpeg_init_memory_io(buffer, *size);
	*size = 0;
	
	result = _jpeg_encode(bmp, palette, quality, flags);
	
	if (result == 0)
		*size = _jpeg_memory_size();
	return result;
}
