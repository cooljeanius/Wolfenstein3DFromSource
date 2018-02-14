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
 *      zippak.c:   Parse zip file
 *
 *      Author:  Michael Liebscher    <johnnycanuck@users.sourceforge.net>
 *      Date:    2004
 *
 */

/*
 *	Notes:
 *	This zip parser is done for speed and so it makes a couple of assumptions.
 *	1. CRC of the uncompressed data is not checked.
 *	2. Assumes length of uncompressed data value is accurate (for now).
 *	3. Only parses the local file headers (central directory headers are
 *		redundant for what this module supports and end of central directory
 *		record is useless).
 *
 *	Reference document:
 *	http://www.pkware.com/company/standards/appnote/appnote.txt
 *
 */

#include <stdio.h>
#include <string.h>

#include "zippak.h"
#include "../../common/arch.h"
#include "../../common/common.h"
#include "../../string/com_string.h"


/* Signatures for zip file information headers */
#define SIG_LOCAL			0x04034b50L
#define SIG_CENTRAL			0x02014b50L
#define SIG_END				0x06054b50L
#define SIG_EXTENDLOCAL		0x08074b50L
#define SIG_EXTENDSPLOCAL	0x30304b50L




/* Length of header (not counting the signature) */
#define LOCALHEAD_SIZE		26
#define CENTRALHEAD_SIZE	42
#define ENDHEAD_SIZE		18


typedef struct localzipheader_s
{
	W16 version_needed;
	W16 flag;
	W16 compression_method;
	W32	timedate;
	W32 crc32;
	W32 compressed_size;
	W32 uncompressed_size;
	W16 filename_length;
	W16 extrafield_length;

} localzipheader_t;

/*
-----------------------------------------------------------------------------
 Function: checkstring -check zip file name string.

 Parameters: string -[in] string to check.

 Returns: false if not acceptable, otherwise true.

 Notes:
	Changes '\' to the more acceptable '/'
-----------------------------------------------------------------------------
*/
/* TODO: actually use this function: */
PRIVATE _boolean checkstring(char *string)
{
	char *s = string;

	while (*s) {
		if (*s == ':') {
			/* absolute path given */
			return false;
		} else if (*s == '\\') {
			*s = '/';
		}

		++s;
	}

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: FS_LoadZipFile -Parse a zip pack file.

 Parameters: packfile -[in] Name of packfile to parse.

 Returns: NULL on error, otherwise a link list of pack files.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC pack_t *FS_LoadZipFile(const char *packfile)
{
	W32 ziphead = 0;
	char tempfilename[MAX_GAMEPATH];
	localzipheader_t zlocalhead;
	FILE	*packhandle;
	pack_t *pack;
	W32		numberofpackedfiles;
	packfile_t *pknewnode;
	packfile_t *pkrootnode;
	W16 temp16;
	W32 temp32;
	W32 ret;

	/* initializations: */
	numberofpackedfiles = 0;
	pknewnode = NULL;
	pkrootnode = NULL;
	/* end of initializations */

	packhandle = fopen(packfile, "rb");
	if (! packhandle) {
		Com_Printf("Unable to open pack file (%s)\n", packfile);

		return NULL;
	}

	/*	Scan local header information */
	for (;;) {
		ret = fread(&ziphead, (size_t)1, (size_t)4, packhandle);
		if (ret != 4) {
			Com_DPrintf("[FS_LoadZipFile]: Read error on file (%s)\n", packfile);
			break;
		}

		/* Only parse local file headers */
		if (ziphead != SIG_LOCAL) {
			break;
		}

		memset(&zlocalhead, 0, sizeof(zlocalhead));

		/* Read in local header */
		ret = fread(&temp16, (size_t)1, (size_t)2, packhandle);
		zlocalhead.version_needed = LittleShort(temp16);

		ret += fread(&temp16, (size_t)1, (size_t)2, packhandle);
		zlocalhead.flag = LittleShort(temp16);

		ret += fread(&temp16, (size_t)1, (size_t)2, packhandle);
		zlocalhead.compression_method = LittleShort(temp16);

		ret += fread(&temp32, (size_t)1, (size_t)4, packhandle);
		zlocalhead.timedate = LittleLong(temp32);

		ret += fread(&temp32, (size_t)1, (size_t)4, packhandle);
		zlocalhead.crc32 = LittleLong(temp32);

		ret += fread(&temp32, (size_t)1, (size_t)4, packhandle);
		zlocalhead.compressed_size = LittleLong(temp32);

		ret += fread(&temp32, (size_t)1, (size_t)4, packhandle);
		zlocalhead.uncompressed_size = LittleLong(temp32);

		ret += fread(&temp16, (size_t)1, (size_t)2, packhandle);
		zlocalhead.filename_length = LittleShort(temp16);

		ret += fread(&temp16, (size_t)1, (size_t)2, packhandle);
		zlocalhead.extrafield_length = LittleShort(temp16);

		if (ret != LOCALHEAD_SIZE) {
			Com_DPrintf("[FS_LoadZipFile]: Read error on local header in file (%s)\n",
						packfile);
			break;
		}

		/* This maybe a directory listing */
		if (zlocalhead.compressed_size == 0) {
			/* seek past filename + extra field */
			ret = (W32)fseek(packhandle,
							 (long)(zlocalhead.filename_length +
									zlocalhead.extrafield_length),
							 SEEK_CUR);

			if (ret != 0) {
				Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n",
							packfile);
				break;
			}

			continue;
		}

		if ((zlocalhead.compression_method != CM_NO_COMPRESSION) &&
			(zlocalhead.compression_method != CM_DEFLATED)) {
			Com_DPrintf("[FS_LoadZipFile] Compression Level not supported\n");

			/* seek past filename + extra field + file data */
			ret = (W32)fseek(packhandle,
							 (long)(zlocalhead.filename_length +
									zlocalhead.extrafield_length +
									zlocalhead.compressed_size),
							 SEEK_CUR);

			if (ret != 0) {
				Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
				break;
			}

			continue;
		}

		if ((zlocalhead.filename_length >= sizeof(tempfilename)) ||
			(zlocalhead.filename_length == 0)) {
			Com_DPrintf("[FS_LoadZipFile]: Invalid file name length\n");

			/* seek past filename + extra field + file data */
			ret = (W32)fseek(packhandle,
							 (long)(zlocalhead.filename_length +
									zlocalhead.extrafield_length +
									zlocalhead.compressed_size),
							 SEEK_CUR);

			if (ret != 0) {
				Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n",
							packfile);
				break;
			}

			continue;
		}

		/* Allocate new packfile node */
		pknewnode = Z_Malloc(sizeof(packfile_t));

		/* Read in file name */
		ret = fread(&tempfilename, (size_t)1,
					(size_t)zlocalhead.filename_length, packhandle);
		if (ret != zlocalhead.filename_length) {
			Com_DPrintf("[FS_LoadZipFile]: Read error on file name in file (%s)\n",
						packfile);
			break;
		}

		tempfilename[zlocalhead.filename_length] = '\0'; /* NUL-terminate string */

#if 0
		if (! checkstring(tempfilename)) {
			Com_DPrintf("[FS_LoadZipFile]: Invalid file name\n");

			/* seek past extra field + file data */
			ret = (W32)fseek(packhandle,
							 (long)(zlocalhead.extrafield_length +
									zlocalhead.compressed_size),
							 SEEK_CUR );

			if (ret != 0) {
				Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n",
							packfile);
				break;
			}

			continue;
		}
#endif /* 0 */

		my_strlcpy(pknewnode->name, tempfilename, sizeof(pknewnode->name));

		pknewnode->hashid = my_strhash( pknewnode->name );

		pknewnode->compression_method = zlocalhead.compression_method;
		pknewnode->uncompressed_length = zlocalhead.uncompressed_size;
		pknewnode->filelength = zlocalhead.compressed_size;

		/* seek past extra field */
		ret = (W32)fseek(packhandle, (long)zlocalhead.extrafield_length,
						 SEEK_CUR);
		if (ret != 0) {
			Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n",
						packfile);
			break;
		}

		/* Get file data offset */
		pknewnode->fileoffset = (W32)ftell(packhandle);


		/* Link into file list */
		pknewnode->next = pkrootnode;
		pkrootnode = pknewnode;


		/* seek past file data to next local file header */
		ret = (W32)fseek(packhandle,
						 (long)(zlocalhead.extrafield_length +
								zlocalhead.compressed_size),
						 SEEK_CUR);
		if (ret != 0) {
			Com_DPrintf("[FS_LoadZipFile]: Seek error in file (%s)\n",
						packfile);
			break;
		}


		++numberofpackedfiles;
	}

	if (numberofpackedfiles == 0) {
		Com_Printf("No file(s) added from packfile (%s)\n", packfile);

		return NULL;
	}


	pack = Z_Malloc(sizeof(pack_t));
	my_strlcpy(pack->filename, packfile, sizeof(pack->filename));
	pack->handle = packhandle;
	pack->numfiles = numberofpackedfiles;
	pack->files = pkrootnode;


	Com_Printf("Added packfile %s (%i files)\n", packfile, numberofpackedfiles);

	return pack;
}

/* EOF */
