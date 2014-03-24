/*
 *	Copyright (C) 2004-2005 Michael Liebscher
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
 *	wolf_map.c:	Decode Wolfenstein 3-D Map data.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Wolfenstein 3-D, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wolf_def.h"

#include "../string/com_string.h"
#include "../loaders/tga.h"
#include "../filesys/file.h"
#include "../../../common/arch.h"
#include "../memory/memory.h"
#include "../../../common/common_utils.h"


#define MAPHEADNAME		"MAPHEAD"
#define MAPNAME			"GAMEMAPS"



PRIVATE FILE *maphandle;


PRIVATE W32	headeroffsets[ 100 ];
PRIVATE W32	TotalMaps;
PRIVATE W16	RLEWtag;

PRIVATE W16 gameversion;


PRIVATE const W32 vgaCeilingWL6[] =
{
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xbfbf,
	0x4e4e,0x4e4e,0x4e4e,0x1d1d,0x8d8d,0x4e4e,0x1d1d,0x2d2d,0x1d1d,0x8d8d,
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x2d2d,0xdddd,0x1d1d,0x1d1d,0x9898,

	0x1d1d,0x9d9d,0x2d2d,0xdddd,0xdddd,0x9d9d,0x2d2d,0x4d4d,0x1d1d,0xdddd,
	0x7d7d,0x1d1d,0x2d2d,0x2d2d,0xdddd,0xd7d7,0x1d1d,0x1d1d,0x1d1d,0x2d2d,
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xdddd,0xdddd,0x7d7d,0xdddd,0xdddd,0xdddd
};

PRIVATE const W32 vgaCeilingSOD[] =
{
	0x6f6f,0x4f4f,0x1d1d,0xdede,0xdfdf,0x2e2e,0x7f7f,0x9e9e,0xaeae,0x7f7f,
	0x1d1d,0xdede,0xdfdf,0xdede,0xdfdf,0xdede,0xe1e1,0xdcdc,0x2e2e,0x1d1d,0xdcdc
};

PRIVATE W32 WL6_songs[] =
{
/*
 * Episode One
 */
	GETTHEM_MUS,
	SEARCHN_MUS,
	POW_MUS,
	SUSPENSE_MUS,
	GETTHEM_MUS,
	SEARCHN_MUS,
	POW_MUS,
	SUSPENSE_MUS,

	WARMARCH_MUS,	/* Boss level */
	CORNER_MUS,		/* Secret level */

/*
 * Episode Two
 */
	NAZI_OMI_MUS,
	PREGNANT_MUS,
	GOINGAFT_MUS,
	HEADACHE_MUS,
	NAZI_OMI_MUS,
	PREGNANT_MUS,
	HEADACHE_MUS,
	GOINGAFT_MUS,

	WARMARCH_MUS,	/* Boss level */
	DUNGEON_MUS,	/* Secret level */

/*
 * Episode Three
 */
	INTROCW3_MUS,
	NAZI_RAP_MUS,
	TWELFTH_MUS,
	ZEROHOUR_MUS,
	INTROCW3_MUS,
	NAZI_RAP_MUS,
	TWELFTH_MUS,
	ZEROHOUR_MUS,

	ULTIMATE_MUS,	/* Boss level */
	PACMAN_MUS,		/* Secret level */

/*
 * Episode Four
 */
	GETTHEM_MUS,
	SEARCHN_MUS,
	POW_MUS,
	SUSPENSE_MUS,
	GETTHEM_MUS,
	SEARCHN_MUS,
	POW_MUS,
	SUSPENSE_MUS,

	WARMARCH_MUS,	/* Boss level */
	CORNER_MUS,		/* Secret level */

/*
 * Episode Five
 */
	NAZI_OMI_MUS,
	PREGNANT_MUS,
	GOINGAFT_MUS,
	HEADACHE_MUS,
	NAZI_OMI_MUS,
	PREGNANT_MUS,
	HEADACHE_MUS,
	GOINGAFT_MUS,

	WARMARCH_MUS,	/* Boss level */
	DUNGEON_MUS,	/* Secret level */

/*
 * Episode Six
 */
	INTROCW3_MUS,
	NAZI_RAP_MUS,
	TWELFTH_MUS,
	ZEROHOUR_MUS,
	INTROCW3_MUS,
	NAZI_RAP_MUS,
	TWELFTH_MUS,
	ZEROHOUR_MUS,

	ULTIMATE_MUS,	/* Boss level */
	FUNKYOU_MUS		/* Secret level */
};


PRIVATE W32 SOD_songs[] =
{
	SOD_XTIPTOE_MUS,
	SOD_XFUNKIE_MUS,
	SOD_XDEATH_MUS,
	SOD_XGETYOU_MUS,	/* DO NOT KNOW */
	SOD_ULTIMATE_MUS,	/* Trans Grosse */

	SOD_DUNGEON_MUS,
	SOD_GOINGAFT_MUS,
	SOD_POW_MUS,
	SOD_TWELFTH_MUS,
	SOD_ULTIMATE_MUS,	/* Barnacle Wilhelm BOSS */

	SOD_NAZI_OMI_MUS,
	SOD_GETTHEM_MUS,
	SOD_SUSPENSE_MUS,
	SOD_SEARCHN_MUS,
	SOD_ZEROHOUR_MUS,
	SOD_ULTIMATE_MUS,	/* Super Mutant BOSS */

	SOD_XPUTIT_MUS,
	SOD_ULTIMATE_MUS,	/* Death Knight BOSS */

	SOD_XJAZNAZI_MUS,	/* Secret level */
	SOD_XFUNKIE_MUS,	/* Secret level (DO NOT KNOW) */

	SOD_XEVIL_MUS		/* Angel of Death BOSS */

};



typedef struct
{
	float time;
	char timestr[6];

} times;


PRIVATE times parTimesWL6[] =
{
	 /*
	  * Episode One Par Times
	  */
	 { (float)1.5,	"01:30" },
	 { (float)2.0,	"02:00" },
	 { (float)2.0,	"02:00" },
	 { (float)3.5,	"03:30" },
	 { (float)3.0,	"03:00" },
	 { (float)3.0,	"03:00" },
	 { (float)2.5,	"02:30" },
	 { (float)2.5,	"02:30" },
	 { (float)0.0,	"??:??" },	/* Boss level */
	 { (float)0.0,	"??:??" },	/* Secret level */

	 /*
	  * Episode Two Par Times
	  */
	 { (float)1.5,	"01:30" },
	 { (float)3.5,	"03:30" },
	 { (float)3.0,	"03:00" },
	 { (float)2.0,	"02:00" },
	 { (float)4.0,	"04:00" },
	 { (float)6.0,	"06:00" },
	 { (float)1.0,	"01:00" },
	 { (float)3.0,	"03:00" },
	 { (float)0.0,	"??:??" },
	 { (float)0.0,	"??:??" },

	 /*
	  * Episode Three Par Times
	  */
	 { (float)1.5,	"01:30" },
	 { (float)1.5,	"01:30" },
	 { (float)2.5,	"02:30" },
	 { (float)2.5,	"02:30" },
	 { (float)3.5,	"03:30" },
	 { (float)2.5,	"02:30" },
	 { (float)2.0,	"02:00" },
	 { (float)6.0,	"06:00" },
	 { (float)0.0,	"??:??" },
	 { (float)0.0,	"??:??" },

	 /*
	  * Episode Four Par Times
	  */
	 { (float)2.0,	"02:00" },
	 { (float)2.0,	"02:00" },
	 { (float)1.5,	"01:30" },
	 { (float)1.0,	"01:00" },
	 { (float)4.5,	"04:30" },
	 { (float)3.5,	"03:30" },
	 { (float)2.0,	"02:00" },
	 { (float)4.5,	"04:30" },
	 { (float)0.0,	"??:??" },
	 { (float)0.0,	"??:??" },

	 /*
	  * Episode Five Par Times
	  */
	 { (float)2.5,	"02:30" },
	 { (float)1.5,	"01:30" },
	 { (float)2.5,	"02:30" },
	 { (float)2.5,	"02:30" },
	 { (float)4.0,	"04:00" },
	 { (float)3.0,	"03:00" },
	 { (float)4.5,	"04:30" },
	 { (float)3.5,	"03:30" },
	 { (float)0.0,	"??:??" },
	 { (float)0.0,	"??:??" },

	 /*
	  * Episode Six Par Times
	  */
	 { (float)6.5,	"06:30" },
	 { (float)4.0,	"04:00" },
	 { (float)4.5,	"04:30" },
	 { (float)6.0,	"06:00" },
	 { (float)5.0,	"05:00" },
	 { (float)5.5,	"05:30" },
	 { (float)5.5,	"05:30" },
	 { (float)8.5,	"08:30" },
	 { (float)0.0,	"??:??" },
	 { (float)0.0,	"??:??" }
};

PRIVATE times parTimesSOD[] =
{
	 /*
	  * SPEAR OF DESTINY TIMES
	  */
	 { (float)1.5,	"01:30" },
	 { (float)3.5,	"03:30" },
	 { (float)2.75,	"02:45" },
	 { (float)3.5,	"03:30" },
	 { (float)0.0,	"??:??" },	/* Boss 1 */
	 { (float)4.5,	"04:30" },
	 { (float)3.25,	"03:15" },
	 { (float)2.75,	"02:45" },
	 { (float)4.75,	"04:45" },
	 { (float)0.0,	"??:??" },	/* Boss 2 */
	 { (float)6.5,	"06:30" },
	 { (float)4.5,	"04:30" },
	 { (float)2.75,	"02:45" },
	 { (float)4.5,	"04:30" },
	 { (float)6.0,	"06:00" },
	 { (float)0.0,	"??:??" },	/* Boss 3 */
	 { (float)6.0,	"06:00" },
	 { (float)0.0,	"??:??" },	/* Boss 4 */
	 { (float)0.0,	"??:??" },	/* Secret level 1 */
	 { (float)0.0,	"??:??" },	/* Secret level 2 */

};


/*
-----------------------------------------------------------------------------
 Function: CAL_SetupMapFile -Setup map files for decoding.

 Parameters: extension -[in] file extension for map data files.

 Returns: Non-zero on success, zero otherwise.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE W8 CAL_SetupMapFile(const char *extension)
{
	FILE *handle;
	SW32 length;
	char fname[13];


/*
 * load maphead.xxx (offsets and tileinfo for map file)
 */
	cs_strlcpy(fname, MAPHEADNAME, sizeof(fname));
	cs_strlcat(fname, extension, sizeof(fname));

	handle = fopen(cs_strupr(fname), "rb");
	if (handle == NULL) {
		handle = fopen(cs_strlwr(fname), "rb");
		if (handle == NULL) {
			printf("Could not open file (%s) for read!\n", fname);
			return 0;
		}
	}

	length = FS_FileLength(handle);


	fread(&RLEWtag, (size_t)2, (size_t)1, handle);

	for ((TotalMaps = 0); ((long)TotalMaps < length); ++TotalMaps) {
		fread(&headeroffsets[TotalMaps], (size_t)4, (size_t)1, handle);
		if (! headeroffsets[TotalMaps]) {
			break;
		}
	}


	fclose(handle);


	cs_strlcpy(fname, MAPNAME, sizeof(fname));
	cs_strlcat(fname, extension, sizeof(fname));

	maphandle = fopen(cs_strupr(fname), "rb");
	if (NULL == maphandle) {
		maphandle = fopen(cs_strlwr(fname), "rb");
		if (NULL == maphandle) {
			return 0;
		}
	}

	return 1;
}

/*
-----------------------------------------------------------------------------
 Function: CAL_ShutdownMapFile -Shutdown map file.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Basically a simple wrapper around fclose() for now.
-----------------------------------------------------------------------------
*/
PRIVATE void CAL_ShutdownMapFile(void)
{
	if (maphandle) {
		fclose(maphandle);
	}
}



/*
-----------------------------------------------------------------------------
 Function: CA_CacheMap -Cache and save map data.

 Parameters:
			ChunkOffset -[in] Chunk offset.
			Chunklength -[in] Length of chunk.
			filename -[in] File name to save map as.
			index_number -[in] File name index number (used to be just 'index').

 Returns: Non-zero on success, otherwise zero.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE W8 CA_CacheMap(W32 ChunkOffset, W32 Chunklength, const char *filename,
					   W32 index_number)
{
	W32 offset[3];
	W32 offsetin[3];
	W32 temp;
	W16 length[3];
	W8 sig[5];
	W16 w, h;
	W8 *data;
	W32 ceiling;
	W32 floor;
	FILE *fout;
	float ftime;
	char *stime;
	char name[32];
	char musicName[64];
	extern char gamepal[];
	SW32 jmp;

	/* dummy condition to use 'Chunklength' parameter: */
	if (Chunklength == 0) {
		;
	}

	if (gameversion == SOD_PAK) {
		temp = (vgaCeilingSOD[index_number] & 0xff) * 3;
		ceiling = ((W32)((gamepal[temp] << 2) << 16) |
				   ((W32)(gamepal[(temp + 1)] << 2) << 8) |
				   (W32)(gamepal[(temp + 2)] << 2));

		temp = (0x19 * 3);
		floor = ((W32)((gamepal[temp] << 2) << 16) |
				 ((W32)(gamepal[(temp + 1)] << 2) << 8) |
				 (W32)(gamepal[(temp + 2)] << 2));

		ftime = parTimesSOD[index_number].time;
		stime = parTimesSOD[index_number].timestr;

		cs_snprintf(musicName, sizeof(musicName), "music/%s.ogg",
					GetMusicFileName_SOD(SOD_songs[index_number]));
	} else {
		temp = ((vgaCeilingWL6[index_number] & 0xff) * 3);
		ceiling = ((W32)((gamepal[temp] << 2)  << 16) |
				   ((W32)(gamepal[(temp + 1)] << 2) << 8) |
				   (W32)(gamepal[(temp + 2)] << 2));

		temp = (0x19 * 3);
		floor = ((W32)((gamepal[temp] << 2)  << 16) |
				 ((W32)(gamepal[(temp + 1)] << 2) << 8) |
				 (W32)(gamepal[(temp + 2)] << 2));

		ftime = parTimesWL6[index_number].time;
		stime = parTimesWL6[index_number].timestr;

		cs_snprintf(musicName, sizeof(musicName), "music/%s.ogg",
					GetMusicFileName_WL6(WL6_songs[index_number]));
	}

	fout = fopen(filename, "wb");
	if (NULL == fout) {
		return 0;
	}


	fseek(maphandle, (long)ChunkOffset, SEEK_SET);


	fread(&offsetin, (size_t)sizeof(W32), (size_t)3, maphandle);
	fread(&length, (size_t)sizeof(W16), (size_t)3, maphandle);

	fread(&w, (size_t)sizeof(W16), (size_t)1, maphandle);
	fread(&h, (size_t)sizeof(W16), (size_t)1, maphandle);


	fread(name, (size_t)sizeof(W8), (size_t)16, maphandle);
	fread(sig, (size_t)sizeof(W8), (size_t)4, maphandle);

/*
 * Output header
 */

	/* Map file header signature */
	fwrite(sig, (size_t)sizeof(W8), (size_t)4, fout);

	/* RLE Word tag */
	fwrite(&RLEWtag, (size_t)sizeof(W16), (size_t)1, fout);

	/* Max Width */
	fwrite(&w, (size_t)sizeof(W16), (size_t)1, fout);

	/* Max Height */
	fwrite(&h, (size_t)sizeof(W16), (size_t)1, fout);

	/* Ceiling Colour */
	fwrite(&ceiling, (size_t)sizeof(W32), (size_t)1, fout);

	/* Floor Colour */
	fwrite(&floor, (size_t)sizeof(W32), (size_t)1, fout);

	/* Length of layers */
	temp = length[0];
	fwrite(&temp, (size_t)sizeof(W16), (size_t)1, fout);	/* Length One */
	temp = length[1];
	fwrite(&temp, (size_t)sizeof(W16), (size_t)1, fout);	/* Length Two */
	temp = length[2];
	fwrite(&temp, (size_t)sizeof(W16), (size_t)1, fout);	/* Length Three */

	jmp = ftell(fout);

	temp = 0;
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset One */
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset Two */
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset Three */


	/* Map name length */
	temp = strlen(name);
	fwrite(&temp, (size_t)sizeof(W16), (size_t)1, fout);

	/* Music name length */
	temp = strlen( musicName );
	fwrite(&temp, (size_t)sizeof(W16), (size_t)1, fout);

	/* Par time Float */
	fwrite(&ftime, (size_t)sizeof(float), (size_t)1, fout);

	/* Par time string */
	fwrite(stime, (size_t)sizeof(W8), (size_t)5, fout);

	/* Map name */
	fwrite(name, (size_t)sizeof(W8), (size_t)strlen(name), fout);

	/* Music file name */
	fwrite(musicName, (size_t)sizeof(W8), (size_t)strlen(musicName), fout);


	data = MM_MALLOC(length[0]);
	if (data == NULL) {
		return 0;
	}

	offset[0] = (W32)(ftell(fout));

	fseek(maphandle, (long)(offsetin[0]), SEEK_SET);
	fread(data, (size_t)1, (size_t)length[0], maphandle);

	fwrite(data, (size_t)1, (size_t)length[0], fout);


	data = MM_REALLOC(data, length[1]);
	if (data == NULL) {
		printf("CA_CacheMap(): realloced null data, returning 0...\n");
		return 0;
	}

	offset[1] = (W32)(ftell(fout));

	fseek(maphandle, (long)(offsetin[1]), SEEK_SET);
	fread(data, (size_t)1, (size_t)length[1], maphandle);

	fwrite(data, (size_t)1, (size_t)length[1], fout);


	data = MM_REALLOC(data, length[2]);
	if (data == NULL) {
		printf("CA_CacheMap(): realloced null data, returning 0...\n");
		return 0;
	}

	offset[2] = (W32)(ftell(fout));

	fseek(maphandle, (long)(offsetin[2]), SEEK_SET);
	fread(data, (size_t)1, (size_t)length[2], maphandle);

	fwrite(data, (size_t)1, (size_t)length[2], fout);

	MM_FREE(data);


	fseek(fout, jmp, SEEK_SET);

	temp = offset[0];
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset One */

	temp = offset[1];
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset Two */

	temp = offset[2];
	fwrite(&temp, (size_t)sizeof(W32), (size_t)1, fout);	/* Offset Three */


	fclose(fout);

	return 1;
}


/*
-----------------------------------------------------------------------------
 Function: MapRipper -Re-encode map data.

 Parameters: extension -[in] file extension for map data files.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC _boolean MapRipper(const char *fextension, W16 version)
{
	W32 i;
	char fname[32];

	gameversion = version;

/*
 * Setup
 */

	if (0 == FS_Mkdir(MAPDIR)) {
		printf("[%s] Could not create directory (%s)!\n", "wolf_map.c", MAPDIR);

		return false;
	}


	if (! CAL_SetupMapFile(fextension)) {
		CAL_ShutdownMapFile();

		return false;
	}

/*
 * Decode Map data
 */

	printf("Decoding Map Data...\n");

	for ((i = 0); (i < TotalMaps); ++i) {
		cs_snprintf(fname, sizeof(fname), "%s/%c%.2d.map", MAPDIR,
					TOLOWER(fextension[1]), i);

		CA_CacheMap(headeroffsets[i],
					(headeroffsets[(i + 1)] - headeroffsets[i]),
					fname, i);

	}

/*
 * Shutdown
 */

	CAL_ShutdownMapFile();

	return true;
}

/* EOF */
