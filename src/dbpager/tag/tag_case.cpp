/*
 * tag_case.cpp
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

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_case::execute(context &ctx, std::ostream &out, const tag *caller) const {
	string p = get_parameter(ctx, "match");
	if (p.empty() || (ctx.get_value("@DBP:SWITCH@") == p)) {
		// call inherited method
		tag_impl::execute(ctx, out, caller);
		throw tag_case_exception("");
	}
}

} // namespace
