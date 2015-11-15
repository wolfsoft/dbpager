/*
 * memcached_cache.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko
 *
 * dbPager Server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * dbPager Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dbPager Server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <time.h>

#include <dcl/strutils.h>

#include "memcached_cache.h"

namespace dbpager {

using namespace std;
using namespace dbp;

memcached_cache::memcached_cache(): servers(NULL), ttl(0) {
	servers = memcached_create(NULL);
}

memcached_cache::~memcached_cache() {
	if (servers)
		memcached_free(servers);
}

void memcached_cache::setup(const dbp::app_config &options) {
	strings s = tokenize()(options.value("cache.memcached", "servers",
	  string("localhost")));
	for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
		strings hp = tokenize()(*i, ":");
		int port = 11211;
		if (hp.size() > 1)
			port = from_string<int>(trim()(hp[1]));
		memcached_server_add(servers, trim()(hp[0]).c_str(), port);
	}
	ttl = options.value("cache", "ttl", 0);
}

bool memcached_cache::get(context &ctx, const std::string &id) {
	if (id.empty())
		return false;
	size_t vlen = 0;
	uint32_t flags = 0;
	memcached_return error;
	char *value = memcached_get(servers, id.c_str(), id.length(),
	  &vlen, &flags, &error);
	if (error == MEMCACHED_SUCCESS && value) {
		strings s = tokenize()(value);
		for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
			string k, v;
			tokenize()(*i, k, v, "=");
			if (!k.empty()) {
				ctx.add_value(k, v, "user");
			}
		}
		free(value);
		return true;
	}
	return false;
}

void memcached_cache::put(context &ctx, const std::string &id) {
	if (id.empty())
		return;
	string s;
	context::variables c = ctx.get_values();
	context::variables::const_iterator i = c.begin();
	while (i != c.end()) {
		s += i->first + "=" + i->second;
		++i;
		if (i != c.end())
			s += ";";
	}
	if (!s.empty()) {
		time_t exp = ttl;
		memcached_set(servers, id.c_str(), id.length(),
		  s.c_str(), s.length(), exp, 0);
	}
}

}

