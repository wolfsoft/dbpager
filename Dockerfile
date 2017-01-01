# dbPager Server Docker Container
#
# This file is part of dbPager Server
#
# Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
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

FROM debian:latest

MAINTAINER Dennis Prochko <wolfsoft@mail.ru>

WORKDIR /root

# Copy full source code
COPY . dbpager/

# Install dependencies, compile from sources, cleaning up
RUN export DEBIAN_FRONTEND=noninteractive \
	&& export CXXFLAGS="-O3" \
	&& echo "deb http://mirror.yandex.ru/debian jessie main contrib non-free" > /etc/apt/sources.list \
	&& echo "deb http://mirror.yandex.ru/debian jessie-updates main contrib non-free" >> /etc/apt/sources.list \
	&& echo "deb http://security.debian.org jessie/updates main contrib non-free" >> /etc/apt/sources.list \
	&& apt-get update && apt-get -y dist-upgrade \
	&& apt-get -y install git autopoint libtool automake pkg-config gettext autoconf autotools-dev xsltproc scons build-essential \
	\
	&& apt-get -y install libssl-dev unixodbc-dev \
	&& git clone https://github.com/wolfsoft/libdcl.git \
	&& cd libdcl && ./autogen.sh && ./configure --disable-gtk --disable-qt --disable-winapi --disable-doxygen-doc --without-apache && make -j4 && make install && cd .. \
	\
	&& apt-get -y install libjsoncpp-dev libdb++-dev libsqlite3-dev libxml2-dev libxslt1-dev libpcre3-dev libcurl4-openssl-dev libmemcached-dev libgearman-dev libevent-dev uuid-dev libboost-thread-dev libboost-filesystem-dev libboost-system-dev libpqxx-dev libv8-dev libhiredis-dev libmosquitto-dev libmosquittopp-dev \
	&& cd dbpager && ./autogen.sh && ./configure --disable-dbp_cgi --disable-mod_dbp --disable-dbp_odbc --disable-dbp_isapi && make -j4 && make install && cd .. \
	\
	&& find /usr/local/ -type f -exec strip -s '{}' 2>/dev/null ';' \
	\
	&& apt-get -y purge git autopoint libtool automake pkg-config gettext autoconf autotools-dev xsltproc scons build-essential \
	&& apt-get -y purge manpages perl-modules unixodbc-dev \
	&& apt-get -y autoremove --purge \
	&& apt-get -y install `dpkg-query -f '${binary:Package}\n' -W|grep -e '^lib'|grep -v dev` \
	&& apt-get -y purge .\*-dev \
	&& apt-get -y autoremove --purge \
	&& apt-get -y clean \
	&& rm -rf /root/* \
	&& rm -rf /var/lib/apt/lists/* \
	&& rm -rf /tmp/* \
	&& rm -rf /var/tmp/* \
	&& find /usr/share/doc/ -type f -delete \
	&& find /usr/share/locale/ -type f -delete \
	&& find /usr/share/man/ -type f -delete \
	&& find /usr/share/zoneinfo/ -type f -delete \
	&& find /var/cache/ -type f -delete \
	&& find /var/log/ -type f -delete

# System configuration
RUN sed -i "s;^# bind = .*$;bind = :9000;g" /usr/local/etc/dbpager/dbpager.conf

# FastCGI port
EXPOSE 9000

# Entrypoint
CMD /usr/local/bin/dbpagerd && tail -F /dev/null

