/*
 * tag_split.cpp
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

#include "tag/tag_split.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_split::execute(context &ctx, std::ostream &out, const tag *caller) const {
	// create new context layer
	ctx.enter();
	// get parameters
	std::string sep = get_parameter(ctx, "separator");
	if (sep.empty())
		sep = ",";
	string source = get_parameter(ctx, "value");
	// split the string
	strings str;
	string::size_type pos = 0, old_pos = 0;
	while ((pos = source.find(sep, old_pos)) != string::npos) {
		str.push_back(trim()(source.substr(old_pos, pos - old_pos)));
		old_pos = pos + sep.length();
	}
	str.push_back(trim()(source.substr(old_pos, source.length())));
	// execute
	try {
		// initialize variables
		strings names = tokenize()(get_parameter(ctx, "name"), ",");
		switch (names.size()) {
			case 0: {
				// no variable name is defined
				throw tag_exception(_("parameter 'name' is undefined"));
				break;
			}
			case 1: {
				// defined one variable name, do the cycled split algorithm
				ctx.add_value(names.at(0), "");
				string order = get_parameter(ctx, "order");
				if (order.empty())
					order = "asc";
				if (order == "asc") {
					for (strings::const_iterator i = str.begin();
					  i != str.end(); ++i) {
						ctx.set_value(names.at(0), *i);
						// call inherited method
						tag_impl::execute(ctx, out, caller);
					}
				}
				else
				if (order == "desc") {
					for (strings::const_reverse_iterator i = str.rbegin();
					  i != str.rend(); ++i) {
						ctx.set_value(names.at(0), *i);
						// call inherited method
						tag_impl::execute(ctx, out, caller);
					}
				}
				else
					throw tag_exception((format(_("invalid 'order' parameter value. "
					"expected (asc,desc). found: {0}")) % order).str());
				break;
			}
			default: {
				// defined several variables, do the cycleless algorithm
				strings::const_iterator i = names.begin();
				strings::const_iterator j = str.begin();
				while ((i != names.end()) && (j != str.end())) {
					ctx.add_value(*i, *j);
					++i;
					++j;
				}
				// add remaining string
				if (j != str.end()) {
					--i;
					string v = ctx.get_value(*i);
					while (j != str.end()) {
						v += sep;
						v += *j;
						++j;
					}
					ctx.set_value(*i, v);
				}
				// call inherited method
				tag_impl::execute(ctx, out, caller);
				break;
			}
		} // switch
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
