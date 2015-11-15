/*
 * tag_transform.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _TAG_TRANSFORM_H_
#define _TAG_TRANSFORM_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/tag_impl.h>

namespace dbpager {

class tag_transform_exception: public dbp::exception {
public:
	tag_transform_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_transform: public tag_impl {
public:
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
private:
};

}

#endif /*_TAG_TRANSFORM_H_*/
