AC_PREREQ(2.59)

m4_define([spectre_major_version], [0])
m4_define([spectre_minor_version], [2])
m4_define([spectre_micro_version], [9])
m4_define([spectre_version],
          [spectre_major_version.spectre_minor_version.spectre_micro_version])

AC_INIT(libspectre, [spectre_version])
AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE([1.7])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

# libtool versioning
#
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details
#

## increment if the interface has additions, changes, removals.
LT_CURRENT=2

## increment any time the source changes; set to
##  0 if you increment CURRENT
LT_REVISION=9

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
LT_AGE=1

VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(VERSION_INFO)

LT_CURRENT_MINUS_AGE=`expr $LT_CURRENT - $LT_AGE`
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

SPECTRE_MAJOR_VERSION=spectre_major_version
SPECTRE_MINOR_VERSION=spectre_minor_version
SPECTRE_MICRO_VERSION=spectre_micro_version
SPECTRE_VERSION=spectre_major_version.spectre_minor_version.spectre_micro_version

AC_SUBST(SPECTRE_MAJOR_VERSION)
AC_SUBST(SPECTRE_MINOR_VERSION)
AC_SUBST(SPECTRE_MICRO_VERSION)
AC_SUBST(SPECTRE_VERSION)

AC_PROG_LIBTOOL
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_CC_STDC
AC_STDC_HEADERS
AC_C_BIGENDIAN

AC_CHECK_FUNC(vasprintf, [ AC_DEFINE(HAVE_VASPRINTF, 1, [Define if the 'vasprintf' function is available.]) ])

AC_CHECK_FUNC(_vscprintf, [ AC_DEFINE(HAVE__VSCPRINTF, 1, [Define if the '_vscprintf' function is available.]) ])

LIBGS_REQUIRED="9.24"

AC_CHECK_LIB(gs, gsapi_new_instance, have_libgs=yes, have_libgs=no)
if test "x$have_libgs" = "xyes"; then
    LIB_GS="-lgs"
    save_LIBS=$LIBS
    LIBS="$LIBS -lgs"
    AC_LANG_PUSH(C)
    AC_MSG_CHECKING([for libgs >= $LIBGS_REQUIRED])
    AC_RUN_IFELSE([AC_LANG_PROGRAM(
[[
#include <ghostscript/iapi.h>
]],
[[
    gsapi_revision_t gsrev;

    if (gsapi_revision (&gsrev, sizeof (gsrev)) != 0)
        return 1;
    if (gsrev.revision < `echo "$LIBGS_REQUIRED" | sed -e 's/\.//'`)
        return 1;
]])],
                   [have_libgs=yes],
		   [have_libgs=no],
		   [have_libgs=yes]
    )
    AC_MSG_RESULT($have_libgs)
    AC_LANG_POP(C)
    LIBS=$save_LIBS
    if test "x$have_libgs" = "xno"; then
        AC_MSG_ERROR([You need libgs >= $LIBGS_REQUIRED in order to compile libspectre])
    fi
else
    AC_MSG_ERROR([You need libgs in order to compile libspectre])
fi
AC_SUBST(LIB_GS)

AC_ARG_ENABLE(asserts,
	AS_HELP_STRING([--enable-asserts],[include assertion checks]),
	enable_asserts=$enableval,
	enable_asserts=$USE_MAINTAINER_MODE)

if test x$enable_asserts = xno; then
   AC_DEFINE(SPECTRE_DISABLE_ASSERT,[1],[Disable assertion checking])
fi
	
AC_ARG_ENABLE(checks,
	AS_HELP_STRING([--enable-checks],[include sanity checks on public API]),
	enable_checks=$enableval,
	enable_checks=yes)

if test x$enable_checks = xno; then
   AC_DEFINE(SPECTRE_DISABLE_CHECKS,[1],[Disable public API sanity checking])
fi
	
dnl Test
AC_ARG_ENABLE(test,
              [AC_HELP_STRING([--enable-test], [Compile tests])],enable_test="$enableval",enable_test=yes)

if test "x$enable_test" = "xyes"; then
    PKG_CHECK_MODULES(CAIRO, cairo cairo-png, enable_test=yes,enable_test=no)

    if test "x$enable_test" != "xyes"; then
        AC_MSG_WARN("Tests disabled since required cairo library was not found")
    fi
fi

AM_CONDITIONAL(ENABLE_TEST, test x$enable_test = xyes)

dnl ===========================================================================
dnl check compiler flags (From cairo)
AC_DEFUN([SPECTRE_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether $CC supports $1])

  spectre_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1"

  AC_COMPILE_IFELSE([ ], [spectre_cc_flag=yes], [spectre_cc_flag=no])
  CFLAGS="$spectre_save_CFLAGS"

  if test "x$spectre_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$spectre_cc_flag])
])


dnl Use lots of warning flags with with gcc and compatible compilers

dnl Note: if you change the following variable, the cache is automatically
dnl skipped and all flags rechecked.  So there's no need to do anything
dnl else.  If for any reason you need to force a recheck, just change
dnl MAYBE_WARN in an ignorable way (like adding whitespace)

MAYBE_WARN="-Wall -Wextra \
-Wsign-compare -Werror-implicit-function-declaration \
-Wpointer-arith -Wstrict-prototypes \
-Wmissing-prototypes -Wmissing-declarations -Wnested-externs \
-Wpacked -Wswitch-enum \
-Wstrict-aliasing=2 -Winit-self -Wunsafe-loop-optimizations \
-Wdeclaration-after-statement \
-Wno-missing-field-initializers -Wno-unused-parameter \
-Wno-attributes -Wno-long-long -Winline"

AC_ARG_ENABLE(iso-c,
              AC_HELP_STRING([--enable-iso-c], [Try to warn if code is not ISO C ]),,[enable_iso_c=no])

if test "x$enable_iso_c" != "xno"; then
   MAYBE_WARN="$MAYBE_WARN -ansi -pedantic"
fi

# invalidate cached value if MAYBE_WARN has changed
if test "x$spectre_cv_warn_maybe" != "x$MAYBE_WARN"; then
	unset spectre_cv_warn_cflags
fi
AC_CACHE_CHECK([for supported warning flags], spectre_cv_warn_cflags, [
	echo
	WARN_CFLAGS=""

	# Some warning options are not supported by all versions of
	# gcc, so test all desired options against the current
	# compiler.
	#
	# Note that there are some order dependencies
	# here. Specifically, an option that disables a warning will
	# have no net effect if a later option then enables that
	# warnings, (perhaps implicitly). So we put some grouped
	# options (-Wall and -Wextra) up front and the -Wno options
	# last.

	for W in $MAYBE_WARN; do
		SPECTRE_CC_TRY_FLAG([$W], [WARN_CFLAGS="$WARN_CFLAGS $W"])
	done

	spectre_cv_warn_cflags=$WARN_CFLAGS
	spectre_cv_warn_maybe=$MAYBE_WARN

	AC_MSG_CHECKING([which warning flags were supported])])
WARN_CFLAGS="$spectre_cv_warn_cflags"
SPECTRE_CFLAGS="$SPECTRE_CFLAGS $WARN_CFLAGS"

AC_SUBST(SPECTRE_CFLAGS)

dnl **********************************
dnl *** va_copy checks (from GLib) ***
dnl **********************************
dnl we currently check for all three va_copy possibilities, so we get
dnl all results in config.log for bug reports.
AC_CACHE_CHECK([for an implementation of va_copy()],spectre_cv_va_copy,[
	AC_LINK_IFELSE([#include <stdarg.h>
#include <stdlib.h>
	void f (int i, ...) {
	va_list args1, args2;
	va_start (args1, i);
	va_copy (args2, args1);
	if (va_arg (args2, int) != 42 || va_arg (args1, int) != 42)
	  exit (1);
	va_end (args1); va_end (args2);
	}
	int main() {
	  f (0, 42);
	  return 0;
	}],
	[spectre_cv_va_copy=yes],
	[spectre_cv_va_copy=no])
])
AC_CACHE_CHECK([for an implementation of __va_copy()],spectre_cv___va_copy,[
	AC_LINK_IFELSE([#include <stdarg.h>
#include <stdlib.h>	
	void f (int i, ...) {
	va_list args1, args2;
	va_start (args1, i);
	__va_copy (args2, args1);
	if (va_arg (args2, int) != 42 || va_arg (args1, int) != 42)
	  exit (1);
	va_end (args1); va_end (args2);
	}
	int main() {
	  f (0, 42);
	  return 0;
	}],
	[spectre_cv___va_copy=yes],
	[spectre_cv___va_copy=no])
])

if test "x$spectre_cv_va_copy" = "xyes"; then
  spectre_va_copy_func=va_copy
else if test "x$spectre_cv___va_copy" = "xyes"; then
  spectre_va_copy_func=__va_copy
fi
fi

if test -n "$spectre_va_copy_func"; then
  AC_DEFINE_UNQUOTED(SPECTRE_VA_COPY,$spectre_va_copy_func,[A 'va_copy' style function])
fi

AC_LANG_PUSH(C)
AC_CACHE_CHECK([whether va_lists can be copied by value],
	spectre_cv_va_val_copy,
	[AC_RUN_IFELSE([AC_LANG_PROGRAM(
[[
	#include <stdarg.h>
	#include <stdlib.h>
]],
[[
	void f (int i, ...) {
	va_list args1, args2;
	va_start (args1, i);
	args2 = args1;
	if (va_arg (args2, int) != 42 || va_arg (args1, int) != 42)
	  exit (1);
	va_end (args1); va_end (args2);
	}
	int main() {
	  f (0, 42);
	  return 0;
	}
]])],
	[spectre_cv_va_val_copy=yes],
	[spectre_cv_va_val_copy=no],
	[spectre_cv_va_val_copy=yes])
])
AC_LANG_POP(C)

if test "x$spectre_cv_va_val_copy" = "xno"; then
  AC_DEFINE(SPECTRE_VA_COPY_AS_ARRAY,1, ['va_lists' cannot be copies as values])
fi

dnl Check for -fvisibility=hidden to determine if we can do GNU-style
dnl visibility attributes for symbol export control
dnl
VISIBILITY_CFLAGS=""
case "$host" in
  *-*-mingw*)
    dnl on mingw32 we do -fvisibility=hidden and __declspec(dllexport)
    AC_DEFINE([SPECTRE_PUBLIC], [__attribute__((visibility("default"))) __declspec(dllexport) extern],
              [defines how to decorate public symbols while building])
    CFLAGS="${CFLAGS} -fvisibility=hidden"
    ;;
  *)
    dnl on other compilers, check if we can do -fvisibility=hidden
    SAVED_CFLAGS="${CFLAGS}"
    CFLAGS="-fvisibility=hidden"
    AC_MSG_CHECKING([for -fvisibility=hidden compiler flag])
    AC_TRY_COMPILE([], [return 0],
                   AC_MSG_RESULT(yes)
                   enable_fvisibility_hidden=yes,
                   AC_MSG_RESULT(no)
                   enable_fvisibility_hidden=no)
    CFLAGS="${SAVED_CFLAGS}"

    AS_IF([test "${enable_fvisibility_hidden}" = "yes"], [
      AC_DEFINE([SPECTRE_PUBLIC], [__attribute__((visibility("default"))) extern],
                [defines how to decorate public symbols while building])
      VISIBILITY_CFLAGS="-fvisibility=hidden"
    ],
    [AC_DEFINE([SPECTRE_PUBLIC],[extern])])
    ;;
esac
AC_SUBST([VISIBILITY_CFLAGS])

AC_OUTPUT([
Makefile
Doxyfile
libspectre/Makefile
libspectre/spectre-version.h
test/Makefile
libspectre.pc
])
