# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_REDIS

pkglib_LTLIBRARIES = libmod_session_redis.la

libmod_session_redis_la_SOURCES = \
	mod_session_redis_factory.cpp \
	mod_session_redis.cpp

libmod_session_redis_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libmod_session_redis_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libhiredis_LIBS@ \
	@libdclbase_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libhiredis_CFLAGS@ \
	@libdclbase_CFLAGS@

confdir = $(sysconfdir)/dbpager/services-available
dist_conf_DATA = mod_session_redis.conf

EXTRA_DIST = *.h

endif
