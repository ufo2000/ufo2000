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
 *      Input/output helper routines module.
 *
 *      See the readme.txt file for instructions on using this package in your
 *      own programs.
 */


#include "internal.h"


static PACKFILE *f = NULL;
static int (*getc_func)() = NULL;
static int (*putc_func)(int c) = NULL;
static int current_bit = 0;
static int current_byte = 0;
static int bytes_read = 0;
static int chunk_len = 0;
static void *chunk = NULL;
static unsigned char *buffer = NULL;
static int buffer_pos = 0;
static int buffer_size = 0;


 
/* file_io_getc:
 *  Byte input routine for file access mode.
 */
static int
file_io_getc()
{
	int result = pack_getc(f);
	
	if (result < 0)
		jpgalleg_error = JPG_ERROR_READING_FILE;
	return result;
}


/* file_io_putc:
 *  Byte output routine for file access mode.
 */
static int
file_io_putc(int c)
{
	if (pack_putc(c, f) != c) {
		jpgalleg_error = JPG_ERROR_WRITING_FILE;
		return -1;
	}
	return 0;
}


/* _jpeg_init_file_io:
 *  Initializes the I/O system for file access.
 */
void
_jpeg_init_file_io(PACKFILE *jpeg_file)
{
	f = jpeg_file;
	getc_func = file_io_getc;
	putc_func = file_io_putc;
	current_bit = 0;
}


/* memory_io_getc:
 *  Byte input routine for memory access mode.
 */
static int
memory_io_getc()
{
	int result;
	
	if (buffer_pos >= buffer_size) {
		jpgalleg_error = JPG_ERROR_INPUT_BUFFER_TOO_SMALL;
		return -1;
	}
	result = buffer[buffer_pos];
	buffer_pos++;
	return result;
}


/* memory_io_putc:
 *  Byte output routine for memory access mode.
 */
static int
memory_io_putc(int c)
{
	if (buffer_pos >= buffer_size) {
		jpgalleg_error = JPG_ERROR_OUTPUT_BUFFER_TOO_SMALL;
		return -1;
	}
	buffer[buffer_pos] = c;
	buffer_pos++;
	return 0;
}


/* _jpeg_init_memory_io:
 *  Initializes the I/O system for memory access.
 */
void
_jpeg_init_memory_io(void *data, int size)
{
	buffer = (unsigned char *)data;
	buffer_size = size;
	buffer_pos = 0;
	getc_func = memory_io_getc;
	putc_func = memory_io_putc;
	current_bit = 0;
}


/* _jpeg_memory size:
 *  Returns the number of bytes read/written in memory.
 */
int
_jpeg_memory_size()
{
	return buffer_pos;
}


/* _jpeg_getc:
 *  Reads a byte from the input stream.
 */
int
_jpeg_getc()
{
	bytes_read++;
	current_bit = 0;
	return getc_func();
}


/* _jpeg_putc:
 *  Writes a byte to the output stream.
 */
int
_jpeg_putc(int c)
{
	return putc_func(c);
}


/* _jpeg_getw:
 *  Reads a word from the input stream.
 */
int
_jpeg_getw()
{
	return (_jpeg_getc() << 8) | _jpeg_getc();
}


/* _jpeg_putw:
 *  Writes a word to the output stream.
 */
int
_jpeg_putw(int w)
{
	int result;
	
	result = _jpeg_putc((w >> 8) & 0xff);
	result |= _jpeg_putc(w & 0xff);
	return result;
}


/* _jpeg_get_bit:
 *  Reads a bit from the input stream.
 */
int
_jpeg_get_bit()
{
	current_bit--;
	if (current_bit < 0) {
		current_byte = getc_func();
		current_bit = 7;
		if (current_byte == 0xff)
			/* Special encoding for 0xff, which in JPGs is encoded like 2 bytes:
			 * 0xff00. Here we skip the next byte (0x00)
			 */
			getc_func();
		else if (current_byte == -1)
			return -1;
	}
	return (current_byte >> current_bit) & 0x1;
}


/* _jpeg_put_bit:
 *  Writes a bit to the output stream.
 */
int
_jpeg_put_bit(int bit)
{
	current_byte |= (bit << current_bit);
	current_bit--;
	if (current_bit < 0) {
		if (putc_func(current_byte))
			return -1;
		if (current_byte == 0xff)
			putc_func(0);
		current_bit = 7;
		current_byte = 0;
	}
	return 0;
}


/* _jpeg_flush_bits:
 *  Flushes the current byte by filling unset bits with 1.
 */
void
_jpeg_flush_bits()
{
	while (current_bit < 7)
		_jpeg_put_bit(1);
}


/* _jpeg_open_chunk:
 *  Opens a chunk for reading.
 */
void
_jpeg_open_chunk()
{
	bytes_read = 0;
	chunk_len = _jpeg_getw();
	current_bit = 0;
}


/* _jpeg_close_chunk:
 *  Closes the chunk being read, eventually skipping unused bytes.
 */
void
_jpeg_close_chunk()
{
	while (bytes_read < chunk_len)
		_jpeg_getc();
	current_bit = 0;
}


/* _jpeg_eoc:
 *  Returns true if the end of chunk being read is reached, otherwise false.
 */
int
_jpeg_eoc()
{
	return (bytes_read < chunk_len) ? FALSE : TRUE;
}


/* _jpeg_new_chunk:
 *  Creates a new chunk for writing.
 */
void
_jpeg_new_chunk(int type)
{
	char *c = (char *)malloc(65536);
	
	c[0] = 0xff;
	c[1] = type;
	chunk_len = 2;
	chunk = c;
}


/* _jpeg_write_chunk:
 *  Writes the current chunk to the output stream.
 */
void
_jpeg_write_chunk()
{
	char *c;
	
	if (!chunk)
		return;
	c = (char *)chunk;
	c[2] = (chunk_len >> 8) & 0xff;
	c[3] = chunk_len & 0xff;
	chunk_len += 2;
	
	while (chunk_len > 0) {
		_jpeg_putc(*c);
		chunk_len--;
		c++;
	}
	free(chunk);
	chunk = NULL;
	current_bit = 7;
	current_byte = 0;
}


/* _jpeg_chunk_putc:
 *  Writes a byte to the current chunk.
 */
void
_jpeg_chunk_putc(int c)
{
	char *p = (char *)chunk + chunk_len + 2;
	
	*p = c;
	chunk_len++;
}


/* _jpeg_chunk_putw:
 *  Writes a word to the current chunk.
 */
void
_jpeg_chunk_putw(int w)
{
	_jpeg_chunk_putc((w >> 8) & 0xff);
	_jpeg_chunk_putc(w & 0xff);
}
