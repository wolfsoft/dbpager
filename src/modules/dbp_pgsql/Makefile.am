# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_LIBPQXX

pkglib_LTLIBRARIES = libdbp_pgsql.la

libdbp_pgsql_la_SOURCES = \
	dbp_pgsql_factory.cpp \
	tag_database.cpp \
	tag_transaction.cpp \
	tag_query.cpp

libdbp_pgsql_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_pgsql_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@libpqxx_LIBS@

AM_CXXFLAGS += \
	-DPQXX_HIDE_EXP_OPTIONAL=1 \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@libpqxx_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_pgsql.conf

EXTRA_DIST = *.h

endif

