#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([wolf3dredux],[0.01],[johnnycanuck@users.sourceforge.net])
AC_CONFIG_SRCDIR([client/client.h])
AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef __CONFIG_H__
# define __CONFIG_H__
])
AH_BOTTOM([
#endif /* !__CONFIG_H__ */
])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_LIBOBJ_DIR([gnulib_replacement_funcs])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PATH_PROG([GLXINFO],[glxinfo])
LT_INIT([win32-dll dlopen])

# Checks for libraries.
PKG_CHECK_MODULES([MESA],[
# 3.4.1 is the first version in the timeline on mesa3d.org to fully conform
# to semantic versioning; not sure which version is actually needed:
gl > 3.4.1
glu > 3.4.1
osmesa > 3.4.1
vg > 3.4.1
],[
CFLAGS="${CFLAGS} ${MESA_CFLAGS}"
CPPFLAGS="${CPPFLAGS} ${MESA_CFLAGS}"
LDFLAGS="${LDFLAGS} ${MESA_LIBS}"
LIBS="${LIBS} ${MESA_LIBS}"
],[
AC_MSG_WARN([mesa not found; if your system uses an alternate OpenGL interface, it will require manual setup])
])

AC_CHECK_LIB([c],[printf])
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11],[main])
# FIXME: Replace `main' with a function in `-lXext':
AC_CHECK_LIB([Xext],[main])
# FIXME: Replace `main' with a function in `-lXxf86dga':
AC_CHECK_LIB([Xxf86dga],[main])
# FIXME: Replace `main' with a function in `-lXxf86vm':
AC_CHECK_LIB([Xxf86vm],[main])
LT_LIB_M
AC_CHECK_LIB([m],[sqrt])
AC_CHECK_LIB([dl],[dlerror])
# FIXME: Replace `main' with a function in `-logg':
AC_CHECK_LIB([ogg],[main])
# FIXME: Replace `main' with a function in `-lvorbis':
AC_CHECK_LIB([vorbis],[main])
# FIXME: Replace `main' with a function in `-lvorbisenc':
AC_CHECK_LIB([vorbisenc],[main])
# FIXME: Replace `main' with a function in `-lvorbisfile':
AC_CHECK_LIB([vorbisfile],[main])
# Official vorbis macro:
XIPH_PATH_VORBIS

# Checks for header files.
AC_HEADER_STAT
AC_HEADER_STDBOOL dnl# also "_CHECK"s it
AC_HEADER_TIME
AC_PATH_X
AC_PATH_XTRA
m4_foreach_w([my_GL_header],[gl.h glu.h glext.h glx.h],[
  # The "<GL/>" directory is where they should be, so check that first:
  AC_CHECK_HEADERS([GL/my_GL_header],[],[
    # If that fails, try without the "<GL/>" prefix:
    # (for cases like using OpenGL.framework on OSX)
    AC_CHECK_HEADERS([my_GL_header],[],[])
  ])
])
# Keep autoheader happy by making sure the proper templates are added:
if test "x" = "y"; then
  test -x `which autoheader`
  AC_CHECK_HEADERS_ONCE([gl.h glu.h glext.h glx.h \
                         GL/gl.h GL/glu.h GL/glext.h GL/glx.h])
  # Keep autoscan happy as well:
  test -x `which autoscan`
  AC_PROG_RANLIB
fi

AC_CHECK_HEADERS([assert.h bits/libc-lock.h ctype.h errno.h fcntl.h \
                  float.h libintl.h libio/iolibio.h libio/libioP.h \
                  limits.h math.h mmsystem.h setjmp.h stdarg.h stddef.h \
                  stdio.h sys/param.h sys/sysctl.h sys/time.h sys/timeb.h \
                  sys/utsname.h wchar.h windows.h xlocale.h Xutil.h])
AC_CHECK_HEADERS([X11/extensions/xf86dga.h],[],[
  AC_CHECK_HEADERS([X11/extensions/Xxf86dga.h])
])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CHAR_UNSIGNED
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_PROTOTYPES
AC_C_VOLATILE
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_INTMAX_T
AC_TYPE_UINTMAX_T
AC_CHECK_TYPE([long long])
AC_CHECK_TYPE([unsigned long long])
AC_CHECK_SIZEOF([unsigned int])
AC_CHECK_ALIGNOF([unsigned int])

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([atoi dlsym fabs floor fwrite getcwd gettimeofday \
                glXChooseVisual glXCreateContext glXMakeCurrent \
                memchr memcmp memcpy memmove memset mkdir pow putc qsort \
                rmdir sqrt sqrtf stpcpy strcat strcpy strdup strerror \
                strlen strncat strncpy strrchr strstr strtok \
                uname unlink])
AC_REPLACE_FUNCS([strnlen1])
AC_LIBOBJ([progname])

# Output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
