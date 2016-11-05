/*
 * tag_try.cpp
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

#include <sstream>

#include <dcl/strutils.h>

#include "tag/tag_try.h"
#include "tag/tag_catch.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_try::execute(context &ctx, std::ostream &out, const tag *caller) const {
	try {
		// cache protected output
		ostringstream s(ostringstream::out | ostringstream::binary);
		// call inherited method
		tag_impl::execute(ctx, s, caller);
		out << s.str();
	} catch (app_exception &e) {
		// find a catch tag
		tag_catch *c = static_cast<tag_catch*>(get_child("catch"));
		if (!c)
		  throw;
		// catch a program exception
		c->process_exception(ctx, out, e, caller);
	} catch (std::exception &e) {
		// find a catch tag
		tag_catch *c = static_cast<tag_catch*>(get_child("catch"));
		if (!c)
		  throw;
		// catch a program exception
		c->process_exception(ctx, out, e, caller);
	}
}

} // namespace
