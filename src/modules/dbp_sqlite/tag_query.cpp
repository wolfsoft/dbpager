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
#include <dcl/strutils.h>

#include <sqlite3.h>

#include "tag_query.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_query::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	// obtain the current database of current context
	string db_ptr = ctx.get_value(string("@SQLITE:DATABASE@") + get_parameter(ctx, "database"));
	if (db_ptr.empty()) {
		string id = get_parameter(ctx, "database");
		if (id.empty()) {
			id = _("(default)");
		}
		throw tag_query_exception(
		  (format(_("database (id='{0}') is not defined in the current context")) % id).str());
	}

	sqlite3 *conn = (sqlite3*)from_string<void*>(db_ptr);

	// intitialize the query
	const string &statement = get_parameter(ctx, "statement");
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	if (int code = sqlite3_prepare_v2(conn, statement.c_str(), statement.length(), &stmt, &tail) != SQLITE_OK) {
		throw tag_query_exception(sqlite3_errstr(code));
	}

	try {
		// process parameters
		vector<string> params = tokenize()(get_parameter(ctx, "parameters"));
		vector<string>::const_iterator j = params.begin();

		int c = sqlite3_bind_parameter_count(stmt);
		for (int i = 1; i <= c; i++) {
			const char *n = sqlite3_bind_parameter_name(stmt, i);
			if (n == NULL) {
				// initialize unnamed parameter
				if (j != params.end()) {
					const string &v = *j;
					if (v.empty()) {
						if (int code = sqlite3_bind_null(stmt, i) != SQLITE_OK) {
							throw tag_query_exception(sqlite3_errstr(code));
						}
					} else {
						if (int code = sqlite3_bind_text(stmt, i, v.c_str(), v.length(), SQLITE_TRANSIENT) != SQLITE_OK) {
							throw tag_query_exception(sqlite3_errstr(code));
						}
					}
					++j;
				}
			} else {
				// cut out first char from parameter name (colon)
				n++;
				// process as desired
				const string &v = ctx.get_value(n);
				if (v.empty()) {
					if (int code = sqlite3_bind_null(stmt, i) != SQLITE_OK) {
						throw tag_query_exception(sqlite3_errstr(code));
					}
				} else {
					if (int code = sqlite3_bind_text(stmt, i, v.c_str(), v.length(), SQLITE_TRANSIENT) != SQLITE_OK) {
						throw tag_query_exception(sqlite3_errstr(code));
					}
				}
			}
		} // for

		// execute the query
		int code = sqlite3_step(stmt);
		if (code == SQLITE_DONE && sqlite3_changes(conn) > 0) {
			sqlite3_reset(stmt);
			ctx.enter();
			try {
				tag_impl::execute(ctx, out, caller);
				ctx.leave();
			} catch (...) {
				ctx.leave();
				throw;
			}
		} else {
			while (true) {
				ctx.enter();
				try {
					if (code == SQLITE_DONE) {
						sqlite3_reset(stmt);
						ctx.leave();
						break;
					} else if (code == SQLITE_ROW) {
						// fetch the results
						int c = sqlite3_column_count(stmt);
						for (int i = 0; i < c; i++) {
							const char *n = sqlite3_column_name(stmt, i);
							if (n == NULL)
								throw tag_query_exception(_("Can't allocate memory to fetch query results"));
							const char *v = (const char*)sqlite3_column_text(stmt, i);
							if (v == NULL)
								ctx.add_value(n, string());
							else
								ctx.add_value(n, v);
						}

						tag_impl::execute(ctx, out, caller);

					} else {
						throw tag_query_exception(sqlite3_errstr(code));
					}

					code = sqlite3_step(stmt);
					ctx.leave();

				} catch (...) {
					ctx.leave();
					throw;
				}
			} // while
		}

		// clean up
		sqlite3_finalize(stmt);

	} catch (...) {
		sqlite3_finalize(stmt);
		throw;
	}

}

} // namespace

