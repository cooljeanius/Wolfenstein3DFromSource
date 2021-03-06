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
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/*
 *	win_main.c: Windows interface to application.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */


#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>

#include "../common/arch.h"
#include "../common/common.h"
#include "../memory/memory.h"
#include "../device/timer/timer.h"

#include "win_def.h" /* new place for prototypes from this file */


HINSTANCE	global_hInstance;


_boolean	ActiveApp;
_boolean	Minimized;


static HANDLE hinput, houtput;

unsigned	sys_msg_time;
W32	sys_frame_time;

static HANDLE hMutex;


#define	MAX_NUM_ARGVS	128

int	 argc;
char *argv[MAX_NUM_ARGVS];



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

		1.  Terminate Client Sub-systems.
		2.  Display error message in MessageBox.
		3.  Exit application.
-----------------------------------------------------------------------------
*/
void Sys_Error(const char *format, ...)
{
	va_list	argptr;
	char	text[1024];

	Client_Shutdown();

	va_start(argptr, format);
	(void)vsnprintf(text, sizeof(text), format, argptr);
	va_end(argptr);

	text[(sizeof(text) - 1)] = '\0';

	MessageBox(NULL, text, "Error", MB_OK);

	if (hMutex) {
		CloseHandle(hMutex);
	}

#if 0
/*	shut down HOST hooks if necessary */
	DeinitConProc();
#endif /* 0 */

	exit(1);
}


/*
-----------------------------------------------------------------------------
 Function: Sys_Quit -Terminate the application after cleanup.

 Parameters: Nothing

 Returns:	Nothing

 Notes: Shutdown Application.

		1.  Terminate Client Sub-systems.
		2.  Exit application.
-----------------------------------------------------------------------------
*/
void Sys_Quit(void)
{
	timeEndPeriod(1);

	Client_Shutdown();

	if (hMutex) {
		CloseHandle(hMutex);
	}


	if (dedicated && dedicated->value) {
		FreeConsole();
	}

#if 0
/* shut down HOST hooks if necessary */
	DeinitConProc();
#endif /* 0 */

	exit(0);
}


static char	console_text[256];
static int	console_textlen;


/*
-----------------------------------------------------------------------------
 Function: Sys_ConsoleInput -Console input for dedicated server.

 Parameters: Nothing

 Returns: NULL or pointer to string with console input.

 Notes:
-----------------------------------------------------------------------------
*/
char *Sys_ConsoleInput(void)
{
	INPUT_RECORD	recs[1024];
	int	dummy;
	int	ch, numread, numevents;

	if (! dedicated || ! dedicated->value) {
		return NULL;
	}

	for (;;) {
		if (! GetNumberOfConsoleInputEvents(hinput, (LPDWORD)&numevents)) {
			Sys_Error("Error getting # of console events" );
		}

		if (numevents <= 0) {
			break;
		}

		if (! ReadConsoleInput(hinput, recs, 1, (LPDWORD)&numread)) {
			Sys_Error("Error reading console input");
		}

		if (numread != 1) {
			Sys_Error("Could NOT read console input");
		}

		if (recs[0].EventType == KEY_EVENT) {
			if (! recs[0].Event.KeyEvent.bKeyDown) {
				ch = recs[0].Event.KeyEvent.uChar.AsciiChar;

				switch (ch) {
					case '\r':
						WriteFile(houtput, "\r\n", 2, (LPDWORD)&dummy, NULL);

						if (console_textlen) {
							console_text[console_textlen] = '\0';
							console_textlen = 0;
							return console_text;
						}
						break;

					case '\b':
						if (console_textlen) {
							console_textlen--;
							WriteFile(houtput, "\b \b", 3, (LPDWORD)&dummy,
									  NULL);
						}
						break;

					default:
						if (ch >= ' ') {
							if ((unsigned long)console_textlen < (sizeof(console_text) - 2)) {
								WriteFile(houtput, &ch, 1, (LPDWORD)&dummy,
										  NULL);
								console_text[console_textlen] = ch;
								console_textlen++;
							}
						}

						break;

				} /* end switch ch */

			} /* end "if (! recs[0].Event.KeyEvent.bKeyDown)" */

		} /* end "if (recs[0].EventType == KEY_EVENT)" */

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
    MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (! GetMessage(&msg, NULL, 0, 0)) {
			Sys_Quit();
		}
		sys_msg_time = msg.time;
      	TranslateMessage(&msg);
      	DispatchMessage(&msg);
	}

	/* grab frame time */
	sys_frame_time = timeGetTime();	/* FIXME: should this be at start? */
}


/*
-----------------------------------------------------------------------------
 Function: Sys_GetClipboardData

 Parameters: Nothing.

 Returns: Pointer to a string on success, NULL otherwise.

 Notes: Grabs text from clipboard.
        Caller is responsible for freeing data.
-----------------------------------------------------------------------------
*/
char *Sys_GetClipboardData(void)
{
	char *data = NULL;
	char *cliptext;

	if (OpenClipboard(NULL) != 0) {
		HANDLE hClipboardData;

		if ((hClipboardData = GetClipboardData(CF_TEXT)) != 0) {
			if ((cliptext = GlobalLock(hClipboardData)) != 0) {
				data = MM_MALLOC(GlobalSize(hClipboardData) + 1);
				strncpy(data, cliptext, GlobalSize(hClipboardData));
				GlobalUnlock(hClipboardData);
			}
		}
		CloseClipboard();
	}
	return data;
}



/*
-----------------------------------------------------------------------------
 Function: ParseCommandLine

 Parameters: lpCmdLine -[in] Pointer to string with commandline args.

 Returns: Nothing.

 Notes: Uses global variables argc argv[].
-----------------------------------------------------------------------------
*/
void ParseCommandLine(LPSTR lpCmdLine)
{
	argc = 1;
	argv[0] = "exe";

	while (*lpCmdLine && (argc < MAX_NUM_ARGVS)) {
		/* Ignore ASCII characters outside the range of '!' and '}' */
		while (*lpCmdLine && ((*lpCmdLine < 33) || (*lpCmdLine > 126))) {
			++lpCmdLine;
		}

		if (*lpCmdLine) {
			argv[argc] = lpCmdLine;
			argc++;

			/* Keep ASCII characters within the range of '!' and '}' */
			while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine < 127))) {
				++lpCmdLine;
			}

			if (*lpCmdLine) {
				*lpCmdLine = '\0';	/* NUL-terminate string */
				++lpCmdLine;
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
 Function: CheckforInstance -Check to stop multiple instances of application
							 from executing.
 Parameters: Nothing.

 Returns: Nothing.

 Notes: Uses global variable hMutex.
-----------------------------------------------------------------------------
*/
void CheckforInstance(void)
{
	/* Create a mutex object on the client to stop
	 * multiple instances of application from running. */

	hMutex = CreateMutex(NULL, 0, "qwcl");

	if (! hMutex || (GetLastError() == ERROR_ALREADY_EXISTS)) {
		Sys_Error("Application is already running on this system");
	}
}

/*
-----------------------------------------------------------------------------
 Function: WinMain	-Application entry point.

 Parameters: Windows Crap.

 Returns: Nothing.

 Notes: This is the application entry point.
 			1.  Check for mulitple instances.
 			2.  Parse commandline arguments.
			3.  Init Sub-systems.
			4.  Enter the message loop.
-----------------------------------------------------------------------------
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nWinMode)
{
	MSG	 msg;
	int	 time, oldtime, newtime;


	/* dummy condition to use 'hPrevInstance': */
	if (hInstance == hPrevInstance) {
		;
	}

	/* dummy condition to use 'nWindMode': */
	if (nWinMode == 0) {
		;
	}

	global_hInstance = hInstance;

	CheckforInstance();

	ParseCommandLine(lpCmdLine);

	common_Init(argc, argv);

	oldtime = Sys_Milliseconds();

	/* main window message loop */
	while (1) {
		/* if at a full screen console, do NOT update unless needed */
		if (Minimized || (dedicated && dedicated->value)) {
			Sleep(1);
		}

		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (! GetMessage(&msg, NULL, 0, 0)) {
				Com_Quit();
			}
			sys_msg_time = msg.time;

			TranslateMessage(&msg);
   			DispatchMessage(&msg);
		}

		do {
			newtime = Sys_Milliseconds();
			time = (newtime - oldtime);
		} while (time < 1);

		common_Frame(time);

		oldtime = newtime;
	}

/* We should never get here! */
	return msg.wParam;
}


/* EOF */
