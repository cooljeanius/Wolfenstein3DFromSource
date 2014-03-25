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

#ifndef strcpy
# include <string.h>
#endif /* !strcpy */
#ifndef strlen
# include <string.h>
#endif /* !strlen */

#include "../common/arch.h"
#include "../common/common.h"
#include "../memory/memory.h"
#include "../device/timer/timer.h"

W32	sys_frame_time;

uid_t saved_euid;
_boolean stdin_active = true;


cvar_t *nostdout;


extern void KBD_Update(void);

_boolean	ActiveApp;
_boolean	Minimized;


unsigned	sys_msg_time;
W32	sys_frame_time;

#define	MAX_NUM_ARGVS	128


/*
 ===============================================================================

 SYSTEM IO

 ===============================================================================
 */



/*
 -----------------------------------------------------------------------------
 Function: Sys_Error -Terminate the application due to error.

 Parameters:
			format -[in] Pointer to null-terminated string.
			... -[in] Optional arguments

 Returns:	Nothing

 Notes: Shutdown Application due to error. See format specification fields.

		1.  Terminate Client Sub-systems (with Client_Shutdown()).
		2.  Display error message (with fprintf() to stderr).
		3.  Exit application (with _exit()).
 -----------------------------------------------------------------------------
 */
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

/*
 -----------------------------------------------------------------------------
 Function: Sys_Quit -Terminate the application after cleanup.

 Parameters: Nothing

 Returns: Nothing

 Notes: Shutdown Application.

		1.  Terminate Client Sub-systems (with Client_Shutdown())
		2.  Exit application (with _exit())
 -----------------------------------------------------------------------------
 */
void Sys_Quit(void)
{
	Client_Shutdown();

    fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);
	_exit(0);
}


static char	console_text[256];
static int	console_textlen;

#ifdef HAVE_SYS_UIO_H
# include <sys/uio.h>
#endif /* HAVE_SYS_UIO_H */

/*
 -----------------------------------------------------------------------------
 Function: Sys_ConsoleInput -Console input for dedicated server.

 Parameters: Nothing

 Returns: NULL or pointer to string with console input.

 Notes: TODO: reimplement using curses or something...
 -----------------------------------------------------------------------------
 */
char *Sys_ConsoleInput(void)
{
	void *recs;
	int	dummy;
	int	ch, numread, numevents;

	dummy = 0;
	ch = 0;
	numread = 0;
	numevents = 0;

	if (! dedicated || ! dedicated->value) {
		return NULL;
	}

	for (;;) {
		if (numevents <= 0) {
			break;
		}

		/* not sure whether to use something from the read() family of funcs,
		 * or something from the scanf() family of funcs here for unix: */
		/* '0' is 'stdin': */
		if (! read(0, recs, (size_t)1)) {
			Sys_Error("Error reading console input");
		}

		if (numread != 1) {
			Sys_Error("Could NOT read console input");
		}

		switch (ch) {
			case '\r':
				/* not sure where to put NULL in here: */
				fwrite("\r\n", (size_t)2, (size_t)(unsigned long int *)&dummy,
					   stdout);

				if (console_textlen) {
					console_text[console_textlen] = '\0';
					console_textlen = 0;
					return console_text;
				}
				break;

			case '\b':
				if (console_textlen) {
					console_textlen--;
					/* not sure where to put NULL in here: */
					fwrite("\b \b", (size_t)3,
						   (size_t)(unsigned long int *)&dummy, stdout);
				}
				break;

			default:
				if (ch >= ' ') {
					if (((unsigned long)console_textlen) <
						(sizeof(console_text) - 2)) {
						/* not sure where to put NULL in here: */
						fwrite(&ch, (size_t)1,
							   (size_t)(unsigned long int *)&dummy, stdout);
						console_text[console_textlen] = ch;
						console_textlen++;
					}
				}

				break;

		} /* end switch ch */

	} /* end "for (;;)" for-loop */

	return NULL;
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
#if defined(__APPLE__) && defined(__OBJC__)
# include "clipboard/OSXClipboard.h"
#elif defined(__unix__) || defined(__linux__) || defined(__FreeBSD__)
# include "clipboard/X11Clipboard.h"
#else /* just put the prototypes here: */
extern _boolean OpenClipboard(char *);
extern _boolean CloseClipboard(void);
extern void *GetClipboardData(unsigned int);
extern char *GlobalLockClipboardData(void *);
extern void GlobalUnlockClipboardData(void *);
#endif /* platform check */
char *Sys_GetClipboardData(void)
{
	char *data = NULL;
	char *cliptext;

/* uncomment the '&& 0' to test the other half of the conditional: */
#if defined(__APPLE__) && defined(__OBJC__) /* && 0 */
	BOOL falseval;
	falseval = NO;
#else /* not both (__APPLE__ && __OBJC__): */
	_boolean falseval;
	falseval = (_boolean)0; /* could also try just 'false' */
#endif /* __APPLE__ && __OBJC__ */
	if (OpenClipboard(NULL) != falseval) {
		void *hClipboardData;

		/* not sure exactly what this actually supposed to be: */
		unsigned int CF_TEXT;
		CF_TEXT = 0;
		if ((hClipboardData = GetClipboardData(CF_TEXT)) != 0) {
			if ((cliptext = GlobalLockClipboardData(hClipboardData)) != 0) {
				data = MM_MALLOC(sizeof(hClipboardData) + 1);
				strncpy(data, cliptext, sizeof(hClipboardData));
				GlobalUnlockClipboardData(hClipboardData);
			}
		}
		CloseClipboard();
	}
	return NULL;
	/*TODO: have this function actually do what it says. */
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
int main(int argc, char *argv[MAX_NUM_ARGVS])
#else
int unix_main(int argc, char *argv[MAX_NUM_ARGVS])
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
#if defined(DEBUG) && 0
        if (getenv("SECRET_MAIN_LOOP_BREAKING_ENVVAR") != NULL) {
			fprintf(stdout, "main(): You have set the secret environment variable; you must be a developer!\n");
        	break;
        }
#endif /* DEBUG && 0 */
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
