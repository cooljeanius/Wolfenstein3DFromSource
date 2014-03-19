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
 *	client_screen.c: Master for refresh, status bar, console, chat, notify, etc.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include "client.h"
#include "console.h"

#include "../video/renderer.h"
#include "../video/video.h"

float		scr_con_current;	/* aproaches scr_conlines at scr_conspeed */
float		scr_conlines;		/* 0.0 to 1.0 lines of console to display */

_boolean	scr_initialized;	/* ready to draw? */

cvar_t		*scr_conspeed;


typedef struct
{

	int		x1, y1, x2, y2;

} dirty_t;

dirty_t	scr_dirty, scr_old_dirty[ 2 ];


/*
-----------------------------------------------------------------------------
 Function: Client_Screen_RunConsole -Figure out how the console should be
									drawn.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Client_Screen_RunConsole(void)
{
	/* decide on the height of the console */
	if (ClientStatic.key_dest == key_console) {
		scr_conlines = 0.5;		/* half screen */
	} else {
		scr_conlines = 0;		/* none visible */
	}

	if (scr_conlines < scr_con_current) {
		scr_con_current -= (scr_conspeed->value * ClientStatic.frametime);
		if (scr_conlines > scr_con_current) {
			scr_con_current = scr_conlines;
		}

	} else if (scr_conlines > scr_con_current) {
		scr_con_current += (scr_conspeed->value * ClientStatic.frametime);
		if (scr_conlines < scr_con_current) {
			scr_con_current = scr_conlines;
		}
	}
}

/*
-----------------------------------------------------------------------------
 Function: Client_Screen_DrawConsole -Draw the console on the screen.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Client_Screen_DrawConsole(void)
{
	Con_CheckResize();

	if ((ClientStatic.state == ca_disconnected) ||
		(ClientStatic.state == ca_connecting)) {
		/* forced full screen console */
		Con_DrawConsole(1.0);
		return;
	}

	if (ClientStatic.state != ca_active /*|| ! ClientState.refresh_prepped*/) {
		/* connected, but cannot render */
		Con_DrawConsole(0.5);
		R_Draw_Fill(0, (viddef.height >> 1), (int)viddef.width,
					(viddef.height >> 1), colourBlack);
		return;
	}

	if( scr_con_current ) {
		Con_DrawConsole( scr_con_current );
	} else {
		if ((ClientStatic.key_dest == key_game) ||
			(ClientStatic.key_dest == key_message)) {
			Con_DrawNotify(); /* only draw notify in game */
		}
	}
}




/*
-----------------------------------------------------------------------------
 Function: SCR_AddDirtyPoint

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void SCR_AddDirtyPoint(int x, int y)
{
	if (x < scr_dirty.x1) {
		scr_dirty.x1 = x;
	}

	if (x > scr_dirty.x2) {
		scr_dirty.x2 = x;
	}


	if (y < scr_dirty.y1) {
		scr_dirty.y1 = y;
	}

	if (y > scr_dirty.y2) {
		scr_dirty.y2 = y;
	}
}

/*
-----------------------------------------------------------------------------
 Function: SCR_DirtyScreen

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void SCR_DirtyScreen(void)
{
#if 0
	SCR_AddDirtyPoint(0, 0);
	SCR_AddDirtyPoint((viddef.width - 1), (viddef.height - 1));
#endif /* 0 */
}



/*
-----------------------------------------------------------------------------
 Function: Client_Screen_Init -Initialize client screen variables.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Client_Screen_Init(void)
{
	scr_conspeed = Cvar_Get("scr_conspeed", "3", CVAR_INIT);

	scr_initialized = true;
}

/* EOF */
