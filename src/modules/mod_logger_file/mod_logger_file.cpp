/*
 * mod_logger_file.cpp
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

#include <sstream>

#include <dcl/strutils.h>

#include "mod_logger_file.h"

namespace dbpager {

using namespace std;
using namespace dbp;

mod_logger_file::mod_logger_file(const std::string &path, bool async) {
	out.open(path, std::ios::app);
}

mod_logger_file::~mod_logger_file() {
	out.close();
}

void mod_logger_file::log(log_level::log_level level, const std::string &message) {
	out << message << endl << flush;
}

} // namespace

