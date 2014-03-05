/*

	Copyright (C) 2004 Michael Liebscher

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
 *	win_file.c:   Handles non-portable file services.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *
 */

#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "../../../string/com_string.h"
#include "../../../common/arch.h"
#include "../../../common/common_utils.h"

#include "../filesystem.h"



PRIVATE char    findbase[ MAX_PATH ];
PRIVATE char    findpath[ MAX_PATH ];
PRIVATE HANDLE  FindHandle;


/*
-----------------------------------------------------------------------------
 Function: FS_CreateDirectory() -Creates a new directory.

 Parameters: dirname -[in] Pointer to a NUL-terminated string that specifies
                      the path of the directory to be created.

 Returns: On success nonzero, otherwise zero.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC W8 FS_CreateDirectory( const char *dirname )
{
	BOOL ret_val = CreateDirectory( dirname, NULL );

	if( ret_val == 0 && GetLastError() == ERROR_ALREADY_EXISTS )
	{
		return 1;
	}

    return (W8)ret_val;
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
PUBLIC W8 FS_ChangeCurrentDirectory( const char *path )
{
	return SetCurrentDirectory( path );
}


/*
-----------------------------------------------------------------------------
 Function: CompareAttributes() -Compare directory and file attributes.

 Parameters: found -[in] Specifies the file attributes of the file found.
			 musthave -[in] File or directory must have these attributes.
			 canthave- [in] File or directory can not have these attributes.

 Returns: On success true, otherwise false.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean CompareAttributes( W32 found, W32 musthave, W32 canthave )
{
	if( ( found & FILE_ATTRIBUTE_READONLY ) && ( canthave & FA_RDONLY ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_HIDDEN ) && ( canthave & FA_HIDDEN ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_SYSTEM ) && ( canthave & FA_SYSTEM ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_DIRECTORY ) && ( canthave & FA_DIR ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_ARCHIVE ) && ( canthave & FA_ARCH ) )
		return false;



	if( ( musthave & FA_RDONLY ) && !( found & FILE_ATTRIBUTE_READONLY ) )
		return false;

	if( ( musthave & FA_HIDDEN ) && !( found & FILE_ATTRIBUTE_HIDDEN ) )
		return false;

	if( ( musthave & FA_SYSTEM ) && !( found & FILE_ATTRIBUTE_SYSTEM ) )
		return false;

	if( ( musthave & FA_DIR ) && !( found & FILE_ATTRIBUTE_DIRECTORY ) )
		return false;

	if( ( musthave & FA_ARCH ) && !( found & FILE_ATTRIBUTE_ARCHIVE ) )
		return false;

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FindFirstFile() -Searches a directory for a file.

 Parameters: path -[in] Pointer to a NUL-terminated string that specifies
                      a valid directory or path and file name.
			musthave -[in] File or directory must have these attributes.
			canthave- [in] File or directory can not have these attributes.

 Returns: On success string of file name or directory, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC char *FS_FindFirst( const char *path, W32 musthave, W32 canthave )
{
    WIN32_FIND_DATA FindFileData;

	if( FindHandle )
	{
		printf( "FS_FindFirst without close\n" );

		return NULL;
	}

	FS_FilePath( path, findbase );

	FindHandle = FindFirstFile( path, &FindFileData );

	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}



	if( CompareAttributes( FindFileData.dwFileAttributes, musthave, canthave ) )
	{
		if( ! *findbase )
		{
			my_strlcpy( findpath, FindFileData.cFileName, sizeof( findpath ) );
		}
		else
		{
			my_snprintf( findpath, sizeof( findpath ), "%s/%s", findbase, FindFileData.cFileName );
		}

		return findpath;
	}


    return FS_FindNext( musthave, canthave );
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
PUBLIC char *FS_FindNext( W32 musthave, W32 canthave )
{
	WIN32_FIND_DATA FindFileData;

	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	while( 1 )
	{
		if( FindNextFile( FindHandle, &FindFileData ) == 0 )
		{
			return NULL;
		}

		if( CompareAttributes( FindFileData.dwFileAttributes, musthave, canthave ) )
		{
			break;
		}
	}

	if( ! *findbase )
	{
		my_snprintf( findpath, sizeof( findpath ), "%s", FindFileData.cFileName );
	}
	else
	{
		my_snprintf( findpath, sizeof( findpath ), "%s/%s", findbase, FindFileData.cFileName );
	}

	return findpath;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FindClose() -Closes the search handle.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void FS_FindClose( void )
{
	if( FindHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( FindHandle );
    }

	FindHandle = 0;
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
	return DeleteFile( filename );
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
	return RemoveDirectory( pathname );
}


