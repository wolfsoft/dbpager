/*
 * tag_map.h
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

#ifndef TAG_MAP_H_
#define TAG_MAP_H_

#include <string>
#include <ostream>

#include <dbpager/tag_impl.h>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

namespace dbpager {

class tag_map_exception: public dbp::exception {
public:
	tag_map_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_map: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_set: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_set(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_unset: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_unset(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_element: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_element(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_elements: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_elements(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_from_json: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_from_json(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_map_to_json: public tag_impl {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> string_map;
#else
	typedef std::tr1::unordered_map<std::string, std::string> string_map;
#endif
	tag_map_to_json(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}

#endif /*TAG_MAP_H_*/
