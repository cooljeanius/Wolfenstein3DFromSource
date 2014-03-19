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
 * wolf_sv_ccmds.c
 */

#include <string.h>

#include "../level/wolf_level.h"

#include "../../../common/cmd.h"
#include "../../../common/common.h"
#include "../../../client/client.h"
#include "../../../device/filesys/filesystem.h"
#include "../../../string/com_string.h"

#include "wolf_client.h" /* new header for prototypes from this file */

extern void Client_PrepRefresh(const char *r_mapname);


/*
-----------------------------------------------------------------------------
 Function: SV_GameMap_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void SV_GameMap_f(void)
{
	char		*map;
	char r_mapname[ 32 ];

	if (Cmd_Argc() != 2) {
		Com_Printf("USAGE: gamemap <map>\n");
		return;
	}

#if 0 || __clang_analyzer__
	Com_DPrintf("SV_GameMap( %s )\n", Cmd_Argv(1));
#endif /* 0 || __clang_analyzer__ */

	FS_CreatePath(va("%s/save/current/", FS_Gamedir()));

	/* check for clearing the current savegame */
	map = Cmd_Argv(1);
#if __clang_analyzer__
	Com_DPrintf("SV_GameMap( %s )\n", map);
#endif /* __clang_analyzer__ (hack to use value stored to map) */

	/* start up the next map */
	my_strlcpy(r_mapname, Cmd_Argv(1), sizeof(r_mapname));

	Client_PrepRefresh(r_mapname);
}

/*
-----------------------------------------------------------------------------
 Function: Map_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
/* prototype moved to "wolf_client.h" */
PUBLIC void Map_f(void)
{
	char	*map;
	char	expanded[MAX_GAMEPATH];

	/* Check to make sure the level exists. */
	map = Cmd_Argv(1);
	if (! strstr(map, ".")) {
		my_snprintf(expanded, sizeof(expanded), "maps/%s.map", map);
	} else {
		my_snprintf(expanded, sizeof(expanded), "maps/%s", map);
	}


#if 0
	/* 'sv' and 'ss_dead' are undeclared: */
	sv.state = ss_dead; /* do NOT save current level when changing */
	SV_WipeSavegame("current");
#endif /* 0 */
	SV_GameMap_f();

	if (r_world) {
		ClientStatic.key_dest = key_game;
		ClientStatic.state = ca_active;
	} else {
		ClientStatic.key_dest = key_console;
	}

}

/* EOF */
