/*
 * tag_connection.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _TAG_connection_H_
#define _TAG_connection_H_

#include <string>
#include <ostream>

#include <dcl/dclbase.h>

#include <dbpager/tag_impl.h>

#include "redis_connection.h"

namespace dbpager {

class tag_connection_exception: public dbp::exception {
public:
	tag_connection_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_connection: public tag_impl {
public:
	int database_number;
	std::string server, password;
	tag_connection(const std::string &tag_name = ""): database_number(0), tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}

#endif /*_TAG_connection_H_*/

