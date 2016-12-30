/*
 * tag_pub.cpp
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

#include <vector>

#include <dcl/strutils.h>

#include "tag_pub.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_pub::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &topic = get_parameter(ctx, "topic");
	string qos = get_parameter(ctx, "qos");
	if (qos.empty())
		qos = string("0");
	string retain = get_parameter(ctx, "retain");
	if (retain.empty())
		retain = string("0");

	// cache protected output
	ostringstream s(ostringstream::out | ostringstream::binary);
	// call inherited method
	tag_impl::execute(ctx, s, caller);
	string payload = s.str();

	string m_ptr = ctx.get_value(string("@MOSQUITTO:CONNECTION@"));
	if (m_ptr.empty())
		throw tag_pub_exception(_("MQTT connection is not established yet"));
	mosquittopp::mosquittopp *conn = (mosquittopp::mosquittopp*)from_string<void*>(m_ptr);

	if (conn->publish(NULL, topic.c_str(), payload.length(), (const uint8_t*)payload.c_str(), from_string<int>(qos), from_string<bool>(retain)) != MOSQ_ERR_SUCCESS)
		throw tag_pub_exception(_("MQTT message publish failed"));
}

} // namespace
