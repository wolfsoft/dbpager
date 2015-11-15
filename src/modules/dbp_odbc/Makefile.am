# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_ODBC

pkglib_LTLIBRARIES = libdbp_odbc.la

libdbp_odbc_la_SOURCES = \
	dbp_odbc_factory.cpp \
	tag_database.cpp \
	tag_transaction.cpp \
	tag_query.cpp

libdbp_odbc_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_odbc_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@libdclodbc_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@libdclodbc_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_odbc.conf

EXTRA_DIST = *.h

endif

