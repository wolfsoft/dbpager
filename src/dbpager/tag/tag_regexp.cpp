/*
 * tag_regexp.cpp
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

#include <pcrecpp.h>

#include <dcl/strutils.h>

#include "tag/tag_regexp.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_regexp::execute(context &ctx, std::ostream &out, const tag *caller) const {
	string expression = string("(") + get_parameter(ctx, "expr") + string(")");
	string value = get_parameter(ctx, "value");
	string variable = get_parameter(ctx, "name");
	string with = get_parameter(ctx, "with");
	if (variable.empty()) {
		variable = "result";
	}
	pcrecpp::RE re(expression,
	  pcrecpp::RE_Options(PCRE_CASELESS|PCRE_MULTILINE|PCRE_UTF8));
	if (!with.empty()) {
		// replace
		if (re.GlobalReplace(with, &value) > 0) {
			// create context layer and execute folded tags
			ctx.enter();
			try {
				ctx.add_value(variable, value);
				// call inherited method
				tag_impl::execute(ctx, out, caller);
			}
			catch (...) {
				ctx.leave();
				throw;
			}
			// free context layer created
			ctx.leave();
		}
	} else {
		// search
		string found;
		pcrecpp::StringPiece input(value);
		while (re.FindAndConsume(&input, &found)) {
			// create context layer and execute folded tags
			ctx.enter();
			try {
				ctx.add_value(variable, found);
				// call inherited method
				tag_impl::execute(ctx, out, caller);
			}
			catch (...) {
				ctx.leave();
				throw;
			}
			// free context layer created
			ctx.leave();
			found.clear();
		}
	}
}

} // namespace

