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

#include <dcl/connection.h>
#include <dcl/strutils.h>

#include <sqlite3.h>

#include "tag_transaction.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_transaction::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	string db_ptr = ctx.get_value(string("@SQLITE:DATABASE@") +
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
	char *err_msg = NULL;
	sqlite3 *conn = (sqlite3*)from_string<void*>(db_ptr);
	if (sqlite3_exec(conn, "BEGIN", NULL, NULL, &err_msg) != SQLITE_OK) {
		string err_msg_str(err_msg);
		sqlite3_free(err_msg);
		throw tag_transaction_exception(err_msg_str);
	}
	try {
		tag_impl::execute(ctx, out, caller);
		if (sqlite3_exec(conn, "COMMIT", NULL, NULL, &err_msg) != SQLITE_OK) {
			string err_msg_str(err_msg);
			sqlite3_free(err_msg);
			throw tag_transaction_exception(err_msg_str);
		}
	}
	catch (...) {
		if (sqlite3_exec(conn, "ROLLBACK", NULL, NULL, &err_msg) != SQLITE_OK) {
			string err_msg_str(err_msg);
			sqlite3_free(err_msg);
			throw tag_transaction_exception(err_msg_str);
		}
		throw;
	}
}

} // namespace

