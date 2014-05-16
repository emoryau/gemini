bin_PROGRAMS=gemini_prime_d
AM_CPPFLAGS=$(DEPS_CFLAGS)
SUBDIRS = dao libmagic
gemini_prime_d_LDADD=dao/libdao.la libmagic/libmagic.la $(DEPS_LIBS)
gemini_prime_d_SOURCES= \
	Filesystem.cpp \
	TagExtractor.cpp \
	MetadataStore.cpp \
	Track.cpp \
	Album.cpp \
	Artist.cpp \
	PlaylistService.cpp \
	GeminiException.cpp \
	Controller.cpp \
	gemini_prime_d.cpp