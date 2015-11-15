/*
 * parser.h
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

#ifndef _PARSER_H_
#define _PARSER_H_

#include <string>

#include <dcl/exception.h>
#include <dcl/url.h>

#include <dbpager/tag.h>

namespace dbpager {

class parser_exception: public dbp::exception {
public:
	int code;
	parser_exception(int err_code, const std::string &msg):
	  dbp::exception(msg), code(err_code) { }
};

//!	Generic parser class.
/*!
	The generic parser class is a basic parser class for all kinds of
	script parsers.
*/
class parser {
public:
	virtual ~parser() { };
	//!	Parse the given file.
	/*!
		This function should be called, if object are constructed with
		default constructor, to parse of the script file.

		\param %url a URL to the script file to load and parse
		\return a pointer to a first (root) node
	*/
	virtual tag* parse() const = 0;
};

}

#endif /*_PARSER_H_*/

