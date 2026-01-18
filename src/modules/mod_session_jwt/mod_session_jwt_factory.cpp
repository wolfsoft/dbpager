/*
 * mod_session_jwt_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2026 - Dennis Prochko <dennis.prochko@gmail.com>
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

#include "mod_session_jwt.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export two functions for creating/destroying object, as required by
// dbp::plugin class

std::string secret;
int ttl;

extern "C" {

void init(dbp::app_config *config) {
	ttl = 0;
	if (config) {
		std::string secret_environment = config->value("services.session.jwt", "secret_environment", string());
		if (!secret_environment.empty()) {
			const char *env = getenv(secret_environment.c_str());
			if (env) {
				secret = string(env);
			}
		}
		if (secret.empty()) {
			secret = config->value("services.session.jwt", "secret", string());
		}
		ttl = config->value("services.session.jwt", "ttl", 0);
	}
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "jwt") == 0) {
		mod_session_jwt_factory *session_factory = new mod_session_jwt_factory();
		session_factory->set_secret(secret);
		session_factory->set_ttl(ttl);
		return session_factory;
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

