/*
 * tag_param.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2012 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <vector>

#include <dcl/strutils.h>

#include "tag_param.h"

namespace dbpager {
namespace mongodb {

using namespace std;
using namespace dbp;
using namespace mongo;

void tag_param::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	string p_ptr = ctx.get_value("@MONGODB:PARAM@");
	if (p_ptr.empty()) {
		throw tag_param_exception(
		  _("query is not defined in the current context"));
	}
	BSONObjBuilder *b = (BSONObjBuilder*)from_string<void*>(p_ptr);

	const string &value = get_parameter(ctx, "value");
	const string &name = get_parameter(ctx, "name");
	if (name.empty()) {
		throw tag_database_exception(
		  _("parameter name (name) is not defined"));
	} else
		b->append(name, value);

}

}} // namespace

