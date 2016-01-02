/*
 * tag_call.cpp
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

#include <dcl/strutils.h>

#include <dbpager/tag_tag.h>
#include <dbpager/tag_usr.h>

#include "tag/tag_call.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_call::execute(context &ctx, std::ostream &out, const tag *caller) const {

	const string &name = get_parameter(ctx, "name");

	// create new context layer
	ctx.enter();
	try {
		for (parameters::const_iterator i = params.begin();
		  i != params.end(); ++i) {
		  	if (i->first != "name") {
				// create and initialize the variable from parameter
				ostringstream s;
				(i->second)->execute(ctx, s, this);
				ctx.add_value(i->first, s.str());
			}
		}

		tag* t = NULL;
		tag *p = parent;
		while (p && !(t = p->get_child("@" + name))) {
			p = p->get_parent();
		}
		if (!t)
			throw tag_exception(
			  (format(_("custom tag '{0}' is not defined at this scope")) % name).str());

		// execute custom tag
		ctx.push(this);
		try {
			static_cast<tag_tag*>(t)->execute(ctx, out, this);
		} catch (...) {
			ctx.pop();
			throw;
		}
		ctx.pop();
	}
	catch (...) {
		// free context layer created
		ctx.leave();
		throw;
	}
	// free context layer created
	ctx.leave();
}

} // namespace
