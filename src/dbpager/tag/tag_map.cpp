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
	string_map data_types;

	ctx.enter();
	try {
		// save the pointer to data for nested tags
		ctx.add_value(string("@MAP@") + name, dbp::to_string<string_map*>(&data));
		ctx.add_value(string("@MAP_TYPES@") + name, dbp::to_string<string_map*>(&data_types));
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
	const string type = get_parameter(ctx, "type");
	const string value = get_parameter(ctx, "value");

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	data->insert({key, value});

	if (!type.empty()) {
		string data_type_ptr = ctx.get_value(string("@MAP_TYPES@") + name);
		if (data_type_ptr.empty()) {
			throw tag_map_exception(
				(format(_("map '{0}' was not defined in the current context")) % name).str()
			);
		}
		string_map *data_types = (string_map*)from_string<void*>(data_type_ptr);

		data_types->insert({key, type});
	}
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

	ctx.enter();
	try {
		auto it = data->find(key);
		if (it != data->end()) {
			ctx.add_value(name + "_" + it->first, it->second);
		}
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

void tag_map_values::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string name = get_parameter(ctx, "name");
	string prefix = get_parameter(ctx, "prefix");
	if (prefix.empty()) {
		prefix = name;
	}

	string data_ptr = ctx.get_value(string("@MAP@") + name);
	if (data_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data = (string_map*)from_string<void*>(data_ptr);

	ctx.enter();
	for (auto it = data->begin(); it != data->end(); ++it) {
		ctx.add_value(prefix + std::string("_") + it->first, it->second);
	}
	try {
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	} catch (...) {
		ctx.leave();
		throw;
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
	std::string buf = s.str();

	if (buf.size() == 0) {
		// silently skip the empty json
		return;
	}

	Json::Value root;
	Json::Reader reader;
	Json::FastWriter writer;
	if (!reader.parse(buf, root)) {
		throw tag_map_exception(
		  (format(_("invalid JSON structure '{0}'")) % buf).str());
	}

	if (root.isObject()) {
		data->clear();
		// parse json object members
		Json::Value::Members members = root.getMemberNames();
		for (Json::Value::Members::const_iterator i = members.begin(); i != members.end(); ++i) {
			if (root[*i].type() == Json::nullValue) {
				data->insert({*i, std::string()});
			} else if (root[*i].isConvertibleTo(Json::ValueType::stringValue)) {
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

	string data_type_ptr = ctx.get_value(string("@MAP_TYPES@") + name);
	if (data_type_ptr.empty()) {
		throw tag_map_exception(
		  (format(_("map '{0}' was not defined in the current context")) % name).str());
	}
	string_map *data_types = (string_map*)from_string<void*>(data_type_ptr);

	Json::Value root;
	for (auto it = data->begin(); it != data->end(); ++it) {
		Json::Value value;
		if (data_types->find(it->first) != data_types->end()) {
			const string &type = (*data_types)[it->first];
			if (type == "integer") {
				value = Json::Value(std::stoi(it->second));
			} else if (type == "number") {
				value = Json::Value(std::stod(it->second));
			} else if (type == "boolean") {
				value = Json::Value(it->second == "true");
			} else if (type == "object") {
				Json::Value object;
				Json::Reader reader;
				if (!reader.parse(it->second, object)) {
					throw tag_map_exception(
					  (format(_("invalid JSON structure '{0}'")) % it->second).str());
				}
				value = object;
			} else {
				value = Json::Value(it->second);
			}
		} else {
			value = Json::Value(it->second);
		}
		root[it->first] = value;
	}

	Json::FastWriter writer;
	out << writer.write(root);
}

} // namespace
