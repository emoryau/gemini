noinst_LTLIBRARIES = libdao.la
libdao_la_CPPFLAGS=$(GSTREAMER_CFLAGS) -I$(top_srcdir)/src
SUBDIRS = sqlite3
libdao_la_LIBADD = sqlite3/libdaosqlite3.la $(GSTREAMER_LIBS) $(SQLITE_LIBS) 
libdao_la_SOURCES = DAOFactory.cpp