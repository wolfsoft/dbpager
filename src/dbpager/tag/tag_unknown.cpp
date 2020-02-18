/*
 * tag_unknown.cpp
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

#include <algorithm>
#include "tag/tag_unknown.h"

namespace dbpager {

using namespace std;

void tag_unknown::execute(context &ctx, std::ostream &out, const tag *caller) const {
	// open the tag
	out << "<" << name;
	// write tag parameters
	for (parameters::const_iterator i = params.begin();
	  i != params.end(); ++i) {
	  	if ((i->first).length() > 0) {
			ostringstream s(stringstream::out | ostringstream::binary);
			(i->second)->execute(ctx, s, this);
			const string &v = s.str();
			if (!v.empty())
				out << " " << i->first << "=\"" << v << "\"";
		}
	}

	ostringstream s(ostringstream::out | ostringstream::binary);
	// call inherited method
	tag_impl::execute(ctx, s, caller);
	const string &data = s.str();

	static const vector<string> tags = {
		"base",
		"meta",
		"link",
		"hr",
		"br",
		"basefont",
		"param",
		"img",
		"area",
		"input",
		"isindex",
		"col"
	};

	if (find(tags.begin(), tags.end(), name) == tags.end())
		out << ">" << data << "</" << name << ">";
	else {
		if (data.empty())
			out << "/>";
		else {
			out << ">" << data << "</" << name << ">";
		}
	}

}

} // namespace
