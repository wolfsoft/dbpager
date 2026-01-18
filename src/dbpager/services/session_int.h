/*
 * session_int.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko
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

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <dcl/dclbase.h>

#include <dbpager/session.h>

namespace dbpager {

class session_holder_int: public session_holder {
public:
	session_holder_int() {
		id = dbp::uuid().str();
		is_new = true;
	};

	session_holder_int(const std::string &session_id) {
		id = session_id;
		if (id.empty()) {
			id = dbp::uuid().str();
			is_new = true;
		}
	};

	virtual void load(context &ctx) override {
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

	virtual void save(const context &ctx, dbp::http_response &resp) override {
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
			/*
			if (session->get_value("SESSION_PERSISTENT") == string("1")) {
				dbp::datetime d;
				d.year(2038).month(1).day(1).hour(0).minute(0).second(0);
				c.expires = d;
			}*/
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

	std::string get_value(const std::string &key) {
		dbp::rwlock_guard_read cs(m);
		const app_cache::const_iterator it = buffer.find(key);
		if (it == buffer.end())
			return std::string("");
		else
			return it->second;
	};
	void put_value(const std::string &key, const std::string &value) {
		dbp::rwlock_guard_write cs(m);
		buffer[key] = value;
	};
private:
	typedef std::unordered_map<std::string, std::string> app_cache;
	dbp::rwlock m;
	app_cache buffer;
};

class session_int: public session_factory {
	virtual std::unique_ptr<session_holder> create_session(const dbp::http_request &req) override {
		const dbp::http_cookies &c = req.get_cookies();
		for (dbp::http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
			if (i->name == "session") {
				session_holder_int* rslt = new session_holder_int(i->value);
				rslt->is_https = req.get_https();
				return std::unique_ptr<session_holder>(rslt);
			}
		}

		session_holder_int* rslt = new session_holder_int();
		rslt->is_https = req.get_https();
		return std::unique_ptr<session_holder>(rslt);
	}
};

} // namespace
