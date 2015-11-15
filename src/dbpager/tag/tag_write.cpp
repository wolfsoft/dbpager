/*
 * tag_write.cpp
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

#include <iostream>
#include <fstream>

#include <dcl/strutils.h>

#include "tag/tag_write.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_write::execute(context &ctx, std::ostream &out, const tag *caller) const {
	stringstream buf(stringstream::in | stringstream::out | stringstream::binary);
	// call inherited method
	tag_impl::execute(ctx, buf, caller);
	// get url to write to
	string href = get_parameter(ctx, "href");
	// open file to write
	ofstream s(href.c_str(), ofstream::out | ofstream::ate | ofstream::binary);
	// write entire buffer buffer into a file
	try {
		s.exceptions(ofstream::eofbit | ofstream::failbit | ofstream::badbit);
		s << buf.str();
	} catch (...) {
		throw tag_exception(
		  (format(_("can't write to file {0}")) % href).str());
	}
}

} // namespace
