/*
 * environment.h
 * This file is part of dbPager Server
 *
 * Copyright (c) 2009 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include <dcl/dclbase.h>
#include <dcl/dclnet.h>

#include <dbpager/context.h>

namespace dbpager {

class interpreter;

class environment: public dbp::noncopyable {
public:
	//! Constructor; initializes the environment
	explicit environment(dbpager::interpreter&);
	//! Destructor; cleans up the resources
	virtual ~environment();
	//! Obtain the environment's context (variables initialized)
	context* get_context();
	//! Initializes the environment parameters
	virtual void init_custom_params() = 0;
protected:
	context *global, *user, *session;
	dbpager::interpreter &in;
};

class http_environment: public environment {
public:
	//! Constructor; initializes the environment
	explicit http_environment(dbpager::interpreter&, const dbp::http_request&);
	//! Initializes the environment parameters
	virtual void init_custom_params();
	//! Initialize response with environment parameters
	void init_response(dbp::http_response&);
	//! Maps uri to application main file, i.e. acts as a request router
	std::string get_path();
private:
	bool is_session_new;
	const dbp::http_request &req;
	std::string session_id;

	void parse_urlencoded(context&, const dbp::http_request&);
	void parse_data(context&, const dbp::http_request&);
	void parse_plain(context&, const dbp::http_request&);
	void parse_json(context&, const dbp::http_request&);
};

} // namespace

#endif /*_ENVIRONMENT_H_*/

