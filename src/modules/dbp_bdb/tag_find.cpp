/*
 * tag_find.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <db_cxx.h>
#include <dcl/strutils.h>
#include "tag_find.h"
#include "tag_storage.h"

namespace dbpager {
namespace bdb {

using namespace std;
using namespace dbp;
using namespace dbpager;

void tag_find::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	string key = get_parameter(ctx, "name");
	if (key.empty())
		throw tag_storage_exception(
		  _("key name (name) is not defined"));
	// obtain the current storage of current context by ugly hack
	string db_ptr = ctx.get_value(string("@BDB:DATABASE@") +
	  get_parameter(ctx, "storage"));
	if (db_ptr.empty()) {
		string id = get_parameter(ctx, "storage");
		if (id.empty()) {
			id = _("(default)");
		}
		throw tag_find_exception(
		  (format(_("storage (id='{0}') is not defined in the current context")) %
		    id).str());
	}
	bdb_database *db = (bdb_database*)from_string<void*>(db_ptr);
	// for each found data, execute the subnodes
	for (bdb_database::iterator it = db->find(key); it != db->end(); ++it) {
		ctx.enter();
		try {
			// add results found
			ctx.add_value("key", it->first);
			ctx.add_value("value", it->second);
			// call inherited method
			tag_impl::execute(ctx, out, caller);
		}
		catch (...) {
			ctx.leave();
			throw;
		}
		ctx.leave();
	}
}

}} // namespace

