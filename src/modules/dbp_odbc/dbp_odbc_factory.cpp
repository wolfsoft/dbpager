/*
 * dbp_odbc_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
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

#include "tag_database.h"
#include "tag_transaction.h"
#include "tag_query.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export functions as required by dbp::plugin class

extern "C" {

void init(app_config *config) {
	dbp::odbc::connection::init_environment();
	database_pool &p = database_pool::instance();
	if (config) {
		int cnt = config->value("modules.odbc", "pool_connections", 0);
		if (cnt) {
			p.reset(auto_ptr<pool_size_policy>(new pool_size_limited_policy(cnt)));
		}
	}
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "database") == 0)
		return new tag_database();
	else
	if (strcmp(object_name, "transaction") == 0)
		return new tag_transaction();
	else
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

