/*
 * tag_connection.cpp
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

#include <dcl/strutils.h>

#include "tag_connection.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_connection::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	string server = get_parameter(ctx, "server");
	if (server.empty())
		server = this->server;
	string password = get_parameter(ctx, "password");
	if (password.empty())
		password = this->password;
	const string &connection = get_parameter(ctx, "id");

	string db_ptr = ctx.get_value(string("@REDIS:CONNECTION@") + connection);
	if (!db_ptr.empty()) {
		tag_impl::execute(ctx, out, caller);
		return;
	}

	ctx.enter();
	try {
		dbp::pool_ptr<redis_connection> c = redis_pool::instance().acquire(server + connection);
		if (!c->is_configured()) {
			c->configure(server);
			if (!password.empty())
				c->login(password);
			if (database_number > 0)
				c->select_database(database_number);
		}
		// save the pointer to connection for nested tags
		ctx.add_value(string("@REDIS:CONNECTION@") + connection, to_string<redis_connection*>(&*c));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
