/*
 * tag_transform.cpp
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

#include <sstream>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>

#include <dcl/strutils.h>

#include "tag_transform.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_transform::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	// initialize stylesheet file
	string href = get_parameter(ctx, "href");
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc = NULL, res = NULL;
	xmlChar *buf = NULL;
	cur = xsltParseStylesheetFile((const xmlChar*)href.c_str());
	if (!cur) {
		throw tag_transform_exception(
		  (format(_("can't parse stylesheet file '{0}'")) % href).str());
	}
	// initialize XML source
	stringstream s;
	tag_impl::execute(ctx, s, caller);
	doc = xmlParseMemory(s.str().c_str(), s.str().length());
	if (!doc) {
		throw tag_transform_exception(
		  _("the input stream does not contain the valid XML data"));
	}
	try {
		// transform file
		res = xsltApplyStylesheet(cur, doc, NULL);
		if (!res) {
			throw tag_transform_exception(
			  _("can't transform XML data"));
		}
		// obtain the result
		int buf_len;
		xsltSaveResultToString(&buf, &buf_len, res, cur);
		out.write((const char*)buf, buf_len);
		// cleaning up
		xmlFree(buf);
		xsltFreeStylesheet(cur);
		xmlFreeDoc(res);
		xmlFreeDoc(doc);
	}
	catch (...) {
		// cleaning up
		if (buf) xmlFree(buf);
		if (cur) xsltFreeStylesheet(cur);
		if (res) xmlFreeDoc(res);
		if (doc) xmlFreeDoc(doc);
		throw;
	}
}

} // namespace

