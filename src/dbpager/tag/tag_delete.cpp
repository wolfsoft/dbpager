/*
 * tag_delete.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2010 Dennis Prochko <wolfsoft@mail.ru>
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

#include <stdio.h>
#include <dcl/strutils.h>

#include "tag/tag_delete.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_delete::execute(context &ctx, std::ostream &out, const tag *caller) const {
	// get url to delete
	string href = get_parameter(ctx, "href");
	// remove file
	if (remove(href.c_str()) != 0)
		throw tag_exception(
		  (format(_("file {0} can't be deleted")) % href).str());
	// call inherited method
	tag_impl::execute(ctx, out, caller);
}

} // namespace
