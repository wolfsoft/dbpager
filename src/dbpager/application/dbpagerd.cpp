/*
 * dbpagerd.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2014 Dennis Prochko <wolfsoft@mail.ru>
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
 
#include <stdexcept>
#include <iostream>
#include <string>

#include <dcl/daemon_application.h>

#include "dbpager/consts.h"
#include "dbpager/context.h"
#include "parser/parser.h"
#include "tag/tag_throw.h"
#include "interpreter/interpreter.h"

#include "fcgiapp.h"

namespace dbpager {

using namespace std;
using namespace dbp;

class dbpagerd {
public:
	dbpagerd():
	  app(daemon_application::instance()), socket(0), dbpager(NULL) {
	  	_config_file = filefs().get_system_config_dir() + config_file;
		// Setup application properties
		app.set_name(app_name);
		app.set_description(_("dbPager FastCGI Server"));
		// Register event handlers
		app.register_cmdline_parameter(cmdline_parameter('v', "version",
		  _("show application version"), "", cmdline_parameter::OPTION),
		  create_delegate(this, &dbpagerd::on_version));
		app.register_cmdline_parameter(cmdline_parameter('f', "config-file",
		  _("use an alternate configuration file"), _("name"),
		  cmdline_parameter::OPTION_WITH_VALUE),
		  create_delegate(this, &dbpagerd::on_alternate_config));
		app.on_reload_configuration(create_delegate(this,
		  &dbpagerd::reload_configuration));
		app.on_execute(create_delegate(this, &dbpagerd::execute));
		app.on_terminate(create_delegate(this, &dbpagerd::terminate));
	}
	virtual ~dbpagerd() {
		if (dbpager)
			delete dbpager;
	}
	daemon_application &app;
private:
	int socket;
	vector<thread*> threads;
	string _config_file;
	interpreter *dbpager;

	// Show program version on -v command line parameter
	bool on_version(cmdline_parameter&) {
		cout << app_full_name << ". " <<
		  _("Copyright (c) 2008-2015 Dennis Prochko <wolfsoft@mail.ru>") << endl;
		return false;
	}

	// Use the alternate configuration file on -f command line parameter
	bool on_alternate_config(cmdline_parameter &param) {
		_config_file = param.value;
		return true;
	}

	// Reload configuration file
	void reload_configuration() {
		dbpager->reload();
	}

	// Terminate the server
	void terminate() {
		FCGX_ShutdownPending();
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			(*i)->stop();
		}
	}

	// Main application code
	int execute() {

		try {
			dbpager = new interpreter(_config_file, app.get_logger());
			const dbp::app_config &config = dbpager->get_config();

			if (FCGX_Init() != 0)
				throw std::runtime_error(_("Initialization failed"));

			string socket_path = config.value("dbpagerd", "bind", fcgi_unix_socket_path);

			mode_t old_umask = umask(0);
			socket = FCGX_OpenSocket(socket_path.c_str(), 128); // SOMAXCONN
			umask(old_umask);

			if (socket < 0)
				throw std::runtime_error(
					(format(_("Can't bind to the {0}. Check your access rights or check for some other program using this resource")) % socket_path).str());

			// drop priveleges
			string user_name = config.value("dbpagerd", "user", "nobody");
			string group_name = config.value("dbpagerd", "group", "nogroup");

			if (group *grp = getgrnam(group_name.c_str())) {
				if (setegid(grp->gr_gid) != 0)
					throw std::runtime_error(
						(format(_("Can't switch current group to {0} - check your access rights")) % group_name).str());
			} else
				throw std::runtime_error(
					(format(_("No such group {0}")) % user_name).str());

			if (passwd *user = getpwnam(user_name.c_str())) {
				if (seteuid(user->pw_uid) != 0)
					throw std::runtime_error(
						(format(_("Can't switch current user to {0} - check your access rights")) % user_name).str());
			} else
				throw std::runtime_error(
					(format(_("No such user {0}")) % user_name).str());

			// run worker threads
			int t_num = config.value("dbpagerd", "threads", 8);
			for (int i = 0; i < t_num; i++) {
				thread *t = new thread();
				t->on_execute(create_delegate(this, &dbpagerd::process_request));
				threads.push_back(t);
				t->start();
			}

			// wait for server exit
			for (vector<thread*>::const_iterator i = threads.begin();
			  i != threads.end(); ++i) {
				(*i)->wait_for();
			}

		} catch(const std::exception &e) {
			cerr << format(_("Internal error: {0}")) % e.what() << endl;
		} catch(...) {
			cerr << _("Unknown error") << endl;
		}

		// clean up
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			delete (*i);
		}

		return 0;
	}

	// single request processing
	void process_request(thread_int&) {
		FCGX_Request request;
		FCGX_InitRequest(&request, socket, FCGI_FAIL_ACCEPT_ON_INTR);

		while(true) {

			if (FCGX_Accept_r(&request) < 0)
				break;

			http_request req = get_request(request.envp, request.in);
			http_response resp;

			try {
				resp = process_cgi_request(req);
			} catch (std::exception &e) {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
			}

			send_response(resp, request.out);

			FCGX_Finish_r(&request);
		}

	};

	void send_response(const http_response &response, FCGX_Stream *stream) {
		stringstream out;
		out << response;

		FCGX_SetExitStatus(response.get_status_code(), stream);
		FCGX_PutStr(out.str().c_str(), out.str().length(), stream);
		FCGX_PutS("\r\n", stream);
		FCGX_FFlush(stream);
	}

	http_response process_cgi_request(const http_request &req) {
		// initialize the response
		http_response resp;
		resp.set_header("X-Powered-By", app_full_name + string(" (FastCGI)"));

		http_environment env(*dbpager, req);
		try {
			// convert application path to URL
			dbp::url u("file://" + env.get_path());
			ostringstream out(ostringstream::out | ostringstream::binary);
			// get interpreter instance from the application cache
			dbpager->get_app(u)->execute(env, out);
			// generate response
			env.init_response(resp);
			resp.set_content(out.str());
		} catch (parser_exception &e) {
			env.init_response(resp);
			if (e.code == 1) {
				resp.set_status(http_error::not_found);
				resp.set_content(e.what());
			} else {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
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
		} catch (dbp::exception &e) {
			env.init_response(resp);
			resp.set_status(http_error::internal_server_error);
			resp.set_content(e.what());
			dbpager->get_logger().error((format(_("Internal error: {0}")) % e.what()).str());
		}
		// send the response to the client
		return resp;
	};

	/* copypaste from cgi_application.cpp */
	http_request get_request(char **env, FCGX_Stream *stream) {
		http_request req;
		int c_len = 0;
		// read environment variables
		while (env && *env) {
			// split PARAM=VALUE of the environment
			string line(*env), left, right;
			string::size_type pos = line.find_first_of("=");
			if (pos == string::npos) {
				left = line;
			} else {
				left = line.substr(0, pos);
				right = line.substr(pos + 1, line.length() - pos - 1);
			}
			// analyze PARAM
			if (left == "AUTH_TYPE")
				req.set_auth_type(right);
			else
			if (left == "CONTENT_LENGTH")
				c_len = from_string<int>(right);
			else
			if (left == "CONTENT_TYPE")
				req.set_content_type(right);
			else
			if (left == "GATEWAY_INTERFACE")
				req.set_gateway_interface(right);
			else
			if (left == "PATH_INFO")
				req.set_path_info(right);
			else
			if (left == "PATH_TRANSLATED")
				req.set_path_translated(right);
			else
			if (left == "QUERY_STRING")
				req.set_query_string(right);
			else
			if (left == "REMOTE_ADDR")
				req.set_remote_addr(right);
			else
			if (left == "REMOTE_HOST")
				req.set_remote_host(right);
			else
			if (left == "REMOTE_IDENT")
				req.set_remote_ident(right);
			else
			if (left == "REMOTE_USER")
				req.set_remote_user(right);
			else
			if (left == "REQUEST_METHOD")
				req.set_method(right);
			else
			if (left == "SCRIPT_NAME")
				req.set_script_name(right);
			else
			if (left == "SERVER_NAME")
				req.set_server_name(right);
			else
			if (left == "SERVER_PORT")
				req.set_server_port(from_string<int>(right));
			else
			if (left == "SERVER_PROTOCOL")
				req.set_server_protocol(right);
			else
			if (left == "SERVER_SOFTWARE")
				req.set_server_software(right);
			else {
				// is it a HTTP header?
				if (left.compare(0, 5, "HTTP_") == 0) {
					left = left.substr(5, left.length() - 5);
					string::size_type pos = left.find_first_of("_");
					while (pos != string::npos) {
						left[pos] = '-';
						pos = left.find_first_of("_");
					}
					req.set_header(left, right);
				}
			}
			// check next environment variable
			env++;
		} // while
		// if there is a content, read it from standard input
		if (c_len > 0) {
			char *buf = new char[c_len + 1];
			int n = FCGX_GetStr(buf, c_len, stream);
			req.set_content(n, buf);
			delete[] buf;
		}

		return req;
	}

};

} // namespace

IMPLEMENT_APP(dbpager::dbpagerd().app);
