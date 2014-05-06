bin_PROGRAMS=gemini_prime_d
AM_CPPFLAGS=$(DEPS_CFLAGS) $(SQLITE_CFLAGS)
SUBDIRS = libmagic dao/sqlite3
gemini_prime_d_LDADD=$(DEPS_LIBS) $(SQLITE_LIBS) libmagic/libmagic.la dao/sqlite3/libdaosqlite3.la
gemini_prime_d_SOURCES=gemini_prime_d.cpp Filesystem.cpp TagExtractor.cpp MetadataStore.cpp TrackManager.cpp Track.cpp Album.cpp Artist.cpp GeminiException.cpp
