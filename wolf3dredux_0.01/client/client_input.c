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
 *	client_input.c: Builds an intended movement command to send to the server.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */


#include <stdlib.h>
#include <string.h>

#include "keys.h"
#include "../device/input/input.h"
#include "client.h"

cvar_t	*cl_nodelta;

extern	W32	sys_frame_time;
PRIVATE W32	frame_msec;
PRIVATE W32	old_sys_frame_time;

/*
===============================================================================

	KEY BUTTONS

	Continuous button event tracking is complicated by the fact that two different
	input sources (say, mouse button 1 and the control key) can both press the
	same button, but the button should only be released when both of the
	pressing key have been released.

	When a key event issues a button command (+forward, +attack, etc), it appends
	its key number as a parameter to the command so it can be matched up with
	the release.

	state bit 0 is the current state of the key
	state bit 1 is edge triggered on the up to down transition
	state bit 2 is edge triggered on the down to up transition


	Key_Event (int key, qboolean down, unsigned time);

	  +mlook src time

===============================================================================
*/


kbutton_t	in_klook;
kbutton_t	in_left, in_right, in_forward, in_back;
kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
kbutton_t	in_strafe, in_speed, in_use, in_attack;
kbutton_t	in_up, in_down;

int			in_impulse;


/*
-----------------------------------------------------------------------------
 Function: KeyDown -Called whenever a key goes into the down state.

 Parameters: b -[in] Valid pointer to a kbutton_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void KeyDown( kbutton_t *b )
{
	int		k;
	char	*c;

	c = Cmd_Argv( 1 );
	if( c[ 0 ] ) {
		k = atoi(c);
	} else {
		k = -1;	/* typed manually at the console for continuous down */
	}

	if( k == b->down[ 0 ] || k == b->down[ 1 ] ) {
		return;		/* repeating key */
	}

	if( ! b->down[ 0 ] ) {
		b->down[ 0 ] = k;
	} else if( ! b->down[ 1 ] ) {
		b->down[ 1 ] = k;
	} else {
		Com_Printf( "Three keys down for a button!\n" );
		return;
	}

	if( b->state & 1 ) {
		return;		/* still down */
	}

	/* save timestamp */
	c = Cmd_Argv( 2 );
	b->downtime = (unsigned int)atoi( c );
	if( ! b->downtime ) {
		b->downtime = ((unsigned int)sys_frame_time - 100);
	}

	b->state |= 1 + 2;	/* down + impulse down */

}


/*
-----------------------------------------------------------------------------
 Function: KeyUp -Called whenever a key goes into the up state.

 Parameters: b -[in] Valid pointer to a kbutton_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void KeyUp( kbutton_t *b )
{
	int		k;
	char	*c;
	unsigned	uptime;

	c = Cmd_Argv( 1 );
	if( c[ 0 ] ) {
		k = atoi( c );
	} else {
		/* typed manually at the console, assume for unsticking, so clear all */
		b->down[ 0 ] = b->down[ 1 ] = 0;
		b->state = 4;	/* impulse up */
		return;
	}

	if( b->down[ 0 ] == k ) {
		b->down[ 0 ] = 0;
	} else if( b->down[ 1 ] == k ) {
		b->down[ 1 ] = 0;
	} else {
		return;		/* key up without coresponding down (menu pass through) */
	}

	if( b->down[ 0 ] || b->down[ 1 ] ) {
		return;		/* some other key is still holding it down */
	}

	if( ! (b->state & 1) ) {
		return;		/* still up (this should not happen) */
	}

	/* save timestamp */
	c = Cmd_Argv( 2 );
	uptime = (unsigned int)atoi( c );
	if( uptime ) {
		b->msec += uptime - b->downtime;
	} else {
		b->msec += 10;
	}

	b->state &= ~1;		/* now up */
	b->state |= 4; 		/* impulse up */
}

void IN_KLookDown( void )		{ KeyDown( &in_klook ); }
void IN_KLookUp( void )			{ KeyUp( &in_klook ); }
void IN_UpDown( void )			{ KeyDown( &in_up ); }
void IN_UpUp( void )			{ KeyUp( &in_up ); }
void IN_DownDown( void )		{ KeyDown( &in_down ); }
void IN_DownUp( void )			{ KeyUp( &in_down ); }
void IN_LeftDown( void )		{ KeyDown( &in_left ); }
void IN_LeftUp( void )			{ KeyUp( &in_left ); }
void IN_RightDown( void )		{ KeyDown( &in_right ); }
void IN_RightUp( void )			{ KeyUp( &in_right ); }
void IN_ForwardDown( void )		{ KeyDown( &in_forward ); }
void IN_ForwardUp( void )		{ KeyUp( &in_forward ); }
void IN_BackDown( void )		{ KeyDown( &in_back ); }
void IN_BackUp( void )			{ KeyUp( &in_back ); }
void IN_LookupDown( void )		{ KeyDown( &in_lookup ); }
void IN_LookupUp( void )		{ KeyUp( &in_lookup ); }
void IN_LookdownDown( void )	{ KeyDown( &in_lookdown ); }
void IN_LookdownUp( void )		{ KeyUp( &in_lookdown ); }
void IN_MoveleftDown( void )	{ KeyDown( &in_moveleft ); }
void IN_MoveleftUp( void )		{ KeyUp( &in_moveleft ); }
void IN_MoverightDown( void )	{ KeyDown( &in_moveright ); }
void IN_MoverightUp( void )		{ KeyUp( &in_moveright ); }

void IN_SpeedDown( void )	{ KeyDown( &in_speed ); }
void IN_SpeedUp( void )		{ KeyUp( &in_speed ); }
void IN_StrafeDown( void )	{ KeyDown( &in_strafe ); }
void IN_StrafeUp( void )	{ KeyUp( &in_strafe ); }

void IN_AttackDown( void )	{ KeyDown( &in_attack ); }
void IN_AttackUp( void )	{ KeyUp( &in_attack ); }

void IN_UseDown( void ) { KeyDown( &in_use ); }
void IN_UseUp( void )	{ KeyUp( &in_use ); }

void IN_Impulse( void )	{ in_impulse = atoi( Cmd_Argv( 1 ) ); }


/*
-----------------------------------------------------------------------------
 Function: Client_KeyState -Key state tracking.

 Parameters: key -[in] Valid pointer to kbutton_t structure.

 Returns: Returns the fraction of the frame that the key was down.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE float Client_KeyState(kbutton_t *key)
{
	float val;
	_boolean impulsedown, impulseup, down;

	val = 0;

	impulsedown = (key->state & 2);
	impulseup = (key->state & 4);
	down = (key->state & 1);

	if (impulsedown && ! impulseup) {
		if (down) {
			val = (float)0.5; /* pressed and held this frame */
		} else {
			val = 0;
		}
	}

	if (impulseup && ! impulsedown) {
		if (down) {
			val = 0;
		} else {
			val = 0;	/* released this frame */
		}
	}

	if (! impulsedown && ! impulseup) {
		if( down ) {
			val = (float)1.0;	/* held the entire frame */
		} else {
			val = 0;	/* up the entire frame */
		}
	}

	if (impulsedown && impulseup) {
		if (down) {
			val = (float)0.75;	/* released and re-pressed this frame */
		} else {
			val = (float)0.25;	/* pressed and released this frame */
		}
	}

	key->state &= 1; /* clear impulses */

	return val;
}




/*==========================================================================*/

cvar_t	*cl_upspeed;
cvar_t	*cl_forwardspeed;
cvar_t	*cl_sidespeed;

cvar_t	*cl_yawspeed;
cvar_t	*cl_pitchspeed;

cvar_t	*cl_rotatespeed;

cvar_t	*cl_run;

cvar_t	*cl_anglespeedkey;



/*
-----------------------------------------------------------------------------
 Function: Client_AdjustAngles -Moves the local angle positions.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void Client_AdjustAngles( void )
{
	float	speed;
	float	up, down;

	if(( ! cl_run->value && in_speed.state & 1) || (cl_run->value && ! (in_speed.state & 1)) ) {
		speed = (float)ClientStatic.frametime * cl_anglespeedkey->value;
	} else {
		speed = (float)ClientStatic.frametime;
	}

	if( ! (in_strafe.state & 1) ) {
		ClientState.viewangles[ YAW ] += (int)(speed * cl_rotatespeed->value * (Client_KeyState(&in_left) - Client_KeyState(&in_right)));
	}

	if( in_klook.state & 1 ) {
		ClientState.viewangles[ PITCH ] -= speed * cl_pitchspeed->value * Client_KeyState( &in_forward );
		ClientState.viewangles[ PITCH ] += speed * cl_pitchspeed->value * Client_KeyState( &in_back );
	}

	up = Client_KeyState( &in_lookup );
	down = Client_KeyState( &in_lookdown );

	ClientState.viewangles[ PITCH ] -= speed * cl_pitchspeed->value * up;
	ClientState.viewangles[ PITCH ] += speed * cl_pitchspeed->value * down;
}


/*
-----------------------------------------------------------------------------
 Function: Client_BaseMove -Send the intended movement message to the server.

 Parameters: cmd -[in] Pointer to valid usercmd_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void Client_BaseMove(usercmd_t *cmd)
{
	ClientState.viewangles[YAW] = 0;

	Client_AdjustAngles();

	memset(cmd, 0, sizeof(usercmd_t));

#if defined(vectorCopy) && 0
	/* casting the first argument here to silence warnings leads to errors: */
	vectorCopy(ClientState.viewangles, cmd->angles); /* FIXME (?) */
#else
	/* for some reason casting only works when the define is expanded: */
	((cmd->angles)[0] = (short)(ClientState.viewangles)[0],
	 (cmd->angles)[1] = (short)(ClientState.viewangles)[1],
	 (cmd->angles)[2] = (short)(ClientState.viewangles)[2]);
#endif /* vectorCopy && 0 */

	if (in_strafe.state & 1) {
		cmd->sidemove += (cl_sidespeed->value * Client_KeyState(&in_right));
		cmd->sidemove -= (cl_sidespeed->value * Client_KeyState(&in_left));
	}

	cmd->sidemove += (cl_sidespeed->value * Client_KeyState(&in_moveright));
	cmd->sidemove -= (cl_sidespeed->value * Client_KeyState(&in_moveleft));

	cmd->forwardmove += (cl_forwardspeed->value * Client_KeyState(&in_forward));
	cmd->forwardmove -= (cl_forwardspeed->value * Client_KeyState(&in_back));

/*
 * adjust for speed key / running
 */
	/* TODO: check to see if "^" is being used for blocks syntax: */
	if ((in_speed.state & 1) ^ (int)(cl_run->value)) {
		cmd->forwardmove *= 1.5;
		cmd->sidemove *= 1.5;
		cmd->upmove *= 1.5;
	}
}


/*
-----------------------------------------------------------------------------
 Function: Client_FinishMove -Final movement adjustments applied to move
							 command.

 Parameters: cmd -[in] Valid pointer to usercmd_t structure.

 Returns:Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void Client_FinishMove( usercmd_t *cmd )
{
	int		ms;
	int		i;

/*
 * figure button bits
 */
	if( in_attack.state & 3 ) {
		cmd->buttons |= BUTTON_ATTACK;
	}
	in_attack.state &= ~2;

	if( in_use.state & 3 ) {
		cmd->buttons |= BUTTON_USE;
	}
	in_use.state &= ~2;

	if( anykeydown && ClientStatic.key_dest == key_game ) {
		cmd->buttons |= BUTTON_ANY;
	}

	/* send milliseconds of time to apply the move */
	ms = (int)ClientStatic.frametime * 1000;
	if( ms > 250 ) {
		ms = 100; /* time was unreasonable */
	}
	cmd->msec = (W8)ms;


	for( i = 0 ; i < 3 ; ++i ) {
		/* you would think that with a name like "ANGLE2SHORT", it would already
		 * return a 'short' even without the cast, but nope: */
		cmd->angles[ i ] = (short)ANGLE2SHORT( ClientState.viewangles[ i ] );
	}

	cmd->impulse = (W8)in_impulse;
	in_impulse = 0;

/* send the ambient light level at the player's current position */
#if 0
	cmd->lightlevel = (W8)cl_lightlevel->value;
#endif /* 0 */
}

/*
-----------------------------------------------------------------------------
 Function: Client_CreateCmd -Create the movement command.

 Parameters: Nothing.

 Returns: Filled out usercmd_t structure.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE usercmd_t Client_CreateCmd( void )
{
	usercmd_t	cmd;

	frame_msec = sys_frame_time - old_sys_frame_time;
	if (frame_msec < 1) {
		frame_msec = 1;
	}

	if (frame_msec > 200) {
		frame_msec = 200;
	}

	/* get basic movement from keyboard */
	Client_BaseMove( &cmd );

	/* allow mice or other external controllers to add to the move */
	IN_Move( &cmd );

	Client_FinishMove( &cmd );

	old_sys_frame_time = sys_frame_time;

#if 0
	cmd.impulse = ClientStatic.framecount;
#endif /* 0 */

	return cmd;
}


void IN_CenterView (void)
{
#if 0
	ClientState.viewangles[ PITCH ] = -SHORT2ANGLE( ClientState.frame.playerstate.pmove.delta_angles[ PITCH ] );
#else
	;
#endif /* 0 */
}

/*
-----------------------------------------------------------------------------
 Function: Client_InitInput -Initialize input commands.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Just a bunch of calls to Cmd_AddCommand() so far...
-----------------------------------------------------------------------------
*/
PUBLIC void Client_InitInput(void)
{
#if 0
	Cmd_AddCommand("centerview", IN_CenterView);
#endif /* 0 */

	Cmd_AddCommand("+moveup", IN_UpDown);
	Cmd_AddCommand("-moveup", IN_UpUp);
	Cmd_AddCommand("+movedown", IN_DownDown);
	Cmd_AddCommand("-movedown", IN_DownUp);
	Cmd_AddCommand("+left", IN_LeftDown);
	Cmd_AddCommand("-left", IN_LeftUp);
	Cmd_AddCommand("+right", IN_RightDown);
	Cmd_AddCommand("-right", IN_RightUp);
	Cmd_AddCommand("+forward", IN_ForwardDown);
	Cmd_AddCommand("-forward", IN_ForwardUp);
	Cmd_AddCommand("+back", IN_BackDown);
	Cmd_AddCommand("-back", IN_BackUp);
	Cmd_AddCommand("+lookup", IN_LookupDown);
	Cmd_AddCommand("-lookup", IN_LookupUp);
	Cmd_AddCommand("+lookdown", IN_LookdownDown);
	Cmd_AddCommand("-lookdown", IN_LookdownUp);
	Cmd_AddCommand("+strafe", IN_StrafeDown);
	Cmd_AddCommand("-strafe", IN_StrafeUp);
	Cmd_AddCommand("+moveleft", IN_MoveleftDown);
	Cmd_AddCommand("-moveleft", IN_MoveleftUp);
	Cmd_AddCommand("+moveright", IN_MoverightDown);
	Cmd_AddCommand("-moveright", IN_MoverightUp);
	Cmd_AddCommand("+speed", IN_SpeedDown);
	Cmd_AddCommand("-speed", IN_SpeedUp);
	Cmd_AddCommand("+attack", IN_AttackDown);
	Cmd_AddCommand("-attack", IN_AttackUp);
	Cmd_AddCommand("+use", IN_UseDown);
	Cmd_AddCommand("-use", IN_UseUp);
	Cmd_AddCommand("impulse", IN_Impulse);
	Cmd_AddCommand("+klook", IN_KLookDown);
	Cmd_AddCommand("-klook", IN_KLookUp);

	cl_nodelta = Cvar_Get("cl_nodelta", "0", CVAR_INIT);
}


/*
-----------------------------------------------------------------------------
 Function: Client_SendCommand -Create a movement command to send to the server

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Client_SendCommand( void )
{
	usercmd_t	*cmd;

	cmd = &ClientState.cmds[ 0 ];
	ClientState.cmd_time[ 0 ] = ClientStatic.realtime;	/* for netgraph ping calculation */

	*cmd = Client_CreateCmd();

	ClientState.cmd = *cmd;


	return;
}

/* EOF */
