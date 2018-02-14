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


/*
 *	-----------------------
 *	ARB_MultiTexture
 *	-----------------------
 */
extern void (APIENTRYP pfglActiveTextureARB) (GLenum);
extern void (APIENTRYP pfglClientActiveTextureARB) (GLenum);
extern void (APIENTRYP pfglMultiTexCoord1dARB) (GLenum, GLdouble);
extern void (APIENTRYP pfglMultiTexCoord1dvARB) (GLenum, const GLdouble *);
extern void (APIENTRYP pfglMultiTexCoord1fARB) (GLenum, GLfloat);
extern void (APIENTRYP pfglMultiTexCoord1fvARB) (GLenum, const GLfloat *);
extern void (APIENTRYP pfglMultiTexCoord1iARB) (GLenum, GLint);
extern void (APIENTRYP pfglMultiTexCoord1ivARB) (GLenum, const GLint *);
extern void (APIENTRYP pfglMultiTexCoord1sARB) (GLenum, GLshort);
extern void (APIENTRYP pfglMultiTexCoord1svARB) (GLenum, const GLshort *);
extern void (APIENTRYP pfglMultiTexCoord2dARB) (GLenum, GLdouble, GLdouble);
extern void (APIENTRYP pfglMultiTexCoord2dvARB) (GLenum, const GLdouble *);
extern void (APIENTRYP pfglMultiTexCoord2fARB) (GLenum, GLfloat, GLfloat);
extern void (APIENTRYP pfglMultiTexCoord2fvARB) (GLenum, const GLfloat *);
extern void (APIENTRYP pfglMultiTexCoord2iARB) (GLenum, GLint, GLint);
extern void (APIENTRYP pfglMultiTexCoord2ivARB) (GLenum, const GLint *);
extern void (APIENTRYP pfglMultiTexCoord2sARB) (GLenum, GLshort, GLshort);
extern void (APIENTRYP pfglMultiTexCoord2svARB) (GLenum, const GLshort *);
extern void (APIENTRYP pfglMultiTexCoord3dARB) (GLenum, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglMultiTexCoord3dvARB) (GLenum, const GLdouble *);
extern void (APIENTRYP pfglMultiTexCoord3fARB) (GLenum, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglMultiTexCoord3fvARB) (GLenum, const GLfloat *);
extern void (APIENTRYP pfglMultiTexCoord3iARB) (GLenum, GLint, GLint, GLint);
extern void (APIENTRYP pfglMultiTexCoord3ivARB) (GLenum, const GLint *);
extern void (APIENTRYP pfglMultiTexCoord3sARB) (GLenum, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglMultiTexCoord3svARB) (GLenum, const GLshort *);
extern void (APIENTRYP pfglMultiTexCoord4dARB) (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglMultiTexCoord4dvARB) (GLenum, const GLdouble *);
extern void (APIENTRYP pfglMultiTexCoord4fARB) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglMultiTexCoord4fvARB) (GLenum, const GLfloat *);
extern void (APIENTRYP pfglMultiTexCoord4iARB) (GLenum, GLint, GLint, GLint, GLint);
extern void (APIENTRYP pfglMultiTexCoord4ivARB) (GLenum, const GLint *);
extern void (APIENTRYP pfglMultiTexCoord4sARB) (GLenum, GLshort, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglMultiTexCoord4svARB) (GLenum, const GLshort *);


/*
 *	-----------------------
 *	ARB_TransposeMatrix
 *	-----------------------
 */
extern void (APIENTRYP pfglLoadTransposeMatrixfARB) (const GLfloat *);
extern void (APIENTRYP pfglLoadTransposeMatrixdARB) (const GLdouble *);
extern void (APIENTRYP pfglMultTransposeMatrixfARB) (const GLfloat *);
extern void (APIENTRYP pfglMultTransposeMatrixdARB) (const GLdouble *);


/*
 *	-----------------------
 *	ARB_MultiSample
 *	-----------------------
 */
extern void (APIENTRYP pfglSampleCoverageARB) (GLclampf, GLboolean);


/*
 *	-----------------------
 *	ARB_TextureCompression
 *	-----------------------
 */
extern void (APIENTRYP pfglCompressedTexImage3DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglCompressedTexImage2DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglCompressedTexImage1DARB) (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglCompressedTexSubImage3DARB) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglCompressedTexSubImage2DARB) (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglCompressedTexSubImage1DARB) (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglGetCompressedTexImageARB) (GLenum, GLint, GLvoid *);


/*
 *	-----------------------
 *	ARB_PointParameters
 *	-----------------------
 */
extern void (APIENTRYP pfglPointParameterfARB) (GLenum, GLfloat);
extern void (APIENTRYP pfglPointParameterfvARB) (GLenum, const GLfloat *);


/*
 *	-----------------------
 *	ARB_VertexBlend
 *	-----------------------
 */
extern void (APIENTRYP pfglWeightbvARB) (GLint, const GLbyte *);
extern void (APIENTRYP pfglWeightsvARB) (GLint, const GLshort *);
extern void (APIENTRYP pfglWeightivARB) (GLint, const GLint *);
extern void (APIENTRYP pfglWeightfvARB) (GLint, const GLfloat *);
extern void (APIENTRYP pfglWeightdvARB) (GLint, const GLdouble *);
extern void (APIENTRYP pfglWeightubvARB) (GLint, const GLubyte *);
extern void (APIENTRYP pfglWeightusvARB) (GLint, const GLushort *);
extern void (APIENTRYP pfglWeightuivARB) (GLint, const GLuint *);
extern void (APIENTRYP pfglWeightPointerARB) (GLint, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglVertexBlendARB) (GLint);


/*
 *	-----------------------
 *	ARB_MatrixPalette
 *	-----------------------
 */
extern void (APIENTRYP pfglCurrentPaletteMatrixARB) (GLint);
extern void (APIENTRYP pfglMatrixIndexubvARB) (GLint, const GLubyte *);
extern void (APIENTRYP pfglMatrixIndexusvARB) (GLint, const GLushort *);
extern void (APIENTRYP pfglMatrixIndexuivARB) (GLint, const GLuint *);
extern void (APIENTRYP pfglMatrixIndexPointerARB) (GLint, GLenum, GLsizei, const GLvoid *);


/*
 *	------------------------------
 *	ARB_VertexProgram
 *	------------------------------
 */
extern void (APIENTRYP pfglVertexAttrib1dARB) (GLuint, GLdouble);
extern void (APIENTRYP pfglVertexAttrib1dvARB) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib1fARB) (GLuint, GLfloat);
extern void (APIENTRYP pfglVertexAttrib1fvARB) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib1sARB) (GLuint, GLshort);
extern void (APIENTRYP pfglVertexAttrib1svARB) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib2dARB) (GLuint, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib2dvARB) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib2fARB) (GLuint, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib2fvARB) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib2sARB) (GLuint, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib2svARB) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib3dARB) (GLuint, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib3dvARB) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib3fARB) (GLuint, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib3fvARB) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib3sARB) (GLuint, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib3svARB) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib4NbvARB) (GLuint, const GLbyte *);
extern void (APIENTRYP pfglVertexAttrib4NivARB) (GLuint, const GLint *);
extern void (APIENTRYP pfglVertexAttrib4NsvARB) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib4NubARB) (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
extern void (APIENTRYP pfglVertexAttrib4NubvARB) (GLuint, const GLubyte *);
extern void (APIENTRYP pfglVertexAttrib4NuivARB) (GLuint, const GLuint *);
extern void (APIENTRYP pfglVertexAttrib4NusvARB) (GLuint, const GLushort *);
extern void (APIENTRYP pfglVertexAttrib4bvARB) (GLuint, const GLbyte *);
extern void (APIENTRYP pfglVertexAttrib4dARB) (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib4dvARB) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib4fARB) (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib4fvARB) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib4ivARB) (GLuint, const GLint *);
extern void (APIENTRYP pfglVertexAttrib4sARB) (GLuint, GLshort, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib4svARB) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib4ubvARB) (GLuint, const GLubyte *);
extern void (APIENTRYP pfglVertexAttrib4uivARB) (GLuint, const GLuint *);
extern void (APIENTRYP pfglVertexAttrib4usvARB) (GLuint, const GLushort *);
extern void (APIENTRYP pfglVertexAttribPointerARB) (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglEnableVertexAttribArrayARB) (GLuint);
extern void (APIENTRYP pfglDisableVertexAttribArrayARB) (GLuint);
extern void (APIENTRYP pfglProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglBindProgramARB) (GLenum, GLuint);
extern void (APIENTRYP pfglDeleteProgramsARB) (GLsizei, const GLuint *);
extern void (APIENTRYP pfglGenProgramsARB) (GLsizei, GLuint *);
extern void (APIENTRYP pfglProgramEnvParameter4dARB) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglProgramEnvParameter4dvARB) (GLenum, GLuint, const GLdouble *);
extern void (APIENTRYP pfglProgramEnvParameter4fARB) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglProgramEnvParameter4fvARB) (GLenum, GLuint, const GLfloat *);
extern void (APIENTRYP pfglProgramLocalParameter4dARB) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglProgramLocalParameter4dvARB) (GLenum, GLuint, const GLdouble *);
extern void (APIENTRYP pfglProgramLocalParameter4fARB) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglProgramLocalParameter4fvARB) (GLenum, GLuint, const GLfloat *);
extern void (APIENTRYP pfglGetProgramEnvParameterdvARB) (GLenum, GLuint, GLdouble *);
extern void (APIENTRYP pfglGetProgramEnvParameterfvARB) (GLenum, GLuint, GLfloat *);
extern void (APIENTRYP pfglGetProgramLocalParameterdvARB) (GLenum, GLuint, GLdouble *);
extern void (APIENTRYP pfglGetProgramLocalParameterfvARB) (GLenum, GLuint, GLfloat *);
extern void (APIENTRYP pfglGetProgramivARB) (GLenum, GLenum, GLint *);
extern void (APIENTRYP pfglGetProgramStringARB) (GLenum, GLenum, GLvoid *);
extern void (APIENTRYP pfglGetVertexAttribdvARB) (GLuint, GLenum, GLdouble *);
extern void (APIENTRYP pfglGetVertexAttribfvARB) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetVertexAttribivARB) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetVertexAttribPointervARB) (GLuint, GLenum, GLvoid* *);
extern GLboolean (APIENTRYP pfglIsProgramARB) (GLuint);


/* All ARB_fragment_program entry points are shared with ARB_vertex_program. */


/*
 *	------------------------------
 *	ARB_VertexBufferObject
 *	------------------------------
 */
extern void (APIENTRYP pfglBindBufferARB) (GLenum, GLuint);
extern void (APIENTRYP pfglDeleteBuffersARB) (GLsizei, const GLuint *);
extern void (APIENTRYP pfglGenBuffersARB) (GLsizei, GLuint *);
extern GLboolean (APIENTRYP pfglIsBufferARB) (GLuint);
extern void (APIENTRYP pfglBufferDataARB) (GLenum, GLsizeiptrARB, const GLvoid *, GLenum);
extern void (APIENTRYP pfglBufferSubDataARB) (GLenum, GLintptrARB, GLsizeiptrARB, const GLvoid *);
extern void (APIENTRYP pfglGetBufferSubDataARB) (GLenum, GLintptrARB, GLsizeiptrARB, GLvoid *);
extern GLvoid* (APIENTRYP pfglMapBufferARB) (GLenum, GLenum);
extern GLboolean (APIENTRYP pfglUnmapBufferARB) (GLenum);
extern void (APIENTRYP pfglGetBufferParameterivARB) (GLenum, GLenum, GLint *);
extern void (APIENTRYP pfglGetBufferPointervARB) (GLenum, GLenum, GLvoid* *);


/*
 *	------------------------------
 *	ARB_OcclusionQuery
 *	------------------------------
 */
extern void (APIENTRYP pfglGenQueriesARB) (GLsizei, GLuint *);
extern void (APIENTRYP pfglDeleteQueriesARB) (GLsizei, const GLuint *);
extern GLboolean (APIENTRYP pfglIsQueryARB) (GLuint);
extern void (APIENTRYP pfglBeginQueryARB) (GLenum, GLuint);
extern void (APIENTRYP pfglEndQueryARB) (GLenum);
extern void (APIENTRYP pfglGetQueryivARB) (GLenum, GLenum, GLint *);
extern void (APIENTRYP pfglGetQueryObjectivARB) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetQueryObjectuivARB) (GLuint, GLenum, GLuint *);


/*
 *	------------------------------
 *	ARB_ShaderObjects
 *	------------------------------
 */
extern void (APIENTRYP pfglDeleteObjectARB) (GLhandleARB);
extern GLhandleARB (APIENTRYP pfglGetHandleARB) (GLenum);
extern void (APIENTRYP pfglDetachObjectARB) (GLhandleARB, GLhandleARB);
extern GLhandleARB (APIENTRYP pfglCreateShaderObjectARB) (GLenum);
extern void (APIENTRYP pfglShaderSourceARB) (GLhandleARB, GLsizei, const GLcharARB* *, const GLint *);
extern void (APIENTRYP pfglCompileShaderARB) (GLhandleARB);
extern GLhandleARB (APIENTRYP pfglCreateProgramObjectARB) (void);
extern void (APIENTRYP pfglAttachObjectARB) (GLhandleARB, GLhandleARB);
extern void (APIENTRYP pfglLinkProgramARB) (GLhandleARB);
extern void (APIENTRYP pfglUseProgramObjectARB) (GLhandleARB);
extern void (APIENTRYP pfglValidateProgramARB) (GLhandleARB);
extern void (APIENTRYP pfglUniform1fARB) (GLint, GLfloat);
extern void (APIENTRYP pfglUniform2fARB) (GLint, GLfloat, GLfloat);
extern void (APIENTRYP pfglUniform3fARB) (GLint, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglUniform4fARB) (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglUniform1iARB) (GLint, GLint);
extern void (APIENTRYP pfglUniform2iARB) (GLint, GLint, GLint);
extern void (APIENTRYP pfglUniform3iARB) (GLint, GLint, GLint, GLint);
extern void (APIENTRYP pfglUniform4iARB) (GLint, GLint, GLint, GLint, GLint);
extern void (APIENTRYP pfglUniform1fvARB) (GLint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglUniform2fvARB) (GLint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglUniform3fvARB) (GLint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglUniform4fvARB) (GLint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglUniform1ivARB) (GLint, GLsizei, const GLint *);
extern void (APIENTRYP pfglUniform2ivARB) (GLint, GLsizei, const GLint *);
extern void (APIENTRYP pfglUniform3ivARB) (GLint, GLsizei, const GLint *);
extern void (APIENTRYP pfglUniform4ivARB) (GLint, GLsizei, const GLint *);
extern void (APIENTRYP pfglUniformMatrix2fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
extern void (APIENTRYP pfglUniformMatrix3fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
extern void (APIENTRYP pfglUniformMatrix4fvARB) (GLint, GLsizei, GLboolean, const GLfloat *);
extern void (APIENTRYP pfglGetObjectParameterfvARB) (GLhandleARB, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetObjectParameterivARB) (GLhandleARB, GLenum, GLint *);
extern void (APIENTRYP pfglGetInfoLogARB) (GLhandleARB, GLsizei, GLsizei *, GLcharARB *);
extern void (APIENTRYP pfglGetAttachedObjectsARB) (GLhandleARB, GLsizei, GLsizei *, GLhandleARB *);
extern GLint (APIENTRYP pfglGetUniformLocationARB) (GLhandleARB, const GLcharARB *);
extern void (APIENTRYP pfglGetActiveUniformARB) (GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *);
extern void (APIENTRYP pfglGetUniformfvARB) (GLhandleARB, GLint, GLfloat *);
extern void (APIENTRYP pfglGetUniformivARB) (GLhandleARB, GLint, GLint *);
extern void (APIENTRYP pfglGetShaderSourceARB) (GLhandleARB, GLsizei, GLsizei *, GLcharARB *);


/*
 *	------------------------------
 *	ARB_VertexShader
 *	------------------------------
 */
extern void (APIENTRYP pfglBindAttribLocationARB) (GLhandleARB, GLuint, const GLcharARB *);
extern void (APIENTRYP pfglGetActiveAttribARB) (GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *);
extern GLint (APIENTRYP pfglGetAttribLocationARB) (GLhandleARB, const GLcharARB *);


/*
 *	-----------------------
 *	EXT_BlendColor
 *	-----------------------
 */
extern void (APIENTRYP pfglBlendColorEXT) (GLclampf, GLclampf, GLclampf, GLclampf);


/*
 *	-------------------------
 *	EXT_CompiledVertexArray
 *	-------------------------
 */
extern void (APIENTRYP pfglLockArraysEXT) (GLint, GLsizei);
extern void (APIENTRYP pfglUnlockArraysEXT) (void);


/*
 *	-----------------------
 *	EXT_SecondaryColor
 *	-----------------------
 */
extern void (APIENTRYP pfglSecondaryColor3bEXT) (GLbyte, GLbyte, GLbyte);
extern void (APIENTRYP pfglSecondaryColor3bvEXT) (const GLbyte *);
extern void (APIENTRYP pfglSecondaryColor3dEXT) (GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglSecondaryColor3dvEXT) (const GLdouble *);
extern void (APIENTRYP pfglSecondaryColor3fEXT) (GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglSecondaryColor3fvEXT) (const GLfloat *);
extern void (APIENTRYP pfglSecondaryColor3iEXT) (GLint, GLint, GLint);
extern void (APIENTRYP pfglSecondaryColor3ivEXT) (const GLint *);
extern void (APIENTRYP pfglSecondaryColor3sEXT) (GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglSecondaryColor3svEXT) (const GLshort *);
extern void (APIENTRYP pfglSecondaryColor3ubEXT) (GLubyte, GLubyte, GLubyte);
extern void (APIENTRYP pfglSecondaryColor3ubvEXT) (const GLubyte *);
extern void (APIENTRYP pfglSecondaryColor3uiEXT) (GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglSecondaryColor3uivEXT) (const GLuint *);
extern void (APIENTRYP pfglSecondaryColor3usEXT) (GLushort, GLushort, GLushort);
extern void (APIENTRYP pfglSecondaryColor3usvEXT) (const GLushort *);
extern void (APIENTRYP pfglSecondaryColorPointerEXT) (GLint, GLenum, GLsizei, const GLvoid *);


/*
 *	-----------------------
 *	EXT_VertexShader
 *	-----------------------
 */
extern void (APIENTRYP pfglBeginVertexShaderEXT) (void);
extern void (APIENTRYP pfglEndVertexShaderEXT) (void);
extern void (APIENTRYP pfglBindVertexShaderEXT) (GLuint);
extern GLuint (APIENTRYP pfglGenVertexShadersEXT) (GLuint);
extern void (APIENTRYP pfglDeleteVertexShaderEXT) (GLuint);
extern void (APIENTRYP pfglShaderOp1EXT) (GLenum, GLuint, GLuint);
extern void (APIENTRYP pfglShaderOp2EXT) (GLenum, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglShaderOp3EXT) (GLenum, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglSwizzleEXT) (GLuint, GLuint, GLenum, GLenum, GLenum, GLenum);
extern void (APIENTRYP pfglWriteMaskEXT) (GLuint, GLuint, GLenum, GLenum, GLenum, GLenum);
extern void (APIENTRYP pfglInsertComponentEXT) (GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglExtractComponentEXT) (GLuint, GLuint, GLuint);
extern GLuint (APIENTRYP pfglGenSymbolsEXT) (GLenum, GLenum, GLenum, GLuint);
extern void (APIENTRYP pfglSetInvariantEXT) (GLuint, GLenum, const GLvoid *);
extern void (APIENTRYP pfglSetLocalConstantEXT) (GLuint, GLenum, const GLvoid *);
extern void (APIENTRYP pfglVariantbvEXT) (GLuint, const GLbyte *);
extern void (APIENTRYP pfglVariantsvEXT) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVariantivEXT) (GLuint, const GLint *);
extern void (APIENTRYP pfglVariantfvEXT) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVariantdvEXT) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVariantubvEXT) (GLuint, const GLubyte *);
extern void (APIENTRYP pfglVariantusvEXT) (GLuint, const GLushort *);
extern void (APIENTRYP pfglVariantuivEXT) (GLuint, const GLuint *);
extern void (APIENTRYP pfglVariantPointerEXT) (GLuint, GLenum, GLuint, const GLvoid *);
extern void (APIENTRYP pfglEnableVariantClientStateEXT) (GLuint);
extern void (APIENTRYP pfglDisableVariantClientStateEXT) (GLuint);
extern GLuint (APIENTRYP pfglBindLightParameterEXT) (GLenum, GLenum);
extern GLuint (APIENTRYP pfglBindMaterialParameterEXT) (GLenum, GLenum);
extern GLuint (APIENTRYP pfglBindTexGenParameterEXT) (GLenum, GLenum, GLenum);
extern GLuint (APIENTRYP pfglBindTextureUnitParameterEXT) (GLenum, GLenum);
extern GLuint (APIENTRYP pfglBindParameterEXT) (GLenum);
extern GLboolean (APIENTRYP pfglIsVariantEnabledEXT) (GLuint, GLenum);
extern void (APIENTRYP pfglGetVariantBooleanvEXT) (GLuint, GLenum, GLboolean *);
extern void (APIENTRYP pfglGetVariantIntegervEXT) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetVariantFloatvEXT) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetVariantPointervEXT) (GLuint, GLenum, GLvoid* *);
extern void (APIENTRYP pfglGetInvariantBooleanvEXT) (GLuint, GLenum, GLboolean *);
extern void (APIENTRYP pfglGetInvariantIntegervEXT) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetInvariantFloatvEXT) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetLocalConstantBooleanvEXT) (GLuint, GLenum, GLboolean *);
extern void (APIENTRYP pfglGetLocalConstantIntegervEXT) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetLocalConstantFloatvEXT) (GLuint, GLenum, GLfloat *);


/*
 *	-----------------------
 *	EXT_VertexWeighting
 *	-----------------------
 */
extern void (APIENTRYP pfglVertexWeightfEXT) (GLfloat);
extern void (APIENTRYP pfglVertexWeightfvEXT) (const GLfloat *);
extern void (APIENTRYP pfglVertexWeightPointerEXT) (GLsizei, GLenum, GLsizei, const GLvoid *);



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
extern void (APIENTRYP pfglDeleteFencesNV) (GLsizei, const GLuint *);
extern void (APIENTRYP pfglGenFencesNV) (GLsizei, GLuint *);
extern GLboolean (APIENTRYP pfglIsFenceNV) (GLuint);
extern GLboolean (APIENTRYP pfglTestFenceNV) (GLuint);
extern void (APIENTRYP pfglGetFenceivNV) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglFinishFenceNV) (GLuint);
extern void (APIENTRYP pfglSetFenceNV) (GLuint, GLenum);


/*
 *	-----------------------
 *	NV_VertexArrayRange
 *	-----------------------
 */
extern void (APIENTRYP pfglFlushVertexArrayRangeNV) (void);
extern void (APIENTRYP pfglVertexArrayRangeNV) (GLsizei, const GLvoid *);


/*
 *	-----------------------
 *	NV_VertexProgram
 *	-----------------------
 */
extern GLboolean (APIENTRYP pfglAreProgramsResidentNV) (GLsizei, const GLuint *, GLboolean *);
extern void (APIENTRYP pfglBindProgramNV) (GLenum, GLuint);
extern void (APIENTRYP pfglDeleteProgramsNV) (GLsizei, const GLuint *);
extern void (APIENTRYP pfglExecuteProgramNV) (GLenum, GLuint, const GLfloat *);
extern void (APIENTRYP pfglGenProgramsNV) (GLsizei, GLuint *);
extern void (APIENTRYP pfglGetProgramParameterdvNV) (GLenum, GLuint, GLenum, GLdouble *);
extern void (APIENTRYP pfglGetProgramParameterfvNV) (GLenum, GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetProgramivNV) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetProgramStringNV) (GLuint, GLenum, GLubyte *);
extern void (APIENTRYP pfglGetTrackMatrixivNV) (GLenum, GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetVertexAttribdvNV) (GLuint, GLenum, GLdouble *);
extern void (APIENTRYP pfglGetVertexAttribfvNV) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetVertexAttribivNV) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglGetVertexAttribPointervNV) (GLuint, GLenum, GLvoid* *);
extern GLboolean (APIENTRYP pfglIsProgramNV) (GLuint);
extern void (APIENTRYP pfglLoadProgramNV) (GLenum, GLuint, GLsizei, const GLubyte *);
extern void (APIENTRYP pfglProgramParameter4dNV) (GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglProgramParameter4dvNV) (GLenum, GLuint, const GLdouble *);
extern void (APIENTRYP pfglProgramParameter4fNV) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglProgramParameter4fvNV) (GLenum, GLuint, const GLfloat *);
extern void (APIENTRYP pfglProgramParameters4dvNV) (GLenum, GLuint, GLuint, const GLdouble *);
extern void (APIENTRYP pfglProgramParameters4fvNV) (GLenum, GLuint, GLuint, const GLfloat *);
extern void (APIENTRYP pfglRequestResidentProgramsNV) (GLsizei, const GLuint *);
extern void (APIENTRYP pfglTrackMatrixNV) (GLenum, GLuint, GLenum, GLenum);
extern void (APIENTRYP pfglVertexAttribPointerNV) (GLuint, GLint, GLenum, GLsizei, const GLvoid *);
extern void (APIENTRYP pfglVertexAttrib1dNV) (GLuint, GLdouble);
extern void (APIENTRYP pfglVertexAttrib1dvNV) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib1fNV) (GLuint, GLfloat);
extern void (APIENTRYP pfglVertexAttrib1fvNV) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib1sNV) (GLuint, GLshort);
extern void (APIENTRYP pfglVertexAttrib1svNV) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib2dNV) (GLuint, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib2dvNV) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib2fNV) (GLuint, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib2fvNV) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib2sNV) (GLuint, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib2svNV) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib3dNV) (GLuint, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib3dvNV) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib3fNV) (GLuint, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib3fvNV) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib3sNV) (GLuint, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib3svNV) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib4dNV) (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern void (APIENTRYP pfglVertexAttrib4dvNV) (GLuint, const GLdouble *);
extern void (APIENTRYP pfglVertexAttrib4fNV) (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void (APIENTRYP pfglVertexAttrib4fvNV) (GLuint, const GLfloat *);
extern void (APIENTRYP pfglVertexAttrib4sNV) (GLuint, GLshort, GLshort, GLshort, GLshort);
extern void (APIENTRYP pfglVertexAttrib4svNV) (GLuint, const GLshort *);
extern void (APIENTRYP pfglVertexAttrib4ubNV) (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
extern void (APIENTRYP pfglVertexAttrib4ubvNV) (GLuint, const GLubyte *);
extern void (APIENTRYP pfglVertexAttribs1dvNV) (GLuint, GLsizei, const GLdouble *);
extern void (APIENTRYP pfglVertexAttribs1fvNV) (GLuint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglVertexAttribs1svNV) (GLuint, GLsizei, const GLshort *);
extern void (APIENTRYP pfglVertexAttribs2dvNV) (GLuint, GLsizei, const GLdouble *);
extern void (APIENTRYP pfglVertexAttribs2fvNV) (GLuint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglVertexAttribs2svNV) (GLuint, GLsizei, const GLshort *);
extern void (APIENTRYP pfglVertexAttribs3dvNV) (GLuint, GLsizei, const GLdouble *);
extern void (APIENTRYP pfglVertexAttribs3fvNV) (GLuint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglVertexAttribs3svNV) (GLuint, GLsizei, const GLshort *);
extern void (APIENTRYP pfglVertexAttribs4dvNV) (GLuint, GLsizei, const GLdouble *);
extern void (APIENTRYP pfglVertexAttribs4fvNV) (GLuint, GLsizei, const GLfloat *);
extern void (APIENTRYP pfglVertexAttribs4svNV) (GLuint, GLsizei, const GLshort *);
extern void (APIENTRYP pfglVertexAttribs4ubvNV) (GLuint, GLsizei, const GLubyte *);


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
extern void (APIENTRYP pfglTexBumpParameterivATI) (GLenum, const GLint *);
extern void (APIENTRYP pfglTexBumpParameterfvATI) (GLenum, const GLfloat *);
extern void (APIENTRYP pfglGetTexBumpParameterivATI) (GLenum, GLint *);
extern void (APIENTRYP pfglGetTexBumpParameterfvATI) (GLenum, GLfloat *);

/*
 *	------------------------------
 *	ATI_FragmentShader
 *	------------------------------
 */
extern GLuint (APIENTRYP pfglGenFragmentShadersATI) (GLuint);
extern void (APIENTRYP pfglBindFragmentShaderATI) (GLuint);
extern void (APIENTRYP pfglDeleteFragmentShaderATI) (GLuint);
extern void (APIENTRYP pfglBeginFragmentShaderATI) (void);
extern void (APIENTRYP pfglEndFragmentShaderATI) (void);
extern void (APIENTRYP pfglPassTexCoordATI) (GLuint, GLuint, GLenum);
extern void (APIENTRYP pfglSampleMapATI) (GLuint, GLuint, GLenum);
extern void (APIENTRYP pfglColorFragmentOp1ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglColorFragmentOp2ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglColorFragmentOp3ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglAlphaFragmentOp1ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglAlphaFragmentOp2ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglAlphaFragmentOp3ATI) (GLenum, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint);
extern void (APIENTRYP pfglSetFragmentShaderConstantATI) (GLuint, const GLfloat *);

/*
 *	------------------------------
 *	ATI_VertexArrayObject
 *	------------------------------
 */
extern GLuint (APIENTRYP pfglNewObjectBufferATI) (GLsizei, const GLvoid *, GLenum);
extern GLboolean (APIENTRYP pfglIsObjectBufferATI) (GLuint);
extern void (APIENTRYP pfglUpdateObjectBufferATI) (GLuint, GLuint, GLsizei, const GLvoid *, GLenum);
extern void (APIENTRYP pfglGetObjectBufferfvATI) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetObjectBufferivATI) (GLuint, GLenum, GLint *);
extern void (APIENTRYP pfglFreeObjectBufferATI) (GLuint);
extern void (APIENTRYP pfglArrayObjectATI) (GLenum, GLint, GLenum, GLsizei, GLuint, GLuint);
extern void (APIENTRYP pfglGetArrayObjectfvATI) (GLenum, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetArrayObjectivATI) (GLenum, GLenum, GLint *);
extern void (APIENTRYP pfglVariantArrayObjectATI) (GLuint, GLenum, GLsizei, GLuint, GLuint);
extern void (APIENTRYP pfglGetVariantArrayObjectfvATI) (GLuint, GLenum, GLfloat *);
extern void (APIENTRYP pfglGetVariantArrayObjectivATI) (GLuint, GLenum, GLint *);

/*
 *	------------------------------
 *	ATI_MapObjectBuffer
 *	------------------------------
 */
extern GLvoid* (APIENTRYP pfglMapObjectBufferATI) (GLuint);
extern void (APIENTRYP pfglUnmapObjectBufferATI) (GLuint);

#endif /* __MYOPENGL_EXTENSION_H__ */

/* EOF */
