/*
 * context.cpp
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

#include <algorithm>

#include <dbpager/context.h>

namespace dbpager {

using namespace std;
using namespace dbp;

context::~context() {
	while (!stack.empty()) {
		delete stack.back();
		stack.pop_back();
	}
};

void context::enter() {
	stack.push_back(new variables());
}

void context::leave() {
	delete stack.back();
	stack.pop_back();
}

std::string context::get_value(const std::string &name) const {
	string upname = name;
	transform(upname.begin(), upname.end(), upname.begin(), ::toupper);
	for (vars_stack::const_reverse_iterator i = stack.rbegin();
	  i < stack.rend(); ++i) {
		variables::const_iterator j = (*i)->find(upname);
		if (j != (*i)->end())
			return j->second;
	}
	if (_parent)
		return _parent->get_value(name);
	else
		return "";
}

context::variables context::get_values() const {
	variables rslt;
	for (vars_stack::const_iterator i = stack.begin();
	  i < stack.end(); ++i)
		for (variables::const_iterator j = (*i)->begin();
		  j != (*i)->end(); ++j)
		  	rslt[j->first] = j->second;
	return rslt;
}

void context::set_value(const std::string &name, const std::string &value) {
	string upname = name;
	transform(upname.begin(), upname.end(), upname.begin(), ::toupper);
	for (vars_stack::reverse_iterator i = stack.rbegin();
	  i < stack.rend(); ++i) {
		variables::iterator j = (*i)->find(upname);
		if (j != (*i)->end()) {
			j->second = value;
			return;
		}
	}
	if (_parent)
		_parent->set_value(name, value);
	else
		throw context_exception(
		  (format(_("variable {0} is not defined")) % name).str());
}

void context::add_value(const std::string &name, const std::string &value,
  const std::string &type) {
	string upname = name;
	transform(upname.begin(), upname.end(), upname.begin(), ::toupper);
	string vtype = type.empty() ? _type : type;
	if (compare()(vtype, _type))
		(*stack.back())[upname] = value;
	else if (_parent)
		_parent->add_value(name, value, vtype);
	else
		throw context_exception(
		  (format(_("unknown variable type {0}")) % type).str());
}

bool context::empty() {
	if (stack.empty())
		return true;
	return (*stack.back()).empty();
}

void context::push(const tag *t) {
	exec.push_back(t);
}

const tag* context::pop() {
	if (exec.empty())
		return NULL;
	const tag* rslt = exec.back();
	exec.pop_back();
	return rslt;
}

}

