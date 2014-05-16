#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([gemini_prime_d], [0.1], [emoryau@gmail.com])
AC_CONFIG_SRCDIR([src/gemini_prime_d.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(GSTREAMER, [gstreamer-1.0 >= 1.0])
PKG_CHECK_MODULES(SQLITE, [sqlite3 >= 3.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
LT_INIT

AC_CONFIG_FILES([Makefile src/Makefile src/libmagic/Makefile src/dao/Makefile src/dao/sqlite3/Makefile])
AC_OUTPUT
