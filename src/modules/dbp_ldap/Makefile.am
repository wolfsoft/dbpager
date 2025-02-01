# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_LDAP

pkglib_LTLIBRARIES = libdbp_ldap.la

libdbp_ldap_la_SOURCES = \
	dbp_ldap_factory.cpp \
	tag_connection.cpp \
	tag_bind.cpp \
	tag_search.cpp \
	ldap_client.cpp

libdbp_ldap_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_ldap_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	@libldap_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	@libldap_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_ldap.conf

EXTRA_DIST = *.h

endif
