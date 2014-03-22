/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	unix_main.c: UNIX interface to application.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 *  Some portions adapted from code from the open-source release of
 *  Wolfenstein 3D for iOS, Copyright (C) 2009 Id Software, Inc.
 *
 */

#ifndef _UNIX_MAIN_C
#define _UNIX_MAIN_C

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <dlfcn.h>
/* <unistd.h> had been listed a second time here, but no need for duplicate
 * includes... */
#include <fcntl.h>

#if defined(__APPLE__) && defined(__OBJC__) && defined(__GNUC__)
# import <Cocoa/Cocoa.h>
# include <string.h>
#endif /* __APPLE__ && __OBJC__ && __GNUC__ */

#include "../common/arch.h"
#include "../common/common.h"
#include "../memory/memory.h"
#include "../device/timer/timer.h"

W32	sys_frame_time;

uid_t saved_euid;
_boolean stdin_active = true;


cvar_t *nostdout;


extern void KBD_Update(void);

#ifndef Sys_Error /* this ifdef is bad */
void Sys_Error(const char *format, ...)
#else
void unix_Sys_Error(const char *format, ...)
#endif /* !Sys_Error */
{
    va_list     argptr;
    char        string[1024];

/* change stdin to non blocking */
    fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY );


	Client_Shutdown();

    va_start(argptr, format);
    (void)vsnprintf(string, sizeof(string), format, argptr);
    va_end(argptr);

	fprintf(stderr, "Error: %s\n", string);

	_exit(1);

}

/* quit */
void Sys_Quit(void)
{
	Client_Shutdown();

    fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);
	_exit(0);
}

/*
-----------------------------------------------------------------------------
 Function: Sys_SendKeyEvents

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Send Key_Event calls.
-----------------------------------------------------------------------------
*/
void Sys_SendKeyEvents(void)
{
	/* grab frame time */
	sys_frame_time = Sys_Milliseconds();
}

/*
-----------------------------------------------------------------------------
 Function: Sys_GetClipboardData

 Parameters: Nothing.

 Returns: Pointer to a string on success, NULL otherwise.
		  (Actually just NULL for now)

 Notes: Grabs text from clipboard.
        Caller is responsible for freeing data.
-----------------------------------------------------------------------------
*/
char *Sys_GetClipboardData(void)
{
	return NULL;
}


/*
-----------------------------------------------------------------------------
 Function: main	-Application entry point.

 Parameters:

 Returns: Nothing.

 Notes: This is the application entry point.
 			1.  Check for mulitple instances.
			2.  Init Sub-systems.
			3.  Enter application loop.
-----------------------------------------------------------------------------
*/
#ifndef main /* this ifdef is bad */
int main(int argc, char *argv[])
#else
int unix_main(int argc, char *argv[])
#endif /* !main */
{
	int main_time, oldtime, newtime; /* 'time' was renamed to 'main_time' to
									  * avoid shadowing a global declaration */
	int i, len;
	/* basically a reimplementation of getcwd(): */
	if (argc == 1) {
    	char cwd[256];
    	strcpy(cwd, argv[0]);
    	len = (int)strlen(cwd);
    	for (i = (len - 1); (i >= 0); i--) {
    		if (cwd[i] == '/') {
    			cwd[i] = 0;
    			break;
    		}
    		cwd[i] = 0;
    	}
    	setenv("CWD", cwd, 1);
    } /* end getcwd() reimplementation */

	/* go back to real user for config loads */
	saved_euid = geteuid();
	seteuid(getuid());

	/* this is where most of the stuff actually gets init-ed: */
	common_Init(argc, argv);

	fcntl(0, F_SETFL, (fcntl(0, F_GETFL, 0) | FNDELAY));

	nostdout = Cvar_Get("nostdout", "0", CVAR_INIT);
	if (! nostdout->value) {
		fcntl(0, F_SETFL, (fcntl(0, F_GETFL, 0) | FNDELAY));
	}

    oldtime = (int)Sys_Milliseconds();
	/* main application loop: */
    while (1) {
		KBD_Update();

		/* find time spent rendering last frame */
		do {
			newtime = (int)Sys_Milliseconds();
			main_time = (newtime - oldtime);

		} while (main_time < 1);

        /* most of our time is spent in here: */
        common_Frame(main_time);
        oldtime = newtime;
#ifdef DEBUG
        if (getenv("SECRET_MAIN_LOOP_BREAKING_ENVVAR") != NULL) {
			fprintf(stdout, "main(): You have set the secret environment variable; you must be a developer!\n");
        	break;
        }
#endif /* DEBUG */
    }

/* Should never get here!
 * (because the 'while' loop above should never break, unless the entire program
 * is being skipped out of, via a call to _exit() or something...)
 * (although there is now a secret environment variable that you can set to get
 * here while compiling for debugging...) */
#if defined(__APPLE__) && defined(__OBJC__) && defined(__GNUC__)
    /* (same conditions for import-ing <Cocoa/Cocoa.h> above) */
    int retval;
    /* same code as from main.m (taken from Wolf3D-iOS): */
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    retval = NSApplicationMain(argc, (const char **)argv);
    [pool release];
    return retval;
#else /* not Objective C: */
	return 0;
#endif /* __APPLE__ && __OBJC__ && __GNUC__ */
}

#endif /* !_UNIX_MAIN_C */

/* EOF */
