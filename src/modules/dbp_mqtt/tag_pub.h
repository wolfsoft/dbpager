/*
 * tag_pub.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2016 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _TAG_PUB_H_
#define _TAG_PUB_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/tag_impl.h>

namespace dbpager {

class tag_pub_exception: public dbp::exception {
public:
	tag_pub_exception(const std::string &msg): dbp::exception(msg) { }
};

/*

<mqtt:connection>
	<mqtt:pub topic="/the/topic/path" qos="0|1|2" retain="false">payload</mqtt:pub>
</mqtt:connection>

*/
class tag_pub: public tag_impl {
public:
	tag_pub(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}

#endif /*_TAG_PUB_H_*/
