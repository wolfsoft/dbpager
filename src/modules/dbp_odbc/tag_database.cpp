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
#include <dcl/query.h>

#include "tag_database.h"

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace dbp::odbc;

void tag_database::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &dsn = get_parameter(ctx, "dsn");
	const string &user = get_parameter(ctx, "user");
	const string &password = get_parameter(ctx, "password");
	const string &database = get_parameter(ctx, "id");
	if (dsn.empty()) {
		throw tag_database_exception(
		  _("data source name (dsn) is not defined"));
	}

	string db_ptr = ctx.get_value(string("@ODBC:DATABASE@") + database);
	if (!db_ptr.empty()) {
		tag_impl::execute(ctx, out, caller);
		return;
	}

	pool_ptr<connection> c = database_pool::instance().acquire(dsn + user + password);
	ctx.enter();
	try {
		if (!c->is_open())
			c->open(dsn, user, password);
		// save the pointer to database for nested tags
		ctx.add_value(string("@ODBC:DATABASE@") + get_parameter(ctx, "id"),
		  to_string<connection*>(&*c));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		c->close();
		ctx.leave();
		throw;
	}
}

} // namespace
