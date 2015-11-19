/*
 * interpreter.cpp
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

#include "algorithm"

#include "dbpager/consts.h"
#include "tag/tag_factory.h"
#include "parser/dbpx_parser.h"
#include "interpreter/interpreter.h"

#include "services/session_int.h"

#include <libxml/parser.h>

namespace dbpager {

using namespace std;
using namespace dbp;

interpreter::interpreter(const std::string config_filename, dbp::logger &logger):
  config_filename(config_filename), options(NULL), session(NULL), loaded_logger(NULL), logger_plugin(NULL),
  session_plugin(NULL), logger(logger) {
	xmlInitParser();
	load();
}

interpreter::~interpreter() {
	apps.clear();
	if (session)
		delete session;

	if (session_plugin)
		delete session_plugin;

	if (loaded_logger)
		delete loaded_logger;

	if (logger_plugin)
		delete logger_plugin;

	if (options)
		delete options;

	xmlCleanupParser();
};

void interpreter::load() {
	try {
		if (options)
			delete options;
		options = new app_config();
		options->load_from_file(config_filename);

		// initialize logger service
		if (loaded_logger) {
			delete loaded_logger;
			loaded_logger = NULL;
		}

		const string &log_type = options->value("services", "log", "internal");
		if (log_type != string("internal")) {
			string module = options->value("services", string("logger.backend.") + log_type, string(""));
			if (!module.empty()) {
				if (logger_plugin) {
					delete logger_plugin;
					logger_plugin = NULL;
				}
				logger_plugin = new plugin();
				logger_plugin->load(module);
				logger_plugin->init(&options);
				loaded_logger = (dbp::logger*)logger_plugin->create_object(log_type);
			}
		}

		// initialize session service
		if (session) {
			delete session;
			session = NULL;
		}

		const string &session_type = options->value("services", "session", "internal");
		if (log_type != string("internal")) {
			const string &module = options->value("services", string("session.backend.") + log_type, string(""));
			if (!module.empty()) {
				if (session_plugin) {
					delete session_plugin;
					session_plugin = NULL;
				}
				session_plugin = new plugin();
				session_plugin->load(module);
				session_plugin->init(&options);
				session = (dbpager::session*)session_plugin->create_object(session_type);
			}
		}

		if (!session)
			session = new session_int();

		// load plugins from a config file "modules" section, if any
		tag_factory &_factory = tag_factory::instance();
		const app_config::strindex &plugins_list =
		  get_config().section("modules");
		for (app_config::strindex::const_iterator i = plugins_list.begin();
		  i != plugins_list.end(); ++i) {
			dbp::shared_ptr<plugin> p(new plugin());
			p->load(i->second);
			p->init(options);
			_factory.register_plugin(i->first, p);
		}

	} catch (app_config_exception &e) {
		get_logger().warning((format(_("Configuration file \"{0}\" was not found; will use default values instead"))
		  % config_filename).str());
	}
}

void interpreter::reload() {
	get_logger().info(_("Configuration realoaded"));
}

interpreter::dbpager_application_ptr interpreter::get_app(const dbp::url &u) {
	app_cache::const_iterator it = apps.find(u.str());
	if (it != apps.end())
		return it->second;

	mutex_guard cs(m);
	it = apps.find(u.str());
	if (it != apps.end())
		return it->second;

	dbpager_application_ptr app(new dbpager_application(u));
	apps.insert(pair<string, dbpager_application_ptr >(u.str(), app));

	return app;

}

} // namespace

