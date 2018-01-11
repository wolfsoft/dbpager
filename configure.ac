#
# configure.ac
# This file is part of dbPager Server
#
# Copyright (c) 2008-2018 Dennis Prochko <wolfsoft@mail.ru>
#
# dbPager Server is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# dbPager Server is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dbPager Server; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor,
# Boston, MA  02110-1301  USA
#

# initialization
AC_INIT([dbpager], [3.2.1], [wolfsoft@mail.ru])
AC_PREREQ([2.63])
AC_CONFIG_AUX_DIR(build)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# version of the libdbpager and dbpager modules is current:revision:age
#
# current
#  The most recent interface number that this library implements.
# revision
#  The implementation number of the current interface.
# age
#  The difference between the newest and oldest interfaces that
#  this library implements. In other words, the library
#  implements all the interface numbers in the range from
#  number current - age to current.
#
# 1. Start with version information of 0:0:0 for each libtool library.
# 2. Update the version information only immediately before a public
#    release of your software. More frequent updates are unnecessary,
#    and only guarantee that the current interface number gets larger faster.
# 3. If the library source code has changed at all since the last
#    update, then increment revision (c:r:a becomes c:r+1:a).
# 4. If any interfaces have been added, removed, or changed since
#    the last update, increment current, and set revision to 0.
# 5. If any interfaces have been added since the last public
#    release, then increment age.
# 6. If any interfaces have been removed since the last public
#    release, then set age to 0.
#
ABI_CUR=7
ABI_REV=0
ABI_AGE=3
AC_SUBST(ABI_CUR)
AC_SUBST(ABI_REV)
AC_SUBST(ABI_AGE)

# automake initialization
AM_INIT_AUTOMAKE([subdir-objects])

# initialize gettext
AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])

# check for programs
AC_ISC_POSIX
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_SED
LT_PROG_RC

# libtool initialization
LT_PREREQ([2.2])
LT_INIT([dlopen win32-dll])
AC_SUBST([LIBTOOL_DEPS])

#
# define build options
#

# debugging support
AC_MSG_CHECKING([whether to build with debug information])

AC_ARG_ENABLE(debug,
	AS_HELP_STRING(
		[--enable-debug],
		[enable debug data generation (default=no)]),
	[debugger="$enableval"],
	[debugger=no]
)

AC_MSG_RESULT($debugger)

if test "$debugger" = "yes"; then
	AC_DEFINE([DEBUG], [1], [enable debug data generation])
	CXXFLAGS="$CXXFLAGS -g -fno-inline -O0"
fi

# Provide the build options

AC_ARG_ENABLE(dbp_cgi,
	AS_HELP_STRING([--enable-dbp_cgi={yes|no}],
	[build CGI application (default=yes)]),
	[use_cgi=$enableval],
	[use_cgi=yes]
)
AM_CONDITIONAL(WITH_CGI, test "$use_cgi" = "yes")

AC_ARG_ENABLE(fcgi,
	AS_HELP_STRING([--enable-fcgi={yes|no}],
	[build FastCGI server (default=yes)]),
	[use_fcgi=$enableval],
	[use_fcgi=yes]
)
AM_CONDITIONAL(WITH_FCGI, test "$use_fcgi" = "yes")

AC_ARG_ENABLE(mod_dbp,
	AS_HELP_STRING([--enable-mod_dbp={yes|no|auto}],
	[build apache module (default=auto)]),
	[use_apache=$enableval],
	[use_apache=auto]
)

AC_ARG_ENABLE(dbp_isapi,
	AS_HELP_STRING([--enable-dbp_isapi={yes|no|auto}],
	[build ISAPI module (default=auto)]),
	[use_isapi=$enableval],
	[use_isapi=auto]
)

AC_ARG_ENABLE(dbp_odbc,
	AS_HELP_STRING([--enable-dbp_odbc={yes|no|auto}],
	[build dbp_odbc plugin (default=auto)]),
	[use_odbc=$enableval],
	[use_odbc=auto]
)

AC_ARG_ENABLE(dbp_sqlite,
	AS_HELP_STRING([--enable-dbp_sqlite={yes|no|auto}],
	[build dbp_sqlite plugin (default=auto)]),
	[use_sqlite=$enableval],
	[use_sqlite=auto]
)

AC_ARG_ENABLE(mod_memcached,
	AS_HELP_STRING([--enable-memcached={yes|no|auto}],
	[build memcached plugins (default=auto)]),
	[use_memcached=$enableval],
	[use_memcached=auto]
)

AC_ARG_ENABLE(mod_redis,
	AS_HELP_STRING([--enable-redis={yes|no|auto}],
	[build redis plugins (default=auto)]),
	[use_redis=$enableval],
	[use_redis=auto]
)

AC_ARG_ENABLE(dbp_pgsql,
	AS_HELP_STRING([--enable-dbp_pgsql={yes|no|auto}],
	[build dbp_pgsql plugin (default=auto)]),
	[use_pgsql=$enableval],
	[use_pgsql=auto]
)

AC_ARG_ENABLE(dbp_xslt,
	AS_HELP_STRING([--enable-dbp_xslt={yes|no|auto}],
	[build dbp_xslt plugin (default=auto)]),
	[use_xslt=$enableval],
	[use_xslt=auto]
)

AC_ARG_ENABLE(dbp_bdb,
	AS_HELP_STRING([--enable-dbp_bdb={yes|no|auto}],
	[build dbp_bdb plugin (default=auto)]),
	[use_bdb=$enableval],
	[use_bdb=auto]
)

AC_ARG_ENABLE(dbp_gearman,
	AS_HELP_STRING([--enable-dbp_gearman={yes|no}],
	[build dbp_gearman plugin (default=no)]),
	[use_gearman=$enableval],
	[use_gearman=no]
)

AC_ARG_ENABLE(dbp_mongo,
	AS_HELP_STRING([--enable-dbp_mongo={yes|no}],
	[build dbp_mongo plugin (default=no)]),
	[use_mongo=$enableval],
	[use_mongo=no]
)

AC_ARG_ENABLE(dbp_script,
	AS_HELP_STRING([--enable-dbp_script={yes|no}],
	[build dbp_script plugin (default=auto)]),
	[use_script=$enableval],
	[use_script=auto]
)

AC_ARG_ENABLE(dbp_mqtt,
	AS_HELP_STRING([--enable-dbp_mqtt={yes|no}],
	[build dbp_mqtt plugin (default=auto)]),
	[use_mqtt=$enableval],
	[use_mqtt=auto]
)

# Checks for c++ features
AX_SHARED_PTR

# Checks for tools to build the software documentation
AC_PATH_PROG([xsltproc], [xsltproc])

if test -z $xsltproc; then
	AC_MSG_ERROR([xsltproc was not found. You should install xsltproc program\
 to compile the documentation.])
fi

AC_PATH_PROG([sed], [sed])

if test -z $sed; then
	AC_MSG_ERROR([sed was not found. You should install sed program\
 to compile the documentation.])
fi

# Checks for libraries
PKG_CHECK_MODULES(jsoncpp, [jsoncpp >= 0.6.0])

PKG_CHECK_MODULES(libdclbase, [libdclbase >= 0.2.0])

PKG_CHECK_MODULES(libdclnet, [libdclnet >= 0.2.0])

PKG_CHECK_MODULES(libmemcached, [libmemcached >= 1.0.0],
[
	if test "$use_memcached" != "no"; then
		AC_DEFINE([HAVE_MEMCACHED], [1], [enable memcached usage])
		use_memcached="yes"
	fi
],
[
	if test "$use_memcached" = "yes"; then
		AC_MSG_ERROR([libmemcached library is required but not found.])
	fi
	use_memcached="no"
])

AM_CONDITIONAL(WITH_MEMCACHED, test "$use_memcached" = "yes")


PKG_CHECK_MODULES(libhiredis, [hiredis >= 0.10.0],
[
	if test "$use_redis" != "no"; then
		AC_DEFINE([HAVE_REDIS], [1], [enable redis usage])
		use_redis="yes"
	fi
],
[
	if test "$use_redis" = "yes"; then
		AC_MSG_ERROR([libhiredis library is required but not found.])
	fi
	use_redis="no"
])

AM_CONDITIONAL(WITH_REDIS, test "$use_redis" = "yes")


PKG_CHECK_MODULES(libdclodbc, [libdclodbc >= 0.2.0],
[
	if test "$use_odbc" != "no"; then
		AC_DEFINE([HAVE_ODBC], [1], [enable odbc usage])
		use_odbc="yes"
	fi
],
[
	if test "$use_odbc" = "yes"; then
		AC_MSG_ERROR([libdclodbc library is required to dbp_odbc plugin,\
 but not found.])
	fi
	use_odbc="no"
])

AM_CONDITIONAL(WITH_ODBC, test "$use_odbc" = "yes")


PKG_CHECK_MODULES(sqlite3, [sqlite3 >= 3.8.0],
[
	if test "$use_sqlite" != "no"; then
		AC_DEFINE([HAVE_SQLITE], [1], [enable sqlite usage])
		use_sqlite="yes"
	fi
],
[
	if test "$use_sqlite" = "yes"; then
		AC_MSG_ERROR([sqlite3 library is required to dbp_sqlite plugin,\
 but not found.])
	fi
	use_sqlite="no"
])

AM_CONDITIONAL(WITH_SQLITE, test "$use_sqlite" = "yes")


PKG_CHECK_MODULES(libxml2, [libxml-2.0 >= 2.6.0])


PKG_CHECK_MODULES(libxslt, [libxslt >= 1.1.0],
[
	if test "$use_xslt" != "no"; then
		use_xslt="yes"
	fi
],
[
	if test "$use_xslt" = "yes"; then
		AC_MSG_ERROR([libxslt library is required to dbp_xslt plugin,\
 but not found.])
	fi
	use_xslt="no"
])

AM_CONDITIONAL(WITH_XSLT, test "$use_xslt" = "yes")


AC_CHECK_LIB(db_cxx, db_env_create,
[
	if test "$use_bdb" != "no"; then
		AC_LANG_PUSH([C++])
		AC_CHECK_HEADERS([db_cxx.h], [
			use_bdb="yes"
			DB_CXX_CFLAGS=""
			DB_CXX_LIBS="-ldb_cxx"
		],
		[
			if test "$use_bdb" = "yes"; then
				AC_MSG_ERROR([development files of libdb++ library\
 are required to dbp_bdb plugin, but not found.])
			fi
			use_bdb="no"
		])
		AC_LANG_POP
	fi
],
[
	if test "$use_bdb" = "yes"; then
		AC_MSG_ERROR([libdb++ library is required to dbp_bdb plugin,\
 but not found.])
	fi
	use_bdb="no"
])

AC_SUBST(DB_CXX_CFLAGS)
AC_SUBST(DB_CXX_LIBS)

AM_CONDITIONAL(WITH_BDB, test "$use_bdb" = "yes")


# check for mod_gearman option
PKG_CHECK_MODULES(libgearman, [gearmand >= 0.10],
[
	if test "$use_gearman" != "no"; then
		use_gearman="yes"
	fi
	AC_CHECK_LIB([event], [event_dispatch], [LIBEVENT=-levent],
	[
		AC_MSG_ERROR([libevent-dev library is required to dbp_gearman plugin,\
 but not found.])
		use_gearman="no"
	])
],
[
	if test "$use_gearman" = "yes"; then
		AC_MSG_ERROR([libgearman-dev library is required to dbp_gearman plugin,\
 but not found.])
	fi
	use_gearman="no"
])
AM_CONDITIONAL(WITH_GEARMAN, test "$use_gearman" = "yes")

# check for mod_mongo option
#if test "$use_mongo" != "no"; then
#	AC_CHECK_PROG([SCONS], [scons], [/usr/bin/scons])
#	AX_BOOST_BASE([1.40], [
#		AX_BOOST_THREAD()
#		AX_BOOST_FILESYSTEM()
#		AX_BOOST_SYSTEM()
#	], [
#		AC_MSG_ERROR([libboost-all-dev library is required to dbp_mongo plugin,\
# but not found.])
#	])
#	use_mongo="yes"
#fi
AM_CONDITIONAL(WITH_MONGO, test "$use_mongo" = "yes")

PKG_CHECK_MODULES(libpcrecpp, [libpcrecpp >= 7.4])

PKG_CHECK_MODULES(libcurl, [libcurl >= 7.0])

PKG_CHECK_MODULES(libpqxx, [libpqxx >= 4.0],
[
	if test "$use_pgsql" != "no"; then
		AC_DEFINE([HAVE_LIBPQXX], [1], [enable libpqxx usage])
		use_pgsql="yes"
	fi
],
[
	if test "$use_pgsql" = "yes"; then
		AC_MSG_ERROR([libpqxx library is required to dbp_pgsql plugin,\
 but not found.])
	fi
	use_pgsql="no"
])

AM_CONDITIONAL(WITH_LIBPQXX, test "$use_pgsql" = "yes")

#check mod_script option
AC_CHECK_LIB(v8, main,
[
	if test "$use_script" != "no"; then
		AC_LANG_PUSH([C++])
		AC_CHECK_HEADERS([v8.h], [
			use_script="yes"
			LIBV8_CFLAGS=""
			LIBV8_LIBS="-lv8"
		],
		[
			if test "$use_script" = "yes"; then
				AC_MSG_ERROR([development files of libv8 library\
 are required to dbp_script plugin, but not found.])
			fi
			use_script="no"
		])
		AC_LANG_POP
	fi
],
[
	if test "$use_script" = "yes"; then
		AC_MSG_ERROR([libv8 library is required to dbp_script plugin,\
 but not found.])
	fi
	use_script="no"
])

AC_SUBST(LIBV8_CFLAGS)
AC_SUBST(LIBV8_LIBS)

AM_CONDITIONAL(WITH_SCRIPT, test "$use_script" = "yes")

#check mod_mqtt option
AC_CHECK_LIB(mosquittopp, main,
[
	if test "$use_mqtt" != "no"; then
		AC_LANG_PUSH([C++])
		AC_CHECK_HEADERS([mosquittopp.h], [
			use_mqtt="yes"
			LIBMQTT_CFLAGS=""
			LIBMQTT_LIBS="-lmosquittopp"
		],
		[
			if test "$use_mqtt" = "yes"; then
				AC_MSG_ERROR([development files of mosquittopp library\
 are required to dbp_mqtt plugin, but not found.])
			fi
			use_mqtt="no"
		])
		AC_LANG_POP
	fi
],
[
	if test "$use_mqtt" = "yes"; then
		AC_MSG_ERROR([libmosquittopp library is required to dbp_mqtt plugin,\
 but not found.])
	fi
	use_mqtt="no"
])

AC_SUBST(LIBMQTT_CFLAGS)
AC_SUBST(LIBMQTT_LIBS)

AM_CONDITIONAL(WITH_MQTT, test "$use_mqtt" = "yes")

AX_WITH_APACHE()

AC_CHECK_HEADERS([windows.h],
[
	if test "$use_isapi" != "no"; then
		use_isapi="yes"
	fi
],
[
	if test "$use_isapi" = "yes"; then
		AC_MSG_ERROR([WinAPI headers are required to dbPager ISAPI module,\
 but not found.])
	fi
	use_isapi="no"
]
)

AM_CONDITIONAL(WITH_ISAPI, test "$use_isapi" = "yes")

# Setup dbpager plugins directory
if test ${prefix} = NONE; then
	PLUGINSDIR=${ac_default_prefix}/lib/dbpager
else
	PLUGINSDIR=${prefix}/lib/dbpager
fi
AC_SUBST(PLUGINSDIR)

# Setup dbpager data directory
if test ${prefix} = NONE; then
	VARDIR=${ac_default_prefix}/var
else
	VARDIR=${prefix}/var
fi
AC_SUBST(VARDIR)

# Setup dbpagerd unix socket path
AS_AC_EXPAND(UNIX_SOCKET_PATH, "\${runstatedir}/dbpagerd.sock")

# Subprojects
AC_TYPE_UINT32_T
AC_FUNC_MMAP
AC_CONFIG_SUBDIRS([src/mimetic-0.9.7])
AC_CONFIG_SUBDIRS([src/fcgi-2.4.1-SNAP-0311112127])

# Generate makefiles
AC_CONFIG_FILES([
   Makefile
   m4/Makefile
   po/Makefile.in
   doc/Makefile
   doc/guide/Makefile
   doc/man/Makefile
   lib/Makefile
   src/Makefile
   src/dbpager.conf
   src/libdbpager/include/dbpager/consts.h
   src/libdbpager/Makefile
   src/libdbpager/include/Makefile
   src/libdbpager/include/dbpager/Makefile
   src/dbpager/Makefile
   src/modules/Makefile
   src/modules/dbp_odbc/Makefile
   src/modules/dbp_odbc/dbp_odbc.conf
   src/modules/dbp_sqlite/Makefile
   src/modules/dbp_sqlite/dbp_sqlite.conf
   src/modules/dbp_pgsql/Makefile
   src/modules/dbp_pgsql/dbp_pgsql.conf
   src/modules/dbp_xslt/Makefile
   src/modules/dbp_xslt/dbp_xslt.conf
   src/modules/dbp_bdb/Makefile
   src/modules/dbp_bdb/dbp_bdb.conf
   src/modules/dbp_gearman/Makefile
   src/modules/dbp_gearman/dbp_gearman.conf
   src/modules/dbp_mongo/Makefile
   src/modules/dbp_mongo/dbp_mongo.conf
   src/modules/dbp_script/Makefile
   src/modules/dbp_script/dbp_script.conf
   src/modules/dbp_mqtt/Makefile
   src/modules/dbp_mqtt/dbp_mqtt.conf
   src/modules/mod_logger_file/Makefile
   src/modules/mod_logger_file/mod_logger_file.conf
   src/modules/mod_session_memcached/Makefile
   src/modules/mod_session_memcached/mod_session_memcached.conf
   src/modules/mod_session_redis/Makefile
   src/modules/mod_session_redis/mod_session_redis.conf
   src/modules/dbp_redis/Makefile
   src/modules/dbp_redis/dbp_redis.conf
   src/data/Makefile
   tests/Makefile
])
AC_OUTPUT
echo
echo Summary of dbPager applications to build:
echo Console application...................................yes
echo CGI application.......................................$use_cgi
echo FastCGI server........................................$use_fcgi
echo ISAPI module..........................................$use_isapi
echo Apache module.........................................$use_apache
echo Summary of dbPager modules \(plugins\) to build:
echo ODBC module...........................................$use_odbc
echo Sqlite module.........................................$use_sqlite
echo Memcached module......................................$use_memcached
echo Redis module..........................................$use_redis
echo PostgreSQL module.....................................$use_pgsql
echo XSLT module...........................................$use_xslt
echo Berkeley DB module....................................$use_bdb
echo Gearman module........................................$use_gearman
echo Mongo module..........................................$use_mongo
echo Script module.........................................$use_script
echo MQTT module...........................................$use_mqtt
echo
echo Type \`make\` to compile dbPager software with these features
echo
