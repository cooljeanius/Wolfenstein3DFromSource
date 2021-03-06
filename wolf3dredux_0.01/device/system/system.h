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
 *	system.h:  Print out system information.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

extern void Sys_OS_Init(void);
extern void Sys_Information(void);

/* These are implemented in either unix_system.c or win_system.c, depending
 * on platform: */
extern void Print_OS_Info(void);
extern void Print_Memory_Stats(void);

#endif /* __SYSTEM_H__ */

/* EOF */
