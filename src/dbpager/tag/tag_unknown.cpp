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
#include <functional>
#include "tag/tag_unknown.h"

namespace dbpager {

using namespace std;

static std::string htmlentities(const std::string& src) {
	std::string rslt;
	for(auto c: src) {
		switch (c) {
			case '\'':
				rslt += "&apos;";
				break;
			case '"':
				rslt += "&quot;";
				break;
			case '<':
				rslt += "&lt;";
				break;
			case '>':
				rslt += "&gt;";
				break;
			default:
				rslt += c;
		}
	}
	return rslt;
}

static std::string stub(const std::string& src) {
	std::string rslt;
	for(auto c: src) {
		switch (c) {
			case '\'':
				rslt += "\x01";
				break;
			case '"':
				rslt += "\x02";
				break;
			case '<':
				rslt += "\x03";
				break;
			case '>':
				rslt += "\x04";
				break;
			default:
				rslt += c;
		}
	}
	return rslt;
}

static void htmlentitiesout(const std::string& src, std::ostream &out) {
	for(auto c: src) {
		switch (c) {
			case 1:
				out << '\'';
				break;
			case 2:
				out << '\"';
				break;
			case 3:
				out << '<';
				break;
			case 4:
				out << '>';
				break;
			default:
				out << c;
				break;
		}
	}
}

void tag_unknown::execute(context &ctx, std::ostream &out, const tag *caller) const {
	auto root = ctx.get_value("@HTML:FIRST@");
	if (root.empty()) {
		ctx.enter();
		ctx.add_value("@HTML:FIRST@", "1");
		try {
			ostringstream s(stringstream::out | ostringstream::binary);
			real_execute(ctx, s, caller);
			htmlentitiesout(s.str(), out);
			ctx.leave();
		} catch(...) {
			ctx.leave();
			throw;
		}
	} else {
		real_execute(ctx, out, caller);
	}
}

void tag_unknown::real_execute(context &ctx, std::ostream &out, const tag *caller) const {
	// open the tag
	out << "\x03" << name;
	
	std::function<std::string(const std::string&)> convert;
	auto asis = params.find("@DBP:PARAM@asis");
	if (asis == params.end() || asis->second->get_text() != std::string("1")) {
		convert = htmlentities;
	} else {
		convert = stub;
	}

	// write tag parameters
	for (parameters::const_iterator i = params.begin();
	  i != params.end(); ++i) {
	  	if ((i->first).length() > 0) {
	  		if (i->first.rfind("@DBP:PARAM@", 0) == 0) {
	  			continue;
	  		}
			ostringstream s(stringstream::out | ostringstream::binary);
			(i->second)->execute(ctx, s, this);
			const string &v = s.str();
			if (!v.empty())
				out << " " << i->first << "=\x02" << convert(v) << "\x02";
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
		out << "\x04" << convert(data) << "\x03/" << name << "\x04";
	else {
		if (data.empty())
			out << "/\x04";
		else {
			out << "\x04" << convert(data) << "\x03/" << name << "\x04";
		}
	}

}

} // namespace
