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

#include "../../../common/cmd.h"

#include "../../../common/common_utils.h"

#include "wolf_common.h" /* new location for prototypes from this file */


extern void Client_Frame(int msec);


/*
-----------------------------------------------------------------------------
 Function: common_Frame

 Parameters:

 Returns: Nothing

 Notes: Wrapper around either Server_Frame() or Client_Frame(), depending on
		the context
		(right now there is no server context, so just use the client one)
-----------------------------------------------------------------------------
*/
/* prototype has moved to "wolf_common.h" */
PUBLIC void common_Frame(int msec)
{
	/* make sure anything left in the command line buffer has been executed: */
	Cbuf_Execute();

#if 0
	Server_Frame(msec);
#endif /* 0 */

	/* client spends most of its time in here: */
	Client_Frame(msec);
}

/* EOF */
