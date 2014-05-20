bin_PROGRAMS=gemini_prime_d
AM_CPPFLAGS=$(GSTREAMER_CFLAGS) $(BOOST_CPPFLAGS)
SUBDIRS = dao libmagic
gemini_prime_d_LDADD=dao/libdao.la libmagic/libmagic.la $(GSTREAMER_LIBS)
gemini_prime_d_SOURCES= \
	Filesystem.cpp \
	TagExtractor.cpp \
	MetadataService.cpp \
	Track.cpp \
	PlaylistService.cpp \
	GeminiException.cpp \
	Controller.cpp \
	TestController.cpp \
	MaintenanceController.cpp \
	PlaybackController.cpp \
	gemini_prime_d.cpp