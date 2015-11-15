# Include common build rules
include $(top_srcdir)/Makefile.rules

pkglib_LTLIBRARIES = libmod_logger_file.la

libmod_logger_file_la_SOURCES = \
	mod_logger_file_factory.cpp \
	mod_logger_file.cpp

libmod_logger_file_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libmod_logger_file_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@

confdir = $(sysconfdir)/dbpager/services-available
dist_conf_DATA = mod_logger_file.conf

EXTRA_DIST = *.h

