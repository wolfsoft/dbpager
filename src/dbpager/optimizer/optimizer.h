/*
 * optimizer.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko
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

#ifndef _OPTIMIZER_H_
#define _OPTIMIZER_H_

#include <string>
#include <list>

#include <dcl/exception.h>

#include <dbpager/tag.h>

namespace dbpager {

class optimizer_exception: public dbp::exception {
public:
	optimizer_exception(const std::string &msg): dbp::exception(msg) { }
};

//!	Generic optimizer class.
/*!
	The generic optimizer class is a basic optimizer class for all kinds of
	script optimizers.
*/
class optimizer {
public:
	//!	Optimize the given tag tree.
	tag* optimize(tag *tree);
private:
	/* removes non-used nodes */
	void optimize_orphans(tag *tag);
	/* removes no needed expressions and enable value caching */
	void optimize_constants(tag *tag);

	std::list<std::string> funcs;
};

}

#endif /*_OPTIMIZER_H_*/

