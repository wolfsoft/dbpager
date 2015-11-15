/*
 * tag_read.cpp
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

#include <algorithm>
#include <iostream>
#include <fstream>

#include <dcl/strutils.h>

#include "tag/tag_read.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_read::execute(context &ctx, std::ostream &out, const tag *caller) const {
	// call inherited method
	tag_impl::execute(ctx, out, caller);
	// get url to read from
	string href = get_parameter(ctx, "href");
	// open file to read
	ifstream s(href.c_str(), ifstream::in | ifstream::binary);
	try {
		s.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
		// read entire file into the output
		copy(istreambuf_iterator<char>(s), istreambuf_iterator<char>(),
		  ostreambuf_iterator<char>(out));
	} catch (...) {
		throw tag_exception(
		  (format(_("file {0} not found or read error")) % href).str());
	}
}

} // namespace
