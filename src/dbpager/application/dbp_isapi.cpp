/*
 * dbp_isapi.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2010 Dennis Prochko <wolfsoft@mail.ru>
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

#include <string>

#include <dcl/dclbase.h>
#include <dcl/isapi_application.h>

#include "dbpager/consts.h"
#include "dbpager/context.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"

#include "tag/tag_throw.h"

namespace dbpager {

using namespace std;
using namespace dbp;

class dbpager_module {
public:
	dbpager_module() {
		// register the execute event handler
		app.on_handle_request(create_delegate(this,
		  &dbpager_module::on_handle_request));
	};
	isapi_application& application_impl() {
		return app;
	};
private:
	// framework application class
	isapi_application app;
	interpreter dbpager;
	// Main application code
	http_response on_handle_request(const http_request &req) {
		// initialize the response
		http_response resp;
		resp.set_header("X-Powered-By", app_full_name + string(" (IIS module)"));
		http_environment env(dbpager.get_options(), req);
		try {
			// convert application path to URL
			dbp::url u(env.get_path());
			u.scheme = "file";
			ostringstream out(ostringstream::out | ostringstream::binary);
			// get interpreter instance from the application cache
			dbpager.get_app(u)->execute(env, out);
			// generate response
			env.init_response(resp);
			resp.set_content(out.str());
		}
		catch(parser_exception &e) {
			env.init_response(resp);
			if (e.code == 1) {
				resp.set_status(http_error::not_found);
				resp.set_content(e.what());
			} else {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
				cout << e.what() << endl;
			}
		} catch (app_exception &e) {
			env.init_response(resp);
			resp.set_status(static_cast<http_error::http_error>(e.get_code()));
			switch (e.get_code()) {
				case 301:
				case 302:
					resp.set_header("Location", e.what());
					break;
				default:
					resp.set_content(e.what());
					break;
			}
		}
		// send the response to the client
		return resp;
	};
};

} // namespace

// initialize and run the application via provided macros
IMPLEMENT_APP(dbpager_module, dbpager::dbpager_module);
