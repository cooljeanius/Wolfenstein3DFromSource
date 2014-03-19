/*
 *  WriteTGA_minimal.c
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/19/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source
 *
 */

/*
 * WriteTGA_minimal.c: a somewhat more minimalistic re-implementation of the
 * WriteTGA() function from either "../../loaders/tga.c" or
 * "../wolfextractor/loaders/tga.c", to avoid having to drag in all the other
 * files that either of them depend upon.
 * TODO: get rid of this file once I have a common shared library (that is
 * shared between all the targets) that includes this function...
 */

#include <string.h> /* for memset() */
#include <stdio.h>  /* for fopen(), printf(), fwrite(), and fclose() */
#include <stdlib.h> /* for malloc() and free() */

#include "WriteTGA_minimal.h"

unsigned char WriteTGA(const char *filename, unsigned short bpp,
					   unsigned short width, unsigned short height,
					   void *Data, unsigned char upsideDown, unsigned char rle)
{
    unsigned short	i, x, y, BytesPerPixel;
	unsigned char	*scanline;
	unsigned char header[18];
	FILE *filestream;
	unsigned char *ptr;
	unsigned char temp;

	ptr = (unsigned char*)Data;

	BytesPerPixel = (bpp >> 3);

	filestream = fopen(filename, "wb");
    if (filestream == NULL) {
		printf("Could not open file (%s) for write!\n", filename);
		return 0;
	}

	memset(header, (int)(0), (size_t)(18));
    header[2] = (rle ? 10 : 2);

    header[12] = (width & 255);	/* width low */
    header[13] = (width >> 8);	/* width high */

    header[14] = (height & 255); /* height low */
    header[15] = (height >> 8);	/* height high */

    header[16] = (bpp & 255);	/* pixel size */

    if (upsideDown) {
		header[17] |= (1 << 5); /* Image Descriptor */
    }

	fwrite(header, sizeof(unsigned char), sizeof(header), filestream);

	scanline = (unsigned char*)malloc((size_t)(width * BytesPerPixel));
    if (scanline == NULL) {
		fclose(filestream);

		return 0;
	}

	for ((y = 0); (y < height); ++y) {
		unsigned long k = 0;

		for ((i = 0); (i < (width * BytesPerPixel)); ++i) {
			scanline[k++] = ptr[((height - y - 1) * width * BytesPerPixel + i)];
		}

		if ((bpp == 24) || (bpp == 32)) {
			/* swap rgb to bgr */
			for ((x = 0); (x < (width * BytesPerPixel)); x += BytesPerPixel) {
				temp = scanline[x];
				scanline[x] = scanline[(x + 2)];
				scanline[(x + 2)] = temp;
			}
		}

		if (rle) {
			printf("skipping call to rle_write() for this version of WriteTGA()\n");
		} else {
			fwrite(scanline, sizeof(unsigned char),
				   (size_t)(width * BytesPerPixel), filestream);
		}
	}

    free(scanline);

    fclose(filestream);

	return 1;
}

/* EOF */
