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
 *	memory.c:	Memory allocation module.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

#ifdef HAVE_CONFIG_H
# include "../../../config.h"
#else
# define MEMORY_C_NON_AUTOTOOLS_BUILD 1
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#elif defined(HAVE_MALLOC_MALLOC_H)
# include <malloc/malloc.h>
#endif /* HAVE_MALLOC[_MALLOC]_H */
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif /* HAVE_LIMITS_H */
#ifdef HAVE_SYS_MALLOC_H
# include <sys/malloc.h>
#endif /* HAVE_SYS_MALLOC_H */
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */
#ifndef HAVE_SYS_SYSCTL_H
# ifdef __APPLE__
#  define HAVE_SYS_SYSCTL_H 1
# endif /* __APPLE__ */
#endif /* !HAVE_SYS_SYSCTL_H */
#ifdef HAVE_SYS_SYSCTL_H
# include <sys/sysctl.h>
#else
# if !defined(MEMORY_C_NON_AUTOTOOLS_BUILD) && defined(HAVE_SYSCTL)
#  if __GNUC__ && !__STRICT_ANSI__
#   warning "<sys/sysctl.h> needs to be included to use sysctl()"
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* !MEMORY_C_NON_AUTOTOOLS_BUILD && HAVE_SYSCTL */
#endif /* HAVE_SYS_SYSCTL_H */
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#else
# if !defined(MEMORY_C_NON_AUTOTOOLS_BUILD) && defined(HAVE_SYSCTL)
#  if __GNUC__ && !__STRICT_ANSI__
#   warning "<sys/types.h> is also necessary for using sysctl()"
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* !MEMORY_C_NON_AUTOTOOLS_BUILD && HAVE_SYSCTL */
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "memory.h"


#ifndef PARANOID
# define PARANOID 1
#endif /* !PARANOID */

#if defined(HAVE_DMALLOC_H) && defined(HAVE_DMALLOC_MALLOC) && defined(WITH_DMALLOC) && PARANOID
# include <dmalloc.h>
#endif /* HAVE_DMALLOC_H && HAVE_DMALLOC_MALLOC && WITH_DMALLOC && PARANOID */

int times_memory_malloc_was_called = 0;

/*
-----------------------------------------------------------------------------
 Function: Memory_malloc -Allocates memory blocks.

 Parameters: size -[in] Bytes to allocate.

 Returns:
			Void pointer to the allocated space on success, or NULL if
			there is insufficient memory available.

 Notes:

-----------------------------------------------------------------------------
*/
/* TODO: put this function in a shared library */
#ifndef Memory_malloc /* this ifdef is bad */
PUBLIC void *Memory_malloc(size_t size)
#else
PUBLIC void *wolfextractor_Memory_malloc(size_t size)
#endif /* !Memory_malloc */
{
    void *ptr;

	times_memory_malloc_was_called++; /* makes it '1' the first time through */
#ifdef HAVE_SYSCTL
	/* taken from memory printing function in
	 * "../../../device/system/unix/unix_system.c": */
	int mib[2];
	int value;
	unsigned int uvalue;
	size_t len;

	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM;
	len = sizeof(value);
	sysctl(mib, 2, &value, &len, NULL, 0);

	uvalue = (unsigned)abs(value);

# if PARANOID || __clang_analyzer__
	if (abs(times_memory_malloc_was_called) <= 1) {
		printf("Available Physical Memory: %u \n", uvalue);
	}
# endif /* PARANOID || __clang_analyzer__ */
	if (size > uvalue) {
		printf("Error: attempt to malloc more memory (%ld) than there is physical memory (%u)\n",
			   size, uvalue);
		return NULL;
	}
#endif /* HAVE_SYSCTL */
#if PARANOID || __clang_analyzer__
	printf("Attempting malloc of size:%ld\n", size);
#endif /* PARANOID || __clang_analyzer__ */
    ptr = malloc(size);
	/* I wish Xcode 3.2.6 had per-target breakpoints... I set 1 here for
	 * debugging wolfextractor, and then when switching back to building my
	 * main target, it for some reason thought that this breakpoint was in the
	 * different, separate memory.c that is compiled for the main target...
	 * darn duplicated code... */

    if (ptr != NULL) {
#if PARANOID || __clang_analyzer__
		printf("Memory malloc: ptr: %p, size: %ld\n", ptr, size);
#endif /* PARANOID || __clang_analyzer__ */
        return ptr;
    }

    printf("[memory.c] Could not allocate %d bytes\n", (int)size);

    return NULL;
}


/*
-----------------------------------------------------------------------------
 Function: Memory_calloc -Allocates an array in memory with elements
							initialized to 0.

 Parameters:
			num -[in] Number of elements.
			size -[in] Bytes to allocate.

 Returns:
			Void pointer to the allocated space on success, or NULL if
			there is insufficient memory available.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void *Memory_calloc(size_t num, size_t size)
{
    void *ptr;
    ptr = calloc(num, size );

    if (ptr != NULL) {
#if PARANOID || __clang_analyzer__
		printf("Memory calloc: %p size:%ld num:%ld\n", ptr, size, num);
#endif /* PARANOID || __clang_analyzer__ */
        return ptr;
    }

    printf("[memory.c] Could not allocate %d objects of size %d\n",
		   (int)num, (int)size);

    return NULL;
}

/*
-----------------------------------------------------------------------------
 Function: Memory_realloc -Reallocate memory blocks.

 Parameters:
			memblock -[in] Pointer to previously allocated memory block.
			size -[in] Bytes to allocate.

 Returns:
		A void pointer to the reallocated (and possibly moved) memory
		block. The return value is NULL if the size is zero and the
		buffer argument is not NULL, or if there is not enough
		available memory to expand the block to the given size.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void *Memory_realloc(void *memblock, size_t size)
{
    void *ptr;

    ptr = realloc(memblock, size);

    if (ptr != NULL) {
#if PARANOID || __clang_analyzer__
		printf("Memory realloc: %p size:%ld\n", ptr, size);
#endif /* PARANOID || __clang_analyzer__ */

        return ptr;
    }

    printf("[memory.c] Could not reallocate %d bytes\n", (int)size);

    return NULL;
}


/*
-----------------------------------------------------------------------------
 Function: Memory_free -Deallocates or frees a memory block.

 Parameters:
			memblock -[in] Previously allocated memory block to be freed.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Memory_free(void *memblock)
{
    if (memblock) {
#if PARANOID || __clang_analyzer__
		printf("Memory free: %p\n", memblock);
#endif /* PARANOID || __clang_analyzer__ */

	    free(memblock);
    }
}


PUBLIC void Memory_outofmem(const char *name, const char *file, W32 line)
{

    printf("%s:%ld failed allocation for \"%s\"\n", file, line, name);

}

/* EOF */
