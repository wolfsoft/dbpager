/*
 * queue.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2016 - Dennis Prochko
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

#ifndef _queue_H_
#define _queue_H_

#include <string>
#include <ostream>

#include <dcl/delegate.h>
#include <dcl/plugin.h>

namespace dbpager {

class queue: public dbp::disposable {
public:
	virtual ~queue() { };
	virtual std::string pop(const std::string &queue) = 0;
	virtual void push(const std::string &queue, const std::string &value) = 0;
};

}

#endif /*_queue_H_*/

