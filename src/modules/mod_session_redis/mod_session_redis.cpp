/*
 * mod_session_redis.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <sstream>

#include <hiredis/hiredis.h>

#include <dcl/dclbase.h>

#include "mod_session_redis.h"
#include "../dbp_redis/redis_connection.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void mod_session_redis::load(context &ctx) {
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

void mod_session_redis::save(const context &ctx, dbp::http_response &resp) {
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

std::string mod_session_redis::get_value(const std::string &key) {
	int attempts = 3;
	string msg = "Unknown error";
	while (attempts) {
		pool_ptr<redis_connection> c = redis_pool::instance().acquire(server);
		if (!c->is_configured()) {
			c->configure(server);
			if (!password.empty())
				c->login(password);
			if (database_number > 0)
				c->select_database(database_number);
		}

		redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "GET %s", key.c_str());
		if (reply) {
			std::string rslt = (reply->type == REDIS_REPLY_STRING && reply->str) ? string(reply->str) : string("");
			freeReplyObject(reply);
			return rslt;
		} else {
			msg = string(c->get_ptr()->errstr);
			c->reset();
			attempts--;
		}
	}
	throw mod_session_redis_exception(msg);
}

void mod_session_redis::put_value(const std::string &key, const std::string &value) {
	int attempts = 3;
	string msg = "Unknown error";
	while (attempts) {
		pool_ptr<redis_connection> c = redis_pool::instance().acquire(server);
		if (!c->is_configured()) {
			c->configure(server);
			if (!password.empty())
				c->login(password);
			if (database_number > 0)
				c->select_database(database_number);
		}

		redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "SET %s %s", key.c_str(), value.c_str());
		if (reply) {
			freeReplyObject(reply);
		} else {
			msg = string(c->get_ptr()->errstr);
			c->reset();
			attempts--;
			continue;
		}

		if (ttl > 0) {
			redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "EXPIRE %s %d", key.c_str(), ttl);
			if (reply) {
				freeReplyObject(reply);
			} else {
				msg = string(c->get_ptr()->errstr);
				c->reset();
				attempts--;
				continue;
			}
		}

		return;
	}

	throw mod_session_redis_exception(msg);
}

std::unique_ptr<session_holder> mod_session_redis_factory::create_session(const dbp::http_request &req) {
	const dbp::http_cookies &c = req.get_cookies();
	for (dbp::http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
		if (i->name == "session") {
			mod_session_redis* rslt = new mod_session_redis(i->value);
			rslt->is_https = req.get_https();
			rslt->ttl = ttl;
			rslt->database_number = database_number;
			rslt->server = server;
			rslt->password = password;
			return std::unique_ptr<session_holder>(rslt);
		}
	}

	mod_session_redis* rslt = new mod_session_redis();
	rslt->is_https = req.get_https();
	rslt->ttl = ttl;
	rslt->database_number = database_number;
	rslt->server = server;
	rslt->password = password;
	return std::unique_ptr<session_holder>(rslt);
}

} // namespace
