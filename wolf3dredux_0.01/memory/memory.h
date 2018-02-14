/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	memory.h: Memory allocation manager.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 */

/*
 *	Notes:
 *	This module is implemented by memory.c.
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>

#include "../common/arch.h"
#include "../common/common_utils.h"


/* Use the macros instead of these prototypes (why?): */
extern void *Memory_malloc(size_t size);
extern void *Memory_calloc(size_t num, size_t size);
extern void *Memory_realloc(void *memblock, size_t size);
extern void Memory_free(void *memblock);

extern void Memory_outofmem(const char *name, const char *file, W32 line)
	ATTR_NORETURN;

/* These are the macros to actually use instead of the prototypes that they
 * are simple wrappers around: */
#define MM_MALLOC(size)				Memory_malloc((size_t)(size))
#define MM_CALLOC(num, size)		Memory_calloc((size_t)(num), (size_t)(size))
#define MM_REALLOC(memblock, size)	Memory_realloc((void *)(memblock), (size_t)(size))

#define MM_FREE(memblock)	{ Memory_free((void *)(memblock)); ((memblock)) = NULL; }

#define MM_OUTOFMEM(name)	Memory_outofmem((const char *)(name), (const char *)__FILE__, (W32)__LINE__)


#endif /* __MEMORY_H__ */

/* EOF */
