/*
 * tag_task.cpp
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

#include "tag_server.h"
#include "tag_task.h"

namespace dbpager {
namespace gearman {

using namespace std;
using namespace dbp;

void tag_task::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	const string &function = get_parameter(ctx, "function");
	string server_ptr = ctx.get_value("@GEARMAN:server@");
	if (server_ptr.empty())
		throw tag_task_exception(
		  _("gearman server is not defined in the current "
		    "context"));
	server *gm = (server*)from_string<void*>(server_ptr);
	ctx.enter();
	try {
		// cache protected output
		ostringstream s(ostringstream::out | ostringstream::binary);
		// call inherited method
		tag_impl::execute(ctx, s, caller);
		gm->add_task(function, s.str());
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

}} // namespace
