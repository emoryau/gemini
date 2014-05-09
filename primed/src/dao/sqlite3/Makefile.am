noinst_LTLIBRARIES = libdaosqlite3.la
libdaosqlite3_la_CPPFLAGS=$(DEPS_CFLAGS) $(SQLITE_CFLAGS)
libdaosqlite3_la_SOURCES = BaseSqlite3Impl.cpp BaseSqlite3Impl.hpp \
	ArtistDAOSqlite3Impl.cpp ArtistDAOSqlite3Impl.hpp \
	AlbumDAOSqlite3Impl.cpp AlbumDAOSqlite3Impl.hpp \
	TrackDAOSqlite3Impl.cpp TrackDAOSqlite3Impl.hpp \
	DAOFactorySqlite3Impl.cpp DAOFactorySqlite3Impl.hpp
libdaosqlite3_la_LIBADD = $(SQLITE_LIBS)