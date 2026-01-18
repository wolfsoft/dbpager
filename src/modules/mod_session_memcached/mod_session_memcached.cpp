/*
 * mod_session_memcached.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
 *
 * dbPager Server is free software; you can memcachedtribute it and/or modify
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

#include <sstream>

#include <libmemcached/memcached.h>

#include <dcl/dclbase.h>

#include "mod_session_memcached.h"

namespace dbpager {

using namespace std;
using namespace dbp;

class memcached_connection {
public:
	memcached_connection(): mc(NULL), configured(false) {
		mc = memcached_create(NULL);
	}

	virtual ~memcached_connection() {
		if (mc) memcached_free(mc);
	}

	bool is_configured() {
		return configured;
	}

	void configure(const std::string &host, int port) {
		memcached_server_add(mc, host.c_str(), port);
		configured = true;
	}

	void reset() {
		if (mc) memcached_free(mc);
		mc = NULL;
		configured = false;
	}

	memcached_st* get_ptr() {
		return mc;
	}

private:
	memcached_st *mc;
	bool configured;
};


class memcached_pool: public dbp::singleton<memcached_pool> {
	friend class dbp::singleton<memcached_pool>;
public:
	void reset(std::unique_ptr<dbp::pool_size_policy> ps) {
		delete pool;
		pool = new dbp::pool<memcached_connection>(std::move(ps));
	}

	dbp::pool_ptr<memcached_connection> acquire(const std::string &arg) {
		return pool->acquire(arg);
	}

	virtual ~memcached_pool() {
		delete pool;
	}

private:
	memcached_pool() {
		pool = new dbp::pool<memcached_connection>();
	}

	dbp::pool<memcached_connection> *pool;
};

void mod_session_memcached::load(context &ctx) {
	const std::string &value = get_value(id);
	dbp::strings s = dbp::tokenize()(value, ";");
	for (dbp::strings::const_iterator i = s.begin(); i != s.end(); ++i) {
		std::string k, v;
		dbp::tokenize()(*i, k, v, false, "=");
		if (!k.empty()) {
			ctx.add_value(k, v);
		}
	}
}

void mod_session_memcached::save(const context &ctx, dbp::http_response &resp) {
	if (!ctx.empty()) {
		std::string s;
		context::variables c = ctx.get_values();
		context::variables::const_iterator i = c.begin();
		while (i != c.end()) {
			s += i->first + "=" + i->second;
			++i;
			if (i != c.end())
				s += ";";
		}
		put_value(id, s);
	}

	// setup cookies
	if (is_new && !ctx.empty()) {
		dbp::http_cookies cs;
		dbp::http_cookie c("session", id);
		c.path = "/";
		c.http_only = true;
		if (is_https) {
			c.same_site = "none";
			c.secure = true;
			c.partitioned = true;
		}
		cs.push_back(c);
		resp.set_cookies(cs);
	}
	if (id.empty()) {
		dbp::http_cookie c("session", "");
		c.path = "/";
		c.http_only = true;
		if (is_https) {
			c.same_site = "none";
			c.secure = true;
			c.partitioned = true;
		}
		dbp::datetime d;
		d.year(1976).month(4).day(21).hour(0).minute(0).second(0);
		c.expires = d;
		resp.set_cookie(c);
	}
}

std::string mod_session_memcached::get_value(const std::string &key) {
	pool_ptr<memcached_connection> c = memcached_pool::instance().acquire(servers);
	try {
		if (!c->is_configured()) {
			strings s = tokenize()(servers);
			for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
				strings hp = tokenize()(*i, ":");
				int port = 11211;
				if (hp.size() > 1)
					port = from_string<int>(trim()(hp[1]));
				c->configure(trim()(hp[0]), port);
			}
		}

		size_t vlen = 0;
		uint32_t flags = 0;
		memcached_return error;

		char *value = memcached_get(c->get_ptr(), key.c_str(), key.length(), &vlen, &flags, &error);

		if (!value && error != MEMCACHED_SUCCESS && error != MEMCACHED_NOTFOUND) {
			string rslt(memcached_strerror(c->get_ptr(), error));
			throw mod_session_memcached_exception(rslt);
		}

		if (value) {
			string rslt(value);
			free(value);
			return rslt;
		} else {
			return "";
		}

	} catch (...) {
		c->reset();
		throw;
	}
}

void mod_session_memcached::put_value(const std::string &key, const std::string &value) {
	pool_ptr<memcached_connection> c = memcached_pool::instance().acquire(servers);
	try {
		if (!c->is_configured()) {
			strings s = tokenize()(servers);
			for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
				strings hp = tokenize()(*i, ":");
				int port = 11211;
				if (hp.size() > 1)
					port = from_string<int>(trim()(hp[1]));
				c->configure(trim()(hp[0]), port);
			}
		}

		memcached_return_t error = memcached_set(c->get_ptr(), key.c_str(), key.length(), value.c_str(), value.length(), ttl, 0);
		if (error != MEMCACHED_SUCCESS) {
			string rslt(memcached_strerror(c->get_ptr(), error));
			throw mod_session_memcached_exception(rslt);
		}

	} catch (...) {
		c->reset();
		throw;
	}

}

std::unique_ptr<session_holder> mod_session_memcached_factory::create_session(const dbp::http_request &req) {
	const dbp::http_cookies &c = req.get_cookies();
	for (dbp::http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
		if (i->name == "session") {
			mod_session_memcached* rslt = new mod_session_memcached(i->value);
			rslt->is_https = req.get_https();
			rslt->set_ttl(ttl);
			rslt->set_servers(servers);
			return std::unique_ptr<session_holder>(rslt);
		}
	}

	mod_session_memcached* rslt = new mod_session_memcached();
	rslt->is_https = req.get_https();
	rslt->set_ttl(ttl);
	rslt->set_servers(servers);
	return std::unique_ptr<session_holder>(rslt);
}

} // namespace

