/*
 * tag_switch.cpp
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

#include "tag/tag_case.h"
#include "tag/tag_switch.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_switch::execute(context &ctx, std::ostream &out, const tag *caller) const {
	// create new context layer
	ctx.enter();
	try {
		ctx.add_value("@DBP:SWITCH@", get_parameter(ctx, "test"));
		// call inherited method
		tag_impl::execute(ctx, out, caller);
	}
	catch (tag_case_exception &e) { }
	catch (...) {
		// free context layer created
		ctx.leave();
		throw;
	}
	// free context layer created
	ctx.leave();
}

} // namespace
