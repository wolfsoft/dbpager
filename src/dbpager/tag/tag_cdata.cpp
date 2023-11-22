/*
 * tag_cdata.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2023 Dennis Prochko <dennis.prochko@gmail.com>
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
#include "tag/tag_cdata.h"

namespace dbpager {

using namespace std;

static std::string keep_entities(const std::string& src) {
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

void tag_cdata::execute(context &ctx, std::ostream &out, const tag *caller) const {
	auto root = ctx.get_value("@HTML:FIRST@");
	if (root.empty()) {
		out << get_text();
	} else {
		out << keep_entities(get_text());
	}
}

} // namespace
