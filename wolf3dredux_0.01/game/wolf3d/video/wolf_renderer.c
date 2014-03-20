/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	wolf_renderer.c:  Wolfenstein 3-D renderer.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../../video/texture_manager.h"
#include "../../../video/renderer.h"
#include "../../../video/video.h"
#include "../../../client/console.h"
#include "../../../client/client.h"
#include "../../../string/com_string.h"
#include "../../../memory/memory.h"
#include "../../../math/random_number.h"

#include "../wolf_local.h"
#include "../menu/wolf_menu.h"
#include "../entities/wolf_sprites.h"
#include "../entities/wolf_player.h"
#include "../entities/wolf_powerups.h"

#include "wolf_raycast.h"
#include "wolf_renderer.h"

extern viddef_t viddef;


LevelData_t *r_world;


PRIVATE const char weaponnames[4][32] =
{
	"pics/KNIFEPIC.tga",
	"pics/GUNPIC.tga",
	"pics/MACHINEGUNPIC.tga",
	"pics/GATLINGGUNPIC.tga"
};

PRIVATE SW32 hud_x, hud_y;


/*
-----------------------------------------------------------------------------
 Function: R_DrawHUD

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawHUD(void)
{
	SW32 w, h;
	W32 score = (W32)Player.score;

	TM_GetTextureSize(&w, &h, "pics/STATUSBARPIC.tga");
	hud_x = (viddef.width - w ) >> 1;
	hud_y = viddef.height - h;
	R_Draw_Pic((int)hud_x, (int)hud_y, "pics/STATUSBARPIC.tga");

	if (Player.items & ITEM_KEY_1) {
		R_Draw_Pic((int)(hud_x + 480), (int)(hud_y + 8), "pics/GOLDKEYPIC.tga");
	}

	if (Player.items & ITEM_KEY_2) {
		R_Draw_Pic((int)(hud_x + 480), (int)(hud_y + 40),
				   "pics/SILVERKEYPIC.tga");
	}


	R_Draw_Pic((int)(hud_x + 512), (int)(hud_y + 15),
			   weaponnames[Player.weapon]);

	/* Clamp score */
	if (score > 999999) {
		score = 999999;
	}

	R_DrawNumber((int)(hud_x + 48), (int)(hud_y + 32),
				 (int)(levelstate.floornum + 1));
	R_DrawNumber((int)(hud_x + 180), (int)(hud_y + 32), (int)score);
	R_DrawNumber((int)(hud_x + 224), (int)(hud_y + 32), Player.lives);
	R_DrawNumber((int)(hud_x + 368), (int)(hud_y + 32), Player.health);
	R_DrawNumber((int)(hud_x + 444), (int)(hud_y + 32),
				 Player.ammo[AMMO_BULLETS]); /* FIXME! */
}

PRIVATE const char mugshotnames[24][32] =
{
	"pics/FACE1APIC.tga",
	"pics/FACE1BPIC.tga",
	"pics/FACE1CPIC.tga",

	"pics/FACE2APIC.tga",
	"pics/FACE2BPIC.tga",
	"pics/FACE2CPIC.tga",

	"pics/FACE3APIC.tga",
	"pics/FACE3BPIC.tga",
	"pics/FACE3CPIC.tga",

	"pics/FACE4APIC.tga",
	"pics/FACE4BPIC.tga",
	"pics/FACE4CPIC.tga",

	"pics/FACE5APIC.tga",
	"pics/FACE5BPIC.tga",
	"pics/FACE5CPIC.tga",

	"pics/FACE6APIC.tga",
	"pics/FACE6BPIC.tga",
	"pics/FACE6CPIC.tga",

	"pics/FACE7APIC.tga",
	"pics/FACE7BPIC.tga",
	"pics/FACE7CPIC.tga",

	"pics/FACE8APIC.tga"
};

PRIVATE const char godmugshotnames[][32] =
{
	"pics/GODMODEFACE0PIC.tga",
	"pics/GODMODEFACE1PIC.tga",
	"pics/GODMODEFACE2PIC.tga"
};

/*
-----------------------------------------------------------------------------
 Function: R_DrawFace

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawFace(void)
{
	Player.facecount += tics;
	if ((W32)Player.facecount > US_RndT()) {
		Player.face_gotgun = Player.face_ouch = false;
		Player.faceframe = (int)(US_RndT() >> 6);
		if (Player.faceframe == 3) {
			Player.faceframe = 0;
		}

		Player.facecount = 0;
	}


	if (Player.health) {
		if ((g_version->value == SPEAROFDESTINY) &&
			(Player.flags & FL_GODMODE)) {
			R_Draw_Pic((int)(hud_x + 272), (int)(hud_y + 8),
					   godmugshotnames[Player.faceframe]);
		} else if (Player.face_gotgun) {
			R_Draw_Pic((int)(hud_x + 272), (int)(hud_y + 8),
					   "pics/GOTGATLINGPIC.tga");
		} else {
			R_Draw_Pic((int)(hud_x + 272), (int)(hud_y + 8),
					   mugshotnames[(3 * ((100 - Player.health) / 16) +
									 Player.faceframe)]);
		}
	} else {
		if (Player.LastAttacker && Player.LastAttacker->type == en_needle) {
			R_Draw_Pic((int)(hud_x + 272), (int)(hud_y + 8),
					   "pics/MUTANTBJPIC.tga");
		} else {
			R_Draw_Pic((int)(hud_x + 272), (int)(hud_y + 8),
					   "pics/FACE8APIC.tga");
		}
	}
}




SW32 r_damageflash = 0;

/*
-----------------------------------------------------------------------------
 Function: R_DamageFlash

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DamageFlash(int damage)
{
	r_damageflash += damage;
	if (r_damageflash > 256) {
		r_damageflash = 256;
	}
}

/*
-----------------------------------------------------------------------------
 Function: R_DrawFlash

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void R_DrawFlash(void)
{
	if (r_damageflash) {
		R_DrawBox(0, 0, (int)viddef.width, (int)viddef.height,
				  (W32)((0xFF << 24) | 245));

		if ((r_damageflash -= 1) < 0) {
			r_damageflash = 0;
		}
	}
}

/*
-----------------------------------------------------------------------------
 Function: R_DrawBackGnd -Renders the background floor / ceiling colours.

 Parameters: floor -[in] floor colour.
			 ceiling -[in] ceiling colour.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void R_DrawBackGnd( colour3_t floor, colour3_t ceiling )
{
	R_Draw_Fill( 0, 0, (int)viddef.width, (int)viddef.height >> 1, ceiling );

	R_Draw_Fill( 0, viddef.height >> 1, (int)viddef.width, (int)viddef.height, floor );
}


W32 intensity = 0;

/*
-----------------------------------------------------------------------------
 Function: R_DrawWorld -Renders the game world.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawWorld( void )
{
	placeonplane_t viewport;

#if 0
	if( ClientStatic.key_dest != key_game )	{
		return;
	}
#endif /* 0 */

/* initializing */
	viewport = Player.position;


	R_DrawBackGnd( r_world->floorColour, r_world->ceilingColour );

	R_SetGL3D( viewport );

	R_RayCast( viewport, r_world );
	R_DrawSprites();


	R_SetGL2D();	/* restore 2D back */

	if( Player.playstate == ex_dead ) {
		R_DrawBox( 0, 0, (int)viddef.width, (int)viddef.height, (0xFF << 24) | (W8)intensity );
		if( ++intensity >= 240 ) {
			intensity = 0;
			if( ! PL_Reborn( &Player ) ) {
				M_Menu_Main_f();
				ClientStatic.state = ca_disconnected;
			} else {
				PL_Spawn( r_world->pSpawn, r_world ); /* Spawn Player */
			}

		}
	} else {
		R_DrawWeapon();
		R_DrawFlash();
	}


	R_DrawHUD();
	R_DrawFace();

}


colour3_t barsecondcolour	= {	252, 0, 0 };
colour3_t barthirdcolour	= {	252, 156, 156 };

/*
-----------------------------------------------------------------------------
 Function: R_DrawPsyched -Draw "Get Psyched" plaque.

 Parameters: percent -[in] percentage of level loading.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawPsyched( W32 percent )
{
	SW32 w, h;
	W32 bar_length;

	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, interbkgnd);

	TM_GetTextureSize( &w, &h, "pics/GETPSYCHEDPIC.tga" );
	R_Draw_Pic((int)((viddef.width - w) >> 1),
			   (int)(((viddef.height - h) >> 1) - 80),
			   "pics/GETPSYCHEDPIC.tga");

	R_Draw_Fill((int)((viddef.width - w) >> 1),
				(int)(((viddef.height - h) >> 1) + h - 80), (int)w, 4,
				colourBlack);

	bar_length = (((unsigned long)w * percent) / 100);
	if (bar_length) {
		R_Draw_Fill((int)((viddef.width - w) >> 1),
					(int)(((viddef.height - h) >> 1) + h - 80),
					(int)bar_length, 4, barsecondcolour);
		R_Draw_Fill((int)((viddef.width - w) >> 1),
					(int)(((viddef.height - h) >> 1) + h - 80),
					(int)(bar_length - 2), 3, barthirdcolour);
	}

	R_DrawHUD();
}

/*
-----------------------------------------------------------------------------
 Function: R_BeginRegistration -Start the rendering registration sequence.

 Parameters: map -[in] The name of the map to load.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_BeginRegistration(const char *map)
{
	char	fullname[MAX_GAMEPATH];

	if( ! map || ! *map ) {
		return;
	}

	++texture_registration_sequence;


	my_snprintf( fullname, sizeof( fullname ), "maps/%s.map", map );

	/* explicitly free the old map if different */
	if (r_world) {
		if (r_world->mapName) {
			Z_Free( r_world->mapName );
		}
		if( r_world->musicName ) {
			Z_Free( r_world->musicName );
		}

		Z_Free( r_world );
	}

#if 0 || __clang_analyzer__
	Door_ResetDoors(&r_world->Doors);
#endif /* 0 */
	Powerup_Reset();
	Sprite_Reset();
	Areas_InitAreas(Player.areanumber);
	PushWall_Reset();

	memset(&levelstate, 0, sizeof(levelstate)); /* Reset gamestate */
	ResetGuards();

	r_world = Level_LoadMap(fullname);

	if (r_world == NULL) {
		Com_Printf("Could not load map (%s)\n", map);
		return;
	}

	levelstate.floornum = floornumber;

	if (g_version->value == SPEAROFDESTINY) {
		if (strlen(map) >= 2) {
			levelstate.floornum = (W32)atoi(map + 1);

			if (levelstate.floornum == 20) {
				levelstate.floornum = 17;
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
 Function: R_EndRegistration -End the rendering registration sequence.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_EndRegistration(void)
{
	TM_FreeUnusedTextures();
}

/* EOF */
