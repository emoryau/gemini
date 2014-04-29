AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS=gemini_prime_d
AM_CPPFLAGS=$(DEPS_CFLAGS) $(SQLITE_CFLAGS)
gemini_prime_d_LDADD=$(DEPS_LIBS) $(SQLITE_LIBS)
gemini_prime_d_SOURCES=gemini_prime_d.cpp Filesystem.cpp TagExtractor.cpp MetadataStore.cpp TrackManager.cpp Track.cpp Album.cpp Artist.cpp libmagic/magic.c
