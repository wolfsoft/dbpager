/*
 * tag_update.cpp
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

#include <vector>

#include <dcl/strutils.h>

#include "tag_update.h"

namespace dbpager {
namespace mongodb {

using namespace std;
using namespace dbp;
using namespace mongo;

void tag_update::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	const string &collection = get_parameter(ctx, "collection");
	if (collection.empty())
		throw tag_update_exception(
		  _("query collection (collection) is not defined"));

	string db_ptr = ctx.get_value("@MONGODB:DATABASE@");
	if (db_ptr.empty()) {
		throw tag_update_exception(
		  _("MongoDB database is not defined in the current context"));
	}
	database *db = (database*)from_string<void*>(db_ptr);

	ctx.enter();
	try {
/*		BSONObjBuilder b_find, b_values;

		ctx.add_value("@MONGODB:PARAM@", to_string<BSONObjBuilder*>(&b_find));
		db->update(collection, b.obj());*/

	}
	catch (...) {
		ctx.leave();
		throw;
	}
	ctx.leave();
}

}} // namespace
