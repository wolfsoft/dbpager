/*
 * tag_impl.cpp
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

#include <dcl/strutils.h>

#include <dbpager/tag_impl.h>

namespace dbpager {

using namespace std;
using namespace dbp;

tag_impl::~tag_impl() {
	// Delete all parameter nodes
	for (parameters::const_iterator i = params.begin();
	  i != params.end(); ++i) {
		delete i->second;
	}
	// Delete all child nodes
	for (childrens::reverse_iterator i = childs.rbegin();
	  i != childs.rend(); ++i) {
		delete *i;
	}
}

void tag_impl::set_text(const std::string &value) {
	std::vector<std::string> s = tokenize()(value, "\n");
	_text.clear();
	for (std::vector<std::string>::const_iterator i = s.begin();
	  i != s.end(); ++i) {
		_text += trim()(*i);
	}
}

const std::string& tag_impl::get_text() const {
	return _text;
}

void tag_impl::execute(context &ctx, std::ostream &out, const tag *caller) const {
	for (childrens::const_iterator i = childs.begin();
	  i != childs.end(); ++i)
		(*i)->execute(ctx, out, caller);
	out << _text;
}

void tag_impl::apply(context &ctx, tag::visitor_handler visitor) {
	if (visitor) {
		for (childrens::iterator i = childs.begin();
		  i != childs.end(); ++i)
			(*i)->apply(ctx, visitor);
		visitor(this);
	}
}

void tag_impl::add_child(tag *node) {
	node->set_parent(this);
	childs.push_back(node);
}

void tag_impl::add_parameter(const std::string &name, tag *value) {
	params.insert(std::make_pair(name, value));
}

tag* tag_impl::get_child(const std::string &name) const {
	for (childrens::const_iterator i = childs.begin(); i != childs.end(); ++i)
		if ((*i)->get_name() == name)
			return *i;
	return NULL;
}

void tag_impl::move_childs_to(tag *dest) {
	for (childrens::iterator i = childs.begin(); i != childs.end(); ++i) {
		dest->add_child(*i);
	}
	childs.clear();
}

std::string tag_impl::get_parameter(context &ctx, const std::string &name) const {
	parameters::const_iterator i = params.find(name);
	if (i != params.end()) {
		ostringstream s;
		(i->second)->execute(ctx, s, this);
		return s.str();
	} else
		return "";
}

} // namespace

