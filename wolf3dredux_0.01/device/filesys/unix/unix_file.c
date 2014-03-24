/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
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
 *	unix_file.c:   Handles non-portable file services.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


#include "glob.h"
#include "../filesystem.h"
#include "../../../common/arch.h"
#include "../../../common/common.h" /* for Com_Printf() */
#include "../../../common/common_utils.h"
#include "../../../string/com_string.h"



PRIVATE	char	findbase[ 128 ];
PRIVATE	char	findpath[ 128 ];
PRIVATE	char	findpattern[ 128 ];
PRIVATE	DIR     *fdir;


/*
-----------------------------------------------------------------------------
 Function: FS_CreateDirectory() -Creates a new directory.

 Parameters: dirname -[in] Pointer to a NUL-terminated string that specifies
                      the path of the directory to be created.

 Returns: On success nonzero, otherwise zero.

 Notes: basically a simple wrapper around mkdir() for now.
-----------------------------------------------------------------------------
*/
PUBLIC W8 FS_CreateDirectory(const char *dirname)
{
	int ret_val;

	ret_val = mkdir(dirname, (mode_t)(S_IRUSR | S_IWUSR | S_IXUSR));

	if ((ret_val == -1) && (errno == EEXIST)) {
		return 1;
	}

	return  (W8)(! ret_val);
}

/*
-----------------------------------------------------------------------------
 Function: FS_ChangeCurrentDirectory() -Changes the current directory

 Parameters: path -[in] Pointer to a NUL-terminated string that specifies
                      the path to the new directory.

 Returns: On success nonzero, otherwise zero.

 Notes:
-----------------------------------------------------------------------------
*/
/* TODO: put this function in a shared library */
#ifndef FS_ChangeCurrentDirectory
PUBLIC W8 FS_ChangeCurrentDirectory( const char *path )
#else
PUBLIC W8 unix_file_FS_ChangeCurrentDirectory( const char *path )
#endif /* !FS_ChangeCurrentDirectory */
{
	return ! chdir( path );
}

/*
-----------------------------------------------------------------------------
 Function: CompareAttributes() -Compare directory and file attributes.

 Parameters: path -[in] Specifies the path to compare file attributes.
			 musthave -[in] File or directory must have these attributes.
			 canthave- [in] File or directory can not have these attributes.

 Returns: On success true, otherwise false.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean CompareAttributes( const char *path, W32 musthave, W32 canthave )
{
	struct stat st;

	if( stat( path, &st ) == -1 ) {
		return false;
	}

	if( ( st.st_mode & S_IFDIR ) && ( canthave & FA_DIR ) ) {
		return false;
	}

	if( ( musthave & FA_DIR ) && !( st.st_mode & S_IFDIR ) ) {
		return false;
	}

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FindFirstFile() -Searches a directory for a file.

 Parameters: path -[in] Pointer to a NUL-terminated string that specifies
                      a valid directory or path and file name.
			musthave -[in] File or directory must have these attributes.
			canthave -[in] File or directory can not have these attributes.

 Returns: On success string of file name or directory, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC char *FS_FindFirst(const char *path, W32 musthave, W32 canthave)
{
	struct dirent *d;
	char *p;

	p = ""; /* initialize */

#if __clang_analyzer__
	Com_DPrintf("FS_FindFirst(%s)\n", p);
#endif /* __clang_analyzer__ (hack) */

	if (fdir) {
		Com_Printf("FS_FindFirst without close\n");

		return NULL;
	}

	/* again, about wishing for those per-target breakpoints in Xcode 3.2.6...
	 * this is another place where placing breakpoints for this target can
	 * confuse Xcode when debugging the other target (wolfextractor), which does
	 * not even include this file, but it does include one with the same name
	 * that has the same functions, so Xcode gets confused... darn duplicated
	 * code... */
	FS_FilePath((char *)path, findbase);
	my_strlcpy(findpattern, (char *)FS_SkipPath((char *)path),
			   sizeof(findpattern)); /* FS_SkipPath() is not worth stepping into */

	if (! *findbase) {
		if ((fdir = opendir(".")) == NULL) {
			return NULL;
		}
	} else {
		if ((fdir = opendir(findbase)) == NULL) {
			return NULL;
		}
	}

	while ((d = readdir(fdir)) != NULL) {
		if ((! *findpattern) || glob_match(findpattern, d->d_name)) {
			if (! *findbase) {
				my_strlcpy(findpath, d->d_name, sizeof(findpath));
			} else {
				my_snprintf(findpath, sizeof(findpath), "%s/%s", findbase,
							d->d_name);
			}

			if (CompareAttributes(findpath, musthave, canthave)) {
				return findpath;
			}
		}
	}

	return NULL;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FindNext -Continues a file search from a previous call to
						the FS_FindFirst function.

 Parameters: musthave -[in] File or directory must have these attributes.
			 canthave- [in] File or directory can not have these attributes.

 Returns: On success string of file name or directory, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC char *FS_FindNext(W32 musthave, W32 canthave)
{
	struct dirent *d;

	if (fdir == NULL) {
		return NULL;
	}

	while ((d = readdir(fdir)) != NULL) {
		if (! *findpattern || glob_match(findpattern, d->d_name)) {
			if (! *findbase) {
				my_strlcpy(findpath, d->d_name, sizeof(findpath));
			} else {
				my_snprintf(findpath, sizeof(findpath), "%s/%s", findbase,
							d->d_name);
			}

			if (CompareAttributes(findpath, musthave, canthave)) {
				return findpath;
			}
		}
	}

	return NULL;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FindClose() -Closes the search handle.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Just a simple wrapper around closedir() for now...
-----------------------------------------------------------------------------
*/
PUBLIC void FS_FindClose(void)
{
	if (fdir) {
		closedir(fdir);
	}

	fdir = NULL;
}

/*
-----------------------------------------------------------------------------
 Function: FS_DeleteFile() -Deletes an existing file.

 Parameters: filename -[in] Pointer to a NUL-terminated string that
							specifies the file to be deleted.

 Returns: If successful the return value is nonzero, otherwise zero.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC _boolean FS_DeleteFile( const char *filename )
{
	return( ! unlink( filename ) );
}

/*
-----------------------------------------------------------------------------
 Function: FS_RemoveDirectory() -Deletes an existing empty directory.

 Parameters: pathname -[in] Pointer to a NUL-terminated string that
							specifies the directory to be deleted.

 Returns: If successful the return value is nonzero, otherwise zero.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC _boolean FS_RemoveDirectory( const char *pathname )
{
	return( ! rmdir( pathname ) );
}

/* EOF */
