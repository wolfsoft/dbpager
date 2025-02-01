/*
 * tag_search.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2025 Dennis Prochko <dennis.prochko@gmail.com>
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

#include "ldap_client.h"
#include "tag_search.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_search::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &basedn = get_parameter(ctx, "base");
	if (basedn.empty()) {
		throw tag_search_exception(
		  _("base DN (base) is not defined"));
	}
	const string &filter = get_parameter(ctx, "filter");
	if (filter.empty()) {
		throw tag_search_exception(
		  _("filter is not defined"));
	}

	string db_ptr = ctx.get_value(string("@LDAP_CONNECTION@"));
	if (db_ptr.empty()) {
		throw tag_search_exception(
		  _("LDAP server is not defined in the current context"));
	}
	ldap_connection *conn = (ldap_connection*)dbp::from_string<void*>(db_ptr);

	for (auto row: conn->search(basedn, filter)) {
		ctx.enter();
		try {
			for (auto field = row.begin(); field != row.end(); ++field) {
				ctx.add_value(field->first, field->second);
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
