/*
 * tag_execute.cpp
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

#include "tag/tag_execute.h"

#include <iostream>
#include <typeinfo>

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_execute::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const tag *c = ctx.pop();
	if (c) {
		const string &name = get_parameter(ctx, "name");
		if (!name.empty()) {
			tag_tag *t = static_cast<tag_tag*>(c->get_child(string("@") + name));
			if (!t)
				throw tag_exception(
				  (format(_("custom tag '{0}' is not defined at this scope")) % name).str());

			t->execute(ctx, out, c);
			ctx.push(c);

		} else {
			static_cast<const tag_usr*>(c)->execute_direct(ctx, out, this);
		}
	}
}

} // namespace
