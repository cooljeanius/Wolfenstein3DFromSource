/*
 *  vorbisenc_inter.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/17/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

/*
 * vorbisenc_inter.h: Ogg Vorbis interface for wolfextractor
 */

/*
 * 	Notes:
 * 	This module is implemented by vorbisenc_inter.c
 */

#ifndef __VORBISENC_INTER_H__
#define __VORBISENC_INTER_H__

#include "../../common/arch.h" /* for types */

#define READSIZE 1024

/* prototypes */
extern long read_samples(float **buffer, int samples);
extern int vorbis_encode(const char *filename, void *data, W32 size,
						 W32 in_channels, W32 in_samplesize, W32 rate,
						 W32 quality, W32 max_bitrate, W32 min_bitrate);

#endif /* __VORBISENC_INTER_H__ */

/* EOF */
