/*
 * dbpager.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008-2015 - Dennis Prochko
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

#include <iostream>
#include <vector>
#include <memory>

#include <dcl/dclbase.h>

#include "dbpager/consts.h"
#include "interpreter/interpreter.h"

#include "tag/tag_throw.h"

namespace dbpager {

using namespace std;
using namespace dbp;

class console_application {
public:
	console_application(): app(application::instance()) {
		_config_file = filefs().get_system_config_dir() + config_file;
		app.set_name(app_name);
		app.set_description(_("dbPager Language interpretator"));
		app.register_cmdline_parameter(cmdline_parameter('v', "version",
		  _("show application version"), "", cmdline_parameter::OPTION),
		  create_delegate(this, &console_application::on_version));
		app.register_cmdline_parameter(cmdline_parameter('f', "config-file",
		  _("use an alternate configuration file"), _("name"),
		  cmdline_parameter::OPTION_WITH_VALUE),
		  create_delegate(this, &console_application::on_alternate_config));
		app.register_cmdline_parameter(cmdline_parameter(
		  _("URL|file [[param1=value] ... [paramN=value]]"),
		  cmdline_parameter::FILENAME),
		  create_delegate(this, &console_application::on_filename));
		app.on_execute(create_delegate(this, &console_application::on_execute));
	};

	// Main application code
	int on_execute() {
		// check for the script file
		if (filenames.empty()) {
			cerr << _("the URL or file name is not defined") << endl;
			return -1;
		}
		// convert local path to URL
		dbp::url u(filenames[0]);
		if (u.scheme == "") {
			u.scheme = "file";
			filefs fs;
			u.normalize(fs.get_current_dir() + "/");
		}
		interpreter dbpager(_config_file, app.get_logger());
		console_environment env(dbpager, filenames);
		// get interpreter instance from the application cache
		dbpager.get_app(u)->execute(env, cout);
		return 0;
	};

	application &app;
private:
	string _config_file;
	strings filenames;

	// Helper application environment class
	class console_environment: public environment {
	public:
		console_environment(dbpager::interpreter& interpreter, strings &vars):
		  environment(interpreter), _vars(vars) { };
		virtual void init_custom_params() {
			// parse script parameters
			for (strings::const_iterator i = _vars.begin() + 1;
			  i < _vars.end(); ++i) {
			  	string p_name, p_value;
			  	tokenize()(*i, p_name, p_value, false, "=");
				session->add_value(p_name, p_value);
			}
		};
	private:
		strings &_vars;
	};

	bool on_version(cmdline_parameter&) {
		cout << app_full_name << ". " <<
		  _("Copyright (c) 2008-2018 Dennis Prochko <wolfsoft@mail.ru>") << endl;
		return false;
	};

	bool on_alternate_config(cmdline_parameter &param) {
		_config_file = param.value;
		return true;
	};

	bool on_filename(cmdline_parameter &param) {
		filenames.push_back(string(param.value));
		return true;
	};

};

} // namespace

IMPLEMENT_APP(dbpager::console_application().app)

