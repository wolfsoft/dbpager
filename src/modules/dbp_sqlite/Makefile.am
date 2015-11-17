# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_SQLITE

pkglib_LTLIBRARIES = libdbp_sqlite.la

libdbp_sqlite_la_SOURCES = \
	dbp_sqlite_factory.cpp \
	tag_database.cpp \
	tag_transaction.cpp \
	tag_query.cpp

libdbp_sqlite_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_sqlite_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@sqlite3_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@sqlite3_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_sqlite.conf

EXTRA_DIST = *.h

endif

