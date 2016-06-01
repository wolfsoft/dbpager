# Include common build rules
include $(top_srcdir)/Makefile.rules

# Static library with common dbpager features
noinst_LTLIBRARIES = libdbpcore.la
libdbpcore_la_LDFLAGS = -static -no-undefined
libdbpcore_la_CPPFLAGS = \
	-fPIC \
	-I../mimetic-0.9.7 \
	@libdclbase_CFLAGS@ \
	@libdclnet_CFLAGS@ \
	@libxml2_CFLAGS@ \
	@libpcrecpp_CFLAGS@ \
	@libcurl_CFLAGS@

libdbpcore_la_SOURCES = \
	parser/dbpx_parser.cpp \
	optimizer/optimizer.cpp \
	interpreter/environment.cpp \
	interpreter/dbpager_application.cpp \
	interpreter/interpreter.cpp \
	tag/tag_factory.cpp \
	tag/tag_call.cpp \
	tag/tag_execute.cpp \
	tag/tag_unknown.cpp \
	tag/tag_var.cpp \
	tag/tag_set.cpp \
	tag/tag_try.cpp \
	tag/tag_catch.cpp \
	tag/tag_throw.cpp \
	tag/tag_case.cpp \
	tag/tag_switch.cpp \
	tag/tag_while.cpp \
	tag/tag_split.cpp \
	tag/tag_substr.cpp \
	tag/tag_regexp.cpp \
	tag/tag_expression.cpp \
	tag/tag_read.cpp \
	tag/tag_write.cpp \
	tag/tag_locale.cpp \
	tag/tag_system.cpp \
	tag/tag_delete.cpp \
	tag/tag_rename.cpp \
	tag/tag_log.cpp \
	tag/tag_http.cpp \
	tag/functions.cpp

libdbpcore_la_LIBADD = \
	../libdbpager/libdbpager.la \
	../mimetic-0.9.7/mimetic/libmimetic.la \
	@libdclbase_LIBS@ \
	@libdclnet_LIBS@ \
	@libxml2_LIBS@ \
	@libpcrecpp_LIBS@ \
	@libcurl_LIBS@ \
	-lcrypt

# Console application
bin_PROGRAMS = dbpager
dbpager_LDADD = libdbpcore.la
dbpager_SOURCES = \
	application/dbpager.cpp

# CGI application
if WITH_CGI
cgidir = $(libdir)/cgi-bin
cgi_PROGRAMS = dbp_cgi
dbp_cgi_LDADD = libdbpcore.la \
	@libdclnet_LIBS@
dbp_cgi_SOURCES = \
	application/dbp_cgi.cpp
endif

# Apache module
if WITH_APACHE
apachedir = $(libdir)/apache2/modules
apache_LTLIBRARIES = mod_dbp.la
mod_dbp_la_LDFLAGS = -Wl,--as-needed -avoid-version -module
mod_dbp_la_LIBADD = libdbpcore.la \
	@libdclnet_LIBS@ \
	@APACHE_LIBTOOL@
mod_dbp_la_CPPFLAGS = @APACHE_CFLAGS@
mod_dbp_la_SOURCES = application/mod_dbp.cpp
endif

# ISAPI module
if WITH_ISAPI
lib_LTLIBRARIES = libdbp_isapi.la
libdbp_isapi_la_LDFLAGS = -Wl,--kill-at -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdbp_isapi_la_LIBADD = libdbpcore.la \
	@libdclnet_LIBS@
libdbp_isapi_la_SOURCES = application/dbp_isapi.cpp
endif

# FastCGI server
if WITH_FCGI
bin_PROGRAMS += dbpagerd
dbpagerd_CPPFLAGS = -I../fcgi-2.4.1-SNAP-0311112127/include
dbpagerd_LDADD = libdbpcore.la \
	../fcgi-2.4.1-SNAP-0311112127/libfcgi/libfcgi.la
dbpagerd_SOURCES = application/dbpagerd.cpp
endif

EXTRA_DIST = */*.h
