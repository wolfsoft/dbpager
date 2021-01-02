/*
 * tag_map.cpp
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
#include <json/json.h>

#include "tag/tag_map.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_map::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");

	string_map data;

	ctx.enter();
	try {
		// save the pointer to data for nested tags
		ctx.add_value(string("@MAP@") + name, dbp::to_string<string_map*>(&data));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

void tag_map_set::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string key = get_parameter(ctx, "key");
	const string value = get_parameter(ctx, "value");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);
	data->insert({key, value});
}

void tag_map_unset::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string key = get_parameter(ctx, "key");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);
	data->erase(key);
}

void tag_map_element::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	const string key = get_parameter(ctx, "key");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	auto it = data->find(key);
	if (it == data->end())
		throw tag_map_exception(
		  (format(_("element '{0}' was not found on map '{1}'")) % key % name).str());

	ctx.enter();
	try {
		ctx.add_value(name + "_" + it->first, it->second);
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
	}
}

void tag_map_elements::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	for (auto it = data->begin(); it != data->end(); ++it) {
		ctx.enter();
		try {
			ctx.add_value(name + "_key", it->first);
			ctx.add_value(name + "_value", it->second);
			tag_impl::execute(ctx, out, caller);
			ctx.leave();
		} catch (...) {
			ctx.leave();
			throw;
		}
	}
}

void tag_map_from_json::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	ostringstream s(ostringstream::out | ostringstream::binary);
	tag_impl::execute(ctx, s, caller);

	Json::Value root;
	Json::Reader reader;
	Json::FastWriter writer;
	if (!reader.parse(s.str(), root)) {
		throw dbp::exception(_("invalid JSON structure of the request"));
	}

	if (root.isObject()) {
		data->clear();
		// parse json object members
		Json::Value::Members members = root.getMemberNames();
		for (Json::Value::Members::const_iterator i = members.begin(); i != members.end(); ++i) {
			if (root[*i].isConvertibleTo(Json::ValueType::stringValue)) {
				data->insert({*i, root[*i].asString()});
			} else {
				data->insert({*i, writer.write(root[*i])});
			}
		}
	}
}

void tag_map_to_json::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	Json::Value root;
	for (auto it = data->begin(); it != data->end(); ++it) {
		root[it->first] = it->second;
	}

	Json::FastWriter writer;
	out << writer.write(root);
}

} // namespace
