/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * ogg.c - Code for using Ogg Vorbis streams        / / \  \
 *         with DUMB.                              | <  /   \_
 *                                                 |  \/ /\   /
 * Written by Sven_                                 \_  /  > /
 *                                                    | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <dumb.h>
#include "dumbogg.h"



#define SIGTYPE_OGG DUMB_ID('O','G','G',' ')



typedef struct OGG_SIGDATA OGG_SIGDATA;
typedef struct OGG_SIGRENDERER OGG_SIGRENDERER;



#define OGG_BUFFER_SIZE 1024



struct OGG_SIGRENDERER
{
	sample_t **buffer;
	DUMB_RESAMPLER *resampler;

	OGG_SIGDATA *sigdata;
	OggVorbis_File stream;
	int bitstream, n_channels, stream_channels, loop;
};



struct OGG_SIGDATA
{
	char *filename;
	long length, average_bitrate;
	int channels, frequency, loop;
};



static long ogg_sigrenderer_get_samples(sigrenderer_t *sigrenderer, float volume,
                                        float delta, long size, sample_t **samples);
static void ogg_sigrenderer_get_current_sample(sigrenderer_t *sigrenderer, float volume, sample_t *samples);
static sigrenderer_t *ogg_start_sigrenderer(DUH *duh, sigdata_t *sigdata, int n_channels, long pos);
static void ogg_end_sigrenderer(sigrenderer_t *sigrenderer);
static void ogg_unload_sigdata(sigdata_t *sigdata);



static DUH_SIGTYPE_DESC sigtype_ogg = {
	SIGTYPE_OGG,
	NULL,				/* entheh says I don't need you */
	&ogg_start_sigrenderer,
	NULL,				/* don't need this either */
	&ogg_sigrenderer_get_samples,
	&ogg_sigrenderer_get_current_sample,
	&ogg_end_sigrenderer,
	&ogg_unload_sigdata
};




/* dumb_load_ogg(): loads an OGG file into a DUH struct, returning a pointer
 * to the DUH struct. When you have finished with it, you must pass the
 * pointer to unload_duh() so that the memory can be freed.
 * Note that I don't use DUMBFILES .. they're dumb ;)
 */
DUH *dumb_load_ogg(const char *filename, int loop)
{
	int n;
	FILE *file;
	sigdata_t *sigdata;
	double time;
	OggVorbis_File ov;
	DUH_SIGTYPE_DESC *descptr = &sigtype_ogg;


#if 0
	if(!exists(filename))
		return NULL;
#endif


	/* open the file */
	file = fopen(filename,"rb");
	if (!file) return NULL;


	/* let oggvorbis take over the file */
	n = ov_open(file, &ov, NULL,0);
	if(n < 0)
	{
		fclose(file);
		return NULL;
	}


	/* allocate the sigdata */
	sigdata = malloc(sizeof(OGG_SIGDATA));
	if (!sigdata) {
		ov_clear(&ov);
	}
	((OGG_SIGDATA*)sigdata)->filename = malloc(sizeof(*((OGG_SIGDATA*)sigdata)->filename) * (strlen(filename) + 1));
	strcpy(((OGG_SIGDATA*)sigdata)->filename,filename);


	/* Store information about it for retrieval later */
	time = ov_time_total(&ov,-1);
	if(time == OV_EINVAL)
	{
		ov_clear(&ov);
		free(sigdata);
		return NULL;
	}

	{
		vorbis_info *vi = ov_info(&ov, -1);
		((OGG_SIGDATA*)sigdata)->length = ov_pcm_total(&ov, -1);
		((OGG_SIGDATA*)sigdata)->average_bitrate = ov_bitrate(&ov,-1);
		((OGG_SIGDATA*)sigdata)->channels = vi->channels;
		((OGG_SIGDATA*)sigdata)->frequency = vi->rate;
		((OGG_SIGDATA*)sigdata)->loop = loop;
	}


	/* close the OggVorbis_File stream */
	ov_clear(&ov);


	if (((OGG_SIGDATA*)sigdata)->length <= 0) {
		free(sigdata);
		return NULL;
	}


	return make_duh(((OGG_SIGDATA*)sigdata)->length, 1, &descptr, &sigdata);
}




static long ogg_fill_buffer(OGG_SIGRENDERER *sigrenderer)
{
	long total = 0;

	while (total < OGG_BUFFER_SIZE) {
		float **pcm;
		long n = ov_read_float(&sigrenderer->stream, &pcm, OGG_BUFFER_SIZE - total, NULL);
		int j;
		long i;

		if (n <= 0) {
			if (sigrenderer->loop)
				ov_pcm_seek(&sigrenderer->stream, 0);
			else
				return total;
		}

		if (sigrenderer->n_channels == sigrenderer->stream.vi->channels) {
			for (j = 0; j < sigrenderer->n_channels; j++)
				for (i = 0; i < n; i++)
					sigrenderer->buffer[j][total+i] = (sample_t)floor(8388608.0f * pcm[j][i] + 0.5f);
		} else {
			float mul = 8388608.0f / sigrenderer->stream.vi->channels;
			for (i = 0; i < n; i++)
				sigrenderer->buffer[0][total+i] = (sample_t)floor(mul * pcm[0][i] + 0.5f);
			for (j = 1; j < sigrenderer->stream.vi->channels; j++)
				for (i = 0; i < n; i++)
					sigrenderer->buffer[0][total+i] += (sample_t)floor(mul * pcm[j][i] + 0.5f);
		}

		total += n;
	}

	return total;
}



/* ogg_render_signal:
 * Rewritten and works accurately now :)
 */
static long ogg_sigrenderer_get_samples(sigrenderer_t *sigrenderer, float volume,
                                        float delta, long size, sample_t **samples)
{
	OGG_SIGRENDERER *ogg_sr = sigrenderer;

	long total_samples_read = 0;
	OggVorbis_File *ov = &ogg_sr->stream;

	delta *= ogg_sr->sigdata->frequency * (1.0f / 65536.0f);

	while(total_samples_read < size)
	{
		long n = 0;

		if (ov->vi->channels == ogg_sr->n_channels) {
			int i;
			for (i = 0; i < ogg_sr->n_channels; i++)
				n = dumb_resample(&ogg_sr->resampler[i], samples[i] + total_samples_read, size - total_samples_read, volume, delta);
		} else {
			int i;
			DUMB_RESAMPLER initial = ogg_sr->resampler[0];
			for (i = 0; i < ogg_sr->n_channels; i++) {
				ogg_sr->resampler[0] = initial;
				n = dumb_resample(&ogg_sr->resampler[0], samples[i] + total_samples_read, size - total_samples_read, volume, delta);
			}
		}

		total_samples_read += n;

		if (total_samples_read < size) {
			long end = ogg_fill_buffer(ogg_sr);
			int n_channels = ogg_sr->n_channels == ogg_sr->stream.vi->channels ? ogg_sr->n_channels : 1;
			int i;
			for (i = 0; i < n_channels; i++) {
				ogg_sr->resampler[i].pos -= OGG_BUFFER_SIZE;
				ogg_sr->resampler[i].end = end;
				ogg_sr->resampler[i].dir = 1;
			}
			if (end <= 0) break;
		}
	}

	return total_samples_read;
}



static void ogg_sigrenderer_get_current_sample(sigrenderer_t *sigrenderer, float volume, sample_t *samples)
{
	OGG_SIGRENDERER *ogg_sr = sigrenderer;

	OggVorbis_File *ov = &ogg_sr->stream;

	// This may fail if the resampler has overshot or hasn't started properly!
	if (ov->vi->channels == ogg_sr->n_channels) {
		int i;
		for (i = 0; i < ogg_sr->n_channels; i++)
			samples[i] += dumb_resample_get_current_sample(&ogg_sr->resampler[i], volume);
	} else {
		sample_t sample = dumb_resample_get_current_sample(&ogg_sr->resampler[0], volume);
		int i;
		for (i = 0; i < ogg_sr->n_channels; i++)
			samples[i] += sample;
	}

#if 0 // We may need to use this!
		total_samples_read += n;

		if (total_samples_read < size) {
			long end = ogg_fill_buffer(ogg_sr);
			int n_channels = ogg_sr->n_channels == ogg_sr->stream.vi->channels ? ogg_sr->n_channels : 1;
			int i;
			for (i = 0; i < n_channels; i++) {
				ogg_sr->resampler[i].pos -= OGG_BUFFER_SIZE;
				ogg_sr->resampler[i].end = end;
				ogg_sr->resampler[i].dir = 1;
			}
			if (end <= 0) break;
		}
#endif
}



/* ogg_start_sigrenderer:
 *
 */
static sigrenderer_t *ogg_start_sigrenderer(DUH *duh, sigdata_t *sigdata, int n_channels, long pos)
{
	FILE *file;
	OGG_SIGRENDERER *sigrenderer;

	(void)duh;

	/* Open the file pointer */
	file = fopen( ((OGG_SIGDATA*)sigdata)->filename, "rb");

	if (!file)
		return NULL;

	/* Allocate the sample info */
	sigrenderer = malloc(sizeof(OGG_SIGRENDERER));
	if(!sigrenderer) {
		fclose(file);
		return NULL;
	}


	/* Open the stream */
	if(ov_open(file, &sigrenderer->stream, NULL, 0) < 0)
	{
		free(sigrenderer);
		fclose(file);
		return NULL;
	}


	/* Save some information */
	sigrenderer->n_channels = n_channels;
	sigrenderer->loop = ((OGG_SIGDATA*)sigdata)->loop;


	{
		int n = sigrenderer->n_channels == sigrenderer->stream.vi->channels ? sigrenderer->n_channels : 1;

		sigrenderer->buffer = create_sample_buffer(n, OGG_BUFFER_SIZE);
		if (!sigrenderer->buffer) {
			sigrenderer->resampler = NULL;
			ogg_end_sigrenderer(sigrenderer);
			return NULL;
		}

		sigrenderer->resampler = malloc(n * sizeof(DUMB_RESAMPLER));
		if (!sigrenderer->resampler) {
			ogg_end_sigrenderer(sigrenderer);
			return NULL;
		}


		/* Seek to the position the user requested */
		ov_pcm_seek(&sigrenderer->stream, (long)((LONG_LONG)pos * ((OGG_SIGDATA*)sigdata)->frequency >> 16));


		long end = ogg_fill_buffer(sigrenderer);
		int i;
		for (i = 0; i < n; i++)
			dumb_reset_resampler(&sigrenderer->resampler[i], sigrenderer->buffer[i], 0, 0, end);
	}


	sigrenderer->sigdata = sigdata;


	return sigrenderer;
}





/* ogg_end_sigrenderer:
 * closes the vorbis file and frees memory
 */
static void ogg_end_sigrenderer(sigrenderer_t *sigrenderer)
{
	if(sigrenderer)
	{
		/* close the sample stream */
		ov_clear( &((OGG_SIGRENDERER*)sigrenderer)->stream );

		destroy_sample_buffer(((OGG_SIGRENDERER*)sigrenderer)->buffer);

		if (((OGG_SIGRENDERER*)sigrenderer)->resampler)
			free(((OGG_SIGRENDERER*)sigrenderer)->resampler);

		free(sigrenderer);
	}

	return;
}



/* ogg_unload_sigdata:
 * Frees memory allocated for the duh sigdata
 */
static void ogg_unload_sigdata(sigdata_t *sigdata)
{
	if(sigdata)
	{
		if( ((OGG_SIGDATA*)sigdata)->filename)
			free( ((OGG_SIGDATA*)sigdata)->filename);

		free(sigdata);
	}

	return;
}
