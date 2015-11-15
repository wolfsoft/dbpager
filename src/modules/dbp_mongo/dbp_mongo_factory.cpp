/*
 * dbp_mongo_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2012 - Dennis Prochko <wolfsoft@mail.ru>
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

#include "tag_database.h"
#include "tag_query.h"
#include "tag_insert.h"
#include "tag_update.h"
#include "tag_delete.h"
#include "tag_param.h"

namespace dbpager {
namespace mongodb {

using namespace dbp;
using namespace std;
using namespace dbpager;

// Export functions as required by dbp::plugin class

extern "C" {

void init(void *config) {
	database::instance();
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "database") == 0)
		return new mongodb::tag_database();
	else
	if (strcmp(object_name, "query") == 0)
		return new mongodb::tag_query();
	else
	if (strcmp(object_name, "insert") == 0)
		return new mongodb::tag_insert();
	else
	if (strcmp(object_name, "update") == 0)
		return new mongodb::tag_update();
	else
	if (strcmp(object_name, "remove") == 0)
		return new mongodb::tag_delete();
	else
	if (strcmp(object_name, "param") == 0)
		return new mongodb::tag_param();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() { };

} // extern

}} // namespace

