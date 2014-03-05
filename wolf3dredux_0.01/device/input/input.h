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
 *	input.h:	External (non-keyboard) input devices.
 *
 *	Author:  Id Software, Inc.
 *	Date:    1997-2001
 *
 */


#ifndef __INPUT_H__
#define __INPUT_H__

#include "../../common/arch.h"
#include "../../client/client.h"


extern void IN_Init( void );

extern void IN_Shutdown( void );

extern void IN_Commands( void );
/* oportunity for devices to stick commands on the script buffer */

extern void IN_Frame( void );

extern void IN_Move( usercmd_t *cmd );
/* add additional movement on top of the keyboard move cmd */

extern void IN_Activate( _boolean active );

extern void IN_ClearStates( void );

/* this one is implemented in either unix_joystick.c or win_joystick.c,
 * depending on platform: */
extern void IN_StartupJoystick( void );

/* these only exist for unix so far, in unix_keys.c: */
#if defined(__unix__) || defined(__APPLE__)
extern void Do_Key_Event( int key, _boolean down );
extern void KBD_Init( void );
extern void KBD_Update( void );
extern void KBD_Close(void);
#endif /* __unix__ || __APPLE__ */

#endif /* __INPUT_H__ */

/* EOF */
