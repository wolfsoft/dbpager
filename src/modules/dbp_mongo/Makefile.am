# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_MONGO

noinst_LIBRARIES = libmongoclient.a

libmongoclient.a:
	cd ./mongo-cxx-driver-nightly/ && $(SCONS) --prefix $(abs_srcdir)/mongo-cxx-driver-nightly/root install

clean-local:
	cd ./mongo-cxx-driver-nightly/ && $(SCONS) --clean && rm -rf build/ && rm -rf root/ && rm -f *.log && rm -rf .sconf_temp && rm -f .sconsign.dblite

pkglib_LTLIBRARIES = libdbp_mongo.la

libdbp_mongo_la_SOURCES = \
	dbp_mongo_factory.cpp \
	tag_database.cpp \
	tag_param.cpp \
	tag_query.cpp \
	tag_insert.cpp \
	tag_update.cpp \
	tag_delete.cpp

libdbp_mongo_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_mongo_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@libdclbase_LIBS@ \
	./mongo-cxx-driver-nightly/root/lib/libmongoclient.a

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@libdclbase_CFLAGS@ \
	-I./mongo-cxx-driver-nightly/root/include/mongo \
	-I./mongo-cxx-driver-nightly/root/include

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_mongo.conf

EXTRA_DIST = *.h

endif

