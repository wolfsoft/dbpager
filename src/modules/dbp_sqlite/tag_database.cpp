/*
 * tag_database.cpp
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

#include <dcl/pool.h>
#include <dcl/singleton.h>
#include <dcl/strutils.h>

#include <sqlite3.h>

#include "tag_database.h"

namespace dbpager {

using namespace std;
using namespace dbp;


class sqlite_connection {
public:
	sqlite_connection(): ppDb(NULL) { }

	virtual ~sqlite_connection() {
		if (ppDb != NULL)
			sqlite3_close_v2(ppDb);
	}

	bool is_open() {
		return (ppDb != NULL);
	}

	void open(const std::string &dsn) {
		if (int code = sqlite3_open_v2(dsn.c_str(), &ppDb,
		  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
		  NULL) != SQLITE_OK) {
			throw tag_database_exception(string(sqlite3_errstr(code)));
		}
	}

	sqlite3 *get_ptr() {
		return ppDb;
	}

private:
	sqlite3 *ppDb;
};


class database_pool: public dbp::singleton<database_pool> {
	friend class dbp::singleton<database_pool>;
public:
	void reset(std::auto_ptr<dbp::pool_size_policy> ps) {
		delete pool;
		pool = new dbp::pool<sqlite_connection>(ps);
	}
	dbp::pool_ptr<sqlite_connection> acquire(const std::string &arg) {
		return pool->acquire(arg);
	}
	virtual ~database_pool() {
		delete pool;
	}
private:
	database_pool() {
		pool = new dbp::pool<sqlite_connection>();
	}
	dbp::pool<sqlite_connection> *pool;
};


void tag_database::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &dsn = get_parameter(ctx, "dsn");
	const string &database = get_parameter(ctx, "id");
	if (dsn.empty()) {
		throw tag_database_exception(
		  _("data source name (dsn) is not defined"));
	}

	string db_ptr = ctx.get_value(string("@SQLITE:DATABASE@") + database);
	if (!db_ptr.empty()) {
		tag_impl::execute(ctx, out, caller);
		return;
	}

	ctx.enter();
	try {
		pool_ptr<sqlite_connection> c = database_pool::instance().acquire(dsn);
		if (!c->is_open())
			c->open(dsn);
		// save the pointer to database for nested tags
		ctx.add_value(string("@SQLITE:DATABASE@") + get_parameter(ctx, "id"),
		  to_string<sqlite3*>(c->get_ptr()));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}

}

} // namespace
