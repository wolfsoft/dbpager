/*
 * environment.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2009 - Dennis Prochko <wolfsoft@mail.ru>
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

#include "dbpager_application.h"

#include <parser/dbpx_parser.h>

namespace dbpager {

using namespace std;
using namespace dbp;

dbpager_application::dbpager_application(services *svc, const dbp::url &u): _services(svc) {
	// load and parse the script
	t.reset(dbpx_parser(u).parse());
	if (!t.get()) {
		throw parser_exception(1,
		  (format(_("Fatal error: can't parse file {0}")) % u.str()).str());
	}
}

void dbpager_application::execute(dbpager::environment &env, std::ostream &out) const {
	env.init_custom_params();
	local_context local(env.get_context());
	local.set_services_provider(_services);
	// if parsed ok, execute it
	t->execute(local, out, NULL);
}

} // namespace

