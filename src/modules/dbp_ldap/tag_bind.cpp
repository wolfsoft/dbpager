/*
 * tag_bind.cpp
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

#include "tag_bind.h"
#include "ldap_client.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_bind::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &dn = get_parameter(ctx, "dn");
	const string &password = get_parameter(ctx, "password");

	string db_ptr = ctx.get_value(string("@LDAP_CONNECTION@"));
	if (db_ptr.empty()) {
		throw tag_bind_exception(
		  _("LDAP server is not defined in the current context"));
	}
	ldap_connection *conn = (ldap_connection*)dbp::from_string<void*>(db_ptr);

	ctx.enter();
	try {
		ctx.add_value(string("@LDAP_BIND_DN@"), dn);
		ctx.add_value(string("@LDAP_BIND_PW@"), password);

		conn->bind(dn, password);
		tag_impl::execute(ctx, out, caller);
		ctx.leave();

		string old_dn = ctx.get_value(string("@LDAP_BIND_DN@"));
		string old_pw = ctx.get_value(string("@LDAP_BIND_PW@"));

		if (!old_dn.empty() && !old_pw.empty()) {
			conn->bind(old_dn, old_pw);
		}
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
