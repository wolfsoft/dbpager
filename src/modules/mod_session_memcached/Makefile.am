# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_MEMCACHED

pkglib_LTLIBRARIES = libmod_session_memcached.la

libmod_session_memcached_la_SOURCES = \
	mod_session_memcached_factory.cpp \
	mod_session_memcached.cpp

libmod_session_memcached_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libmod_session_memcached_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libmemcached_LIBS@ \
	@libdclbase_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libmemcached_CFLAGS@ \
	@libdclbase_CFLAGS@

confdir = $(sysconfdir)/dbpager/services-available
dist_conf_DATA = mod_session_memcached.conf

EXTRA_DIST = *.h

endif
