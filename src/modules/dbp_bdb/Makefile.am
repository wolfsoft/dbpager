# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_BDB

pkglib_LTLIBRARIES = libdbp_bdb.la

libdbp_bdb_la_SOURCES = \
	dbp_bdb_factory.cpp \
	tag_storage.cpp \
	tag_get.cpp \
	tag_set.cpp \
	tag_find.cpp \
	tag_delete.cpp

libdbp_bdb_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_bdb_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@DB_CXX_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@DB_CXX_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_bdb.conf

EXTRA_DIST = *.h

endif

