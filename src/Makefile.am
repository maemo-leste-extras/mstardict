SUBDIRS=lib
bin_PROGRAMS = mstardict
mstardict_SOURCES = \
	conf.cpp conf.hpp \
	dictmngr.cpp dictmngr.hpp \
	libwrapper.cpp libwrapper.hpp \
	mstardict.cpp mstardict.hpp \
	prefsdlg.cpp prefsdlg.hpp \
	transwin.cpp transwin.hpp \
	tts.cpp tts.hpp

mstardict_DEPENDENCIES = lib/libstardict.a
mstardict_LDADD = lib/libstardict.a @MSTARDICT_LIBS@ @LIBINTL@
localedir = $(datadir)/locale

AM_CPPFLAGS = @MSTARDICT_CFLAGS@  -I$(top_builddir) -Ilib
DEFS=-DLOCALEDIR=\"$(localedir)\" @DEFS@
