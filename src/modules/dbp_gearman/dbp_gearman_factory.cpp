/*
 * dbp_gearman_factory.cpp
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

#include "tag_server.h"
#include "tag_task.h"

namespace dbpager {
namespace gearman {

using namespace dbp;
using namespace std;
using namespace dbpager;

// Export functions as required by dbp::plugin class

extern "C" {

void init(void *config) {
	server::instance();
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "server") == 0)
		return new gearman::tag_server();
	if (strcmp(object_name, "task") == 0)
		return new gearman::tag_task();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() { };

} // extern

}} // namespace

