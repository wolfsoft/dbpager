/*
 * tag_factory.h
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
 
#ifndef _TAG_FACTORY_H_
#define _TAG_FACTORY_H_

#include "config.h"

#include <string>
#include <vector>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <dcl/singleton.h>
#include <dcl/shared_ptr.h>
#include <dcl/plugin.h>
#include <dcl/factory.h>

#include <dbpager/tag.h>

namespace dbpager {

class tag_factory: public dbp::singleton<tag_factory> {
	friend class dbp::singleton<tag_factory>;
public:
	virtual ~tag_factory() {
		_factories.clear();
		_plugins.clear();
	};
	//! Register custom tags object factory plugin
	void register_plugin(const std::string &namespace_uri,
	  dbp::shared_ptr<dbp::plugin> plugin);
	//! Register custom tag
	template <class T>
	void register_tag(const std::string &namespace_uri, const std::string &name);
	//! Create the specified tag object
	tag* create(const std::string &namespace_uri,
	  const std::string &namespace_prefix,
	  const std::string &object);
private:
	tag_factory();
	tag_factory(const tag_factory&) { };
private:
	typedef std::vector<dbp::shared_ptr<dbp::plugin> > plugins_list;
#ifdef HAVE_CXX11
	std::unordered_map<std::string, plugins_list> _plugins;
#else
	std::tr1::unordered_map<std::string, plugins_list> _plugins;
#endif
	typedef dbp::factory<tag> factory;
#ifdef HAVE_CXX11
	std::unordered_map<std::string, factory> _factories;
#else
	std::tr1::unordered_map<std::string, factory> _factories;
#endif
};

} // namespace

#endif /*_TAG_FACTORY_H_*/
