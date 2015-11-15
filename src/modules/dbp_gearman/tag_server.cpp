/*
 * tag_server.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2012 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <dcl/strutils.h>
#include <dcl/uuid.h>

#include "tag_server.h"

namespace dbpager {
namespace gearman {

using namespace std;
using namespace dbp;

server::server() {
	gm = gearman_client_create(NULL);
}

server::~server() {
	if (gm)
		gearman_client_free(gm);
}

void server::add_servers(const std::string &servers) {
	gearman_return_t rslt = gearman_client_add_servers(gm, servers.c_str());
	if (rslt != GEARMAN_SUCCESS)
		throw tag_server_exception(std::string(gearman_client_error(gm)));
}

void server::add_task(const std::string &function, const std::string &workload) {
	char job_handle[GEARMAN_JOB_HANDLE_SIZE];
	uuid id;
	gearman_return_t rslt = gearman_client_do_background(gm, function.c_str(),
	  id.str().c_str(), workload.c_str(), workload.length(), job_handle);
	if (rslt != GEARMAN_SUCCESS)
		throw tag_server_exception(std::string(gearman_client_error(gm)));
}

void tag_server::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &servers = get_parameter(ctx, "host");
	server &gm = server::instance();
	gm.add_servers(servers);
	ctx.enter();
	try {
		// save the pointer to server for nested tags
		ctx.add_value("@GEARMAN:server@", to_string<server*>(&gm));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

}} // namespace
