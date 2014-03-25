/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *	Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 *	tga.c:   Handle Targa file format.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was
 *	originally written by id Software, Inc.
 *
 *	Portion of this code was derived from The GIMP -- an image manipulation
 *	program, and was originally written by Spencer Kimball and Peter Mattis.
 */

#include <string.h>
#include <stdio.h>

#include "../../../common/arch.h"
#include "../memory/memory.h"
#include "../../../common/common_utils.h"

#include "tga.h" /* prototypes should be here */


/*
-----------------------------------------------------------------------------
 Function: rle_write -Run length encode scanline.

 Parameters: fp -[in] Pointer to valid FILE structure.
			 buffer -[in] Scanline data.
		     width -[in] Image scanline width.
			 bytes -[in] Bytes per pixel.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void rle_write(FILE *fp, W8 *buffer, W32 width, W32 bytes)
{
	SW32    repeat = 0;
	SW32    direct = 0;
	W8	 *from   = buffer;
	W32    x;

	for ((x = 1); (x < width); ++x) {
		if (memcmp(buffer, (buffer + bytes), bytes)) {
			/* next pixel is different */
			if (repeat) {
				putc((int)(128 + repeat), fp);
				fwrite(from, bytes, (size_t)1, fp);
				from = (buffer + bytes); /* point to first different pixel */
				repeat = 0;
				direct = 0;
			} else {
				direct += 1;
			}
		} else {
			/* next pixel is the same */
			if (direct) {
				putc((int)(direct - 1), fp);
				fwrite(from, bytes, (size_t)direct, fp);
				from = buffer; /* point to first identical pixel */
				direct = 0;
				repeat = 1;
			} else {
				repeat += 1;
			}
		}

		if (repeat == 128) {
			putc(255, fp);
			fwrite(from, bytes, (size_t)1, fp);
			from = (buffer + bytes);
			direct = 0;
			repeat = 0;
		} else if (direct == 128) {
			putc(127, fp);
			fwrite(from, bytes, (size_t)direct, fp);
			from = (buffer + bytes);
			direct = 0;
			repeat = 0;
		}

		buffer += bytes;
    }

	if (repeat > 0) {
		putc((int)(128 + repeat), fp);
		fwrite(from, bytes, (size_t)1, fp);
	} else {
		putc((int)direct, fp);
		fwrite(from, bytes, (size_t)(direct + 1), fp);
	}
}


/*
-----------------------------------------------------------------------------
 Function: WriteTGA -Write targa image file.

 Parameters: filename -[in] Name of TGA file to save as.
			 depth -[in] Bytes per pixel. (16, 24 or 32).
		     width -[in] Width of image.
			 height -[in] Height of image.
			 Data -[in] Raw image data.
			 upsideDown -[in] Is the data upside down? 1 yes, 0 no.
			 rle -[in] Run Length encode? 1 yes, 0 no.

 Returns: 0 on error, otherwise 1.

 Notes:
-----------------------------------------------------------------------------
*/
/* TODO: put this function in a shared library */
#ifndef WriteTGA /* this if(n)def is bad */
/* prototype has moved to "tga.h" */
PUBLIC W8 WriteTGA(const char *filename, W16 bpp, W16 width, W16 height,
				   void *Data, W8 upsideDown, W8 rle)
#else /* WriteTGA already defined: */
/* as above with the previous condition, the prototype has moved to "tga.h" */
PUBLIC W8 wolfextractor_WriteTGA(const char *filename, W16 bpp, W16 width,
								 W16 height, void *Data, W8 upsideDown, W8 rle)
#endif /* !WriteTGA */
{
    W16	i, x, y, BytesPerPixel;
	W8	*scanline;
	W8 header[18];
	FILE *filestream;
	W8 *ptr;
	W8 temp;

	ptr = (PW8)Data;

	BytesPerPixel = (bpp >> 3);

	filestream = fopen(filename, "wb");
    if (filestream == NULL) {
		printf("Could not open file (%s) for write!\n", filename);
		return 0;
	}

	memset(header, 0, (size_t)18);
    header[2] = (rle ? 10 : 2);

    header[12] = (width & 255);	/* width low */
    header[13] = (width >> 8);	/* width high */

    header[14] = (height & 255); /* height low */
    header[15] = (height >> 8);	/* height high */

    header[16] = (bpp & 255);	/* pixel size */

    if (upsideDown) {
		header[17] |= (1 << 5); /* Image Descriptor */
    }


	fwrite(header, sizeof(W8), sizeof(header), filestream);


	scanline = (PW8)MM_MALLOC((size_t)(width * BytesPerPixel));
    if (scanline == NULL) {
		fclose(filestream);
		return 0;
	}

	for ((y = 0); (y < height); ++y) {
		W32 k = 0;

		for ((i = 0); (i < (width * BytesPerPixel)); ++i ) {
			scanline[k++] = ptr[((height - y - 1) * width * BytesPerPixel + i)];
		}


		if ((bpp == 24) || (bpp == 32)) {
			/* swap rgb to bgr */
			for ((x = 0); (x < (width * BytesPerPixel)); (x += BytesPerPixel)) {
				temp = scanline[x];
				scanline[x] = scanline[(x + 2)];
				scanline[(x + 2)] = temp;
			}
		}

		if (rle) {
			rle_write(filestream, scanline, (W32)width, (W32)BytesPerPixel);
		} else {
			fwrite(scanline, sizeof(W8), (size_t)(width * BytesPerPixel),
				   filestream);
		}
	}

    MM_FREE(scanline);

	if (filestream == NULL) {
		printf("WriteTGA(): file (%s) somehow became null!\n", filename);
		return 0;
	}

	if (! filestream) {
		printf("WriteTGA(): something bad happened to the filestream (%s) here!\n",
			   filename);
		return 0;
	}

    fclose(filestream);

	return 1;
}

/* EOF */
