/*
 *	Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *
 */

/*
 *	files.c: Interface to file i/o layer.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

/*
 *	Notes:
 *
 *	This module accesses data through a hierarchal file system, but the
 *	contents of the file system can be transparently merged from several
 *	sources.
 *
 *	The "base directory" is the path to the directory holding the
 *	executable and all game directories. The sys_* files pass this to
 *	host_init in  quakeparms_t->basedir. This can be overridden with the
 *	"-basedir" command line parm to allow code debugging in a different
 *	directory. The base directory is only used during file system
 *	initialization.
 *
 *	The "game directory" is the first tree on the search path and directory
 *	that all generated files (save games, screen shots, demos, config
 *	files) will be saved to. This  can  be overridden with the "-game"
 *	command line parameter. The game directory can never be changed while
 *	the application is executing. This is a precaution against having a
 *	malicious server instruct clients to write files over areas they
 *	should NOT be able to write files to.
 */

#include <string.h>

#include "../../common/common.h"
#include "../../string/com_string.h"
#include "../../memory/zmem.h"
#include "../../memory/memory.h"
#include "filesystem.h"
#include "filelink.h"

#include "zippak.h"

#include "../../zlib/zlib.h"



PRIVATE char fs_gamedir[ MAX_OSPATH ];

PRIVATE cvar_t	*fs_basedir;
PRIVATE cvar_t	*fs_cddir;
PRIVATE cvar_t	*fs_gamedirvar;



/*
-----------------------------------------------------------------------------
 Function: FS_FreeFile -Free a file buffer created with FS_LoadFile().

 Parameters: buffer -[in] Previously allocated memory block to be freed.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void FS_FreeFile( void *buffer )
{
	Z_Free( buffer );
}


/*
-----------------------------------------------------------------------------
 Function: FS_AddGameDirectory -Add directory to search path.

 Parameters: dir -[in] Game directory path.

 Returns: Nothing.

 Notes:
	Sets fs_gamedir, adds the directory to the head of the path,
	then loads and adds *.pak then *.zip files.

-----------------------------------------------------------------------------
*/
PRIVATE void FS_AddGameDirectory( const char *dir )
{
	searchpath_t	*search;
	pack_t			*pak;
	char			path[ MAX_OSPATH ];
	char			*pakfile;

	my_strlcpy( fs_gamedir, dir, sizeof( fs_gamedir ) );

	/*
	 * add the directory to the search path
	 */
	search = Z_Malloc( sizeof( searchpath_t ) );
	my_strlcpy( search->filename, dir, sizeof( search->filename ) );
	search->next = fs_searchpaths;
	fs_searchpaths = search;

	/*
	 * add any pak files
	 */
	my_snprintf( path, sizeof( path ), "%s/*.pak", fs_gamedir );

	pakfile = FS_FindFirst( path, 0, 0 );
	if( pakfile ) {
		pak = FS_LoadZipFile( pakfile );
		if( pak ) {
			search = Z_Malloc( sizeof( searchpath_t ) );
			search->pack = pak;
			search->next = fs_searchpaths;
			fs_searchpaths = search;
		}


		while( (pakfile = FS_FindNext( 0, 0 )) != NULL ) {
			pak = FS_LoadZipFile( pakfile );
			if( ! pak ) {
				continue;
			}
			search = Z_Malloc( sizeof( searchpath_t ) );
			search->pack = pak;
			search->next = fs_searchpaths;
			fs_searchpaths = search;
		}
	}

	FS_FindClose();

	/*
	 * add any zip files
	 */
	my_snprintf( path, sizeof( path ), "%s/*.zip", fs_gamedir );

	pakfile = FS_FindFirst( path, 0, 0 );
	if( pakfile ) {
		pak = FS_LoadZipFile( pakfile );
		if( pak ) {
			search = Z_Malloc( sizeof( searchpath_t ) );
			search->pack = pak;
			search->next = fs_searchpaths;
			fs_searchpaths = search;
		}


		while( (pakfile = FS_FindNext( 0, 0 )) != NULL ) {
			pak = FS_LoadZipFile( pakfile );
			if( ! pak ) {
				continue;
			}
			search = Z_Malloc( sizeof( searchpath_t ) );
			search->pack = pak;
			search->next = fs_searchpaths;
			fs_searchpaths = search;
		}
	}

	FS_FindClose();

}

/*
-----------------------------------------------------------------------------
 Function: FS_Gamedir -Get root directory.

 Parameters: Nothing.

 Returns: String with the name of the root directory.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC char *FS_Gamedir( void )
{
	if( *fs_gamedir ) {
		return fs_gamedir;
	} else {
		return BASEDIRNAME;
	}
}

/*
-----------------------------------------------------------------------------
 Function: FS_ExecAutoexec -Get root directory.

 Parameters: Nothing.

 Returns: String with the name of the root directory.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void FS_ExecAutoexec( void )
{
	char *dir;
	char name[ MAX_GAMEPATH ];

	dir = Cvar_VariableString( "gamedir" );
	if( *dir ) {
		my_snprintf( name, sizeof( name ), "%s/%s/autoexec.cfg", fs_basedir->string, dir );
	} else {
		my_snprintf(name, sizeof(name), "%s/%s/autoexec.cfg", fs_basedir->string, BASEDIRNAME);
	}

	if( FS_FindFirst( name, 0, FA_DIR | FA_HIDDEN | FA_SYSTEM ) ) {
		Cbuf_AddText( "exec autoexec.cfg\n" );
	}

	FS_FindClose();
}


/*
-----------------------------------------------------------------------------
 Function: FS_SetGamedir -Sets the gamedir and path to a different directory.

 Parameters: dir -[in] New game directory.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void FS_SetGamedir( char *dir )
{
	searchpath_t	*next;

	if( strstr( dir, ".." ) || strstr( dir, "/" )
		|| strstr( dir, "\\" ) || strstr( dir, ":" ) ) {
		Com_Printf( "Gamedir should be a single filename, not a path\n" );
		return;
	}

	/*
	 * free up any current game dir info
	 */
	while( fs_searchpaths != fs_base_searchpaths ) {
		if( fs_searchpaths->pack ) {
			fclose( fs_searchpaths->pack->handle );
			Z_Free( fs_searchpaths->pack->files );
			Z_Free( fs_searchpaths->pack );
		}
		next = fs_searchpaths->next;
		Z_Free( fs_searchpaths );
		fs_searchpaths = next;
	}

#if 0
	/*
	 * flush all data, so it will be forced to reload
	 */
	if (dedicated && !dedicated->value) {
		Cbuf_AddText ("vid_restart\nsnd_restart\n");
	}
#endif /* 0 */

	my_snprintf( fs_gamedir, sizeof(fs_gamedir), "%s/%s", fs_basedir->string, dir);

	if( ! strcmp( dir,BASEDIRNAME ) || (*dir == 0) ) {
		Cvar_FullSet( "gamedir", "", CVAR_SERVERINFO|CVAR_NOSET );
		Cvar_FullSet( "game", "", CVAR_LATCH | CVAR_SERVERINFO );
	} else {
		Cvar_FullSet( "gamedir", dir, CVAR_SERVERINFO | CVAR_NOSET );
		if( fs_cddir->string[ 0 ] ) {
			FS_AddGameDirectory( va( "%s/%s", fs_cddir->string, dir ) );
		}
		FS_AddGameDirectory( va("%s/%s", fs_basedir->string, dir) );
	}
}


/*
-----------------------------------------------------------------------------
 Function: FS_Link_f -Creates a filelink_t.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void FS_Link_f( void )
{
	filelink_t	*flink, **prev;

	if( Cmd_Argc() != 3 ) {
		Com_Printf( "Usage: link <from> <to>\n" );
		return;
	}

	/* see if the link already exists */
	prev = &fs_links;
	for( flink = fs_links ; flink ; flink = flink->next ) {
		if( ! strcmp( flink->from, Cmd_Argv(1) ) ) {
			Z_Free( flink->to );
			if( ! strlen( Cmd_Argv( 2 ) ) ) {
				/* delete it */
				*prev = flink->next;
				Z_Free( flink->from );
				Z_Free( flink );
				return;
			}
			flink->to = my_CopyString( Cmd_Argv( 2 ) );
			return;
		}
		prev = &flink->next;
	}

	/* create a new link */
	flink = Z_Malloc( sizeof( *flink ) );
	flink->next = fs_links;
	fs_links = flink;
	flink->from = my_CopyString( Cmd_Argv( 1 ) );
	flink->fromlength = strlen( flink->from );
	flink->to = my_CopyString( Cmd_Argv( 2 ) );
}

/*
-----------------------------------------------------------------------------
 Function: FS_ListFiles -List files.

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE char **FS_ListFiles( char *findname, int *numfiles, unsigned musthave, unsigned canthave )
{
	char *s;
	int nfiles = 0;
	char **list = 0;

	s = FS_FindFirst( findname, musthave, canthave );
	while ( s ) {
		if ( s[strlen(s)-1] != '.' ) {
			nfiles++;
		}
		s = FS_FindNext( musthave, canthave );
	}
	FS_FindClose ();

	if ( !nfiles ) {
		return NULL;
	}

	nfiles++; /* add space for a guard */
	*numfiles = nfiles;

	list = MM_MALLOC( sizeof( char * ) * nfiles );
	if( list == NULL ) {
		MM_OUTOFMEM( "list" );
	}

	memset( list, 0, sizeof( char * ) * nfiles );

	s = FS_FindFirst( findname, musthave, canthave );
	nfiles = 0;
	while( s ) {
		if( s[ strlen( s ) - 1 ] != '.' ) {
			list[ nfiles ] = strdup( s );

			(void)my_strlwr( list[ nfiles ] );

			nfiles++;
		}
		s = FS_FindNext( musthave, canthave );
	}
	FS_FindClose();

	return list;
}

/*
-----------------------------------------------------------------------------
 Function: FS_Dir_f -List directories and files.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void FS_Dir_f( void )
{
	char	*path = NULL;
	char	findname[1024];
	char	wildcard[1024] = "*.*";
	char	**dirnames;
	int		ndirs;

	if ( Cmd_Argc() != 1 ) {
		my_strlcpy( wildcard, Cmd_Argv( 1 ), sizeof( wildcard )  );
	}

	while ( ( path = FS_NextPath( path ) ) != NULL ) {
		char *tmp = findname;

		my_snprintf( findname, sizeof( findname ), "%s/%s", path, wildcard );

		while ( *tmp != 0 ) {
			if ( *tmp == '\\' ) {
				*tmp = '/';
			}
			tmp++;
		}
		Com_Printf( "Directory of %s\n", findname );
		Com_Printf( "----\n" );

		if ( ( dirnames = FS_ListFiles( findname, &ndirs, 0, 0 ) ) != 0 ) {
			int i;

			for ( i = 0; i < ndirs-1; i++ ) {
				if ( strrchr( dirnames[i], '/' ) ) {
					Com_Printf( "%s\n", strrchr( dirnames[i], '/' ) + 1 );
				} else {
					Com_Printf( "%s\n", dirnames[i] );
				}

				MM_FREE( dirnames[i] );
			}
			MM_FREE( dirnames );
		}
		Com_Printf( "\n" );
	};
}

/*
-----------------------------------------------------------------------------
 Function: FS_Dir_f -List search path and file links.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void FS_Path_f( void )
{
	searchpath_t	*s;
	filelink_t		*l;

	Com_Printf( "Current search path:\n" );
	for( s = fs_searchpaths ; s ; s = s->next ) {
		if (s == fs_base_searchpaths) {
			Com_Printf ("----------\n");
		}
		if (s->pack) {
			Com_Printf ("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
		} else {
			Com_Printf ("%s\n", s->filename);
		}
	}

	Com_Printf ("\nLinks:\n");
	for( l = fs_links ; l ; l = l->next ) {
		Com_Printf ("%s : %s\n", l->from, l->to);
	}
}


/*
-----------------------------------------------------------------------------
 Function: FS_NextPath -Allows enumerating all of the directories in the
						search path.

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC char *FS_NextPath( char *prevpath )
{
	searchpath_t	*s;
	char			*prev;

	if( ! prevpath ) {
		return fs_gamedir;
	}

	prev = fs_gamedir;
	for( s = fs_searchpaths ; s ; s = s->next ) {
		if( s->pack ) {
			continue;
		}

		if( prevpath == prev ) {
			return s->filename;
		}

		prev = s->filename;
	}

	return NULL;
}


/*
-----------------------------------------------------------------------------
 Function: FS_InitFilesystem -Initialize file system.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void FS_InitFilesystem( void )
{
	Cmd_AddCommand( "path", FS_Path_f );
	Cmd_AddCommand( "link", FS_Link_f );
	Cmd_AddCommand( "dir", FS_Dir_f );

	/*
	 * basedir <path>
	 * allows the game to run from outside the data tree
	 */
	fs_basedir = Cvar_Get( "basedir", ".", CVAR_NOSET );

	/*
	 * cddir <path>
	 * Logically concatenates the cddir after the basedir. This
	 * allows the game to run from outside the data tree.
	 */
	fs_cddir = Cvar_Get( "cddir", "", CVAR_NOSET );
	if( fs_cddir->string[ 0 ] ) {
		FS_AddGameDirectory( va( "%s/"BASEDIRNAME, fs_cddir->string ) );
	}

	/*
	 * start up with BASEDIRNAME by default
	 */
	FS_AddGameDirectory( va( "%s/"BASEDIRNAME, fs_basedir->string ) );

	/* any set gamedirs will be freed up to here */
	fs_base_searchpaths = fs_searchpaths;

	/* check for game override */
	fs_gamedirvar = Cvar_Get( "game", "", CVAR_LATCH | CVAR_SERVERINFO );
	if( fs_gamedirvar->string[ 0 ] ) {
		FS_SetGamedir( fs_gamedirvar->string );
	}
}

/* EOF */
