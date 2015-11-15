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

#include <pqxx/pqxx>

#include <vector>

#include <dcl/strutils.h>

#include "tag_query.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_query::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	// obtain the current database of current context
	string db_ptr = ctx.get_value(string("@PGSQL:DATABASE@") +
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
	pqxx::connection *conn = (pqxx::connection*)from_string<void*>(db_ptr);

	bool need_to_free = false;
	pqxx::work *work;
	pqxx::result r;

	string work_ptr = ctx.get_value(string("@PGSQL:WORK@"));
	if (!work_ptr.empty()) {
		work = (pqxx::work*)from_string<void*>(work_ptr);
	} else {
		try {
			need_to_free = true;
			work = new pqxx::work(*conn);
		} catch(const std::exception &e) {
			throw tag_query_exception(e.what());
		}
	}

	try {
		string self_ptr = to_string<const tag_query*>(this);

		string statement;
		strings params;
		parse(get_parameter(ctx, "statement"), statement, params);

		conn->unprepare(self_ptr);
		conn->prepare(self_ptr, statement);
		pqxx::prepare::invocation q = work->prepared(self_ptr);
		strings passed_params = tokenize()(get_parameter(ctx, "parameters"));
		strings::const_iterator j = passed_params.begin();
		for (strings::const_iterator i = params.begin(); i != params.end(); ++i) {
			if (i->empty()) {
				q(*j);
				++j;
			} else {
				string value = ctx.get_value(*i);
				if (value.empty())
					q();
				else
					q(value);
			}
		}

		r = q.exec();

		if (need_to_free) {
			work->commit();
			delete work;
		}

	} catch(const std::exception &e) {
		if (need_to_free) {
			work->abort();
			delete work;
		}
		throw tag_query_exception(e.what());
	}

	for (pqxx::result::const_iterator row = r.begin(); row != r.end(); ++row) {
		ctx.enter();
		try {
			for (pqxx::result::tuple::const_iterator field = row->begin(); field != row->end(); ++field) {
				if (field.type() == 16) { // boolean
					try {
						if (field.as<bool>())
							ctx.add_value(field.name(), "1");
						else
							ctx.add_value(field.name(), "0");
					} catch(...) {
						ctx.add_value(field.name(), "");
					}
				} else
					ctx.add_value(field.name(), field.c_str());
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

void tag_query::parse(const std::string &statement, std::string &parsed_query, dbp::strings &parameters) const {
	// clear existing parameters
	parameters.clear();
	// parsing the query to obtain its parameters
	string q;
	bool ssq = false, dsq = false;
	string::const_iterator i = statement.begin();
	int n = 0;
	while (i != statement.end()) {
		switch (*i) {
			case '\'':
				ssq = not ssq;
				q += *i;
				break;
			case '"':
				dsq = not dsq;
				q += *i;
				break;
			case '?':
				if (!ssq && !dsq) {
					parameters.push_back("");
					q += "$" + to_string<int>(++n);
				} else {
				  q += *i;
				}
				break;
			case ':':
				if (!ssq && !dsq) {
					string p;
					++i;
					while ((i != statement.end()) && (isalnum(*i) || (*i == '_'))) {
						p += *i;
						++i;
					}
					--i;
					parameters.push_back(p);
					q += "$" + to_string<int>(++n);
				} else {
					q += *i;
				}
				break;
			default:
				q += *i;
				break;
		}
		++i;
	}
	parsed_query = q;
}

} // namespace

