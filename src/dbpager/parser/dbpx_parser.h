/*
 * dbpx_parser.h
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

#ifndef _DBPX_PARSER_H_
#define _DBPX_PARSER_H_

#include <string>
#include <libxml/xmlreader.h>

#include <dbpager/tag.h>

#include "parser/parser.h"

namespace dbpager {

//!	dbPager XML format scripts parser class.
/*!
	The dbpx_parser class is a parser for scripts in new XML-based dbpager
	scripts.
*/
class dbpx_parser: public parser {
public:
	dbpx_parser(const dbp::url &url): _url(url) { };
	virtual ~dbpx_parser() { };
	virtual tag* parse() const;
private:
	dbp::url _url;
	std::string error_message;
	tag* process_node(const dbp::url &current_url, xmlTextReaderPtr reader,
	  tag *current_node) const;
	void process_attributes(xmlTextReaderPtr reader, tag &node) const;
	// add parent-child relationship beetween current and created tag
	void make_relationship(xmlTextReaderPtr reader, tag *current_node, tag *rslt) const;
	static void error_callback(void *arg, const char *msg,
	  xmlParserSeverities severity, xmlTextReaderLocatorPtr locator);
};

}

#endif /*_DBPX_PARSER_H_*/

