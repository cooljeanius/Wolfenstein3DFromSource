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
 *	myopengl_extension.h:   Interface to OpenGL extensions.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */

/*
 *	Notes:
 *
 *	This module is implemented by opengl_extenstion.c
 */


#ifndef __MYOPENGL_EXTENSION_H__
#define __MYOPENGL_EXTENSION_H__

#ifdef __APPLE__
/* assume we are doing a build against OpenGL.framework */
# include <OpenGL/glext.h>
# include <OpenGL/OpenGL.h> /* also includes <OpenGL/gl.h> */
/* TODO: allow building against non-framework OpenGL installations */
#else /* not __APPLE__ */
# include <GL/glext.h>	/* Download from http://www.opengl.org/registry/ */
# include <GL/gl.h>
#endif /* __APPLE__ */


#ifndef APIENTRY
# define APIENTRY
#endif /* !APIENTRY */

#ifndef APIENTRYP
# define APIENTRYP APIENTRY *
#endif /* !APIENTRYP */


/*****************************************************************************
 *
 * OpenGL Extensions
 *
 *****************************************************************************/


#define GL_POINT_SIZE_MIN_EXT				0x8126
#define GL_POINT_SIZE_MAX_EXT				0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT	0x8128
#define GL_DISTANCE_ATTENUATION_EXT			0x8129

#ifdef __sgi
# define GL_SHARED_TEXTURE_PALETTE_EXT		GL_TEXTURE_COLOR_TABLE_SGI
#else
# define GL_SHARED_TEXTURE_PALETTE_EXT		0x81FB
#endif /* __sgi */


#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3

#ifndef WGL_ARB_pixel_format
	#define WGL_ARB_pixel_format				1
	#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
	#define WGL_DRAW_TO_WINDOW_ARB				0x2001
	#define WGL_DRAW_TO_BITMAP_ARB				0x2002
	#define WGL_ACCELERATION_ARB				0x2003
	#define WGL_NEED_PALETTE_ARB				0x2004
	#define WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
	#define WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
	#define WGL_SWAP_METHOD_ARB					0x2007
	#define WGL_NUMBER_OVERLAYS_ARB				0x2008
	#define WGL_NUMBER_UNDERLAYS_ARB			0x2009
	#define WGL_TRANSPARENT_ARB					0x200A
	#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
	#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
	#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
	#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
	#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
	#define WGL_SHARE_DEPTH_ARB					0x200C
	#define WGL_SHARE_STENCIL_ARB				0x200D
	#define WGL_SHARE_ACCUM_ARB					0x200E
	#define WGL_SUPPORT_GDI_ARB					0x200F
	#define WGL_SUPPORT_OPENGL_ARB				0x2010
	#define WGL_DOUBLE_BUFFER_ARB				0x2011
	#define WGL_STEREO_ARB						0x2012
	#define WGL_PIXEL_TYPE_ARB					0x2013
	#define WGL_COLOR_BITS_ARB					0x2014
	#define WGL_RED_BITS_ARB					0x2015
	#define WGL_RED_SHIFT_ARB					0x2016
	#define WGL_GREEN_BITS_ARB					0x2017
	#define WGL_GREEN_SHIFT_ARB					0x2018
	#define WGL_BLUE_BITS_ARB					0x2019
	#define WGL_BLUE_SHIFT_ARB					0x201A
	#define WGL_ALPHA_BITS_ARB					0x201B
	#define WGL_ALPHA_SHIFT_ARB					0x201C
	#define WGL_ACCUM_BITS_ARB					0x201D
	#define WGL_ACCUM_RED_BITS_ARB				0x201E
	#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
	#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
	#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
	#define WGL_DEPTH_BITS_ARB					0x2022
	#define WGL_STENCIL_BITS_ARB				0x2023
	#define WGL_AUX_BUFFERS_ARB					0x2024

	#define WGL_NO_ACCELERATION_ARB				0x2025
	#define WGL_GENERIC_ACCELERATION_ARB		0x2026
	#define WGL_FULL_ACCELERATION_ARB			0x2027

	#define WGL_SWAP_EXCHANGE_ARB				0x2028
	#define WGL_SWAP_COPY_ARB					0x2029
	#define WGL_SWAP_UNDEFINED_ARB				0x202A

	#define WGL_TYPE_RGBA_ARB					0x202B
	#define WGL_TYPE_COLORINDEX_ARB				0x202C
#endif /* WGL_ARB_pixel_format */

#ifndef WGL_ARB_pbuffer
	#define WGL_ARB_pbuffer					1
	#define WGL_DRAW_TO_PBUFFER_ARB			0x202D
	#define WGL_MAX_PBUFFER_PIXELS_ARB		0x202E
	#define WGL_MAX_PBUFFER_WIDTH_ARB		0x202F
	#define WGL_MAX_PBUFFER_HEIGHT_ARB		0x2030
	#define WGL_PBUFFER_LARGEST_ARB			0x2033
	#define WGL_PBUFFER_WIDTH_ARB			0x2034
	#define WGL_PBUFFER_HEIGHT_ARB			0x2035
	#define WGL_PBUFFER_LOST_ARB			0x2036
#endif /* !WGL_ARB_pbuffer */

/* need to add "extern" for clang which defaults to -fno-common, but that leads
 * to missing symbol link errors: */
#ifndef OPENGL_EXTERN
# if 0
#  define OPENGL_EXTERN extern
# else
#  define OPENGL_EXTERN /* (nothing) */
# endif /* 0 */
#endif /* !OPENGL_EXTERN */
/*
 *	-----------------------
 *	ARB_MultiTexture
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglActiveTextureARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglClientActiveTextureARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1dARB) (GLenum, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1dvARB) (GLenum, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1fARB) (GLenum, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1fvARB) (GLenum, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1iARB) (GLenum, GLint);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1ivARB) (GLenum, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1sARB) (GLenum, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord1svARB) (GLenum, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2dARB) (GLenum, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2dvARB) (GLenum, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2fARB) (GLenum, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2fvARB) (GLenum, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2iARB) (GLenum, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2ivARB) (GLenum, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2sARB) (GLenum, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord2svARB) (GLenum, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3dARB) (GLenum, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3dvARB) (GLenum, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3fARB) (GLenum, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3fvARB) (GLenum, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3iARB) (GLenum, GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3ivARB) (GLenum, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3sARB) (GLenum, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord3svARB) (GLenum, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4dARB) (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4dvARB) (GLenum, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4fARB) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4fvARB) (GLenum, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4iARB) (GLenum, GLint, GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4ivARB) (GLenum, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4sARB) (GLenum, GLshort, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglMultiTexCoord4svARB) (GLenum, const GLshort *);


/*
 *	-----------------------
 *	ARB_TransposeMatrix
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglLoadTransposeMatrixfARB) (const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglLoadTransposeMatrixdARB) (const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglMultTransposeMatrixfARB) (const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglMultTransposeMatrixdARB) (const GLdouble *);


/*
 *	-----------------------
 *	ARB_MultiSample
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglSampleCoverageARB) (GLclampf, GLboolean);


/*
 *	-----------------------
 *	ARB_TextureCompression
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexImage3DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexImage2DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexImage1DARB) (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexSubImage3DARB) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexSubImage2DARB) (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglCompressedTexSubImage1DARB) (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglGetCompressedTexImageARB) (GLenum, GLint, GLvoid *);


/*
 *	-----------------------
 *	ARB_PointParameters
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglPointParameterfARB) (GLenum, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglPointParameterfvARB) (GLenum, const GLfloat *);


/*
 *	-----------------------
 *	ARB_VertexBlend
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglWeightbvARB) (GLint, const GLbyte *);
OPENGL_EXTERN void (APIENTRYP pfglWeightsvARB) (GLint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglWeightivARB) (GLint, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglWeightfvARB) (GLint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglWeightdvARB) (GLint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglWeightubvARB) (GLint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglWeightusvARB) (GLint, const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglWeightuivARB) (GLint, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglWeightPointerARB) (GLint, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglVertexBlendARB) (GLint);


/*
 *	-----------------------
 *	ARB_MatrixPalette
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglCurrentPaletteMatrixARB) (GLint);
OPENGL_EXTERN void (APIENTRYP pfglMatrixIndexubvARB) (GLint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglMatrixIndexusvARB) (GLint, const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglMatrixIndexuivARB) (GLint, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglMatrixIndexPointerARB) (GLint, GLenum, GLsizei, const GLvoid *);


/*
 *	------------------------------
 *	ARB_VertexProgram
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1dARB) (GLuint, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1dvARB) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1fARB) (GLuint, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1fvARB) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1sARB) (GLuint, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1svARB) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2dARB) (GLuint, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2dvARB) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2fARB) (GLuint, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2fvARB) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2sARB) (GLuint, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2svARB) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3dARB) (GLuint, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3dvARB) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3fARB) (GLuint, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3fvARB) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3sARB) (GLuint, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3svARB) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NbvARB) (GLuint, const GLbyte *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NivARB) (GLuint, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NsvARB) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NubARB) (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NubvARB) (GLuint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NuivARB) (GLuint, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4NusvARB) (GLuint, const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4bvARB) (GLuint, const GLbyte *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4dARB) (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4dvARB) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4fARB) (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4fvARB) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4ivARB) (GLuint, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4sARB) (GLuint, GLshort, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4svARB) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4ubvARB) (GLuint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4uivARB) (GLuint, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4usvARB) (GLuint, const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribPointerARB) (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglEnableVertexAttribArrayARB) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDisableVertexAttribArrayARB) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglBindProgramARB) (GLenum, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDeleteProgramsARB) (GLsizei, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglGenProgramsARB) (GLsizei, GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglProgramEnvParameter4dARB) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglProgramEnvParameter4dvARB) (GLenum, GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglProgramEnvParameter4fARB) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglProgramEnvParameter4fvARB) (GLenum, GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglProgramLocalParameter4dARB) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglProgramLocalParameter4dvARB) (GLenum, GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglProgramLocalParameter4fARB) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglProgramLocalParameter4fvARB) (GLenum, GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramEnvParameterdvARB) (GLenum, GLuint, GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramEnvParameterfvARB) (GLenum, GLuint, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramLocalParameterdvARB) (GLenum, GLuint, GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramLocalParameterfvARB) (GLenum, GLuint, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramivARB) (GLenum, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramStringARB) (GLenum, GLenum, GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribdvARB) (GLuint, GLenum, GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribfvARB) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribivARB) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribPointervARB) (GLuint, GLenum, GLvoid* *);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsProgramARB) (GLuint);


/* All ARB_fragment_program entry points are shared with ARB_vertex_program. */


/*
 *	------------------------------
 *	ARB_VertexBufferObject
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglBindBufferARB) (GLenum, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDeleteBuffersARB) (GLsizei, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglGenBuffersARB) (GLsizei, GLuint *);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsBufferARB) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglBufferDataARB) (GLenum, GLsizeiptrARB, const GLvoid *, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglBufferSubDataARB) (GLenum, GLintptrARB, GLsizeiptrARB, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglGetBufferSubDataARB) (GLenum, GLintptrARB, GLsizeiptrARB, GLvoid *);
OPENGL_EXTERN GLvoid* (APIENTRYP pfglMapBufferARB) (GLenum, GLenum);
OPENGL_EXTERN GLboolean (APIENTRYP pfglUnmapBufferARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglGetBufferParameterivARB) (GLenum, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetBufferPointervARB) (GLenum, GLenum, GLvoid* *);


/*
 *	------------------------------
 *	ARB_OcclusionQuery
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglGenQueriesARB) (GLsizei, GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglDeleteQueriesARB) (GLsizei, const GLuint *);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsQueryARB) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglBeginQueryARB) (GLenum, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglEndQueryARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglGetQueryivARB) (GLenum, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetQueryObjectivARB) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetQueryObjectuivARB) (GLuint, GLenum, GLuint *);


/*
 *	------------------------------
 *	ARB_ShaderObjects
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglDeleteObjectARB) (GLhandleARB);
OPENGL_EXTERN GLhandleARB (APIENTRYP pfglGetHandleARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglDetachObjectARB) (GLhandleARB, GLhandleARB);
OPENGL_EXTERN GLhandleARB (APIENTRYP pfglCreateShaderObjectARB) (GLenum);
OPENGL_EXTERN void (APIENTRYP pfglShaderSourceARB) (GLhandleARB, GLsizei, const GLcharARB* *, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglCompileShaderARB) (GLhandleARB);
OPENGL_EXTERN GLhandleARB (APIENTRYP pfglCreateProgramObjectARB) (void);
OPENGL_EXTERN void (APIENTRYP pfglAttachObjectARB) (GLhandleARB, GLhandleARB);
OPENGL_EXTERN void (APIENTRYP pfglLinkProgramARB) (GLhandleARB);
OPENGL_EXTERN void (APIENTRYP pfglUseProgramObjectARB) (GLhandleARB);
OPENGL_EXTERN void (APIENTRYP pfglValidateProgramARB) (GLhandleARB);
OPENGL_EXTERN void (APIENTRYP pfglUniform1fARB) (GLint, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglUniform2fARB) (GLint, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglUniform3fARB) (GLint, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglUniform4fARB) (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglUniform1iARB) (GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglUniform2iARB) (GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglUniform3iARB) (GLint, GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglUniform4iARB) (GLint, GLint, GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglUniform1fvARB) (GLint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniform2fvARB) (GLint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniform3fvARB) (GLint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniform4fvARB) (GLint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniform1ivARB) (GLint, GLsizei, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglUniform2ivARB) (GLint, GLsizei, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglUniform3ivARB) (GLint, GLsizei, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglUniform4ivARB) (GLint, GLsizei, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglUniformMatrix2fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniformMatrix3fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglUniformMatrix4fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetObjectParameterfvARB) (GLhandleARB, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetObjectParameterivARB) (GLhandleARB, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetInfoLogARB) (GLhandleARB, GLsizei, GLsizei *, GLcharARB *);
OPENGL_EXTERN void (APIENTRYP pfglGetAttachedObjectsARB) (GLhandleARB, GLsizei, GLsizei *, GLhandleARB *);
OPENGL_EXTERN GLint (APIENTRYP pfglGetUniformLocationARB) (GLhandleARB, const GLcharARB *);
OPENGL_EXTERN void (APIENTRYP pfglGetActiveUniformARB) (GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *);
OPENGL_EXTERN void (APIENTRYP pfglGetUniformfvARB) (GLhandleARB, GLint, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetUniformivARB) (GLhandleARB, GLint, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetShaderSourceARB) (GLhandleARB, GLsizei, GLsizei *, GLcharARB *);


/*
 *	------------------------------
 *	ARB_VertexShader
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglBindAttribLocationARB) (GLhandleARB, GLuint, const GLcharARB *);
OPENGL_EXTERN void (APIENTRYP pfglGetActiveAttribARB) (GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *);
OPENGL_EXTERN GLint (APIENTRYP pfglGetAttribLocationARB) (GLhandleARB, const GLcharARB *);


/*
 *	-----------------------
 *	EXT_BlendColor
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglBlendColorEXT) (GLclampf, GLclampf, GLclampf, GLclampf);


/*
 *	-------------------------
 *	EXT_CompiledVertexArray
 *	-------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglLockArraysEXT) (GLint, GLsizei);
OPENGL_EXTERN void (APIENTRYP pfglUnlockArraysEXT) (void);


/*
 *	-----------------------
 *	EXT_SecondaryColor
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3bEXT) (GLbyte, GLbyte, GLbyte);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3bvEXT) (const GLbyte *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3dEXT) (GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3dvEXT) (const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3fEXT) (GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3fvEXT) (const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3iEXT) (GLint, GLint, GLint);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3ivEXT) (const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3sEXT) (GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3svEXT) (const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3ubEXT) (GLubyte, GLubyte, GLubyte);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3ubvEXT) (const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3uiEXT) (GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3uivEXT) (const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3usEXT) (GLushort, GLushort, GLushort);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColor3usvEXT) (const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglSecondaryColorPointerEXT) (GLint, GLenum, GLsizei, const GLvoid *);


/*
 *	-----------------------
 *	EXT_VertexShader
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglBeginVertexShaderEXT) (void);
OPENGL_EXTERN void (APIENTRYP pfglEndVertexShaderEXT) (void);
OPENGL_EXTERN void (APIENTRYP pfglBindVertexShaderEXT) (GLuint);
OPENGL_EXTERN GLuint (APIENTRYP pfglGenVertexShadersEXT) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDeleteVertexShaderEXT) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglShaderOp1EXT) (GLenum, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglShaderOp2EXT) (GLenum, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglShaderOp3EXT) (GLenum, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglSwizzleEXT) (GLuint, GLuint, GLenum, GLenum, GLenum, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglWriteMaskEXT) (GLuint, GLuint, GLenum, GLenum, GLenum, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglInsertComponentEXT) (GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglExtractComponentEXT) (GLuint, GLuint, GLuint);
OPENGL_EXTERN GLuint (APIENTRYP pfglGenSymbolsEXT) (GLenum, GLenum, GLenum, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglSetInvariantEXT) (GLuint, GLenum, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglSetLocalConstantEXT) (GLuint, GLenum, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglVariantbvEXT) (GLuint, const GLbyte *);
OPENGL_EXTERN void (APIENTRYP pfglVariantsvEXT) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVariantivEXT) (GLuint, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglVariantfvEXT) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVariantdvEXT) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVariantubvEXT) (GLuint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglVariantusvEXT) (GLuint, const GLushort *);
OPENGL_EXTERN void (APIENTRYP pfglVariantuivEXT) (GLuint, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglVariantPointerEXT) (GLuint, GLenum, GLuint, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglEnableVariantClientStateEXT) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDisableVariantClientStateEXT) (GLuint);
OPENGL_EXTERN GLuint (APIENTRYP pfglBindLightParameterEXT) (GLenum, GLenum);
OPENGL_EXTERN GLuint (APIENTRYP pfglBindMaterialParameterEXT) (GLenum, GLenum);
OPENGL_EXTERN GLuint (APIENTRYP pfglBindTexGenParameterEXT) (GLenum, GLenum, GLenum);
OPENGL_EXTERN GLuint (APIENTRYP pfglBindTextureUnitParameterEXT) (GLenum, GLenum);
OPENGL_EXTERN GLuint (APIENTRYP pfglBindParameterEXT) (GLenum);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsVariantEnabledEXT) (GLuint, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantBooleanvEXT) (GLuint, GLenum, GLboolean *);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantIntegervEXT) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantFloatvEXT) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantPointervEXT) (GLuint, GLenum, GLvoid* *);
OPENGL_EXTERN void (APIENTRYP pfglGetInvariantBooleanvEXT) (GLuint, GLenum, GLboolean *);
OPENGL_EXTERN void (APIENTRYP pfglGetInvariantIntegervEXT) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetInvariantFloatvEXT) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetLocalConstantBooleanvEXT) (GLuint, GLenum, GLboolean *);
OPENGL_EXTERN void (APIENTRYP pfglGetLocalConstantIntegervEXT) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetLocalConstantFloatvEXT) (GLuint, GLenum, GLfloat *);


/*
 *	-----------------------
 *	EXT_VertexWeighting
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglVertexWeightfEXT) (GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexWeightfvEXT) (const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexWeightPointerEXT) (GLsizei, GLenum, GLsizei, const GLvoid *);



/*********************************
 *
 *	nVidia Extensions
 *
 *********************************/


/*
 *	-----------------------
 *	NV_Fence
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglDeleteFencesNV) (GLsizei, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglGenFencesNV) (GLsizei, GLuint *);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsFenceNV) (GLuint);
OPENGL_EXTERN GLboolean (APIENTRYP pfglTestFenceNV) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglGetFenceivNV) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglFinishFenceNV) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglSetFenceNV) (GLuint, GLenum);


/*
 *	-----------------------
 *	NV_VertexArrayRange
 *	-----------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglFlushVertexArrayRangeNV) (void);
OPENGL_EXTERN void (APIENTRYP pfglVertexArrayRangeNV) (GLsizei, const GLvoid *);


/*
 *	-----------------------
 *	NV_VertexProgram
 *	-----------------------
 */
OPENGL_EXTERN GLboolean (APIENTRYP pfglAreProgramsResidentNV) (GLsizei, const GLuint *, GLboolean *);
OPENGL_EXTERN void (APIENTRYP pfglBindProgramNV) (GLenum, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDeleteProgramsNV) (GLsizei, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglExecuteProgramNV) (GLenum, GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGenProgramsNV) (GLsizei, GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramParameterdvNV) (GLenum, GLuint, GLenum, GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramParameterfvNV) (GLenum, GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramivNV) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetProgramStringNV) (GLuint, GLenum, GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglGetTrackMatrixivNV) (GLenum, GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribdvNV) (GLuint, GLenum, GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribfvNV) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribivNV) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetVertexAttribPointervNV) (GLuint, GLenum, GLvoid* *);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsProgramNV) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglLoadProgramNV) (GLenum, GLuint, GLsizei, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameter4dNV) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameter4dvNV) (GLenum, GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameter4fNV) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameter4fvNV) (GLenum, GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameters4dvNV) (GLenum, GLuint, GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglProgramParameters4fvNV) (GLenum, GLuint, GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglRequestResidentProgramsNV) (GLsizei, const GLuint *);
OPENGL_EXTERN void (APIENTRYP pfglTrackMatrixNV) (GLenum, GLuint, GLenum, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribPointerNV) (GLuint, GLint, GLenum, GLsizei, const GLvoid *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1dNV) (GLuint, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1dvNV) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1fNV) (GLuint, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1fvNV) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1sNV) (GLuint, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib1svNV) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2dNV) (GLuint, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2dvNV) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2fNV) (GLuint, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2fvNV) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2sNV) (GLuint, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib2svNV) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3dNV) (GLuint, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3dvNV) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3fNV) (GLuint, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3fvNV) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3sNV) (GLuint, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib3svNV) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4dNV) (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4dvNV) (GLuint, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4fNV) (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4fvNV) (GLuint, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4sNV) (GLuint, GLshort, GLshort, GLshort, GLshort);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4svNV) (GLuint, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4ubNV) (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttrib4ubvNV) (GLuint, const GLubyte *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs1dvNV) (GLuint, GLsizei, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs1fvNV) (GLuint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs1svNV) (GLuint, GLsizei, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs2dvNV) (GLuint, GLsizei, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs2fvNV) (GLuint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs2svNV) (GLuint, GLsizei, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs3dvNV) (GLuint, GLsizei, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs3fvNV) (GLuint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs3svNV) (GLuint, GLsizei, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs4dvNV) (GLuint, GLsizei, const GLdouble *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs4fvNV) (GLuint, GLsizei, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs4svNV) (GLuint, GLsizei, const GLshort *);
OPENGL_EXTERN void (APIENTRYP pfglVertexAttribs4ubvNV) (GLuint, GLsizei, const GLubyte *);


/*********************************
 *
 *	ATI Extensions
 *
 *********************************/

/*
 *	------------------------------
 *	ATI_EnvmapBumpmap
 *	------------------------------
 */
OPENGL_EXTERN void (APIENTRYP pfglTexBumpParameterivATI) (GLenum, const GLint *);
OPENGL_EXTERN void (APIENTRYP pfglTexBumpParameterfvATI) (GLenum, const GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetTexBumpParameterivATI) (GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglGetTexBumpParameterfvATI) (GLenum, GLfloat *);

/*
 *	------------------------------
 *	ATI_FragmentShader
 *	------------------------------
 */
OPENGL_EXTERN GLuint (APIENTRYP pfglGenFragmentShadersATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglBindFragmentShaderATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglDeleteFragmentShaderATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglBeginFragmentShaderATI) (void);
OPENGL_EXTERN void (APIENTRYP pfglEndFragmentShaderATI) (void);
OPENGL_EXTERN void (APIENTRYP pfglPassTexCoordATI) (GLuint, GLuint, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglSampleMapATI) (GLuint, GLuint, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglColorFragmentOp1ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglColorFragmentOp2ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglColorFragmentOp3ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglAlphaFragmentOp1ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglAlphaFragmentOp2ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglAlphaFragmentOp3ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglSetFragmentShaderConstantATI) (GLuint, const GLfloat *);

/*
 *	------------------------------
 *	ATI_VertexArrayObject
 *	------------------------------
 */
OPENGL_EXTERN GLuint (APIENTRYP pfglNewObjectBufferATI) (GLsizei, const GLvoid *, GLenum);
OPENGL_EXTERN GLboolean (APIENTRYP pfglIsObjectBufferATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglUpdateObjectBufferATI) (GLuint, GLuint, GLsizei, const GLvoid *, GLenum);
OPENGL_EXTERN void (APIENTRYP pfglGetObjectBufferfvATI) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetObjectBufferivATI) (GLuint, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglFreeObjectBufferATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglArrayObjectATI) (GLenum, GLint, GLenum, GLsizei, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglGetArrayObjectfvATI) (GLenum, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetArrayObjectivATI) (GLenum, GLenum, GLint *);
OPENGL_EXTERN void (APIENTRYP pfglVariantArrayObjectATI) (GLuint, GLenum, GLsizei, GLuint, GLuint);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantArrayObjectfvATI) (GLuint, GLenum, GLfloat *);
OPENGL_EXTERN void (APIENTRYP pfglGetVariantArrayObjectivATI) (GLuint, GLenum, GLint *);

/*
 *	------------------------------
 *	ATI_MapObjectBuffer
 *	------------------------------
 */
OPENGL_EXTERN GLvoid* (APIENTRYP pfglMapObjectBufferATI) (GLuint);
OPENGL_EXTERN void (APIENTRYP pfglUnmapObjectBufferATI) (GLuint);

#endif /* __MYOPENGL_EXTENSION_H__ */

/* EOF */
