/*
 * tag_list.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko
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

#include <dcl/strutils.h>

#include "tag/tag_list.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_list::execute(context &ctx, std::ostream &out, const tag *caller) const {
	string name = get_parameter(ctx, "name");

	vector<string> data;

	ctx.enter();
	try {
		// save the pointer to data for nested tags
		ctx.add_value(string("@LIST@") + name, dbp::to_string<vector<string>*>(&data));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

void tag_list_add::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string value = get_parameter(ctx, "value");

	string data_ptr = ctx.get_value(string("@LIST@") + name);
	if (data_ptr.empty()) {
		throw tag_list_exception(
		  (format(_("list '{0}' was not defined in the current context")) % name).str());
	}
	vector<string> *data = (vector<string>*)from_string<void*>(data_ptr);
	data->push_back(value);
}

void tag_list_remove::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string key = get_parameter(ctx, "number");

	string data_ptr = ctx.get_value(string("@LIST@") + name);
	if (data_ptr.empty()) {
		throw tag_list_exception(
		  (format(_("list '{0}' was not defined in the current context")) % name).str());
	}
	vector<string> *data = (vector<string>*)from_string<void*>(data_ptr);
	data->erase(data->begin() + from_string<int>(key));
}

void tag_list_element::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string key = get_parameter(ctx, "number");

	string data_ptr = ctx.get_value(string("@LIST@") + name);
	if (data_ptr.empty()) {
		throw tag_list_exception(
		  (format(_("list '{0}' was not defined in the current context")) % name).str());
	}
	vector<string> *data = (vector<string>*)from_string<void*>(data_ptr);

	ctx.enter();
	try {
		ctx.add_value(name + "_value", data->at(from_string<int>(key)));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

void tag_list_elements::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");

	string data_ptr = ctx.get_value(string("@LIST@") + name);
	if (data_ptr.empty()) {
		throw tag_list_exception(
		  (format(_("list '{0}' was not defined in the current context")) % name).str());
	}
	vector<string> *data = (vector<string>*)from_string<void*>(data_ptr);

	for (auto it = data->begin(); it != data->end(); ++it) {
		ctx.enter();
		try {
			ctx.add_value(name + "_value", *it);
			tag_impl::execute(ctx, out, caller);
			ctx.leave();
		} catch (...) {
			ctx.leave();
			throw;
		}
	}
}

} // namespace
