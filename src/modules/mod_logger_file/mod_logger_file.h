/*
 * mod_logger_file.h
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

#ifndef _MOD_LOGGER_FILE_H_
#define _MOD_LOGGER_FILE_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>
#include <dcl/logger.h>
#include <dcl/plugin.h>

namespace dbpager {

class mod_logger_file_exception: public dbp::exception {
public:
	mod_logger_file_exception(const std::string &msg): dbp::exception(msg) { }
};

class mod_logger_file: public dbp::logger {
public:
	virtual void log(dbp::log_level::log_level level, const std::string &message);
};

}

#endif /*_MOD_LOGGER_FILE_H_*/
