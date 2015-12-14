/*
 * context.h
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

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "config.h"

#include <string>
#include <deque>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <dcl/strutils.h>
#include <dcl/exception.h>
#include <dcl/mutex.h>

#include <dbpager/services.h>
#include <dbpager/tag.h>

namespace dbpager {

class context_exception: public dbp::exception {
public:
	context_exception(const std::string &msg): dbp::exception(msg) { }
};

class context {
public:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> variables;
#else
	typedef std::tr1::unordered_map<std::string, std::string> variables;
#endif
	context(context *parent = NULL): _parent(parent), _services(NULL) {
		enter();
	}
	virtual ~context();
	virtual void enter();
	virtual void leave();
	virtual std::string get_value(const std::string &name) const;
	virtual variables get_values() const;
	virtual void set_value(const std::string &name, const std::string &value);
	virtual void add_value(const std::string &name, const std::string &value,
	  const std::string &type = "");
	virtual services* get_services_provider() const {
		return _services;
	}
	virtual void set_services_provider(services* services) {
		_services = services;
	}
	bool empty();
	void push(const tag *t);
	const tag* pop();
private:
	context() { };
	typedef std::deque<variables*> vars_stack;
	typedef std::deque<const tag*> call_stack;
	vars_stack stack;
	call_stack exec;
	context *_parent;
	services *_services;
protected:
	std::string _type;
};

class local_context: public context {
public:
	local_context(context *parent = NULL): context(parent) {
		_type = "local";
	};
};

class global_context: public context {
public:
	global_context(context *parent = NULL): context(parent) {
		_type = "global";
	};
};

class user_context: public context {
public:
	user_context(context *parent = NULL): context(parent) {
		_type = "user";
	};
};

}

#endif /*CONTEXT_H_*/

