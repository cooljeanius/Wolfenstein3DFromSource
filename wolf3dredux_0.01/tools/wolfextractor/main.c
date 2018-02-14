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
 *	main.c:   Decode Wolfenstein 3-D data files.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 */


/*
 *
 * This program decodes the following Wolfenstein 3-D/Spear of Destiny files:
 *
 * AUDIOHED.xxx 	-Contains offsets and lengths of the audio in AUDIOT.
 * AUDIOT.xxx		-Contains Adlib audio data.
 *
 * VGAHEAD.xxx		-Contains data offsets for VGAGRAPH.
 * VGAGRAPH.xxx 	-Contains graphic data.
 * VGADICT.xxx		-Contains Huffman dictionary data.
 *
 * VSWAP.xxx		-Contains audio and graphic data.
 *
 * Wolfenstein 3D(tm).mac -Macintosh binary of Wolfenstein 3D
 *
 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>
#include <errno.h>


#include "../../common/arch.h"
#include "../../common/common_utils.h"
#include "string/com_string.h"
#include "wolf/wolf_def.h"
#include "filesys/file.h"
#include "hq2x.h"
#include "mac/mac.h"

#include "wolfextractor.h" /* new location for prototypes for functions from this file */


#define APP_VERSION "0.01k"

#define BASEDIR	"base/" /* why is this hardcoded? FIXME */


extern void PAK_builder(const char *filename, W16 version);


/*
-----------------------------------------------------------------------------
 Function: deleteCacheDirectories -Delete directories created when caching.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void deleteCacheDirectories(W16 version)
{
	FS_RemoveDirectory(GFXWALLDIR);
	FS_RemoveDirectory(MAPDIR);
	FS_RemoveDirectory(LGFXDIR);

	if ((version & WL1_PAK) || (version & WL6_PAK)) {
		FS_RemoveDirectory(SFXDIR);
		FS_RemoveDirectory(GFXSPRITEDIR);
		FS_RemoveDirectory(LSFXDIR);
	}

	if ((version & SDM_PAK) || (version & SOD_PAK)) {
		FS_RemoveDirectory(SODSFXDIR);
		FS_RemoveDirectory(SODGFXSPRITEDIR);
		FS_RemoveDirectory(SODLSFXDIR);
	}
}

/*
-----------------------------------------------------------------------------
 Function: CheckForDataFiles -Check for data files.

 Parameters: Wolf_Ext -[out] zero nothing found.
						Bit 0 high -WL1 data files found.
						Bit 1 high -WL6 data files found.
						Bit 2 high -SDM data files found.
						Bit 3 high -SOD data files found.
						Bit 4 high -MAC data files found.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype has moved to "wolfextractor.h" */
void CheckForDataFiles(W16 *Wolf_Ext)
{
	char ext[13];

/*
 *	Wolfenstein 3-D	Demo
 */
	cs_strlcpy(ext, WL1_FEXT, sizeof(ext));

    if (FS_FindFirst(cs_strupr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= WL1_PAK;
    }

	FS_FindClose();

	if (FS_FindFirst(cs_strlwr(ext), (W32)0, (W32)0)) {
		*Wolf_Ext |= WL1_PAK;
	}

	FS_FindClose();


/*
 *	Wolfenstein 3-D
 */
	cs_strlcpy(ext, WL6_FEXT, sizeof(ext));

    if (FS_FindFirst(cs_strupr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= WL6_PAK;
    }

	FS_FindClose();

	if (FS_FindFirst(cs_strlwr(ext), (W32)0, (W32)0)) {
		*Wolf_Ext |= WL6_PAK;
	}

	FS_FindClose();


/*
 *	Spear of Destiny Demo
 */
    cs_strlcpy(ext, SDM_FEXT, sizeof(ext));

    if (FS_FindFirst(cs_strupr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= SDM_PAK;
    }

	FS_FindClose();

	if (FS_FindFirst(cs_strlwr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= SDM_PAK;
    }

    FS_FindClose();


/*
 *	Spear of Destiny
 */
    cs_strlcpy(ext, SOD_FEXT, sizeof(ext));

    if (FS_FindFirst(cs_strupr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= SOD_PAK;
    }

	FS_FindClose();

	if (FS_FindFirst(cs_strlwr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= SOD_PAK;
    }

    FS_FindClose();

/*
 *	Macintosh Wolfenstein 3-D
 */
	cs_strlcpy(ext, MAC_FEXT, sizeof(ext));

    if (FS_FindFirst(cs_strupr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= MAC_PAK;
    }

	FS_FindClose();

	if (FS_FindFirst(cs_strlwr(ext), (W32)0, (W32)0)) {
        *Wolf_Ext |= MAC_PAK;
    }

    FS_FindClose();
}

/*
-----------------------------------------------------------------------------
 Function: main -Interface to Wolfenstein data decoder.

 Parameters: Nothing.

 Returns: 0 on success, non-zero otherwise.

 Notes: This is the application entry point.

		1. Search for Wolfenstein data files.
		2. Decode data accordingly.
		3. TODO: only write data to new files if output file does NOT already
			exist, or if the '--force' ('-f') flag was passed...
-----------------------------------------------------------------------------
*/
#ifndef main /* this ifdef is bad */
int main(int argc, char *argv[])
#else
int wolfextractor_main(int argc, char *argv[])
#endif /* !main */
{
	W16 Wolf_Ext = 0;	/* bit 0 high -WL1 extension
						 * bit 1 high -WL6 extension
						 * bit 2 high -SDM extension
						 * bit 3 high -SOD extension
						 * bit 4 high -MAC extension
						 ***************************/
	W32 retval;

	char currentdir[MAXPATHLEN];

#if 0 || 1
	extern char gamepal[];

	int test;
	test = (0x32 * 3);
	printf("running gamepal startup test first...\n");
	printf("%d, %d, %d\n", gamepal[test]<<2, gamepal[(test + 1)] << 2,
		   gamepal[(test + 2)] << 2);
# ifdef RETURN_EARLY
	return 0;
# else
	printf("now back to your regularly scheduled program...\n");
# endif /* RETURN_EARLY */
#endif /* 0 || 1 */

	printf("\n WolfExtractor %s %s\n", BUILDSTRING, CPUSTRING);
	printf("running executable path %s with %i argument(s)\n", argv[0], argc);
	printf("Version %s built on %s at %s\n\n", APP_VERSION, __DATE__, __TIME__);

	if (getcwd(currentdir, (size_t)MAXPATHLEN) == NULL) {
		printf("getcwd() failed; errno: %d (%s)", errno, strerror(errno));
		/* FIXME: print more specific stuff for each of the errno values */
		return -1;
	}

	if (! FS_ChangeCurrentDirectory(BASEDIR)) {
		printf("Unable to change into directory (%s)\n", BASEDIR);
		printf("(this would be coming from %s)\n", currentdir);

		return 1;
	}


    CheckForDataFiles(&Wolf_Ext);
    if (! Wolf_Ext) {
        printf("No Wolfenstein data files found!\n");

        return 1;
    }

    InitLUTs(); /* This is for hq2x */

	if (Wolf_Ext & WL1_PAK) {
		printf("\n Found Wolfenstein 3-D demo data files \n");

		retval = 0;

		retval += LumpExtractor((WL1_FEXT + 1),
								(W32)(WL1_LATCHPICS_LUMP_END + 5),
								(W16)WL1_PAK);
		retval += PExtractor((WL1_FEXT + 1), (W16)WL1_PAK);
		retval += AudioRipper((WL1_FEXT + 1), (W32)87, (W32)174, (W16)WL1_PAK);
		retval += MapRipper((WL1_FEXT + 1), (W16)WL1_PAK);

		if (retval != 4) {
			printf("\n wolfextractor: main(): An error occurred in one of the extraction steps, exiting now!\n");
#ifdef DEBUG
			printf(" (accumulated retval was '%i')\n", (int)retval);
#endif /* DEBUG */

			printf("\n");
			return 1;
		}

		PAK_builder("wolf_demo.pak", (W16)WL1_PAK);
    }


    if (Wolf_Ext & WL6_PAK) {
		printf("\n Found Wolfenstein 3-D data files here, trying to extract them...\n");

		retval = 0;

		retval += LumpExtractor((const char *)(WL6_FEXT + 1),
								(W32)WL6_LATCHPICS_LUMP_END, (W16)WL6_PAK);
#ifdef I_HATE_MYSELF
		retval += PExtractor((const char *)(WL6_FEXT + 1), (W16)WL6_PAK);
#else /* I do NOT hate myself: */
		retval++;
#endif /* I_HATE_MYSELF */
		retval += AudioRipper((WL6_FEXT + 1), (W32)87, (W32)174, (W16)WL6_PAK);
		retval += MapRipper((WL6_FEXT + 1), (W16)WL6_PAK);

		if (retval != 4) {
			printf("\n wolfextractor: main(): An error occurred in one of the extraction steps, exiting now!\n");
#ifdef DEBUG
			printf(" (accumulated retval was '%i')\n", (int)retval);
#endif /* DEBUG */

			printf("\n");
			return 1;
		}

		PAK_builder("wolf.pak", (W16)WL6_PAK);
    }

	if (Wolf_Ext & SDM_PAK) {
		printf("\n Found Spear of Destiny demo data files \n");

		retval = 0;

		retval += LumpExtractor((SDM_FEXT + 1), (W32)127, (W16)SDM_PAK);
		retval += PExtractor((SDM_FEXT + 1), (W16)SDM_PAK);
		retval += AudioRipper((SDM_FEXT + 1), (W32)81, (W32)162, (W16)SDM_PAK);
		retval += MapRipper((SDM_FEXT + 1), (W16)SDM_PAK);

		if (retval != 4) {
			printf("\n wolfextractor: main(): An error occurred in one of the extraction steps, exiting now!\n");
#ifdef DEBUG
			printf(" (accumulated retval was '%i')\n", (int)retval);
#endif /* DEBUG */

			printf("\n");
			return 1;
		}

		return (int)retval;

		PAK_builder("spear_demo.pak", (W16)SDM_PAK);
    }

	if (Wolf_Ext & SOD_PAK) {
		printf("\n Found Spear of Destiny data files \n");

		retval = 0;

		retval += LumpExtractor((SOD_FEXT + 1), (W32)149, (W16)SOD_PAK);
		retval += PExtractor((SOD_FEXT + 1), (W16)SOD_PAK);
		retval += AudioRipper((SOD_FEXT + 1), (W32)81, (W32)162, (W16)SOD_PAK);
		retval += MapRipper((SOD_FEXT + 1), (W16)SOD_PAK);

		if (retval != 4) {
			printf("\n wolfextractor: main(): An error occurred in one of the extraction steps, exiting now!\n");
#ifdef DEBUG
			printf(" (accumulated retval was '%i')\n", (int)retval);
#endif /* DEBUG */

			printf("\n");
			return 1;
		}

		PAK_builder("spear.pak", (W16)SOD_PAK);
    }

	if (Wolf_Ext & MAC_PAK) {
		printf("\n Found Macintosh binary file \n");

		ripMac();

#if 0 || __clang_analyzer__ || __MACOS_CLASSIC__
		PAK_builder("mac.pak", (W16)MAC_PAK);
#endif /* 0 || __clang_analyzer__ || __MACOS_CLASSIC__ */
    }

	deleteCacheDirectories((W16)Wolf_Ext);

	return 0;
}

/* EOF */

