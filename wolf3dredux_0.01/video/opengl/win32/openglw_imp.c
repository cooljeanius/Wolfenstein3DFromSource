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
 * 	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * Notes:
 *
 * This file contains ALL Win32 specific stuff having to do with the
 * OpenGL refresh. When a port is being made the following functions
 * must be implemented by the port:
 *
 * GLimp_EndFrame()
 * GLimp_Init()
 * GLimp_Shutdown()
 * GLimp_SwitchFullscreen()
 *
 */

#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <math.h>

/* header guard against Wine's <crtdefs.h> has moved into "../myopengl.h" */
#include "../opengl_local.h"
#include "../myopengl.h"
#include "../opengl_binding.h"

#include "../../video.h"

/* header guard against conflict between Wine's <stdio.h> and Wine's <stdlib.h>
 * has moved to "../../../common/common.h" */
#include "../../../common/common.h"
#include "../../../string/com_string.h"
#include "../../../common/arch.h"

#include "../../../game/app_def.h"



#define	WINDOW_CLASS_NAME	GAME_NAME


static _boolean GLimp_SwitchFullscreen(int width, int height);
#ifdef PRIVATE
PRIVATE _boolean GLimp_InitGL(void);
#else
static _boolean GLimp_InitGL(void);
#endif /* PRIVATE */

glwstate_t glw_state;

extern cvar_t *r_fullscreen;
extern cvar_t *r_ref;


BOOL	RestoreGammaRamp;
WORD	GammaRampBackup[(3 * 256)];

typedef struct
{
	float	Contrast[3];		/* Contrast RGB,	[-1.f ~ 1.f] */
	float	Luminosity[3];		/* Luminosity RGB,	[-1.f ~ 1.f] */
	float	Gamma[3];			/* Gamma RGB,		[-1.f ~ 1.f] */

} CMonitorColorProperties;

CMonitorColorProperties properties;


/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean VerifyDriver(void)
{
	char buffer[1024];

	my_strlcpy(buffer, (char *)pfglGetString(GL_RENDERER), sizeof(buffer));
	(void)my_strlwr(buffer);

	if (strcmp(buffer, "gdi generic") == 0) {
		return false;
	}

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: BackupGammaRamp

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void BackupGammaRamp(void)
{
	HDC hDC;

	// Backup monitor colour parameters
	RestoreGammaRamp = false;
	hDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
	if( hDC )
	{
		RestoreGammaRamp = GetDeviceGammaRamp( hDC, GammaRampBackup );

		if( ! RestoreGammaRamp )
		{
			Com_Printf( "[VID_CreateWindow]: Could NOT GetDeviceGammaRamp\n" );
		}
	}

	properties.Contrast[ 0 ] = 0;
	properties.Contrast[ 1 ] = 0;
	properties.Contrast[ 2 ] = 0;

	properties.Gamma[ 0 ] = 0;
	properties.Gamma[ 1 ] = 0;
	properties.Gamma[ 2 ] = 0;

	properties.Luminosity[ 0 ] = 0;
	properties.Luminosity[ 1 ] = 0;
	properties.Luminosity[ 2 ] = 0;

	if( hDC )
	{
		// The ramp
		WORD ramp[ 256 * 3 ];

		// For each component
		W32 c;
		W32 i;

		for( c = 0; c < 3; ++c )
		for( i = 0; i < 256; ++i )
		{
			float value = (float)(i / 256);

			// Contrast
			value = (float) max( 0.0f, (value-0.5f) * (float) pow( 3.f, properties.Contrast[ c ] ) + 0.5f );

			// Gamma
			value = (float) pow( value, (properties.Gamma[ c ] > 0) ? 1 - 3 * properties.Gamma[ c ] / 4 : 1 - properties.Gamma[ c ] );

			// Luminosity
			value = value + properties.Luminosity[c] / 2.f;

			ramp[i+(c<<8)] = min (65535, max (0, (int)(value * 65535)));
		}


		/* Set new ramp */
		if( SetDeviceGammaRamp( hDC, ramp ) ) {
			Com_Printf( "(GLimp_SetMode): Set new GammaRamp\n");
		} else {
			Com_Printf( "(GLimp_SetMode): Could NOT set new GammaRamp\n");
		}

		DeleteDC( hDC );
	}
}

/*
-----------------------------------------------------------------------------
 Function: VID_CreateWindow -Set window mode fullscreen or windowed.

 Parameters:width -[in] window width.
			height -[in] window height.
			fullscreen -[in] true for fullscreen; false for windowed.


 Returns: true window created, false otherwise.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE _boolean VID_CreateWindow( int width, int height, _boolean fullscreen )
{
	WNDCLASS		wc;
	RECT			r;
	cvar_t			*vid_xpos, *vid_ypos;
	int				stylebits;
	SW32			x, y, w, h;
	int				exstyle;


	/* Register the frame class */
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)glw_state.wndproc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = glw_state.hInstance;
    wc.hIcon         = 0;
    wc.hCursor       = LoadCursor( NULL,IDC_ARROW );
	wc.hbrBackground = (void *)COLOR_GRAYTEXT;
    wc.lpszMenuName  = 0;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if( ! RegisterClass( &wc ) ) {
		Com_Error( ERR_FATAL, "Could NOT register window class" );
	}


	if( fullscreen ) {
		exstyle = WS_EX_TOPMOST;
		stylebits = WS_POPUP | WS_VISIBLE;
	} else {
		exstyle = 0;
		stylebits = WS_CAPTION | WS_BORDER | WS_SYSMENU;
	}

	r.left = 0;
	r.top = 0;
	r.right  = width;
	r.bottom = height;

	AdjustWindowRect( &r, stylebits, FALSE );

	w = r.right - r.left;
	h = r.bottom - r.top;

	if( fullscreen )
	{
		x = 0;
		y = 0;
	}
	else
	{
		vid_xpos = Cvar_Get( "win_xpos", "0", CVAR_INIT );
		vid_ypos = Cvar_Get( "win_ypos", "0", CVAR_INIT );
		x = (SW32)vid_xpos->value;
		y = (SW32)vid_ypos->value;
	}

	glw_state.hWnd = CreateWindowEx(
		 exstyle,
		 WINDOW_CLASS_NAME,
		 WINDOW_CLASS_NAME,
		 stylebits,
		 x, y, w, h,
		 NULL,
		 NULL,
		 glw_state.hInstance,
		 NULL );

	if( ! glw_state.hWnd )
	{
		Com_Error( ERR_FATAL, "Could not create window" );
	}

	ShowWindow( glw_state.hWnd, SW_SHOW );
	UpdateWindow( glw_state.hWnd );

	// init all the OpenGL stuff for the window
	if( ! GLimp_InitGL() )
	{
		Com_Printf( "VID_CreateWindow() - GLimp_InitGL failed\n");
		return false;
	}

	SetForegroundWindow( glw_state.hWnd );
	SetFocus( glw_state.hWnd );

	// let the sound and input subsystems know about the new window
	VID_NewWindow( width, height );


	return true;
}


/*
-----------------------------------------------------------------------------
 Function: GLimp_SetMode -Set window mode fullscreen of windowed.

 Parameters:pwidth -[out] width of window.
			pheight -[out] height of window.
			mode -[in] Which video mode to use. (see vidmode_t vid_modes[])
			fullscreen -[in] true for fullscreen; false for windowed.
						Setting fullscreen higher than 1 will cause a
						buffer overflow.

 Returns: rserr_t value.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC int GLimp_SetMode( int *pwidth, int *pheight, int mode, _boolean fullscreen )
{
	int width, height;
	const char *win_fs[] = { "W", "FS" };



	Com_Printf( "...setting mode %d:", mode );

	if( ! VID_GetModeInfo( &width, &height, mode ) )
	{
		Com_Printf( " invalid mode\n" );

		return rserr_invalid_mode;
	}

	Com_Printf( " %d %d %s\n", width, height, win_fs[ fullscreen ] );

	// destroy the existing window
	if( glw_state.hWnd )
	{
		GLimp_Shutdown();
	}

	// do a CDS if needed
	if( fullscreen )
	{
		DEVMODE dm;

		Com_Printf( "...attempting fullscreen\n" );

		memset( &dm, 0, sizeof( dm ) );

		dm.dmSize = sizeof( dm );

		dm.dmPelsWidth  = width;
		dm.dmPelsHeight = height;
		dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;

		if ( gl_bitdepth->value != 0 )
		{
			dm.dmBitsPerPel = (W32)gl_bitdepth->value;
			dm.dmFields |= DM_BITSPERPEL;
			Com_Printf( "...using gl_bitdepth of %d\n", (int)gl_bitdepth->value );
		}
		else
		{
			HDC hdc = GetDC( NULL );
			int bitspixel = GetDeviceCaps( hdc, BITSPIXEL );

			Com_Printf( "...using desktop display depth of %d\n", bitspixel );

			ReleaseDC( 0, hdc );
		}

		Com_Printf( "...calling CDS: " );
		if( ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) == DISP_CHANGE_SUCCESSFUL )
		{
			*pwidth = width;
			*pheight = height;

			gl_state.fullscreen = true;

			Com_Printf( "ok\n" );

			if( ! VID_CreateWindow( width, height, true ) )
			{
				return rserr_invalid_mode;
			}

			return rserr_ok;
		}
		else
		{
			*pwidth = width;
			*pheight = height;

			Com_Printf( "failed\n" );

			Com_Printf( "...calling CDS assuming dual monitors:" );

			dm.dmPelsWidth = width * 2;
			dm.dmPelsHeight = height;
			dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

			if ( gl_bitdepth->value != 0 )
			{
				dm.dmBitsPerPel = (W32)gl_bitdepth->value;
				dm.dmFields |= DM_BITSPERPEL;
			}

			/*
			** our first CDS failed, so maybe we're running on some weird dual monitor
			** system
			*/
			if( ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			{
				Com_Printf( " failed\n" );

				Com_Printf( "...setting windowed mode\n" );

				ChangeDisplaySettings( 0, 0 );

				*pwidth = width;
				*pheight = height;
				gl_state.fullscreen = false;
				if( ! VID_CreateWindow( width, height, false ) )
				{
					return rserr_invalid_mode;
				}

				return rserr_invalid_fullscreen;
			}
			else
			{
				Com_Printf( " ok\n" );
				if( ! VID_CreateWindow( width, height, true ) )
				{
					return rserr_invalid_mode;
				}

				gl_state.fullscreen = true;
				return rserr_ok;
			}
		}
	}
	else
	{
		Com_Printf( "...setting windowed mode\n" );

		ChangeDisplaySettings( 0, 0 );

		*pwidth = width;
		*pheight = height;
		gl_state.fullscreen = false;
		if( ! VID_CreateWindow( width, height, false ) )
		{
			return rserr_invalid_mode;
		}
	}


	return rserr_ok;

}


/*
-----------------------------------------------------------------------------
 Function: GLimp_Shutdown -Shutdown OS implementation of graphics layer.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	This routine does all OS specific shutdown procedures for the OpenGL
	subsystem. Under OpenGL this means NULLing out the current DC and
	HGLRC, deleting the rendering context, and releasing the DC acquired
	for the window. The state structure is also NULLed out.
-----------------------------------------------------------------------------
*/
PUBLIC void GLimp_Shutdown( void )
{
	if( pfwglMakeCurrent && ! pfwglMakeCurrent( NULL, NULL ) )
		Com_Printf( "ref_gl::R_Shutdown() - wglMakeCurrent failed\n");

	if ( glw_state.hGLRC )
	{
		if (  pfwglDeleteContext && ! pfwglDeleteContext( glw_state.hGLRC ) )
			Com_Printf( "ref_gl::R_Shutdown() - wglDeleteContext failed\n");

		glw_state.hGLRC = NULL;
	}
	// Restore monitor color parameters

	if( RestoreGammaRamp )
	{
		HDC hDC;

		hDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
		if( hDC )
		{
			if( SetDeviceGammaRamp( hDC, GammaRampBackup ) )
			{
				Com_Printf( "(GLimp_Shutdown): Restored GammaRampBackup\n");
			}
			else
			{
				Com_Printf( "(GLimp_Shutdown): GammaRampBackup failed\n");
			}

			DeleteDC( hDC );
		}
	}

	if( glw_state.hDC )
	{
		if ( !ReleaseDC( glw_state.hWnd, glw_state.hDC ) )
			Com_Printf( "ref_gl::R_Shutdown() - ReleaseDC failed\n" );

		glw_state.hDC   = NULL;
	}

	if( glw_state.hWnd )
	{
		ShowWindow( glw_state.hWnd, SW_HIDE );
		DestroyWindow( glw_state.hWnd );
		glw_state.hWnd = NULL;
	}


	UnregisterClass( WINDOW_CLASS_NAME, glw_state.hInstance );

	if ( gl_state.fullscreen )
	{
		ChangeDisplaySettings( 0, 0 );
		gl_state.fullscreen = false;
	}
}


/*
-----------------------------------------------------------------------------
 Function: GLimp_Init	-Initialize OS specific parts of OpenGL.

 Parameters: hinstance -[in] Handle to application instance.
 		     wndproc -[in] Pointer to window procedure.

 Returns: true on success, otherwise false.

 Notes: This routine is responsible for initializing the OS specific portions
	     of OpenGL. Under Win32 this means dealing with the pixelformats and
	     doing the wgl interface stuff.
-----------------------------------------------------------------------------
*/
PUBLIC _boolean GLimp_Init( void *hinstance, void *wndproc )
{
	glw_state.hInstance = ( HINSTANCE ) hinstance;
	glw_state.wndproc = wndproc;

	return true;
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean GLimp_InitGL( void )
{
    PIXELFORMATDESCRIPTOR pfd;
    int  pixelformat;
	W8 Depth;


	// Get a DC for the specified window
	if( glw_state.hDC != NULL )
		Com_Printf( "GLimp_Init() - non-NULL DC exists\n" );

    if ( ( glw_state.hDC = GetDC( glw_state.hWnd ) ) == NULL )
	{
		Com_Printf( "GLimp_Init() - GetDC failed\n" );
		return false;
	}

	Depth = GetDeviceCaps( glw_state.hDC, BITSPIXEL );

	memset( &pfd, 0, sizeof( pfd ) );
	pfd.nSize		= sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= (char)Depth;

	// Choose depth buffer.
	if( Depth <= 16 )
	{
		pfd.cDepthBits = 16;
	}
	else
	{
		pfd.cDepthBits = 24;
		pfd.cAlphaBits = 8;
	}

	pfd.iLayerType = PFD_MAIN_PLANE;


	if ( ( pixelformat = ChoosePixelFormat( glw_state.hDC, &pfd)) == 0 )
	{
		Com_Printf( "GLimp_Init() - ChoosePixelFormat failed\n");
		return false;
	}
	if ( SetPixelFormat( glw_state.hDC, pixelformat, &pfd) == FALSE )
	{
		Com_Printf( "GLimp_Init() - SetPixelFormat failed\n");
		return false;
	}
	DescribePixelFormat( glw_state.hDC, pixelformat, sizeof( pfd ), &pfd );



	//
	//	 Create an OpenGL context and make it current
	//
	if( ( glw_state.hGLRC = pfwglCreateContext( glw_state.hDC ) ) == 0 )
	{
		Com_Printf( "GLimp_Init() - pfwglCreateContext failed\n");
		goto fail;
	}

    if( ! pfwglMakeCurrent( glw_state.hDC, glw_state.hGLRC ) )
	{
		Com_Printf( "GLimp_Init() - wglMakeCurrent failed\n");
		goto fail;
	}

	if( ! VerifyDriver() )
	{
		Com_Printf( "GLimp_Init() - no hardware acceleration detected\n" );
		goto fail;
	}


	//	Print out Pixel Format Description specifics
	Com_Printf( "PIXELFORMAT: colour(%d-bits) Z(%d-bit) stencil(%d-bits)\n", (int)pfd.cColorBits, (int)pfd.cDepthBits, (int)pfd.cStencilBits );

	return true;

fail:
	if( glw_state.hGLRC )
	{
		pfwglDeleteContext( glw_state.hGLRC );
		glw_state.hGLRC = NULL;
	}

	if( glw_state.hDC )
	{
		ReleaseDC( glw_state.hWnd, glw_state.hDC );
		glw_state.hDC = NULL;
	}

	return false;
}

/*
-----------------------------------------------------------------------------
 Function: GLimp_BeginFrame -Called before frame rendering.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	Responsible for doing a swapbuffers and possibly for other stuff
	as yet to be determined. Probably better not to make this a GLimp
	function and instead do a call to GLimp_SwapBuffers.
-----------------------------------------------------------------------------
*/
PUBLIC void GLimp_BeginFrame( void )
{
	if( gl_bitdepth->modified )
	{
		gl_bitdepth->modified = false;
	}

	pfglDrawBuffer( GL_BACK );

}


/*
-----------------------------------------------------------------------------
 Function: GLimp_EndFrame -Called at the end of frame rendering.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	Responsible for doing a swapbuffers and possibly for other stuff
	as yet to be determined. Probably better not to make this a GLimp
	function and instead do a call to GLimp_SwapBuffers.
-----------------------------------------------------------------------------
*/
void GLimp_EndFrame( void )
{
	W32	err;

	err = pfglGetError();
	if( err != GL_NO_ERROR )
	{
		PrintGLError( err, "GLimp_EndFrame()" );
	}


	if ( stricmp( gl_drawbuffer->string, "GL_BACK" ) == 0 )
	{
		if ( ! pfwglSwapBuffers( glw_state.hDC ) )
			Com_Error( ERR_FATAL, "GLimp_EndFrame() - SwapBuffers() failed!\n" );
	}
}


/*
-----------------------------------------------------------------------------
 Function: GLimp_AppActivate -Called when and the window is being activated
								or deactivated windows.

 Parameters: active -[in] true if application is active, false otherwise.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
void GLimp_AppActivate(_boolean active)
{
	if (active) {
		SetForegroundWindow(glw_state.hWnd);
		ShowWindow(glw_state.hWnd, SW_RESTORE);
	} else {
		if (r_fullscreen->value) {
			ShowWindow(glw_state.hWnd, SW_MINIMIZE);
		}
	}
}

/* EOF */
