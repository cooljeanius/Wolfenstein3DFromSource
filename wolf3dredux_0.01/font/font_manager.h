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
 *	font_manager.h: Font management.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

/*
 *	Notes:
 *	This module is implemented by font_manager.c.
 */

#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__


#include "../common/arch.h"
#include "../video/texture_manager.h"


typedef enum { FONT0 = 0, FONT1, FONT2, FONT3 } FONTSELECT;

typedef struct
{
	W8 nCharWidth[ 128 ]; /* width of each character */
	W8 nMaxWidth; /* box width */
	W8 nMaxHeight; /* box height */
	SW32 spacing; /* space between characters */
	W32 nSize;
	float wFrac;
	float hFrac;
	colour4_t colour; /* Font colour */
	texture_t *texfont;

} font_t;

typedef struct
{
	char *start, *end;

} string_seg_t; /* used to be in font_manager.c */
/* just declared it, so we should have it now: */
#ifndef HAVE_STRING_SEG_T
# define HAVE_STRING_SEG_T 1
#endif /* !HAVE_STRING_SEG_T */

extern font_t *createFont(const char *filename);

extern void Font_Init(void);
extern void Font_Shutdown(void);


extern void Font_SetSize(FONTSELECT fs, W16 size);
extern W16 Font_GetSize(FONTSELECT fs);

extern void Font_SetColour(FONTSELECT fs, colour3_t c);

extern void Font_put_line(FONTSELECT fs, int x, int y, const char *string);
extern void Font_put_lineR2L(FONTSELECT fs, int x, int y, const char *string);
extern void Font_put_line_size(FONTSELECT fs, int x, int y, const char *start,
							   const char *end);
#if defined(HAVE_STRING_SEG_T) || defined(string_seg_t)
W8 Font_get_line(FONTSELECT fs, int line_width, string_seg_t *sst);
#else
# warning "cannot declare a prototype for Font_get_line() due to missing string_seg_t type."
#endif /* HAVE_STRING_SEG_T || string_seg_t */
extern W16 Font_put_character(FONTSELECT fs, int x, int y, W16 num);

extern void Font_put_paragraph(FONTSELECT fs, short x, short y,
							   const char *string,
							   int space_between_lines,
							   int line_width_in_pixel);

extern void Font_GetMsgDimensions(FONTSELECT fs, const char *string, int *w, int *h);

#endif /* __FONT_MANAGER_H__ */

/* EOF */
