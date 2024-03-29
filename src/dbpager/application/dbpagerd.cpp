/*
 * dbpagerd.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2008-2024 Dennis Prochko <dennis.prochko@gmail.com>
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
#include <fstream>
#include <string>

#include <dcl/daemon_application.h>

#include "dbpager/consts.h"
#include "dbpager/context.h"
#include "parser/parser.h"
#include "tag/tag_throw.h"
#include "interpreter/interpreter.h"

#define TCP_NODELAY 1
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
		app.register_cmdline_parameter(cmdline_parameter('p', "pid-file",
		  _("store pid to this file"), _("name"),
		  cmdline_parameter::OPTION_WITH_VALUE),
		  create_delegate(this, &dbpagerd::on_pid_file));
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
	string _pid_file;
	interpreter *dbpager;

#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	typedef pair<dbp::http_request, string_map> req_env;

	// Show program version on -v command line parameter
	bool on_version(cmdline_parameter&) {
		cout << app_full_name << ". " <<
		  _("Copyright (c) 2008-2024 Dennis Prochko <dennis.prochko@gmail.com>") << endl;
		return false;
	}

	// Use the alternate configuration file on -f command line parameter
	bool on_alternate_config(cmdline_parameter &param) {
		_config_file = param.value;
		return true;
	}

	// Use the alternate configuration file on -f command line parameter
	bool on_pid_file(cmdline_parameter &param) {
		_pid_file = param.value;
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
		if (!_pid_file.empty()) {
			ofstream f;
			f.open(_pid_file);
			f << getpid();
			f.close();
		}
		try {
			dbpager = new interpreter(_config_file, app.get_logger());
			const dbp::app_config &config = dbpager->get_config();

			if (FCGX_Init() != 0)
				throw std::runtime_error(_("Initialization failed"));

			string socket_path = config.value("dbpagerd", "bind", string(fcgi_unix_socket_path));

			mode_t old_umask = umask(0);
			socket = FCGX_OpenSocket(socket_path.c_str(), 128); // SOMAXCONN
			umask(old_umask);

			if (socket < 0)
				throw std::runtime_error(
					(format(_("Can't bind to the {0}. Check your access rights or check for some other program using this resource")) % socket_path).str());

			// drop priveleges
			string user_name = config.value("dbpagerd", "user", string("nobody"));
			string group_name = config.value("dbpagerd", "group", string("nogroup"));

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

		while (FCGX_Accept_r(&request) == 0) {
			req_env req = get_request(request.envp, request.in);
			http_response resp;

			try {
				resp = process_cgi_request(req);
			} catch (std::exception &e) {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
				resp.set_content_type("text/plain; charset=utf-8");
			}

			send_response(resp, request.out);

			// No need because FCGX_Accept_r do the same internally
			// FCGX_Finish_r(&request);
		}

		FCGX_Finish_r(&request);

	};

	void send_response(const http_response &response, FCGX_Stream *stream) {
		stringstream out;
		out << response;

		string status = string("Status: ") + response.get_status();
		FCGX_PutStr(status.c_str(), status.length(), stream);
		FCGX_PutS("\r\n", stream);

		FCGX_PutStr(out.str().c_str(), out.str().length(), stream);
		// No need because data will flushed on finish request
		// FCGX_FFlush(stream);
	}

	http_response process_cgi_request(const req_env &req) {
		// initialize the response
		http_response resp;
		resp.set_header("X-Powered-By", app_full_name + string(" (FastCGI)"));

		fastcgi_environment env(*dbpager, req);
		try {
			// convert application path to URL
			dbp::url u("file://" + env.get_path());
			ostringstream out(ostringstream::out | ostringstream::binary);
			// get interpreter instance from the application cache
			dbpager->get_app(u)->execute(env, out);
			// generate response
			env.init_response(resp);
			resp.set_content(out.str());

		} catch (const parser_exception &e) {
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

		} catch (const dbpager::app_exception &e) {
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
					if (resp.get_content_type().empty())
						resp.set_content_type("text/plain; charset=utf-8");
					break;
			}

		} catch (const dbp::exception &e) {
			env.init_response(resp);
			resp.set_status(http_error::internal_server_error);
			resp.set_content(e.what());
			resp.set_content_type("text/plain; charset=utf-8");
			dbpager->get_logger().error((format(_("Internal error: {0}")) % e.what()).str());
		}
		// send the response to the client
		return resp;
	};

	/* copypaste from cgi_application.cpp */
	req_env get_request(char **env, FCGX_Stream *stream) {
		http_request req;
		int c_len = 0;
		string_map env_vars;
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
			else if (left == "CONTENT_LENGTH")
				c_len = from_string<int>(right);
			else if (left == "CONTENT_TYPE")
				req.set_content_type(right);
			else if (left == "GATEWAY_INTERFACE")
				req.set_gateway_interface(right);
			else if (left == "PATH_INFO")
				req.set_path_info(right);
			else if (left == "PATH_TRANSLATED")
				req.set_path_translated(right);
			else if (left == "QUERY_STRING")
				req.set_query_string(right);
			else if (left == "REMOTE_ADDR")
				req.set_remote_addr(right);
			else if (left == "REMOTE_HOST")
				req.set_remote_host(right);
			else if (left == "REMOTE_IDENT")
				req.set_remote_ident(right);
			else if (left == "REMOTE_USER")
				req.set_remote_user(right);
			else if (left == "REQUEST_METHOD")
				req.set_method(right);
			else if (left == "SCRIPT_NAME")
				req.set_script_name(right);
			else if (left == "SERVER_NAME")
				req.set_server_name(right);
			else if (left == "SERVER_PORT")
				req.set_server_port(from_string<int>(right));
			else if (left == "SERVER_PROTOCOL")
				req.set_server_protocol(right);
			else if (left == "SERVER_SOFTWARE")
				req.set_server_software(right);
			else if (left == "HTTPS")
				req.set_https(right == "on");
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
				} else {
					env_vars[string("ENV_") + left] = right;
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

		return make_pair(req, env_vars);
	}

	// Helper application environment class
	class fastcgi_environment: public http_environment {
	public:
		explicit fastcgi_environment(dbpager::interpreter &interpreter, const req_env &req):
		  http_environment(interpreter, req.first), _req(req) { };
		virtual void init_custom_params() {
			http_environment::init_custom_params();
			const string_map &env = _req.second;
			for (string_map::const_iterator i = env.begin(); i != env.end(); ++i) {
				session->add_value(i->first, i->second);
			}
		};
	private:
		const req_env &_req;
	};

};

} // namespace

IMPLEMENT_APP(dbpager::dbpagerd().app);
