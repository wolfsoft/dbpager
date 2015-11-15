# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_GEARMAN

pkglib_LTLIBRARIES = libdbp_gearman.la

libdbp_gearman_la_SOURCES = \
	dbp_gearman_factory.cpp \
	tag_server.cpp \
	tag_task.cpp

libdbp_gearman_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_gearman_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@libgearman_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@libgearman_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_gearman.conf

EXTRA_DIST = *.h

endif

