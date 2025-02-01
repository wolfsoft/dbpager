/*
 * dbp_ldap_factory.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2025 Dennis Prochko <dennis.prochko@gmail.com>
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
#include "tag_bind.h"
#include "tag_search.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export functions as required by dbp::plugin class

extern "C" {

void init(app_config *config) { };

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "connection") == 0)
		return new tag_connection();
	else
	if (strcmp(object_name, "bind") == 0)
		return new tag_bind();
	else
	if (strcmp(object_name, "search") == 0)
		return new tag_search();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() { };

} // extern

} // namespace

