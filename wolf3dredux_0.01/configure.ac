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
if test "x${LIBOBJDIR}" = "x"; then
  # automake implicit rules require the "/" at the end:
  test -z "${LIBOBJDIR}" && export LIBOBJDIR="gnulib_replacement_funcs/"
fi
AC_SUBST([LIBOBJDIR])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

AC_ARG_ENABLE([old-style-build],
              [AS_HELP_STRING([--enable-old-style-build],
                               [also run the pre-automake build rules when
                                building])],
              [DO_WOLF_OLD_BUILD=${enableval}],
              [DO_WOLF_OLD_BULD=no])

AC_MSG_CHECKING([whether to use pre-automake build rules])
if test "x${DO_WOLF_OLD_BUILD}" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AM_CONDITIONAL([WOLF_OLD_BUILD],[test "x${DO_WOLF_OLD_BUILD}" = "xyes"])

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PATH_PROG([GLXINFO],[glxinfo],[:],[${PATH}:/opt/local/bin])
AC_SUBST([GLXINFO])
AC_PATH_PROG([SCAN_BUILD],[scan-build scan-build-mp-3.3],[:],
             [${PATH}:/opt/local/bin:/opt/local/libexec/llvm-3.3/bin:/opt/local/libexec/llvm-3.3/libexec/scan-buid])
AC_SUBST([SCAN_BUILD])
AC_PATH_PROG([LLVM_SYMBOLIZER],
             [llvm-symbolizer llvm-symbolizer-mp-3.3],[:],
             [${PATH}:/opt/local/bin:/opt/local/libexec/llvm-3.3/bin])
AC_SUBST([LLVM_SYMBOLIZER])

# check for clang taken from the emacs configure script:
AC_CACHE_CHECK([whether the compiler is actually clang],[wolf_cv_clang],
  [AC_COMPILE_IFELSE(
     [AC_LANG_PROGRAM([[
	  #ifndef __clang__
	    #error "not clang"
	  #endif /* !__clang__ */
        ]])],
     [wolf_cv_clang=yes],
     [wolf_cv_clang=no])
])
AC_SUBST([wolf_cv_clang])
AM_CONDITIONAL([CLANG_RULES],[test "x${wolf_cv_clang}" = "xyes"])

# libtool
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
# simpler library checks:
AC_CHECK_LIB([c],[printf])
AC_CHECK_LIB([X11],[XCloseDisplay])
# wolf3dredux does not actually use any symbols from `-lXext', so just
# check for `main':
AC_CHECK_LIB([Xext],[main])
AC_CHECK_LIB([Xxf86dga],[XF86DGADirectVideo])
AC_CHECK_LIB([Xxf86vm],[XF86VidModeGetAllModeLines])
LT_LIB_M
AC_CHECK_LIB([m],[sqrt])
AC_SEARCH_LIBS([logerror],[m crlibm log gnu iberty])
AC_CHECK_LIB([dl],[dlerror])
AC_CHECK_LIB([ogg],[ogg_page_eos])
AC_CHECK_LIB([vorbis],[vorbis_analysis])
AC_CHECK_LIB([vorbisenc],[vorbis_encode_ctl])
AC_CHECK_LIB([vorbisfile],[ov_clear])
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

AC_CHECK_HEADERS([alloc.h assert.h bits/libc-lock.h ctype.h dmalloc.h \
                  errno.h fcntl.h float.h libintl.h \
                  libio/iolibio.h libio/libioP.h limits.h \
                  mach/mach.h mach/vm_param.h malloc.h malloc/malloc.h \
                  math.h mmsystem.h setjmp.h signal.h stdarg.h stddef.h \
                  stdio.h sys/gmon.h sys/malloc.h sys/mount.h sys/param.h \
                  sys/socket.h sys/sysctl.h sys/syslimits.h sys/time.h \
                  sys/timeb.h sys/utsname.h sys/vmmeter.h \
                  unix.h wchar.h windef.h windows.h xlocale.h Xutil.h])
AC_CHECK_HEADERS([X11/extensions/xf86dga.h],[],[
  AC_CHECK_HEADERS([X11/extensions/Xxf86dga.h])
])
AC_CHECK_HEADERS([X11/extensions/xf86vmode.h],[],[
  AC_CHECK_HEADERS([X11/extensions/Xxf86vmode.h])
])

# Checks for typedefs, structures, and compiler characteristics.
AC_SYS_LARGEFILE
AC_C_CHAR_UNSIGNED
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_PROTOTYPES
AC_C_RESTRICT
AC_C_VOLATILE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_INTMAX_T
AC_TYPE_UINTMAX_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_STRUCT_TIMEZONE dnl# also checks struct tm
AC_CHECK_TYPE([long long])
AC_CHECK_TYPE([unsigned long long])
AC_CHECK_TYPES([time_t, __darwin_time_t, malloc_zone_t])
AC_CHECK_MEMBERS([struct timeval.tv_sec])
AC_CHECK_SIZEOF([short],[2])
AC_CHECK_ALIGNOF([short])
AC_CHECK_SIZEOF([int],[4])
AC_CHECK_ALIGNOF([int])
AC_CHECK_SIZEOF([unsigned int],[4])
AC_CHECK_ALIGNOF([unsigned int])
AC_CHECK_SIZEOF([long],[8])
AC_CHECK_ALIGNOF([long])
AC_CHECK_SIZEOF([unsigned long],[8])
AC_CHECK_ALIGNOF([unsigned long])
AC_CHECK_DECLS_ONCE([true, false])

# Checks for library functions.
AM_WITH_DMALLOC
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_LSTAT
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([atoi atof dlsym dmalloc_malloc fabs floor ftell fwrite \
                getcwd gettimeofday \
                glXChooseVisual glXCreateContext glXMakeCurrent \
                malloc_good_size malloc_size malloc_create_zone \
                malloc_destroy_zone malloc_default_zone \
                malloc_zone_from_ptr malloc_zone_malloc \
                malloc_zone_calloc malloc_zone_valloc malloc_zone_realloc \
                malloc_zone_memalign malloc_zone_free memchr memcmp \
                memcpy memmove memset mkdir pow posix_memalign putc \
                qsort rmdir signal sqrt sqrtf stpcpy strcat strcpy strdup \
                strerror strlen strncat strncpy strrchr strstr strtok \
                sysconf sysctl sysctlbyname sysctlnametomib uname unlink])
AC_REPLACE_FUNCS([strnlen1])
AC_LIBOBJ([progname])

# Output
AC_CONFIG_FILES([Makefile gnulib_replacement_funcs/Makefile])
AC_OUTPUT
