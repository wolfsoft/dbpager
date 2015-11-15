/*
 * tag_impl.h
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

#ifndef _TAG_IMPL_H_
#define _TAG_IMPL_H_

#include "config.h"

#include <string>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <vector>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/tag.h>
#include <dbpager/context.h>

namespace dbpager {

class tag_exception: public dbp::exception {
public:
	tag_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_impl: public tag {
public:
	//! Constructor
	tag_impl(const std::string &tag_name = ""): name(tag_name), depth(0),
	  parent(NULL) { };
	//! Destructor
	virtual ~tag_impl();
	virtual void set_parent(const tag *tag_parent) {
		parent = const_cast<tag*>(tag_parent);
		depth = parent->get_depth() + 1;
	};
	virtual tag* get_parent() const {
		return parent;
	};
	virtual void set_name(const std::string &tag_name) {
		name = tag_name;
	};
	virtual std::string& get_name() const {
		return const_cast<std::string&>(name);
	};
	virtual void set_depth(int tag_depth) {
		depth = tag_depth;
	};
	virtual int get_depth() {
		return depth;
	};
	virtual void add_child(tag *node);
	virtual void move_childs_to(tag *dest);
	virtual void add_parameter(const std::string &name, tag *value);
	virtual std::string get_parameter(context &ctx, const std::string &name) const;
	virtual tag* get_child(const std::string &name) const;
	virtual void set_text(const std::string &value);
	virtual const std::string& get_text() const;
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
	virtual void apply(context &ctx, tag::visitor_handler handler);
protected:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, tag*> parameters;
#else
	typedef std::tr1::unordered_map<std::string, tag*> parameters;
#endif
	//! Tag parameters, in form name = expression
	parameters params;
	//! Tag name
	std::string name;
	//! Tag depth level
	int depth;
	//! Parent tag
	tag *parent;
private:
	typedef std::vector<tag*> childrens;
	//! Tag child nodes
	childrens childs;
	//! Node text
	std::string _text;
};

}

#endif /*_TAG_H_*/

