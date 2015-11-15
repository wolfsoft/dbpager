/*
 * mod_logger_file_factory.cpp
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

#include "mod_logger_file.h"

namespace dbpager {

using namespace dbp;
using namespace std;

// Export two functions for creating/destroying object, as required by
// dbp::plugin class

extern "C" {

void init(void *config) {
//	string dest = config->value("services.logger.file", "path", "/dev/null");
//	bool async = config->value("services.logger.file", "async", false);
};

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "file") == 0)
		return new mod_logger_file();
	else
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

