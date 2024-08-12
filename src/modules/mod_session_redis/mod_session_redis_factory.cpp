/*
 * mod_session_redis_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <dcl/app_config.h>
#include <dcl/plugin.h>

#include "mod_session_redis.h"
#include "../dbp_redis/redis_connection.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export two functions for creating/destroying object, as required by
// dbp::plugin class

std::string server;
std::string password;
int ttl;
int database_number;

extern "C" {

void init(dbp::app_config *config) {
	server = string("localhost:6379");
	ttl = 0;
	database_number = 0;
	redis_pool &p = redis_pool::instance();
	if (config) {
		int cnt = config->value("services.session.redis", "pool_connections", 0);
		if (cnt) {
			p.reset(auto_ptr<pool_size_policy>(new pool_size_limited_policy(cnt)));
		}
		server = config->value("services.session.redis", "server", string("localhost:6379"));
		password = config->value("services.session.redis", "password", string());
		database_number = config->value("services.session.redis", "database", 0);
		ttl = config->value("services.session.redis", "ttl", 0);
	}
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "redis") == 0) {
		mod_session_redis *msc = new mod_session_redis();
		msc->set_server(server);
		msc->set_password(password);
		msc->set_database_number(database_number);
		msc->set_ttl(ttl);
		return msc;
	} else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() {
	// Nothing to do
};

} // extern

} // namespace

