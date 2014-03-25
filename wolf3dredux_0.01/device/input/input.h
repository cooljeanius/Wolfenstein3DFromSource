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


extern void IN_Init(void);

extern void IN_Shutdown(void);

extern void IN_Commands(void);
/* oportunity for devices to stick commands on the script buffer */

extern void IN_Frame(void);

extern void IN_Move(usercmd_t *cmd);
/* add additional movement on top of the keyboard move cmd */

extern void IN_Activate(_boolean active);

extern void IN_ClearStates(void);

/* this one is implemented in either unix_joystick.c or win_joystick.c,
 * depending on platform: */
extern void IN_StartupJoystick(void);

/* these ones are implemented in either unix_mouse.c or win_mouse.c,
 * depending on platform: */
extern void IN_StartupMouse(void);
extern void IN_ActivateMouse(void);
extern void IN_DeactivateMouse(void);
extern void IN_MouseMove(usercmd_t *cmd);

/* these only exist for unix so far: */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(_WIN32)
/* these ones are from unix_keys.c: */
extern void Do_Key_Event(int key, _boolean down);
extern void KBD_Init(void);
extern void KBD_Update(void);
extern void KBD_Close(void);
/* these ones are from unix_mouse.c: */
extern void install_grabs(void);
extern void uninstall_grabs(void);
extern void RW_IN_Activate(_boolean active);
extern void RW_IN_Shutdown(void);
#endif /* (__unix__ || __APPLE__) && !_WIN32 */

/* these only exist for Windows so far: */
#ifdef _WIN32
# if defined(HAVE_WINDOWS_H) || defined(_MSC_VER)
#  define WIN32_LEAN_AND_MEAN 1
#  include <windows.h>
# endif /* HAVE_WINDOWS_H || _MSC_VER */
/* these ones are from win_joystick.c: */
# if defined(PDWORD) || defined(HAVE_PDWORD) || defined(_MSC_VER)
extern PDWORD RawValuePointer(int axis);
# elif defined(HAVE_UINT32_T)
extern uint32_t RawValuePointer(int axis);
# else
extern unsigned int RawValuePointer(int axis);
# endif /* (PDWORD || HAVE_PDWORD || _MSC_VER) || HAVE_UINT32_T */
extern void Joy_AdvancedUpdate_f(void);
extern _boolean IN_ReadJoystick(void);
extern void IN_JoyMove(usercmd_t *cmd);
/* these ones are from win_keys.c: */
extern void WIN_DisableAltTab(void);
extern void WIN_EnableAltTab(void);
extern void AppActivate(BOOL fActive, BOOL minimize);
# if defined(LONG) || defined(_MSC_VER)
extern LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
# else
extern long WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
# endif /* LONG || _MSC_VER */
/* these ones are from win_mouse.c: */
extern void IN_MouseLookDown(void);
extern void IN_MouseLookUp(void);
extern void IN_MouseEvent(int mstate);
#endif /* _WIN32 */

#endif /* __INPUT_H__ */

/* EOF */
