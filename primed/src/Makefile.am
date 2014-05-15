bin_PROGRAMS=gemini_prime_d
AM_CPPFLAGS=$(DEPS_CFLAGS) $(SQLITE_CFLAGS)
SUBDIRS = libmagic dao/sqlite3
gemini_prime_d_LDADD=libmagic/libmagic.la dao/sqlite3/libdaosqlite3.la $(DEPS_LIBS) $(SQLITE_LIBS) 
gemini_prime_d_SOURCES= \
	Filesystem.cpp \
	TagExtractor.cpp \
	MetadataStore.cpp \
	Track.cpp \
	Album.cpp \
	Artist.cpp \
	PlaylistService.cpp \
	GeminiException.cpp \
	gemini_prime_d.cpp