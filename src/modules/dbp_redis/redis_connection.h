/*
 * tag_connection.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _TAG_REDIS_CONNECTION_H_
#define _TAG_REDIS_CONNECTION_H_

#include <string>

#include <hiredis/hiredis.h>

#include <dcl/dclbase.h>

namespace dbpager {

class redis_exception: public dbp::exception {
public:
	redis_exception(const std::string &msg): dbp::exception(msg) { }
};

class redis_connection {
public:
	redis_connection(): c(NULL), configured(false) { }
	redis_connection(const std::string &server, const std::string &password): c(NULL), configured(false) { }

	virtual ~redis_connection() {
		if (c) redisFree(c);
	}

	bool is_configured() {
		return configured;
	}

	void configure(const std::string &host_and_port) {
		// split by comma
		dbp::strings s = dbp::tokenize()(dbp::trim()(host_and_port));
		for (dbp::strings::const_iterator i = s.begin(); i != s.end(); ++i) {
			dbp::strings hp = dbp::tokenize()(*i, ":");
			int p = 6379;
			if (hp.size() > 1)
				p = dbp::from_string<int>(dbp::trim()(hp[1]));
			std::string host = dbp::trim()(hp[0]);

			c = redisConnect(host.c_str(), p);
			if (c == NULL || c->err) {
				throw redis_exception(std::string(c->errstr));
			}

			configured = true;
		}
	}

	void login(const std::string &password) {
		if (!configured)
			return;

		redisReply *reply = (redisReply*)redisCommand(c, "AUTH %s", password.c_str());
		if (!reply)
			throw redis_exception(c->errstr);

		freeReplyObject(reply);
	}

	void select_database(int database) {
		if (!configured)
			return;

		redisReply *reply = (redisReply*)redisCommand(c, "SELECT %d", database);
		if (!reply)
			throw redis_exception(c->errstr);

		freeReplyObject(reply);
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

}

#endif /*_TAG_REDIS_CONNECTION_H_*/

