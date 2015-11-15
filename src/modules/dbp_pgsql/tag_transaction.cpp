/*
 * tag_transaction.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008-2014 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <pqxx/pqxx>

#include <dcl/strutils.h>

#include "tag_transaction.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_transaction::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	string db_ptr = ctx.get_value(string("@PGSQL:DATABASE@") +
	  get_parameter(ctx, "database"));

	if (db_ptr.empty()) {
		string id = get_parameter(ctx, "database");
		if (id.empty()) {
			id = _("(default)");
		}
		throw tag_transaction_exception(
		  (format(_("database (id='{0}') is not defined in the current "
		    "context")) % id).str());
	}

	pqxx::connection *conn = (pqxx::connection*)dbp::from_string<void*>(db_ptr);
	pqxx::work *work;

	try {
		work = new pqxx::work(*conn);
	} catch(const std::exception &e) {
		throw tag_transaction_exception(e.what());
	}

	ctx.enter();
	try {
		ctx.add_value(string("@PGSQL:WORK@"), dbp::to_string<pqxx::work*>(work));
		tag_impl::execute(ctx, out, caller);
		work->commit();
		delete work;
		ctx.leave();
	} catch (...) {
		work->abort();
		delete work;
		ctx.leave();
		throw;
	}

}

} // namespace

