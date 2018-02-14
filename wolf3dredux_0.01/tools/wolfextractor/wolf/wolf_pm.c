/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	wolf_pm.c:	Decode Wolfenstein3-D Page data.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Wolfenstein 3-D, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wolf_def.h"

#include "../../../common/arch.h"
#include "../../../common/common_utils.h"
#include "../loaders/tga.h"
#include "../hq2x.h"
#include "../loaders/wav.h"
#include "../filesys/file.h"
#include "../string/com_string.h"
#include "../memory/memory.h"


extern char gamepal[];

#define PAGEFNAME   "VSWAP"


#define SAMPLERATE 7000 /* In Hz */


typedef	struct
{
    W32  offset; /* Offset of chunk into file */
    W16  length; /* Length of the chunk */

} PageList_t;

typedef struct
{
	W16 leftpix, rightpix;
	W16	dataofs[64];
    /* table data after dataofs[(rightpix - leftpix + 1)] */

} t_compshape;


PRIVATE PageList_t	*PMPages;

PRIVATE FILE	*PageFile = NULL;

PRIVATE W16 PMNumBlocks;
PRIVATE W16 PMSpriteStart, PMSoundStart;



/*
-----------------------------------------------------------------------------
 Function: CAL_GetGrChunkLength() -Opens the page file and sets up
                                    the page info.

 Parameters: extension -[in] Pointer to a null-terminated string that
                            specifies the file extension.
                            (must be in '.XXX' format)

 Returns: 1 on success, otherwise 0.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE W8 PML_OpenPageFile(const char *extension)
{
	W32    i;
	W32   size;
	void   *buf;
	char filename[16];
	W32  *offsetptr;
	W16 *lengthptr;
	PageList_t *page;

	if (! extension || ! *extension) {
	   printf("Invalid file extension passed to PML_OpenPageFile()\n");
	   return 0;
    }

    cs_strlcpy(filename, PAGEFNAME, sizeof(filename));
    cs_strlcat(filename, extension, sizeof(filename));

	PageFile = fopen(cs_strupr(filename), "rb");
	if (PageFile == NULL) {
		PageFile = fopen(cs_strlwr(filename), "rb");
		if (PageFile == NULL) {
			printf("Could not open file (%s) for read!\n", filename);
			return 0;
		}
	}

	/* Read in header variables */
	(void)fread(&PMNumBlocks, sizeof(PMNumBlocks), (size_t)1, PageFile);
	(void)fread(&PMSpriteStart, sizeof(PMSpriteStart), (size_t)1, PageFile);
	(void)fread(&PMSoundStart, sizeof(PMSoundStart), (size_t)1, PageFile);


	/* Allocate and clear the page list */
	PMPages = (PageList_t *)MM_MALLOC(sizeof(PageList_t) * PMNumBlocks);
	if (PMPages == NULL) {
		return 0;
	}

	memset(PMPages, 0, (sizeof(PageList_t) * PMNumBlocks));

	/* Read in the chunk offsets */
	size = (sizeof(W32) * PMNumBlocks);

    buf = MM_MALLOC(size);
	if (buf == NULL) {
		return 0;
	}

	if (fread(buf, (size_t)1, size, PageFile) == 0) {
		printf("PML_OpenPageFile(): Length read failed\n");
	}
	offsetptr = (PW32)buf;
	for ((i = 0), (page = PMPages); (i < PMNumBlocks); i++, page++) {
		page->offset = *offsetptr++;
    }
	MM_FREE(buf);

	/* Read in the chunk lengths */
	size = (sizeof(W16) * PMNumBlocks);

	buf = MM_MALLOC(size);
	if (buf == NULL) {
		return 0;
	}

	if (fread(buf, (size_t)1, size, PageFile) == 0) {
		printf("PML_OpenPageFile: Length read failed\n");
	}

    lengthptr = (PW16)buf;
	for ((i = 0), (page = PMPages); (i < PMNumBlocks); ++i, page++) {
		page->length = *lengthptr++;
	}

    MM_FREE(buf);

    return 1;
}

/*
-----------------------------------------------------------------------------
 Function: PML_ReadFromFile() -Reads in data from Page file.

 Parameters: buf -[out] Storage location for data.
			 offset -[in] Number of bytes from beginning of file.
			 length -[in] Maximum number of items to be read.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PML_ReadFromFile(W8 *buf, SW32 offset, W16 length)
{
	if (! buf) {
		printf("[PML_ReadFromFile]: NULL pointer\n");
		return;
	}

	if (buf == NULL) {
		printf("[PML_ReadFromFile]: buf is null\n");
		return;
	}

	if (! offset) {
		printf("[PML_ReadFromFile]: Zero offset\n");
		return;
	}

	if (fseek(PageFile, offset, SEEK_SET)) {
		printf("[PML_ReadFromFile]: Seek failed with offset '%lu'\n",
			   offset);
		return;
	}

	if (! fread(buf, (size_t)1, (size_t)length, PageFile)) {
		printf("[PML_ReadFromFile]: Reading file using buf '%s' and length '%i' failed\n",
			   buf, length);
		return;
	}
}

/*
-----------------------------------------------------------------------------
 Function: PML_LoadPage() -Reads in data from Page file.

 Parameters: buf -[out] Storage location for data.
			 offset -[in] Number of bytes from beginning of file.
			 clength -[in] Maximum number of items to be read.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void *PML_LoadPage(W32 pagenum, W16 *clength)
{
    W8 *addr;
    PageList_t *page;

    page = &PMPages[pagenum];
	if (page->length == 0) {
		return NULL;
	}

    *clength = page->length;

    addr = MM_MALLOC(page->length);
	if (addr == NULL) {
		return NULL;
	}

    PML_ReadFromFile(addr, (SW32)page->offset, (W16)page->length);

    return addr;
}

extern void ReduxAlphaChannel_hq2x(W8 *data, W32 width, W32 height);

/*
-----------------------------------------------------------------------------
 Function: PML_LoadPage() -Save image data from page file.

 Parameters: nPage -[in] Page number to save.
			 filename -[in] Pointer to string with filename.
			 buffer -[in] Allocated memory buffer to hold image data.
			 buffer2 -[in] Allocated memory buffer to hold hq2x data.

 Returns: Non-zero on success, otherwise zero.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE W8 PML_SaveGFXPage(W32 nPage, const char *filename, W8 *buffer,
						   W8 *buffer2, _boolean iswall, W32 GunFlash)
{
	W16 x, y, i;
	W8 *data;
	W8 *ptr;
	W16 temp;
	W16 rgb;
	W8 r,g,b;
	W16 clength; /* Chunk length */

	ptr = buffer; /* try initializing here like this to avoid error later */

	if (filename != NULL) {
		printf("PML_SaveGFXPage(): trying to save '%s'\n", filename);
	}

	/* dummy condition to use GunFlash parameter: */
	if (GunFlash == 0) {
		;
	}

	data = (PW8)PML_LoadPage(nPage, &clength);
	if (data == NULL) {
        return 0;
    }

    if (nPage < PMSpriteStart) {
	   for ((x = 0); (x < 64); ++x) {
            for ((y = 0); (y < 64); ++y) {
                temp = ((data[((x << 6) + y)]) * 3);

                ptr = (buffer + (((y << 6) + x) * 2));

                r = (W8)(gamepal[temp] >> 1);
                g = (W8)(gamepal[(temp + 1)]);
                b = (W8)(gamepal[(temp + 2)] >> 1);

                rgb = (W16)((b << 11) | (g << 5) | r);
                ptr[0] = (rgb & 0xff);
                ptr[1] = (rgb >> 8);
            }
        }
    } else if ((nPage >= PMSpriteStart) && (nPage < PMSoundStart)) {
        W16 *cmdptr;
#if defined(SIZEOF_SHORT) && defined(SIZEOF_INT)
# if (SIZEOF_INT) > (SIZEOF_SHORT)
		int *linecmds;
# else
        short *linecmds; /* this is a bad size to use */
# endif /* SIZEOF_INT > SIZEOF_SHORT */
#else
		short *linecmds; /* this is a bad size to use */
#endif /* SIZEOF_SHORT && SIZEOF_INT */
        t_compshape *shape;

#ifdef ALIGNOF_SHORT
# if (ALIGNOF_SHORT == 0) || (ALIGNOF_SHORT != 2)
		printf("PML_SaveGFXPage(): linecmds might be misaligned; offsetof 'short' is '%i'...\n",
			   (int)(ALIGNOF_SHORT));
# endif /* (ALIGNOF_SHORT == 0) || (ALIGNOF_SHORT != 2) */
#endif /* ALIGNOF_SHORT */

#ifdef ALIGNOF_INT
# if (ALIGNOF_INT == 0) || (ALIGNOF_INT != 4)
		printf("PML_SaveGFXPage(): linecmds might be misaligned; offsetof 'int' is '%i'...\n",
			   (int)(ALIGNOF_INT));
# endif /* (ALIGNOF_INT == 0) || (ALIGNOF_INT != 4) */
#endif /* ALIGNOF_INT */

#if 0 || __clang_analyzer__ || 1
		memset(buffer, 0, (size_t)(64 * 64 * 2)); /* (64 * 64 * 2) = 8192 */
#endif /* 0 || __clang_analyzer__ || 1 */

        /* all transparent at the beginning */
        for ((x = 0); (x < (64 * 64 * 2)); (x += 2)) {
			ptr = (buffer + x);

			r = 31;
			g = 0;
			b = 31;

			rgb = (W16)((b << 11) | (g << 5) | r);
			ptr[0] = (rgb & 0xff);
			ptr[1] = (rgb >> 8);
		}

        shape = (t_compshape *)data;

        cmdptr = shape->dataofs;
		/* hardcoded number is because shape->rightpix apparently was
		 * not actually the correct maximum for 'x'...
		 * TODO: figure out why... */
        for ((x = shape->leftpix); (x <= shape->rightpix), (x < 183); ++x) {
            linecmds = (int *)(data + *cmdptr++);
			/* one of clang's sanitizers throws a runtime error around here */
            for ((i = 0); (*linecmds); (linecmds += (short)(3))) {
				/* dummy condition to use 'i': */
				if (i == 0) {
					;
				}
                i = (W16)((linecmds[2] / 2) + linecmds[1]);
				/* hardcoded number is because (linecmds[0] /2) apparently was
				 * not actually the correct maximum for 'y'...
				 * TODO: figure out why... */
                for ((y = (W16)(linecmds[2] / 2)); (y < (linecmds[0] / 2)),
					 (y < 65332), (i <= 7987); ++y, ++i) {
				    temp = ((data[i]) * 3);

					if (buffer && (&buffer != NULL)) {
#if defined(DEBUG) && 0
						printf("PML_SaveGFXPage(): progress saving '%s': ", filename);
						printf(" math: 'y' = '%i', 'x' = '%i', (((y * 64) + x) * 2) = '%i', ",
							   y, x, (((y * 64) + x) * 2));
						printf(" loop counters: 'i' = '%i', 'temp' = '%i' \n",
							   i, temp);
#endif /* DEBUG && 0 */
						ptr = (W8*)(buffer + (((y * 64) + x) * 2));
					} else if (buffer2 && (&buffer2 != NULL)) {
						ptr = (W8*)(buffer2 + (((y * 64) + x) * 2));
					} else if (data && (&data != NULL)) {
						ptr = (W8*)(data + (((y * 64) + x) * 2));
					} else {
						ptr = (W8*)"error";
					}

                    r = (W8)(gamepal[temp] >> 1);
                    g = (W8)(gamepal[(temp + 1)]);
                    b = (W8)(gamepal[(temp + 2)] >> 1);

                    rgb = (W16)((b << 11) | (g << 5) | r);
					if (ptr && (&ptr != NULL) && (&ptr[0] != NULL) && rgb) {
						ptr[0] = (rgb & 0xff); /* bad access here... */
						ptr[1] = (rgb >> 8);
					} else {
						break;
					}
                }
				/* This is a bad value for (linecmds[0] / 2): */
				if ((linecmds[0] / 2) > 65332) {
					break;
				} else if (i > 7987) {
					break;
				} else if (!linecmds || (&linecmds == NULL)) {
					break;
				}
#ifdef DEBUG
				break;
#endif /* DEBUG */
            }
        }
    } else {
        MM_FREE(data);
        printf("Out of bounds page number passed into PML_SavePage()!\n");
        return 0;
    }


	MM_FREE(data);

    hq2x_32(buffer, buffer2, 64, 64, (128 * 4)); /* (128 * 4) = 512 */

	if (iswall) {
		/* wtf is the prototype doing here in the middle of another function? */
		extern void RGB32toRGB24(const W8 *src, W8 *dest, size_t size);
		/* I mean, I get that it is right before it is used, but still... */
		RGB32toRGB24(buffer2, buffer2, (size_t)(128 * 128 * 4));
		/* (128 * 128 * 4) = 65536 */
		WriteTGA((const char *)filename, (W16)24, (W16)128, (W16)128,
				 (void *)buffer2, (W8)0, (W8)1);
	} else {
        ReduxAlphaChannel_hq2x(buffer2, (W32)128, (W32)128);

		/* Mod alpha channel for gun flash */
#if 0 || defined MOD_ALPHA_CHANNEL_FOR_GUN_FLASH || __clang_analyzer__
		if (GunFlash && (GunFlash != 521)) {
			for ((i = 200); (i < (128 * 128)); ++i) {
				ptr = (buffer2 + (i * 4));

				if ((ptr[0] > 135) &&
					(ptr[1] >= 0) &&
					(ptr[2] >= 0) &&
					(ptr[1] < 63) &&
					(ptr[2] < 63)) {
					ptr[3] = 103;
				}


				if ((ptr[0] < 250) &&
					(ptr[1] > 59) &&
					(ptr[2] > 59) &&
					(ptr[0] > 227) &&
					(ptr[1] < 185) &&
					(ptr[2] < 185)) {
					ptr[3] = 103;
				}

				/* TODO: continue adding the extra parentheses (from here on) */
				if (ptr[0] > 233 &&
					ptr[1] < 253 &&
					ptr[2] > 75 &&
					ptr[0] < 250 &&
					ptr[1] > 185 &&
					ptr[2] < 172) {
					ptr[3] = 103;
				}

				if (ptr[0] > 0 &&
					ptr[1] == 0 &&
					ptr[2] >= 0 &&
					ptr[2] < 34 ) {
					ptr[3] = 103;
				}

				if (ptr[0] == 248 &&
					ptr[1] < 253 &&
					ptr[2] < 241 &&
					ptr[1] > 190 &&
					ptr[2] > 174) {
					ptr[3] = 103;
				}

				if (ptr[0] == 248 &&
					ptr[1] == 252 &&
					ptr[2] == 248 &&
					i < (110 * 110)) { /* (110 * 110) = 12100 */
					ptr[3] = 103;
				}

			} /* end of for-loop */


			if (GunFlash == 526) {
				buffer2[(92 * 128 * 4) + (62 * 4) + 3] = 103;
				buffer2[(92 * 128 * 4) + (67 * 4) + 3] = 103;

				buffer2[(94 * 128 * 4) + (70 * 4) + 3] = 103;
				buffer2[(95 * 128 * 4) + (59 * 4) + 3] = 103;
				buffer2[(95 * 128 * 4) + (70 * 4) + 3] = 103;

				buffer2[(120 * 128 * 4) + (52 * 4) + 3] = 255;
				buffer2[(120 * 128 * 4) + (53 * 4) + 3] = 255;
				buffer2[(121 * 128 * 4) + (53 * 4) + 3] = 255;
			} else if (GunFlash == 532) {
				buffer2[(87 * 128 * 4) + (58 * 4) + 3] = 255;

				buffer2[(86 * 128 * 4) + (66 * 4) + 3] = 255;
				buffer2[(86 * 128 * 4) + (67 * 4) + 3] = 255;
				buffer2[(86 * 128 * 4) + (68 * 4) + 3] = 255;

				buffer2[(87 * 128 * 4) + (66 * 4) + 3] = 255;
				buffer2[(87 * 128 * 4) + (67 * 4) + 3] = 255;
				buffer2[(87 * 128 * 4) + (68 * 4) + 3] = 255;
			} else if (GunFlash == 531) {
				buffer2[(90 * 128 * 4) + (50 * 4) + 3] = 103;
				buffer2[(91 * 128 * 4) + (48 * 4) + 3] = 103;

				buffer2[(90 * 128 * 4) + (79 * 4) + 3] = 103;
				buffer2[(91 * 128 * 4) + (81 * 4) + 3] = 103;
				buffer2[(93 * 128 * 4) + (82 * 4) + 3] = 103;
			}

		} else if (GunFlash && (GunFlash == 521)) {
			for ((i = 200); (i < (128 * 128)); ++i) {
				ptr = (buffer2 + (i * 4));

				if (ptr[0] > 135 &&
					ptr[1] >= 0 &&
					ptr[2] >= 0 &&
					ptr[1] < 63 &&
					ptr[2] < 63) {
					ptr[3] = 103;
				}

				if (ptr[0] == 248 &&
					ptr[1] >= 70 &&
					ptr[2] >= 70 &&
					ptr[1] < 157 &&
					ptr[2] < 153 &&
					ptr[2] != 96) {
					ptr[3] = 103;
				}

				if (ptr[0] < 249 &&
					ptr[1] > 230 &&
					ptr[2] > 79 &&
					ptr[0] > 237 &&
					ptr[1] < 253 &&
					ptr[2] < 92) {
					ptr[3] = 103;
				}

				if (ptr[0] == 236 &&
					ptr[1] == 80 &&
					ptr[2] == 77) {
					ptr[3] = 103;
				}

				if (ptr[0] == 247 &&
					ptr[1] == 136 &&
					ptr[2] == 133) {
					ptr[3] = 103;
				}

				if (ptr[0] == 248 &&
					ptr[1] == 100 &&
					ptr[2] == 96) {
					ptr[3] = 103;
				}
			}
		}
#endif /* 0 || MOD_ALPHA_CHANNEL_FOR_GUN_FLASH || __clang_analyzer__ */

		WriteTGA((const char *)filename, (W16)32, (W16)128, (W16)128,
				 (void *)buffer2, (W8)0, (W8)1);
	}

    return 1;
} /* end of PML_Save_GFXPage() (phew, that was a long function!) */

/*
-----------------------------------------------------------------------------
 Function: PML_SaveSoundPage() -Save sound data from Page file.

 Parameters: nPage -[in] Page number to save.
			 filename -[in] Pointer to string with filename.
			 buffer -[in] Allocated memory buffer to hold sound data.
			 size -[in] Size of allocated memory buffer.

 Returns: Non-zero on success, otherwise zero.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE W8 PML_SaveSoundPage(W32 nPage, char *filename, W8 *buffer, W32 size)
{
    static W16 totallength = 0;
    W8 *data;
    W16 clength; /* Chunk length */

    if ((nPage < PMSoundStart) || (nPage > PMNumBlocks)) {
        printf("Out of bounds page number passed into PML_SaveSound()!\n");
        return 1;
    }

    data = (PW8)PML_LoadPage(nPage, &clength);
	if (data == NULL) {
        return 1;
    }

    if (totallength > size) {
        printf( "[wolf_pmc] Buffer not large enough!\n" );
        return 2;
    }

    memcpy((buffer + totallength), data, (size_t)clength);

    totallength += clength;
    if (clength < 4096) {
        write_wav(filename, buffer, (W32)totallength, (W16)1,
				  (W32)SAMPLERATE, (W16)1);
        totallength = 0;
    }

    MM_FREE(data);

    return 0;
}

/*
-----------------------------------------------------------------------------
 Function: PML_Shutdown() -Shutdown page cache.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PML_Shutdown()
{
    if( PageFile ) {
        fclose( PageFile );
        PageFile = NULL;
    }

    if( PMPages ) {
        MM_FREE( PMPages );
    }
}


/*
-----------------------------------------------------------------------------
 Function: PExtractor() -Interface to page manager.

 Parameters: extension -[in] Ponter to string with file extenion of data
							files.

 Returns: Nothing (really? looks like a boolean to me...).

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC _boolean PExtractor(const char *extension, W16 version)
{
    W32 i, j;
    char filename[256];
    W8 *buffer, *buffer2;
	W32 Flash;

/*
 * Setup
 */
	if (0 == FS_Mkdir(GFXWALLDIR)) {
		printf("[%s] Could not create directory (%s)!\n", "PExtractor",
			   GFXWALLDIR);

		return false;
	}

	if ((version == SOD_PAK) || (version == SDM_PAK)) {
		if (0 == FS_Mkdir(SODGFXSPRITEDIR)) {
			printf("[%s] Could not create directory (%s)!\n", "PExtractor",
				   GFXSPRITEDIR );

			return false;
		}

		if (0 == FS_Mkdir(SODSFXDIR)) {
			printf("[%s] Could not create directory (%s)!\n", "PExtractor",
				   SODSFXDIR);

			return false;
		}
	} else {
		if (0 == FS_Mkdir(GFXSPRITEDIR)) {
			printf("[%s] Could not create directory (%s)!\n", "PExtractor",
				   GFXSPRITEDIR);

			return false;
		}

		if (0 == FS_Mkdir(SFXDIR)) {
			printf("[%s] Could not create directory (%s)!\n", "PExtractor",
				   SFXDIR);

			return false;
		}
	}


    if (! PML_OpenPageFile(extension)) {
		PML_Shutdown();

		return false;
    }

/*
 * Allocate buffers
 */
	buffer = MM_MALLOC((size_t)(64 * 64 * 2)); /* (64 * 64 * 2) = 8192 */
	if (buffer == NULL) {
		PML_Shutdown();

		return false;
	}

    buffer2 = MM_MALLOC((size_t)(128 * 128 * 4)); /* (128 * 128 * 4) = 65536 */
	if (buffer2 == NULL) {
		MM_FREE(buffer);
		PML_Shutdown();

		return false;
	}

/*
 * Decode Page data
 */
	printf("PExtractor(): attempting to decode Page Data...\n");

	/* '414' is a bad one for sprites, '134' is a bad one for walls */
	for ((i = 0), (j = 0); (i < PMSpriteStart), (j < 414); ++i, ++j) {
		/* Hacks */
		if ((version == WL6_PAK) || (version == WL1_PAK)) {
			if (98 == j) {
				/* huh? */
				j = 126;
			}
		}

		if (version == 2) {
			/* these two error out: */
			if ((j == 134) || (j == 135)) {
				j = 136;
			}
		}

		cs_snprintf(filename, sizeof(filename), "%s/%.3d.tga", GFXWALLDIR, j);

		PML_SaveGFXPage(i, filename, buffer, buffer2, (_boolean)1, (W32)0);
	}

	/* '521' is the highest hardcoded number in the section below; I am unsure
	 * what the actual max value for 'j' is... */
    for ((i = PMSpriteStart), (j = 0); (i < PMSoundStart), (j <= 521);
		 ++i, ++j) {
		/* Hacks (need documentation) */
		if (version == WL1_PAK) {
			if (j == 50) {
				j = 54;
			}

			if (j == 191) {
				j = 300;
				i += 109;
			}

			if (j == 311) {
				j = 431;
				i += 101;
			}

			if (j == 439) {
				j = 514;
			}
		}

		if (version == WL6_PAK) {
			if (j == 50) {
				j = 54;
			}

			if (j == 389) {
				j = 408;
			}

			if (j == 439) {
				j = 514;
			}
		}

		if (version == SDM_PAK) {
			if (j == 401) {
				j = 514;
			}
		}

		if (version == SOD_PAK) {
			if (j == 292) {
				j = 374;
			}

			if (j == 408) {
				j = 439;
			}
		}

		if (version == 2) {
			/* try to avoid error: */
			if (j == 414) {
				j = 415; /* (might not be necessary any more...) */
			}
		}

		/* this is where the filename actually gets set, so if you want to hack
		 * 'j' to make it skip a particular file, it needs to be done before
		 * this point: */
		if ((version == SOD_PAK) || (version == SDM_PAK)) {
			cs_snprintf(filename, sizeof(filename), "%s/%.3d.tga",
						SODGFXSPRITEDIR, j);
		} else {
			cs_snprintf(filename, sizeof(filename), "%s/%.3d.tga",
						GFXSPRITEDIR, j);
		}

		if ((j == 531) ||
			(j == 532) ||
			(j == 526) ||
			(j == 521)) {
			Flash = j;
		} else {
			Flash = 0;
		}

        PML_SaveGFXPage(i, filename, buffer, buffer2, (_boolean)0, (W32)Flash);
    }

    for ((i = PMSoundStart), (j = 0); (i < (W32)(PMNumBlocks - 1)); ++i, ++j) {
		if ((version == SOD_PAK) || (version == SDM_PAK)) {
			cs_snprintf(filename, sizeof(filename), "%s/%.3d.wav",
						SODSFXDIR, j);
		} else {
			cs_snprintf(filename, sizeof(filename), "%s/%.3d.wav", SFXDIR, j);
		}

        PML_SaveSoundPage(i, filename, buffer2, (W32)(128 * 128 * 4));
		/* (128 * 128 * 4) = 65536 */
    }

/*
 * Shutdown
 */
    MM_FREE(buffer);
    MM_FREE(buffer2);

    PML_Shutdown();

	return true;
} /* end of PExtractor() (another long one!) */

/* EOF */
