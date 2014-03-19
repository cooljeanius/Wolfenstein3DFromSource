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
 *	unix_system.c: Non-portable system functions.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

#include <sys/types.h>
#include <sys/sysctl.h>
#include <string.h>

#include "../../../config.h"

#if __linux__ || defined(HAVE_SYS_UTSNAME_H)
# include <sys/utsname.h>
#endif /* __linux__ || HAVE_SYS_UTSNAME_H */


#include "../system.h" /* has prototypes for Print_OS_Info() and Print_Memory_Stats() */
#include "../../../memory/memory.h"
#include "../../../common/arch.h"
#include "../../../common/common.h"

/* Calculate in mebibytes */
#define DIV 1024 / 1024

/*
-----------------------------------------------------------------------------
 Function: Sys_OS_Init -Init OS specfic junk.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sys_OS_Init(void)
{
	Com_Printf("Initializng OS specific things...\n");
/* should something more actually go here? */
}

/*
-----------------------------------------------------------------------------
 Function: Print_OS_Info -Print OS specfic junk.

 Parameters: Nothing

 Returns: Nothing

 Notes:  print OS version information.
-----------------------------------------------------------------------------
*/
/* prototype should be in "../system.h" */
PUBLIC void Print_OS_Info(void)
{
#if defined(__FreeBSD__) || defined(__APPLE__)
	int mib[2];
	size_t len;
	char *p;

	Com_Printf("Using version of Print_OS_Info() originally designed for FreeBSD...\n");

	mib[0] = CTL_KERN;
	mib[1] = KERN_OSTYPE;


	if (sysctl(mib, 2, NULL, &len, NULL, 0) == -1) {
		return;
	}

	p = MM_MALLOC(len);

	if (sysctl(mib, 2, p, &len, NULL, 0) == -1) {
		free(p);
		return;
	}

	Com_Printf("%*s ", len, p);

	mib[1] = KERN_OSRELEASE;

	if (sysctl(mib, 2, NULL, &len, NULL, 0) == -1) {
		free(p);
		return;
	}

	p = MM_REALLOC(p, len);

	if (sysctl(mib, 2, p, &len, NULL, 0) == 0) {
		Com_Printf( "%*s\n", len, p );
	}


	MM_FREE( p );
#elif defined(__linux__) || defined(__APPLE__)
	struct utsname myname;

	Com_Printf("Using version of Print_OS_Info() originally designed for Linux...\n");

	if (uname(&myname) != 0) {
		Com_Printf("[Print_OS_Info] Could not get OS info.\n");
		return;
	}

	Com_Printf("%s %s\n", myname.sysname, myname.release);
#elif defined __APPLE__
	/* TODO: actually do something here */
	/* (not sure whether to steal from the __FreeBSD__ or __linux__ portion for
	 * this one... they both work and print identical results...) */
	Com_Printf("[Print_OS_Info] Implementation for OSX is incomplete.\n");
#else
	Com_Printf("[Print_OS_Info] Unknown Operating System.\n");
#endif /* __FreeBSD__ || __linux__ || __APPLE__ */

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
#if defined(__FreeBSD__) || defined(__APPLE__)
	int mib[2];
	int value;
	size_t len;

	Com_Printf("Using version of Print_Memory_Stats() originally designed for FreeBSD...\n");

	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM;
	len = sizeof(value);
	sysctl(mib, 2, &value, &len, NULL, 0);

	Com_Printf("Physical Memory: %lu MiB\n", (value / DIV));
#elif defined __linux__
	FILE* fp;
	char buffer[1024];
	size_t bytes_read;
	char *match;

	Com_Printf("Using version of Print_Memory_Stats() originally designed for Linux...\n");

	/* Read the contents of /proc/meminfo */
	fp = fopen("/proc/meminfo", "r");
	bytes_read = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);

	/* Bail if read failed or if buffer is not big enough. */
	if ((bytes_read == 0) || (bytes_read == sizeof(buffer))) {
		return;
	}

	/* NUL-terminate the string. */
	buffer[bytes_read] =  '\0';

	/* Locate the line that starts with  'Mem:' */
	match = strstr(buffer, "Mem:");

	if (match == NULL) {
		return;
	}

	strtok(match, " ");

	Com_Printf("Total Memory: %lu MiB\n", (atoi(strtok(NULL, " ")) / DIV));
	Com_Printf("Used Memory: %lu MiB\n", (atoi(strtok(NULL, " ")) / DIV));
	Com_Printf("Free Memory: %lu MiB\n", (atoi(strtok(NULL, " ")) / DIV));
#elif defined __APPLE__
	/* TODO: actually do something here */
	/* (The __FreeBSD__ section is probably better to take from here, as there
	 * is no /proc/meminfo on darwin...) */
	Com_Printf("[Print_Memory_Stats] Probing memory is unimplemented on OSX.\n");
#else
	Com_Printf("[Print_Memory_Stats] Could not probe memory.\n");
#endif /* __FreeBSD__ || __linux__ || __APPLE__ */

}

/* EOF */
