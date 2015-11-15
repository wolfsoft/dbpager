# Include common build rules
include $(top_srcdir)/Makefile.rules

# Static library with common dbpager features
noinst_LTLIBRARIES = libdbpcore.la
libdbpcore_la_LDFLAGS = -static -no-undefined
libdbpcore_la_CPPFLAGS = \
	-fPIC \
	-I@top_builddir@/src/dbpager \
	-I@top_builddir@/src/dbpager/../mimetic-0.9.7 \
	-I@top_builddir@/src/dbpager/../libmemcached-0.44 \
	@libdclbase_CFLAGS@ \
	@libdclnet_CFLAGS@ \
	@libxml2_CFLAGS@ \
	@libpcrecpp_CFLAGS@

libdbpcore_la_SOURCES = \
	@top_builddir@/src/dbpager/parser/dbpx_parser.cpp \
	@top_builddir@/src/dbpager/optimizer/optimizer.cpp \
	@top_builddir@/src/dbpager/interpreter/environment.cpp \
	@top_builddir@/src/dbpager/interpreter/interpreter.cpp \
	@top_builddir@/src/dbpager/pool/memcached_cache.cpp \
	@top_builddir@/src/dbpager/tag/tag_factory.cpp \
	@top_builddir@/src/dbpager/tag/tag_execute.cpp \
	@top_builddir@/src/dbpager/tag/tag_unknown.cpp \
	@top_builddir@/src/dbpager/tag/tag_var.cpp \
	@top_builddir@/src/dbpager/tag/tag_set.cpp \
	@top_builddir@/src/dbpager/tag/tag_try.cpp \
	@top_builddir@/src/dbpager/tag/tag_catch.cpp \
	@top_builddir@/src/dbpager/tag/tag_throw.cpp \
	@top_builddir@/src/dbpager/tag/tag_case.cpp \
	@top_builddir@/src/dbpager/tag/tag_switch.cpp \
	@top_builddir@/src/dbpager/tag/tag_while.cpp \
	@top_builddir@/src/dbpager/tag/tag_split.cpp \
	@top_builddir@/src/dbpager/tag/tag_substr.cpp \
	@top_builddir@/src/dbpager/tag/tag_regexp.cpp \
	@top_builddir@/src/dbpager/tag/tag_expression.cpp \
	@top_builddir@/src/dbpager/tag/tag_read.cpp \
	@top_builddir@/src/dbpager/tag/tag_write.cpp \
	@top_builddir@/src/dbpager/tag/tag_locale.cpp \
	@top_builddir@/src/dbpager/tag/tag_system.cpp \
	@top_builddir@/src/dbpager/tag/tag_delete.cpp \
	@top_builddir@/src/dbpager/tag/tag_rename.cpp \
	@top_builddir@/src/dbpager/tag/tag_log.cpp \
	@top_builddir@/src/dbpager/tag/functions.cpp

libdbpcore_la_LIBADD = \
	@top_builddir@/src/dbpager/../libdbpager/libdbpager.la \
	@top_builddir@/src/dbpager/../mimetic-0.9.7/mimetic/libmimetic.la \
	@top_builddir@/src/dbpager/../libmemcached-0.44/libmemcached/libmemcached.la \
	@libdclbase_LIBS@ \
	@libdclnet_LIBS@ \
	@libxml2_LIBS@ \
	@libpcrecpp_LIBS@

check_PROGRAMS = \
	test_functions \
	test_tag_regexp \
	test_parser \
	test_execute \
	test_interpreter

test_functions_SOURCES = test_functions.cpp
test_functions_LDADD = libdbpcore.la

test_tag_regexp_SOURCES = test_tag_regexp.cpp
test_tag_regexp_LDADD =  libdbpcore.la

test_parser_SOURCES = test_parser.cpp
test_parser_LDADD = libdbpcore.la

test_execute_SOURCES = test_execute.cpp
test_execute_LDADD = libdbpcore.la

test_interpreter_SOURCES = test_interpreter.cpp
test_interpreter_LDADD = libdbpcore.la

TESTS = \
	test_functions \
	test_tag_regexp \
	test_parser \
	test_execute \
	test_interpreter

TEST_FILE_PATH = $(abs_top_srcdir)/tests
DEFS += -DTEST_FILE_PATH=\"$(TEST_FILE_PATH)\"

EXTRA_DIST = *.dbpx *.conf

