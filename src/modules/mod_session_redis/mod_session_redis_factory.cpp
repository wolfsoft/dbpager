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

namespace dbpager {

using namespace dbp;
using namespace std;

// Export two functions for creating/destroying object, as required by
// dbp::plugin class

std::string server;
int ttl = 0;

extern "C" {

void init(dbp::app_config *config) {
	server = config->value("services.session.redis", "server", "localhost:6379");
	ttl = config->value("services.session.redis", "ttl", 0);
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "redis") == 0) {
		mod_session_redis *msc = new mod_session_redis();
		msc->set_server(server);
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

