/*
 * tag_execute.h
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

#ifndef TAG_EXECUTE_H_
#define TAG_EXECUTE_H_

#include <string>
#include <ostream>

#include <dbpager/tag_impl.h>

namespace dbpager {

class tag_execute: public tag_impl {
public:
	tag_execute(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}

#endif /*TAG_EXECUTE_H_*/
