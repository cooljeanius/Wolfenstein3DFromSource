/*
 *	Copyright (C) 2004 Michael Liebscher
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
 *	renderer.h: Interface to graphics API.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

/*
 *	Notes:
 *
 *	This module communicates with the graphics API. The API can be any graphics
 *	API, e.g OpenGL, DirectX, SDL, GDI, etc; as long as the functions listed in
 *	this header are implemented.
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "../device/filesys/filesystem.h"


#include "../common/arch.h"
#include "texture_manager.h"
#include "../font/font_manager.h"

/* OPENGL_DLL_NAME must be dlopen()-able */
#ifdef _WIN32
# define OPENGL_DLL_NAME   "opengl32.dll"
#elif __unix__
# define OPENGL_DLL_NAME   "libGL.so.1"
#elif __APPLE__
# define OPENGL_DLL_NAME "libGL.1.dylib" /* needs the ".1", without would be a symlink */
/* (no, seriously, adding the ".1" is what made the difference for me...) */
/* dylib is for non-framework builds; not sure what to do for OpenGL.framework... */
#else
# error "Define OPENGL_DLL_NAME for your platform"
#endif /* platform */


typedef enum
{
        rserr_ok,

        rserr_invalid_fullscreen,
        rserr_invalid_mode,

        rserr_unknown

} rserr_t;


#ifndef registration_sequence
# ifdef __private_extern__
__private_extern__ int registration_sequence;
# elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
static const int registration_sequence;
# else
int registration_sequence;
# endif /* __private_extern__ || (__GNUC__ && !__STRICT_ANSI__) */
#else /* do already have 'registration_sequence' defined: */
# ifdef __private_extern__
__private_extern__ int renderer_registration_sequence;
# elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
static const int renderer_registration_sequence;
# else
int renderer_registration_sequence;
# endif /* __private_extern__ || (__GNUC__ && !__STRICT_ANSI__) */
#endif /* !registration_sequence */

extern int R_Init(void *hinstance, void *hWnd);
extern void R_Shutdown(void);

extern void R_BeginRegistration(const char *model);
extern void R_EndRegistration(void);
extern void R_BeginFrame(void);
extern void R_EndFrame(void);

extern void R_AppActivate(_boolean active);
extern void R_SwapBuffers(int);
extern void R_SetPalette(const unsigned char *palette);



extern void R_DeleteTexture(unsigned int texnum);
extern _boolean R_UploadTexture(texture_t *tex, PW8 data);


extern void R_SetGL2D(void);


extern void R_Draw_Pic(int x, int y, const char *name);
extern void R_Draw_StretchPic(int x, int y, int w, int h, const char *name);
extern void R_Draw_Character(int x, int y, int num, font_t *myfont);
extern void R_Draw_Tile(int x, int y, int w, int h, const char *name);
extern void R_Draw_Fill(int x, int y, int w, int h, colour3_t c);
extern void R_Draw_Line(int nXStart, int nYStart, int nXEnd, int nYEnd,
						int width, colour3_t c);

#endif /* __RENDERER_H__ */

/* EOF */
