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

namespace dbpager {

using namespace std;
using namespace dbp;

class redis_connection {
public:
	redis_connection(): c(NULL), configured(false) { }

	virtual ~redis_connection() {
		if (c) redisFree(c);
	}

	bool is_configured() {
		return configured;
	}

	void configure(const std::string &host, int port) {
		c = redisConnect(host.c_str(), port);
		if (c == NULL || c->err) {
			throw mod_session_redis_exception(c->errstr);
		}
		configured = true;
	}

	redisContext* get_ptr() {
		return c;
	}

private:
	redisContext *c;
	bool configured;
};


class redis_pool: public dbp::singleton<redis_pool> {
	friend class dbp::singleton<redis_pool>;
public:
	void reset(std::auto_ptr<dbp::pool_size_policy> ps) {
		delete pool;
		pool = new dbp::pool<redis_connection>(ps);
	}

	dbp::pool_ptr<redis_connection> acquire(const std::string &arg) {
		return pool->acquire(arg);
	}

	virtual ~redis_pool() {
		delete pool;
	}

private:
	redis_pool() {
		pool = new dbp::pool<redis_connection>();
	}

	dbp::pool<redis_connection> *pool;
};


std::string mod_session_redis::get(const std::string &key) {
	pool_ptr<redis_connection> c = redis_pool::instance().acquire(server);

	if (!c->is_configured()) {
		strings s = tokenize()(server);
		for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
			strings hp = tokenize()(*i, ":");
			int port = 6379;
			if (hp.size() > 1)
				port = from_string<int>(trim()(hp[1]));
			c->configure(trim()(hp[0]), port);
		}
	}

	redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "GET %s", key.c_str());

	if (reply) {
		std::string rslt = (reply->type == REDIS_REPLY_STRING && reply->str) ? string(reply->str) : string("");
		freeReplyObject(reply);
		return rslt;
	} else {
		throw mod_session_redis_exception(c->get_ptr()->errstr);
	}

}

void mod_session_redis::put(const std::string &key, const std::string &value) {
	pool_ptr<redis_connection> c = redis_pool::instance().acquire(server);

	if (!c->is_configured()) {
		strings s = tokenize()(server);
		for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
			strings hp = tokenize()(*i, ":");
			int port = 6379;
			if (hp.size() > 1)
				port = from_string<int>(trim()(hp[1]));
			c->configure(trim()(hp[0]), port);
		}
	}

	redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "SET %s %s", key.c_str(), value.c_str());
	if (reply) {
		freeReplyObject(reply);
	} else {
		throw mod_session_redis_exception(c->get_ptr()->errstr);
	}

	if (ttl > 0) {
		redisReply *reply = (redisReply*)redisCommand(c->get_ptr(), "EXPIRE %s %d", key.c_str(), ttl);
		if (reply) {
			freeReplyObject(reply);
		} else {
			throw mod_session_redis_exception(c->get_ptr()->errstr);
		}
	}

}


} // namespace

