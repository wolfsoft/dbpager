# Include common build rules
include ../Makefile.rules

# Static library with common dbpager features
noinst_LTLIBRARIES = libdbpcore.la
libdbpcore_la_LDFLAGS = -static -no-undefined
libdbpcore_la_CPPFLAGS = \
	-fPIC \
	-I../src/dbpager \
	-I../src/dbpager/../mimetic-0.9.7 \
	@libdclbase_CFLAGS@ \
	@libdclnet_CFLAGS@ \
	@libxml2_CFLAGS@ \
	@libpcrecpp_CFLAGS@ \
	@libcurl_CFLAGS@ \
	@jsoncpp_CFLAGS@

libdbpcore_la_SOURCES = \
	../src/dbpager/parser/dbpx_parser.cpp \
	../src/dbpager/optimizer/optimizer.cpp \
	../src/dbpager/interpreter/environment.cpp \
	../src/dbpager/interpreter/dbpager_application.cpp \
	../src/dbpager/interpreter/interpreter.cpp \
	../src/dbpager/tag/tag_factory.cpp \
	../src/dbpager/tag/tag_call.cpp \
	../src/dbpager/tag/tag_execute.cpp \
	../src/dbpager/tag/tag_unknown.cpp \
	../src/dbpager/tag/tag_var.cpp \
	../src/dbpager/tag/tag_map.cpp \
	../src/dbpager/tag/tag_list.cpp \
	../src/dbpager/tag/tag_set.cpp \
	../src/dbpager/tag/tag_try.cpp \
	../src/dbpager/tag/tag_catch.cpp \
	../src/dbpager/tag/tag_throw.cpp \
	../src/dbpager/tag/tag_case.cpp \
	../src/dbpager/tag/tag_switch.cpp \
	../src/dbpager/tag/tag_while.cpp \
	../src/dbpager/tag/tag_split.cpp \
	../src/dbpager/tag/tag_substr.cpp \
	../src/dbpager/tag/tag_regexp.cpp \
	../src/dbpager/tag/tag_expression.cpp \
	../src/dbpager/tag/tag_read.cpp \
	../src/dbpager/tag/tag_write.cpp \
	../src/dbpager/tag/tag_locale.cpp \
	../src/dbpager/tag/tag_system.cpp \
	../src/dbpager/tag/tag_delete.cpp \
	../src/dbpager/tag/tag_rename.cpp \
	../src/dbpager/tag/tag_log.cpp \
	../src/dbpager/tag/tag_http.cpp \
	../src/dbpager/tag/functions.cpp

libdbpcore_la_LIBADD = \
	../src/dbpager/../libdbpager/libdbpager.la \
	../src/dbpager/../mimetic-0.9.7/mimetic/libmimetic.la \
	@libdclbase_LIBS@ \
	@libdclnet_LIBS@ \
	@libxml2_LIBS@ \
	@libpcrecpp_LIBS@ \
	@libcurl_LIBS@ \
	@jsoncpp_LIBS@ \
	-lcrypt

check_PROGRAMS = \
	test_functions \
	test_tag_regexp \
	test_parser \
	test_interpreter

test_functions_SOURCES = test_functions.cpp
test_functions_LDADD = libdbpcore.la

test_tag_regexp_SOURCES = test_tag_regexp.cpp
test_tag_regexp_LDADD =  libdbpcore.la

test_parser_SOURCES = test_parser.cpp
test_parser_LDADD = libdbpcore.la

test_interpreter_SOURCES = test_interpreter.cpp
test_interpreter_LDADD = libdbpcore.la

TESTS = \
	test_functions \
	test_tag_regexp \
	test_parser \
	test_interpreter

TEST_FILE_PATH = $(abs_top_srcdir)/tests
DEFS += -DTEST_FILE_PATH=\"$(TEST_FILE_PATH)\"

EXTRA_DIST = *.dbpx *.conf

