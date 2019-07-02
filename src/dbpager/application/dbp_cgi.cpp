/*
 * dbp_cgi.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko
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

#include <stdlib.h>
#include <stdio.h>

#include <dcl/dclbase.h>
#include <dcl/cgi_application.h>

#include "dbpager/consts.h"
#include "dbpager/context.h"
#include "parser/parser.h"
#include "tag/tag_throw.h"
#include "interpreter/interpreter.h"

namespace dbpager {

using namespace std;
using namespace dbp;

class dbp_cgi {
public:
	dbp_cgi(): app(cgi_application::instance()) {
		app.on_handle_request(create_delegate(this, &dbp_cgi::process_request));
	};
	cgi_application &app;
private:
	// Main application code
	http_response process_request(const http_request &req) {
		// initialize the response
		http_response resp;
		resp.set_header("X-Powered-By", app_full_name + string(" (CGI)"));
		// correct PATH_TRANSLATED
		string p = req.get_path_translated();
		string pi;
		FILE *f = fopen(p.c_str(), "r");
		while (!f && !p.empty()) {
			string left, right;
			tokenize()(p, left, right);
			if (pi.empty())
				pi = right;
			else
				pi = right + "/" + pi;
			p = left;
			f = fopen(p.c_str(), "r");
		}
		if (!f) {
			resp.set_status(http_error::not_found);
			return resp;
		}
		fclose(f);
		http_request r(req);
		r.set_path_translated(p);
		r.set_path_info("/" + pi);
		// initialize environment
		interpreter dbpager(filefs().get_system_config_dir() + config_file, app.get_logger());
		http_environment env(dbpager, r);
		try {
			// convert application path to URL
			dbp::url u("file://" + env.get_path());
			ostringstream out(ostringstream::out | ostringstream::binary);
			// get interpreter instance from the application cache
			dbpager.get_app(u)->execute(env, out);
			// generate response
			env.init_response(resp);
			resp.set_content(out.str());
		} catch (parser_exception &e) {
			env.init_response(resp);
			if (e.code == 1) {
				resp.set_status(http_error::not_found);
				resp.set_content(e.what());
				resp.set_content_type("text/plain; charset=utf-8");
			} else {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
				resp.set_content_type("text/plain; charset=utf-8");
			}
		} catch (app_exception &e) {
			env.init_response(resp);
			resp.set_status(static_cast<http_error::http_error>(e.get_code()));
			switch (e.get_code()) {
				case 301:
				case 302:
				case 303:
				case 307:
				case 308:
					resp.set_header("Location", e.what());
					break;
				default:
					resp.set_content(e.what());
					resp.set_content_type("text/plain; charset=utf-8");
					break;
			}
		} catch (dbp::exception &e) {
			env.init_response(resp);
			resp.set_status(http_error::internal_server_error);
			resp.set_content(e.what());
			resp.set_content_type("text/plain; charset=utf-8");
			cerr << (format(_("Internal error: {0}")) % e.what()).str() << endl;
		}
		// send the response to the client
		return resp;
	};
};

} // namespace

IMPLEMENT_APP(dbpager::dbp_cgi().app)

