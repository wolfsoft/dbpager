/*
 * interpreter.h
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

#pragma once

#include <string>

#include <unordered_map>

#include <dcl/dclbase.h>

#include <dbpager/services.h>

#include <interpreter/dbpager_application.h>

namespace dbpager {

//! Program interpreter class.
/*!
	Holds global application objects: options, caches, etc.
*/
class interpreter: public dbpager::services, private dbp::noncopyable {
public:
	typedef dbp::shared_ptr<dbpager_application> dbpager_application_ptr;

	interpreter(const std::string config_filename, dbp::logger &default_logger);

	virtual ~interpreter();

	//! Obtain the options from the config file
	dbp::app_config& get_config() {
		return *options;
	}

	//! Obtain the session factory service
	dbpager::session_factory& get_session() const {
		return *session;
	}

	//! Obtain the logger service
	dbp::logger& get_logger() {
		if (loaded_logger)
			return *loaded_logger;
		else
			return logger;
	}

	//! Get cached application instance or create new one and cache it
	dbpager_application_ptr get_app(const dbp::url &u);

	//! Reload configuration
	void reload();

private:
	typedef std::unordered_map<std::string, dbpager_application_ptr > app_cache;
	std::string config_filename;
	dbp::app_config *options{nullptr};
	dbpager::session_factory *session{nullptr};
	dbp::logger *loaded_logger{nullptr};
	dbp::plugin *logger_plugin{nullptr};
	dbp::plugin *session_plugin{nullptr};
	dbp::logger &logger;
	bool use_cache{false};
	dbp::mutex m;
	app_cache apps;

	//! load configuration
	void load();

};

} // namespace
