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
 *	unix_mouse.c: unix mouse input layer.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>

#include <X11/keysym.h>
#include <X11/cursorfont.h>

#ifdef HAVE_X11_EXTENSIONS_XF86DGA_H
# include <X11/extensions/xf86dga.h>
#elif defined HAVE_X11_EXTENSIONS_XXF86DGA_H || 1
# include <X11/extensions/Xxf86dga.h>
#else
# warning unix_mouse.c expects a certain X11 header to be included, check the ifdefs
#endif /* HAVE_X11_EXTENSIONS_[X|XX]F86DGA_H */
#include <X11/extensions/xf86vmode.h>

#include "../../../client/keys.h"
#include "../../../common/common_utils.h"
#include "../../../client/client.h"
#include "../../../video/video.h"
#include "../../../string/com_string.h"

#include "../input.h" /* should have the prototype for install_grabs(), and others */

extern Display *display;
extern Window mainwin;

extern _boolean vidmode_active;

PRIVATE _boolean        mouse_avail;
int mx, my; /* TODO: consider changing these ints to floating-point-type vars? */
PRIVATE int old_mouse_x, old_mouse_y;

PRIVATE cvar_t	*m_filter;
PRIVATE cvar_t	*in_mouse;
PRIVATE cvar_t	*in_dgamouse;

PRIVATE _boolean	mlooking;

_boolean mouse_active = false;
_boolean dgamouse = false;

#ifndef __CLIENT_H__
PRIVATE cvar_t *sensitivity;
PRIVATE cvar_t *lookstrafe;
PRIVATE cvar_t *m_side;
PRIVATE cvar_t *m_yaw;
PRIVATE cvar_t *m_pitch;
PRIVATE cvar_t *m_forward;
PRIVATE cvar_t *freelook;
#endif /* !__CLIENT_H__ */



/*
-----------------------------------------------------------------------------
 Function: CreateNullCursor -Creates a null cursor.

 Parameters: display -[in] Specifies the connection to the X server.
			 root -[in]  Specifies the screen.

 Returns: cursor ID associated with the null cursor.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE Cursor CreateNullCursor(Display *local_display, Window root)
{
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap(local_display, root, 1, 1, 1);
    xgc.function = GXclear;
    gc = XCreateGC(local_display, cursormask, GCFunction, &xgc);
    XFillRectangle(local_display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(local_display, cursormask, cursormask,
								 &dummycolour, &dummycolour, 0, 0);
    XFreePixmap(local_display, cursormask);
    XFreeGC(local_display, gc);

    return cursor;
}

/*
-----------------------------------------------------------------------------
 Function: install_grabs

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void install_grabs(void)
{
	/* inviso cursor */
	XDefineCursor(display, mainwin, CreateNullCursor(display, mainwin));
	/* (CreateNullCursor() contains more X11-specific calls) */

	XGrabPointer(display, mainwin,
				 True,
				 0,
				 GrabModeAsync, GrabModeAsync,
				 mainwin,
				 None,
				 CurrentTime);

	if (in_dgamouse->value) {
		int MajorVersion, MinorVersion;

		if (! XF86DGAQueryVersion(display, &MajorVersion, &MinorVersion)) {
			/* unable to query, probalby not supported */
			Com_Printf("install_grabs(): Failed to detect XF86DGA Mouse\n");
			Cvar_Set("in_dgamouse", "0");
		} else {
			dgamouse = true;
			XF86DGADirectVideo(display, DefaultScreen(display), XF86DGADirectMouse);
			XWarpPointer(display, None, mainwin, 0, 0, 0, 0, 0, 0);
		}
	} else {
		XWarpPointer(display, None, mainwin,
					 0, 0, 0, 0,
					 (int)(viddef.width / 2), (int)(viddef.height / 2));
	}

	XGrabKeyboard(display, mainwin,
				  False,
				  GrabModeAsync, GrabModeAsync,
				  CurrentTime);

	mouse_active = true;

#if 0 || (__clang_analyzer__ && defined(True))
	XSync(display, True);
#endif /* 0 || (__clang_analyzer__ && defined(True)) */
}


/*
-----------------------------------------------------------------------------
 Function: uninstall_grabs

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void uninstall_grabs(void)
{
	if (! display || ! mainwin) {
		return;
	}

	if (dgamouse) {
		dgamouse = false;
		XF86DGADirectVideo( display, DefaultScreen( display ), 0 );
	}

	XUngrabPointer( display, CurrentTime );
	XUngrabKeyboard( display, CurrentTime );

	/* inviso cursor */
	XUndefineCursor( display, mainwin );

	mouse_active = false;
}


/*
-----------------------------------------------------------------------------
 Function: Force_CenterView_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void Force_CenterView_f(void)
{
	ClientState.viewangles[PITCH] = 0;
}

/*
-----------------------------------------------------------------------------
 Function: RW_IN_MLookDown

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void RW_IN_MLookDown(void)
{
	mlooking = true;
}

/*
-----------------------------------------------------------------------------
 Function: RW_IN_MLookUp

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void RW_IN_MLookUp(void)
{
	mlooking = false;
	IN_CenterView();
}

/*
-----------------------------------------------------------------------------
 Function: IN_StartupMouse

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void IN_StartupMouse(void)
{
	/* mouse variables */
	m_filter = Cvar_Get("m_filter", "0", 0);
    in_mouse = Cvar_Get("in_mouse", "1", CVAR_ARCHIVE);
    in_dgamouse = Cvar_Get("in_dgamouse", "1", CVAR_ARCHIVE);
	freelook = Cvar_Get("freelook", "0", 0);
	lookstrafe = Cvar_Get("lookstrafe", "0", 0);
	sensitivity = Cvar_Get("m_sensitivity", "3", 0);
	m_pitch = Cvar_Get("m_pitch", "0.022", 0);
	m_yaw = Cvar_Get("m_yaw", "0.022", 0);
	m_forward = Cvar_Get("m_forward", "1", 0);
	m_side = Cvar_Get("m_side", "0.8", 0);

	Cmd_AddCommand("+mlook", RW_IN_MLookDown);
	Cmd_AddCommand("-mlook", RW_IN_MLookUp);

	Cmd_AddCommand("force_centerview", Force_CenterView_f);

	/* used to be "0.0", but that was a double, and "mx" and "my" are ints... */
	mx = my = 0;
	mouse_avail = true;

	install_grabs(); /* has a lot of X11-specific calls in the Unix version */
}

/*
-----------------------------------------------------------------------------
 Function: RW_IN_Shutdown

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void RW_IN_Shutdown(void)
{
	mouse_avail = false;
}

/*
-----------------------------------------------------------------------------
 Function: IN_MouseMove

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void IN_MouseMove(usercmd_t *cmd)
{
	if (! mouse_avail) {
		return;
	}

	if (m_filter->value) {
		mx = (int)((mx + old_mouse_x) * 0.5);
		my = (int)((my + old_mouse_y) * 0.5);
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mx *= sensitivity->value;
	my *= sensitivity->value;

	/* add mouse X/Y movement to cmd */
	if ((in_strafe.state & 1) ||
		(lookstrafe->value && mlooking)) {
		cmd->sidemove += m_side->value * mx;
	} else {
		ClientState.viewangles[ YAW ] -= m_yaw->value * mx;
	}


	if( (mlooking || freelook->value) &&
		! (in_strafe.state & 1) ) {
		ClientState.viewangles[ PITCH ] += m_pitch->value * my;
	} else {
		cmd->forwardmove -= m_forward->value * my;
	}

	mx = my = 0;
}

/*
-----------------------------------------------------------------------------
 Function: IN_DeactivateMouse

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype moved to "../input.h" */
PUBLIC void IN_DeactivateMouse(void)
{
	if (! mouse_avail || ! display || ! mainwin) {
		return;
	}

	if (mouse_active) {
		uninstall_grabs();
		mouse_active = false;
	}
}

/* prototype moved to "../input.h" */
PUBLIC void IN_ActivateMouse(void)
{
	if (! mouse_avail || ! display || ! mainwin) {
		return;
	}

	if (! mouse_active) {
		mx = my = 0; /* avoid spazzing out */
		install_grabs();
		mouse_active = true;
	}
}

/* prototype moved to "../input.h" */
void RW_IN_Activate(_boolean active)
{
	if (active || vidmode_active) {
		IN_ActivateMouse();
	} else {
		IN_DeactivateMouse();
	}
}

/* EOF */
