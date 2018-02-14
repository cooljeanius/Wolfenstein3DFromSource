dnl#                                               -*- Autoconf -*-
dnl# Process this file with autoconf to produce a configure script.

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
export LIBOBJDIRNOSLASH="gnulib_replacement_funcs"
AC_SUBST([LIBOBJDIR])dnl
AC_SUBST([LIBOBJDIRNOSLASH])dnl
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS

dnl# automake
AC_MSG_CHECKING([for some automake things...])
AC_MSG_RESULT([... ])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

dnl# our own additional configure flags:
AC_ARG_ENABLE([old-style-build],
              [AS_HELP_STRING([--enable-old-style-build],
                               [also run the pre-automake build rules when
                                building])],
              [DO_WOLF_OLD_BUILD=${enableval}],
              [DO_WOLF_OLD_BUILD=no])dnl

AC_MSG_CHECKING([whether to use pre-automake build rules])
if test "x${DO_WOLF_OLD_BUILD}" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  export DO_WOLF_OLD_BUILD=no
  AC_MSG_RESULT([no])
fi

AM_CONDITIONAL([WOLF_OLD_BUILD],[test "x${DO_WOLF_OLD_BUILD}" = "xyes"])dnl

AC_ARG_ENABLE([ms-compat],
              [AS_HELP_STRING([--enable-ms-compat],
                               [try to check for compatibility with
                                Microsoft Windows
                                (requires a new enough version of clang
                                and an installation of Wine)])],
              [CHECK_MS_COMPAT=${enableval}],
              [CHECK_MS_COMPAT=no])dnl

dnl# the actual checks associated with this flag have moved down below;
dnl# this just stays up here to keep the flags together.

# Checks for programs.
AC_MSG_CHECKING([for some programs...])
AC_MSG_RESULT([... ])
AM_PROG_AR
AM_PROG_AS
AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_GCC_TRADITIONAL
AC_PROG_OBJC
AC_SUBST([ac_cv_objc_compiler_gnu])
dnl# only do test for Objective C things if the OBJC test was successful:
if test "x${ac_cv_objc_compiler_gnu}" = "xyes"; then
  test -x `which cpp`
  AC_PROG_OBJCPP
  AC_CHECK_HEADERS([objc/objc.h])
  AC_SUBST([ac_cv_header_objc_objc_h])
fi
AC_MSG_CHECKING([whether we can build objc sources])
if test "x${ac_cv_objc_compiler_gnu}" = "xyes" -a "x${OBJCPP}" != "x" -a "x${ac_cv_header_objc_objc_h}" = "xyes"; then
  export CAN_BUILD_OBJC=yes
  AC_SUBST([CAN_BUILD_OBJC])
  AC_MSG_RESULT([${CAN_BUILD_OBJC}])
else
  export CAN_BUILD_OBJC=no
  AC_MSG_RESULT([${CAN_BUILD_OBJC}])
fi
AM_CONDITIONAL([BUILD_OBJC_SOURCES],[test "x${CAN_BUILD_OBJC}" = "xyes"])
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PATH_PROG([GLXINFO],[glxinfo],[:],[${PATH}:/opt/local/bin])
AC_SUBST([GLXINFO])dnl
AC_PATH_PROG([SCAN_BUILD],[scan-build scan-build-mp-3.4 scan-build-mp-3.3],[:],
             [${PATH}:/opt/local/bin:/opt/local/libexec/llvm-3.3/bin:/opt/local/libexec/llvm-3.3/libexec/scan-buid])
AC_SUBST([SCAN_BUILD])dnl
AC_PATH_PROG([LLVM_SYMBOLIZER],
             [llvm-symbolizer llvm-symbolizer-mp-3.3],[:],
             [${PATH}:/opt/local/bin:/opt/local/libexec/llvm-3.3/bin])
AC_SUBST([LLVM_SYMBOLIZER])dnl

m4_ifdef([AM_BINUTILS_CLANG_STATIC_ANALYSIS],[
AM_BINUTILS_CLANG_STATIC_ANALYSIS
])dnl

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
AM_CONDITIONAL([CLANG_RULES],[test "x${wolf_cv_clang}" = "xyes"])dnl


AC_MSG_CHECKING([whether to try to build sources for Microsoft Windows])
if test "x${CHECK_MS_COMPAT}" = "xyes"; then
  AC_MSG_RESULT([${CHECK_MS_COMPAT}])
  export CAN_CHECK_MS_COMPAT=${CHECK_MS_COMPAT}
  if test "x${wolf_cv_clang}" != "xyes"; then
    export CAN_CHECK_MS_COMPAT=no
    AC_MSG_NOTICE([only clang can successfully fake being MSVC so far])
    AC_SUBST([CAN_CHECK_MS_COMPAT])
  fi
  AH_TEMPLATE([HAVE_MMSYSTEM_H],
              [Define to 1 if you have the <mmsystem.h> header file.])
  AH_TEMPLATE([HAVE_WINDEF_H],
              [Define to 1 if you have the <windef.h> header file.])
  AH_TEMPLATE([HAVE_WINDOWS_H],
              [Define to 1 if you have the <windows.h> header file.])
  m4_foreach_w([my_MS_header],[math.h mmsystem.h windef.h windows.h],[
    # First just check un-prefixed:
    AC_CHECK_HEADERS([my_MS_header],[],[
      # Now try with "wine":
      AC_CHECK_HEADERS([wine/my_MS_header],[],[
        # Now try with "wine/windows":
        AC_CHECK_HEADERS([wine/windows/my_MS_header],[],[
          # Now try with "wine/msvcrt":
          AC_CHECK_HEADERS([wine/msvcrt/my_MS_header],[],[
            # Now try with just "windows":
            AC_CHECK_HEADERS([windows/my_MS_header],[],[
              # Now try with just "msvcrt":
              AC_CHECK_HEADERS([msvcrt/my_MS_header],[],[
                # If we really still cannot find it, fail:
                break && export CAN_CHECK_MS_COMPAT=no
              ],[AC_INCLUDES_DEFAULT
              ])
            ],[AC_INCLUDES_DEFAULT
            ])
          ],[AC_INCLUDES_DEFAULT
          ])
        ],[AC_INCLUDES_DEFAULT
        ])
      ],[AC_INCLUDES_DEFAULT
      ])
    ],[AC_INCLUDES_DEFAULT
    ])
  ])
  AC_LIBOBJ([msvc-inval])
  AC_LIBOBJ([msvc-nothrow])
else
  export CAN_CHECK_MS_COMPAT=no
  AC_MSG_RESULT([${CAN_CHECK_MS_COMPAT}])
fi

AM_CONDITIONAL([MS_COMPAT_CHECK],[test "x${CAN_CHECK_MS_COMPAT}" = "xyes"])dnl

# libtool
AC_MSG_CHECKING([for some libtool things...])
AC_MSG_RESULT([... ])
LT_INIT([win32-dll dlopen])
LT_LANG([C])
if test "x${ac_cv_prog_ac_ct_DSYMUTIL}" = "x"; then
  test -z "${DSYMUTIL}"
  AC_PATH_PROG([DSYMUTIL],[dsymutil],[:],[${PATH}:/Developer/usr/bin])
  AC_SUBST([DSYMUTIL])
fi

# Checks for libraries.
PKG_CHECK_MODULES([MESA],[
dnl# 3.4.1 is the 1st version in the timeline on mesa3d.org to fully
dnl# conform to semver; not sure which version is actually needed:
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
# X11 checks
AC_PATH_X
AC_SUBST([with_x])dnl
AC_SUBST([have_x])dnl
AC_SUBST([ac_cv_have_x])dnl
AC_SUBST([no_x])dnl
AC_SUBST([x_includes])dnl
AC_SUBST([x_libraries])dnl
AC_SUBST([ac_x_includes])dnl
AC_SUBST([ac_x_libraries])dnl
AC_MSG_CHECKING([if we can actually build against X11])
if test "x${no_x}" != "xyes" -a "x${have_x}" = "xyes" -a "x${with_x}" != "xno"; then
  export CAN_BUILD_AGAINST_X11=yes
  AC_SUBST([CAN_BUILD_AGAINST_X11])
  AC_MSG_RESULT([${CAN_BUILD_AGAINST_X11}])
  AC_PATH_XTRA
  AC_MSG_CHECKING([for some useful but unnecessary X11 headers...])
  AC_MSG_RESULT([... ])
  AC_CHECK_HEADERS([Xlib.h Xutil.h X.h X11/X.h X11/Xfuncproto.h X11/Xmd.h \
                    X11/extensions/xf86vm.h])
  AC_MSG_CHECKING([for the X11 headers that we actually need...])
  AC_MSG_RESULT([... ])
  AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h X11/keysym.h \
                    X11/Xresource.h X11/cursorfont.h],[],[
     break && export CAN_BUILD_AGAINST_X11=no && export CAUSE_OF_X11_FAILURE="missing/bad headers"])
  # need this for the next few conftests:
  export ac_xeleven_includes_default="\
#ifdef HAVE_X_H
# include <X.h>
#else
# ifdef HAVE_X11_X_H
#  include <X11/X.h>
# else
#  if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#   warning this conftest is missing an X header.
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* HAVE_X11_X_H */
#endif /* HAVE_X_H */
#ifdef HAVE_XLIB_H
# include <Xlib.h>
#else
# ifdef HAVE_X11_XLIB_H
#  include <X11/Xlib.h>
# else
#  if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#   warning this conftest is missing an Xlib header.
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* HAVE_X11_XLIB_H */
#endif /* HAVE_XLIB_H */
#ifdef HAVE_X11_XFUNCPROTO_H
# include <X11/Xfuncproto.h>
#endif /* HAVE_X11_XFUNCPROTO_H */"
dnl# (I wanted to use m4_divert_text like autoconf does for its default
dnl# includes, but that led to errors...)
  AC_MSG_CHECKING([for X11 headers that require special treatment...])
  AC_MSG_RESULT([... ])
  AC_CHECK_HEADERS([X11/extensions/xf86dga.h],[],[
    AC_CHECK_HEADERS([X11/extensions/xf86dgaconst.h],[],[
      AC_CHECK_HEADERS([X11/extensions/xf86dga1.h],[],[
        AC_CHECK_HEADERS([X11/extensions/Xxf86dga.h],[],[
          export CAN_BUILD_AGAINST_X11=no
          export CAUSE_OF_X11_FAILURE="missing/bad headers"
        ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
        ])
      ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
      ])
    ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
    ])
  ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
  ])dnl
  AC_CHECK_HEADERS([X11/extensions/xf86vmode.h],[],[
    AC_CHECK_HEADERS([X11/extensions/Xxf86vmode.h],[],[
      export CAN_BUILD_AGAINST_X11=no
      export CAUSE_OF_X11_FAILURE="missing/bad headers"
    ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
    ])
  ],[AC_INCLUDES_DEFAULT
${ac_xeleven_includes_default}
  ])dnl
  AC_MSG_CHECKING([for X11 libraries that we would like to link with...])
  AC_MSG_RESULT([... ])
  AC_CHECK_LIB([X11],[XCloseDisplay],[],[
    export CAN_BUILD_AGAINST_X11=no
    if test -z "${CAUSE_OF_X11_FAILURE}"; then
      export CAUSE_OF_X11_FAILURE="missing/bad libX11"
    else
      export CAUSE_OF_X11_FAILURE="${CAUSE_OF_X11_FAILURE}, and missing/bad libX11"
    fi
  ])dnl
  # wolf3dredux does not actually use any symbols from `-lXext', so just
  # check for `main', and skip the failure condition:
  AC_CHECK_LIB([Xext],[main],[],[:])
  AC_CHECK_LIB([Xxf86dga],[XF86DGADirectVideo],[],[
    export CAN_BUILD_AGAINST_X11=no
    if test -z "${CAUSE_OF_X11_FAILURE}"; then
      export CAUSE_OF_X11_FAILURE="missing/bad libXxf86dga"
    else
      export CAUSE_OF_X11_FAILURE="${CAUSE_OF_X11_FAILURE}, and missing/bad libXxf86dga"
    fi
  ])dnl
  AC_CHECK_LIB([Xxf86vm],[XF86VidModeGetAllModeLines],[],[
    export CAN_BUILD_AGAINST_X11=no
    if test -z "${CAUSE_OF_X11_FAILURE}"; then
      export CAUSE_OF_X11_FAILURE="missing/bad libXxf86vm"
    else
      export CAUSE_OF_X11_FAILURE="${CAUSE_OF_X11_FAILURE}, and missing/bad libXxf86vm"
    fi
  ])dnl
  AC_MSG_CHECKING([for all of the X11 functions that we need...])
  AC_MSG_RESULT([... ])
  # these are all the functions in XSyms.txt:
  AC_CHECK_FUNCS([XF86VidModeGetAllModeLines XF86VidModeQueryVersion \
                  XF86VidModeSetViewPort XF86VidModeSwitchToMode \
                  XF86DGADirectVideo XF86DGAQueryVersion XCloseDisplay \
                  XCreateColormap XCreateGC XCreatePixmap \
                  XCreatePixmapCursor XCreateWindow XDefineCursor \
                  XDestroyWindow XFillRectangle XFlush XFreeGC \
                  XFreePixmap XGrabKeyboard XGrabPointer XLookupString \
                  XMapWindow XMoveWindow XNextEvent XOpenDisplay XPending \
                  XRaiseWindow XSelectInput XSetStandardProperties \
                  XUndefineCursor XUngrabKeyboard XUngrabPointer \
                  XWarpPointer],[],[
    break && export CAN_BUILD_AGAINST_X11=no && export CAUSE_OF_X11_FAILURE="missing required functions"])
  AC_MSG_CHECKING([if it is still true that we can actually build against X11])
  if test -z "${CAUSE_OF_X11_FAILURE}"; then
    AC_MSG_RESULT([${CAN_BUILD_AGAINST_X11}])
  else
    AC_MSG_RESULT([${CAN_BUILD_AGAINST_X11}, because of ${CAUSE_OF_X11_FAILURE}])
  fi
else
  export CAN_BUILD_AGAINST_X11=no
  AC_MSG_RESULT([${CAN_BUILD_AGAINST_X11}])
fi
AM_CONDITIONAL([BUILD_X11_SOURCES],
               [test "x${CAN_BUILD_AGAINST_X11}" = "xyes"])dnl

# simpler library checks:
AC_MSG_CHECKING([for some other libraries...])
AC_MSG_RESULT([... ])
AC_CHECK_LIB([c],[printf])
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
AC_MSG_CHECKING([to see which OpenGL headers we can use...])
AC_MSG_RESULT([... ])
# need this for the next few conftests:
export ac_gl_includes_default="\
#ifdef HAVE_GL_GL_H
# include <GL/gl.h>
#else
# ifdef HAVE_GL_H
#  include <gl.h>
# else
#  ifdef HAVE_OPENGL_GL_H
#   include <OpenGL/gl.h>
#  endif /* HAVE_OPENGL_GL_H */
# endif /* HAVE_GL_H */
#endif /* HAVE_GL_GL_H */"
m4_foreach_w([my_GL_header],[gl.h glu.h glext.h glx.h],[
  # The "<GL/>" directory is where they should be, so check that first:
  AC_CHECK_HEADERS([GL/my_GL_header],[],[
    # If that fails, try without the "<GL/>" prefix:
    # (for cases like when the Headers directory in OpenGL.framework on
    # OSX is put directly into the include path)
    AC_CHECK_HEADERS([my_GL_header],[],[
      # If that fails, try with the "<OpenGL/>" prefix:
      # (which is the normal way to grab headers from OpenGL.framework
      # on OSX)
      AC_CHECK_HEADERS([OpenGL/my_GL_header],[],[],[AC_INCLUDES_DEFAULT
${ac_gl_includes_default}
      ])
    ],[AC_INCLUDES_DEFAULT
${ac_gl_includes_default}
    ])
  ],[AC_INCLUDES_DEFAULT
${ac_gl_includes_default}
  ])
])
# Keep autoheader happy by making sure the proper templates are added:
if test "x0" = "Y1"; then
  test -x `which autoheader`
  unset ac_cv_header_gl_h
  unset ac_cv_header_glu_h
  unset ac_cv_header_glext_h
  unset ac_cv_header_glx_h
  unset ac_cv_header_GL_gl_h
  unset ac_cv_header_GL_glu_h
  unset ac_cv_header_GL_glext_h
  unset ac_cv_header_GL_glx_h
  unset ac_cv_header_OpenGL_gl_h
  unset ac_cv_header_OpenGL_glu_h
  unset ac_cv_header_OpenGL_glext_h
  unset ac_cv_header_OpenGL_glx_h
  dnl# using "_ONCE" makes these checks break out of this conditional,
  dnl# which means that they are already cached by the time the actual
  dnl# tests for them are run above, so instead just use the normal macro
  dnl# to keep it inside this conditional so they will never actually be
  dnl# checked again:
  AC_CHECK_HEADERS([gl.h glu.h glext.h glx.h \
                    GL/gl.h GL/glu.h GL/glext.h GL/glx.h \
                    OpenGL/gl.h OpenGL/glu.h OpenGL/glext.h OpenGL/glx.h])
  # Keep autoscan happy as well:
  test -x "`which autoscan`"
  AC_PROG_RANLIB
fi
dnl# okay, now that that is out of the way...
AC_MSG_CHECKING([for miscellaneous other headers...])
AC_MSG_RESULT([... ])
AC_HEADER_DIRENT
AC_HEADER_STAT
AC_HEADER_STDBOOL dnl# also "_CHECK"s it
AC_HEADER_TIME
dnl# having libtool check dlopen also has it check for dlfcn.h, so no
dnl# need to check for it here as well...
unset ac_cv_header_stddef_h
unset ac_cv_header_sys_param_h
unset ac_cv_header_math_h
AC_CHECK_HEADERS([alloc.h assert.h bits/libc-lock.h crtdefs.h ctype.h \
                  dmalloc.h endian.h errno.h excpt.h fcntl.h float.h \
                  io.h libintl.h libio/iolibio.h libio/libioP.h \
                  limits.h locale.h mach/mach.h mach/vm_param.h \
                  malloc.h malloc/malloc.h math.h memcopy.h search.h \
                  setjmp.h signal.h stdarg.h stddef.h stdio.h \
                  sys/gmon.h sys/malloc.h sys/mount.h sys/param.h \
                  sys/socket.h sys/sysctl.h sys/syslimits.h sys/time.h \
                  sys/timeb.h sys/uio.h sys/utsname.h sys/vmmeter.h \
                  unix.h wchar.h xlocale.h])dnl

# Checks for typedefs, structures, and compiler characteristics.
AC_MSG_CHECKING([into the characteristics of your C compiler...])
AC_MSG_RESULT([... ])
AC_SYS_LARGEFILE
AC_C_CHAR_UNSIGNED
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
gl_EXTERN_INLINE
AC_C_PROTOTYPES
AC_C_RESTRICT
AC_C_VOLATILE
gl_UNKNOWN_WARNINGS_ARE_ERRORS
gl_WARN_ADD([-Wall])
gl_WARN_ADD([-Wmissing-field-initializers])
gl_WARN_ADD([-Wmissing-prototypes])
gl_WARN_ADD([-Wmissing-declarations])
if test "x${PRINT_LOTS_OF_DECLARATION_WARNINGS}" = "xyes"; then
  gl_WARN_ADD([-Wmissing-variable-declarations])
  gl_WARN_ADD([-Wredundant-decls])
fi
gl_WARN_ADD([-Wunneeded-internal-declaration])
gl_WARN_ADD([-Wgnu-redeclared-enum])
gl_WARN_ADD([-Wold-style-declaration])
gl_WARN_ADD([-Wnested-externs])
gl_WARN_ADD([-Wmissing-parameter-type])
gl_WARN_ADD([-Wimplicit])
gl_WARN_ADD([-Wreturn-type])
gl_WARN_ADD([-Wmissing-braces])
gl_WARN_ADD([-Wparentheses])
gl_WARN_ADD([-Wswitch])
gl_WARN_ADD([-Wmissing-noreturn])
gl_WARN_ADD([-Wnull-dereference])
gl_WARN_ADD([-Walloc-zero])
gl_WARN_ADD([-Wmisleading-indentation])
gl_WARN_ADD([-Wgcc-compat])
gl_WARN_ADD([-Wpointer-arith])
gl_WARN_ADD([-Wunreachable-code-loop-increment])
gl_WARN_ADD([-Wunreachable-code-break])
gl_WARN_ADD([-Wunreachable-code-return])
gl_WARN_ADD([-Wreturn-local-addr])
gl_COMPILER_OPTION_IF([-Warray-bounds=2],
                      [WARN_CFLAGS="${WARN_CFLAGS} -Warray-bounds=2"],
                      [gl_WARN_ADD([-Warray-bounds])])
gl_WARN_ADD([-Wno-unused])
gl_WARN_ADD([-Wno-trigraphs])
gl_WARN_ADD([-Wsign-compare],[EXTRA_WARN_CFLAGS])
gl_WARN_ADD([-Wunknown-pragmas],[EXTRA_WARN_CFLAGS])
AC_MSG_CHECKING([for some types...])
AC_MSG_RESULT([... ])
dnl# begin what used to be AC\_TYPE\_SIGNAL
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],
               [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <signal.h>
               ]],[[
return *(signal (0, 0)) (0) == 1;
               ]])],[ac_cv_type_signal=int],[ac_cv_type_signal=void])
])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[${ac_cv_type_signal}],
         [Define as the return type of signal handlers (`int' or `void').])
dnl# end what used to be AC\_TYPE\_SIGNAL
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INTMAX_T
AC_TYPE_INTPTR_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINTMAX_T
AC_TYPE_UINTPTR_T
gt_TYPE_WCHAR_T
AC_CHECK_TYPE([long long])
AC_CHECK_TYPE([unsigned long long])
AC_CHECK_TYPES([time_t, __darwin_time_t, malloc_zone_t])
AC_CHECK_TYPES([ptrdiff_t])
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
AC_MSG_CHECKING([for a couple of declarations...])
AC_MSG_RESULT([... ])
AC_CHECK_DECLS_ONCE([true, false])
AC_MSG_CHECKING([for some structs and struct members...])
AC_MSG_RESULT([... ])
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_STRUCT_TIMEZONE dnl# also checks struct tm
AC_CHECK_MEMBERS([struct timeval.tv_sec])dnl

# Checks for library functions.
AC_MSG_CHECKING([for some miscellaneous functions...])
AC_MSG_RESULT([... ])
AC_FUNC_ALLOCA
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_LSTAT
AC_FUNC_MALLOC
if test "x${ac_cv_func_malloc_0_nonnull}" = "xyes"; then
  test ! -z "${ac_cv_func_malloc_0_nonnull}"
  AM_WITH_DMALLOC
fi
AC_FUNC_MBRTOWC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([arc4random atoi atof dlsym dmalloc_malloc fabs fclose floor \
                fopen ftell fscanf fread fwrite getcwd getenv gettimeofday \
                glXChooseVisual glXCreateContext glXMakeCurrent \
                longjmp malloc_good_size malloc_size malloc_create_zone \
                malloc_destroy_zone malloc_default_zone \
                malloc_zone_from_ptr malloc_zone_malloc \
                malloc_zone_calloc malloc_zone_valloc malloc_zone_realloc \
                malloc_zone_memalign malloc_zone_free memchr memcmp \
                memcpy memmove memset mkdir pow posix_memalign putc pread \
                qsort rand read rmdir scanf setenv signal sqrt sqrtf stpcpy \
                strcat strcpy strdup strerror strlen strncat strncpy \
                strrchr strstr strtok sscanf sysconf sysctl sysctlbyname \
                sysctlnametomib uname unlink])
AC_REPLACE_FUNCS([strnlen1])
AC_LIBOBJ([progname])dnl

for my_libobj in `eval echo ${LIBOBJS}`; do
  export JUST_LIBOBJS="${JUST_LIBOBJS} `echo ${my_libobj} | cut -d\/ -f2`"
done
AC_SUBST([JUST_LIBOBJS])dnl

# Output
AC_CONFIG_FILES([Makefile \
gnulib_replacement_funcs/Makefile \
tools/wolfextractor/Makefile])
AC_OUTPUT

echo ""
echo "Building with the following characteristics:"
echo ""
echo "compiler: ${CC}"
echo "compiler flags: ${CFLAGS}"
echo "preprocessor flags: ${CPPFLAGS}"
echo "linker flags: ${LDFLAGS}"
echo "libraries to link against: ${LIBS}"
echo "old-style build: ${DO_WOLF_OLD_BUILD}"
echo "build Microsoft sources: ${CHECK_MS_COMPAT}"
echo "build Objective C sources: ${CAN_BUILD_OBJC}"
echo "enable clang analyzer target: ${wolf_cv_clang}"
if test -z "${CAUSE_OF_X11_FAILURE}"; then
  echo "build X11 sources: ${CAN_BUILD_AGAINST_X11}"
else
  echo "build X11 sources: ${CAN_BUILD_AGAINST_X11}, because of ${CAUSE_OF_X11_FAILURE}"
fi
echo "objects to build for replacement functions: ${JUST_LIBOBJS}"
echo ""
