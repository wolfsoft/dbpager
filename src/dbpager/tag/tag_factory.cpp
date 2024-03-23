/*
 * tag_factory.cpp
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

#include <dcl/plugin.h>

#include <dbpager/tag_impl.h>
#include <dbpager/tag_tag.h>
#include <dbpager/tag_usr.h>
#include <dbpager/consts.h>

#include "tag/tag_factory.h"
#include "parser/parser.h"

#include "tag/tag_var.h"
#include "tag/tag_map.h"
#include "tag/tag_list.h"
#include "tag/tag_set.h"
#include "tag/tag_try.h"
#include "tag/tag_catch.h"
#include "tag/tag_throw.h"
#include "tag/tag_case.h"
#include "tag/tag_switch.h"
#include "tag/tag_while.h"
#include "tag/tag_split.h"
#include "tag/tag_substr.h"
#include "tag/tag_regexp.h"
#include "tag/tag_call.h"
#include "tag/tag_execute.h"
#include "tag/tag_read.h"
#include "tag/tag_write.h"
#include "tag/tag_http.h"
#include "tag/tag_locale.h"
#include "tag/tag_system.h"
#include "tag/tag_delete.h"
#include "tag/tag_rename.h"
#include "tag/tag_log.h"
#include "tag/functions.h"

namespace dbpager {

using namespace std;
using namespace dbp;

tag_factory::tag_factory() {
	// register builtin classes
	register_tag<tag_impl>(dbpager_uri, "module");
	register_tag<tag_impl>(dbpager_uri, "include");
	register_tag<tag_var>(dbpager_uri, "var");
	register_tag<tag_set>(dbpager_uri, "set");
	register_tag<tag_map>(dbpager_uri, "map");
	register_tag<tag_map_set>(dbpager_uri, "map-set");
	register_tag<tag_map_unset>(dbpager_uri, "map-unset");
	register_tag<tag_map_element>(dbpager_uri, "map-element");
	register_tag<tag_map_elements>(dbpager_uri, "map-elements");
	register_tag<tag_map_from_json>(dbpager_uri, "map-from-json");
	register_tag<tag_map_to_json>(dbpager_uri, "map-to-json");
	register_tag<tag_list>(dbpager_uri, "list");
	register_tag<tag_list_add>(dbpager_uri, "list-add");
	register_tag<tag_list_remove>(dbpager_uri, "list-remove");
	register_tag<tag_list_element>(dbpager_uri, "list-element");
	register_tag<tag_list_elements>(dbpager_uri, "list-elements");
	register_tag<tag_try>(dbpager_uri, "try");
	register_tag<tag_catch>(dbpager_uri, "catch");
	register_tag<tag_throw>(dbpager_uri, "throw");
	register_tag<tag_case>(dbpager_uri, "case");
	register_tag<tag_switch>(dbpager_uri, "switch");
	register_tag<tag_while>(dbpager_uri, "while");
	register_tag<tag_split>(dbpager_uri, "split");
	register_tag<tag_substr>(dbpager_uri, "substr");
	register_tag<tag_regexp>(dbpager_uri, "regexp");
	register_tag<tag_tag>(dbpager_uri, "tag");
	register_tag<tag_call>(dbpager_uri, "call");
	register_tag<tag_execute>(dbpager_uri, "execute");
	register_tag<tag_read>(dbpager_uri, "read");
	register_tag<tag_write>(dbpager_uri, "write");
	register_tag<tag_locale>(dbpager_uri, "locale");
	register_tag<tag_system>(dbpager_uri, "system");
	register_tag<tag_delete>(dbpager_uri, "delete");
	register_tag<tag_rename>(dbpager_uri, "rename");
	register_tag<tag_log>(dbpager_uri, "log");
	register_tag<tag_http>(dbpager_uri, "http");
	// register builtin functions
	register_tag<function_iif>(dbpager_uri, "@iif");
	register_tag<function_byte>(dbpager_uri, "@byte");
	register_tag<function_length>(dbpager_uri, "@len");
	register_tag<function_concatenate>(dbpager_uri, "@concat");
	register_tag<function_pos>(dbpager_uri, "@pos");
	register_tag<function_encode_base64>(dbpager_uri, "@encode_base64");
	register_tag<function_decode_base64>(dbpager_uri, "@decode_base64");
	register_tag<function_md5>(dbpager_uri, "@md5");
	register_tag<function_sha256>(dbpager_uri, "@sha256");
	register_tag<function_crypt>(dbpager_uri, "@crypt");
	register_tag<function_uuid>(dbpager_uri, "@uuid");
	register_tag<function_rnd>(dbpager_uri, "@rnd");
	register_tag<function_upper>(dbpager_uri, "@upper");
	register_tag<function_lower>(dbpager_uri, "@lower");
	register_tag<function_urlencode>(dbpager_uri, "@encode_url");
	register_tag<function_urldecode>(dbpager_uri, "@decode_url");
	register_tag<function_date_utc>(dbpager_uri, "@date_utc");
	register_tag<function_date_from>(dbpager_uri, "@date_from");
	register_tag<function_date_fmt>(dbpager_uri, "@date_fmt");
	register_tag<function_env>(dbpager_uri, "@env");
	register_tag<function_translate>(dbpager_uri, "@_");
};

void tag_factory::register_plugin(const std::string &namespace_uri,
  dbp::shared_ptr<dbp::plugin> plugin) {
	_plugins[namespace_uri].push_back(plugin);
}

template <class T>
void tag_factory::register_tag(const std::string &namespace_uri,
  const std::string &name) {
	_factories[namespace_uri].register_class<T>(name);
}

tag* tag_factory::create(const std::string &namespace_uri,
  const std::string &namespace_prefix, const std::string &object) {
	tag *t = NULL;
	// search in the builtin tags
	if (_factories.find(namespace_uri) != _factories.end()) {
		if (_factories[namespace_uri].is_registered(object)) {
			t = _factories[namespace_uri].create(object);
		} else
			throw parser_exception(0, (dbp::format(_("unknown tag found ({0})")) % object).str());
	} else {
		// if is not a builtin tag, search in plugins
		if (_plugins.find(namespace_uri) != _plugins.end()) {
			for (plugins_list::const_iterator i = _plugins[namespace_uri].begin();
			  i != _plugins[namespace_uri].end(); ++i) {
				t = static_cast<tag*>((*i)->create_object(object));
				if (t)
					break;
			}
			if (!t) {
				throw parser_exception(0, (dbp::format(_("unknown tag found ({0})")) % object).str());
			}
		}
	}
	t->set_name(object);
	return t;
}

} // namespace
