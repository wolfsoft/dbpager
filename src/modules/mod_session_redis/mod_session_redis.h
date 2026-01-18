/*
 * mod_session_redis.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <string>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/context.h>
#include <dbpager/session.h>

namespace dbpager {

class mod_session_redis_exception: public dbp::exception {
public:
	mod_session_redis_exception(const std::string &msg): dbp::exception(msg) { }
};

class mod_session_redis: public session_holder {
public:
	mod_session_redis() {
		id = dbp::uuid().str();
		is_new = true;
	};
	mod_session_redis(const std::string &session_id) {
		id = session_id;
		if (id.empty()) {
			id = dbp::uuid().str();
			is_new = true;
		}
	};
	virtual void load(context &ctx) override;
	virtual void save(const context &ctx, dbp::http_response &resp) override;
public:
	int ttl{0};
	int database_number{0};
	std::string empty;
	std::string server;
	std::string password;
private:
	std::string get_value(const std::string &key);
	void put_value(const std::string &key, const std::string &value);
};

class mod_session_redis_factory: public session_factory {
public:
	virtual std::unique_ptr<session_holder> create_session(const dbp::http_request &req) override;
	void set_server(const std::string &server) {
		this->server = server;
	}
	void set_ttl(int ttl) {
		this->ttl = ttl;
	}
	void set_database_number(int database_number) {
		this->database_number = database_number;
	}
	void set_password(const std::string &password) {
		this->password = password;
	}
private:
	int ttl;
	int database_number;
	std::string empty;
	std::string server;
	std::string password;
};

}
