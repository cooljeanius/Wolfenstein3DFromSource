dnl# Common configure.ac fragment

# AC_EGREP_CPP_FOR_BUILD(PATTERN, PROGRAM,
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------
AC_DEFUN([AC_EGREP_CPP_FOR_BUILD],
[AC_LANG_PREPROC_REQUIRE()dnl
AC_REQUIRE([AC_PROG_EGREP])dnl
AC_LANG_CONFTEST([AC_LANG_SOURCE([[$2]])])
AS_IF([dnl# eval is necessary to expand ac_cpp_for_build.
dnl# Ultrix/Pyramid sh refuse to redirect output of eval, so use subshell.
(eval "$ac_cpp_for_build conftest.$ac_ext") 2>&AS_MESSAGE_LOG_FD |
dnl# Quote $1 to prevent m4 from eating character classes
  $EGREP "[$1]" >/dev/null 2>&1],
  [$3],
  [$4])
rm -f conftest*
])dnl# AC_EGREP_CPP_FOR_BUILD

AC_DEFUN([AM_BINUTILS_WARNINGS],[
# Set acp_cpp_for_build variable
ac_cpp_for_build="${CC_FOR_BUILD} -E ${CPPFLAGS_FOR_BUILD}"

# "-W" and "-Wextra" are redundant.
WARN_CFLAGS="-Wall -Wstrict-prototypes -Wmissing-prototypes \
-Wmissing-declarations -Wimplicit -Wparentheses -Wextra -Wc++-compat \
-Wgcc-compat -Wasm -Wdangling-else -Wused-but-marked-unused \
-Wundef -Wold-style-declaration -Wold-style-definition -Wnested-externs \
-Wmissing-parameter-type -Wabi -Wmissing-include-dirs \
-Wmisleading-indentation -Wformat=2 -Wmissing-format-attribute \
-Wswitch -Wswitch-default -Wpacked -Wnull-dereference -Whsa \
-Wduplicate-decl-specifier -Wmemset-elt-size -Wswitch-unreachable \
-Wscalar-storage-order -Wrestrict -Wimplicit-fallthrough \
-Walloca-larger-than=4032 -Wvla-larger-than=4032 -Wformat-overflow=2 \
-Wformat-truncation=2 -Wstringop-overflow=2"
# (4032 is MAX_ALLOCA_SIZE in "libiberty.h")
if test "x${WANT_CONVERSION_WARNS}" = "x1"; then
  test -n "${WANT_CONVERSION_WARNS}"
  # "-Wconversion" and friends are because of a comment in libbfd.c
  WARN_CFLAGS="${WARN_CFLAGS} -Wnon-gcc -Wconversion -Wfloat-conversion \
  -Wsign-conversion -Wsign-compare -Wshorten-64-to-32 -Wdouble-promotion"
fi
WARN_DEFS="-D_FORTIFY_SOURCE=2 -Dlint -DDEBUG_GEN_RELOC -DDEBUG_HASH \
-DDEBUG_PIC -DDEBUG_RELAX -DDEBUG_RELOC -DDJDEBUG -DFATAL_DEBUG \
-DLINUX_LINK_DEBUG -DVMS_DEBUG=1 \
-DBFD_AOUT_DEBUG -DBFD_TRACK_OPEN_CLOSE"
WARN_LDFLAGS=""
GCC_WARN_CFLAGS_FOR_BUILD="-Wall -Wextra -Wstrict-prototypes \
-Wmissing-prototypes"

# Add -Wshadow if the compiler is a sufficiently recent version of GCC.
AC_EGREP_CPP([^[0-3]$],[__GNUC__],[],
             [WARN_CFLAGS="${WARN_CFLAGS} -Wshadow"])dnl

# Add -Wstack-usage if the compiler is a sufficiently recent version of GCC
AC_EGREP_CPP([^[0-4]$],[__GNUC__],[],
             [WARN_CFLAGS="${WARN_CFLAGS} -Wstack-usage=262144"])dnl

# Verify CC_FOR_BUILD to be compatible with warning flags

# Add -Wshadow if the compiler is a sufficiently recent version of GCC.
AC_EGREP_CPP_FOR_BUILD([^[0-3]$],[__GNUC__],[],
[GCC_WARN_CFLAGS_FOR_BUILD="${GCC_WARN_CFLAGS_FOR_BUILD} -Wshadow"])dnl

if test "x${GCC_WARN_CFLAGS}" != "x"; then
  if test "x${GCC}" = "xyes"; then
    test -n "${GCC}" && test -n "${GCC_WARN_CFLAGS}" && export WARN_CFLAGS="${WARN_CFLAGS} ${GCC_WARN_CFLAGS}"
  fi
fi

# Set WARN_WRITE_STRINGS if the compiler supports -Wwrite-strings.
WARN_WRITE_STRINGS=""
AC_EGREP_CPP([^[0-3]$],[__GNUC__],[],
             [WARN_WRITE_STRINGS="-Wwrite-strings"])dnl

AC_REQUIRE([AC_CANONICAL_HOST])dnl

if test "x${acl_cv_wl}" = "x"; then
  test -z "${acl_cv_wl}" && export acl_cv_wl='-Wl,'
fi
AC_SUBST([acl_cv_wl])dnl

case "${host}" in
  *-apple-darwin* | *-apple-macos*)
    # -Wmost is APPLE_LOCAL, as are -Wextra-tokens and -Wnewline-eof:
    WARN_CFLAGS="${WARN_CFLAGS} -Wmost -Wextra-tokens -Wnewline-eof"
    WARN_LDFLAGS="${WARN_LDFLAGS} ${acl_cv_wl}-warn_compact_unwind"
    WARN_LDFLAGS="${WARN_LDFLAGS} ${acl_cv_wl}-warn_weak_exports"
    WARN_LDFLAGS="${WARN_LDFLAGS} ${acl_cv_wl}-warn_stabs"
    WARN_LDFLAGS="${WARN_LDFLAGS} ${acl_cv_wl}-warn_commons"
  ;;
esac

# GCC supports -Wuninitialized only with -O or -On, n != 0.
# ...and also -Winit-self only works when -Wuninitialized is also used.
# -Wmissing-noreturn requires -O or higher, or -fipa-pure-const to work.
# Others in this section are technically supported at other optimization
# levels, but really only make sense with it on.
opt_warnings="-Wuninitialized -Winit-self -Wmissing-noreturn \
-Wvolatile-register-var -Wdisabled-optimization \
-Waggressive-loop-optimizations -Wvector-operation-performance \
-Wmaybe-uninitialized -Wunsafe-loop-optimizations"
if test x${CFLAGS+set} = xset; then
  case "${CFLAGS}" in
    *"-O0"* ) ;;
    *"-O"* )
      WARN_CFLAGS="${WARN_CFLAGS} ${opt_warnings}"
    ;;
  esac
else
  WARN_CFLAGS="${WARN_CFLAGS} ${opt_warnings}"
fi

AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([--enable-werror],[treat compile warnings as errors])],
  [case "${enableval}" in
     yes | y) ERROR_ON_WARNING="yes" ;;
     no | n)  ERROR_ON_WARNING="no" ;;
     *) AC_MSG_ERROR([bad value ${enableval} for --enable-werror]) ;;
   esac])dnl

AC_ARG_ENABLE([pedantic],
  [AS_HELP_STRING([--enable-pedantic],[enable pedantic warnings])],
  [case "${enableval}" in
     yes | y) PEDANTIC_WARNINGS="yes" ;;
     no | n)  PEDANTIC_WARNINGS="no" ;;
     *) AC_MSG_ERROR([bad value ${enableval} for --enable-pedantic]) ;;
   esac])dnl

AC_REQUIRE([AC_PROG_CC])dnl
# Enable -Werror by default when using gcc:
if test "${GCC}" = yes -a -z "${ERROR_ON_WARNING}"; then
    ERROR_ON_WARNING=yes
fi

NO_WERROR=""
if test "x${ERROR_ON_WARNING}" = "xyes"; then
    WARN_CFLAGS="${WARN_CFLAGS} -Werror"
    GCC_WARN_CFLAGS_FOR_BUILD="${GCC_WARN_CFLAGS_FOR_BUILD} -Werror"
    NO_WERROR="-Wno-error"
fi

if test "x${PEDANTIC_WARNINGS}" = "xyes"; then
    WARN_CFLAGS="${WARN_CFLAGS} -pedantic -Wdeclaration-after-statement"
    ## avoid warnings about "long long" when we are using it:
    if test "x${want64}" = "xtrue" && test "x${bfd_cv_has_long_long}" = "xyes" && test "x${BFD_HOST_64BIT_LONG_LONG}" = "x1"; then
      ## add something else to make up for the removed warnings:
      WARN_CFLAGS="${WARN_CFLAGS} -Wno-long-long -Wpointer-arith"
    else
      WARN_CFLAGS="${WARN_CFLAGS} -Wexpansion-to-defined"
    fi
    ## fake the '-ansi' flag; actually using it has additional effects:
    WARN_CFLAGS="${WARN_CFLAGS} -D__STRICT_ANSI__"
    WARN_DEFS="${WARN_DEFS} -D__STRICT_ANSI__"
fi

AC_REQUIRE([gl_UNKNOWN_WARNINGS_ARE_ERRORS])dnl
		   
AC_ARG_ENABLE([build-warnings],
[AS_HELP_STRING([--enable-build-warnings],
                [Enable build-time compiler warnings])],
[case "${enableval}" in
  yes)	;;
  no)	WARN_CFLAGS="-w";;
  ,*)   t=`echo "${enableval}" | sed -e "s/,/ /g"`
        WARN_CFLAGS="${WARN_CFLAGS} ${t}";;
  *,)   t=`echo "${enableval}" | sed -e "s/,/ /g"`
        WARN_CFLAGS="${t} ${WARN_CFLAGS}";;
  *)    WARN_CFLAGS=`echo "${enableval}" | sed -e "s/,/ /g"`;;
esac])dnl

if test "x${WARN_DEFS}" != "x" -a "x${GCC}" = "xyes"
then
    AC_MSG_CHECKING([preprocessor definitons to use when warning])
    for w in ${WARN_DEFS}; do
	case ${w} in
	*) # Check its use with the preprocessor.
	    saved_DEFS="${DEFS}"
	    DEFS="${DEFS} ${w}"
            # The above fails to actually get it into the compile command,
            # so do this as well:
	    saved_CFLAGS="${CFLAGS}"
	    CFLAGS="${CFLAGS} -Wp,${w}"
            # Actually that fails as well, so one more try:
            saved_CPPFLAGS="${CPPFLAGS}"
	    CPPFLAGS="${CPPFLAGS} ${w}"
	    AC_PREPROC_IFELSE([AC_LANG_PROGRAM([[]],[[]])],
                              [MY_WARN_DEFS="${MY_WARN_DEFS} ${w}"],[])
	    DEFS="${saved_DEFS}"
	    CFLAGS="${saved_CFLAGS}"
	    CPPFLAGS="${saved_CPPFLAGS}"
	esac
    done
    # Have to use second variable to avoid overwriting and/or duplicates:
    WARN_DEFS="${MY_WARN_DEFS}"
    AC_MSG_RESULT([${WARN_DEFS}])
fi

BAD_WARN_CFLAGS=""
if test "x${WARN_CFLAGS}" != "x" -a "x${GCC}" = "xyes"
then
    AC_MSG_CHECKING([compiler warning flags to use])
    for w in ${WARN_CFLAGS}; do
	case ${w} in
	*) # Check that GCC accepts it:
	    saved_CFLAGS="${CFLAGS}"
	    CFLAGS="${CFLAGS} ${w} ${gl_unknown_warnings_are_errors}"
	    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])],
                              [MY_WARN_CFLAGS="${MY_WARN_CFLAGS} ${w}"],
                              [BAD_WARN_CFLAGS="${BAD_WARN_CFLAGS} ${w}"])
	    CFLAGS="${saved_CFLAGS}"
	esac
    done
    # Have to use second variable to avoid overwriting and/or duplicates:
    WARN_CFLAGS="${MY_WARN_CFLAGS}"
    AC_MSG_RESULT([using: ${WARN_CFLAGS}])
    if test "x${BAD_WARN_CFLAGS}" != "x"; then
      test -n "${BAD_WARN_CFLAGS}"
      AC_MSG_WARN([compiler failed to accept: ${BAD_WARN_CFLAGS}])
    fi
    # Also:
    if test "x${WARN_CFLAGS_FOR_BUILD}" = "x"; then
      test -z "${WARN_CFLAGS_FOR_BUILD}" && export WARN_CFLAGS_FOR_BUILD="${GCC_WARN_CFLAGS_FOR_BUILD}"
    fi
fi

if test "x${WARN_LDFLAGS}" != "x" -a "x${GCC}" = "xyes"
then
    AC_MSG_CHECKING([linker warning flags to use])
    for w in ${WARN_LDFLAGS}; do
	case ${w} in
	*) # Check that the compiler successfully passes it to the linker.
	    saved_LDFLAGS="${LDFLAGS}"
	    LDFLAGS="${LDFLAGS} ${w}"
	    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]],[[]])],
                           [MY_WARN_LDFLAGS="${MY_WARN_LDFLAGS} ${w}"],[])
	    LDFLAGS="${saved_LDFLAGS}"
	esac
    done
    # Have to use second variable to avoid overwriting and/or duplicates:
    WARN_LDFLAGS="${MY_WARN_LDFLAGS}"
    AC_MSG_RESULT([${WARN_LDFLAGS}])
fi

if test x"${silent}" != x"yes" && test x"${WARN_CFLAGS}" != x""; then
  echo "Setting C compiler warning flags = ${WARN_CFLAGS}" 6>&1
fi
if test x"${silent}" != x"yes" && test x"${WARN_LDFLAGS}" != x""; then
  echo "Setting linker warning flags = ${WARN_LDFLAGS}" 6>&1
fi

AC_SUBST([WARN_CFLAGS])dnl
AC_SUBST([WARN_CFLAGS_FOR_BUILD])dnl
AC_SUBST([WARN_DEFS])dnl
AC_SUBST([WARN_LDFLAGS])dnl
AC_SUBST([NO_WERROR])dnl
AC_SUBST([WARN_WRITE_STRINGS])dnl
])dnl

AC_DEFUN([AM_BINUTILS_CLANG_STATIC_ANALYSIS],[
AC_ARG_VAR([CLANG_ANALYZER],[Path to the clang static analyzer])dnl
AC_CACHE_CHECK([for the clang static analyzer],[ac_cv_path_CLANG_ANALYZER],
  [AC_PATH_PROGS_FEATURE_CHECK([CLANG_ANALYZER],
    [clang clang++ clang-mp-3.5 clang-mp-3.4 clang-mp-3.3 clang-mp-3.2],
    [[${ac_path_CLANG_ANALYZER} --analyze /dev/null > /dev/null 2>&1 && \
      ac_cv_path_CLANG_ANALYZER=${ac_path_CLANG_ANALYZER}
      ac_path_CLANG_ANALYZER_found=:]],
    [AC_MSG_WARN([we will not be able to do static analysis with clang])],
    [${PATH}:/opt/local/bin])dnl# end program check
  ])dnl# end cache check
  ## (need this extra line here)
AC_SUBST([CLANG_ANALYZER],[${ac_cv_path_CLANG_ANALYZER}])dnl
])dnl

