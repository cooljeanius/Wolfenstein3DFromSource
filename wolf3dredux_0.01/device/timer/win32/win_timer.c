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
 *	win_timer.c:   Windows timer.
 *
 *	Author:	    Id Software, Inc.
 *	Date:	    1997-2001
 *
 */

#ifndef _WIN32
# error "You do not appear to be on Windows, do not compile this file on this platform."
#endif /* !_WIN32 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>

#include "../timer.h" /* should have prototype for Sys_Milliseconds() */
#include "../../../common/arch.h"
#include "../../../common/common_utils.h"

PUBLIC W32	curtime;


/*
-----------------------------------------------------------------------------
 Function: Sys_Milliseconds

 Parameters: Nothing.

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
/* prototype should be in "../timer.h" */
PUBLIC W32 Sys_Milliseconds(void)
{
	static W32 base;
	static _boolean	initialized = false;

	if (! initialized) {
		/* let base retain 16 bits of effectively random data */
		base = (timeGetTime() & 0xffff0000);
		initialized = true;
	}
	curtime = (timeGetTime() - base);

	return curtime;
}

/* EOF */
