#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(BilibiliConsole, 0.0.1, codehz@outlook.com)
AC_CONFIG_SRCDIR([test])
AC_CONFIG_LIBOBJ_DIR([player])
AC_PROG_RANLIB
AM_INIT_AUTOMAKE
# AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(SDL2, SDL_Init)
AC_CHECK_LIB(vlc, libvlc_new)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([getcwd])

AC_OUTPUT(Makefile test/Makefile player/Makefile)
