AC_INIT([rationL], [v0.1.0], [simon.scatton@epita.fr])

AC_CONFIG_SRCDIR([src/])

AM_INIT_AUTOMAKE([
  -Wall
  -Werror
  foreign
  subdir-objects
])

AM_SILENT_RULES([yes])


AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR

AC_CONFIG_MACRO_DIRS([m4])

LT_PREREQ([2.2])
LT_INIT([dlopen shared])


# Checking for criterion in pkg-config
PKG_CHECK_MODULES([CRITERION], [criterion], [AC_SUBST([CRITERION_CFLAGS])
AC_SUBST([CRITERION_LIBS])
], [
  AC_MSG_WARN([Could not find criterion - continue without Unit test support])
])


#Checking for Doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
   AC_MSG_WARN([Doxygen not found - continue without Doxygen support])
else
   #Checking for dot if doxygen is installed
   AC_CHECK_PROGS([DOT], [dot])
   if test -z "$DOT"; then
      AC_MSG_WARN([Doxygen needs dot, please install dot first if you want to compile docs])
   fi
fi

AC_REQUIRE_AUX_FILE([tap-driver.sh])

#Checking for coverage support
AC_CHECK_PROGS([LCOV], [lcov])
AC_CHECK_PROGS([GENHTML], [genhtml])
AM_CONDITIONAL([COVERAGE], [[test -n "$LCOV"] && [test -n "$GENHTML"]])

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/DoxyfilePublic docs/DoxyfileInternal])])

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [enable debug build]), [], [])
AS_IF([test "$enable_debug" = "yes"], [
        AC_DEFINE([DEBUG], [1], [debug build])
])

AH_TEMPLATE([DEBUG],
            [Define to 1 if debug is enabled, 0 otherwise])

AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([Makefile include/Makefile test/Makefile docs/Makefile rationl.pc])
# Output the files
AC_OUTPUT
