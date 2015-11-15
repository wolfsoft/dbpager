/*
 * tag_database.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2012 - Dennis Prochko <wolfsoft@mail.ru>
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
namespace mongodb {

using namespace std;
using namespace dbp;
using namespace mongo;

void tag_database::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	const string &host = get_parameter(ctx, "host");
	if (host.empty()) {
		throw tag_database_exception(
		  _("MongoDB server host name (host) is not defined"));
	}

	database &db = database::instance();
	db.connect(host);

	ctx.enter();
	try {
		ctx.add_value(string("@MONGODB:DATABASE@"), to_string<database*>(&db));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}

}

}} //namespace
