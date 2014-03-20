/*
 *	Copyright (C) 2004 Michael Liebscher
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
 *	wolf_gfx.c:   Decode Wolfenstein3-D GFX data.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Wolfenstein3-D, and was originally
 *	written by Id Software, Inc.
 *
 */

#ifdef HAVE_CONFIG_H
# include "../../../config.h"
#else
# define WOLF_GFX_C_NON_AUTOTOOLS_BUILD 1
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif /* HAVE_LIMITS_H */
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */
#ifdef HAVE_SYS_SYSCTL_H
# include <sys/sysctl.h>
#endif /* HAVE_SYS_SYSCTL_H */

#ifdef __WOLF_DEF_H__
# warning "wolf_def.h seems to have already been included once."
#endif /* __WOLF_DEF_H__ */
#include "wolf_def.h"
#ifndef __WOLF_DEF_H__
# warning "wolf_def.h was not actually included."
#endif /* !__WOLF_DEF_H__ */

#include "../loaders/tga.h"
#include "../hq2x.h"
#include "../string/com_string.h"
#include "../filesys/file.h"
#include "../../../common/common_utils.h"
#include "../../../common/arch.h"
#include "../memory/memory.h"


extern char gamepal[];

#define FILEPOSSIZE	3


#define GFXHEADFNAME    "VGAHEAD"
#define GFXFNAME        "VGAGRAPH"
#define GFXDICTFNAME    "VGADICT"


#define FONTWIDTH	256
#define FONTHEIGHT	128


typedef struct
{
	short height;
	short location[FONTWIDTH]; /* 256 */
	char width[FONTWIDTH]; /* 256 */

} fontstruct;


typedef struct
{
  /* 0-255 is a character, > is a pointer to a node */
  W16 bit0, bit1;

} huffnode;

typedef struct
{
	W16 width, height;

} pictabletype;

pictabletype *pictable;
huffnode	grhuffman[255];


FILE *grhandle; /* TODO: get actual filename of this file */
const char *grnameofhandle; /* (maybe use this variable to store it to?) */
SW32 *grstarts;	/* array of offsets in vgagraph, -1 for sparse */

void *grsegs[NUMCHUNKS]; /* NUMCHUNKS is defined to '169' in "wolf_def.h" */



/*
-----------------------------------------------------------------------------
 Function: GRFILEPOS() -Calculate graphic file position.

 Parameters: chunk -[in] Chunk number to calculate file offset.

 Returns: File offset value or -1 for sparse tile.

 Notes: See 'wl6_name.h' for valid chunk numbers for SOD and WL6.
        grstarts must be allocated and initialized before call.
        Uses global variable grstarts.

-----------------------------------------------------------------------------
*/
PRIVATE SW32 GRFILEPOS(W32 c)
{
	SW32 value;
	W32 offset;

	offset = (c * 3);

#ifdef ALIGNOF_UNSIGNED_LONG
# if (ALIGNOF_UNSIGNED_LONG == 0) || (ALIGNOF_UNSIGNED_LONG != 8)
	printf("GRFILEPOS(): 'value' and/or 'offset' might be misaligned; offsetof 'unsigned long' is '%i'...\n",
		   (int)(ALIGNOF_UNSIGNED_LONG));
# endif /* (ALIGNOF_UNSIGNED_LONG == 0) || (ALIGNOF_UNSIGNED_LONG != 8) */
#endif /* ALIGNOF_UNSIGNED_LONG */
	/* one of clang's sanitizers prints a warning here when running this: */
	value = (SW32)*(PW32)(((PW8)grstarts) + offset);

	value &= 0x00ffffffl;

	if (value == 0xffffffl) {
		value = -1;
    }

	return value;
}/*;*/


/*
-----------------------------------------------------------------------------
 Function: CAL_GetGrChunkLength() -Calculate length of compressed graphic
                                   chunk.

 Parameters: chunk -[in] Chunk number to calculate file offset.

 Returns: The length of the compressed graphic chunk.

 Notes: Gets the length of an explicit length chunk (not tiles).
        The file pointer is positioned so the compressed data
        can be read in next.

-----------------------------------------------------------------------------
*/
PRIVATE SW32 CAL_GetGrChunkLength(W32 chunk)
{
	long offset;
	SW32 retval;

	printf("CAL_GetGrChunkLength(): inspecting chunk '%lu'...\n", chunk);
	offset = (long)((unsigned long)GRFILEPOS(chunk) + sizeof(W32));

	if (grnameofhandle == NULL) {
		printf("CAL_GetGrChunkLength(): seeking ahead '%li' bytes in 'grhandle'...\n",
			   offset);
	} else {
		printf("CAL_GetGrChunkLength(): seeking ahead '%li' bytes in '%s'\n",
			   offset, grnameofhandle);
	}
	fseek(grhandle, offset, SEEK_SET);

	retval = (GRFILEPOS(chunk + 1) - GRFILEPOS(chunk) - 4);
	printf("CAL_GetGrChunkLength(): returning '%lu'\n", retval);
    return retval;
}

/*
-----------------------------------------------------------------------------
 Function: CAL_HuffExpand() -Expand compressed data.

 Parameters: source -[in] Pointer to compressed data.
             destination -[out] Pointer to hold decompressed data.
             length -[in] Length of expanded data.
             hufftable -[in] Huffman dictionary data.

 Returns: Nothing.

 Notes: Uses classic Huffman node tree (not optimized).
-----------------------------------------------------------------------------
*/
PRIVATE void CAL_HuffExpand(const W8 *source,
							W8 *destination,
							W32 length,
							huffnode *hufftable)
{
	W32 bx;	/* node pointer */
	W32 dx;

	register W32 ch;	/* high order of CX */
	register W32 cl;	/* low order of CX */

	register const W8 *si; /* Source Index */
	W8 *di; /* Destination Index */

    bx = 254; /* head node is always node 254 */
	si = source;
	di = destination;


    ch = *si; /* load first byte */
    si++;
    cl = 1;

    do {
        if ((ch & cl) & 0xff) { /* bit set? */
            dx = hufftable[bx].bit1;  /* take bit1 path from node */
        } else {
            dx = hufftable[bx].bit0;  /* take bit0 path from node */
		}


        cl <<= 1;   /* advance to next bit position */

        if (cl & 0x100) {
            ch = *si; /* load next byte */
            si++;
            cl = 1; /* back to first bit */
        }

		if ((cl & 0x100) == 0) {
            if (dx < 256) {
                *di = (W8)(dx & 0xff); /* write a decompressed byte out */
                di++;
                bx = 254; /* back to the head node for next bit */

				length--;
            } else {
                bx = (dx - 256);  /* next node = (huffnode *)code */
            }
        }
    } while (length);

}

/*
-----------------------------------------------------------------------------
 Function: CAL_SetupGrFile() -Initialize graphic files and arrays.

 Parameters: extension -[in] Pointer to a null-terminated string that
                            specifies the file extension.
                            (must be in '.XXX' format).

 Returns: 1 on success, 0 otherwise.

 Notes:
        Uses global variables grhandle and pictable.
        1. Open vgadict.XXX, read huffman dictionary data.
        2. Open vgahead.XXX, read data offsets.
        3. Open vgagraph.XXX, read pic and sprite header, expand data.
-----------------------------------------------------------------------------
*/
PRIVATE W8 CAL_SetupGrFile(const char *extension)
{
	void *compseg;
	FILE *handle;
	char filename[16];
	SW32 chunkcomplen; /* chunk compressed length */

/*
 * load vgadict.ext (huffman dictionary for graphics files)
 */
	cs_strlcpy(filename, GFXDICTFNAME, sizeof(filename));
	cs_strlcat(filename, extension, sizeof(filename));

	if ((handle = fopen(cs_strupr(filename), "rb")) == NULL) {
		if ((handle = fopen(cs_strlwr(filename), "rb")) == NULL) {
			printf("Could not open file (%s) for read!\n", filename);
			return 0;
		}
	}

	fread(grhuffman, sizeof(grhuffman), (size_t)1, handle);
	fclose(handle);

/*
 * Open then load the data offsets from vgahead.ext
 */

	cs_strlcpy(filename, GFXHEADFNAME, sizeof(filename));
	cs_strlcat(filename, extension, sizeof(filename));

	if ((handle = fopen(cs_strupr(filename), "rb")) == NULL) {
		if ((handle = fopen(cs_strlwr(filename), "rb")) == NULL) {
			printf("Could not open file (%s) for read!\n", filename);
			return 0;
		}
	}

	grstarts = MM_MALLOC((size_t)((NUMCHUNKS + 1) * FILEPOSSIZE));
	if (grstarts == NULL) {
		return 0;
	}

	fread(grstarts, sizeof(long), (size_t)((NUMCHUNKS + 1) * FILEPOSSIZE),
		  handle);

	fclose(handle);

/*
 * Open the graphics file 'vgagraph.XXX'.
 */

	cs_strlcpy(filename, GFXFNAME, sizeof(filename));
	cs_strlcat(filename, extension, sizeof(filename));

	printf("CAL_SetupGrFile(): trying to open file '%s' for use by global filehandle 'grhandle'...\n",
		   filename);
	if ((grhandle = fopen(cs_strupr(filename), "rb")) == NULL) {
		if ((grhandle = fopen(cs_strlwr(filename), "rb")) == NULL) {
			printf("Could not open file (%s) for read!\n", filename);
			return 0;
		} else {
			grnameofhandle = (const char*)filename;
		}
	} else {
		grnameofhandle = (const char*)filename;
	}

/*
 * load the pic and sprite headers into the arrays.
 */
	pictable = MM_MALLOC(NUMPICS * sizeof(pictabletype));
	if (pictable == NULL) {
		return 0;
	}


	chunkcomplen = CAL_GetGrChunkLength(STRUCTPIC); /* position file pointer */

	compseg = MM_MALLOC((size_t)chunkcomplen);
	if (compseg == NULL) {
		printf("CAL_SetupGrFile(): 'compseg' was null, returning...\n");
		return 0;
	}

	if (grnameofhandle == NULL) {
		printf("CAL_SetupGrFile(): Reading 1 object '%lu' bytes long from 'grhandle'...\n",
			   chunkcomplen);
	} else {
		printf("CAL_SetupGrFile(): Reading 1 object '%lu' bytes long from '%s'...\n",
			   chunkcomplen, grnameofhandle);
	}
	fread(compseg, (size_t)chunkcomplen, (size_t)1, grhandle);

	CAL_HuffExpand(compseg, (PW8)pictable,
				   (NUMPICS * sizeof(pictabletype)),
				   grhuffman);

	MM_FREE(compseg);

    return 1;
}

/*
-----------------------------------------------------------------------------
 Function: CAL_Shutdown() -Shutdown module.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Frees grstarts, pictable and grsegs data. Closes grhandle file handle.
-----------------------------------------------------------------------------
*/
PUBLIC void CAL_Shutdown(void)
{
    W32 i;

    if (grstarts) {
		printf("CAL_Shutdown(): freeing 'grstarts'...\n");
        MM_FREE(grstarts);
    }

    if (pictable) {
		printf("CAL_Shutdown(): freeing 'pictable'...\n");
        MM_FREE(pictable);
    }

    if (grhandle) {
		if (grnameofhandle == NULL) {
			printf("CAL_Shutdown(): closing 'grhandle'...\n");
		} else {
			printf("CAL_Shutdown(): closing file '%s'...\n", grnameofhandle);
		}
        fclose(grhandle);
	}

    for ((i = 0); (i < NUMCHUNKS); ++i) {
        if (grsegs[i]) {
			printf("CAL_Shutdown(): freeing 'grsegs[%lu]'...\n", i);
            MM_FREE(grsegs[i]);
        }
    }

}

/*
-----------------------------------------------------------------------------
 Function: CAL_ExpandGrChunk() -Expand compressed graphic chunk.

 Parameters: chunk -[in] Chunk number to expand.
             source -[in] Pointer to compressed data.
             version -[in] extension version.
                        1 -WL6
                        2 -SOD

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void CAL_ExpandGrChunk(W32 chunk, const W8 *source, W16 version)
{
	W32 expanded; /* size (?) */
	W16	 offset;

	expanded = 0; /* initialize separately */
	offset = 0; /* initialize separately */

	/* dummy condition to use value stored to 'expanded': */
	if (expanded == 0) {
		;
	}

	if (version & SOD_PAK) {
        offset = SOD_STARTDIFF;
	}

	printf("CAL_ExpandGrChunk(): seeing if number of chunk (%lu) is between '%i' and '%i'...\n",
		   chunk, (STARTTILE8 + offset), (STARTEXTERNS + offset));
	/* STARTTILE8 is defined to '135' in "wolf_def.h" */
	/* STARTEXTERNS is defined to '136' in "wolf_def.h" */
	if ((chunk >= (STARTTILE8 + offset)) &&
        (chunk < (STARTEXTERNS + offset))) {
	/*
	 * expanded sizes of tile8/16/32 are implicit
	 */

#define BLOCK		64
#define MASKBLOCK	128

		if (chunk < (STARTTILE8M + offset)) { /* tile 8s are all in 1 chunk! */
			/* NUMTILE8 is defined to '72' in "wolf_def.h": */
			expanded = (BLOCK * NUMTILE8); /* (64 * 72) = 4608 */
		} else if (chunk < (STARTTILE16 + offset)) {
			/* NUMTILE8M is defined to '0' in "wolf_def.h": */
			expanded = (MASKBLOCK * NUMTILE8M); /* (128 * 0) = 0 */
		} else if (chunk < (STARTTILE16M + offset)) {
			/* all other tiles are 1/chunk */
			expanded = (BLOCK << 2); /* (64 << 2) = 256 */
		} else if (chunk < (STARTTILE32 + offset)) {
			expanded = (MASKBLOCK << 2); /* (128 << 2) = 512 */
		} else if (chunk < (STARTTILE32M + offset)) {
			expanded = (BLOCK << 4); /* (64 << 4) = 1024 */
		} else {
			expanded = (MASKBLOCK << 4); /* (128 << 4) = 2048 */
		}
	} else {
	/*
	 * everything else has an explicit size longword
	 */
		/* TODO: do a better job of checking the value of 'source' before using
		 * it; this check against 'NULL' seems ineffective: */
		if (source != NULL) {
			printf("CAL_ExpandGrChunk(): setting 'expanded' to 'source', which is '%s' \n",
				   source);
			expanded = *(PW32)source;
			source += 4; /* skip over length */
		} else {
			printf("CAL_ExpandGrChunk(): 'source' is null, cannot set 'expanded' to it!\n");
			return;
		}
	}

/*
 * allocate final space and decompress it.
 * Sprites need to have shifts made and various other junk.
 */
	/* TODO: check 'expanded' to make sure that it has not overflowed: */
	printf("CAL_ExpandGrChunk(): expanded size is: %lu \n", expanded);
	grsegs[chunk] = MM_MALLOC((size_t)expanded);
	if (grsegs[chunk] == NULL) {
		printf("warning: grsegs[%lu] ended up null, returning...\n", chunk);
		return;
	}

	CAL_HuffExpand(source, grsegs[chunk], expanded, grhuffman);
}

/*
-----------------------------------------------------------------------------
 Function: CA_CacheGrChunk() -Cache graphic chunk.

 Parameters: chunk -[in] Chunk number to cache.
             version -[in] extension version.
                        1 -WL6
                        2 -SOD

 Returns: Nothing.

 Notes: Makes sure a given chunk is in memory, loading it if needed.
-----------------------------------------------------------------------------
*/
W16 previous_version = 0;

PRIVATE void CA_CacheGrChunk(W32 chunk, W16 version)
{
	SW32	pos;
	W32		compressed;
	void	*buffer;
	W32		next;

	printf("CA_CacheGrChunk(): inspecting chunk %lu...\n", chunk);
	if (grsegs[chunk]) {
		printf("CA_CacheGrChunk(): chunk %lu is already in grsegs[]\n", chunk);
		return;	/* should already be in memory */
	}


/*
 * load the chunk into a buffer
 */
	pos = GRFILEPOS(chunk);
	if (pos < 0) { /* $FFFFFFFF start is a sparse tile */
		return;
	}

	next = (chunk + 1);
	while (GRFILEPOS(next) == -1) { /* skip past any sparse tiles */
		printf(".");
		next++;
	}

	printf("CA_CacheGrChunk(): value of 'next' is now '%lu'\n", next);
	compressed = (W32)(GRFILEPOS(next) - pos);

	/* 'grhandle' is a file pointer declared globally up top; it should have
	 * been initialized by the CAL_SetupGRFile() function, which should have
	 * been invoked before this function */
	if (grhandle) {
		if (grnameofhandle == NULL) {
			printf("CA_CacheGrChunk(): position in file 'grhandle' is '%li'\n",
				   ftell(grhandle));
		} else {
			printf("CA_CacheGrChunk(): position in file '%s' is '%li'\n",
				   grnameofhandle, ftell(grhandle));
			/* FIXME: filename is truncated? */
		}
		fseek(grhandle, pos, SEEK_SET); /* not sure which SEEK_SET it uses? */
		if (grnameofhandle == NULL) {
			printf("CA_CacheGrChunk(): position in file 'grhandle' is '%li'\n",
				   ftell(grhandle));
		} else {
			printf("CA_CacheGrChunk(): position in file '%s' is '%li'\n",
				   grnameofhandle, ftell(grhandle));
			/* FIXME: filename is truncated? */
		}
	} else {
		printf("CA_CacheGrChunk: no grhandle!\n");
		return;
	}

	buffer = MM_MALLOC((size_t)compressed);
	if (buffer == NULL) {
		printf("CA_CacheGrChunk(): buffer is null, returning...\n");
		return;
	}

	if (grhandle) {
		if (grnameofhandle == NULL) {
			printf("CA_CacheGrChunk(): reading '%lu' items from 'grhandle'\n",
				   (size_t)compressed);
		} else {
			printf("CA_CacheGrChunk(): reading '%lu' items from '%s'\n",
				   (size_t)compressed, grnameofhandle);
			/* FIXME: filename is truncated? */
		}
		/* read the 'grhandle' file and store it to 'buffer': */
		fread(buffer, (size_t)1, (size_t)compressed, grhandle);
		/* TODO: make sure that we do not go past EOF! */
		if (grnameofhandle == NULL) {
			printf("CA_CacheGrChunk(): position in file 'grhandle' is '%li'\n",
				   ftell(grhandle));
		} else {
			printf("CA_CacheGrChunk(): position in file '%s' is '%li'\n",
				   grnameofhandle, ftell(grhandle));
			/* FIXME: filename is truncated? */
		}
	} else {
		printf("CA_CacheGrChunk: no grhandle!\n");
		return;
	}
	if (buffer == NULL) {
		printf("CA_CacheGrChunk(): buffer is null, returning...\n");
		return;
	}

#if PARANOID
	printf("CA_CacheGrChunk(): passing the following (new) values to CAL_ExpandGrChunk() this time around:\n");
	/* usually is a new value, so print unconditionally: */
	printf(" chunk: %lu \n", chunk);
 /* seeing as I am NOT printing the actual _value_ of the buffer here yet, only
  * print this if we are being pedantic: */
# if __STRICT_ANSI__
	printf(" (buffer) \n");
# endif /* __STRICT_ANSI__ */
	/* only interesting if it has changed, which is rarely: */
	if (version != previous_version) {
		printf(" version: %i \n", version);
		previous_version = version; /* set now for the next time around */
	}
#endif /* PARANOID */
	CAL_ExpandGrChunk(chunk, buffer, version);


	MM_FREE(buffer);
}



/*
-----------------------------------------------------------------------------
 Function: RGB32toRGB24() -Convert image from 32bits to 24bits.

 Parameters: src -[in] Buffer to convert from.
             dest -[in/out] Buffer to convert to.
			 size -[in] Size of src buffer.

 Returns: Nothing.

 Notes: src and dest can point to the same memory block.
-----------------------------------------------------------------------------
*/
PUBLIC void RGB32toRGB24(const W8 *src, W8 *dest, size_t size)
{
	size_t i;
	size_t npixels = size >> 2;

	for ((i = 0); (i < npixels); ++i) {
		dest[((i * 3) + 0)] = src[((i * 4) + 0)];
		dest[((i * 3) + 1)] = src[((i * 4) + 1)];
		dest[((i * 3) + 2)] = src[((i * 4) + 2)];
	}
}



/*
-----------------------------------------------------------------------------
 Function: MergePics() -Merge image inside another image.

 Parameters: src -[in] Source image to copy from.
             dest -[in] Destination image to copy to.
             width -[in] Width of source image.
			 height -[in] Height of source image.
			 bpp -[in] Bytes per pixel of images.
			 totalwidth -[in] -Total width (in pixels) of destination image.
			 x_offset -[in] X offset (in pixels) in destination image.
			 y_offset -[in] Y offset (in pixels) in destination image.


 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void MergePics(const W8 *src, W8 *dest, W16 width, W16 height, W16 bpp,
					   W16 totalwidth,
					   W32 x_offset,
					   W32 y_offset)
{
	W16	x, y;
	W8 *dptr;

	dptr = (dest + (y_offset * totalwidth * bpp) + (x_offset * bpp));
	for ((y = 0); (y < height); ++y ) {
		for ((x = 0); (x < width); ++x ) {
			dptr[0] = src[0];
			dptr[1] = src[1];

			src += bpp;
			dptr += bpp;
		}

		dptr += ((totalwidth * bpp) - (width * bpp));
	}
}

PRIVATE void MergeImages(W8 *src, W16 src_bpp, W16 src_totalwidth,
						 W16 src_region_width, W16 src_region_height,
						 W32 src_x_offset, W32 src_y_offset,
						 W8 *dest, W16 dest_bpp, W16 dest_totalwidth,
						 W16 dest_region_width, W16 dest_region_height,
						 W32 dest_x_offset, W32 dest_y_offset)
{
	W16	x, y;
	W8 *dptr;
	W8 *sptr;

	/* dummy condition to use parameter 'dest_region_height': */
	if (dest_region_width == dest_region_height) {
		;
	}

	dptr = (dest + (dest_y_offset * dest_totalwidth * dest_bpp) +
			(dest_x_offset * dest_bpp));
	sptr = (src + (src_y_offset * src_totalwidth * src_bpp) +
			(src_x_offset * src_bpp));
	for ((y = 0); (y < src_region_height); ++y) {
		for ((x = 0); (x < src_region_width); ++x) {
			dptr[0] = sptr[0];
			dptr[1] = sptr[1];

			sptr += src_bpp;
			dptr += dest_bpp;
		}

		/* additional parentheses as a reminder of the order of operations: */
		dptr += ((dest_totalwidth * dest_bpp) - (dest_region_width * dest_bpp));
		sptr += ((src_totalwidth * src_bpp) - (src_region_width * src_bpp));
	}
}



/**
 * \brief Remove fringe from scaled hq2x images.
 * \param[in,out] data Image data.
 * \param[in] width Width (in pixels) of image data.
 * \param[in] height Height (in pixels) of image image.
 * \return Nothing.
 */
PUBLIC void ReduxAlphaChannel_hq2x(W8 *data, W32 width, W32 height)
{

	W8 *ptr;
	W32 i;

	for( i = 0 ; i < (width * height) ; ++i ) {
		ptr = data + i * 4;


        if( ptr[ 0 ] >= 150 &&
            ptr[ 1 ] <= 60 &&
            ptr[ 2 ] >= 150 ) {
			ptr[ 3 ] = 0x00;
		}

        /* Start (...of...?) */

        else if( ptr[ 0 ] >= 124 &&
            ptr[ 1 ] == 0 &&
            ptr[ 2 ] >= 124 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 60 &&
            ptr[ 2 ] >= 170 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 160 &&
            ptr[ 1 ] <= 100 &&
            ptr[ 2 ] >= 220 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 40 &&
            ptr[ 2 ] >= 150 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 140 &&
            ptr[ 1 ] <= 90 &&
            ptr[ 2 ] >= 200 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 190 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 160 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 200 &&
            ptr[ 1 ] <= 120 &&
            ptr[ 2 ] >= 230 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 150 &&
            ptr[ 1 ] <= 110 &&
            ptr[ 2 ] >= 150 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 120 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 100 &&
            ptr[ 1 ] <= 55 &&
            ptr[ 2 ] >= 100 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 180 &&
            ptr[ 1 ] <= 130 &&
            ptr[ 2 ] >= 180 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 200 &&
            ptr[ 1 ] <= 150 &&
            ptr[ 2 ] >= 200 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 190 &&
            ptr[ 1 ] <= 140 &&
            ptr[ 2 ] >= 190 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 170 &&
            ptr[ 1 ] <= 120 &&
            ptr[ 2 ] >= 170 ) {
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 130 &&
            ptr[ 1 ] <= 80 &&
            ptr[ 2 ] >= 130 ) {
			ptr[ 3 ] = 0x00;
		}

        /* End (...of...?) */

		else {
			ptr[ 3 ] = 0xFF;
		}
	}

}

/*
-----------------------------------------------------------------------------
 Function: SavePic() -Save graphic lump in targa image format.

 Parameters: chunk -[in] Chunk number to save.
             version -[in] extension version.
                        1 -WL6
                        2 -SOD

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void SavePic(W32 chunknum, W16 version, W8 *buffer, W8 *buffer2)
{
    W16 i;
    W16 temp;
    char filename[32];
    char *fname;
    W8 *ptr;
    W8 *pic;
	W32	picnum;
	W16 width, height;
    W16 linewidth, plane, sx, sy;
    W8 r,g,b;
	W16 rgb;
	W8 *tpalette;
	static W16 offset;

	tpalette = (W8*)gamepal; /* initialize separately */
	offset = 0; /* initialize separately */


	if ((((chunknum == WL1_N_BLANKPIC) ||
		  (chunknum == WL1_NOKEYPIC)) && (version & WL1_PAK))) {
		return;
	}

	if ((((chunknum == N_BLANKPIC) ||
		  (chunknum == NOKEYPIC)) && (version & WL6_PAK))) {
		return;
	}

	if ((((chunknum == SDM_N_BLANKPIC) ||
		  (chunknum == SDM_NOKEYPIC)) && (version & SDM_PAK))) {
		return;
	}

	if ((((chunknum == SOD_N_BLANKPIC) ||
		  (chunknum == SOD_NOKEYPIC)) && (version & SOD_PAK))) {
		return;
	}

	/* Spear used multiple palettes, so
	 * pull out the one we need. */
	if (version & SDM_PAK) {
		switch (chunknum) {
			case SDM_TITLE1PIC:
            case SDM_TITLE2PIC:
                CA_CacheGrChunk(SDM_TITLEPALETTE, version);
                tpalette = grsegs[SDM_TITLEPALETTE];
                break;

			 default:
                tpalette = (W8*)gamepal;
                break;
		}
	}

	if (version & SOD_PAK) {
        switch (chunknum) {
            case SOD_IDGUYS1PIC:
            case SOD_IDGUYS2PIC:
                CA_CacheGrChunk(SOD_IDGUYSPALETTE, version);
                tpalette = grsegs[SOD_IDGUYSPALETTE];
                break;

            case SOD_TITLE1PIC:
            case SOD_TITLE2PIC:
                CA_CacheGrChunk(SOD_TITLEPALETTE, version);
                tpalette = grsegs[SOD_TITLEPALETTE];
                break;

            case SOD_ENDSCREEN11PIC:
                CA_CacheGrChunk(SOD_END1PALETTE, version);
                tpalette = grsegs[SOD_END1PALETTE];
                break;

            case SOD_ENDSCREEN12PIC:
                CA_CacheGrChunk(SOD_END2PALETTE, version);
                tpalette = grsegs[SOD_END2PALETTE];
                break;

            case SOD_ENDSCREEN3PIC:
                CA_CacheGrChunk(SOD_END3PALETTE, version);
                tpalette = grsegs[SOD_END3PALETTE];
                break;

            case SOD_ENDSCREEN4PIC:
                CA_CacheGrChunk(SOD_END4PALETTE, version);
                tpalette = grsegs[SOD_END4PALETTE];
                break;

            case SOD_ENDSCREEN5PIC:
                CA_CacheGrChunk(SOD_END5PALETTE, version);
                tpalette = grsegs[SOD_END5PALETTE];
                break;

            case SOD_ENDSCREEN6PIC:
                CA_CacheGrChunk(SOD_END6PALETTE, version);
                tpalette = grsegs[SOD_END6PALETTE];
                break;

            case SOD_ENDSCREEN7PIC:
                CA_CacheGrChunk(SOD_END7PALETTE, version);
                tpalette = grsegs[SOD_END7PALETTE];
                break;

            case SOD_ENDSCREEN8PIC:
                CA_CacheGrChunk(SOD_END8PALETTE, version);
                tpalette = grsegs[SOD_END8PALETTE];
                break;

            case SOD_ENDSCREEN9PIC:
                CA_CacheGrChunk(SOD_END9PALETTE, version);
                tpalette = grsegs[SOD_END9PALETTE];
                break;

            default:
                tpalette = (W8*)gamepal;
                break;

        }    /* End switch chunknum */

    } /* End if version & SOD_PAK */

STATUSBARHACK:

	picnum = (chunknum - STARTPICS);

	pic = grsegs[chunknum];
	width = (W16)pictable[picnum].width;
	height = (W16)pictable[picnum].height;


    linewidth = (width / 4);
    for ((i = 0); (i < (width * height)); ++i, pic++) {
        plane = (i / ((width * height) / 4));
        sx = (((i % (linewidth)) * 4) + plane);
        sy = ((i / linewidth) % height);

        ptr = (buffer + ((sx * 2) + (sy * width) * 2));

		if (pic == NULL) {
			printf("SavePic(): 'pic' is null, cannot use it to set 'temp' (in loop iteration '%i').\n", i);
			return;
		} else {
			temp = ((*pic) * 3);
		}
		r = (tpalette[temp] >> 1);
        g = tpalette[(temp + 1)];
        b = (tpalette[(temp + 2)] >> 1);

        rgb = (W16)((W16)(b << 11) | (g << 5) | r);
        ptr[0] = (rgb & 0xff);
        ptr[1] = (rgb >> 8);
    }


/*
 *	Hacks to reassemble images
 */
	if (version & WL1_PAK) {
		if (chunknum == WL1_STATUSBARPIC) {
			/* Save Status bar pic: */
			memcpy(buffer2, buffer, (size_t)(width * height * 2));
			CA_CacheGrChunk(WL1_NOKEYPIC, version); /* cache NOKEYPIC */
			chunknum = WL1_NOKEYPIC;

			goto STATUSBARHACK;
		} else if( chunknum == WL1_H_BOTTOMINFOPIC ) {
			MergeImages( buffer, 2, 304, 91, 16, 24, 4,
						 buffer2, 2, 91, 91, 16, 0, 0 );
			MergeImages( buffer, 2, 304, 91, 16, 192, 4,
						 buffer2, 2, 91, 91, 16, 0, 16 );


			hq2x_32( buffer2, buffer, 91, 16, (91*2)*4 );
			RGB32toRGB24( buffer, buffer, 182*32*4 );

			cs_snprintf( filename, sizeof( filename ), "%s/%s.tga", LGFXDIR, "PLAQUE_PAGE" );

			WriteTGA( filename, 24, 182, 32, buffer, 0, 0 );

			hq2x_32( buffer2 + (16 * 91 * 2), buffer, 91, 16, (91*2)*4 );
			RGB32toRGB24( buffer, buffer, 182*32*4 );

			cs_snprintf( filename, sizeof( filename ), "%s/%s.tga", LGFXDIR, "PLAQUE_BLANK" );

			WriteTGA( filename, 24, 182, 32, buffer, 0, 1 );

			return;
		} else if( chunknum == WL1_NOKEYPIC ) {
			chunknum = WL1_STATUSBARPIC;
			MergePics(buffer, buffer2, (W16)width, (W16)height, (W16)2,
					  (W16)320, (W32)240, (W32)4);
			MergePics(buffer, buffer2, (W16)width, (W16)height, (W16)2,
					  (W16)320, (W32)240, (W32)(4 + height));

			memcpy(buffer, buffer2, (size_t)(320 * 40 * 2));
			width = (W16)320;
			height = (W16)40;
		} else if (chunknum == WL1_L_COLONPIC) {
			memset(buffer2, 0, (size_t)(256 * 64 * 2));

			MergePics(buffer, buffer2, (W16)width, (W16)height, (W16)2,
					  (W16)256, (W32)160, (W32)16);

			return;
		} else if (chunknum == WL1_L_EXPOINTPIC) {
			MergePics(buffer, buffer2, (W16)width, (W16)height, (W16)2,
					  (W16)256, (W32)16, (W32)0);

			return;
		} else if( chunknum == WL1_L_APOSTROPHEPIC ) {
			/* "W16 i;" is already available, from the beginning of the func */

			MergePics(buffer, buffer2, (W16)width, (W16)height, (W16)2,
					  (W16)256, (W32)112, (W32)0);

			memcpy(buffer, buffer2, (size_t)(256 * 64 * 2));


			for( i = 0 ; i < 256 * 64 * 2 ; i += 2 ) {
				if( buffer[ i ] == 0 && buffer[ i + 1 ] == 0 ) {
					 buffer[ i + 1 ] = 66;
				}
			}

			offset = 0;
			width = 256;
			height = 64;
		} else if( chunknum == WL1_L_PERCENTPIC ) {
			offset = 16; /* this is for L_APIC... */

			MergePics( buffer, buffer2, width, height, 2, 256, 80, 0 );

			return;
		} else if( chunknum >= WL1_L_NUM0PIC && chunknum <= WL1_L_NUM9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, offset, 16 );

			offset += width;

			return;
		} else if( chunknum >= WL1_N_0PIC && chunknum < WL1_N_9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			offset += width + 1;

			return;
		} else if( chunknum == WL1_N_9PIC ) {
			W32 i; /* different than original "i" */

			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			memcpy(buffer, buffer2, (size_t)(90 * height * 2));

			for ((i = 0); (i < (90 * 16 * 2)); (i += 2)) {
				if (! (i % 9) && (i != 0)) {
					buffer[(i - 2)] = 0;
					buffer[(i - 1)] = 160;
				}
			}

			width = 90;
			offset = 0;
		} else if( chunknum >= WL1_L_APIC && chunknum <= WL1_L_ZPIC ) {
			static W32 yoffset = 32;

			MergePics( buffer, buffer2, width, height, 2, 256, offset, yoffset );

			offset += width;

			if( offset >= 256 ) {
				offset = 0;
				yoffset += 16;
			}

			return;
		} else if( chunknum == WL1_FACE5CPIC ) {
			/* hmmm... Why is this one messed up? */

			MergeImages( buffer, 2, 24, 18, height-2, 8, 2,
						 buffer2, 2, 24, 18, height-2, 0, 0 );
			MergeImages( buffer, 2, 24, 8, height-3, 0, 3,
						 buffer2, 2, 24, 8, height-2, 16, 0 );

			MergeImages( buffer, 2, 24, 18, 2, 9, 0,
						 buffer2, 2, 24, 18, 2, 0, height-2 );
			MergeImages( buffer, 2, 24, 7, 3, 1, 0,
						 buffer2, 2, 24, 7, 3, 16, height-3 );

			memcpy(buffer, buffer2, (size_t)(24 * 32 * 2));


			buffer[(30 * 24 * 2) + (3 * 2) ] = 73;
			buffer[(30 * 24 * 2) + (3 * 2) + 1 ] = 74;

			buffer[(31 * 24 * 2) + (3 * 2) ] = 73;
			buffer[(31 * 24 * 2) + (3 * 2) + 1 ] = 74;


			buffer[(29 * 24 * 2) + (23 * 2) ] = 73;
			buffer[(29 * 24 * 2) + (23 * 2) + 1 ] = 74;

			buffer[(30 * 24 * 2) + (23 * 2) ] = 73;
			buffer[(30 * 24 * 2) + (23 * 2) + 1 ] = 74;

			buffer[(31 * 24 * 2) + (23 * 2) ] = 73;
			buffer[(31 * 24 * 2) + (23 * 2) + 1 ] = 74;


			buffer[(29 * 24 * 2) + (19 * 2) ] = 255;
			buffer[(29 * 24 * 2) + (19 * 2) + 1 ] = 100;

			buffer[(30 * 24 * 2) + (19 * 2) ] = 63;
			buffer[(30 * 24 * 2) + (19 * 2) + 1 ] = 117;

			buffer[(31 * 24 * 2) + (19 * 2) ] = 52;
			buffer[(31 * 24 * 2) + (19 * 2) + 1 ] = 59;


			buffer[(30 * 24 * 2) + (7 * 2) ] = 19;
			buffer[(30 * 24 * 2) + (7 * 2) + 1 ] = 59;

			buffer[(31 * 24 * 2) + (7 * 2) ] = 19;
			buffer[(31 * 24 * 2) + (7 * 2) + 1 ] = 59;


			buffer[(30 * 24 * 2) + (11 * 2) ] = 91;
			buffer[(30 * 24 * 2) + (11 * 2) + 1 ] = 84;

			buffer[(31 * 24 * 2) + (11 * 2) ] = 190;
			buffer[(31 * 24 * 2) + (11 * 2) + 1 ] = 92;


			buffer[(30 * 24 * 2) + (15 * 2) ] = 249;
			buffer[(30 * 24 * 2) + (15 * 2) + 1 ] = 75;

			buffer[(31 * 24 * 2) + (15 * 2) ] = 190;
			buffer[(31 * 24 * 2) + (15 * 2) + 1 ] = 92;
		}
	} else if( version & WL6_PAK ) {
		if( chunknum == STATUSBARPIC ) {
			memcpy(buffer2, buffer, (size_t)(width * height * 2)); /* Save Status bar pic */
			CA_CacheGrChunk( NOKEYPIC, version ); /* cache NOKEYPIC */
			chunknum = NOKEYPIC;

			goto STATUSBARHACK;
		} else if( chunknum == H_BOTTOMINFOPIC ) {
			MergeImages( buffer, 2, 304, 91, 16, 24, 4,
						 buffer2, 2, 91, 91, 16, 0, 0 );
			MergeImages( buffer, 2, 304, 91, 16, 192, 4,
						 buffer2, 2, 91, 91, 16, 0, 16 );


			hq2x_32( buffer2, buffer, 91, 16, (91*2)*4 );
			RGB32toRGB24( buffer, buffer, 182*32*4 );

			cs_snprintf( filename, sizeof( filename ), "%s/%s.tga", LGFXDIR, "PLAQUE_PAGE" );

			WriteTGA( filename, 24, 182, 32, buffer, 0, 0 );

			hq2x_32( buffer2 + (16 * 91 * 2), buffer, 91, 16, (91*2)*4 );
			RGB32toRGB24( buffer, buffer, 182*32*4 );

			cs_snprintf( filename, sizeof( filename ), "%s/%s.tga", LGFXDIR, "PLAQUE_BLANK" );

			WriteTGA( filename, 24, 182, 32, buffer, 0, 1 );

			return;
		} else if( chunknum == NOKEYPIC ) {
			chunknum = STATUSBARPIC;
			MergePics( buffer, buffer2, width, height, 2, 320, 240, 4 );
			MergePics( buffer, buffer2, width, height, 2, 320, 240, 4+height );

			memcpy(buffer, buffer2, (size_t)(320 * 40 * 2));
			width = 320;
			height = 40;
		} else if( chunknum == L_COLONPIC ) {
			memset( buffer2, 0, (size_t)(256 * 64 * 2));

			MergePics( buffer, buffer2, width, height, 2, 256, 160, 16 );

			return;
		} else if( chunknum == L_EXPOINTPIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, 16, 0 );

			return;
		} else if( chunknum == L_APOSTROPHEPIC ) {
			W16 i; /* original "i" is "W16" like here, but there was a "W32 i;" in between... */

			MergePics( buffer, buffer2, width, height, 2, 256, 112, 0 );

			memcpy(buffer, buffer2, (size_t)(256 * 64 * 2));


			for ((i = 0); (i < (256 * 64 * 2)); (i += 2)) {
				if ((buffer[i] == 0) && (buffer[(i + 1)] == 0)) {
					 buffer[(i + 1)] = 66;
				}
			}

			offset = 0;
			width = 256;
			height = 64;
		} else if( chunknum == L_PERCENTPIC ) {
			offset = 16; /* this is for L_APIC... */

			MergePics( buffer, buffer2, width, height, 2, 256, 80, 0 );

			return;
		} else if( chunknum >= L_NUM0PIC && chunknum <= L_NUM9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, offset, 16 );

			offset += width;

			return;
		} else if( chunknum >= N_0PIC && chunknum < N_9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			offset += width + 1;

			return;
		} else if( chunknum == N_9PIC ) {
			W32 i; /* different than original "i" */

			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			memcpy(buffer, buffer2, (size_t)(90 * height * 2));

			for ((i = 0); (i < (90 * 16 * 2)); (i += 2)) {
				if (! (i % 9) && (i != 0)) {
					buffer[(i - 2)] = 0;
					buffer[(i - 1)] = 160;
				}
			}

			width = 90;
			offset = 0;
		} else if( chunknum >= L_APIC && chunknum <= L_ZPIC ) {
			static W32 yoffset = 32;

			MergePics( buffer, buffer2, width, height, 2, 256, offset, yoffset );

			offset += width;

			if( offset >= 256 ) {
				offset = 0;
				yoffset += 16;
			}

			return;
		}
	} else if( version & SDM_PAK ) {
		if( chunknum == SDM_STATUSBARPIC ) {
			memcpy(buffer2, buffer, (size_t)(width * height * 2)); /* Save Status bar pic */
			CA_CacheGrChunk( SDM_NOKEYPIC, version ); /* cache SOD_NOKEYPIC */
			chunknum = SDM_NOKEYPIC;

			goto STATUSBARHACK;
		} else if( chunknum == SDM_NOKEYPIC ) {
			chunknum = SDM_STATUSBARPIC;
			MergePics(buffer, buffer2, width, height, 2, 320, 240, 4);
			MergePics(buffer, buffer2, width, height, 2, 320, 240, (4 + height));

			memcpy(buffer, buffer2, (size_t)(320 * 40 * 2));
			width = 320;
			height = 40;
		} else if (chunknum == SDM_L_COLONPIC) {
			memset(buffer2, 0, (size_t)(256 * 64 * 2));

			MergePics( buffer, buffer2, width, height, 2, 256, 160, 16 );

			return;
		} else if( chunknum == SDM_L_EXPOINTPIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, 16, 0 );

			return;
		} else if( chunknum == SDM_L_APOSTROPHEPIC ) {
			W16 i; /* original "i" is "W16" like here, but there were some
					* "W32 i;" declarations in between... */

			MergePics( buffer, buffer2, width, height, 2, 256, 112, 0 );

			memcpy(buffer, buffer2, (size_t)(256 * 64 * 2));


			for ((i = 0); (i < (256 * 64 * 2)); (i += 2)) {
				if ((buffer[i] == 0) && (buffer[(i + 1)] == 0)) {
					 buffer[(i + 1)] = 66;
				}
			}

			offset = 0;
			width = 256;
			height = 64;
		} else if (chunknum == SDM_L_PERCENTPIC) {
			offset = 16; /* this is for L_APIC... */

			MergePics( buffer, buffer2, width, height, 2, 256, 80, 0 );

			return;
		} else if( chunknum >= SDM_L_NUM0PIC && chunknum <= SDM_L_NUM9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, offset, 16 );

			offset += width;

			return;
		} else if( chunknum >= SDM_N_0PIC && chunknum < SDM_N_9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			offset += width + 1;

			return;
		} else if( chunknum == SDM_N_9PIC ) {
			W32 i; /* different than original "i" */

			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			memcpy(buffer, buffer2, (size_t)(90 * height * 2));

			for ((i = 0); (i < (90 * 16 * 2)); (i += 2)) {
				if (! (i % 9) && (i != 0)) {
					buffer[(i - 2)] = 0;
					buffer[(i - 1)] = 160;
				}
			}

			width = 90;
			offset = 0;
		} else if( chunknum >= SDM_L_APIC && chunknum <= SDM_L_ZPIC ) {
			static W32 yoffset = 32;

			MergePics( buffer, buffer2, width, height, 2, 256, offset, yoffset );

			offset += width;

			if (offset >= 256) {
				offset = 0;
				yoffset += 16;
			}

			return;
		} else if (chunknum == SDM_TITLE1PIC) {
			memcpy((buffer2+offset), buffer, (size_t)(width * height * 2));
			offset += (width * height * 2);

			return;
		} else if (chunknum == SDM_TITLE2PIC) {
			memcpy((buffer2 + offset), buffer, (size_t)(width * height * 2));
			memcpy(buffer, buffer2, (size_t)(320 * 200 * 2));
			height = 200;
			offset = 0;
		}
    } else if (version & SOD_PAK) {
		if (chunknum == SOD_STATUSBARPIC) {
			/* Save Status bar pic: */
			memcpy(buffer2, buffer, (size_t)(width * height * 2));
			CA_CacheGrChunk( SOD_NOKEYPIC, version ); /* cache SOD_NOKEYPIC */
			chunknum = SOD_NOKEYPIC;

			goto STATUSBARHACK;
		} else if( chunknum == SOD_NOKEYPIC ) {
			chunknum = SOD_STATUSBARPIC;
			MergePics(buffer, buffer2, width, height, 2, 320, 240, 4);
			MergePics(buffer, buffer2, width, height, 2, 320, 240, (4 + height));

			memcpy( buffer, buffer2, (size_t)(320 * 40 * 2));
			width = 320;
			height = 40;
		} else if( chunknum == SOD_L_COLONPIC ) {
			memset( buffer2, 0, (size_t)(256 * 64 * 2));

			MergePics( buffer, buffer2, width, height, 2, 256, 160, 16 );

			return;
		} else if( chunknum == SOD_L_EXPOINTPIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, 16, 0 );

			return;
		} else if( chunknum == SOD_L_APOSTROPHEPIC ) {
			W16 i; /* original "i" is "W16" like here, but there were some
					* "W32 i;" declarations in between... */

			MergePics( buffer, buffer2, width, height, 2, 256, 112, 0 );

			memcpy(buffer, buffer2, (size_t)(256 * 64 * 2));

			for ((i = 0); (i < (256 * 64 * 2)); (i += 2)) {
				if ((buffer[i] == 0) && (buffer[(i + 1)] == 0)) {
					 buffer[(i + 1)] = 66;
				}
			}

			offset = 0;
			width = 256;
			height = 64;
		} else if( chunknum == SOD_L_PERCENTPIC ) {
			offset = 16; /* this is for L_APIC... */

			MergePics( buffer, buffer2, width, height, 2, 256, 80, 0 );

			return;
		} else if( chunknum >= SOD_L_NUM0PIC && chunknum <= SOD_L_NUM9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 256, offset, 16 );

			offset += width;

			return;
		} else if( chunknum >= SOD_N_0PIC && chunknum < SOD_N_9PIC ) {
			MergePics( buffer, buffer2, width, height, 2, 90, offset, 0 );

			offset += width + 1;

			return;
		} else if( chunknum == SOD_N_9PIC ) {
			W32 i; /* different than original "i" */

			MergePics(buffer, buffer2, width, height, 2, 90, offset, 0);

			memcpy(buffer, buffer2, (size_t)(90 * height * 2));

			for ((i = 0); (i < (90 * 16 * 2)); (i += 2)) {
				if (! (i % 9) && (i != 0)) {
					buffer[(i - 2)] = 0;
					buffer[(i - 1)] = 160;
				}
			}

			width = 90;
			offset = 0;
		} else if ((chunknum >= SOD_L_APIC) && (chunknum <= SOD_L_ZPIC)) {
			static W32 yoffset = 32;

			MergePics(buffer, buffer2, width, height, 2, 256, offset, yoffset);

			offset += width;

			if( offset >= 256 ) {
				offset = 0;
				yoffset += 16;
			}

			return;
		} else if( chunknum == SOD_IDGUYS1PIC ) {
			memcpy((buffer2 + offset), buffer, (size_t)(width * height * 2));
			offset += width*height*2;

			return;
		} else if( chunknum == SOD_IDGUYS2PIC ) {
			memcpy((buffer2 + offset), buffer, (size_t)(width * height * 2));
			memcpy(buffer, buffer2, (size_t)(320 * 200 * 2));
			height = 200;
			offset = 0;
		} else if (chunknum == SOD_TITLE1PIC) {
			memcpy((buffer2+offset), buffer, (size_t)(width * height * 2));
			offset += width*height*2;

			return;
		} else if (chunknum == SOD_TITLE2PIC) {
			memcpy((buffer2 + offset), buffer, (size_t)(width * height * 2));
			memcpy(buffer, buffer2, (size_t)(320 * 200 * 2));
			height = 200;
			offset = 0;
		}
    }
/*
 *	End of images hacks
 */


	if( version & WL1_PAK ) {
		fname = GetLumpFileName_WL1( chunknum );
	} else if( version & WL6_PAK ) {
        fname = GetLumpFileName_WL6( chunknum );
    } else if( version & SDM_PAK ) {
        fname = GetLumpFileName_SDM( chunknum );
    } else if( version & SOD_PAK ) {
        fname = GetLumpFileName_SOD( chunknum );
    } else {
        printf("Unknown file extension!\n");

        return;
    }

    if (fname == NULL) {
        printf("File name not found for item: (%d)\n", (int)chunknum);

		return;
    }

	cs_snprintf( filename, sizeof( filename ), "%s/%s.tga", LGFXDIR, fname );


    hq2x_32( buffer, buffer2, width, height, (width*2)*4 );


	/* Get rid of alpha channel */
	RGB32toRGB24( buffer2, buffer2, (width*2)*(height*2)*4 );


	WriteTGA( filename, 24, (width*2), (height*2), buffer2, 0, 1 );

	return;
} /* end of SavePic() function (that was really long!) */



/*
-----------------------------------------------------------------------------
 Function: Fontline() -Extract and save font.

 Parameters: fontnumber -[in] font to save.
             version -[in] extension version.
                        1 -WL6
                        2 -SOD

 Returns: Nothing.

 Notes: Font must be cached (with what?) in grsegs[] before calling this.
-----------------------------------------------------------------------------
*/
PRIVATE void Fontline(W32 fontnumber, W16 version)
{
	fontstruct	*font; /* declared up top in this file */
	W16 i;
	W16 x, y;
	W16 px, py;
	W8 *buffer;
	W8 *source;
	W8 *ptr;
	char filename[256];

	printf("looking up font number %lu in grsegs[]...\n", fontnumber);
	if (fontnumber > NUMCHUNKS) {
		printf("warning: font number requested is higher than the number of fonts available (%i) in grsegs[]\n",
			   NUMCHUNKS);
	}
	font = (fontstruct *)grsegs[fontnumber];

	if (font == NULL) {
		printf("Error: font %lu was NOT cached in grsegs[]!\n", fontnumber);
		return;
	}

							   /* (256 * 128 * 4) = 131072 */
	buffer = MM_MALLOC((size_t)(FONTWIDTH * FONTHEIGHT * 4));
	if (buffer == NULL) {
		return;
	}

	ptr = buffer;
	for ((x = 0); (x < FONTWIDTH); ++x) {
		for ((y = 0); (y < FONTHEIGHT); ++y, (ptr += 4)) {
			ptr[0] = ptr[1] = ptr[2] = 0xFF;
			ptr[3] = 0x00;
		}
	}

	px = py = 0;
	for ((i = 0); (i < 256); ++i) {
		/* dummy condition to use 'version' parameter: */
		if (version == i) {
			printf("Easter Egg: version number (%i) is the same as the number of times through this loop (%i)! \n",
				   version, i);
		}

		/* I thought that 'width' was already a 'char' though? */
		/* The warning produced by the old version of clang that comes with
		 * Xcode 3.2.6 can be silenced with a cast here, but other compilers
		 * will still complain: */
		if (font->width[i] == NULL) {
			printf("Error: font %i was NOT cached in grsegs[]!\n", i);
			return;
		}

		if (! font->width[i]) { /* bad access here? */
			continue;
		} else {
			printf("%c", font->width[i]);
		}

									/* (256 - 1) = 255 */
		if ((px + font->width[i]) > (FONTWIDTH - 1)) {
			py += font->height;
			px = 0;
		}

		source = (((PW8)font) + font->location[i]);

		ptr = (buffer + (py * FONTWIDTH + px) * 4);
		for ((y = 0); (y < font->height); ++y, (ptr += (FONTWIDTH * 4))) {
			for ((x = 0); (x < font->width[i]); ++x) {
				if (*source++) {
					ptr[((x * 4) + 3)] = 0xFF;
				}
			}
		}

		px += 16;

	} /* end of the "for (i = 0); (i < 256); ++i" for-loop */


	cs_snprintf(filename, sizeof(filename), "%s/font%d.tga", LGFXDIR,
				fontnumber);

	WriteTGA(filename, 32, FONTWIDTH, FONTHEIGHT, buffer, 0, 1);

	MM_FREE(buffer);

}

/* TODO: actually use this function */
/* (it _is_ actually used; it is just that the place where it is used had been
 * ifdef-ed out (in the LumpExtractor() function)...) */
PRIVATE void DecodeText(W16 version)
{
	char *text;
	int artnum;
	int endextern;
	int i;
	int length;
	FILE *fhandle;
	int limit;


	limit = 6;
	if (version & SOD_PAK) {
		endextern = 168;
		fhandle = fopen("sod.txt", "wb");
		if (! fhandle) {
			return;
		}

		limit = 1;
	} else if (version & WL6_PAK) {
		endextern = 143;
		fhandle = fopen("wl6.txt", "wb");
		if (! fhandle) {
			return;
		}
	} else {
		return;
	}

	for ((i = 0); (i < limit); ++i) {
		artnum = (endextern + i);
		CA_CacheGrChunk((W32)artnum, version);
		text = (char *)grsegs[artnum];

		if (text == NULL) {
			printf("DecodeText(): text is null, cannot find length of it.\n");
			return;
		} else {
			length = (int)strlen(text);
		}
		/* do we really need 'length' as an intermediate variable here? All it
		 * does is cause the result of strlen() to be casted to from 'size_t' to
		 * 'int', which is pointless because it needs to be casted right back to
		 * 'size_t' again the next time that it is used... */

		fwrite(text, sizeof(W8), (size_t)length, fhandle);
		fprintf(fhandle, "\n\n");

		MM_FREE(grsegs[artnum]);
	}

	fclose(fhandle);
}

/*
-----------------------------------------------------------------------------
 Function: LumpExtractor() -Extract Lump gfx from Wolf3D and SOD data files.

 Parameters: fextension -[in] String holding file extension
                            (must be in '.XXX' format).
             limit -[in] max
             version -[in] extension version.
                        1 -WL6
                        2 -SOD

 Returns:   Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC _boolean LumpExtractor(const char *fextension, W32 limit, W16 version)
{
    W32 i;
    W8 *buffer, *buffer2; /* is this really the correct type to use here? */


    if (! fextension || ! *fextension) {
        printf("Invalid file extension passed into LumpExtractor!\n");

		return false;
    }

/*
 * Setup
 */

	if (0 == FS_Mkdir(LGFXDIR)) {
		printf("[%s] Could not create directory (%s)!\n", "wolf_gfx.c",
			   LGFXDIR);

		return false;
	}


    if (! CAL_SetupGrFile(fextension)) {
		CAL_Shutdown();

		return false;
	}

/*
 * Allocate buffers
 */

	buffer = (W8*)MM_MALLOC((size_t)(320 * 416 * 2));
	if (buffer == NULL) {
		printf("LumpExtractor(): null buffer malloced, returning...\n");
		CAL_Shutdown();

		return false;
	}

	/* grnameofhandle changes somewhere between these two... */
    buffer2 = (W8*)MM_MALLOC((size_t)(640 * 400 * 4));
	if (buffer2 == NULL) {
		printf("LumpExtractor(): second buffer failed to malloc, freeing first one and returning...\n");
		MM_FREE(buffer);
		CAL_Shutdown();

		return false;
	}

/*
 * Decode GFX data
 */

	printf("LumpExtractor(): attempting to decode GFX Data...\n");

#if 1 || __clang_analyzer__
	(void)DecodeText(version);
#endif /* 1 || __clang_analyzer__ */

	/* STARTPICS is defined to '3' in "wolf_def.h", so it _should_ always be
	 * smaller than NUMCHUNKS (i.e. '169'), which is the number of elements
	 * in grsegs[], which Fontline() accesses: */
	for ((i = STARTFONT); (i < STARTPICS); ++i) {
        CA_CacheGrChunk(i, version); /* use 'i' as 'chunk' */
        Fontline(i, version); /* use 'i' as 'fontnumber' */
    }


    for ((i = STARTPICS); (i < (limit + 1)); ++i) {
		if (i == limit) {
			printf("LumpExtractor(): reached limit '%lu' \n", limit);
		}
        CA_CacheGrChunk(i, version); /* use 'i' as 'chunk' */
        SavePic(i, version, buffer, buffer2); /* use 'i' as 'chunknum' */
    }


/*
 * Shutdown
 */

    MM_FREE(buffer2);
	MM_FREE(buffer);

    CAL_Shutdown();

	return true;
}

/* EOF */

