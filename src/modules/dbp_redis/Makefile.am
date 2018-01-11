# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_REDIS

pkglib_LTLIBRARIES = libdbp_redis.la

libdbp_redis_la_SOURCES = \
	dbp_redis_factory.cpp \
	tag_connection.cpp \
	tag_query.cpp

libdbp_redis_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_redis_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libhiredis_LIBS@ \
	@libdclbase_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libhiredis_CFLAGS@ \
	@libdclbase_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_redis.conf

EXTRA_DIST = *.h

endif

