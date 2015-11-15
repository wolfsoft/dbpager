/*
 * tag_tag.cpp
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

#include <ostream>
#include <typeinfo>

#include <dcl/strutils.h>

#include <dbpager/context.h>
#include <dbpager/tag_tag.h>
#include <dbpager/tag_usr.h>

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_tag::add_parameter(const std::string &p_name, tag *p_value) {
	// call inherited method
	tag_impl::add_parameter(p_name, p_value);
	// setting up custom tag name
	if (p_name == "name") {
		ostringstream s;
		context ctx(NULL);
		p_value->execute(ctx, s, this);
		name = "@" + s.str();
	}
}

void tag_tag::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	if (caller && (typeid(*caller) == typeid(tag_usr))) {
		tag_impl::execute(ctx, out, caller);
	}
}

} // namespace

