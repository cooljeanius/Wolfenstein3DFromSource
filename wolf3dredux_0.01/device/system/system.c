/*
 *	Copyright (C) 2004 Michael Liebscher
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 * 	as published by the Free Software Foundation; either version 2
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
 *	system.c:  Print out system information.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

#ifdef __PROCESSOR_H__
# warning "../cpu/processor.h was already included once."
#endif /* __PROCESSOR_H__ */
#include "../cpu/processor.h"
#ifndef __PROCESSOR_H__
# warning "../cpu/processor.h was not actually included."
#endif /* !__PROCESSOR_H__ */
#include "../../common/common.h"
#include "system.h"

extern void Print_Memory_Stats(void);
extern void Print_OS_Info(void);

/*
-----------------------------------------------------------------------------
 Function: Sys_Init -Initialize system

 Parameters: Nothing

 Returns: Nothing

 Notes: Initilize CPU, print out memory usage and do any OS dependent stuff.
-----------------------------------------------------------------------------
*/
PUBLIC void Sys_Information(void)
{
	Com_Printf("\n----- Determining System Specs -----\n");

#if defined(Get_CPU_info) || defined(HAVE_GET_CPU_INFO)
	Com_Printf("[CPU]\n");
	Get_CPU_info(); /* (should have been from ../cpu/cpuid.c) */
#endif /* 0 */

	Com_Printf("[Memory]\n");
	Print_Memory_Stats();

	Com_Printf("[Operating System]\n");
	Print_OS_Info();

	Com_Printf("------------------------------------\n\n");
}

/* EOF */
