/*
 *	Copyright (C) 2005 Michael Liebscher
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
 *	MyOpenGL.h:   OpenGL Interface
 *
 *	Author:  Michael Liebscher    <johnnycanuck@users.sourceforge.net>
 *
 */

/*
 *	Notes:
 *
 *	This module is implemented by opengl_win.c (what about on unix?)
 */

#ifndef __MYOPENGL_H__
#define __MYOPENGL_H__

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN 1
# include <windows.h>
#endif /* _WIN32 */

#ifdef __APPLE__
/* assume we are doing a build against OpenGL.framework */
# include <OpenGL/OpenGL.h> /* also includes <OpenGL/gl.h> */
/* TODO: allow building against non-framework OpenGL installations for Apple */
#else /* not __APPLE__: */
# include <GL/gl.h>
#endif /* __APPLE__ */

/* Wine's <crtdefs.h> has conflicting declarations for intptr_t, uintptr_t, and
 * wchar_t, so be sure to let <stdint.h> and <inttypes.h> that we already have
 * these types: */
#ifdef __WINE_CRTDEFS_H
# ifndef _INTPTR_T
#  define _INTPTR_T 1
# endif /* !_INTPTR_T */
# ifndef _UINTPTR_T
#  define _UINTPTR_T 1
# endif /* !_UINTPTR_T */
# ifndef _WCHAR_T
#  define _WCHAR_T 1
# endif /* !_WCHAR_T */
#endif /* __WINE_CRTDEFS_H */

#if (__unix__ || __APPLE__ || HAVE_GL_GLX_H) && !defined(_WIN32)
/* for __APPLE__ to be correct, X11 or XQuartz needs to be installed
 * (not sure if I should just rely on the HAVE_GL_GLX_H macro instead?) */
# include <GL/glx.h>
#endif /* (__unix__ || __APPLE__ || HAVE_GL_GLX_H) && !_WIN32 */


extern int	OpenGL_Init(const char *dllname);
extern void	OpenGL_Shutdown(void);


#ifndef APIENTRY
# define APIENTRY /* (nothing) */
#endif /* !APIENTRY */

#ifndef APIENTRYP
# define APIENTRYP APIENTRY *
#endif /* !APIENTRYP */


typedef GLvoid (APIENTRYP GLACCUM)(GLenum op, GLfloat value);
typedef GLvoid (APIENTRYP GLALPHAFUNC)(GLenum func, GLclampf ref);
typedef GLboolean (APIENTRYP GLARETEXTURESRESIDENT)(GLsizei n,
													const GLuint *textures,
													GLboolean *residences);
typedef GLvoid (APIENTRYP GLARRAYELEMENT)(GLint i);
typedef GLvoid (APIENTRYP GLBEGIN)(GLenum mode);
typedef GLvoid (APIENTRYP GLBINDTEXTURE)(GLenum target, GLuint texture);
typedef GLvoid (APIENTRYP GLBITMAP)(GLsizei width, GLsizei height,
									GLfloat xorig, GLfloat yorig,
									GLfloat xmove, GLfloat ymove,
									const GLubyte *bitmap);
typedef GLvoid (APIENTRYP GLBLENDFUNC)(GLenum sfactor, GLenum dfactor);
typedef GLvoid (APIENTRYP GLCALLLIST)(GLuint list);
typedef GLvoid (APIENTRYP GLCALLLISTS)(GLsizei n, GLenum type,
									   const GLvoid *lists);
typedef GLvoid (APIENTRYP GLCLEAR)(GLbitfield mask);
typedef GLvoid (APIENTRYP GLCLEARACCUM)(GLfloat red, GLfloat green,
										GLfloat blue, GLfloat alpha);
typedef GLvoid (APIENTRYP GLCLEARCOLOR)(GLclampf red, GLclampf green,
										GLclampf blue, GLclampf alpha);
typedef GLvoid (APIENTRYP GLCLEARDEPTH)(GLclampd depth);
typedef GLvoid (APIENTRYP GLCLEARINDEX)(GLfloat c);
typedef GLvoid (APIENTRYP GLCLEARSTENCIL)(GLint s);
typedef GLvoid (APIENTRYP GLCLIPPLANE)(GLenum plane,
									   const GLdouble *equation);
typedef GLvoid (APIENTRYP GLCOLOR3B)(GLbyte red, GLbyte green, GLbyte blue);
typedef GLvoid (APIENTRYP GLCOLOR3BV)(const GLbyte *v);
typedef GLvoid (APIENTRYP GLCOLOR3D)(GLdouble red, GLdouble green,
									 GLdouble blue);
typedef GLvoid (APIENTRYP GLCOLOR3DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLCOLOR3F)(GLfloat red, GLfloat green,
									 GLfloat blue);
typedef GLvoid (APIENTRYP GLCOLOR3FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLCOLOR3I)(GLint red, GLint green, GLint blue);
typedef GLvoid (APIENTRYP GLCOLOR3IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLCOLOR3S)(GLshort red, GLshort green,
									 GLshort blue);
typedef GLvoid (APIENTRYP GLCOLOR3SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLCOLOR3UB)(GLubyte red, GLubyte green,
									  GLubyte blue);
typedef GLvoid (APIENTRYP GLCOLOR3UBV)(const GLubyte *v);
typedef GLvoid (APIENTRYP GLCOLOR3UI)(GLuint red, GLuint green,
									  GLuint blue);
typedef GLvoid (APIENTRYP GLCOLOR3UIV)(const GLuint *v);
typedef GLvoid (APIENTRYP GLCOLOR3US)(GLushort red, GLushort green,
									  GLushort blue);
typedef GLvoid (APIENTRYP GLCOLOR3USV)(const GLushort *v);
typedef GLvoid (APIENTRYP GLCOLOR4B)(GLbyte red, GLbyte green, GLbyte blue,
									 GLbyte alpha);
typedef GLvoid (APIENTRYP GLCOLOR4BV)(const GLbyte *v);
typedef GLvoid (APIENTRYP GLCOLOR4D)(GLdouble red, GLdouble green,
									 GLdouble blue, GLdouble alpha);
typedef GLvoid (APIENTRYP GLCOLOR4DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLCOLOR4F)(GLfloat red, GLfloat green,
									 GLfloat blue, GLfloat alpha);
typedef GLvoid (APIENTRYP GLCOLOR4FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLCOLOR4I)(GLint red, GLint green,
									 GLint blue, GLint alpha);
typedef GLvoid (APIENTRYP GLCOLOR4IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLCOLOR4S)(GLshort red, GLshort green,
									 GLshort blue, GLshort alpha);
typedef GLvoid (APIENTRYP GLCOLOR4SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLCOLOR4UB)(GLubyte red, GLubyte green,
									  GLubyte blue, GLubyte alpha);
typedef GLvoid (APIENTRYP GLCOLOR4UBV)(const GLubyte *v);
typedef GLvoid (APIENTRYP GLCOLOR4UI)(GLuint red, GLuint green, GLuint blue,
									  GLuint alpha);
typedef GLvoid (APIENTRYP GLCOLOR4UIV)(const GLuint *v);
typedef GLvoid (APIENTRYP GLCOLOR4US)(GLushort red, GLushort green,
									  GLushort blue, GLushort alpha);
typedef GLvoid (APIENTRYP GLCOLOR4USV)(const GLushort *v);
typedef GLvoid (APIENTRYP GLCOLORMASK)(GLboolean red, GLboolean green,
									   GLboolean blue, GLboolean alpha);
typedef GLvoid (APIENTRYP GLCOLORMATERIAL)(GLenum face, GLenum mode);
typedef GLvoid (APIENTRYP GLCOLORPOINTER)(GLint size, GLenum type,
										  GLsizei stride,
										  const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLCOPYPIXELS)(GLint x, GLint y, GLsizei width,
										GLsizei height, GLenum type);
typedef GLvoid (APIENTRYP GLCOPYTEXIMAGE1D)(GLenum target, GLint level,
											GLenum internalFormat,
											GLint x, GLint y,
											GLsizei width, GLint border);
typedef GLvoid (APIENTRYP GLCOPYTEXIMAGE2D)(GLenum target, GLint level,
											GLenum internalFormat,
											GLint x, GLint y,
											GLsizei width, GLsizei height,
											GLint border);
typedef GLvoid (APIENTRYP GLCOPYTEXSUBIMAGE1D)(GLenum target, GLint level,
											   GLint xoffset, GLint x, GLint y,
											   GLsizei width);
typedef GLvoid (APIENTRYP GLCOPYTEXSUBIMAGE2D)(GLenum target, GLint level,
											   GLint xoffset, GLint yoffset,
											   GLint x, GLint y,
											   GLsizei width, GLsizei height);
typedef GLvoid (APIENTRYP GLCULLFACE)(GLenum mode);
typedef GLvoid (APIENTRYP GLDELETELISTS)(GLuint list, GLsizei range);
typedef GLvoid (APIENTRYP GLDELETETEXTURES)(GLsizei n, const GLuint *textures);
typedef GLvoid (APIENTRYP GLDEPTHFUNC)(GLenum func);
typedef GLvoid (APIENTRYP GLDEPTHMASK)(GLboolean flag);
typedef GLvoid (APIENTRYP GLDEPTHRANGE)(GLclampd zNear, GLclampd zFar);
typedef GLvoid (APIENTRYP GLDISABLE)(GLenum cap);
typedef GLvoid (APIENTRYP GLDISABLECLIENTSTATE)(GLenum array);
typedef GLvoid (APIENTRYP GLDRAWARRAYS)(GLenum mode, GLint first,
										GLsizei count);
typedef GLvoid (APIENTRYP GLDRAWBUFFER)(GLenum mode);
typedef GLvoid (APIENTRYP GLDRAWELEMENTS)(GLenum mode, GLsizei count,
										  GLenum type, const GLvoid *indices);
typedef GLvoid (APIENTRYP GLDRAWPIXELS)(GLsizei width, GLsizei height,
										GLenum format, GLenum type,
										const GLvoid *pixels);
typedef GLvoid (APIENTRYP GLEDGEFLAG)(GLboolean flag);
typedef GLvoid (APIENTRYP GLEDGEFLAGPOINTER)(GLsizei stride,
											 const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLEDGEFLAGV)(const GLboolean *flag);
typedef GLvoid (APIENTRYP GLENABLE)(GLenum cap);
typedef GLvoid (APIENTRYP GLENABLECLIENTSTATE)(GLenum array);
typedef GLvoid (APIENTRYP GLEND)(void);
typedef GLvoid (APIENTRYP GLENDLIST)(void);
typedef GLvoid (APIENTRYP GLEVALCOORD1D)(GLdouble u);
typedef GLvoid (APIENTRYP GLEVALCOORD1DV)(const GLdouble *u);
typedef GLvoid (APIENTRYP GLEVALCOORD1F)(GLfloat u);
typedef GLvoid (APIENTRYP GLEVALCOORD1FV)(const GLfloat *u);
typedef GLvoid (APIENTRYP GLEVALCOORD2D)(GLdouble u, GLdouble v);
typedef GLvoid (APIENTRYP GLEVALCOORD2DV)(const GLdouble *u);
typedef GLvoid (APIENTRYP GLEVALCOORD2F)(GLfloat u, GLfloat v);
typedef GLvoid (APIENTRYP GLEVALCOORD2FV)(const GLfloat *u);
typedef GLvoid (APIENTRYP GLEVALMESH1)(GLenum mode, GLint i1, GLint i2);
typedef GLvoid (APIENTRYP GLEVALMESH2)(GLenum mode, GLint i1, GLint i2,
									   GLint j1, GLint j2);
typedef GLvoid (APIENTRYP GLEVALPOINT1)(GLint i);
typedef GLvoid (APIENTRYP GLEVALPOINT2)(GLint i, GLint j);
typedef GLvoid (APIENTRYP GLFEEDBACKBUFFER)(GLsizei size, GLenum type,
											GLfloat *buffer);
typedef GLvoid (APIENTRYP GLFINISH)(void);
typedef GLvoid (APIENTRYP GLFLUSH)(void);
typedef GLvoid (APIENTRYP GLFOGF)(GLenum pname, GLfloat param);
typedef GLvoid (APIENTRYP GLFOGFV)(GLenum pname, const GLfloat *params);
typedef GLvoid (APIENTRYP GLFOGI)(GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLFOGIV)(GLenum pname, const GLint *params);
typedef GLvoid (APIENTRYP GLFRONTFACE)(GLenum mode);
typedef GLvoid (APIENTRYP GLFRUSTUM)(GLdouble left, GLdouble right,
									 GLdouble bottom, GLdouble top,
									 GLdouble zNear, GLdouble zFar);
typedef GLuint (APIENTRYP GLGENLISTS)(GLsizei range);
typedef GLvoid (APIENTRYP GLGENTEXTURES)(GLsizei n, GLuint *textures);
typedef GLvoid (APIENTRYP GLGETBOOLEANV)(GLenum pname, GLboolean *params);
typedef GLvoid (APIENTRYP GLGETCLIPPLANE)(GLenum plane, GLdouble *equation);
typedef GLvoid (APIENTRYP GLGETDOUBLEV)(GLenum pname, GLdouble *params);
typedef GLenum (APIENTRYP GLGETERROR )(void);
typedef GLvoid (APIENTRYP GLGETFLOATV)(GLenum pname, GLfloat *params);
typedef GLvoid (APIENTRYP GLGETINTEGERV)(GLenum pname, GLint *params);
typedef GLvoid (APIENTRYP GLGETLIGHTFV)(GLenum light, GLenum pname,
										GLfloat *params);
typedef GLvoid (APIENTRYP GLGETLIGHTIV)(GLenum light, GLenum pname,
										GLint *params);
typedef GLvoid (APIENTRYP GLGETMAPDV)(GLenum target, GLenum query, GLdouble *v);
typedef GLvoid (APIENTRYP GLGETMAPFV)(GLenum target, GLenum query, GLfloat *v);
typedef GLvoid (APIENTRYP GLGETMAPIV)(GLenum target, GLenum query, GLint *v);
typedef GLvoid (APIENTRYP GLGETMATERIALFV)(GLenum face, GLenum pname,
										   GLfloat *params);
typedef GLvoid (APIENTRYP GLGETMATERIALIV)(GLenum face, GLenum pname,
										   GLint *params);
typedef GLvoid (APIENTRYP GLGETPIXELMAPFV)(GLenum map, GLfloat *values);
typedef GLvoid (APIENTRYP GLGETPIXELMAPUIV)(GLenum map, GLuint *values);
typedef GLvoid (APIENTRYP GLGETPIXELMAPUSV)(GLenum map, GLushort *values);
typedef GLvoid (APIENTRYP GLGETPOINTERV)(GLenum pname, GLvoid* *params);
typedef GLvoid (APIENTRYP GLGETPOLYGONSTIPPLE)(GLubyte *mask);
typedef const GLubyte * (APIENTRYP GLGETSTRING)(GLenum name);
typedef GLvoid (APIENTRYP GLGETTEXENVFV)(GLenum target, GLenum pname,
										 GLfloat *params);
typedef GLvoid (APIENTRYP GLGETTEXENVIV)(GLenum target, GLenum pname,
										 GLint *params);
typedef GLvoid (APIENTRYP GLGETTEXGENDV)(GLenum coord, GLenum pname,
										 GLdouble *params);
typedef GLvoid (APIENTRYP GLGETTEXGENFV)(GLenum coord, GLenum pname,
										 GLfloat *params);
typedef GLvoid (APIENTRYP GLGETTEXGENIV)(GLenum coord, GLenum pname,
										 GLint *params);
typedef GLvoid (APIENTRYP GLGETTEXIMAGE)(GLenum target, GLint level,
										 GLenum format, GLenum type,
										 GLvoid *pixels);
typedef GLvoid (APIENTRYP GLGETTEXPARAMETERFV)(GLenum target, GLenum pname,
											   GLfloat *params);
typedef GLvoid (APIENTRYP GLGETTEXPARAMETERIV)(GLenum target, GLenum pname,
											   GLint *params);
typedef GLvoid (APIENTRYP GLHINT)(GLenum target, GLenum mode);
typedef GLvoid (APIENTRYP GLINDEXMASK)(GLuint mask);
typedef GLvoid (APIENTRYP GLINDEXPOINTER)(GLenum type, GLsizei stride,
										  const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLINDEXD)(GLdouble c);
typedef GLvoid (APIENTRYP GLINDEXDV)(const GLdouble *c);
typedef GLvoid (APIENTRYP GLINDEXF)(GLfloat c);
typedef GLvoid (APIENTRYP GLINDEXFV)(const GLfloat *c);
typedef GLvoid (APIENTRYP GLINDEXI)(GLint c);
typedef GLvoid (APIENTRYP GLINDEXIV)(const GLint *c);
typedef GLvoid (APIENTRYP GLINDEXS)(GLshort c);
typedef GLvoid (APIENTRYP GLINDEXSV)(const GLshort *c);
typedef GLvoid (APIENTRYP GLINDEXUB)(GLubyte c);
typedef GLvoid (APIENTRYP GLINDEXUBV)(const GLubyte *c);
typedef GLvoid (APIENTRYP GLINITNAMES )(void);
typedef GLvoid (APIENTRYP GLINTERLEAVEDARRAYS)(GLenum format, GLsizei stride,
											   const GLvoid *pointer);
typedef GLboolean (APIENTRYP GLISENABLED)(GLenum cap);
typedef GLboolean (APIENTRYP GLISLIST)(GLuint list);
typedef GLboolean (APIENTRYP GLISTEXTURE)(GLuint texture);
typedef GLvoid (APIENTRYP GLLIGHTMODELF)(GLenum pname, GLfloat param);
typedef GLvoid (APIENTRYP GLLIGHTMODELFV)(GLenum pname, const GLfloat *params);
typedef GLvoid (APIENTRYP GLLIGHTMODELI)(GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLLIGHTMODELIV)(GLenum pname, const GLint *params);
typedef GLvoid (APIENTRYP GLLIGHTF)(GLenum light, GLenum pname, GLfloat param);
typedef GLvoid (APIENTRYP GLLIGHTFV)(GLenum light, GLenum pname,
									 const GLfloat *params);
typedef GLvoid (APIENTRYP GLLIGHTI)(GLenum light, GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLLIGHTIV)(GLenum light, GLenum pname,
									 const GLint *params);
typedef GLvoid (APIENTRYP GLLINESTIPPLE)(GLint factor, GLushort pattern);
typedef GLvoid (APIENTRYP GLLINEWIDTH)(GLfloat width);
typedef GLvoid (APIENTRYP GLLISTBASE)(GLuint base);
typedef GLvoid (APIENTRYP GLLOADIDENTITY )(void);
typedef GLvoid (APIENTRYP GLLOADMATRIXD)(const GLdouble *m);
typedef GLvoid (APIENTRYP GLLOADMATRIXF)(const GLfloat *m);
typedef GLvoid (APIENTRYP GLLOADNAME)(GLuint name);
typedef GLvoid (APIENTRYP GLLOGICOP)(GLenum opcode);
typedef GLvoid (APIENTRYP GLMAP1D)(GLenum target, GLdouble u1, GLdouble u2,
								   GLint stride, GLint order,
								   const GLdouble *points);
typedef GLvoid (APIENTRYP GLMAP1F)(GLenum target, GLfloat u1, GLfloat u2,
								   GLint stride, GLint order,
								   const GLfloat *points);
typedef GLvoid (APIENTRYP GLMAP2D)(GLenum target, GLdouble u1, GLdouble u2,
								   GLint ustride, GLint uorder,
								   GLdouble v1, GLdouble v2,
								   GLint vstride, GLint vorder,
								   const GLdouble *points);
typedef GLvoid (APIENTRYP GLMAP2F)(GLenum target, GLfloat u1, GLfloat u2,
								   GLint ustride, GLint uorder,
								   GLfloat v1, GLfloat v2,
								   GLint vstride, GLint vorder,
								   const GLfloat *points);
typedef GLvoid (APIENTRYP GLMAPGRID1D)(GLint un, GLdouble u1, GLdouble u2);
typedef GLvoid (APIENTRYP GLMAPGRID1F)(GLint un, GLfloat u1, GLfloat u2);
typedef GLvoid (APIENTRYP GLMAPGRID2D)(GLint un, GLdouble u1, GLdouble u2,
									   GLint vn, GLdouble v1, GLdouble v2);
typedef GLvoid (APIENTRYP GLMAPGRID2F)(GLint un, GLfloat u1, GLfloat u2,
									   GLint vn, GLfloat v1, GLfloat v2);
typedef GLvoid (APIENTRYP GLMATERIALF)(GLenum face, GLenum pname,
									   GLfloat param);
typedef GLvoid (APIENTRYP GLMATERIALFV)(GLenum face, GLenum pname,
										const GLfloat *params);
typedef GLvoid (APIENTRYP GLMATERIALI)(GLenum face, GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLMATERIALIV)(GLenum face, GLenum pname,
										const GLint *params);
typedef GLvoid (APIENTRYP GLMATRIXMODE)(GLenum mode);
typedef GLvoid (APIENTRYP GLMULTMATRIXD)(const GLdouble *m);
typedef GLvoid (APIENTRYP GLMULTMATRIXF)(const GLfloat *m);
typedef GLvoid (APIENTRYP GLNEWLIST)(GLuint list, GLenum mode);
typedef GLvoid (APIENTRYP GLNORMAL3B)(GLbyte nx, GLbyte ny, GLbyte nz);
typedef GLvoid (APIENTRYP GLNORMAL3BV)(const GLbyte *v);
typedef GLvoid (APIENTRYP GLNORMAL3D)(GLdouble nx, GLdouble ny, GLdouble nz);
typedef GLvoid (APIENTRYP GLNORMAL3DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLNORMAL3F)(GLfloat nx, GLfloat ny, GLfloat nz);
typedef GLvoid (APIENTRYP GLNORMAL3FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLNORMAL3I)(GLint nx, GLint ny, GLint nz);
typedef GLvoid (APIENTRYP GLNORMAL3IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLNORMAL3S)(GLshort nx, GLshort ny, GLshort nz);
typedef GLvoid (APIENTRYP GLNORMAL3SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLNORMALPOINTER)(GLenum type, GLsizei stride,
										   const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLORTHO)(GLdouble left, GLdouble right,
								   GLdouble bottom, GLdouble top,
								   GLdouble zNear, GLdouble zFar);
typedef GLvoid (APIENTRYP GLPASSTHROUGH)(GLfloat token);
typedef GLvoid (APIENTRYP GLPIXELMAPFV)(GLenum map, GLsizei mapsize,
										const GLfloat *values);
typedef GLvoid (APIENTRYP GLPIXELMAPUIV)(GLenum map, GLsizei mapsize,
										 const GLuint *values);
typedef GLvoid (APIENTRYP GLPIXELMAPUSV)(GLenum map, GLsizei mapsize,
										 const GLushort *values);
typedef GLvoid (APIENTRYP GLPIXELSTOREF)(GLenum pname, GLfloat param);
typedef GLvoid (APIENTRYP GLPIXELSTOREI)(GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLPIXELTRANSFERF)(GLenum pname, GLfloat param);
typedef GLvoid (APIENTRYP GLPIXELTRANSFERI)(GLenum pname, GLint param);
typedef GLvoid (APIENTRYP GLPIXELZOOM)(GLfloat xfactor, GLfloat yfactor);
typedef GLvoid (APIENTRYP GLPOINTSIZE)(GLfloat size);
typedef GLvoid (APIENTRYP GLPOLYGONMODE)(GLenum face, GLenum mode);
typedef GLvoid (APIENTRYP GLPOLYGONOFFSET)(GLfloat factor, GLfloat units);
typedef GLvoid (APIENTRYP GLPOLYGONSTIPPLE)(const GLubyte *mask);
typedef GLvoid (APIENTRYP GLPOPATTRIB )(void);
typedef GLvoid (APIENTRYP GLPOPCLIENTATTRIB )(void);
typedef GLvoid (APIENTRYP GLPOPMATRIX )(void);
typedef GLvoid (APIENTRYP GLPOPNAME )(void);
typedef GLvoid (APIENTRYP GLPRIORITIZETEXTURES)(GLsizei n,
												const GLuint *textures,
												const GLclampf *priorities);
typedef GLvoid (APIENTRYP GLPUSHATTRIB)(GLbitfield mask);
typedef GLvoid (APIENTRYP GLPUSHCLIENTATTRIB)(GLbitfield mask);
typedef GLvoid (APIENTRYP GLPUSHMATRIX )(void);
typedef GLvoid (APIENTRYP GLPUSHNAME)(GLuint name);
typedef GLvoid (APIENTRYP GLRASTERPOS2D)(GLdouble x, GLdouble y);
typedef GLvoid (APIENTRYP GLRASTERPOS2DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLRASTERPOS2F)(GLfloat x, GLfloat y);
typedef GLvoid (APIENTRYP GLRASTERPOS2FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLRASTERPOS2I)(GLint x, GLint y);
typedef GLvoid (APIENTRYP GLRASTERPOS2IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLRASTERPOS2S)(GLshort x, GLshort y);
typedef GLvoid (APIENTRYP GLRASTERPOS2SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLRASTERPOS3D)(GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRYP GLRASTERPOS3DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLRASTERPOS3F)(GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRYP GLRASTERPOS3FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLRASTERPOS3I)(GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRYP GLRASTERPOS3IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLRASTERPOS3S)(GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRYP GLRASTERPOS3SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLRASTERPOS4D)(GLdouble x, GLdouble y, GLdouble z,
										 GLdouble w);
typedef GLvoid (APIENTRYP GLRASTERPOS4DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLRASTERPOS4F)(GLfloat x, GLfloat y, GLfloat z,
										 GLfloat w);
typedef GLvoid (APIENTRYP GLRASTERPOS4FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLRASTERPOS4I)(GLint x, GLint y, GLint z, GLint w);
typedef GLvoid (APIENTRYP GLRASTERPOS4IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLRASTERPOS4S)(GLshort x, GLshort y, GLshort z,
										 GLshort w);
typedef GLvoid (APIENTRYP GLRASTERPOS4SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLREADBUFFER)(GLenum mode);
typedef GLvoid (APIENTRYP GLREADPIXELS)(GLint x, GLint y,
										GLsizei width, GLsizei height,
										GLenum format, GLenum type,
										GLvoid *pixels);
typedef GLvoid (APIENTRYP GLRECTD)(GLdouble x1, GLdouble y1, GLdouble x2,
								   GLdouble y2);
typedef GLvoid (APIENTRYP GLRECTDV)(const GLdouble *v1, const GLdouble *v2);
typedef GLvoid (APIENTRYP GLRECTF)(GLfloat x1, GLfloat y1, GLfloat x2,
								   GLfloat y2);
typedef GLvoid (APIENTRYP GLRECTFV)(const GLfloat *v1, const GLfloat *v2);
typedef GLvoid (APIENTRYP GLRECTI)(GLint x1, GLint y1, GLint x2, GLint y2);
typedef GLvoid (APIENTRYP GLRECTIV)(const GLint *v1, const GLint *v2);
typedef GLvoid (APIENTRYP GLRECTS)(GLshort x1, GLshort y1, GLshort x2,
								   GLshort y2);
typedef GLvoid (APIENTRYP GLRECTSV)(const GLshort *v1, const GLshort *v2);
typedef GLint (APIENTRYP GLRENDERMODE)(GLenum mode);
typedef GLvoid (APIENTRYP GLROTATED)(GLdouble angle, GLdouble x, GLdouble y,
									 GLdouble z);
typedef GLvoid (APIENTRYP GLROTATEF)(GLfloat angle, GLfloat x, GLfloat y,
									 GLfloat z);
typedef GLvoid (APIENTRYP GLSCALED)(GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRYP GLSCALEF)(GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRYP GLSCISSOR)(GLint x, GLint y, GLsizei width,
									 GLsizei height);
typedef GLvoid (APIENTRYP GLSELECTBUFFER)(GLsizei size, GLuint *buffer);
typedef GLvoid (APIENTRYP GLSHADEMODEL)(GLenum mode);
typedef GLvoid (APIENTRYP GLSTENCILFUNC)(GLenum func, GLint ref, GLuint mask);
typedef GLvoid (APIENTRYP GLSTENCILMASK)(GLuint mask);
typedef GLvoid (APIENTRYP GLSTENCILOP)(GLenum fail, GLenum zfail, GLenum zpass);
typedef GLvoid (APIENTRYP GLTEXCOORD1D)(GLdouble s);
typedef GLvoid (APIENTRYP GLTEXCOORD1DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLTEXCOORD1F)(GLfloat s);
typedef GLvoid (APIENTRYP GLTEXCOORD1FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLTEXCOORD1I)(GLint s);
typedef GLvoid (APIENTRYP GLTEXCOORD1IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLTEXCOORD1S)(GLshort s);
typedef GLvoid (APIENTRYP GLTEXCOORD1SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLTEXCOORD2D)(GLdouble s, GLdouble t);
typedef GLvoid (APIENTRYP GLTEXCOORD2DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLTEXCOORD2F)(GLfloat s, GLfloat t);
typedef GLvoid (APIENTRYP GLTEXCOORD2FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLTEXCOORD2I)(GLint s, GLint t);
typedef GLvoid (APIENTRYP GLTEXCOORD2IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLTEXCOORD2S)(GLshort s, GLshort t);
typedef GLvoid (APIENTRYP GLTEXCOORD2SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLTEXCOORD3D)(GLdouble s, GLdouble t, GLdouble r);
typedef GLvoid (APIENTRYP GLTEXCOORD3DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLTEXCOORD3F)(GLfloat s, GLfloat t, GLfloat r);
typedef GLvoid (APIENTRYP GLTEXCOORD3FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLTEXCOORD3I)(GLint s, GLint t, GLint r);
typedef GLvoid (APIENTRYP GLTEXCOORD3IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLTEXCOORD3S)(GLshort s, GLshort t, GLshort r);
typedef GLvoid (APIENTRYP GLTEXCOORD3SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLTEXCOORD4D)(GLdouble s, GLdouble t, GLdouble r,
										GLdouble q);
typedef GLvoid (APIENTRYP GLTEXCOORD4DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLTEXCOORD4F)(GLfloat s, GLfloat t, GLfloat r,
										GLfloat q);
typedef GLvoid (APIENTRYP GLTEXCOORD4FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLTEXCOORD4I)(GLint s, GLint t, GLint r, GLint q);
typedef GLvoid (APIENTRYP GLTEXCOORD4IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLTEXCOORD4S)(GLshort s, GLshort t, GLshort r,
										GLshort q);
typedef GLvoid (APIENTRYP GLTEXCOORD4SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLTEXCOORDPOINTER)(GLint size, GLenum type,
											 GLsizei stride,
											 const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLTEXENVF)(GLenum target, GLenum pname,
									 GLfloat param);
typedef GLvoid (APIENTRYP GLTEXENVFV)(GLenum target, GLenum pname,
									  const GLfloat *params);
typedef GLvoid (APIENTRYP GLTEXENVI)(GLenum target, GLenum pname,
									 GLint param);
typedef GLvoid (APIENTRYP GLTEXENVIV)(GLenum target, GLenum pname,
									  const GLint *params);
typedef GLvoid (APIENTRYP GLTEXGEND)(GLenum coord, GLenum pname,
									 GLdouble param);
typedef GLvoid (APIENTRYP GLTEXGENDV)(GLenum coord, GLenum pname,
									  const GLdouble *params);
typedef GLvoid (APIENTRYP GLTEXGENF)(GLenum coord, GLenum pname,
									 GLfloat param);
typedef GLvoid (APIENTRYP GLTEXGENFV)(GLenum coord, GLenum pname,
									  const GLfloat *params);
typedef GLvoid (APIENTRYP GLTEXGENI)(GLenum coord, GLenum pname,
									 GLint param);
typedef GLvoid (APIENTRYP GLTEXGENIV)(GLenum coord, GLenum pname,
									  const GLint *params);
typedef GLvoid (APIENTRYP GLTEXIMAGE1D)(GLenum target, GLint level,
										GLint internalformat, GLsizei width,
										GLint border, GLenum format,
										GLenum type, const GLvoid *pixels);
typedef GLvoid (APIENTRYP GLTEXIMAGE2D)(GLenum target, GLint level,
										GLint internalformat,
										GLsizei width, GLsizei height,
										GLint border, GLenum format,
										GLenum type, const GLvoid *pixels);
typedef GLvoid (APIENTRYP GLTEXPARAMETERF)(GLenum target, GLenum pname,
										   GLfloat param);
typedef GLvoid (APIENTRYP GLTEXPARAMETERFV)(GLenum target, GLenum pname,
											const GLfloat *params);
typedef GLvoid (APIENTRYP GLTEXPARAMETERI)(GLenum target, GLenum pname,
										   GLint param);
typedef GLvoid (APIENTRYP GLTEXPARAMETERIV)(GLenum target, GLenum pname,
											const GLint *params);
typedef GLvoid (APIENTRYP GLTEXSUBIMAGE1D)(GLenum target, GLint level,
										   GLint xoffset, GLsizei width,
										   GLenum format, GLenum type,
										   const GLvoid *pixels);
typedef GLvoid (APIENTRYP GLTEXSUBIMAGE2D)(GLenum target, GLint level,
										   GLint xoffset, GLint yoffset,
										   GLsizei width, GLsizei height,
										   GLenum format, GLenum type,
										   const GLvoid *pixels);
typedef GLvoid (APIENTRYP GLTRANSLATED)(GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRYP GLTRANSLATEF)(GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRYP GLVERTEX2D)(GLdouble x, GLdouble y);
typedef GLvoid (APIENTRYP GLVERTEX2DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLVERTEX2F)(GLfloat x, GLfloat y);
typedef GLvoid (APIENTRYP GLVERTEX2FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLVERTEX2I)(GLint x, GLint y);
typedef GLvoid (APIENTRYP GLVERTEX2IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLVERTEX2S)(GLshort x, GLshort y);
typedef GLvoid (APIENTRYP GLVERTEX2SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLVERTEX3D)(GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRYP GLVERTEX3DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLVERTEX3F)(GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRYP GLVERTEX3FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLVERTEX3I)(GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRYP GLVERTEX3IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLVERTEX3S)(GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRYP GLVERTEX3SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLVERTEX4D)(GLdouble x, GLdouble y, GLdouble z,
									  GLdouble w);
typedef GLvoid (APIENTRYP GLVERTEX4DV)(const GLdouble *v);
typedef GLvoid (APIENTRYP GLVERTEX4F)(GLfloat x, GLfloat y, GLfloat z,
									  GLfloat w);
typedef GLvoid (APIENTRYP GLVERTEX4FV)(const GLfloat *v);
typedef GLvoid (APIENTRYP GLVERTEX4I)(GLint x, GLint y, GLint z, GLint w);
typedef GLvoid (APIENTRYP GLVERTEX4IV)(const GLint *v);
typedef GLvoid (APIENTRYP GLVERTEX4S)(GLshort x, GLshort y, GLshort z,
									  GLshort w);
typedef GLvoid (APIENTRYP GLVERTEX4SV)(const GLshort *v);
typedef GLvoid (APIENTRYP GLVERTEXPOINTER)(GLint size, GLenum type,
										   GLsizei stride,
										   const GLvoid *pointer);
typedef GLvoid (APIENTRYP GLVIEWPORT)(GLint x, GLint y, GLsizei width,
									  GLsizei height);

/* need to add "extern" for clang which defaults to -fno-common, but that leads
 * to missing symbol link errors: */
#ifndef OPENGL_EXTERN
# if 0
#  define OPENGL_EXTERN extern
# else
#  define OPENGL_EXTERN /* (nothing) */
# endif /* 0 */
#endif /* !OPENGL_EXTERN */
OPENGL_EXTERN GLACCUM pfglAccum;
OPENGL_EXTERN GLALPHAFUNC pfglAlphaFunc;
OPENGL_EXTERN GLARETEXTURESRESIDENT pfglAreTexturesResident;
OPENGL_EXTERN GLARRAYELEMENT pfglArrayElement;
OPENGL_EXTERN GLBEGIN pfglBegin;
OPENGL_EXTERN GLBINDTEXTURE pfglBindTexture;
OPENGL_EXTERN GLBITMAP pfglBitmap;
OPENGL_EXTERN GLBLENDFUNC pfglBlendFunc;
OPENGL_EXTERN GLCALLLIST pfglCallList;
OPENGL_EXTERN GLCALLLISTS pfglCallLists;
OPENGL_EXTERN GLCLEAR pfglClear;
OPENGL_EXTERN GLCLEARACCUM pfglClearAccum;
OPENGL_EXTERN GLCLEARCOLOR pfglClearColor;
OPENGL_EXTERN GLCLEARDEPTH pfglClearDepth;
OPENGL_EXTERN GLCLEARINDEX pfglClearIndex;
OPENGL_EXTERN GLCLEARSTENCIL pfglClearStencil;
OPENGL_EXTERN GLCLIPPLANE pfglClipPlane;
OPENGL_EXTERN GLCOLOR3B pfglColor3b;
OPENGL_EXTERN GLCOLOR3BV pfglColor3bv;
OPENGL_EXTERN GLCOLOR3D pfglColor3d;
OPENGL_EXTERN GLCOLOR3DV pfglColor3dv;
OPENGL_EXTERN GLCOLOR3F pfglColor3f;
OPENGL_EXTERN GLCOLOR3FV pfglColor3fv;
OPENGL_EXTERN GLCOLOR3I pfglColor3i;
OPENGL_EXTERN GLCOLOR3IV pfglColor3iv;
OPENGL_EXTERN GLCOLOR3S pfglColor3s;
OPENGL_EXTERN GLCOLOR3SV pfglColor3sv;
OPENGL_EXTERN GLCOLOR3UB pfglColor3ub;
OPENGL_EXTERN GLCOLOR3UBV pfglColor3ubv;
OPENGL_EXTERN GLCOLOR3UI pfglColor3ui;
OPENGL_EXTERN GLCOLOR3UIV pfglColor3uiv;
OPENGL_EXTERN GLCOLOR3US pfglColor3us;
OPENGL_EXTERN GLCOLOR3USV pfglColor3usv;
OPENGL_EXTERN GLCOLOR4B pfglColor4b;
OPENGL_EXTERN GLCOLOR4BV pfglColor4bv;
OPENGL_EXTERN GLCOLOR4D pfglColor4d;
OPENGL_EXTERN GLCOLOR4DV pfglColor4dv;
OPENGL_EXTERN GLCOLOR4F pfglColor4f;
OPENGL_EXTERN GLCOLOR4FV pfglColor4fv;
OPENGL_EXTERN GLCOLOR4I pfglColor4i;
OPENGL_EXTERN GLCOLOR4IV pfglColor4iv;
OPENGL_EXTERN GLCOLOR4S pfglColor4s;
OPENGL_EXTERN GLCOLOR4SV pfglColor4sv;
OPENGL_EXTERN GLCOLOR4UB pfglColor4ub;
OPENGL_EXTERN GLCOLOR4UBV pfglColor4ubv;
OPENGL_EXTERN GLCOLOR4UI pfglColor4ui;
OPENGL_EXTERN GLCOLOR4UIV pfglColor4uiv;
OPENGL_EXTERN GLCOLOR4US pfglColor4us;
OPENGL_EXTERN GLCOLOR4USV pfglColor4usv;
OPENGL_EXTERN GLCOLORMASK pfglColorMask;
OPENGL_EXTERN GLCOLORMATERIAL pfglColorMaterial;
OPENGL_EXTERN GLCOLORPOINTER pfglColorPointer;
OPENGL_EXTERN GLCOPYPIXELS pfglCopyPixels;
OPENGL_EXTERN GLCOPYTEXIMAGE1D pfglCopyTexImage1D;
OPENGL_EXTERN GLCOPYTEXIMAGE2D pfglCopyTexImage2D;
OPENGL_EXTERN GLCOPYTEXSUBIMAGE1D pfglCopyTexSubImage1D;
OPENGL_EXTERN GLCOPYTEXSUBIMAGE2D pfglCopyTexSubImage2D;
OPENGL_EXTERN GLCULLFACE pfglCullFace;
OPENGL_EXTERN GLDELETELISTS pfglDeleteLists;
OPENGL_EXTERN GLDELETETEXTURES pfglDeleteTextures;
OPENGL_EXTERN GLDEPTHFUNC pfglDepthFunc;
OPENGL_EXTERN GLDEPTHMASK pfglDepthMask;
OPENGL_EXTERN GLDEPTHRANGE pfglDepthRange;
OPENGL_EXTERN GLDISABLE pfglDisable;
OPENGL_EXTERN GLDISABLECLIENTSTATE pfglDisableClientState;
OPENGL_EXTERN GLDRAWARRAYS pfglDrawArrays;
OPENGL_EXTERN GLDRAWBUFFER pfglDrawBuffer;
OPENGL_EXTERN GLDRAWELEMENTS pfglDrawElements;
OPENGL_EXTERN GLDRAWPIXELS pfglDrawPixels;
OPENGL_EXTERN GLEDGEFLAG pfglEdgeFlag;
OPENGL_EXTERN GLEDGEFLAGPOINTER pfglEdgeFlagPointer;
OPENGL_EXTERN GLEDGEFLAGV pfglEdgeFlagv;
OPENGL_EXTERN GLENABLE pfglEnable;
OPENGL_EXTERN GLENABLECLIENTSTATE pfglEnableClientState;
OPENGL_EXTERN GLEND pfglEnd;
OPENGL_EXTERN GLENDLIST pfglEndList;
OPENGL_EXTERN GLEVALCOORD1D pfglEvalCoord1d;
OPENGL_EXTERN GLEVALCOORD1DV pfglEvalCoord1dv;
OPENGL_EXTERN GLEVALCOORD1F pfglEvalCoord1f;
OPENGL_EXTERN GLEVALCOORD1FV pfglEvalCoord1fv;
OPENGL_EXTERN GLEVALCOORD2D pfglEvalCoord2d;
OPENGL_EXTERN GLEVALCOORD2DV pfglEvalCoord2dv;
OPENGL_EXTERN GLEVALCOORD2F pfglEvalCoord2f;
OPENGL_EXTERN GLEVALCOORD2FV pfglEvalCoord2fv;
OPENGL_EXTERN GLEVALMESH1 pfglEvalMesh1;
OPENGL_EXTERN GLEVALMESH2 pfglEvalMesh2;
OPENGL_EXTERN GLEVALPOINT1 pfglEvalPoint1;
OPENGL_EXTERN GLEVALPOINT2 pfglEvalPoint2;
OPENGL_EXTERN GLFEEDBACKBUFFER pfglFeedbackBuffer;
OPENGL_EXTERN GLFINISH pfglFinish;
OPENGL_EXTERN GLFLUSH pfglFlush;
OPENGL_EXTERN GLFOGF pfglFogf;
OPENGL_EXTERN GLFOGFV pfglFogfv;
OPENGL_EXTERN GLFOGI pfglFogi;
OPENGL_EXTERN GLFOGIV pfglFogiv;
OPENGL_EXTERN GLFRONTFACE pfglFrontFace;
OPENGL_EXTERN GLFRUSTUM pfglFrustum;
OPENGL_EXTERN GLGENLISTS pfglGenLists;
OPENGL_EXTERN GLGENTEXTURES pfglGenTextures;
OPENGL_EXTERN GLGETBOOLEANV pfglGetBooleanv;
OPENGL_EXTERN GLGETCLIPPLANE pfglGetClipPlane;
OPENGL_EXTERN GLGETDOUBLEV pfglGetDoublev;
OPENGL_EXTERN GLGETERROR pfglGetError;
OPENGL_EXTERN GLGETFLOATV pfglGetFloatv;
OPENGL_EXTERN GLGETINTEGERV pfglGetIntegerv;
OPENGL_EXTERN GLGETLIGHTFV pfglGetLightfv;
OPENGL_EXTERN GLGETLIGHTIV pfglGetLightiv;
OPENGL_EXTERN GLGETMAPDV pfglGetMapdv;
OPENGL_EXTERN GLGETMAPFV pfglGetMapfv;
OPENGL_EXTERN GLGETMAPIV pfglGetMapiv;
OPENGL_EXTERN GLGETMATERIALFV pfglGetMaterialfv;
OPENGL_EXTERN GLGETMATERIALIV pfglGetMaterialiv;
OPENGL_EXTERN GLGETPIXELMAPFV pfglGetPixelMapfv;
OPENGL_EXTERN GLGETPIXELMAPUIV pfglGetPixelMapuiv;
OPENGL_EXTERN GLGETPIXELMAPUSV pfglGetPixelMapusv;
OPENGL_EXTERN GLGETPOINTERV pfglGetPointerv;
OPENGL_EXTERN GLGETPOLYGONSTIPPLE pfglGetPolygonStipple;
OPENGL_EXTERN GLGETSTRING pfglGetString;
OPENGL_EXTERN GLGETTEXENVFV pfglGetTexEnvfv;
OPENGL_EXTERN GLGETTEXENVIV pfglGetTexEnviv;
OPENGL_EXTERN GLGETTEXGENDV pfglGetTexGendv;
OPENGL_EXTERN GLGETTEXGENFV pfglGetTexGenfv;
OPENGL_EXTERN GLGETTEXGENIV pfglGetTexGeniv;
OPENGL_EXTERN GLGETTEXIMAGE pfglGetTexImage;
OPENGL_EXTERN GLGETTEXPARAMETERFV pfglGetTexParameterfv;
OPENGL_EXTERN GLGETTEXPARAMETERIV pfglGetTexParameteriv;
OPENGL_EXTERN GLHINT pfglHint;
OPENGL_EXTERN GLINDEXMASK pfglIndexMask;
OPENGL_EXTERN GLINDEXPOINTER pfglIndexPointer;
OPENGL_EXTERN GLINDEXD pfglIndexd;
OPENGL_EXTERN GLINDEXDV pfglIndexdv;
OPENGL_EXTERN GLINDEXF pfglIndexf;
OPENGL_EXTERN GLINDEXFV pfglIndexfv;
OPENGL_EXTERN GLINDEXI pfglIndexi;
OPENGL_EXTERN GLINDEXIV pfglIndexiv;
OPENGL_EXTERN GLINDEXS pfglIndexs;
OPENGL_EXTERN GLINDEXSV pfglIndexsv;
OPENGL_EXTERN GLINDEXUB pfglIndexub;
OPENGL_EXTERN GLINDEXUBV pfglIndexubv;
OPENGL_EXTERN GLINITNAMES pfglInitNames;
OPENGL_EXTERN GLINTERLEAVEDARRAYS pfglInterleavedArrays;
OPENGL_EXTERN GLISENABLED pfglIsEnabled;
OPENGL_EXTERN GLISLIST pfglIsList;
OPENGL_EXTERN GLISTEXTURE pfglIsTexture;
OPENGL_EXTERN GLLIGHTMODELF pfglLightModelf;
OPENGL_EXTERN GLLIGHTMODELFV pfglLightModelfv;
OPENGL_EXTERN GLLIGHTMODELI pfglLightModeli;
OPENGL_EXTERN GLLIGHTMODELIV pfglLightModeliv;
OPENGL_EXTERN GLLIGHTF pfglLightf;
OPENGL_EXTERN GLLIGHTFV pfglLightfv;
OPENGL_EXTERN GLLIGHTI pfglLighti;
OPENGL_EXTERN GLLIGHTIV pfglLightiv;
OPENGL_EXTERN GLLINESTIPPLE pfglLineStipple;
OPENGL_EXTERN GLLINEWIDTH pfglLineWidth;
OPENGL_EXTERN GLLISTBASE pfglListBase;
OPENGL_EXTERN GLLOADIDENTITY pfglLoadIdentity;
OPENGL_EXTERN GLLOADMATRIXD pfglLoadMatrixd;
OPENGL_EXTERN GLLOADMATRIXF pfglLoadMatrixf;
OPENGL_EXTERN GLLOADNAME pfglLoadName;
OPENGL_EXTERN GLLOGICOP pfglLogicOp;
OPENGL_EXTERN GLMAP1D pfglMap1d;
OPENGL_EXTERN GLMAP1F pfglMap1f;
OPENGL_EXTERN GLMAP2D pfglMap2d;
OPENGL_EXTERN GLMAP2F pfglMap2f;
OPENGL_EXTERN GLMAPGRID1D pfglMapGrid1d;
OPENGL_EXTERN GLMAPGRID1F pfglMapGrid1f;
OPENGL_EXTERN GLMAPGRID2D pfglMapGrid2d;
OPENGL_EXTERN GLMAPGRID2F pfglMapGrid2f;
OPENGL_EXTERN GLMATERIALF pfglMaterialf;
OPENGL_EXTERN GLMATERIALFV pfglMaterialfv;
OPENGL_EXTERN GLMATERIALI pfglMateriali;
OPENGL_EXTERN GLMATERIALIV pfglMaterialiv;
OPENGL_EXTERN GLMATRIXMODE pfglMatrixMode;
OPENGL_EXTERN GLMULTMATRIXD pfglMultMatrixd;
OPENGL_EXTERN GLMULTMATRIXF pfglMultMatrixf;
OPENGL_EXTERN GLNEWLIST pfglNewList;
OPENGL_EXTERN GLNORMAL3B pfglNormal3b;
OPENGL_EXTERN GLNORMAL3BV pfglNormal3bv;
OPENGL_EXTERN GLNORMAL3D pfglNormal3d;
OPENGL_EXTERN GLNORMAL3DV pfglNormal3dv;
OPENGL_EXTERN GLNORMAL3F pfglNormal3f;
OPENGL_EXTERN GLNORMAL3FV pfglNormal3fv;
OPENGL_EXTERN GLNORMAL3I pfglNormal3i;
OPENGL_EXTERN GLNORMAL3IV pfglNormal3iv;
OPENGL_EXTERN GLNORMAL3S pfglNormal3s;
OPENGL_EXTERN GLNORMAL3SV pfglNormal3sv;
OPENGL_EXTERN GLNORMALPOINTER pfglNormalPointer;
OPENGL_EXTERN GLORTHO pfglOrtho;
OPENGL_EXTERN GLPASSTHROUGH pfglPassThrough;
OPENGL_EXTERN GLPIXELMAPFV pfglPixelMapfv;
OPENGL_EXTERN GLPIXELMAPUIV pfglPixelMapuiv;
OPENGL_EXTERN GLPIXELMAPUSV pfglPixelMapusv;
OPENGL_EXTERN GLPIXELSTOREF pfglPixelStoref;
OPENGL_EXTERN GLPIXELSTOREI pfglPixelStorei;
OPENGL_EXTERN GLPIXELTRANSFERF pfglPixelTransferf;
OPENGL_EXTERN GLPIXELTRANSFERI pfglPixelTransferi;
OPENGL_EXTERN GLPIXELZOOM pfglPixelZoom;
OPENGL_EXTERN GLPOINTSIZE pfglPointSize;
OPENGL_EXTERN GLPOLYGONMODE pfglPolygonMode;
OPENGL_EXTERN GLPOLYGONOFFSET pfglPolygonOffset;
OPENGL_EXTERN GLPOLYGONSTIPPLE pfglPolygonStipple;
OPENGL_EXTERN GLPOPATTRIB pfglPopAttrib;
OPENGL_EXTERN GLPOPCLIENTATTRIB pfglPopClientAttrib;
OPENGL_EXTERN GLPOPMATRIX pfglPopMatrix;
OPENGL_EXTERN GLPOPNAME pfglPopName;
OPENGL_EXTERN GLPRIORITIZETEXTURES pfglPrioritizeTextures;
OPENGL_EXTERN GLPUSHATTRIB pfglPushAttrib;
OPENGL_EXTERN GLPUSHCLIENTATTRIB pfglPushClientAttrib;
OPENGL_EXTERN GLPUSHMATRIX pfglPushMatrix;
OPENGL_EXTERN GLPUSHNAME pfglPushName;
OPENGL_EXTERN GLRASTERPOS2D pfglRasterPos2d;
OPENGL_EXTERN GLRASTERPOS2DV pfglRasterPos2dv;
OPENGL_EXTERN GLRASTERPOS2F pfglRasterPos2f;
OPENGL_EXTERN GLRASTERPOS2FV pfglRasterPos2fv;
OPENGL_EXTERN GLRASTERPOS2I pfglRasterPos2i;
OPENGL_EXTERN GLRASTERPOS2IV pfglRasterPos2iv;
OPENGL_EXTERN GLRASTERPOS2S pfglRasterPos2s;
OPENGL_EXTERN GLRASTERPOS2SV pfglRasterPos2sv;
OPENGL_EXTERN GLRASTERPOS3D pfglRasterPos3d;
OPENGL_EXTERN GLRASTERPOS3DV pfglRasterPos3dv;
OPENGL_EXTERN GLRASTERPOS3F pfglRasterPos3f;
OPENGL_EXTERN GLRASTERPOS3FV pfglRasterPos3fv;
OPENGL_EXTERN GLRASTERPOS3I pfglRasterPos3i;
OPENGL_EXTERN GLRASTERPOS3IV pfglRasterPos3iv;
OPENGL_EXTERN GLRASTERPOS3S pfglRasterPos3s;
OPENGL_EXTERN GLRASTERPOS3SV pfglRasterPos3sv;
OPENGL_EXTERN GLRASTERPOS4D pfglRasterPos4d;
OPENGL_EXTERN GLRASTERPOS4DV pfglRasterPos4dv;
OPENGL_EXTERN GLRASTERPOS4F pfglRasterPos4f;
OPENGL_EXTERN GLRASTERPOS4FV pfglRasterPos4fv;
OPENGL_EXTERN GLRASTERPOS4I pfglRasterPos4i;
OPENGL_EXTERN GLRASTERPOS4IV pfglRasterPos4iv;
OPENGL_EXTERN GLRASTERPOS4S pfglRasterPos4s;
OPENGL_EXTERN GLRASTERPOS4SV pfglRasterPos4sv;
OPENGL_EXTERN GLREADBUFFER pfglReadBuffer;
OPENGL_EXTERN GLREADPIXELS pfglReadPixels;
OPENGL_EXTERN GLRECTD pfglRectd;
OPENGL_EXTERN GLRECTDV pfglRectdv;
OPENGL_EXTERN GLRECTF pfglRectf;
OPENGL_EXTERN GLRECTFV pfglRectfv;
OPENGL_EXTERN GLRECTI pfglRecti;
OPENGL_EXTERN GLRECTIV pfglRectiv;
OPENGL_EXTERN GLRECTS pfglRects;
OPENGL_EXTERN GLRECTSV pfglRectsv;
OPENGL_EXTERN GLRENDERMODE pfglRenderMode;
OPENGL_EXTERN GLROTATED pfglRotated;
OPENGL_EXTERN GLROTATEF pfglRotatef;
OPENGL_EXTERN GLSCALED pfglScaled;
OPENGL_EXTERN GLSCALEF pfglScalef;
OPENGL_EXTERN GLSCISSOR pfglScissor;
OPENGL_EXTERN GLSELECTBUFFER pfglSelectBuffer;
OPENGL_EXTERN GLSHADEMODEL pfglShadeModel;
OPENGL_EXTERN GLSTENCILFUNC pfglStencilFunc;
OPENGL_EXTERN GLSTENCILMASK pfglStencilMask;
OPENGL_EXTERN GLSTENCILOP pfglStencilOp;
OPENGL_EXTERN GLTEXCOORD1D pfglTexCoord1d;
OPENGL_EXTERN GLTEXCOORD1DV pfglTexCoord1dv;
OPENGL_EXTERN GLTEXCOORD1F pfglTexCoord1f;
OPENGL_EXTERN GLTEXCOORD1FV pfglTexCoord1fv;
OPENGL_EXTERN GLTEXCOORD1I pfglTexCoord1i;
OPENGL_EXTERN GLTEXCOORD1IV pfglTexCoord1iv;
OPENGL_EXTERN GLTEXCOORD1S pfglTexCoord1s;
OPENGL_EXTERN GLTEXCOORD1SV pfglTexCoord1sv;
OPENGL_EXTERN GLTEXCOORD2D pfglTexCoord2d;
OPENGL_EXTERN GLTEXCOORD2DV pfglTexCoord2dv;
OPENGL_EXTERN GLTEXCOORD2F pfglTexCoord2f;
OPENGL_EXTERN GLTEXCOORD2FV pfglTexCoord2fv;
OPENGL_EXTERN GLTEXCOORD2I pfglTexCoord2i;
OPENGL_EXTERN GLTEXCOORD2IV pfglTexCoord2iv;
OPENGL_EXTERN GLTEXCOORD2S pfglTexCoord2s;
OPENGL_EXTERN GLTEXCOORD2SV pfglTexCoord2sv;
OPENGL_EXTERN GLTEXCOORD3D pfglTexCoord3d;
OPENGL_EXTERN GLTEXCOORD3DV pfglTexCoord3dv;
OPENGL_EXTERN GLTEXCOORD3F pfglTexCoord3f;
OPENGL_EXTERN GLTEXCOORD3FV pfglTexCoord3fv;
OPENGL_EXTERN GLTEXCOORD3I pfglTexCoord3i;
OPENGL_EXTERN GLTEXCOORD3IV pfglTexCoord3iv;
OPENGL_EXTERN GLTEXCOORD3S pfglTexCoord3s;
OPENGL_EXTERN GLTEXCOORD3SV pfglTexCoord3sv;
OPENGL_EXTERN GLTEXCOORD4D pfglTexCoord4d;
OPENGL_EXTERN GLTEXCOORD4DV pfglTexCoord4dv;
OPENGL_EXTERN GLTEXCOORD4F pfglTexCoord4f;
OPENGL_EXTERN GLTEXCOORD4FV pfglTexCoord4fv;
OPENGL_EXTERN GLTEXCOORD4I pfglTexCoord4i;
OPENGL_EXTERN GLTEXCOORD4IV pfglTexCoord4iv;
OPENGL_EXTERN GLTEXCOORD4S pfglTexCoord4s;
OPENGL_EXTERN GLTEXCOORD4SV pfglTexCoord4sv;
OPENGL_EXTERN GLTEXCOORDPOINTER pfglTexCoordPointer;
OPENGL_EXTERN GLTEXENVF pfglTexEnvf;
OPENGL_EXTERN GLTEXENVFV pfglTexEnvfv;
OPENGL_EXTERN GLTEXENVI pfglTexEnvi;
OPENGL_EXTERN GLTEXENVIV pfglTexEnviv;
OPENGL_EXTERN GLTEXGEND pfglTexGend;
OPENGL_EXTERN GLTEXGENDV pfglTexGendv;
OPENGL_EXTERN GLTEXGENF pfglTexGenf;
OPENGL_EXTERN GLTEXGENFV pfglTexGenfv;
OPENGL_EXTERN GLTEXGENI pfglTexGeni;
OPENGL_EXTERN GLTEXGENIV pfglTexGeniv;
OPENGL_EXTERN GLTEXIMAGE1D pfglTexImage1D;
OPENGL_EXTERN GLTEXIMAGE2D pfglTexImage2D;
OPENGL_EXTERN GLTEXPARAMETERF pfglTexParameterf;
OPENGL_EXTERN GLTEXPARAMETERFV pfglTexParameterfv;
OPENGL_EXTERN GLTEXPARAMETERI pfglTexParameteri;
OPENGL_EXTERN GLTEXPARAMETERIV pfglTexParameteriv;
OPENGL_EXTERN GLTEXSUBIMAGE1D pfglTexSubImage1D;
OPENGL_EXTERN GLTEXSUBIMAGE2D pfglTexSubImage2D;
OPENGL_EXTERN GLTRANSLATED pfglTranslated;
OPENGL_EXTERN GLTRANSLATEF pfglTranslatef;
OPENGL_EXTERN GLVERTEX2D pfglVertex2d;
OPENGL_EXTERN GLVERTEX2DV pfglVertex2dv;
OPENGL_EXTERN GLVERTEX2F pfglVertex2f;
OPENGL_EXTERN GLVERTEX2FV pfglVertex2fv;
OPENGL_EXTERN GLVERTEX2I pfglVertex2i;
OPENGL_EXTERN GLVERTEX2IV pfglVertex2iv;
OPENGL_EXTERN GLVERTEX2S pfglVertex2s;
OPENGL_EXTERN GLVERTEX2SV pfglVertex2sv;
OPENGL_EXTERN GLVERTEX3D pfglVertex3d;
OPENGL_EXTERN GLVERTEX3DV pfglVertex3dv;
OPENGL_EXTERN GLVERTEX3F pfglVertex3f;
OPENGL_EXTERN GLVERTEX3FV pfglVertex3fv;
OPENGL_EXTERN GLVERTEX3I pfglVertex3i;
OPENGL_EXTERN GLVERTEX3IV pfglVertex3iv;
OPENGL_EXTERN GLVERTEX3S pfglVertex3s;
OPENGL_EXTERN GLVERTEX3SV pfglVertex3sv;
OPENGL_EXTERN GLVERTEX4D pfglVertex4d;
OPENGL_EXTERN GLVERTEX4DV pfglVertex4dv;
OPENGL_EXTERN GLVERTEX4F pfglVertex4f;
OPENGL_EXTERN GLVERTEX4FV pfglVertex4fv;
OPENGL_EXTERN GLVERTEX4I pfglVertex4i;
OPENGL_EXTERN GLVERTEX4IV pfglVertex4iv;
OPENGL_EXTERN GLVERTEX4S pfglVertex4s;
OPENGL_EXTERN GLVERTEX4SV pfglVertex4sv;
OPENGL_EXTERN GLVERTEXPOINTER pfglVertexPointer;
OPENGL_EXTERN GLVIEWPORT pfglViewport;


#ifdef _WIN32

typedef BOOL (WINAPI *WGLCOPYCONTEXT)(HGLRC, HGLRC, UINT);
typedef HGLRC (WINAPI *WGLCREATECONTEXT)(HDC);
typedef HGLRC (WINAPI *WGLCREATELAYERCONTEXT)(HDC, int);
typedef BOOL (WINAPI *WGLDELETECONTEXT)(HGLRC);
typedef BOOL (WINAPI *WGLDESCRIBELAYERPLANE)(HDC, int, int, UINT,
											 LPLAYERPLANEDESCRIPTOR);
typedef HGLRC (WINAPI *WGLGETCURRENTCONTEXT)(VOID);
typedef HDC (WINAPI *WGLGETCURRENTDC)(VOID);
typedef int (WINAPI *WGLGETLAYERPALETTEENTRIES)(HDC, int, int, int, COLORREF *);
typedef PROC (WINAPI *WGLGETPROCADDRESS)(LPCSTR);
typedef BOOL (WINAPI *WGLMAKECURRENT)(HDC, HGLRC);
typedef BOOL (WINAPI *WGLREALIZELAYERPALETTE)(HDC, int, BOOL);
typedef int (WINAPI *WGLSETLAYERPALETTEENTRIES)(HDC, int, int, int,
												CONST COLORREF *);
typedef BOOL (WINAPI *WGLSHARELISTS)(HGLRC, HGLRC);
typedef BOOL (WINAPI *WGLSWAPLAYERBUFFERS)(HDC, UINT);
typedef BOOL (WINAPI *WGLUSEFONTBITMAPS)(HDC, DWORD, DWORD, DWORD);
typedef BOOL (WINAPI *WGLUSEFONTOUTLINES)(HDC, DWORD, DWORD, DWORD, FLOAT,
										   FLOAT, int, LPGLYPHMETRICSFLOAT);


WGLCOPYCONTEXT pfwglCopyContext;
WGLCREATECONTEXT pfwglCreateContext;
WGLCREATELAYERCONTEXT pfwglCreateLayerContext;
WGLDELETECONTEXT pfwglDeleteContext;
WGLDESCRIBELAYERPLANE pfwglDescribeLayerPlane;
WGLGETCURRENTCONTEXT pfwglGetCurrentContext;
WGLGETCURRENTDC pfwglGetCurrentDC;
WGLGETLAYERPALETTEENTRIES pfwglGetLayerPaletteEntries;
WGLGETPROCADDRESS pfwglGetProcAddress;
WGLMAKECURRENT pfwglMakeCurrent;
WGLREALIZELAYERPALETTE pfwglRealizeLayerPalette;
WGLSETLAYERPALETTEENTRIES pfwglSetLayerPaletteEntries;
WGLSHARELISTS pfwglShareLists;
WGLSWAPLAYERBUFFERS pfwglSwapLayerBuffers;
WGLUSEFONTBITMAPS pfwglUseFontBitmaps;
WGLUSEFONTOUTLINES pfwglUseFontOutlines;


typedef int (WINAPI *WGLCHOOSEPIXELFORMAT)(HDC, CONST PIXELFORMATDESCRIPTOR *);
typedef int (WINAPI *WGLDESCRIBEPIXELFORMAT) (HDC, int, UINT,
											  LPPIXELFORMATDESCRIPTOR);
typedef int (WINAPI *WGLGETPIXELFORMAT)(HDC);
typedef BOOL (WINAPI *WGLSETPIXELFORMAT)(HDC, int,
										 CONST PIXELFORMATDESCRIPTOR *);
typedef BOOL (WINAPI *WGLSWAPBUFFERS)(HDC);


WGLCHOOSEPIXELFORMAT pfwglChoosePixelFormat;
WGLDESCRIBEPIXELFORMAT pfwglDescribePixelFormat;
WGLGETPIXELFORMAT pfwglGetPixelFormat;
WGLSETPIXELFORMAT pfwglSetPixelFormat;
WGLSWAPBUFFERS pfwglSwapBuffers;


typedef BOOL (WINAPI *WGLSWAPINTERVALEXT)(int);
typedef int (WINAPI *WGLGETSWAPINTERVALEXT)(void);

WGLSWAPINTERVALEXT pfwglSwapIntervalEXT;
WGLGETSWAPINTERVALEXT pfwglGetSwapIntervalEXT;


extern void *(WINAPI *pfwglAllocateMemoryNV)(GLsizei size,
											 GLfloat readFrequency,
											 GLfloat writeFrequency,
											 GLfloat priority);
extern void *(WINAPI *pfwglFreeMemoryNV)(void *pointer);


#ifndef HPBUFFERARB
DECLARE_HANDLE(HPBUFFERARB);
#endif /* !HPBUFFERARB */

HPBUFFERARB (WINAPI *pfwglCreatePbufferARB)(HDC hDC, int iPixelFormat,
											int iWidth, int iHeight,
											const int *piAttribList);
HDC (WINAPI *pfwglGetPbufferDCARB)(HPBUFFERARB hPbuffer);
int (WINAPI *pfwglReleasePbufferDCARB)(HPBUFFERARB hPbuffer, HDC hDC);
BOOL (WINAPI *pfwglDestroyPbufferARB)(HPBUFFERARB hPbuffer);
BOOL (WINAPI *pfwglQueryPbufferARB)(HPBUFFERARB hPbuffer, int iAttribute,
									int *piValue);


BOOL (WINAPI *pfwglGetPixelFormatAttribivARB)(HDC hdc, int iPixelFormat,
											  int iLayerPlane, UINT nAttributes,
											  const int *piAttributes,
											  int *piValues);
BOOL (WINAPI *pfwglGetPixelFormatAttribfvARB)(HDC hdc, int iPixelFormat,
											  int iLayerPlane, UINT nAttributes,
											  const int *piAttributes,
											  FLOAT *pfValues);
BOOL (WINAPI *pfwglChoosePixelFormatARB)(HDC hdc, const int *piAttribIList,
										 const FLOAT *pfAttribFList,
										 UINT nMaxFormats, int *piFormats,
										 UINT *nNumFormats);


#endif /* _WIN32 */

#if (defined(__unix__) || defined(__APPLE__)) && !defined(_WIN32)
/* for "__APPLE__" to be correct, X11 or XQuartz needs to be installed
 * (see above) */

/*	Local function in dll */
OPENGL_EXTERN void *pfwglGetProcAddress(const char *symbol);


OPENGL_EXTERN void *(*pfwglAllocateMemoryNV)(GLsizei size, GLfloat readFrequency,
									  GLfloat writeFrequency, GLfloat priority);
OPENGL_EXTERN void *(*pfwglFreeMemoryNV)(void *pointer);


/*	GLX Functions */
OPENGL_EXTERN XVisualInfo * (*pfglXChooseVisual)(Display *dpy, int screen,
										  int *attribList);
#ifndef HAVE_PFGLXCHOOSEVISUAL
/* we just declared the prototype for it, so we should have it now: */
# define HAVE_PFGLXCHOOSEVISUAL 1
#endif /* !HAVE_PFGLXCHOOSEVISUAL */
OPENGL_EXTERN void (*pfglXCopyContext)(Display *dpy, GLXContext src, GLXContext dst,
								GLuint mask);
OPENGL_EXTERN GLXContext (*pfglXCreateContext)(Display *dpy, XVisualInfo *vis,
								 GLXContext shareList, Bool direct);
#ifndef HAVE_PFGLXCREATECONTEXT
/* we just declared the prototype for it, so we should have it now: */
# define HAVE_PFGLXCREATECONTEXT 1
#endif /* !HAVE_PFGLXCREATECONTEXT */
OPENGL_EXTERN GLXPixmap (*pfglXCreateGLXPixmap)(Display *dpy, XVisualInfo *vis,
										 Pixmap pixmap);
OPENGL_EXTERN void (*pfglXDestroyContext)(Display *dpy, GLXContext ctx);
OPENGL_EXTERN void (*pfglXDestroyGLXPixmap)(Display *dpy, GLXPixmap pix);
OPENGL_EXTERN int (*pfglXGetConfig)(Display *dpy, XVisualInfo *vis, int attrib,
							 int *value);
OPENGL_EXTERN GLXContext (*pfglXGetCurrentContext)(void);
OPENGL_EXTERN GLXDrawable (*pfglXGetCurrentDrawable)(void);
OPENGL_EXTERN Bool (*pfglXIsDirect)(Display *dpy, GLXContext ctx);
OPENGL_EXTERN Bool (*pfglXMakeCurrent)(Display *dpy, GLXDrawable drawable,
								GLXContext ctx);
#ifndef HAVE_PFGLXMAKECURRENT
/* we just declared the prototype for it, so we should have it now: */
# define HAVE_PFGLXMAKECURRENT 1
#endif /* !HAVE_PFGLXMAKECURRENT */
OPENGL_EXTERN Bool (*pfglXQueryExtension)(Display *dpy, int *errorBase,
								   int *eventBase);
OPENGL_EXTERN Bool (*pfglXQueryVersion)(Display *dpy, int *major, int *minor);
OPENGL_EXTERN void (*pfglXSwapBuffers)(Display *dpy, GLXDrawable drawable);
OPENGL_EXTERN void (*pfglXUseXFont)(Font font, int first, int count, int listBase);
OPENGL_EXTERN void (*pfglXWaitGL)(void);
OPENGL_EXTERN void (*pfglXWaitX)(void);

#endif /* (__unix__ || __APPLE__) && !_WIN32 */

#endif /* __MYOPENGL_H__ */

/* EOF */
