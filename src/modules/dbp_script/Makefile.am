# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_SCRIPT

pkglib_LTLIBRARIES = libdbp_script.la

libdbp_script_la_SOURCES = \
	dbp_script_factory.cpp \
	tag_javascript.cpp

libdbp_script_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_script_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@LIBV8_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@LIBV8_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_script.conf

EXTRA_DIST = *.h

endif

