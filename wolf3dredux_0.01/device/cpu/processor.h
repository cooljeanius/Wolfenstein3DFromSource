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
 *      processor.h:	CPU Identification
 *
 *      Author:  Michael Liebscher    <johnnycanuck@users.sourceforge.net>
 *      Date:    2004
 *
 */

/*
 *	Notes:
 *
 *	Reference documents:
 *	ftp://download.intel.com/design/pro/applnots/24161809.pdf
 *	http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/20734.pdf
 *  (dead links)
 *
 *	This module is implemented by x86_cpu.c (actually by cpuid.c)
 */


#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "../../common/arch.h"


typedef enum
{
	UNKNOWN_X86 = 0,
	NON_X86		= 1,
	INTEL_X86   = 2,
	AMD_X86     = 3

} CPU_CLASS;


typedef struct
{
	/* TODO: check compiler flags for these */
	_boolean bRDTSC;	/* Is RDTSC supported? */
	_boolean bCMOV;		/* Is CMOV supported? */
	_boolean bFCMOV;	/* Is FCMOV supported? */
	_boolean bMMX;		/* Is MMX supported? */
	_boolean bMMXExt;	/* Is MMX Extension supported? */
	_boolean b3DNow;	/* Is 3DNow! Supported?	*/
	_boolean b3DNowExt;	/* Is 3DNow! Extension Supported? */
	_boolean bSSE;		/* Is SSE supported? */
	_boolean bSSE2;		/* Is SSE2 Supported? */
	_boolean bSSE3;		/* Is SSE3 Supported? */

	CPU_CLASS cpu_type;
	W32 cpu_frequency;

}  cpu_info_struct;


extern cpu_info_struct main_cpu_s;


extern void Get_CPU_info(void);
/* Now that we have the prototype for it, we should "HAVE" it: */
#ifndef HAVE_GET_CPU_INFO
# define HAVE_GET_CPU_INFO 1
#endif /* !HAVE_GET_CPU_INFO */


#endif /* __PROCESSOR_H__ */

/* EOF */
