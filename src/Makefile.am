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
mstardict_LDADD = @MSTARDICT_LIBS@ @LIBINTL@ lib/libstardict.a
localedir = $(datadir)/locale

INCLUDES = @MSTARDICT_CFLAGS@  -I$(top_builddir) -Ilib
DEFS=-DLOCALEDIR=\"$(localedir)\" @DEFS@
