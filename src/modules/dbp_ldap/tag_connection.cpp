/*
 * tag_connection.cpp
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

#include "tag_connection.h"
#include "ldap_client.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_connection::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &href = get_parameter(ctx, "href");
	if (href.empty()) {
		throw tag_connection_exception(
		  _("LDAP connection URI (href) is not defined"));
	}

	ctx.enter();
	std::unique_ptr<ldap_connection> c(new ldap_connection(href));
	try {
		ctx.add_value(string("@LDAP_CONNECTION@"), to_string<ldap_connection*>(c.get()));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
