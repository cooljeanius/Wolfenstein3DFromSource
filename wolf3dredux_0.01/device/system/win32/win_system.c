/*
 *	Copyright (C) 2004 Michael Liebscher
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
 *	win_system.c:   Non-portable system functions.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *
 */

#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>

#include "../system.h" /* has prototypes for Print_OS_Info() and Print_Memory_Stats() */
#include "../../../common/arch.h"
#include "../../../common/common.h"


_boolean s_win95;


/*
-----------------------------------------------------------------------------
 Function: Sys_OS_Init -Init OS specfic junk.

 Parameters: Nothing

 Returns: Nothing

 Notes: A simple wrapper around timeBeginPeriod() here...
-----------------------------------------------------------------------------
*/
PUBLIC void Sys_OS_Init(void)
{
	timeBeginPeriod(1);
}

/*
-----------------------------------------------------------------------------
 Function: Print_OS_Info -Print OS specfic junk.

 Parameters: Nothing

 Returns: Nothing

 Notes:  print Windows version.
-----------------------------------------------------------------------------
*/
/* prototype should be in "../system.h" */
PUBLIC void Print_OS_Info(void)
{
	OSVERSIONINFOEX vinfo;
	_boolean bOsVersionInfoEx;

/*
 * Find out what version of Windows is running
 */
	memset(&vinfo, 0, sizeof(OSVERSIONINFOEX));
	vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	/* Try calling GetVersionEx using the OSVERSIONINFOEX structure. */
	/* If that fails, try using the OSVERSIONINFO structure. */
	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&vinfo))) {
		vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx((OSVERSIONINFO *)&vinfo)) {
			Com_Printf("[%s]: Could not get OS info\n", "Print_OS_Info");
			return;
		}
	}

	switch (vinfo.dwPlatformId) {
		/* Check for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:

			/* Test for the specific product family. */
			if ((vinfo.dwMajorVersion == 5) && (vinfo.dwMinorVersion == 2)) {
				Com_Printf("Windows Server 2003");

				if (bOsVersionInfoEx) {
					if (vinfo.wSuiteMask & VER_SUITE_DATACENTER) {
						Com_Printf(" Datacenter Edition");
					} else if (vinfo.wSuiteMask & VER_SUITE_ENTERPRISE) {
						Com_Printf(" Enterprise Edition");
					} else if (vinfo.wSuiteMask & VER_SUITE_BLADE) {
						Com_Printf(" Web Edition");
					} else {
						Com_Printf(" Standard Edition");
					}
				}
			} else if ((vinfo.dwMajorVersion == 5) &&
					   (vinfo.dwMinorVersion == 1)) {
				Com_Printf("Windows XP");

				if( bOsVersionInfoEx ) {
					if (vinfo.wSuiteMask & VER_SUITE_PERSONAL) {
						Com_Printf(" Home Edition");
					} else {
						Com_Printf(" Professional");
					}
				}
			} else if ((vinfo.dwMajorVersion == 5) &&
					   (vinfo.dwMinorVersion == 0)) {
				Com_Printf("Windows 2000");

				if (bOsVersionInfoEx) {
					if (vinfo.wProductType ==  VER_NT_SERVER) {
						if (vinfo.wSuiteMask & VER_SUITE_DATACENTER) {
							Com_Printf(" Datacenter Server");
						} else if (vinfo.wSuiteMask & VER_SUITE_ENTERPRISE) {
							Com_Printf(" Advanced Server");
						} else {
							Com_Printf(" Server");
						}
					} else if (vinfo.wProductType == VER_NT_DOMAIN_CONTROLLER) {
						Com_Printf(" Domain Controller");
					} else {
						Com_Printf(" Professional");
					}
				}
			} else if (vinfo.dwMajorVersion <= 4) {
				Com_Printf("Windows NT");
			} else {
				Com_Printf("Unknown Windows product family");
			}


			/* Display service pack (if any) and build number. */
			Com_Printf(" %s (Build %d)", vinfo.szCSDVersion,
					   (vinfo.dwBuildNumber & 0xFFFF));
			break;

		/* Test for the Windows 9x product family. */
		case VER_PLATFORM_WIN32_WINDOWS:
			s_win95 = true;
			if ((vinfo.dwMajorVersion == 4) && (vinfo.dwMinorVersion == 0)) {
				Com_Printf("Windows 95");
				if ((vinfo.szCSDVersion[1] == 'B') ||
					(vinfo.szCSDVersion[1] == 'C')) {
					Com_Printf(" OSR2");
				}
			} else if ((vinfo.dwMajorVersion == 4) &&
					   (vinfo.dwMinorVersion == 10)) {
				Com_Printf("Windows 98");
				if (vinfo.szCSDVersion[1] == 'A') {
					Com_Printf(" SE");
				}
			} else if ((vinfo.dwMajorVersion == 4) &&
					   (vinfo.dwMinorVersion == 90)) {
				Com_Printf("Windows Millennium Edition");
			} else {
				Com_Printf("Unknown Windows product family");
			}
			break;

		case VER_PLATFORM_WIN32s:
			Sys_Error(" does NOT run on Win32s");
			break;

		default:
			Com_Printf("Unknown OS");
			break;
	}

	Com_Printf("\n");

}

/*
-----------------------------------------------------------------------------
 Function: Print_Memory_Stats -Print memory usage.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype should be in "../system.h" */
PUBLIC void Print_Memory_Stats(void)
{
	/* Calculate in mebibytes */
	#define DIV 1024/1024

	MEMORYSTATUS MemStat;

	GlobalMemoryStatus(&MemStat);

	Com_Printf("Physical Memory: %lu MiB", (MemStat.dwTotalPhys / DIV));
	Com_Printf(" (%lu MiB free)\n", (MemStat.dwAvailPhys / DIV));
	Com_Printf("Virtual Memory: 0x%lx MiB", (MemStat.dwTotalVirtual / DIV));
	Com_Printf(" (0x%lx MiB free)\n", (MemStat.dwAvailVirtual / DIV));
	Com_Printf("Page File: %lu MiB", (MemStat.dwTotalPageFile / DIV));
	Com_Printf(" (%lu MiB free)\n", (MemStat.dwAvailPageFile / DIV));

}

/* EOF */
