# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_MQTT

pkglib_LTLIBRARIES = libdbp_mqtt.la

libdbp_mqtt_la_SOURCES = \
	dbp_mqtt_factory.cpp \
	tag_connection.cpp \
	tag_pub.cpp \
	tag_sub.cpp

libdbp_mqtt_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined

libdbp_mqtt_la_LIBADD = \
	../../libdbpager/libdbpager.la \
	@LIBMQTT_LIBS@ \
	@libdclbase_LIBS@

AM_CXXFLAGS += \
	-I../../libdbpager/include \
	@LIBMQTT_CFLAGS@ \
	@libdclbase_CFLAGS@

confdir = $(sysconfdir)/dbpager/modules-available
dist_conf_DATA = dbp_mqtt.conf

EXTRA_DIST = *.h

endif

