#
# SYNOPSIS
#
#   AX_WITH_APACHE()
#
# DESCRIPTION
#
#   Locates installed apache headers.
#
# LAST MODIFICATION
#
#   2008-07-18
#
# COPYRIGHT
#
#   Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
#
#   This is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation version 3.
#
#   dbPager Server is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with dbPager Server; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor,
#   Boston, MA  02110-1301  USA

AC_DEFUN([AX_WITH_APACHE],
[

AC_ARG_VAR([APXS],[path to the apxs utility])
AC_ARG_VAR([APR],[path to the apr utility])

# searching for apache utilites if not specified by environment variables

if test "$use_apache" != "no"; then

use_apache="no"

if test "x$APXS" = "x"; then
	AC_PATH_PROG(APXS, apxs)
fi

if test "x$APXS" = "x"; then
	AC_PATH_PROG(APXS, apxs2)
fi

if test "x$APR" = "x"; then
	AC_PATH_PROG(APR, apr-config)
fi

if test "x$APXS" != "x"; then
	if test "x$APR" != "x"; then
		APACHE_INCLUDE=`$APXS -q INCLUDEDIR`
		APR_INCLUDE=`$APR --includedir`
		APACHE_CFLAGS="-I$APACHE_INCLUDE -I$APR_INCLUDE"
		APACHE_LIBS=`$APR --link-ld --libs`
		APACHE_LIBTOOL=`$APR --link-libtool`
		CPPFLAGS_SAVE=$CPPFLAGS
		CPPFLAGS="$APACHE_CFLAGS $CPPFLAGS"
		AC_CHECK_HEADER([httpd.h], [use_apache="yes"], [use_apache="no"])
		CPPFLAGS=$CPPFLAGS_SAVE
	fi
fi

fi

AM_CONDITIONAL(WITH_APACHE, test "$use_apache" = "yes")

AC_SUBST(APACHE_CFLAGS)
AC_SUBST(APACHE_LIBS)
AC_SUBST(APACHE_LIBTOOL)

])

