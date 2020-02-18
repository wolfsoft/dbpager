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

FROM debian:stretch AS builder

MAINTAINER Dennis Prochko <wolfsoft@mail.ru>

WORKDIR /root

# Copy full source code
COPY . dbpager/

# Install dependencies, compile from sources, cleaning up
RUN export DEBIAN_FRONTEND=noninteractive \
	&& export CXXFLAGS="-O3" \
	&& echo "deb http://deb.debian.org/debian stretch main contrib non-free" > /etc/apt/sources.list \
	&& echo "deb http://deb.debian.org/debian stretch-updates main contrib non-free" >> /etc/apt/sources.list \
	&& echo "deb http://security.debian.org stretch/updates main contrib non-free" >> /etc/apt/sources.list \
	&& apt-get update && apt-get -y dist-upgrade \
	&& apt-get -y install git autopoint libtool automake pkg-config gettext autoconf autotools-dev xsltproc scons build-essential \
	\
	&& apt-get -y install netbase libssl-dev unixodbc-dev \
	&& git clone https://github.com/wolfsoft/libdcl.git \
	&& cd libdcl && ./autogen.sh && ./configure --disable-gtk --disable-qt --disable-winapi --disable-doxygen-doc --without-apache && make -j8 && make install && cd .. \
	\
	&& apt-get -y install libjsoncpp-dev libdb++-dev libsqlite3-dev libxml2-dev libxslt1-dev libpcre3-dev libcurl4-openssl-dev libmemcached-dev libevent-dev uuid-dev libboost-thread-dev libboost-filesystem-dev libboost-system-dev libpqxx-dev libhiredis-dev libmosquitto-dev libmosquittopp-dev \
	&& cd dbpager && ./autogen.sh && ./configure --disable-dbp_cgi --disable-mod_dbp --disable-dbp_odbc --disable-dbp_isapi --disable-dbp_gearman --disable-dbp_mongo --disable-dbp_script && make -j8 && make install && cd .. \
	\
	&& find /usr/local/ -type f -exec strip -s '{}' 2>/dev/null ';'

# System configuration
RUN sed -i "s;^# bind = .*$;bind = :9000;g" /usr/local/etc/dbpager/dbpager.conf

FROM debian:stretch

COPY --from=builder /usr/local/ /usr/local/

RUN export DEBIAN_FRONTEND=noninteractive \
	&& echo "deb http://deb.debian.org/debian stretch main contrib non-free" > /etc/apt/sources.list \
	&& echo "deb http://deb.debian.org/debian stretch-updates main contrib non-free" >> /etc/apt/sources.list \
	&& echo "deb http://security.debian.org stretch/updates main contrib non-free" >> /etc/apt/sources.list \
	&& apt-get update && apt-get -y dist-upgrade \
	&& apt-get -y install libstdc++6 libgcc1 libapr1 libssl1.1 libcurl3 libjsoncpp1 libpcre3 libpcrecpp0v5 libxml2 libsqlite3-0 libmemcached11 libmemcachedutil2 libxslt1.1 libdb5.3++ libpqxx-4.0v5 libhiredis0.13 libmosquittopp1 \
	&& apt-get -y clean \
	&& rm -rf /tmp/* \
	&& rm -rf /var/tmp/* \
	&& find /usr/share/doc/ -type f -delete \
	&& find /usr/share/locale/ -type f -delete \
	&& find /usr/share/man/ -type f -delete \
	&& find /usr/share/zoneinfo/ -type f -delete \
	&& find /usr/local/share/ -type f -delete \
	&& ldconfig

# FastCGI port
EXPOSE 9000

# Entrypoint
CMD /usr/local/bin/dbpagerd && tail -F /dev/null

