/*
 * session.h
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

#include <dcl/dclbase.h>
#include <dcl/http_header.h>

namespace dbpager {

class context;

class session_holder {
public:
	virtual ~session_holder() { };
	const std::string& get_id() const {
		return id;
	};
	virtual void load(context &ctx) = 0;
	virtual void save(const context &ctx, dbp::http_response &resp) = 0;
public:
	bool is_new{false};
	bool is_https{false};
protected:
	std::string id;
};

class session_factory: public dbp::disposable {
public:
	virtual ~session_factory() { };
	virtual std::unique_ptr<session_holder> create_session(const dbp::http_request &req) = 0;
};

} // namespace dbpager