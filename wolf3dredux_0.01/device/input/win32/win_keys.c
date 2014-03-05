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
 *      win_keys.c:   Windows key input layer
 *
 *      Author:     Id Software, Inc.
 *      Date:       1997-2001
 *
 */

#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "../input.h"
#include "../../../win32/win_def.h"
#include "../../../common/arch.h"
#include "../../../common/common_utils.h"
#include "../../../common/common.h"
#include "../../../common/cvar.h"
#include "../../../client/keys.h"
#include "../../../video/renderer.h"
#include "../../../sound/sound.h"


HWND	cl_hwnd;	/* Main window handle for life of program */

cvar_t *win_noalttab;

extern cvar_t	*vid_xpos; /* X coordinate of window position */
extern cvar_t	*vid_ypos; /* Y coordinate of window position */
extern cvar_t	*r_fullscreen;

#ifndef WM_MOUSEWHEEL
# define WM_MOUSEWHEEL (WM_MOUSELAST+1)  /* message that will be supported by the OS */
#endif /* !WM_MOUSEWHEEL */

static UINT MSH_MOUSEWHEEL;


static _boolean s_alttab_disabled;

extern	unsigned	sys_msg_time;


/*
** WIN32 helper functions
*/
extern _boolean s_win95;



/*
-----------------------------------------------------------------------------
 Function: WIN_DisableAltTab

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
void WIN_DisableAltTab( void )
{
	if( s_alttab_disabled ) {
		return;
	}

	if( s_win95 ) {
		BOOL old;

		SystemParametersInfo( SPI_SCREENSAVERRUNNING, 1, &old, 0 );
	} else {
		RegisterHotKey( 0, 0, MOD_ALT, VK_TAB );
		RegisterHotKey( 0, 1, MOD_ALT, VK_RETURN );
	}
	s_alttab_disabled = true;
}

/*
-----------------------------------------------------------------------------
 Function: WIN_EnableAltTab

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void WIN_EnableAltTab( void )
{
	if ( s_alttab_disabled ) {
		if ( s_win95 ) {
			BOOL old;

			SystemParametersInfo( SPI_SCREENSAVERRUNNING, 0, &old, 0 );
		} else {
			UnregisterHotKey( 0, 0 );
			UnregisterHotKey( 0, 1 );
		}

		s_alttab_disabled = false;
	}
}




W8	scantokey[ 128 ] =
					{
/*  0		1       2       3       4       5       6       7 */
/*  8		9		A       B       C       D       E       F */
	0,		27,     '1',    '2',    '3',    '4',    '5',    '6',
	'7',	'8',    '9',    '0',    '-',    '=',    K_BACKSPACE, 9,		/* 0 */
	'q',	'w',    'e',    'r',    't',    'y',    'u',    'i',
	'o',	'p',    '[',    ']',    13 ,    K_CTRL,	'a',	's',		/* 1 */
	'd',	'f',    'g',    'h',    'j',    'k',    'l',    ';',
	'\'',	'`',    K_SHIFT,'\\',	'z',    'x',    'c',    'v',		/* 2 */
	'b',    'n',    'm',    ',',    '.',    '/',    K_SHIFT,'*',
	K_ALT,' ',   0  ,    K_F1, K_F2, K_F3, K_F4, K_F5,					/* 3 */
	K_F6, K_F7, K_F8, K_F9, K_F10,  K_PAUSE,    0  , K_HOME,
	K_UPARROW,K_PGUP,K_KP_MINUS,K_LEFTARROW,K_KP_5,K_RIGHTARROW, K_KP_PLUS,K_END, /*4*/
	K_DOWNARROW,K_PGDN,K_INS,K_DEL,0,0,             0,              K_F11,
	K_F12,0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,			/* 5 */
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,			/* 6 */
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0			/* 7 */

};


/*
-----------------------------------------------------------------------------
 Function: MapKey -Map from windows to application keynums.

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE int MapKey( int key )
{
	int result;
	int modified = ( key >> 16 ) & 255;
	_boolean is_extended = false;

	if( modified > 127) {
		return 0;
	}

	if( key & ( 1 << 24 ) ) {
		is_extended = true;
	}

	result = scantokey[ modified ];

	if( ! is_extended ) {
		switch ( result ) {
		case K_HOME:
			return K_KP_HOME;

		case K_UPARROW:
			return K_KP_UPARROW;

		case K_PGUP:
			return K_KP_PGUP;

		case K_LEFTARROW:
			return K_KP_LEFTARROW;

		case K_RIGHTARROW:
			return K_KP_RIGHTARROW;

		case K_END:
			return K_KP_END;

		case K_DOWNARROW:
			return K_KP_DOWNARROW;

		case K_PGDN:
			return K_KP_PGDN;

		case K_INS:
			return K_KP_INS;

		case K_DEL:
			return K_KP_DEL;

		default:
			return result;
		}
	} else {
		switch( result ) {
		case 0x0D:
			return K_KP_ENTER;

		case 0x2F:
			return K_KP_SLASH;

		case 0xAF:
			return K_KP_PLUS;
		}
		return result;
	}
}

/*
-----------------------------------------------------------------------------
 Function: AppActivate

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void AppActivate( BOOL fActive, BOOL minimize )
{
	Minimized = minimize;

	Key_ClearStates();

	/* we do NOT want to act like we are/were active if we are/were minimized */
	if( fActive && ! Minimized ) {
		ActiveApp = true;
	} else {
		ActiveApp = false;
	}

	/* minimize/restore mouse-capture on demand */
	if( ! ActiveApp ) {
		IN_Activate( false );
/*		CDAudio_Activate (false); */
		Sound_Activate( false );

		if ( win_noalttab->value ) {
			WIN_EnableAltTab();
		}
	} else {
		IN_Activate( true );
/*		CDAudio_Activate (true); */
		Sound_Activate( true );
		if ( win_noalttab->value ) {
			WIN_DisableAltTab();
		}
	}
}

/*
-----------------------------------------------------------------------------
 Function: MainWndProc -Main window procedure

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
LONG WINAPI MainWndProc( HWND    hWnd,
						 UINT    uMsg,
						 WPARAM  wParam,
						 LPARAM  lParam)
{
	LONG lRet = 0;

	if( uMsg == MSH_MOUSEWHEEL )
	{
		if( ( ( int ) wParam ) > 0 )
		{
			Key_Event( K_MWHEELUP, true, sys_msg_time );
			Key_Event( K_MWHEELUP, false, sys_msg_time );
		}
		else
		{
			Key_Event( K_MWHEELDOWN, true, sys_msg_time );
			Key_Event( K_MWHEELDOWN, false, sys_msg_time );
		}

        return DefWindowProc (hWnd, uMsg, wParam, lParam);
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		/*
		** this chunk of code theoretically only works under NT4 and Win98
		** since this message does NOT exist under Win95
		*/
		if ( ( short ) HIWORD( wParam ) > 0 )
		{
			Key_Event( K_MWHEELUP, true, sys_msg_time );
			Key_Event( K_MWHEELUP, false, sys_msg_time );
		}
		else
		{
			Key_Event( K_MWHEELDOWN, true, sys_msg_time );
			Key_Event( K_MWHEELDOWN, false, sys_msg_time );
		}
		break;

	case WM_HOTKEY:
		return 0;

	case WM_CREATE:
		cl_hwnd = hWnd;

		MSH_MOUSEWHEEL = RegisterWindowMessage( "MSWHEEL_ROLLMSG" );
        return DefWindowProc( hWnd, uMsg, wParam, lParam );

	case WM_PAINT:
#if 0
		SCR_DirtyScreen();	/* force entire screen to update next frame */
#endif /* 0 */
        return DefWindowProc (hWnd, uMsg, wParam, lParam);

	case WM_CLOSE:
		Com_Quit();
		return 0;

	case WM_DESTROY:
		/* let sound and input know about this? */
		cl_hwnd = NULL;
        return DefWindowProc (hWnd, uMsg, wParam, lParam);

	case WM_ACTIVATE:
		{
			int	fActive, fMinimized;

			/* KJB: Watch this for problems in fullscreen modes with Alt-tabbing. */
			fActive = LOWORD(wParam);
			fMinimized = (BOOL) HIWORD(wParam);

			AppActivate( (fActive != WA_INACTIVE), fMinimized );

			R_AppActivate( ! (fActive == WA_INACTIVE) );
		}
        return 0;

	case WM_MOVE:
		{
			int		xPos, yPos;
			RECT r;
			int		style;

			if( ! r_fullscreen->value )
			{
				xPos = (short) LOWORD( lParam ); /* horizontal position */
				yPos = (short) HIWORD( lParam ); /* vertical position */

				r.left   = 0;
				r.top    = 0;
				r.right  = 1;
				r.bottom = 1;

				style = GetWindowLong( hWnd, GWL_STYLE );
				AdjustWindowRect( &r, style, FALSE );

				Cvar_SetValue( "win_xpos", (float)(xPos + r.left) );
				Cvar_SetValue( "win_ypos", (float)(yPos + r.top) );
				vid_xpos->modified = false;
				vid_ypos->modified = false;
				if( ActiveApp )
					IN_Activate( true );
			}
		}
        return DefWindowProc( hWnd, uMsg, wParam, lParam );

/* This is complicated because Win32 seems to pack multiple mouse events into
 * one update sometimes, so we always check all states and look for events: */
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		{
			int	temp;

			temp = 0;

			if( wParam & MK_LBUTTON ) {
				temp |= 1;
			}

			if( wParam & MK_RBUTTON ) {
				temp |= 2;
			}

			if( wParam & MK_MBUTTON ) {
				temp |= 4;
			}

			IN_MouseEvent( temp );
		}
		break;

	case WM_SYSCOMMAND:
			if( wParam == SC_SCREENSAVE ) {
				return 0;
			}
        return DefWindowProc( hWnd, uMsg, wParam, lParam );

	case WM_SYSKEYDOWN:
		if( wParam == 13 )
		{
			if( r_fullscreen )
			{
				Cvar_SetValue( "r_fullscreen", (float)(! r_fullscreen->value) );
			}
			return 0;
		}
		/* fall through */
	case WM_KEYDOWN:
		Key_Event( MapKey( lParam ), true, sys_msg_time );
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		Key_Event( MapKey( lParam ), false, sys_msg_time );
		break;

#if 0
	case MM_MCINOTIFY:
		{
			LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			lRet = CDAudio_MessageHandler (hWnd, uMsg, wParam, lParam);
		}
		break;
#endif /* 0 */

	default:	/* pass all unhandled messages to DefWindowProc */
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }

    /* return 0 if handled message, 1 if not */
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/* EOF */
