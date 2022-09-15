dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.52)

AC_INIT(mstardict, 0.6, roman.moravcik@gmail.com)
AC_CONFIG_SRCDIR(src/mstardict.cpp)

dnl Don't include maintainer make-rules by default
AM_MAINTAINER_MODE

AM_INIT_AUTOMAKE([dist-bzip2])

AM_CONFIG_HEADER(config.h)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_LANG([C++])
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_LIBTOOL

dnl ================================================================
dnl Gettext stuff.
dnl ================================================================
IT_PROG_INTLTOOL([0.35])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14.1)
GETTEXT_PACKAGE=mstardict
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package])
AM_ICONV

AC_FUNC_MMAP

AC_CHECK_LIB(z,zlibVersion,LIBS="$LIBS -lz",[AC_MSG_ERROR([zlib not found])])

PKG_CHECK_MODULES(MSTARDICT, glib-2.0 >= 2.6.1 gmodule-2.0 hildon-1 gconf-2.0 >= 2.6.2 libosso)

MSTARDICT_CFLAGS="-Wall $MSTARDICT_CFLAGS"

AC_SUBST(MSTARDICT_LIBS)
AC_SUBST(MSTARDICT_CFLAGS)

# check for libespeak (consider moving this to maemo5)
AC_CHECK_HEADERS(espeak/speak_lib.h, [
MSTARDICT_CFLAGS="$MSTARDICT_CFLAGS -DESPEAK"
MSTARDICT_LIBS="$MSTARDICT_LIBS -lespeak"
])

dnl fill cflags for libstardict
PKG_CHECK_MODULES(LIB_STARDICT, glib-2.0 >= 2.6.1 gmodule-2.0 gtk+-2.0 >= 2.12 sigc++-2.0)
AC_SUBST(LIB_STARDICT_CFLAGS)

AC_OUTPUT([
 Makefile
 data/Makefile
 data/mstardict.desktop.in
 m4/Makefile
 po/Makefile.in
 src/Makefile
 src/lib/Makefile
 stardict-plugins/Makefile
 stardict-plugins/stardict-html-parsedata-plugin/Makefile
 stardict-plugins/stardict-powerword-parsedata-plugin/Makefile
 stardict-plugins/stardict-wiki-parsedata-plugin/Makefile
 stardict-plugins/stardict-xdxf-parsedata-plugin/Makefile
])

echo "

Type \"make\" to compile.

"
