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
 *	openglw_imp.c: unix OpenGL implementation.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

/*
 * Notes:
 *
 * This file contains ALL X11 specific stuff having to do with the
 * OpenGL refresh. When a port is being made the following functions
 * must be implemented by the port:
 *
 * GLimp_EndFrame
 * GLimp_Init
 * GLimp_Shutdown
 * GLimp_SwitchFullscreen
 */

#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>


#include <GL/glx.h>

/* <X11/keysym.h> had been listed a second time here, but no need for duplicate
 * includes... */
#include <X11/cursorfont.h>

#ifdef HAVE_X11_EXTENSIONS_XF86DGA_H
# include <X11/extensions/xf86dga.h>
#elif defined(_XF86DGA_SERVER_) && defined(HAVE_X11_EXTENSIONS_XF86DGACONST_H)
# include <X11/extensions/xf86dgaconst.h>
#elif defined HAVE_X11_EXTENSIONS_XXF86DGA_H || 1
# include <X11/extensions/Xxf86dga.h>
#else
# warning unix_mouse.c expects a certain X11 header to be included, check the ifdefs
#endif /* HAVE_X11_EXTENSIONS_[X|XX]F86DGA_H */
#include <X11/extensions/xf86vmode.h>

#ifdef __OPENGL_LOCAL_H__
# warning "opengl_local.h already included once."
#endif /* __OPENGL_LOCAL_H__ */
#include "../opengl_local.h"
#ifndef __OPENGL_LOCAL_H__
# warning "failed to properly include opengl_local.h"
#endif /* !__OPENGL_LOCAL_H__ */
#include "../myopengl.h"

#include "../../video.h"
#include "../../../string/com_string.h"
#include "../../../common/common.h"
#include "../../../common/arch.h"
#include "../../../common/common_utils.h"

#include "../../../game/app_def.h"


extern void uninstall_grabs(void);
extern void install_grabs(void);

Display *display = NULL;
PRIVATE int screen_num;
Window mainwin;
PRIVATE GLXContext ctx = NULL;


PRIVATE XF86VidModeModeInfo **vidmodes;

PRIVATE _boolean vidmode_ext = false;

PRIVATE int num_vidmodes;
_boolean vidmode_active = false;

PRIVATE cvar_t	*r_fakeFullscreen;


extern _boolean mouse_active;
extern _boolean dgamouse;


#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask | ExposureMask | EnterWindowMask | LeaveWindowMask )


PRIVATE char *signal_ErrorString(int sig)
{
	switch (sig) {
		case SIGHUP:	return "Hangup (POSIX)";
		case SIGINT:	return "Interrupt (ANSI)";
		case SIGQUIT:	return "Quit (POSIX)";
		case SIGILL:	return "Illegal instruction (ANSI)";
		case SIGTRAP:	return "Trace trap (POSIX)";
		case SIGIOT:	return "Abort (ANSI) | IOT trap (4.2 BSD)";
		case SIGBUS:	return "BUS error (4.2 BSD)";
		case SIGFPE:	return "Floating-point exception (ANSI)";
		case SIGKILL:	return "Kill, unblockable (POSIX)";
		case SIGUSR1:	return "User-defined signal 1 (POSIX)";
		case SIGSEGV:	return "Segmentation violation (ANSI)";
		case SIGUSR2:	return "User-defined signal 2 (POSIX)";
		case SIGPIPE:	return "Broken pipe (POSIX)";
		case SIGALRM:	return "Alarm clock (POSIX)";
		case SIGTERM:	return "Termination (ANSI)";

		default:	return "Unknown SIG";
	}

	assert(sig);
}



PRIVATE ATTR_NORETURN void signal_handler(int sig)
{
	printf("Received signal (%s), exiting...\n", signal_ErrorString(sig));
	GLimp_Shutdown();
	_exit(0);
}

/* just a bunch of calls to signal() so far: */
PRIVATE void InitSig(void)
{
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}


/* */
int GLimp_SetMode(int *pwidth, int *pheight, int mode, _boolean fullscreen)
{
	int width, height;

	int attrib[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};

	Window root;
	XVisualInfo *visinfo;
	XSetWindowAttributes attr;
	W32 attribmask;
	int MajorVersion, MinorVersion;
	int actualWidth, actualHeight;
	int i;
	XEvent report;
	XSizeHints size_hints;
	char display_name[512];

	r_fakeFullscreen = Cvar_Get("r_fakeFullscreen", "0", CVAR_ARCHIVE);

	if (fullscreen) {
		Com_Printf("...setting fullscreen mode %d:", mode);
	} else {
		Com_Printf("...setting mode %d:", mode);
	}

	if (! VID_GetModeInfo(&width, &height, mode)) {
		Com_Printf(" invalid mode\n");
		return rserr_invalid_mode;
	}

	Com_Printf(" %d %d\n", width, height);

	/* destroy the existing window */
	GLimp_Shutdown();

	/* get the default display name from the environment variable DISPLAY */
	my_strlcpy(display_name, getenv("DISPLAY"), sizeof(display_name));
	if (display_name[0] == 0) {
    	my_strlcpy(display_name, "unix:0.0", sizeof(display_name));
	}

	if (!(display = XOpenDisplay(display_name))) {
		fprintf(stderr,
				"GLimp_SetMode(): Error: could not open the X display\n");

		return rserr_invalid_mode;
	}

	screen_num = DefaultScreen(display);
	root = RootWindow(display, screen_num);

	/* Get video mode list */
	MajorVersion = MinorVersion = 0;
	if (! XF86VidModeQueryVersion(display, &MajorVersion, &MinorVersion)) {
		vidmode_ext = false;
	} else {
		Com_Printf("GLimp_SetMode(): Using XFree86-VidModeExtension Version %d.%d\n",
				   MajorVersion, MinorVersion);
		vidmode_ext = true;
	}

#if !defined(pfglXChooseVisual) && !defined(HAVE_PFGLXCHOOSEVISUAL)
# if defined(HAVE_GLXCHOOSEVISUAL) || defined(glXChooseVisual) || defined(GLX_H)
#  define pfglXChooseVisual glXChooseVisual
# else
#  warning "cannot redefine pfglXChooseVisual as glXChooseVisual because it is not available."
# endif /* HAVE_GLXCHOOSEVISUAL || glXChooseVisual || GLX_H */
#endif /* !pfglXChooseVisual && !HAVE_PFGLXCHOOSEVISUAL */
	visinfo = pfglXChooseVisual(display, screen_num, attrib);
	if (! visinfo) {
		fprintf(stderr,
				"GLimp_SetMode(): Error: could not get an RGB, Double-buffered, Depth visual\n");
		return rserr_invalid_mode;
	}

	if (vidmode_ext) {
		int best_fit, best_dist, dist, x, y;

		XF86VidModeGetAllModeLines(display, screen_num, &num_vidmodes, &vidmodes);

		/* Are we going fullscreen?  If so, let us change video mode */
		if (fullscreen && ! r_fakeFullscreen->value) {
			best_dist = 9999999;
			best_fit = -1;

			for ((i = 0); (i < num_vidmodes); i++) {
				if ((width > vidmodes[i]->hdisplay) ||
					(height > vidmodes[i]->vdisplay)) {
					continue;
				}

				x = (width - vidmodes[i]->hdisplay);
				y = (height - vidmodes[i]->vdisplay);
				dist = ((x * x) + (y * y));
				if (dist < best_dist) {
					best_dist = dist;
					best_fit = i;
				}
			}

			if (best_fit != -1) {
				actualWidth = vidmodes[best_fit]->hdisplay;
				actualHeight = vidmodes[best_fit]->vdisplay;

				/* dummy to silence clang static analyzer warnings about values
				 * stored to 'actualWidth' & 'actualHeight' never being read: */
				if ((actualWidth == 0) || (actualHeight == 0)) {
					;
				}

				/* change to the mode */
				XF86VidModeSwitchToMode(display, screen_num, vidmodes[best_fit]);
				vidmode_active = true;

				/* Move the viewport to top left */
				XF86VidModeSetViewPort(display, screen_num, 0, 0);
			} else {
				fullscreen = 0;
				/* dummy to silence clang static analyzer warning about value
				 * stored to 'fullscreen' never being read: */
				if (fullscreen == 0) {
					;
				}
			}
		}
	}

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(display, root, visinfo->visual, AllocNone);
	if (vidmode_active) {
		attribmask = (CWBackPixel | CWColormap | CWSaveUnder | CWBackingStore |
					  CWOverrideRedirect);
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	} else {
		attribmask = (CWBackPixel | CWBorderPixel | CWColormap);
	}

	mainwin = XCreateWindow(display, root, 0, 0,
							(unsigned int)width, (unsigned int)height,
							0, visinfo->depth, InputOutput,
							visinfo->visual, attribmask, &attr);

	size_hints.flags = (PSize | PMinSize | PMaxSize);
	size_hints.min_width = width;
	size_hints.max_width = width;
	size_hints.min_height = height;
	size_hints.max_height = height;

	XSetStandardProperties(display, mainwin, GAME_NAME, NULL, None, 0, 0,
						   &size_hints);

	XSelectInput(display, mainwin, X_MASK);

	XMapWindow(display, mainwin);

	do {
		XNextEvent(display, &report);

	} while (report.type != Expose); /* wait for our window to pop up */

	if (vidmode_active) {
		XMoveWindow(display, mainwin, 0, 0);
		XRaiseWindow(display, mainwin);
		XWarpPointer(display, None, mainwin, 0, 0, 0, 0, 0, 0);
		XFlush(display);
		/* Move the viewport to top left */
		XF86VidModeSetViewPort(display, screen_num, 0, 0);
	}

	XFlush(display);

#if !defined(pfglXCreateContext) && !defined(HAVE_PFGLXCREATECONTEXT)
# if defined(HAVE_GLXCREATECONTEXT) || defined(glXCreateContext) || defined(GLX_H)
#  define pfglXCreateContext glXCreateContext
# else
#  warning "cannot redefine pfglXCreateContext as glXCreateContext because it is not available."
# endif /* HAVE_GLXCREATECONTEXT || glXCreateContext || GLX_H */
#endif /* !pfglXCreateContext && !HAVE_PFGLXCREATECONTEXT */
	ctx = pfglXCreateContext(display, visinfo, NULL, True);

#if !defined(pfglXMakeCurrent) && !defined(HAVE_PFGLXMAKECURRENT)
# if defined(HAVE_GLXMAKECURRENT) || defined(glXMakeCurrent) || defined(GLX_H)
#  define pfglXMakeCurrent glXMakeCurrent
# else
#  if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#   warning "cannot redefine pfglXMakeCurrent as glXMakeCurrent because it is not available."
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* HAVE_GLXMAKECURRENT || glXMakeCurrent || GLX_H */
#endif /* !pfglXMakeCurrent && !HAVE_PFGLXMAKECURRENT */
	pfglXMakeCurrent(display, mainwin, ctx);

	*pwidth = width;
	*pheight = height;

	/* let the sound and input subsystems know about the new window */
	VID_NewWindow(width, height);

	pfglXMakeCurrent(display, mainwin, ctx);

	return rserr_ok;
}

void GLimp_Shutdown(void)
{
	uninstall_grabs();
	mouse_active = false;
	dgamouse = false;

	if (display) {
		if (ctx) {
			pfglXDestroyContext(display, ctx);
		}

		if (mainwin) {
			XDestroyWindow(display, mainwin);
		}

		if (vidmode_active) {
			XF86VidModeSwitchToMode(display, screen_num, vidmodes[0]);
		}

		XCloseDisplay(display);
	}

	ctx = NULL;
	display = NULL;
	mainwin = 0;
}

_boolean GLimp_Init(void *hinstance, void *wndproc)
{
	/* dummy condition to use parameters: */
	if (hinstance == wndproc) {
		;
	}
	/* just a bunch of calls to signal() so far: */
	InitSig();

	return true;
}

/* this prototype should have been dragged in from including "../opengl.h", but
 * if not...: */
#ifndef GLimp_BeginFrame
extern void GLimp_BeginFrame(void);
#endif /* !GLimp_BeginFrame */
void GLimp_BeginFrame(void)
{
	;
/* TODO: actually put something here? */
}

/* */
void GLimp_EndFrame(void)
{
	/* entry points into OpenGL: */
	pfglFlush();
	pfglXSwapBuffers(display, mainwin);
}

/* */
void GLimp_AppActivate(_boolean active)
{
	/* dummy condition to use parameter 'active': */
	if (active) {
		;
	}
/* TODO: actually put something here for real? */
}

/* EOF */
