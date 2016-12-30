/*
 * dbp_mqtt_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2016 - Dennis Prochko <wolfsoft@mail.ru>
 *
 * dbPager Server is free software; you can mqtttribute it and/or modify
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

#include <string.h>

#include <dcl/plugin.h>
#include <dcl/app_config.h>

#include "tag_connection.h"
#include "tag_pub.h"
#include "tag_sub.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export functions as required by dbp::plugin class

string host, port, user, password;

extern "C" {

void init(app_config *config) {
	mosquittopp::mosquittopp::lib_init();
	if (config) {
		host = config->value("modules.mqtt", "host", "127.0.0.1");
		port = config->value("modules.mqtt", "port", "1883");
		host = config->value("modules.mqtt", "user", string());
		port = config->value("modules.mqtt", "password", string());
	}
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "connection") == 0) {
		tag_connection *conn = new tag_connection();
		conn->host = host;
		conn->port = port;
		conn->user = user;
		conn->password = password;
		return conn;
	} else
	if (strcmp(object_name, "pub") == 0)
		return new tag_pub();
	else
	if (strcmp(object_name, "sub") == 0)
		return new tag_sub();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() {
	mosquittopp::mosquittopp::lib_cleanup();
};

} // extern

} // namespace
