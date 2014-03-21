/*
 *	Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	arch.h:	System dependant #defines and macros.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#ifndef __ARCH_H__
# define __ARCH_H__

# include "../config.h"

/* Define BUILDSTRING and CPUSTRING based on platform */
# ifdef _WIN32
#  ifdef _DEBUG
#   define BUILDSTRING "Win32 DEBUG"
#  else
#   define BUILDSTRING "Win32 RELEASE"
#  endif /* _DEBUG */
#  ifdef _M_IX86
#   define CPUSTRING       "x86"
#  elif defined _M_ALPHA
#   define CPUSTRING       "AXP"
#  else
#   define CPUSTRING       "Unknown CPU"
#  endif /* _M_IX86 || _M_ALPHA */
# elif defined __linux__
#  define BUILDSTRING "Linux"
#  ifdef __i386__
#   define CPUSTRING "i386"
#  elif defined __alpha__
#   define CPUSTRING "AXP"
#  else
#   define CPUSTRING "Unknown CPU"
#  endif /* __i386__ || __alpha__ */
# elif defined __FreeBSD__
#  define BUILDSTRING "FreeBSD"
#  ifdef __i386__
#   define CPUSTRING "i386"
#  else
#   define CPUSTRING "Unknown CPU"
#  endif /* __i386__ */
# elif defined __sun__
#  define BUILDSTRING "Solaris"
#  ifdef __i386__
#   define CPUSTRING "i386"
#  else
#   define CPUSTRING "sparc"
#  endif /* __i386__ */
# elif defined MACOS || defined __APPLE__
#  define BUILDSTRING "MAC"
#  ifdef __MACH__
#   define BUILDSTRING_EXTENSION "OSX"
#  endif /* __MACH__ */
#  ifdef __powerpc__
#   define CPUSTRING "PowerPC"
#  elif defined __i386__ || defined __x86_64__
#   define CPUSTRING "i386"
#  else
#   define CPUSTRING "Unknown CPU"
#  endif /* __powerpc__ || (__i386__ || __x86_64__) */
# else
#  warning "Unknown OS and CPU"
#  define BUILDSTRING "Unknown OS"
#  define CPUSTRING "Unknown CPU"
# endif /* WIN32 || __linux__ || __FreeBSD__ || __sun__ || MACOS */


/*
 *	correct numeric types:  W8, SW8, W16, SW16, W32, SW32, W64, SW64
 *	correct misc types:	void, float, _boolean
 *
 *	s       -signed
 *	XX      -Number of bits
 */
# if ( __GNUC__ || __WATCOMC__ || _MSC_VER )
typedef unsigned char   W8,		*PW8;
typedef signed char		SW8,    *PSW8;
typedef unsigned short  W16,    *PW16;
typedef signed short    SW16,   *PSW16;
typedef unsigned long   W32,    *PW32;
typedef signed long		SW32,   *PSW32;
#  if ( __GNUC__ )
/* tentative check for 64-bit support, not sure if correct: */
#   if __STDC__ && (__LP64__ || _LP64 || __amd64 || __amd64__ || __x86_64 || __x86_64__ || __ppc64__ || _ARCH_PPC64 || (HAVE_LONG_LONG && HAVE_UNSIGNED_LONG_LONG))
#    if __STRICT_ANSI__ && 0
/* remove the "0" to make this an actual error: */
#     error "files including arch.h cannot be compiled with the '-ansi' compiler flag; 'long long' support is necessary."
#    else
typedef unsigned long long	W64,    *PW64;
typedef long long			SW64,   *PSW64;
#    endif /* __STRICT_ANSI__ && 0 */
#   elif (HAVE_LONG_LONG_INT && HAVE_UNSIGNED_LONG_LONG_INT)
typedef unsigned long long int	W64,    *PW64;
typedef long long int			SW64,   *PSW64;
#   else
#    error "Your gcc is missing 64-bit support"
#   endif /* 64-bit check */
#  elif ( _MSC_VER || __WATCOMC__ )
typedef unsigned __int64    W64,	*PW64;
typedef __int64				SW64,   *PSW64;
#  else
#   error "please define W64"
#  endif /* specific compiler check */
# else
#  error "Unknown compiler, please define basic types"
# endif /* compiler check */


/* Define NULL pointer value */
# ifndef NULL
#  ifdef __cplusplus
#   define NULL    0
#  else
#   define NULL    ((void *)0)
#  endif /* __cplusplus */
# endif /* NULL */


/* Define INLINECALL keyword */
/* (TODO: just use what the AC_C_INLINE autoconf macro tells us, in "../config.h") */
/* (or could check for __GNUC_GNU_INLINE__ or __GNUC_STDC_INLINE__ macros) */
# ifndef INLINECALL
#  if defined(__cplusplus) || defined(__GNUC__)
#   ifdef __STRICT_ANSI__
#    ifdef __inline__
#     define INLINECALL __inline__
#    else
#     if defined(__NO_INLINE__) && defined(__GNUC__) && !defined(__STDC__)
#      warning "INLINECALL will be unavailable when using the '-ansi' compiler flag"
#     endif /* __NO_INLINE__ && __GNUC__ && !__STDC__ */
#     define INLINECALL	/* nothing (not supported) */
#    endif /* __inline__ */
#   else
#    define INLINECALL inline
#   endif /* __STRICT_ANSI__ */
#  elif defined(_WIN32) && !defined(__WATCOMC__)
#   define INLINECALL __inline
#  else
#   ifdef __NO_INLINE__
#    warning "INLINECALL will be unavailable"
#   endif /* __NO_INLINE__ */
#   define INLINECALL	/* Not supported */
#  endif /* compiler check */
# endif /* INLINECALL */

/* Now that we have the INLINECALL keyword, we can put our prototype: */
extern float FloatSwap(float f);
/* (the actual implementation of the function in arch.c uses the INLINECALL
 *  keyword, even if it is not present here in the prototype for it) */

typedef W8	colour3_t[3]; /* RGB */
typedef W8	colour4_t[4]; /* RGBA */
typedef W32 COLOURVAL, *PCOLOURVAL;  /* Represents a 32-bit colour value. */


typedef W8 _boolean;

/* TODO: fix conflicts with system definitions of 'true' and 'false': */
# if !defined false && !defined true
enum { false = 0,
	   true = 1 };
# endif /* !false && !true */


# define ShortSwap(x)  \
(((((W16)(x)) & 0x00FF) << 8) | ((((W16)(x)) & 0xFF00) >> 8))
# define LongSwap(x)   \
((((W32)(x)) & 0xFF000000) >> 24) | (((((W32)(x)) & 0xFF0000) >> 8)) | (((((W32)(x)) & 0xFF00) << 8)) | ((((W32)(x)) & 0xFF) << 24)


# if defined(__i386__) || defined(_M_IX86) || defined __LITTLE_ENDIAN__
/* Little endian: */
#  define BigShort(x)           ShortSwap(x)
#  define LittleShort(x)        (x)
#  define BigLong(x)            LongSwap(x)
#  define LittleLong(x)         (x)
#  define BigFloat(x)           FloatSwap(x)
#  define LittleFloat(x)        (x)
# else /* Big endian: */
#  define BigShort(x)           (x)
#  define LittleShort(x)        ShortSwap(x)
#  define BigLong(x)            (x)
#  define LittleLong(x)         LongSwap(x)
#  define BigFloat(x)           (x)
#  define LittleFloat(x)        FloatSwap(x)
# endif /* endianness check */

#endif /* __ARCH_H__ */

/* EOF */
