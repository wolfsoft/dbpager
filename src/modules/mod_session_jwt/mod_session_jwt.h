/*
 * mod_session_jwt.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2026 - Dennis Prochko <dennis.prochko@gmail.com>
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

#define JWT_DISABLE_PICOJSON
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/defaults.h"
#include "jwt-cpp/jwt.h"

#include <dcl/exception.h>

#include <dbpager/context.h>
#include <dbpager/session.h>

namespace dbpager {

class mod_session_jwt_exception: public dbp::exception {
public:
	mod_session_jwt_exception(const std::string &msg): dbp::exception(msg) { }
};

class mod_session_jwt: public session_holder {
public:
	mod_session_jwt();
	mod_session_jwt(const std::string &token);
	virtual void load(context &ctx) override;
	virtual void save(const context &ctx, dbp::http_response &resp) override;

	void set_secret(const std::string &secret) {
		this->secret = secret;
	}
	void set_ttl(int ttl) {
		this->ttl = ttl;
	}
private:
	int ttl{0};
	std::string secret;
};

class mod_session_jwt_factory: public session_factory {
public:
	virtual std::unique_ptr<session_holder> create_session(const dbp::http_request &req) override;
	void set_secret(const std::string &secret) {
		this->secret = secret;
	}
	void set_ttl(int ttl) {
		this->ttl = ttl;
	}
private:
	int ttl{0};
	std::string secret;
};

} // namespace dbpager