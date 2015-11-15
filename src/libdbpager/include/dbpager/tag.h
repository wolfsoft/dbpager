/*
 * tag.h
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

#ifndef _TAG_H_
#define _TAG_H_

#include <string>
#include <ostream>

#include <dcl/delegate.h>
#include <dcl/plugin.h>

namespace dbpager {

class context;

class tag: public dbp::disposable {
public:
	//! Custom data
	void* data;
	tag(): data(NULL) { };
	//! Visitor callback function
	typedef dbp::delegate1<tag*, void> visitor_handler;
	//! Destructor
	virtual ~tag() { };
	//! Set a relationship
	virtual void set_parent(const tag *parent) = 0;
	//! Get a relationship
	virtual tag* get_parent() const = 0;
	//! Set a tree depth
	virtual void set_depth(int depth) = 0;
	//! Get a tree depth
	virtual int get_depth() = 0;
	//! Set a name
	virtual void set_name(const std::string &name) = 0;
	//! Get a name
	virtual std::string& get_name() const = 0;
	//! Adds given node as a child
	virtual void add_child(tag *node) = 0;
	//! Returns a first child by name or NULL if not found
	virtual tag* get_child(const std::string &name) const = 0;
	//! Appends all own childs to the given node and removes it from own list
	virtual void move_childs_to(tag *dest) = 0;
	//! Adds a parameter
	virtual void add_parameter(const std::string &name, tag *value) = 0;
	//! Returns the calculated value of given parameter
	virtual std::string get_parameter(context &ctx,
	  const std::string &name) const = 0;
	//! Sets a node text
	virtual void set_text(const std::string &value) = 0;
	//! Returns a node text
	virtual const std::string& get_text() const = 0;
	//! Execute current tag
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const = 0;
	//! Call the visitor
	virtual void apply(context &ctx, visitor_handler handler) = 0;
};

}

#endif /*_TAG_H_*/

