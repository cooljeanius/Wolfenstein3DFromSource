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
 * wolf_client_main.c
 */

#include <string.h>

#include "../../../client/client.h"
#include "../../../video/video.h"
#include "../../../video/renderer.h"
#include "../../../device/timer/timer.h"
#include "../../../string/com_string.h"
#include "../../../client/console.h"
#include "../wolf_local.h"
#include "../level/wolf_level.h"
#include "../entities/wolf_player.h"
#include "../../../device/input/input.h"
#include "../../../sound/sound.h"

#include "wolf_client.h" /* new header for prototypes from this file */

extern void R_DrawWorld(void);


/*
-----------------------------------------------------------------------------
 Function: V_RenderView

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Basically a simple wrapper around R_DrawWorld() for now.

-----------------------------------------------------------------------------
*/
PRIVATE void V_RenderView(void)
{
	if (ClientStatic.key_dest != key_game) {
		return;
	}

	R_DrawWorld();
}

/*
-----------------------------------------------------------------------------
 Function: Client_Screen_UpdateScreen

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
/* prototype moved to "wolf_client.h" */
PUBLIC void Client_Screen_UpdateScreen(void)
{
	R_BeginFrame(); /********/


		V_RenderView();

		Client_Screen_DrawConsole(); /* not worth breaking on */

		M_Draw();


	R_EndFrame();

}

/*
-----------------------------------------------------------------------------
 Function: Client_PrepRefresh

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
/* prototype moved to "wolf_client.h" */
PUBLIC void Client_PrepRefresh(const char *r_mapname)
{
	char mapname[ 32 ];
#if 0 || __clang_analyzer__
	int			i;
	char		name[MAX_GAMEPATH];
	float		rotate;
	vec3_t		axis;
#endif /* 0 || __clang_analyzer__ */

    if (! r_mapname || ! *r_mapname) {
		return;
	}

	R_DrawPsyched(0);
	R_EndFrame();

	if (g_version->value == SPEAROFDESTINY) {
		spritelocation = SODSPRITESDIRNAME;
	} else {
		spritelocation = WL6SPRITESDIRNAME;
	}

#if 0 || __clang_analyzer__
	SCR_AddDirtyPoint(0, 0);
	SCR_AddDirtyPoint((viddef.width - 1), (viddef.height - 1));
#endif /* 0 || __clang_analyzer__ */

	my_strlcpy(mapname, r_mapname, sizeof(mapname)); /* skip "maps/" */
	if((NULL != strstr(mapname, ".map")) || (NULL != strstr(mapname, ".MAP"))) {
        mapname[(strlen(mapname) - 4)] = '\0'; /* cut off ".map" */
    }

	/* register models, pics, and skins	*/
	R_BeginRegistration(mapname);

	if (r_world == NULL) {
		return;
	}

	Com_Printf("Map: %s\n", r_world->mapName);

	R_DrawPsyched( 30 );
	R_EndFrame();

	Level_ScanInfoPlane(r_world); /* Spawn items/guards */

	Com_Printf("Spawning Entities\n");
	PL_Spawn(r_world->pSpawn, r_world); /* Spawn Player */

	Com_Printf("Caching Textures and Sounds\n");
	Level_PrecacheTextures_Sound( r_world );

	R_DrawPsyched(80);
	R_EndFrame();

	/* the renderer can now free unneeded stuff */
	R_EndRegistration();

	R_DrawPsyched(100);
	R_EndFrame();

	/* clear any lines of console text */
	Con_ClearNotify();

	if (r_world->musicName) {
		Sound_StartBGTrack(r_world->musicName, r_world->musicName);
	}

	R_EndFrame();

}


int tics;

extern void M_Intermission_f(void);

/*
-----------------------------------------------------------------------------
 Function: Client_Frame

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Client_Frame(int msec)
{
	static int extratime;
#if 0 || __clang_analyzer__
	static int lasttimecalled;
#endif /* 0 || __clang_analyzer__ */


	vec3_t vnull = { 0, 0, 0 };

	extratime += msec;


	/* let the mouse activate or deactivate */
	IN_Frame();

	/* decide the simulation time */
	ClientStatic.frametime = (extratime / 1000.0f);
#if 0
	cl.time += extratime; /* 'cl' is undeclared */
#endif /* 0 */
	ClientStatic.realtime = (int)curtime;

	extratime = 0;

#if 0
	if (ClientStatic.frametime > (1.0f / cl_minfps->value)) {
		ClientStatic.frametime = (1.0f / cl_minfps->value);
	}
#else
	if (ClientStatic.frametime > (1.0f / 5)) {
		ClientStatic.frametime = (1.0f / 5);
	}
#endif /* 0 */

	ClientState.time = (int)(ClientStatic.frametime * 100);

    tics = 1;

	/* allow rendering change */
	Video_CheckChanges();
	/* TODO: handle errors thrown by Video_CheckChanges() */
#if 0
	if (! ClientState.refresh_prepped && (ClientStatic.state == ca_active)) {
		Client_PrepRefresh(levelstate./**/); /* expected identifier */
	}
#endif /* 0 */

	Sound_Update(vnull, vnull, vnull, vnull);

	if ((ClientStatic.key_dest == key_game) && (ClientStatic.state == ca_active)) {
		if (Player.playstate != ex_dead) {
            int tempFineAngle; /* unused (?) (was previously) */

			tempFineAngle = 0;

			/* dummy to silence clang static analyzer warning about value stored to
			 * 'tempFineAngle' never being read: */
			if (tempFineAngle == 0) {
				;
			}

			Client_SendCommand();

            Player.position.angle = (angle_normalize((float)FINE2RAD(ClientState.viewangles[YAW]) +
													 Player.position.angle));
		} else {
			memset(&ClientState.cmd, 0, sizeof(ClientState.cmd));
		}


		if ((Player.playstate == ex_complete) || (Player.playstate == ex_secretlevel)) {
			M_Intermission_f();
		} else {
			PL_Process(&Player, r_world); /* Player processing */
			ProcessGuards(); /* if single */
			PushWall_Process();
			Door_ProcessDoors_e(&r_world->Doors, tics, msec);

			levelstate.time += tics;
		}
	}

	Client_Screen_RunConsole(); /* not worth going into, no decent places to
								 * break on within it... */

    Client_Screen_UpdateScreen(); /* calls 5 other functions in a row */

	++ClientStatic.framecount;
}

/* EOF */
