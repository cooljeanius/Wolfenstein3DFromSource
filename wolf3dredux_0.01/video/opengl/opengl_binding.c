/*
 *	Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*
 *	opengl_binding.c:   Interface to OpenGL library.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 */


/*
 *  Notes:
 *
 *	This module implements the OS binding of OpenGL to our
 *	function pointers. The following two functions must be
 *	implemented when doing a port:
 *
 *	OpenGL_Init() - Loads libraries and assigns function pointers.
 *	OpenGL_Shutdown() - Unloads libraries, NULLs function pointers.
 *
 */

#ifdef _WIN32
# include <float.h>
# include <windows.h>
HINSTANCE hinstOpenGL;
#elif __unix__ || __APPLE__
# include <dlfcn.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include "../../gnulib_replacement_funcs/strnlen1.h"
void *OpenGLLib;
#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */


#include "opengl_local.h"
#include "myopengl.h"


#include "../../common/common.h"



/*
-----------------------------------------------------------------------------
 Function: OpenGL_Shutdown -Shutdown interface to OpenGL.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	Unloads the specified Dynamic Link Library then NULLs out all the
	proc pointers.

-----------------------------------------------------------------------------
*/
PUBLIC void OpenGL_Shutdown(void)
{

#ifdef _WIN32
	if (hinstOpenGL) {
		FreeLibrary(hinstOpenGL);
		hinstOpenGL = NULL;
	}

	hinstOpenGL = NULL;
#elif __unix__ || __APPLE__
	if (OpenGLLib) {
		dlclose(OpenGLLib);
		OpenGLLib = NULL;
	}

	OpenGLLib = NULL;
#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */

	pfglAccum = NULL;
	pfglAlphaFunc = NULL;
	pfglAreTexturesResident = NULL;
	pfglArrayElement = NULL;
	pfglBegin = NULL;
	pfglBindTexture = NULL;
	pfglBitmap = NULL;
	pfglBlendFunc = NULL;
	pfglCallList = NULL;
	pfglCallLists = NULL;
	pfglClear = NULL;
	pfglClearAccum = NULL;
	pfglClearColor = NULL;
	pfglClearDepth = NULL;
	pfglClearIndex = NULL;
	pfglClearStencil = NULL;
	pfglClipPlane = NULL;
	pfglColor3b = NULL;
	pfglColor3bv = NULL;
	pfglColor3d = NULL;
	pfglColor3dv = NULL;
	pfglColor3f = NULL;
	pfglColor3fv = NULL;
	pfglColor3i = NULL;
	pfglColor3iv = NULL;
	pfglColor3s = NULL;
	pfglColor3sv = NULL;
	pfglColor3ub = NULL;
	pfglColor3ubv = NULL;
	pfglColor3ui = NULL;
	pfglColor3uiv = NULL;
	pfglColor3us = NULL;
	pfglColor3usv = NULL;
	pfglColor4b = NULL;
	pfglColor4bv = NULL;
	pfglColor4d = NULL;
	pfglColor4dv = NULL;
	pfglColor4f = NULL;
	pfglColor4fv = NULL;
	pfglColor4i = NULL;
	pfglColor4iv = NULL;
	pfglColor4s = NULL;
	pfglColor4sv = NULL;
	pfglColor4ub = NULL;
	pfglColor4ubv = NULL;
	pfglColor4ui = NULL;
	pfglColor4uiv = NULL;
	pfglColor4us = NULL;
	pfglColor4usv = NULL;
	pfglColorMask = NULL;
	pfglColorMaterial = NULL;
	pfglColorPointer = NULL;
	pfglCopyPixels = NULL;
	pfglCopyTexImage1D = NULL;
	pfglCopyTexImage2D = NULL;
	pfglCopyTexSubImage1D = NULL;
	pfglCopyTexSubImage2D = NULL;
	pfglCullFace = NULL;
	pfglDeleteLists = NULL;
	pfglDeleteTextures = NULL;
	pfglDepthFunc = NULL;
	pfglDepthMask = NULL;
	pfglDepthRange = NULL;
	pfglDisable = NULL;
	pfglDisableClientState = NULL;
	pfglDrawArrays = NULL;
	pfglDrawBuffer = NULL;
	pfglDrawElements = NULL;
	pfglDrawPixels = NULL;
	pfglEdgeFlag = NULL;
	pfglEdgeFlagPointer = NULL;
	pfglEdgeFlagv = NULL;
	pfglEnable = NULL;
	pfglEnableClientState = NULL;
	pfglEnd = NULL;
	pfglEndList = NULL;
	pfglEvalCoord1d = NULL;
	pfglEvalCoord1dv = NULL;
	pfglEvalCoord1f = NULL;
	pfglEvalCoord1fv = NULL;
	pfglEvalCoord2d = NULL;
	pfglEvalCoord2dv = NULL;
	pfglEvalCoord2f = NULL;
	pfglEvalCoord2fv = NULL;
	pfglEvalMesh1 = NULL;
	pfglEvalMesh2 = NULL;
	pfglEvalPoint1 = NULL;
	pfglEvalPoint2 = NULL;
	pfglFeedbackBuffer = NULL;
	pfglFinish = NULL;
	pfglFlush = NULL;
	pfglFogf = NULL;
	pfglFogfv = NULL;
	pfglFogi = NULL;
	pfglFogiv = NULL;
	pfglFrontFace = NULL;
	pfglFrustum = NULL;
	pfglGenLists = NULL;
	pfglGenTextures = NULL;
	pfglGetBooleanv = NULL;
	pfglGetClipPlane = NULL;
	pfglGetDoublev = NULL;
	pfglGetError = NULL;
	pfglGetFloatv = NULL;
	pfglGetIntegerv = NULL;
	pfglGetLightfv = NULL;
	pfglGetLightiv = NULL;
	pfglGetMapdv = NULL;
	pfglGetMapfv = NULL;
	pfglGetMapiv = NULL;
	pfglGetMaterialfv = NULL;
	pfglGetMaterialiv = NULL;
	pfglGetPixelMapfv = NULL;
	pfglGetPixelMapuiv = NULL;
	pfglGetPixelMapusv = NULL;
	pfglGetPointerv = NULL;
	pfglGetPolygonStipple = NULL;
	pfglGetString = NULL;
	pfglGetTexEnvfv = NULL;
	pfglGetTexEnviv = NULL;
	pfglGetTexGendv = NULL;
	pfglGetTexGenfv = NULL;
	pfglGetTexGeniv = NULL;
	pfglGetTexImage = NULL;
	pfglGetTexParameterfv = NULL;
	pfglGetTexParameteriv = NULL;
	pfglHint = NULL;
	pfglIndexMask = NULL;
	pfglIndexPointer = NULL;
	pfglIndexd = NULL;
	pfglIndexdv = NULL;
	pfglIndexf = NULL;
	pfglIndexfv = NULL;
	pfglIndexi = NULL;
	pfglIndexiv = NULL;
	pfglIndexs = NULL;
	pfglIndexsv = NULL;
	pfglIndexub = NULL;
	pfglIndexubv = NULL;
	pfglInitNames = NULL;
	pfglInterleavedArrays = NULL;
	pfglIsEnabled = NULL;
	pfglIsList = NULL;
	pfglIsTexture = NULL;
	pfglLightModelf = NULL;
	pfglLightModelfv = NULL;
	pfglLightModeli = NULL;
	pfglLightModeliv = NULL;
	pfglLightf = NULL;
	pfglLightfv = NULL;
	pfglLighti = NULL;
	pfglLightiv = NULL;
	pfglLineStipple = NULL;
	pfglLineWidth = NULL;
	pfglListBase = NULL;
	pfglLoadIdentity = NULL;
	pfglLoadMatrixd = NULL;
	pfglLoadMatrixf = NULL;
	pfglLoadName = NULL;
	pfglLogicOp = NULL;
	pfglMap1d = NULL;
	pfglMap1f = NULL;
	pfglMap2d = NULL;
	pfglMap2f = NULL;
	pfglMapGrid1d = NULL;
	pfglMapGrid1f = NULL;
	pfglMapGrid2d = NULL;
	pfglMapGrid2f = NULL;
	pfglMaterialf = NULL;
	pfglMaterialfv = NULL;
	pfglMateriali = NULL;
	pfglMaterialiv = NULL;
	pfglMatrixMode = NULL;
	pfglMultMatrixd = NULL;
	pfglMultMatrixf = NULL;
	pfglNewList = NULL;
	pfglNormal3b = NULL;
	pfglNormal3bv = NULL;
	pfglNormal3d = NULL;
	pfglNormal3dv = NULL;
	pfglNormal3f = NULL;
	pfglNormal3fv = NULL;
	pfglNormal3i = NULL;
	pfglNormal3iv = NULL;
	pfglNormal3s = NULL;
	pfglNormal3sv = NULL;
	pfglNormalPointer = NULL;
	pfglOrtho = NULL;
	pfglPassThrough = NULL;
	pfglPixelMapfv = NULL;
	pfglPixelMapuiv = NULL;
	pfglPixelMapusv = NULL;
	pfglPixelStoref = NULL;
	pfglPixelStorei = NULL;
	pfglPixelTransferf = NULL;
	pfglPixelTransferi = NULL;
	pfglPixelZoom = NULL;
	pfglPointSize = NULL;
	pfglPolygonMode = NULL;
	pfglPolygonOffset = NULL;
	pfglPolygonStipple = NULL;
	pfglPopAttrib = NULL;
	pfglPopClientAttrib = NULL;
	pfglPopMatrix = NULL;
	pfglPopName = NULL;
	pfglPrioritizeTextures = NULL;
	pfglPushAttrib = NULL;
	pfglPushClientAttrib = NULL;
	pfglPushMatrix = NULL;
	pfglPushName = NULL;
	pfglRasterPos2d = NULL;
	pfglRasterPos2dv = NULL;
	pfglRasterPos2f = NULL;
	pfglRasterPos2fv = NULL;
	pfglRasterPos2i = NULL;
	pfglRasterPos2iv = NULL;
	pfglRasterPos2s = NULL;
	pfglRasterPos2sv = NULL;
	pfglRasterPos3d = NULL;
	pfglRasterPos3dv = NULL;
	pfglRasterPos3f = NULL;
	pfglRasterPos3fv = NULL;
	pfglRasterPos3i = NULL;
	pfglRasterPos3iv = NULL;
	pfglRasterPos3s = NULL;
	pfglRasterPos3sv = NULL;
	pfglRasterPos4d = NULL;
	pfglRasterPos4dv = NULL;
	pfglRasterPos4f = NULL;
	pfglRasterPos4fv = NULL;
	pfglRasterPos4i = NULL;
	pfglRasterPos4iv = NULL;
	pfglRasterPos4s = NULL;
	pfglRasterPos4sv = NULL;
	pfglReadBuffer = NULL;
	pfglReadPixels = NULL;
	pfglRectd = NULL;
	pfglRectdv = NULL;
	pfglRectf = NULL;
	pfglRectfv = NULL;
	pfglRecti = NULL;
	pfglRectiv = NULL;
	pfglRects = NULL;
	pfglRectsv = NULL;
	pfglRenderMode = NULL;
	pfglRotated = NULL;
	pfglRotatef = NULL;
	pfglScaled = NULL;
	pfglScalef = NULL;
	pfglScissor = NULL;
	pfglSelectBuffer = NULL;
	pfglShadeModel = NULL;
	pfglStencilFunc = NULL;
	pfglStencilMask = NULL;
	pfglStencilOp = NULL;
	pfglTexCoord1d = NULL;
	pfglTexCoord1dv = NULL;
	pfglTexCoord1f = NULL;
	pfglTexCoord1fv = NULL;
	pfglTexCoord1i = NULL;
	pfglTexCoord1iv = NULL;
	pfglTexCoord1s = NULL;
	pfglTexCoord1sv = NULL;
	pfglTexCoord2d = NULL;
	pfglTexCoord2dv = NULL;
	pfglTexCoord2f = NULL;
	pfglTexCoord2fv = NULL;
	pfglTexCoord2i = NULL;
	pfglTexCoord2iv = NULL;
	pfglTexCoord2s = NULL;
	pfglTexCoord2sv = NULL;
	pfglTexCoord3d = NULL;
	pfglTexCoord3dv = NULL;
	pfglTexCoord3f = NULL;
	pfglTexCoord3fv = NULL;
	pfglTexCoord3i = NULL;
	pfglTexCoord3iv = NULL;
	pfglTexCoord3s = NULL;
	pfglTexCoord3sv = NULL;
	pfglTexCoord4d = NULL;
	pfglTexCoord4dv = NULL;
	pfglTexCoord4f = NULL;
	pfglTexCoord4fv = NULL;
	pfglTexCoord4i = NULL;
	pfglTexCoord4iv = NULL;
	pfglTexCoord4s = NULL;
	pfglTexCoord4sv = NULL;
	pfglTexCoordPointer = NULL;
	pfglTexEnvf = NULL;
	pfglTexEnvfv = NULL;
	pfglTexEnvi = NULL;
	pfglTexEnviv = NULL;
	pfglTexGend = NULL;
	pfglTexGendv = NULL;
	pfglTexGenf = NULL;
	pfglTexGenfv = NULL;
	pfglTexGeni = NULL;
	pfglTexGeniv = NULL;
	pfglTexImage1D = NULL;
	pfglTexImage2D = NULL;
	pfglTexParameterf = NULL;
	pfglTexParameterfv = NULL;
	pfglTexParameteri = NULL;
	pfglTexParameteriv = NULL;
	pfglTexSubImage1D = NULL;
	pfglTexSubImage2D = NULL;
	pfglTranslated = NULL;
	pfglTranslatef = NULL;
	pfglVertex2d = NULL;
	pfglVertex2dv = NULL;
	pfglVertex2f = NULL;
	pfglVertex2fv = NULL;
	pfglVertex2i = NULL;
	pfglVertex2iv = NULL;
	pfglVertex2s = NULL;
	pfglVertex2sv = NULL;
	pfglVertex3d = NULL;
	pfglVertex3dv = NULL;
	pfglVertex3f = NULL;
	pfglVertex3fv = NULL;
	pfglVertex3i = NULL;
	pfglVertex3iv = NULL;
	pfglVertex3s = NULL;
	pfglVertex3sv = NULL;
	pfglVertex4d = NULL;
	pfglVertex4dv = NULL;
	pfglVertex4f = NULL;
	pfglVertex4fv = NULL;
	pfglVertex4i = NULL;
	pfglVertex4iv = NULL;
	pfglVertex4s = NULL;
	pfglVertex4sv = NULL;
	pfglVertexPointer = NULL;
	pfglViewport = NULL;

#ifdef _WIN32
	pfwglCopyContext = NULL;
	pfwglCreateContext = NULL;
	pfwglCreateLayerContext = NULL;
	pfwglDeleteContext = NULL;
	pfwglDescribeLayerPlane = NULL;
	pfwglGetCurrentContext = NULL;
	pfwglGetCurrentDC = NULL;
	pfwglGetLayerPaletteEntries = NULL;
	pfwglGetProcAddress = NULL;
	pfwglMakeCurrent = NULL;
	pfwglRealizeLayerPalette = NULL;
	pfwglSetLayerPaletteEntries = NULL;
	pfwglShareLists = NULL;
	pfwglSwapLayerBuffers = NULL;
	pfwglUseFontBitmaps = NULL;
	pfwglUseFontOutlines = NULL;

	pfwglChoosePixelFormat = NULL;
	pfwglDescribePixelFormat = NULL;
	pfwglGetPixelFormat = NULL;
	pfwglSetPixelFormat = NULL;
	pfwglSwapBuffers = NULL;
#elif __unix__ || __APPLE__
/* __APPLE__ depends on tentative similar preprocessor macro check also existing
 * in "myopengl.h" */
	pfglXChooseVisual = NULL;
	pfglXCreateContext = NULL;
	pfglXDestroyContext = NULL;
	pfglXMakeCurrent = NULL;
	pfglXCopyContext = NULL;
	pfglXSwapBuffers = NULL;
#elif __APPLE__
# warning "Please define interface to OpenGL library for your platform!"
#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */


/*
 *	Extensions
 */
	pfglMultiTexCoord2fARB		 = NULL;

}


#ifdef _WIN32
# define GPA(a) GetProcAddress(hinstOpenGL, a)
#elif __unix__ || __APPLE__
# define GPA(a) dlsym(OpenGLLib, a)
	void *pfwglGetProcAddress(const char *symbol) {
		if (OpenGLLib) {
			return GPA(symbol);
		}

		return NULL;
	}
#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */


/*
-----------------------------------------------------------------------------
 Function: OpenGL_Init -Interface to OpenGL.

 Parameters: dllname -[in] Pointer to a NUL-terminated string with the
                        name of the OpenGL dynamic link library.

 Returns: Non-zero on success, otherwise zero.

 Notes: This is responsible for binding our gl function pointers to
		the appropriate OpenGL stuff. In Windows this means doing a
		LoadLibrary and a bunch of calls to GetProcAddress. On Unix-like
		systems, like GNU/Linux, FreeBSD, and Darwin (i.e. OS X), this means
		doing a call to dlopen() and a bunch of calls to dlsym(). On other
		operating systems we need to do the right thing, whatever that
		might be.

-----------------------------------------------------------------------------
*/
PUBLIC int OpenGL_Init(const char *dllname)
{
#ifdef _WIN32
	char buffer[1024], *ptr;

	SearchPath(NULL, dllname, NULL, (sizeof(buffer) - 1), buffer, &ptr);

	Com_Printf("...calling LoadLibrary(%s): ", buffer);

	if ((hinstOpenGL = LoadLibrary(dllname)) == 0) {
		char *buf = NULL;

		Com_Printf("failed\n");

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL, GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&buf, 0, NULL);

		Com_Printf("%s\n", buf);

		return 0;
	}
#elif __unix__ || __APPLE__
	char *loadpath;
	loadpath = getenv("LD_LIBRARY_PATH");
# if __APPLE__
	/* __APPLE__ has two additional environment variables besides LD_LIBRARY_PATH
	 * that dlopen looks in (see the dlopen manpage), so check those as well,
	 * then try concatenating the Apple-specific variables first to get an
	 * overall Apple-specific variable, and finally try concatenating that
	 * (if it exists) with the standard Unix one: */
	char *appleloadpath, *applefallbackloadpath, *applecombinedloadpath, *allcombinedloadpath;
	appleloadpath = getenv("DYLD_LIBRARY_PATH");
	applefallbackloadpath = getenv("DYLD_DYLD_FALLBACK_LIBRARY_PATH");
	/* TODO: make sure path separators (i.e. ":"s) are preserved */
#  ifndef PATH_MAX
#   warning "opengl_binding.c expects PATH_MAX to be defined when building for __APPLE__."
#  endif /* !PATH_MAX */
	/* trying to concatenate a NULL string leads to errors, so make sure the
	 * strings to avoid that: */
	if ((applefallbackloadpath != NULL) && (appleloadpath != NULL)) {
		if (strnlen1(applefallbackloadpath, (size_t)PATH_MAX) >
			sizeof(appleloadpath) - strlen(appleloadpath)) {
			Com_Printf("appleloadpath will be truncated in string concatenation.\n");
		}
		applecombinedloadpath = strncat(appleloadpath, applefallbackloadpath,
										(sizeof(appleloadpath) - strlen(appleloadpath) - 1));
	} else if (appleloadpath != NULL) {
		applecombinedloadpath = appleloadpath;
	} else if (applefallbackloadpath != NULL) {
		applecombinedloadpath = applefallbackloadpath;
	} else {
		applecombinedloadpath = ":";
	}

	/* same reasoning as above for the checks against NULL: */
	if ((applecombinedloadpath != NULL) && (loadpath != NULL)) {
		if (strnlen1(applecombinedloadpath, (size_t)PATH_MAX) >
			(sizeof(loadpath) - strlen(loadpath))) {
			Com_Printf("loadpath will be truncated in string concatenation.\n");
		}
		allcombinedloadpath = strncat(loadpath, applecombinedloadpath,
									  sizeof(loadpath) - strlen(loadpath) - 1);
	} else if (applecombinedloadpath != NULL) {
		allcombinedloadpath = applecombinedloadpath;
	} else if (loadpath != NULL) {
		allcombinedloadpath = loadpath;
	} else {
		allcombinedloadpath = ":";
	}

	/* the only point of calculating allcombinedloadpath is to be able to print
	 * it here, for debugging purposes. It may not quite look the same as what
	 * is actually being used.
	 * (e.g., there may be missing colons - see the TODO above) */
	Com_Printf("...calling dlopen(%s) with searchpath %s: ", dllname, allcombinedloadpath);
# else
	Com_Printf("...calling dlopen(%s) with searchpath %s: ", dllname, loadpath);
# endif /* __APPLE__ */

	if ((OpenGLLib = dlopen(dllname, (RTLD_LAZY | RTLD_GLOBAL))) == 0) {
		Com_Printf(" failed;\n");

		Com_Printf("%s\n", dlerror());

		Com_Printf("(try checking your LD_LIBRARY_PATH directories.)\n");
# ifdef __APPLE__
		Com_Printf("(Apple users should try checking their DYLD_*_PATH directories as well.)\n");
# endif /* __APPLE__ */

		return 0;
	} else {
		Com_Printf(" ...dlopen()-ed successfully, at least, and... ");
	}

#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */

	if (!(pfglAccum = (GLACCUM)GPA("glAccum"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglAlphaFunc = (GLALPHAFUNC)GPA("glAlphaFunc"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglAreTexturesResident = (GLARETEXTURESRESIDENT)GPA("glAreTexturesResident"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglArrayElement = (GLARRAYELEMENT)GPA("glArrayElement"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglBegin = (GLBEGIN)GPA("glBegin"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglBindTexture = (GLBINDTEXTURE)GPA("glBindTexture"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglBitmap = (GLBITMAP)GPA("glBitmap"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglBlendFunc = (GLBLENDFUNC)GPA("glBlendFunc"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCallList = (GLCALLLIST)GPA("glCallList"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCallLists = (GLCALLLISTS)GPA("glCallLists"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClear = (GLCLEAR)GPA("glClear"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClearAccum = (GLCLEARACCUM)GPA("glClearAccum"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClearColor = (GLCLEARCOLOR)GPA("glClearColor"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClearDepth = (GLCLEARDEPTH)GPA("glClearDepth"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClearIndex = (GLCLEARINDEX)GPA("glClearIndex"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClearStencil = (GLCLEARSTENCIL)GPA("glClearStencil"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglClipPlane = (GLCLIPPLANE)GPA("glClipPlane"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3b = (GLCOLOR3B)GPA("glColor3b"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3bv = (GLCOLOR3BV)GPA("glColor3bv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3d = (GLCOLOR3D)GPA("glColor3d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3dv = (GLCOLOR3DV)GPA("glColor3dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3f = (GLCOLOR3F)GPA("glColor3f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3fv = (GLCOLOR3FV)GPA("glColor3fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3i = (GLCOLOR3I)GPA("glColor3i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3iv = (GLCOLOR3IV)GPA("glColor3iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3s = (GLCOLOR3S)GPA("glColor3s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3sv = (GLCOLOR3SV)GPA("glColor3sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3ub = (GLCOLOR3UB)GPA("glColor3ub"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3ubv = (GLCOLOR3UBV)GPA("glColor3ubv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3ui = (GLCOLOR3UI)GPA("glColor3ui"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3uiv = (GLCOLOR3UIV)GPA("glColor3uiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3us = (GLCOLOR3US)GPA("glColor3us"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor3usv = (GLCOLOR3USV)GPA("glColor3usv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4b = (GLCOLOR4B)GPA("glColor4b"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4bv = (GLCOLOR4BV)GPA("glColor4bv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4d = (GLCOLOR4D)GPA("glColor4d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4dv = (GLCOLOR4DV)GPA("glColor4dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4f = (GLCOLOR4F)GPA("glColor4f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4fv = (GLCOLOR4FV)GPA("glColor4fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4i = (GLCOLOR4I)GPA("glColor4i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4iv = (GLCOLOR4IV)GPA("glColor4iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4s = (GLCOLOR4S)GPA("glColor4s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4sv = (GLCOLOR4SV)GPA("glColor4sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4ub = (GLCOLOR4UB)GPA("glColor4ub"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4ubv = (GLCOLOR4UBV)GPA("glColor4ubv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4ui = (GLCOLOR4UI)GPA("glColor4ui"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4uiv = (GLCOLOR4UIV)GPA("glColor4uiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4us = (GLCOLOR4US)GPA("glColor4us"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColor4usv = (GLCOLOR4USV)GPA("glColor4usv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColorMask = (GLCOLORMASK)GPA("glColorMask"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColorMaterial = (GLCOLORMATERIAL)GPA("glColorMaterial"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglColorPointer = (GLCOLORPOINTER)GPA("glColorPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCopyPixels = (GLCOPYPIXELS)GPA("glCopyPixels"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCopyTexImage1D = (GLCOPYTEXIMAGE1D)GPA("glCopyTexImage1D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCopyTexImage2D = (GLCOPYTEXIMAGE2D)GPA("glCopyTexImage2D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCopyTexSubImage1D = (GLCOPYTEXSUBIMAGE1D)GPA("glCopyTexSubImage1D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCopyTexSubImage2D = (GLCOPYTEXSUBIMAGE2D)GPA("glCopyTexSubImage2D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglCullFace = (GLCULLFACE)GPA("glCullFace"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDeleteLists = (GLDELETELISTS)GPA("glDeleteLists"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDeleteTextures = (GLDELETETEXTURES)GPA("glDeleteTextures"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDepthFunc = (GLDEPTHFUNC)GPA("glDepthFunc"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDepthMask = (GLDEPTHMASK)GPA("glDepthMask"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDepthRange = (GLDEPTHRANGE)GPA("glDepthRange"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDisable = (GLDISABLE)GPA("glDisable"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDisableClientState = (GLDISABLECLIENTSTATE)GPA("glDisableClientState"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDrawArrays = (GLDRAWARRAYS)GPA("glDrawArrays"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDrawBuffer = (GLDRAWBUFFER)GPA("glDrawBuffer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDrawElements = (GLDRAWELEMENTS)GPA("glDrawElements"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglDrawPixels = (GLDRAWPIXELS)GPA("glDrawPixels"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEdgeFlag = (GLEDGEFLAG)GPA("glEdgeFlag"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEdgeFlagPointer = (GLEDGEFLAGPOINTER)GPA("glEdgeFlagPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEdgeFlagv = (GLEDGEFLAGV)GPA("glEdgeFlagv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEnable = (GLENABLE)GPA("glEnable"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEnableClientState	= (GLENABLECLIENTSTATE)GPA("glEnableClientState"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEnd = (GLEND)GPA("glEnd"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEndList = (GLENDLIST)GPA("glEndList"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord1d = (GLEVALCOORD1D)GPA("glEvalCoord1d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord1dv = (GLEVALCOORD1DV)GPA("glEvalCoord1dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord1f = (GLEVALCOORD1F)GPA("glEvalCoord1f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord1fv = (GLEVALCOORD1FV)GPA("glEvalCoord1fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord2d = (GLEVALCOORD2D)GPA("glEvalCoord2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord2dv = (GLEVALCOORD2DV)GPA("glEvalCoord2dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord2f = (GLEVALCOORD2F)GPA("glEvalCoord2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalCoord2fv = (GLEVALCOORD2FV)GPA("glEvalCoord2fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalMesh1 = (GLEVALMESH1)GPA("glEvalMesh1"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalMesh2 = (GLEVALMESH2)GPA("glEvalMesh2"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalPoint1 = (GLEVALPOINT1)GPA("glEvalPoint1"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglEvalPoint2 = (GLEVALPOINT2)GPA("glEvalPoint2"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFeedbackBuffer = (GLFEEDBACKBUFFER)GPA("glFeedbackBuffer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFinish = (GLFINISH)GPA("glFinish"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFlush = (GLFLUSH)GPA("glFlush"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFogf = (GLFOGF)GPA("glFogf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFogfv = (GLFOGFV)GPA("glFogfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFogi = (GLFOGI)GPA("glFogi"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFogiv = (GLFOGIV)GPA("glFogiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFrontFace = (GLFRONTFACE)GPA("glFrontFace"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglFrustum = (GLFRUSTUM)GPA("glFrustum"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGenLists = (GLGENLISTS)GPA("glGenLists"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGenTextures = (GLGENTEXTURES)GPA("glGenTextures"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetBooleanv = (GLGETBOOLEANV)GPA("glGetBooleanv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetClipPlane = (GLGETCLIPPLANE)GPA("glGetClipPlane"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetDoublev = (GLGETDOUBLEV)GPA("glGetDoublev"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetError = (GLGETERROR)GPA("glGetError"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetFloatv = (GLGETFLOATV)GPA("glGetFloatv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetIntegerv = (GLGETINTEGERV)GPA("glGetIntegerv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetLightfv = (GLGETLIGHTFV)GPA("glGetLightfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetLightiv = (GLGETLIGHTIV)GPA("glGetLightiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetMapdv = (GLGETMAPDV)GPA("glGetMapdv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetMapfv = (GLGETMAPFV)GPA("glGetMapfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetMapiv = (GLGETMAPIV)GPA("glGetMapiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetMaterialfv = (GLGETMATERIALFV)GPA("glGetMaterialfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetMaterialiv = (GLGETMATERIALIV)GPA("glGetMaterialiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetPixelMapfv = (GLGETPIXELMAPFV)GPA("glGetPixelMapfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetPixelMapuiv = (GLGETPIXELMAPUIV)GPA("glGetPixelMapuiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetPixelMapusv = (GLGETPIXELMAPUSV)GPA("glGetPixelMapusv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetPointerv = (GLGETPOINTERV)GPA("glGetPointerv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetPolygonStipple = (GLGETPOLYGONSTIPPLE)GPA("glGetPolygonStipple"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetString = (GLGETSTRING)GPA("glGetString"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexEnvfv = (GLGETTEXENVFV)GPA("glGetTexEnvfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexEnviv = (GLGETTEXENVIV)GPA("glGetTexEnviv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexGendv = (GLGETTEXGENDV)GPA("glGetTexGendv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexGenfv = (GLGETTEXGENFV)GPA("glGetTexGenfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexGeniv = (GLGETTEXGENIV)GPA("glGetTexGeniv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexImage = (GLGETTEXIMAGE)GPA("glGetTexImage"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexParameterfv = (GLGETTEXPARAMETERFV)GPA("glGetTexParameterfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglGetTexParameteriv = (GLGETTEXPARAMETERIV)GPA("glGetTexParameteriv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglHint = (GLHINT)GPA("glHint"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexMask = (GLINDEXMASK)GPA("glIndexMask"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexPointer = (GLINDEXPOINTER)GPA("glIndexPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexd = (GLINDEXD)GPA("glIndexd"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexdv = (GLINDEXDV)GPA("glIndexdv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexf = (GLINDEXF)GPA("glIndexf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexfv = (GLINDEXFV)GPA("glIndexfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexi = (GLINDEXI)GPA("glIndexi"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexiv = (GLINDEXIV)GPA("glIndexiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexs = (GLINDEXS)GPA("glIndexs"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexsv = (GLINDEXSV)GPA("glIndexsv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexub = (GLINDEXUB)GPA("glIndexub"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIndexubv = (GLINDEXUBV)GPA("glIndexubv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglInitNames = (GLINITNAMES)GPA("glInitNames"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglInterleavedArrays = (GLINTERLEAVEDARRAYS)GPA("glInterleavedArrays"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIsEnabled = (GLISENABLED)GPA("glIsEnabled"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIsList = (GLISLIST)GPA("glIsList"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglIsTexture = (GLISTEXTURE)GPA("glIsTexture"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightModelf = (GLLIGHTMODELF)GPA("glLightModelf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightModelfv = (GLLIGHTMODELFV)GPA("glLightModelfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightModeli = (GLLIGHTMODELI)GPA("glLightModeli"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightModeliv = (GLLIGHTMODELIV)GPA("glLightModeliv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightf = (GLLIGHTF)GPA("glLightf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightfv = (GLLIGHTFV)GPA("glLightfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLighti = (GLLIGHTI)GPA("glLighti"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLightiv = (GLLIGHTIV)GPA("glLightiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLineStipple = (GLLINESTIPPLE)GPA("glLineStipple"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLineWidth = (GLLINEWIDTH)GPA("glLineWidth"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglListBase = (GLLISTBASE)GPA("glListBase"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLoadIdentity = (GLLOADIDENTITY)GPA("glLoadIdentity"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLoadMatrixd = (GLLOADMATRIXD)GPA("glLoadMatrixd"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLoadMatrixf = (GLLOADMATRIXF)GPA("glLoadMatrixf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLoadName = (GLLOADNAME)GPA("glLoadName"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglLogicOp = (GLLOGICOP)GPA("glLogicOp"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMap1d = (GLMAP1D)GPA("glMap1d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMap1f = (GLMAP1F)GPA("glMap1f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMap2d = (GLMAP2D)GPA("glMap2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMap2f = (GLMAP2F)GPA("glMap2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMapGrid1d = (GLMAPGRID1D)GPA("glMapGrid1d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMapGrid1f = (GLMAPGRID1F)GPA("glMapGrid1f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMapGrid2d = (GLMAPGRID2D)GPA("glMapGrid2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMapGrid2f = (GLMAPGRID2F)GPA("glMapGrid2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMaterialf = (GLMATERIALF)GPA("glMaterialf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMaterialfv = (GLMATERIALFV)GPA("glMaterialfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMateriali = (GLMATERIALI)GPA("glMateriali"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMaterialiv = (GLMATERIALIV)GPA("glMaterialiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMatrixMode = (GLMATRIXMODE)GPA("glMatrixMode"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMultMatrixd = (GLMULTMATRIXD)GPA("glMultMatrixd"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglMultMatrixf = (GLMULTMATRIXF)GPA("glMultMatrixf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNewList = (GLNEWLIST)GPA("glNewList"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3b = (GLNORMAL3B)GPA("glNormal3b"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3bv = (GLNORMAL3BV)GPA("glNormal3bv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3d = (GLNORMAL3D)GPA("glNormal3d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3dv = (GLNORMAL3DV)GPA("glNormal3dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3f = (GLNORMAL3F)GPA("glNormal3f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3fv = (GLNORMAL3FV)GPA("glNormal3fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3i = (GLNORMAL3I)GPA("glNormal3i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3iv = (GLNORMAL3IV)GPA("glNormal3iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3s = (GLNORMAL3S)GPA("glNormal3s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormal3sv = (GLNORMAL3SV)GPA("glNormal3sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglNormalPointer = (GLNORMALPOINTER)GPA("glNormalPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglOrtho = (GLORTHO)GPA("glOrtho"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPassThrough = (GLPASSTHROUGH)GPA("glPassThrough"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelMapfv = (GLPIXELMAPFV)GPA("glPixelMapfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelMapuiv = (GLPIXELMAPUIV)GPA("glPixelMapuiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelMapusv = (GLPIXELMAPUSV)GPA("glPixelMapusv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelStoref = (GLPIXELSTOREF)GPA("glPixelStoref"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelStorei = (GLPIXELSTOREI)GPA("glPixelStorei"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelTransferf = (GLPIXELTRANSFERF)GPA("glPixelTransferf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelTransferi = (GLPIXELTRANSFERI)GPA("glPixelTransferi"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPixelZoom = (GLPIXELZOOM)GPA("glPixelZoom"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPointSize = (GLPOINTSIZE)GPA("glPointSize"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPolygonMode = (GLPOLYGONMODE)GPA("glPolygonMode"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPolygonOffset = (GLPOLYGONOFFSET)GPA("glPolygonOffset"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPolygonStipple = (GLPOLYGONSTIPPLE)GPA("glPolygonStipple"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPopAttrib = (GLPOPATTRIB)GPA("glPopAttrib"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPopClientAttrib = (GLPOPCLIENTATTRIB)GPA("glPopClientAttrib"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPopMatrix = (GLPOPMATRIX)GPA("glPopMatrix"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPopName = (GLPOPNAME)GPA("glPopName"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPrioritizeTextures = (GLPRIORITIZETEXTURES)GPA("glPrioritizeTextures"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPushAttrib = (GLPUSHATTRIB)GPA("glPushAttrib"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPushClientAttrib = (GLPUSHCLIENTATTRIB)GPA("glPushClientAttrib"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPushMatrix = (GLPUSHMATRIX)GPA("glPushMatrix"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglPushName = (GLPUSHNAME)GPA("glPushName"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2d = (GLRASTERPOS2D)GPA("glRasterPos2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2dv = (GLRASTERPOS2DV)GPA("glRasterPos2dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2f = (GLRASTERPOS2F)GPA("glRasterPos2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2fv = (GLRASTERPOS2FV)GPA("glRasterPos2fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2i = (GLRASTERPOS2I)GPA("glRasterPos2i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2iv = (GLRASTERPOS2IV)GPA("glRasterPos2iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2s = (GLRASTERPOS2S)GPA("glRasterPos2s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos2sv = (GLRASTERPOS2SV)GPA("glRasterPos2sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3d = (GLRASTERPOS3D)GPA("glRasterPos3d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3dv = (GLRASTERPOS3DV)GPA("glRasterPos3dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3f = (GLRASTERPOS3F)GPA("glRasterPos3f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3fv = (GLRASTERPOS3FV)GPA("glRasterPos3fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3i = (GLRASTERPOS3I)GPA("glRasterPos3i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3iv = (GLRASTERPOS3IV)GPA("glRasterPos3iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3s = (GLRASTERPOS3S)GPA("glRasterPos3s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos3sv = (GLRASTERPOS3SV)GPA("glRasterPos3sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4d = (GLRASTERPOS4D)GPA("glRasterPos4d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4dv = (GLRASTERPOS4DV)GPA("glRasterPos4dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4f = (GLRASTERPOS4F)GPA("glRasterPos4f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4fv = (GLRASTERPOS4FV)GPA("glRasterPos4fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4i = (GLRASTERPOS4I)GPA("glRasterPos4i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4iv = (GLRASTERPOS4IV)GPA("glRasterPos4iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4s = (GLRASTERPOS4S)GPA("glRasterPos4s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRasterPos4sv = (GLRASTERPOS4SV)GPA("glRasterPos4sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglReadBuffer = (GLREADBUFFER)GPA("glReadBuffer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglReadPixels = (GLREADPIXELS)GPA("glReadPixels"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectd = (GLRECTD)GPA("glRectd"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectdv = (GLRECTDV)GPA("glRectdv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectf = (GLRECTF)GPA("glRectf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectfv = (GLRECTFV)GPA("glRectfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRecti = (GLRECTI)GPA("glRecti"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectiv = (GLRECTIV)GPA("glRectiv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRects = (GLRECTS)GPA("glRects"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRectsv = (GLRECTSV)GPA("glRectsv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRenderMode = (GLRENDERMODE)GPA("glRenderMode"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRotated = (GLROTATED)GPA("glRotated"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglRotatef = (GLROTATEF)GPA("glRotatef"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglScaled = (GLSCALED)GPA("glScaled"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglScalef = (GLSCALEF)GPA("glScalef"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglScissor = (GLSCISSOR)GPA("glScissor"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglSelectBuffer = (GLSELECTBUFFER)GPA("glSelectBuffer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglShadeModel = (GLSHADEMODEL)GPA("glShadeModel"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglStencilFunc = (GLSTENCILFUNC)GPA("glStencilFunc"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglStencilMask = (GLSTENCILMASK)GPA("glStencilMask"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglStencilOp = (GLSTENCILOP)GPA("glStencilOp"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1d = (GLTEXCOORD1D)GPA("glTexCoord1d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1dv = (GLTEXCOORD1DV)GPA("glTexCoord1dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1f = (GLTEXCOORD1F)GPA("glTexCoord1f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1fv = (GLTEXCOORD1FV)GPA("glTexCoord1fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1i = (GLTEXCOORD1I)GPA("glTexCoord1i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1iv = (GLTEXCOORD1IV)GPA("glTexCoord1iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1s = (GLTEXCOORD1S)GPA("glTexCoord1s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord1sv = (GLTEXCOORD1SV)GPA("glTexCoord1sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2d = (GLTEXCOORD2D)GPA("glTexCoord2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2dv = (GLTEXCOORD2DV)GPA("glTexCoord2dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2f = (GLTEXCOORD2F)GPA("glTexCoord2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2fv = (GLTEXCOORD2FV)GPA("glTexCoord2fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2i = (GLTEXCOORD2I)GPA("glTexCoord2i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2iv = (GLTEXCOORD2IV)GPA("glTexCoord2iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2s = (GLTEXCOORD2S)GPA("glTexCoord2s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord2sv = (GLTEXCOORD2SV)GPA("glTexCoord2sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3d = (GLTEXCOORD3D)GPA("glTexCoord3d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3dv = (GLTEXCOORD3DV)GPA("glTexCoord3dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3f = (GLTEXCOORD3F)GPA("glTexCoord3f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3fv = (GLTEXCOORD3FV)GPA("glTexCoord3fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3i = (GLTEXCOORD3I)GPA("glTexCoord3i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3iv = (GLTEXCOORD3IV)GPA("glTexCoord3iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3s = (GLTEXCOORD3S)GPA("glTexCoord3s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord3sv = (GLTEXCOORD3SV)GPA("glTexCoord3sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4d = (GLTEXCOORD4D)GPA("glTexCoord4d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4dv = (GLTEXCOORD4DV)GPA("glTexCoord4dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4f = (GLTEXCOORD4F)GPA("glTexCoord4f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4fv = (GLTEXCOORD4FV)GPA("glTexCoord4fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4i = (GLTEXCOORD4I)GPA("glTexCoord4i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4iv = (GLTEXCOORD4IV)GPA("glTexCoord4iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4s = (GLTEXCOORD4S)GPA("glTexCoord4s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoord4sv = (GLTEXCOORD4SV)GPA("glTexCoord4sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexCoordPointer = (GLTEXCOORDPOINTER)GPA("glTexCoordPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexEnvf = (GLTEXENVF)GPA("glTexEnvf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexEnvfv = (GLTEXENVFV)GPA("glTexEnvfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexEnvi = (GLTEXENVI)GPA("glTexEnvi"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexEnviv = (GLTEXENVIV)GPA("glTexEnviv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGend = (GLTEXGEND)GPA("glTexGend"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGendv = (GLTEXGENDV)GPA("glTexGendv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGenf = (GLTEXGENF)GPA("glTexGenf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGenfv = (GLTEXGENFV)GPA("glTexGenfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGeni = (GLTEXGENI)GPA("glTexGeni"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexGeniv = (GLTEXGENIV)GPA("glTexGeniv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexImage1D = (GLTEXIMAGE1D)GPA("glTexImage1D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexImage2D = (GLTEXIMAGE2D)GPA("glTexImage2D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexParameterf = (GLTEXPARAMETERF)GPA("glTexParameterf"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexParameterfv = (GLTEXPARAMETERFV)GPA("glTexParameterfv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexParameteri = (GLTEXPARAMETERI)GPA("glTexParameteri"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexParameteriv = (GLTEXPARAMETERIV)GPA("glTexParameteriv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexSubImage1D = (GLTEXSUBIMAGE1D)GPA("glTexSubImage1D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTexSubImage2D = (GLTEXSUBIMAGE2D)GPA("glTexSubImage2D"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTranslated = (GLTRANSLATED)GPA("glTranslated"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglTranslatef = (GLTRANSLATEF)GPA("glTranslatef"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2d = (GLVERTEX2D)GPA("glVertex2d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2dv = (GLVERTEX2DV)GPA("glVertex2dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2f = (GLVERTEX2F)GPA("glVertex2f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2fv = (GLVERTEX2FV)GPA("glVertex2fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2i = (GLVERTEX2I)GPA("glVertex2i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2iv = (GLVERTEX2IV)GPA("glVertex2iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2s = (GLVERTEX2S)GPA("glVertex2s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex2sv = (GLVERTEX2SV)GPA("glVertex2sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3d = (GLVERTEX3D)GPA("glVertex3d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3dv = (GLVERTEX3DV)GPA("glVertex3dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3f = (GLVERTEX3F)GPA("glVertex3f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3fv = (GLVERTEX3FV)GPA("glVertex3fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3i = (GLVERTEX3I)GPA("glVertex3i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3iv = (GLVERTEX3IV)GPA("glVertex3iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3s = (GLVERTEX3S)GPA("glVertex3s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex3sv = (GLVERTEX3SV)GPA("glVertex3sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4d = (GLVERTEX4D)GPA("glVertex4d"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4dv = (GLVERTEX4DV)GPA("glVertex4dv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4f = (GLVERTEX4F)GPA("glVertex4f"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4fv = (GLVERTEX4FV)GPA("glVertex4fv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4i = (GLVERTEX4I)GPA("glVertex4i"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4iv = (GLVERTEX4IV)GPA("glVertex4iv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4s = (GLVERTEX4S)GPA("glVertex4s"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertex4sv = (GLVERTEX4SV)GPA("glVertex4sv"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglVertexPointer = (GLVERTEXPOINTER)GPA("glVertexPointer"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglViewport = (GLVIEWPORT)GPA("glViewport"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}

#ifdef _WIN32
	if (!(pfwglCopyContext = (WGLCOPYCONTEXT)GPA("wglCopyContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglCreateContext = (WGLCREATECONTEXT)GPA("wglCreateContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglCreateLayerContext = (WGLCREATELAYERCONTEXT)GPA("wglCreateLayerContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglDeleteContext = (WGLDELETECONTEXT)GPA("wglDeleteContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglDescribeLayerPlane = (WGLDESCRIBELAYERPLANE)GPA("wglDescribeLayerPlane"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglGetCurrentContext = (WGLGETCURRENTCONTEXT)GPA("wglGetCurrentContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglGetCurrentDC = (WGLGETCURRENTDC)GPA("wglGetCurrentDC"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglGetLayerPaletteEntries = (WGLGETLAYERPALETTEENTRIES)GPA("wglGetLayerPaletteEntries"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglGetProcAddress = (WGLGETPROCADDRESS)GPA("wglGetProcAddress"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglMakeCurrent = (WGLMAKECURRENT)GPA("wglMakeCurrent"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglRealizeLayerPalette = (WGLREALIZELAYERPALETTE)GPA("wglRealizeLayerPalette"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglSetLayerPaletteEntries = (WGLSETLAYERPALETTEENTRIES)GPA("wglSetLayerPaletteEntries"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglShareLists = (WGLSHARELISTS)GPA("wglShareLists"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglSwapLayerBuffers = (WGLSWAPLAYERBUFFERS)GPA("wglSwapLayerBuffers"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglUseFontBitmaps = (WGLUSEFONTBITMAPS)GPA("wglUseFontBitmapsA"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglUseFontOutlines = (WGLUSEFONTOUTLINES)GPA("wglUseFontOutlinesA"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglChoosePixelFormat = (WGLCHOOSEPIXELFORMAT)GPA("wglChoosePixelFormat"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglDescribePixelFormat = (WGLDESCRIBEPIXELFORMAT)GPA("wglDescribePixelFormat"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglGetPixelFormat = (WGLGETPIXELFORMAT)GPA("wglGetPixelFormat"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglSetPixelFormat = (WGLSETPIXELFORMAT)GPA("wglSetPixelFormat"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfwglSwapBuffers = (WGLSWAPBUFFERS)GPA("wglSwapBuffers"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
#elif __unix__ || __APPLE__
/* __APPLE__ depends on tentative similar preprocessor macro check also existing
 * in "myopengl.h" */
	if (!(pfglXChooseVisual =
		  (XVisualInfo *(*)(Display *, int, int *))GPA("glXChooseVisual"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglXCreateContext =
		  (GLXContext (*)(Display *, XVisualInfo *, GLXContext, int))GPA("glXCreateContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglXDestroyContext =
		  (void (*)(Display *, GLXContext))GPA("glXDestroyContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglXMakeCurrent =
		  (int (*)(Display *, GLXDrawable, GLXContext))GPA("glXMakeCurrent"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglXCopyContext =
		  (void (*)(Display *, GLXContext, GLXContext, GLuint))GPA("glXCopyContext"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
	if (!(pfglXSwapBuffers =
		  (void (*)(Display *, GLXDrawable))GPA("glXSwapBuffers"))) {
		Com_Printf(" ...failed. \n");
		return 0;
	}
#elif __APPLE__
# warning "Please define interface to the OpenGL library for your platform!"
#else
# error "Please define interface to OpenGL library for your platform!"
#endif /* platform check */

	Com_Printf(" ...succeeded. \n");

	return 1;
}

/* EOF */
