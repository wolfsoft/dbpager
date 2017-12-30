/*
 * tag_substr.cpp
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

#include "tag/tag_substr.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void replace_with(string &source, const string &find, const string &replace) {
	const string::size_type r_length = replace.length();
	const string::size_type f_length = find.length();
	for (string::size_type i = 0; (i = source.find(find, i)) != string::npos;) {
		source.replace(i, f_length, replace);
		i += r_length;
	}
}

void tag_substr::execute(context &ctx, ostream &out, const tag *caller) const {
	// create new context layer
	ctx.enter();
	try {
		// get parameters
		string str = get_parameter(ctx, "value");
		string what = get_parameter(ctx, "what");
		string with = get_parameter(ctx, "with");
		// replace substrings
		replace_with(str, what, with);
		// create variable with result
		ctx.add_value(get_parameter(ctx, "name"), str);
		// call inherited method
		tag_impl::execute(ctx, out, caller);
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
