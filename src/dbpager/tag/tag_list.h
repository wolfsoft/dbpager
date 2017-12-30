/*
 * tag_list.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko
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

#ifndef TAG_LIST_H_
#define TAG_LIST_H_

#include <vector>
#include <string>
#include <ostream>

#include <dbpager/tag_impl.h>

namespace dbpager {

class tag_list_exception: public dbp::exception {
public:
	tag_list_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_list: public tag_impl {
public:
	tag_list(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_list_add: public tag_impl {
public:
	tag_list_add(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_list_remove: public tag_impl {
public:
	tag_list_remove(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_list_element: public tag_impl {
public:
	tag_list_element(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class tag_list_elements: public tag_impl {
public:
	tag_list_elements(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}

#endif /*TAG_LIST_H_*/
