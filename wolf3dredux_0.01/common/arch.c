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
 *	arch.c: Portable byte swapping.
 *
 *	Author:	    Id Software, Inc.
 *	Date:	    1997-2001
 *
 */

#include "arch.h"

#ifndef INLINECALL
# ifdef __STRICT_ANSI__
#  ifdef __inline__
#   define INLINECALL __inline__
#  else
#   ifdef __NO_INLINE__
#    warning "INLINECALL will be unavailable when using the '-ansi' compiler flag"
#   endif /* __NO_INLINE__ */
#   define INLINECALL /* nothing */
#  endif /* __inline__ */
# else
#  define INLINECALL inline
# endif /* __STRICT_ANSI__ */
#endif /* !INLINECALL */

/*
-----------------------------------------------------------------------------
 Function: FloatSwap	-Endian byte swapping on Float value.

 Parameters: f -[in] Float value to byte swap.

 Returns: Byte swapped float value.

 Notes:

-----------------------------------------------------------------------------
*/
INLINECALL float FloatSwap( float f )
{
	union {
		float	f;
		W8	b[ 4 ];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[ 0 ] = dat1.b[ 3 ];
	dat2.b[ 1 ] = dat1.b[ 2 ];
	dat2.b[ 2 ] = dat1.b[ 1 ];
	dat2.b[ 3 ] = dat1.b[ 0 ];

	return dat2.f;
}

/* EOF */
