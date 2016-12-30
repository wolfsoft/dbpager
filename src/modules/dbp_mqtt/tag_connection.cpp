/*
 * tag_connection.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2016 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <mosquittopp.h>

#include <dcl/strutils.h>

#include "tag_connection.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_connection::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	string m_ptr = ctx.get_value(string("@MOSQUITTO:CONNECTION@"));
	if (!m_ptr.empty()) {
		tag_impl::execute(ctx, out, caller);
		return;
	}

	ctx.enter();
	mosquittopp::mosquittopp conn(NULL);
	try {
		if (!user.empty())
			if (conn.username_pw_set(user.c_str(), password.c_str()) != MOSQ_ERR_SUCCESS)
				throw new tag_connection_exception("Can't authenticate on MQTT server");
		if (conn.connect(host.c_str(), from_string<int>(port)) != MOSQ_ERR_SUCCESS)
			throw new tag_connection_exception("Can't establish MQTT connection");
		// save the pointer to connection for nested tags
		ctx.add_value(string("@MOSQUITTO:CONNECTION@"),
		  dbp::to_string<mosquittopp::mosquittopp*>(&conn));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
		conn.disconnect();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
