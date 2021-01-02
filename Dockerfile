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

FROM debian:buster AS builder

WORKDIR /root

# Copy full source code
COPY . dbpager/

# Install dependencies, compile from sources, cleaning up
# N.B.: gcc 8 is broken (dbp::system segfaults)
RUN export DEBIAN_FRONTEND=noninteractive && export LANG=C \
	&& export CXXFLAGS="-O3 -pipe" \
	&& export CC="gcc-7" \
	&& export CXX="g++-7" \
	&& echo "deb http://deb.debian.org/debian buster main contrib non-free" > /etc/apt/sources.list \
	&& echo "deb http://deb.debian.org/debian buster-updates main contrib non-free" >> /etc/apt/sources.list \
	&& echo "deb http://security.debian.org buster/updates main contrib non-free" >> /etc/apt/sources.list \
	\
	&& apt-get update && apt-get -y upgrade && apt-get -y install wget gnupg \
	&& wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - \
	&& echo "deb http://apt.postgresql.org/pub/repos/apt/ buster-pgdg main" >> /etc/apt/sources.list \
	\
	&& apt-get update && apt-get -y install gcc-7 g++-7 \
	&& apt-get -y install git autopoint libtool automake pkg-config gettext autoconf autotools-dev xsltproc scons build-essential \
	\
	&& apt-get -y install netbase libssl-dev unixodbc-dev \
	\
	&& git clone https://github.com/wolfsoft/libdcl.git \
	&& cd libdcl && ./autogen.sh && ./configure --disable-gtk --disable-qt --disable-winapi --disable-doxygen-doc --without-apache && make -j`nproc` && make install && cd .. \
	\
	&& apt-get -y install libpq-dev libpqxx-dev libjsoncpp-dev libdb++-dev libsqlite3-dev libxml2-dev libxslt1-dev libpcre3-dev libpcrecpp0v5 libcurl4-openssl-dev libmemcached-dev libevent-dev uuid-dev libboost-thread-dev libboost-filesystem-dev libboost-system-dev libhiredis-dev libmosquitto-dev libmosquittopp-dev \
	\
	&& cd dbpager && ./autogen.sh && ./configure --disable-dbp_cgi --disable-mod_dbp --disable-dbp_isapi --disable-dbp_mongo --disable-dbp_script && make -j`nproc` && make install && cd .. \
	\
	&& find /usr/local/ -type f -exec strip -s '{}' 2>/dev/null ';' \
	&& ldconfig

# System configuration
RUN sed -i "s;^# bind = .*$;bind = :9000;g" /usr/local/etc/dbpager/dbpager.conf

FROM debian:buster

COPY --from=builder /usr/local/ /usr/local/

RUN export DEBIAN_FRONTEND=noninteractive && export LANG=C \
	&& echo "deb http://deb.debian.org/debian buster main contrib non-free" > /etc/apt/sources.list \
	&& echo "deb http://deb.debian.org/debian buster-updates main contrib non-free" >> /etc/apt/sources.list \
	&& echo "deb http://security.debian.org buster/updates main contrib non-free" >> /etc/apt/sources.list \
	\
	&& apt-get update && apt-get -y upgrade && apt-get -y install wget gnupg \
	&& wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - \
	&& echo "deb http://apt.postgresql.org/pub/repos/apt/ buster-pgdg main" >> /etc/apt/sources.list \
	\
	&& apt-get update && apt-get -y upgrade \
	\
	&& apt-get -y install netbase libssl-dev unixodbc-dev \
	&& apt-get -y install libpq-dev libpqxx-dev libjsoncpp-dev libdb++-dev libsqlite3-dev libxml2-dev libxslt1-dev libpcre3-dev libpcrecpp0v5 libcurl4-openssl-dev libmemcached-dev libevent-dev uuid-dev libboost-thread-dev libboost-filesystem-dev libboost-system-dev libhiredis-dev libmosquitto-dev libmosquittopp-dev \
	&& apt-get -y remove wget gnupg *-dev *-doc \
	&& apt-mark manual `dpkg -l | awk '($1 == "ii") && ($2 ~ /^lib|lib$/) { print $2 }'` \
	&& apt-get -y autoremove --purge \
	\
	&& apt-get -y clean \
	&& rm -rf /tmp/* \
	&& rm -rf /var/tmp/* \
	&& ldconfig

# FastCGI port
EXPOSE 9000

# Entrypoint
CMD /usr/local/bin/dbpagerd -p /var/run/dbpager.pid && tail -f /dev/null --pid=`cat /var/run/dbpager.pid`
