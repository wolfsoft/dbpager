/*
 * dbp_sqlite_factory.cpp
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

#include <dcl/plugin.h>
#include <dcl/app_config.h>

#include <sqlite3.h>

#include "tag_database.h"
#include "tag_transaction.h"
#include "tag_query.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export functions as required by dbp::plugin class

extern "C" {

void init(app_config *config) {
	sqlite3_initialize();
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

void finalize() {
	sqlite3_shutdown();
};

} // extern

} // namespace

