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
 *
 */

/*
 *	unix_timer.c: unix timer.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */


#include <sys/time.h>

#include "../timer.h" /* should have prototype for Sys_Milliseconds() */
#include "../../../common/arch.h"
#include "../../../common/common_utils.h"

PUBLIC W32	curtime;

/*
-----------------------------------------------------------------------------
 Function: Sys_Milliseconds

 Parameters: Nothing.

 Returns: The current time.

 Notes: Basically a simple wrapper around gettimeofday() for now...
-----------------------------------------------------------------------------
*/
/* prototype should be in "../timer.h" */
PUBLIC W32 Sys_Milliseconds(void)
{
	struct timeval tp;
	struct timezone tzp;
/* not sure if I should change the "&&" here to another "||"...: */
#if defined(__APPLE__) && (defined(__darwin_time_t) || defined(HAVE___DARWIN_TIME_T))
	/* sizes of members of struct timeval vary by system, so since we set the
	 * value of secbase to a member of struct timeval later, make sure that it
	 * is the right type here first, so that it will not be necessary to cast
	 * it later: */
	static __darwin_time_t secbase;
#elif !defined(__APPLE__) && (defined(time_t) || defined(HAVE_TIME_T))
	static time_t	secbase;
#elif defined(SIZEOF_LONG) && defined(SIZEOF_INT)
/* this is not entirely right; we should really be checking the size of the
 * type of the member tv_sec of struct timeval, and then comparing _that_ with
 * the size of int, but I am not sure exactly how to do that: */
# if SIZEOF_LONG > SIZEOF_INT
	static long		secbase;
# else
	static int		secbase;
# endif /* SIZEOF_LONG > SIZEOF_INT */
#else
	static int		secbase;
#endif /* end conditions for different types of tv_sec */

	gettimeofday(&tp, &tzp);

	if (! secbase) {
		secbase = tp.tv_sec;
		return (W32)(tp.tv_usec / 1000);
	}

	/* some of these parentheses are just to help me remember the order of
	 * operations (others are actually needed though): */
	curtime = (W32)((tp.tv_sec - secbase) * 1000) +
					(unsigned long)(tp.tv_usec / 1000);

	return curtime;
}

/* EOF */
