noinst_LTLIBRARIES = libdaosqlite3.la
libdaosqlite3_la_CPPFLAGS=$(GSTREAMER_CFLAGS) $(SQLITE_CFLAGS) -I$(top_srcdir)/src
libdaosqlite3_la_LIBADD = $(GSTREAMER_LIBS) $(SQLITE_LIBS)
libdaosqlite3_la_SOURCES = \
	BaseSqlite3Impl.cpp \
	ArtistDAOSqlite3Impl.cpp \
	AlbumDAOSqlite3Impl.cpp \
	TrackDAOSqlite3Impl.cpp \
	PlaylistDAOSqlite3Impl.cpp \
	SettingsDAOSqlite3Impl.cpp \
	DAOFactorySqlite3Impl.cpp 
