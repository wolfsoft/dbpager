/*
 * tag_database.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2012 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _TAG_DATABASE_H_
#define _TAG_DATABASE_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>
#include <dcl/singleton.h>

#include <dbpager/tag_impl.h>

#include "client/dbclient.h"

namespace dbpager {
namespace mongodb {

using namespace mongo;

class database: public DBClientConnection, public dbp::singleton<database> {
	friend class dbp::singleton<database>;
};

class tag_database_exception: public dbp::exception {
public:
	tag_database_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_database: public tag_impl {
public:
	tag_database(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}} //namespace

#endif /*_TAG_DATABASE_H_*/

