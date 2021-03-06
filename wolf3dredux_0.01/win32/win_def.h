/*
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 * 	as published by the Free Software Foundation; either version 2
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
 *	windef.h:  Win32 specific header file,
 *
 *	Author:	Id Software, Inc.
 *	Date:	1997-2001
 *
 */


#ifndef __WIN_DEF_H__
#define __WIN_DEF_H__


#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>


#include "../common/arch.h"


#define	WINDOW_STYLE ( WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE )

extern	HINSTANCE	global_hInstance;


extern HWND			cl_hwnd;
extern _boolean		ActiveApp, Minimized;

extern void IN_Activate(_boolean active);
extern void IN_MouseEvent(int mstate);

extern int		window_center_x, window_center_y;
extern RECT		window_rect;


/* prototypes for functions from "win_main.c": */
extern void ParseCommandLine(LPSTR lpCmdLine);
extern void CheckforInstance(void);


#endif /* __WIN_DEF_H__ */
