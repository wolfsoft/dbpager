/*
 * dbp_redis_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <string.h>

#include <dcl/plugin.h>
#include <dcl/app_config.h>

#include "tag_connection.h"
#include "tag_query.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export functions as required by dbp::plugin class

string server, password;
int database_number;

extern "C" {

void init(app_config *config) {
	database_number = 0;
	redis_pool &p = redis_pool::instance();
	if (config) {
		int cnt = config->value("modules.redis", "pool_connections", 0);
		if (cnt) {
			p.reset(auto_ptr<pool_size_policy>(new pool_size_limited_policy(cnt)));
		}
		server = config->value("modules.redis", "server", "localhost:6379");
		password = config->value("modules.redis", "password", string());
		database_number = config->value("modules.redis", "database", 0);
	}
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "connection") == 0) {
		tag_connection *conn = new tag_connection();
		conn->server = server;
		conn->password = password;
		conn->database_number = database_number;
		return conn;
	} else
	if (strcmp(object_name, "query") == 0)
		return new tag_query();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() { };

} // extern

} // namespace

