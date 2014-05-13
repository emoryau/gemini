noinst_LTLIBRARIES = libdaosqlite3.la
libdaosqlite3_la_CPPFLAGS=$(DEPS_CFLAGS) $(SQLITE_CFLAGS) -I$(top_srcdir)/src
libdaosqlite3_la_LIBADD = $(SQLITE_LIBS)
libdaosqlite3_la_SOURCES = \
	BaseSqlite3Impl.cpp \
	ArtistDAOSqlite3Impl.cpp \
	AlbumDAOSqlite3Impl.cpp \
	TrackDAOSqlite3Impl.cpp \
	PlaylistDAOSqlite3Impl.cpp \
	DAOFactorySqlite3Impl.cpp 
