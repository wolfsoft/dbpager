# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_XSLT

pkglib_LTLIBRARIES = libdbp_xslt.la

libdbp_xslt_la_SOURCES = \
	dbp_xslt_factory.cpp \
	tag_transform.cpp

libdbp_xslt_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_xslt_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@libxml2_LIBS@ \
	@libxslt_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@libxml2_CFLAGS@ \
	@libxslt_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_xslt.conf

EXTRA_DIST = *.h

endif

