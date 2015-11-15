# Include common build rules
include $(top_srcdir)/Makefile.rules

SUBDIRS = include

lib_LTLIBRARIES = libdbpager.la

libdbpager_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdbpager_la_LIBADD = @libdclbase_LIBS@
libdbpager_la_SOURCES = \
	context.cpp \
	tag_impl.cpp \
	tag_usr.cpp \
	tag_tag.cpp

AM_CXXFLAGS += \
	-I../libdbpager/include \
	@libdclbase_CFLAGS@

