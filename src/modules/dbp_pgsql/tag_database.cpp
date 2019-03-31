/*
 * tag_database.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008-2019 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <dcl/strutils.h>

#include "tag_database.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_database::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &dsn = get_parameter(ctx, "dsn");
	const string &database = get_parameter(ctx, "id");
	if (dsn.empty()) {
		throw tag_database_exception(
		  _("data source name (dsn) is not defined"));
	}

	string db_ptr = ctx.get_value(string("@PGSQL:DATABASE@") + database);
	if (!db_ptr.empty()) {
		tag_impl::execute(ctx, out, caller);
		return;
	}

	dbp::pool_ptr<database_pool::pool_item> pp = database_pool::instance().acquire(dsn);
	ctx.enter();
	try {
		if (!(*pp))
			pp->reset(new pqxx::connection(dsn));
		pqxx::connection &c = **pp;
		// save the pointer to database for nested tags
		ctx.add_value(string("@PGSQL:DATABASE@") + get_parameter(ctx, "id"),
		  dbp::to_string<pqxx::connection*>(&c));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (const pqxx::broken_connection &e) {
		pp->reset(new pqxx::connection(dsn));
		ctx.leave();
		throw tag_database_exception(e.what());
	} catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
