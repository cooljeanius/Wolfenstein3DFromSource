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
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 *	unix_vid.c: Main windowed and fullscreen graphics interface module.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "../client/client.h"
#include "../client/keys.h"
#include "../client/console.h"
#include "../video/renderer.h"
#include "../video/video.h"
#include "../common/common.h"
#include "../common/common_utils.h"
#include "../device/input/input.h"
#include "../sound/sound.h"


/* Console variables that we need to access from this module */
cvar_t		*vid_gamma;
cvar_t		*r_ref;			/* Name of Refresh DLL loaded */
cvar_t		*vid_xpos;		/* X coordinate of window position */
cvar_t		*vid_ypos;		/* Y coordinate of window position */
cvar_t		*r_fullscreen;

/* Global variables used internally by this module */
viddef_t	viddef;	/* global video state; used by other modules */


#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[ 0 ] ) )


extern void RW_IN_Shutdown( void );

/*==========================================================================*/


/*
-----------------------------------------------------------------------------
 Function: VID_Restart_f -Console command to re-start the video mode.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	We do this simply by setting the modified flag for the r_ref variable,
	which will cause the entire video mode to be reset on the next frame.

-----------------------------------------------------------------------------
*/
PRIVATE void VID_Restart_f( void )
{
	r_ref->modified = true;
}



/*
 *	VID_GetModeInfo
 */
typedef struct vidmode_s
{
	const char *description;
	int         width, height;
	int         mode;

} vidmode_t;

vidmode_t vid_modes[] =
{
	{ "Mode 0: 640x480",   640,   480, 0 },
	{ "Mode 1: 800x600",   800,   600, 1 },
	{ "Mode 2: 960x720",   960,   720, 2 },
	{ "Mode 3: 1024x768",  1024,  768, 3 },
	{ "Mode 4: 1152x864",  1152,  864, 4 },
	{ "Mode 5: 1280x960",  1280,  960, 5 },
	{ "Mode 6: 1600x1200", 1600, 1200, 6 },
	{ "Mode 7: 2048x1536", 2048, 1536, 7 }
};


/*
-----------------------------------------------------------------------------
 Function: VID_GetModeInfo -Get window mode information.

 Parameters: width -[out] window width.
			 height -[out] window height.
			 mode -[in] Video mode (see vidmode_t for details).

 Returns: True if mode was accepted, otherwise false.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC _boolean VID_GetModeInfo( int *width, int *height, int mode )
{
	if( mode < 0 || mode >= VID_NUM_MODES ) {
		return false;
	}

	*width  = vid_modes[ mode ].width;
	*height = vid_modes[ mode ].height;

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: VID_NewWindow -Set new window width height.

 Parameters: width -[in] width of new window.
			 height -[in] height of new window.

 Returns:	Nothing

 Notes: Sets global variables [viddef.width] [viddef.height]
-----------------------------------------------------------------------------
*/
PUBLIC void VID_NewWindow( int width, int height )
{
	viddef.width  = width;
	viddef.height = height;

#if 0
	ClientState.force_refdef = true; /* cannot use a paused refdef */
#endif /* 0 */
}


/*
-----------------------------------------------------------------------------
 Function: VID_LoadRefresh -Restart the rendering layer.

 Parameters: Nothing

 Returns: false on error, true on success.

 Notes:
 	This function refreshes the rendering layer.
-----------------------------------------------------------------------------
*/
extern void IN_ActivateMouse( void ) ;

PRIVATE _boolean VID_LoadRefresh( void )
{
	R_Shutdown();

	if( R_Init( 0, 0 ) == -1 ) {
		Com_Printf("VID_LoadRefresh() encountered an error, running R_Shutdown()...\n");
		R_Shutdown();
		Com_Printf("...and returning false.\n");
		return false;
	} else {
		Com_Printf("R_Init(0,0) did NOT equal -1\n");
		Com_Printf(" (in VID_LoadRefresh() in unix_vid_sys.c)");
	}


	IN_ActivateMouse();

	Com_Printf( "------------------------------------\n");

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: Video_CheckChanges -Check for video mode changes.

 Parameters: Nothing

 Returns: Nothing

 Notes:
 	This function gets called once just before drawing each frame, and
	its sole purpose is to check to see if any of the video mode
	parameters have changed, and if they have to update the video
	mode to match.
-----------------------------------------------------------------------------
*/
PUBLIC void Video_CheckChanges( void )
{
	if( r_ref->modified ) {
		Sound_StopAllSounds();
	}

	while( r_ref->modified ) {
		/* refresh has changed */
		r_ref->modified = false;
		r_fullscreen->modified = true;
		ClientState.refresh_prepped = false;
#if 0
		ClientStatic.disable_screen = true;
#endif /* 0 */

		if ( ! VID_LoadRefresh() ) {
			Com_Printf("in Video_CheckChanges() in unix_vid_sys.c:\n");
			Com_Printf("VID_LoadRefresh() failed:\n");
			Com_Error( ERR_FATAL, "Could not start OpenGL module!" );
			Com_Printf(" (in Video_CheckChanges() in unix_vid_sys.c)");
			Com_Printf(" (this means that VID_LoadRefresh() failed)");

			/* drop the console if we fail to load a refresh */
			if ( ClientStatic.key_dest != key_console ) {
				Con_ToggleConsole_f();
			}
		}
#if 0
		ClientStatic.disable_screen = false;
#endif /* 0 */
	}

}


/*
-----------------------------------------------------------------------------
 Function: Video_Init -Initialize video sub-system.

 Parameters: Nothing

 Returns:	Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Video_Init( void )
{
	/* Create the video variables so we know how to start the graphics drivers */
	/* if DISPLAY is defined, try X */
	if( getenv( "DISPLAY" ) ) {
		r_ref = Cvar_Get( "r_ref", "softx", CVAR_ARCHIVE );
	} else {
		r_ref = Cvar_Get( "r_ref", "soft", CVAR_ARCHIVE );
	}
	vid_xpos = Cvar_Get( "win_xpos", "3", CVAR_ARCHIVE );
	vid_ypos = Cvar_Get( "win_ypos", "22", CVAR_ARCHIVE );
	r_fullscreen = Cvar_Get( "r_fullscreen", "0", CVAR_ARCHIVE );
	vid_gamma = Cvar_Get( "vid_gamma", "1", CVAR_ARCHIVE );

	/* Add some console commands that we want to handle */
	Cmd_AddCommand( "vid_restart", VID_Restart_f );


	/* Start the graphics mode and load refresh DLL */
	Video_CheckChanges();
}

/*
-----------------------------------------------------------------------------
 Function: Video_Shutdown -Shutdown video sub-system.

 Parameters: Nothing

 Returns:	Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Video_Shutdown( void )
{
#if 0
	RW_IN_Shutdown();
#endif /* 0 */

	R_Shutdown();
}

/* EOF */
