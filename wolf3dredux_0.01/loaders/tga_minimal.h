/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *	Copyright (C) 1995 Spencer Kimball and Peter Mattis
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
 *	tga.h:   Handle Targa file format.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was
 *	originally written by id Software, Inc.
 *
 *	Portion of this code was derived from The GIMP -- an image manipulation
 *	program, and was originally written by Spencer Kimball and Peter Mattis.
 */

#ifndef __TGA_H__
# define __TGA_H__
# include "../common/arch.h"

extern void LoadTGA(const char *filename, W8 **pic, W16 *width, W16 *height, W16 *bytes);


extern W8 WriteTGA(const char *filename, W16 bpp, W16 width, W16 height,
				   void *Data, W8 upsideDown, W8 rle);

#else /* in case also including the other "tga.h" header: */
# ifndef __TGA_MINIMAL_H__
#  define __TGA_MINIMAL_H__
#  include "../common/arch.h"
/* prototypes for functions from tga.c: */
#  ifndef LoadTGA
extern void LoadTGA(const char *filename, W8 **pic, W16 *width, W16 *height, W16 *bytes);
#  endif /* !LoadTGA */
#  ifndef bgr2rgb
/* originally said "private" in the implementation, but I need to extern it anyways: */
extern void bgr2rgb(W8 *dest, W8 *src, W32 width, W32 bytes, W32 alpha);
#  else /* bgr2rgb already defined */
#   ifndef __TGAbgr2rgb
#    warning "using bgr2rgb as a replacement for __TGAbgr2rgb; this might not work."
#    define __TGAbgr2rgb bgr2rgb
#   endif /* !__TGAbgr2rgb */
#  endif /* !bgr2rgb */
/* prototypes for functions from tgaread.c: */
#  ifndef TGARead
extern size_t TGARead(TGA *tga, tbyte *buf, size_t size, size_t n);
#  endif /* !TGARead */
#  ifndef TGAReadRLE
extern int TGAReadRLE(TGA *tga, tbyte *buf);
#  endif /* !TGAReadRLE */
/* prototypes for functions from tgawrite.c: */
#  ifndef TGAWrite
extern size_t TGAWrite(TGA *tga, const tbyte *buf, size_t size, size_t n);
#  endif /* !TGAWrite */
#  ifndef TGAWriteRLE
extern int TGAWriteRLE(TGA *tga, tbyte *buf);
#  endif /* !TGAWriteRLE */
# endif /* !__TGA_MINIMAL_H__ */
#endif /* __TGA_H__ */

/* EOF */
