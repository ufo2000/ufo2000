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
 *      Decoder core module.
 *
 *      See the readme.txt file for instructions on using this package in your
 *      own programs.
 */


#include "internal.h"


static HUFFMAN_TABLE huffman_ac_table[4];
static HUFFMAN_TABLE huffman_dc_table[4];
static HUFFMAN_TABLE *ac_luminance_table, *dc_luminance_table;
static HUFFMAN_TABLE *ac_chrominance_table, *dc_chrominance_table;
static DATA_BUFFER *data_buffer[3];
static short quantization_table[128];
static short *luminance_quantization_table, *chrominance_quantization_table;
static int jpeg_w, jpeg_h, jpeg_components;
static int sampling, restart_interval, skip_count;
static int spectrum_start, spectrum_end, successive_high, successive_low;
static int scan_components, component[3];
static void (*ycbcr2rgb)(int address, int y1, int cb1, int cr1, int y2, int cb2, int cr2);
static void (*plot)(int addr, int pitch, short *y1, short *y2, short *y3, short *y4, short *cb, short *cr);



/* apply_idct:
 *  Applies the inverse discrete cosine transform to the given input data,
 *  in the form of a vector of 64 coefficients.
 *  This uses integer fixed point math and is based on code by the IJG.
 */
static void
apply_idct(short *data, short *output, short *dequant, int *workspace)
{
	int tmp0, tmp1, tmp2, tmp3;
	int tmp10, tmp11, tmp12, tmp13;
	int z1, z2, z3, z4, z5;
	int i, temp;
	short *inptr, *dqptr, *outptr;
	int *wsptr;
	
	/* Pass 1 */
	inptr = data;
	dqptr = dequant;
	wsptr = workspace;
	for (i = 8; i; i--) {
		if ((inptr[8] | inptr[16] | inptr[24] | inptr[32] | inptr[40] | inptr[48] | inptr[56]) == 0) {
			/* AC terms all zero */
			temp = (inptr[0] * dqptr[0]) << 2;
			
			wsptr[0] = temp;
			wsptr[8] = temp;
			wsptr[16] = temp;
			wsptr[24] = temp;
			wsptr[32] = temp;
			wsptr[40] = temp;
			wsptr[48] = temp;
			wsptr[56] = temp;
		}
		else {
			z2 = inptr[16] * dqptr[16];
			z3 = inptr[48] * dqptr[48];
			z1 = (z2 + z3) * FIX_0_541196100;
			tmp2 = z1 + (z3 * (- FIX_1_847759065));
			tmp3 = z1 + (z2 * FIX_0_765366865);
			z2 = inptr[0] * dqptr[0];
			z3 = inptr[32] * dqptr[32];
			tmp0 = (z2 + z3) << 13;
			tmp1 = (z2 - z3) << 13;
			tmp10 = tmp0 + tmp3;
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			tmp0 = inptr[56] * dqptr[56];
			tmp1 = inptr[40] * dqptr[40];
			tmp2 = inptr[24] * dqptr[24];
			tmp3 = inptr[8] * dqptr[8];
			z1 = tmp0 + tmp3;
			z2 = tmp1 + tmp2;
			z3 = tmp0 + tmp2;
			z4 = tmp1 + tmp3;
			z5 = (z3 + z4) * FIX_1_175875602;
			tmp0 *= FIX_0_298631336;
			tmp1 *= FIX_2_053119869;
			tmp2 *= FIX_3_072711026;
			tmp3 *= FIX_1_501321110;
			z1 *= (- FIX_0_899976223);
			z2 *= (- FIX_2_562915447);
			z3 *= (- FIX_1_961570560);
			z4 *= (- FIX_0_390180644);
			z3 += z5;
			z4 += z5;
			tmp0 += z1 + z3;
			tmp1 += z2 + z4;
			tmp2 += z2 + z3;
			tmp3 += z1 + z4;
			
			wsptr[0]  = (tmp10 + tmp3) >> 11;
			wsptr[56] = (tmp10 - tmp3) >> 11;
			wsptr[8]  = (tmp11 + tmp2) >> 11;
			wsptr[48] = (tmp11 - tmp2) >> 11;
			wsptr[16] = (tmp12 + tmp1) >> 11;
			wsptr[40] = (tmp12 - tmp1) >> 11;
			wsptr[24] = (tmp13 + tmp0) >> 11;
			wsptr[32] = (tmp13 - tmp0) >> 11;
		}
		inptr++;
		dqptr++;
		wsptr++;
	}
	
	/* Pass 2 */
	wsptr = workspace;
	outptr = output;
	for (i = 8; i; i--) {
		z2 = wsptr[2];
		z3 = wsptr[6];
		z1 = (z2 + z3) * FIX_0_541196100;
		tmp2 = z1 + (z3 * (- FIX_1_847759065));
		tmp3 = z1 + (z2 * FIX_0_765366865);
		tmp0 = (wsptr[0] + wsptr[4]) << 13;
		tmp1 = (wsptr[0] - wsptr[4]) << 13;
		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;
		tmp0 = wsptr[7];
		tmp1 = wsptr[5];
		tmp2 = wsptr[3];
		tmp3 = wsptr[1];
		z1 = tmp0 + tmp3;
		z2 = tmp1 + tmp2;
		z3 = tmp0 + tmp2;
		z4 = tmp1 + tmp3;
		z5 = (z3 + z4) * FIX_1_175875602;
		tmp0 *= FIX_0_298631336;
		tmp1 *= FIX_2_053119869;
		tmp2 *= FIX_3_072711026;
		tmp3 *= FIX_1_501321110;
		z1 *= (- FIX_0_899976223);
		z2 *= (- FIX_2_562915447);
		z3 *= (- FIX_1_961570560);
		z4 *= (- FIX_0_390180644);
		z3 += z5;
		z4 += z5;
		tmp0 += z1 + z3;
		tmp1 += z2 + z4;
		tmp2 += z2 + z3;
		tmp3 += z1 + z4;
		
		outptr[0] = ((tmp10 + tmp3) >> 18) + 128;
		outptr[7] = ((tmp10 - tmp3) >> 18) + 128;
		outptr[1] = ((tmp11 + tmp2) >> 18) + 128;
		outptr[6] = ((tmp11 - tmp2) >> 18) + 128;
		outptr[2] = ((tmp12 + tmp1) >> 18) + 128;
		outptr[5] = ((tmp12 - tmp1) >> 18) + 128;
		outptr[3] = ((tmp13 + tmp0) >> 18) + 128;
		outptr[4] = ((tmp13 - tmp0) >> 18) + 128;
		
		outptr += 8;
		wsptr += 8;
	}
}


/* zigzag_reorder:
 *  Reorders a vector of 64 coefficients by the zigzag scan.
 */
static void
zigzag_reorder(short *input, short *output)
{
	int i;
	
	for (i = 0; i < 64; i++)
		output[i] = input[_jpeg_zigzag_scan[i]];
}


/* free_huffman_table:
 *  Frees memory used by the huffman decoding lookup tables.
 */
static void
free_huffman_table(HUFFMAN_TABLE *table)
{
	int i;
	
	for (i = 0; i < 16; i++) {
		if (table->entry_of_length)
			free(table->entry_of_length[i]);
	}
}


/* read_dht_chunk:
 *  Reads a DHT (Define Huffman Table) chunk from the input stream.
 */
static int
read_dht_chunk()
{
	int i, j, table_id, num_codes[16];
	int code, value;
	unsigned char data;
	HUFFMAN_TABLE *table;
	HUFFMAN_ENTRY *entry;
	
	_jpeg_open_chunk();
	do {
		data = _jpeg_getc();
		if (data & 0xe0) {
			jpgalleg_error = JPG_ERROR_BAD_IMAGE;
			return -1;
		}
		table_id = data & 0xf;
		if (table_id > 3) {
			jpgalleg_error = JPG_ERROR_BAD_IMAGE;
			return -1;
		}
		if (data & 0x10)
			table = &huffman_ac_table[table_id];
		else
			table = &huffman_dc_table[table_id];
		for (i = 0; i < 16; i++)
			num_codes[i] = _jpeg_getc();
		code = 0;
		for (i = 0; i < 16; i++) {
			if (table->entry_of_length[i])
				free(table->entry_of_length[i]);
			table->entry_of_length[i] = (HUFFMAN_ENTRY *)calloc(1 << (i + 1), sizeof(HUFFMAN_ENTRY));
			if (!table->entry_of_length[i]) {
				jpgalleg_error = JPG_ERROR_OUT_OF_MEMORY;
				return -1;
			}
			for (j = 0; j < num_codes[i]; j++) {
				value = _jpeg_getc();
				entry = &table->entry_of_length[i][code];
				entry->value = value;
				entry->encoded_value = code;
				entry->bits_length = i + 1;
				code++;
			}
			code <<= 1;
		}
	} while (!_jpeg_eoc());
	_jpeg_close_chunk();
	
	return 0;
}


/* read_sof0_chunk:
 *  Reads a SOF0 (Start Of Frame 0) chunk from the input stream.
 */
static int
read_sof0_chunk()
{
	int i, data;
	
	_jpeg_open_chunk();
	if (_jpeg_getc() != 8) {
		jpgalleg_error = JPG_ERROR_UNSUPPORTED_DATA_PRECISION;
		return -1;
	}
	jpeg_h = _jpeg_getw();
	jpeg_w = _jpeg_getw();
	jpeg_components = _jpeg_getc();
	if ((jpeg_components != 1) && (jpeg_components != 3)) {
		jpgalleg_error = JPG_ERROR_UNSUPPORTED_COLOR_SPACE;
		return -1;
	}
	for (i = 0; i < jpeg_components; i++) {
		switch (_jpeg_getc()) {
			case 1:
				data = _jpeg_getc();
				sampling = (data & 0xf) * (data >> 4);
				if ((sampling != 1) && (sampling != 2) && (sampling != 4)) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				data = _jpeg_getc();
				if (data > 1) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				luminance_quantization_table = &quantization_table[data * 64];
				break;
			case 2:
			case 3:
				_jpeg_getc();
				data = _jpeg_getc();
				if (data > 1) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				chrominance_quantization_table = &quantization_table[data * 64];
				break;
		}
	}
	_jpeg_close_chunk();
	
	return 0;
}


/* read_dqt_chunk:
 *  Reads a DQT (Define Quantization Table) chunk from the input stream.
 */
static int
read_dqt_chunk()
{
	int i, data;
	short *table, temp[64];
	
	_jpeg_open_chunk();
	do {
  		data = _jpeg_getc();
		if ((data & 0xf) > 1) {
			jpgalleg_error = JPG_ERROR_BAD_IMAGE;
			return -1;
		}
		if (data & 0xf0) {
			jpgalleg_error = JPG_ERROR_UNSUPPORTED_DATA_PRECISION;
			return -1;
		}
		table = &quantization_table[(data & 0xf) * 64];
		for (i = 0; i < 64; i++)
			temp[i] = _jpeg_getc();
		zigzag_reorder(temp, table);
	} while (!_jpeg_eoc());
	_jpeg_close_chunk();
	
	return 0;
}


/* read_sos_chunk:
 *  Reads a SOS (Start Of Scan) chunk from the input stream.
 */
static int
read_sos_chunk()
{
	int i, data;
	
	_jpeg_open_chunk();
	scan_components = _jpeg_getc();
	if (scan_components > 3) {
		jpgalleg_error = JPG_ERROR_UNSUPPORTED_COLOR_SPACE;
		return -1;
	}
	for (i = 0; i < scan_components; i++) {
		component[i] = _jpeg_getc();
		switch (component[i]) {
			case 1:
				data = _jpeg_getc();
				if (((data & 0xf) > 3) || ((data >> 4) > 3)) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				ac_luminance_table = &huffman_ac_table[data & 0xf];
				dc_luminance_table = &huffman_dc_table[data >> 4];
				break;
			case 2:
			case 3:
				data = _jpeg_getc();
				if (((data & 0xf) > 3) || ((data >> 4) > 3)) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				ac_chrominance_table = &huffman_ac_table[data & 0xf];
				dc_chrominance_table = &huffman_dc_table[data >> 4];
				break;
		}
	}
	spectrum_start = _jpeg_getc();
	spectrum_end = _jpeg_getc();
	data = _jpeg_getc();
	successive_high = data >> 4;
	successive_low = data & 0xf;
	_jpeg_close_chunk();
	skip_count = 0;
	return 0;
}


/* read_app0_chunk:
 *  Reads an APP0 (JFIF descriptor) chunk from the input stream.
 */
static int
read_app0_chunk()
{
	const char *jfif_id = "JFIF";
	int i;
	
	_jpeg_open_chunk();
	for (i = 0; i < 5; i++) {
		if (_jpeg_getc() != jfif_id[i]) {
			_jpeg_close_chunk();
			jpgalleg_error = JPG_ERROR_NOT_JFIF;
			return -1;
		}
	}
	/* Only JFIF version 1.x is supported */
	if (_jpeg_getc() != 1) { _jpeg_close_chunk(); return -1; }
	_jpeg_close_chunk();
	return 0;
}


/* read_dri_chunk:
 *  Reads a DRI (Define Restart Interval) chunk from the input stream.
 */
static int
read_dri_chunk()
{
	_jpeg_open_chunk();
	restart_interval = _jpeg_getw();
	_jpeg_close_chunk();
	return 0;
}


/* get_bits:
 *  Reads a value from the input stream, given its category.
 */
static int
get_bits(int category)
{
	int i, bit, result = 0;
	
	for (i = 0; i < category; i++) {
		if ((bit = _jpeg_get_bit()) < 0)
			return 0x80000000;
		result = (result << 1) | bit;
	}
	if (result >= (1 << (category - 1)))
		return result;
	else
		return result - ((1 << category) - 1);
}


/* huffman_decode:
 *  Fetches bits from the input stream until a valid huffman code is found,
 *  then returns the value associated with that code.
 */
static int
huffman_decode(HUFFMAN_TABLE *table)
{
	HUFFMAN_ENTRY *entry;
	int i, value = 0;

	for (i = 1; i <= 16; i++) {
		value = (value << 1) | _jpeg_get_bit();
		entry = &table->entry_of_length[i - 1][value];
		if (entry->bits_length == i)
			return entry->value;
	}
	return -1;
}


/* decode_baseline_block:
 *  Decodes an 8x8 basic block of coefficients of given type (luminance or
 *  chrominance) from the input stream. Used for baseline decoding.
 */
static int
decode_baseline_block(short *block, int type, int *old_dc)
{
	HUFFMAN_TABLE *dc_table, *ac_table;
	short *quant_table;
	int data, i, index;
	int num_zeroes, category;
	int workspace[64];
	short pre_idct_block[80];
	short ordered_pre_idct_block[64];
	
	if (type == LUMINANCE) {
		dc_table = dc_luminance_table;
		ac_table = ac_luminance_table;
		quant_table = luminance_quantization_table;
	}
	else {
		dc_table = dc_chrominance_table;
		ac_table = ac_chrominance_table;
		quant_table = chrominance_quantization_table;
	}
	
	data = huffman_decode(dc_table);
	if ((data = get_bits(data & 0xf)) == (int)0x80000000)
		return -1;
	*old_dc += data;
	pre_idct_block[0] = *old_dc;
	
	index = 1;
	do {
		data = huffman_decode(ac_table);
		if (data < 0) {
			/* Bad block */
			jpgalleg_error = JPG_ERROR_BAD_IMAGE;
			return -1;
		}
		num_zeroes = data >> 4;
		category = data & 0xf;
		if ((num_zeroes == 0) && (category == 0)) {
			/* End of block */
			for (i = index; i < 64; i++)
				pre_idct_block[i] = 0;
			break;
		}
		else if ((num_zeroes == 15) && (category == 0)) {
			/* 16 zeroes special case */
			for (i = 0; i < 16; i++)
				pre_idct_block[index++] = 0;
		}
		else {
			/* Normal zero run length coding */
			for (i = 0; i < num_zeroes; i++)
				pre_idct_block[index++] = 0;
			if ((data = get_bits(category)) == (int)0x80000000)
				return -1;
			pre_idct_block[index++] = data;
		}
	} while (index < 64);
	
	zigzag_reorder(pre_idct_block, ordered_pre_idct_block);
	
	apply_idct(ordered_pre_idct_block, block, quant_table, workspace);
	
	return 0;
}


/* decode_progressive_block
 *  Decodes some coefficients for an 8x8 block from the input stream. Used in
 *  progressive mode decoding.
 */
static int
decode_progressive_block(int addr, int type, int *old_dc)
{
	HUFFMAN_TABLE *dc_table, *ac_table;
	short *block = (short *)addr;
	int data, i, index, value;
	int num_zeroes, category;
	int p_bit, n_bit;
	
	if (type == LUMINANCE) {
		dc_table = dc_luminance_table;
		ac_table = ac_luminance_table;
	}
	else {
		dc_table = dc_chrominance_table;
		ac_table = ac_chrominance_table;
	}
	
	if (spectrum_start == 0) {
		/* DC scan */
		if (successive_high == 0) {
			/* First DC scan */
			data = huffman_decode(dc_table);
			if ((data = get_bits(data & 0xf)) == (int)0x80000000)
				return -1;
			*old_dc += data;
			block[0] = *old_dc << successive_low;
		}
		else {
			/* DC successive approximation */
			if ((data = _jpeg_get_bit()) < 0)
				return -1;
			if (data)
				block[0] |= (1 << successive_low);
		}
	}
	else {
		/* AC scan */
		if (successive_high == 0) {
			/* First AC scan */
			if (skip_count) {
				skip_count--;
				return 0;
			}
			index = spectrum_start;
			do {
				data = huffman_decode(ac_table);
				if (data < 0) {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return -1;
				}
				num_zeroes = data >> 4;
				category = data & 0xf;
				if (category == 0) {
					if (num_zeroes == 15)
						index += 16;
					else {
						index++;
						skip_count = 0;
						if (num_zeroes) {
							for (value = 0, i = 0; i < num_zeroes; i++) {
								if ((data = _jpeg_get_bit()) < 0)
									return -1;
								value = (value << 1) | data;
							}
							skip_count = (1 << num_zeroes) + value - 1;
						}
						break;
					}
				}
				else {
					index += num_zeroes;
					if ((data = get_bits(category)) == (int)0x80000000)
						return -1;
					block[index++] = data << successive_low;
				}
			} while (index <= spectrum_end);
		}
		else {
			/* AC successive approximation */
			index = spectrum_start;
			p_bit = 1 << successive_low;
			n_bit = (-1) << successive_low;
			if (skip_count == 0) {
				do {
					data = huffman_decode(ac_table);
					if (data < 0) {
						jpgalleg_error = JPG_ERROR_BAD_IMAGE;
						return -1;
					}
					num_zeroes = data >> 4;
					category = data & 0xf;
					if (category == 0) {
						if (num_zeroes < 15) {
							skip_count = 1 << num_zeroes;
							if (num_zeroes) {
								for (value = 0, i = 0; i < num_zeroes; i++) {
									if ((data = _jpeg_get_bit()) < 0)
										return -1;
									value = (value << 1) | data;
								}
								skip_count += value;
							}
							break;
						}
					}
					else if (category == 1) {
						if ((data = _jpeg_get_bit()) < 0)
							return -1;
						if (data)
							category = p_bit;
						else
							category = n_bit;
					}
					else {
						jpgalleg_error = JPG_ERROR_BAD_IMAGE;
						return -1;
					}
					do {
						if (block[index]) {
							if ((data = _jpeg_get_bit()) < 0)
								return -1;
							if ((data) && (!(block[index] & p_bit))) {
								if (block[index] >= 0)
									block[index] += p_bit;
								else
									block[index] += n_bit;
							}
						}
						else {
							num_zeroes--;
							if (num_zeroes < 0)
								break;
						}
						index++;
					} while (index <= spectrum_end);
					if ((category) && (index < 64))
						block[index] = category;
					index++;
				} while (index <= spectrum_end);
			}
			if (skip_count > 0) {
				while (index <= spectrum_end) {
					if (block[index]) {
						if ((data = _jpeg_get_bit()) < 0)
							return -1;
						if ((data) && (!(block[index] & p_bit))) {
							if (block[index] >= 0)
								block[index] += p_bit;
							else
								block[index] += n_bit;
						}
					}
					index++;
				}
				skip_count--;
			}
		}
	}
	
	return 0;
}


/* _jpeg_c_ycbcr2rgb:
 *  C version of the YCbCr -> RGB color conversion routine. Converts 2 pixels
 *  at a time.
 */
static void
_jpeg_c_ycbcr2rgb(int addr, int y1, int cb1, int cr1, int y2, int cb2, int cr2)
{
	int r, g, b;
	unsigned int *ptr = (unsigned int *)addr;

	r = MID(0, ((y1 << 8)                       + (359 * (cr1 - 128))) >> 8, 255);
	g = MID(0, ((y1 << 8) -  (88 * (cb1 - 128)) - (183 * (cr1 - 128))) >> 8, 255);
	b = MID(0, ((y1 << 8) + (453 * (cb1 - 128))                      ) >> 8, 255);
	ptr[0] = makecol32(r, g, b);
	r = MID(0, ((y2 << 8)                       + (359 * (cr2 - 128))) >> 8, 255);
	g = MID(0, ((y2 << 8) -  (88 * (cb2 - 128)) - (183 * (cr2 - 128))) >> 8, 255);
	b = MID(0, ((y2 << 8) + (453 * (cb2 - 128))                      ) >> 8, 255);
	ptr[1] = makecol32(r, g, b);
	return;
}


/* plot_444:
 *  Plots an 8x8 MCU block for 444 mode. Also used to plot greyscale MUCs.
 */
static void
plot_444(int addr, int pitch, short *y1, short *y2, short *y3, short *y4, short *cb, short *cr)
{
	int x, y;
	short *y1_ptr = y1, *cb_ptr = cb, *cr_ptr = cr;
	
	(void)y2;
	(void)y3;
	(void)y4;
	
	if (jpeg_components == 1) {
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x += 2) {
				ycbcr2rgb(addr, *y1_ptr, 128, 128, *(y1_ptr + 1), 128, 128);
				y1_ptr += 2;
				addr += 8;
			}
			addr += (pitch - 32);
		}
	}
	else {
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x += 2) {
				ycbcr2rgb(addr, *y1_ptr, *cb_ptr, *cr_ptr, *(y1_ptr + 1), *(cb_ptr + 1), *(cr_ptr + 1));
				y1_ptr += 2;
				cb_ptr += 2;
				cr_ptr += 2;
				addr += 8;
			}
			addr += (pitch - 32);
		}
	}
}


/* plot_422:
 *  Plots a 16x8 MCU block for 422 mode.
 */
static void
plot_422(int addr, int pitch, short *y1, short *y2, short *y3, short *y4, short *cb, short *cr)
{
	int x, y;
	short *y1_ptr = y1, *y2_ptr = y2, *cb_ptr = cb, *cr_ptr = cr;
	
	(void)y3;
	(void)y4;
	
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x += 2) {
			ycbcr2rgb(addr, *y1_ptr, *cb_ptr, *cr_ptr, *(y1_ptr + 1), *cb_ptr, *cr_ptr);
			ycbcr2rgb(addr + 32, *y2_ptr, *(cb_ptr + 4), *(cr_ptr + 4), *(y2_ptr + 1), *(cb_ptr + 4), *(cr_ptr + 4));
			y1_ptr += 2;
			y2_ptr += 2;
			cb_ptr++;
			cr_ptr++;
			addr += 8;
		}
		cb_ptr += 4;
		cr_ptr += 4;
		addr += (pitch - 32);
	}
}


/* plot_411:
 *  Plots a 16x16 MCU block for 411 mode.
 */
static void
plot_411(int addr, int pitch, short *y1, short *y2, short *y3, short *y4, short *cb, short *cr)
{
	int x, y;
	short *y1_ptr = y1, *y2_ptr = y2, *y3_ptr = y3, *y4_ptr = y4, *cb_ptr = cb, *cr_ptr = cr;
	
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x += 2) {
			ycbcr2rgb(addr, *y1_ptr, *cb_ptr, *cr_ptr, *(y1_ptr + 1), *cb_ptr, *cr_ptr);
			ycbcr2rgb(addr + 32, *y2_ptr, *(cb_ptr + 4), *(cr_ptr + 4), *(y2_ptr + 1), *(cb_ptr + 4), *(cr_ptr + 4));
			ycbcr2rgb(addr + (pitch * 8), *y3_ptr, *(cb_ptr + 32), *(cr_ptr + 32), *(y3_ptr + 1), *(cb_ptr + 32), *(cr_ptr + 32));
			ycbcr2rgb(addr + (pitch * 8) + 32, *y4_ptr, *(cb_ptr + 36), *(cr_ptr + 36), *(y4_ptr + 1), *(cb_ptr + 36), *(cr_ptr + 36));
			y1_ptr += 2;
			y2_ptr += 2;
			y3_ptr += 2;
			y4_ptr += 2;
			cb_ptr++;
			cr_ptr++;
			addr += 8;
		}
		if (y & 1) {
			cb_ptr += 4;
			cr_ptr += 4;
		}
		else {
			cb_ptr -= 4;
			cr_ptr -= 4;
		}
		addr += (pitch - 32);
	}
}


/* _jpeg_decode:
 *  Main decoding function.
 */
BITMAP *
_jpeg_decode(RGB *pal)
{
	const int x_ofs[4] = { 0, 1, 0, 1 }, y_ofs[4] = { 0, 0, 1, 1 };
	short coefs_buffer[384], coefs[64], *coefs_ptr, *temp_ptr;
	short *y1, *y2, *y3, *y4, *cb, *cr;
	int workspace[64];
	int addr, pitch, i, j;
	int block_x, block_y, block_max_x, block_max_y;
	int blocks_per_row[3];
	int blocks_in_mcu, block_component[6];
	short *block_ptr[6];
	int block_x_ofs[6], block_y_ofs[6];
	int mcu_w, mcu_h, component_w[3], component_h[3], c;
	int old_dc[3] = { 0, 0, 0 };
	BITMAP *bmp, *temp;
	int data, flags = 0;
	int restart_count;
	int depth;
	PALETTE tmppal;
	int want_palette = 1;
	
	if (!pal) {
		want_palette = 0;
		pal = tmppal;
	}

	jpgalleg_error = JPG_ERROR_NONE;
	
#ifdef JPGALLEG_MMX
	if (cpu_capabilities & CPU_MMX) {
		if (_rgb_r_shift_32 == 0)
			ycbcr2rgb = _jpeg_mmx_ycbcr2rgb;
		else if (_rgb_r_shift_32 == 16)
			ycbcr2rgb = _jpeg_mmx_ycbcr2bgr;
		else
			ycbcr2rgb = _jpeg_c_ycbcr2rgb;
	}
	else
#endif
	ycbcr2rgb = _jpeg_c_ycbcr2rgb;
	
	memset(huffman_dc_table, 0, 4 * sizeof(HUFFMAN_TABLE));
	memset(huffman_ac_table, 0, 4 * sizeof(HUFFMAN_TABLE));
	y1 = coefs_buffer;
	y2 = coefs_buffer + 64;
	y3 = coefs_buffer + 128;
	y4 = coefs_buffer + 192;
	cb = coefs_buffer + 256;
	cr = coefs_buffer + 320;
	
	if (_jpeg_getw() != CHUNK_SOI) {
		jpgalleg_error = JPG_ERROR_NOT_JFIF;
		return NULL;
	}
	
	/* Examine header */
	do {
		data = _jpeg_getc();
		if (data < 0)
			return NULL;
		else if (data == 0xff) {
			while ((data = _jpeg_getc()) == 0xff)
				;
			switch (data) {
				case -1:
					return NULL;
				case 0:
					break;
				case CHUNK_SOF2:
					flags |= IS_PROGRESSIVE;
					/* fallthrough */
				case CHUNK_SOF0:
				case CHUNK_SOF1:
					if (read_sof0_chunk())
						return NULL;
					flags |= SOF0_DEFINED;
					break;
				case CHUNK_DHT:
					if (read_dht_chunk())
						return NULL;
					flags |= DHT_DEFINED;
					break;
				case CHUNK_SOS:
					if (read_sos_chunk())
						return NULL;
					flags |= SOS_DEFINED;
					break;
				case CHUNK_DQT:
					if (read_dqt_chunk())
						return NULL;
					flags |= DQT_DEFINED;
					break;
				case CHUNK_APP0:
					if (read_app0_chunk())
						break;
					flags |= APP0_DEFINED;
					break;
				case CHUNK_DRI:
					if (read_dri_chunk())
						return NULL;
					flags |= DRI_DEFINED;
					break;
				case CHUNK_SOF3:
				case CHUNK_SOF5:
				case CHUNK_SOF6:
				case CHUNK_SOF7:
				case CHUNK_SOF9:
				case CHUNK_SOF10:
				case CHUNK_SOF11:
				case CHUNK_SOF13:
				case CHUNK_SOF14:
				case CHUNK_SOF15:
					jpgalleg_error = JPG_ERROR_UNSUPPORTED_ENCODING;
					return NULL;
				case CHUNK_JPG:
				case CHUNK_TEM:
				case CHUNK_RST0:
				case CHUNK_RST1:
				case CHUNK_RST2:
				case CHUNK_RST3:
				case CHUNK_RST4:
				case CHUNK_RST5:
				case CHUNK_RST6:
				case CHUNK_RST7:
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					return NULL;
				default:
					_jpeg_open_chunk();
					_jpeg_close_chunk();
					break;
			}
		}
	} while ((flags & JFIF_OK) != JFIF_OK);
	
	bmp = create_bitmap_ex(32, (jpeg_w + 15) & ~0xf, (jpeg_h + 15) & ~0xf);
	if (!bmp)
		return NULL;
	pitch = (int)(bmp->line[1] - bmp->line[0]);
	
	block_x = block_y = 0;
	restart_count = 0;
	memset(data_buffer, 0, 3 * sizeof(DATA_BUFFER *));
	
	if (!(flags & IS_PROGRESSIVE)) {
		/* Baseline decoding */
		blocks_in_mcu = 0;
		coefs_ptr = coefs_buffer;
		for (i = 0; i < sampling; i++) {
			block_component[blocks_in_mcu] = 0;
			block_ptr[blocks_in_mcu] = coefs_ptr;
			coefs_ptr += 64;
			blocks_in_mcu++;
		}
		for (i = 1; i < jpeg_components; i++) {
			block_component[blocks_in_mcu] = i;
			block_ptr[blocks_in_mcu] = coefs_ptr;
			coefs_ptr += 64;
			blocks_in_mcu++;
		}
		mcu_w = (sampling == 1) ? 8 : 16;
		mcu_h = (sampling != 4) ? 8 : 16;
		plot = plot_411;
		if (sampling < 4) {
			plot = plot_422;
			cb -= 128;
			cr -= 128;
		}
		if (sampling < 2) {
			plot = plot_444;
			cb -= 64;
			cr -= 64;
		}
		/* Start decoding! */
		do {
			for (i = 0; i < blocks_in_mcu; i++) {
				if (decode_baseline_block(block_ptr[i], (block_component[i] == 0) ? LUMINANCE : CHROMINANCE, &old_dc[block_component[i]]))
					goto exit_error;
			}
			addr = (int)bmp->line[block_y] + (block_x * 4);
			plot(addr, pitch, y1, y2, y3, y4, cb, cr);
			block_x += mcu_w;
			if (block_x >= jpeg_w) {
				block_x = 0;
				block_y += mcu_h;
			}
			restart_count++;
			if ((flags & DRI_DEFINED) && (restart_count >= restart_interval)) {
				data = _jpeg_getw();
				if (data == CHUNK_EOI)
					break;
				if ((data < CHUNK_RST0) || (data > CHUNK_RST7))
					goto exit_error;
				memset(old_dc, 0, 3 * sizeof(int));
				restart_count = 0;
			}
		} while (block_y < jpeg_h);
	}
	else {
		/* Progressive decoding */
		blocks_per_row[0] = bmp->w / 8;
		data_buffer[0] = (DATA_BUFFER *)calloc(1, sizeof(DATA_BUFFER) * (bmp->w / 8) * (bmp->h / 8));
		if (!data_buffer[0]) {
			jpgalleg_error = JPG_ERROR_OUT_OF_MEMORY;
			goto exit_error;
		}
		for (i = 1; i < jpeg_components; i++) {
			blocks_per_row[i] = (sampling == 1) ? (bmp->w / 8) : (bmp->w / 16);
			component_w[i] = component_h[i] = 1;
			data_buffer[i] = (DATA_BUFFER *)calloc(1, sizeof(DATA_BUFFER) * (bmp->w / 8) * (bmp->h / 8) / sampling);
			if (!data_buffer[i]) {
				jpgalleg_error = JPG_ERROR_OUT_OF_MEMORY;
				goto exit_error;
			}
		}
		while (1) {
			/* Decode new scan */
			if (((spectrum_start > spectrum_end) || (spectrum_end > 63)) ||
					((spectrum_start == 0) && (spectrum_end != 0)) ||
					((successive_high != 0) && (successive_high != successive_low + 1))) {
				jpgalleg_error = JPG_ERROR_BAD_IMAGE;
				goto exit_error;
			}
			restart_count = 0;
			block_x = block_y = 0;
			memset(old_dc, 0, 3 * sizeof(int));
			/* Setup MCU layout for this scan */
			blocks_in_mcu = 0;
			mcu_w = mcu_h = 8;
			for (i = 0; i < scan_components; i++) {
				switch (component[i]) {
					case 1:
						for (j = 0; j < sampling; j++) {
							block_component[blocks_in_mcu] = 0;
							block_x_ofs[blocks_in_mcu] = x_ofs[j];
							block_y_ofs[blocks_in_mcu] = y_ofs[j];
							blocks_in_mcu++;
						}
						break;
						
					case 2:
					case 3:
						block_component[blocks_in_mcu] = component[i] - 1;
						block_x_ofs[blocks_in_mcu] = 0;
						block_y_ofs[blocks_in_mcu] = 0;
						blocks_in_mcu++;
						mcu_w = MAX(mcu_w, (sampling == 1) ? 8 : 16);
						mcu_h = MAX(mcu_h, (sampling != 4) ? 8 : 16);
						break;
				}
			}
			block_max_x = (((jpeg_w + mcu_w - 1) & ~(mcu_w - 1)) / mcu_w);
			block_max_y = (((jpeg_h + mcu_h - 1) & ~(mcu_h - 1)) / mcu_h);
			/* Remove sampling dependency from luminance only scans */
			if ((scan_components == 1) && (block_component[0] == 0)) {
				blocks_in_mcu = 1;
				component_w[0] = component_h[0] = 1;
			}
			else {
				component_w[0] = (sampling == 1) ? 1 : 2;
				component_h[0] = (sampling != 4) ? 1 : 2;
			}
			/* Start decoding! */
			do {
				restart_count++;
				if ((flags & DRI_DEFINED) && (restart_count > restart_interval)) {
					data = _jpeg_getw();
					if ((data < CHUNK_RST0) || (data > CHUNK_RST7))
						goto exit_error;
					memset(old_dc, 0, 3 * sizeof(int));
					restart_count = skip_count = 0;
				}
				for (i = 0; i < blocks_in_mcu; i++) {
					c = block_component[i];
					addr = (int)(data_buffer[c][((block_y * component_h[c]) * blocks_per_row[c]) + (block_y_ofs[i] * blocks_per_row[c]) + (block_x * component_w[c]) + block_x_ofs[i]].data);
					if (decode_progressive_block(addr, (c == 0) ? LUMINANCE : CHROMINANCE, &old_dc[c]))
						goto exit_error;
				}
				block_x++;
				if (block_x >= block_max_x) {
					block_x = 0;
					block_y++;
				}
			} while (block_y < block_max_y);
			/* Process inter-scan chunks */
			while (1) {
				while ((data = _jpeg_getc()) == 0xff)
					;
				if (data == CHUNK_SOS) {
					if (read_sos_chunk()) {
						jpgalleg_error = JPG_ERROR_BAD_IMAGE;
						goto exit_error;
					}
					break;
				}
				else if (data == CHUNK_DHT) {
					if (read_dht_chunk()) {
						jpgalleg_error = JPG_ERROR_BAD_IMAGE;
						goto exit_error;
					}
				}
				else if (data == CHUNK_DRI) {
					if (read_dri_chunk()) {
						jpgalleg_error = JPG_ERROR_BAD_IMAGE;
						goto exit_error;
					}
				}
				else if (data == (CHUNK_EOI & 0xff))
					break;
				else {
					jpgalleg_error = JPG_ERROR_BAD_IMAGE;
					goto exit_error;
				}
			}
			if (data == (CHUNK_EOI & 0xff))
				break;
		}
		/* Apply idct and plot image */
		component_w[0] = (sampling == 1) ? 1 : 2;
		component_h[0] = (sampling != 4) ? 1 : 2;
		mcu_w = (sampling == 1) ? 8 : 16;
		mcu_h = (sampling != 4) ? 8 : 16;
		blocks_in_mcu = sampling;
		for (i = 0; i < sampling; i++) {
			block_component[i] = 0;
			block_x_ofs[i] = x_ofs[i];
			block_y_ofs[i] = y_ofs[i];
		}
		for (i = 1; i < jpeg_components; i++) {
			block_component[blocks_in_mcu] = i;
			block_x_ofs[blocks_in_mcu] = 0;
			block_y_ofs[blocks_in_mcu] = 0;
			blocks_in_mcu++;
		}
		plot = plot_411;
		if (sampling < 4) {
			plot = plot_422;
			cb -= 128;
			cr -= 128;
		}
		if (sampling < 2) {
			plot = plot_444;
			cb -= 64;
			cr -= 64;
		}
		for (block_y = 0; block_y < bmp->h / mcu_h; block_y++) {
			for (block_x = 0; block_x < bmp->w / mcu_w; block_x++) {
				coefs_ptr = coefs_buffer;
				for (i = 0; i < blocks_in_mcu; i++) {
					c = block_component[i];
					temp_ptr = data_buffer[c][(block_y * blocks_per_row[c] * component_h[c]) + (blocks_per_row[c] * block_y_ofs[i]) + (block_x * component_w[c]) + block_x_ofs[i]].data;
					zigzag_reorder(temp_ptr, coefs);
					apply_idct(coefs, coefs_ptr, (c == 0) ? luminance_quantization_table : chrominance_quantization_table, workspace);
					coefs_ptr += 64;
				}
				addr = (int)bmp->line[block_y * mcu_h] + (block_x * mcu_w * 4);
				plot(addr, pitch, y1, y2, y3, y4, cb, cr);
			}
		}
	}

	temp = create_bitmap_ex(32, jpeg_w, jpeg_h);
	if (!temp)
		goto exit_error;
	blit(bmp, temp, 0, 0, 0, 0, jpeg_w, jpeg_h);
	destroy_bitmap(bmp);
	bmp = temp;

   	generate_332_palette(pal);
	depth = _color_load_depth(32, FALSE);
	if (depth != 32)
		bmp = _fixup_loaded_bitmap(bmp, want_palette ? pal : NULL, depth);
	
exit_ok:
	for (i = 0; i < jpeg_components; i++) {
		if (data_buffer[i])
			free(data_buffer[i]);
	}
	for (i = 0; i < 4; i++) {
		free_huffman_table(&huffman_dc_table[i]);
		free_huffman_table(&huffman_ac_table[i]);
	}
	return bmp;

exit_error:
	destroy_bitmap(bmp);
	bmp = NULL;
	goto exit_ok;
}
