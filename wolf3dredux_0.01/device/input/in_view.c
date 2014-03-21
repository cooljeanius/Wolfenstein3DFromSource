/*
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
 *	in_view.c:   Interface to mouse / joystick
 *
 *	Author:	    Id Software, Inc.
 *	Date:	    1997-2001
 *
 */

#include "../../client/client.h"
#include "input.h"


_boolean	mouseinitialized;
int			mouse_oldbuttonstate;
_boolean	mouseactive;	/* false when not focus app */
int mx_accum;
int my_accum;

_boolean	in_appactive;

_boolean	mlooking;

cvar_t	*in_mouse;



extern void IN_StartupMouse( void );
extern void IN_ActivateMouse( void );
extern void IN_DeactivateMouse( void );
extern void IN_MouseMove( usercmd_t *cmd );


extern void IN_StartupJoystick( void );
extern void IN_JoyMove( usercmd_t *cmd );

/*
=========================================================================

VIEW CENTERING

=========================================================================
*/

cvar_t	*v_centermove;
cvar_t	*v_centerspeed;


/*
-----------------------------------------------------------------------------
 Function: IN_Init -Initialize mouse and joy-stick.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
void IN_Init(void)
{
	/* centering */
	v_centermove  = Cvar_Get("v_centermove", "0.15", 0);
	v_centerspeed = Cvar_Get("v_centerspeed", "500", 0);

	IN_StartupMouse(); /* has a lot of X11-specific calls in the Unix version */
	IN_StartupJoystick(); /* just a bunch of calls to Cvar_Get() so far... */
}

/*
-----------------------------------------------------------------------------
 Function: IN_Shutdown -Shutdown input layer.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Simple wrapper around IN_DeactivateMouse() for now.

-----------------------------------------------------------------------------
*/
void IN_Shutdown(void)
{
	IN_DeactivateMouse();
}

/*
-----------------------------------------------------------------------------
 Function: IN_Activate -Is the application still active?

 Parameters: active -[in] true if application is active, otherwise false.

 Returns: Nothing.

 Notes:
	Called when the main window gains or loses focus.
	The window may have been destroyed and recreated
	between a deactivate and an activate.
-----------------------------------------------------------------------------
*/
void IN_Activate(_boolean active)
{
	in_appactive = active;
	mouseactive = ! active;	/* force a new window check or turn off */
}

/*
-----------------------------------------------------------------------------
 Function: IN_Frame -Called every frame, even if not generating commands.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
void IN_Frame(void)
{
	if (! mouseinitialized) {
		return;
	}

	if (! in_mouse || ! in_appactive) {
		IN_DeactivateMouse();
		return;
	}

	if (! ClientState.refresh_prepped
		|| (ClientStatic.key_dest == key_console)
		|| (ClientStatic.key_dest == key_menu)) {
		/* temporarily deactivate if in fullscreen */
		if (Cvar_VariableValue("r_fullscreen") == 0) {
			IN_DeactivateMouse();
			return;
		}
	}

	IN_ActivateMouse();
}

/*
-----------------------------------------------------------------------------
 Function: IN_Move -Apply mouse and joy-stick movements to movement command.

 Parameters: cmd -[in] Valid pointer to usercmd_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
void IN_Move(usercmd_t *cmd)
{
	IN_MouseMove(cmd);

#if 0 /* try other conditions */
	if (ActiveApp) {
		IN_JoyMove(cmd);
	}
#endif /* 0 */
}


/*
-----------------------------------------------------------------------------
 Function: IN_ClearStates -Clear input states.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
void IN_ClearStates(void)
{
	mx_accum = 0;
	my_accum = 0;
	mouse_oldbuttonstate = 0;
}

/* EOF */
