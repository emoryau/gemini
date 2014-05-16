noinst_LTLIBRARIES = libdao.la
libdao_la_CPPFLAGS=$(DEPS_CFLAGS) -I$(top_srcdir)/src
SUBDIRS = sqlite3
libdao_la_LIBADD = sqlite3/libdaosqlite3.la $(DEPS_LIBS) $(SQLITE_LIBS) 
libdao_la_SOURCES = DAOFactory.cpp