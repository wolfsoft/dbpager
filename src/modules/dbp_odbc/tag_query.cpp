/*
 * tag_query.cpp
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

#include <vector>

#include <dcl/connection.h>
#include <dcl/query.h>
#include <dcl/strutils.h>

#include "tag_query.h"

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace dbp::odbc;

void tag_query::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	// obtain the current database of current context
	string db_ptr = ctx.get_value(string("@ODBC:DATABASE@") +
	  get_parameter(ctx, "database"));
	if (db_ptr.empty()) {
		string id = get_parameter(ctx, "database");
		if (id.empty()) {
			id = _("(default)");
		}
		throw tag_query_exception(
		  (format(_("database (id='{0}') is not defined in the current "
		    "context")) % id).str());
	}
	connection *db = (connection*)from_string<void*>(db_ptr);
	// intitialize the query
	query q(*db);
	query::parameters &p = q.prepare(get_parameter(ctx, "statement"));
	// process parameters
	vector<string> params = tokenize()(get_parameter(ctx, "parameters"));
	vector<string>::const_iterator j = params.begin();
	for (query::parameters::iterator i = p.begin(); i != p.end(); ++i) {
		if (i->name.empty()) {
			// initialize unnamed parameter
			if (j != params.end()) {
				i->value = *j;
				++j;
			}
		} else {
			// initialize named parameter
			i->value = ctx.get_value(i->name);
		}
	}
	// execute the query
	const query::fields &f = q.execute();
	// for each row execute the subnodes
	while (q.next()) {
		ctx.enter();
		try {
			for (query::fields::const_iterator i = f.begin(); i != f.end(); ++i) {
				ctx.add_value(i->name, i->get_value());
			}
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

} // namespace

