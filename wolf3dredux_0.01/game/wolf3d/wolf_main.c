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

#include <string.h>
#ifndef setenv
# include <unistd.h>
#endif /* !setenv */


#include "wolf_local.h"
#include "entities/wolf_powerups.h"
#include "entities/wolf_sprites.h"
#include "entities/wolf_player.h"

#include "script/episode.h"


level_locals_t	levelstate;

LRstruct LevelRatios;

cvar_t	*g_version; /* Wolfenstein or Spear of Destiny */
cvar_t	*episode;
cvar_t	*skill;
cvar_t	*deathmatch;
cvar_t	*maxentities;


char *spritelocation = WL6SPRITESDIRNAME;


/*
-----------------------------------------------------------------------------
 Function: Game_Reset

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Simply calls memset() for the 'levelstate' global variable.
-----------------------------------------------------------------------------
*/
PUBLIC void Game_Reset(void)
{
	memset(&levelstate, 0, sizeof(levelstate));
}

extern void Map_f(void);


/*
-----------------------------------------------------------------------------
 Function: Game_Init

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Game_Init(void)
{
	Com_Printf("\n------ Game Init ------\n");

#if defined(sv_cheats)
	/* noset vars */
	dedicated = Cvar_Get("dedicated", "0", CVAR_NOSET);

	/* latched vars */
	/* 'sv_cheats' is undeclared */
	sv_cheats = Cvar_Get("cheats", "0", (CVAR_SERVERINFO | CVAR_LATCH));
#endif /* sv_cheats */


	episode = Cvar_Get("episode", "0", CVAR_LATCH);
	skill = Cvar_Get("skill", "1", CVAR_LATCH);
	g_version = Cvar_Get("g_version", "0", (CVAR_ARCHIVE | CVAR_LATCH));
#if 0 || __clang_analyzer__
	maxentities = Cvar_Get("maxentities", "1024", CVAR_LATCH);
#endif /* 0 || __clang_analyzer__ */


	/* dm map list */
	Cmd_AddCommand("map", Map_f);

#if 0
	/* items */
	InitItems(); /* unimplemented */
#endif /* 0 */

	G_Build_Tables(); /* turn on random number generators (pretty simple) */

#if 0
	Lvl_Init(); /* unimplemented */
#endif /* 0 */
	Powerup_Reset(); /* Removes all powerups set in the 'powerups' global var. */
	Sprite_Reset(); /* just calls memset() on the 'Spr_Sprites' global var. */

	Game_Reset(); /* just calls memset() for the 'levelstate' global var. */
	PL_Init(); /* 'PL' is short for "player" */


/*
 *	Init Scripts
 */
	if (! episode_init("script/episode.scp")) {
		Com_Printf("Game_Init(): error parsing script (episode.scp)\n");
	}

	Com_Printf("\n-----------------------\n");
	setenv("GAME_INIT_HAS_RUN", "1", 0);
}


/*
-----------------------------------------------------------------------------
 Function: Game_Shutdown

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Game_Shutdown(void)
{
	Com_Printf("==== Game Shutdown ====\n");

/*
 *	Shutdown Scripts
 */
	episode_shutdown();

	Z_FreeTags(TAG_LEVEL);
	Z_FreeTags(TAG_GAME);
}

/* EOF */
