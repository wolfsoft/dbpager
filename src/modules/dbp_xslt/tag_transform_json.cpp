/*
 * tag_transform_json.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2020 - Dennis Prochko <wolfsoft@mail.ru>
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
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>

#include <dcl/strutils.h>

#include "tag_transform_json.h"

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace Json;

void tag_transform_json::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	// initialize stylesheet file
	string href = get_parameter(ctx, "href");
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc = NULL, res = NULL;
	xmlChar *buf = NULL;
	cur = xsltParseStylesheetFile((const xmlChar*)href.c_str());
	if (!cur) {
		throw tag_transform_json_exception(
		  (format(_("can't parse stylesheet file '{0}'")) % href).str());
	}

	// initialize JSON source
	stringstream s;
	tag_impl::execute(ctx, s, caller);
	Json::Value root;
	Json::Reader reader;
	s.seekg(0, s.beg);
	reader.parse(s, root);

	doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr n = xmlNewNode(NULL, BAD_CAST "root");
	write_value(n, root);
	xmlDocSetRootElement(doc, n);

	try {
		// extract other parameters
		std::vector<std::string> pstr;
		for (const auto& p : params) {
			if (p.first == "href") continue;
			pstr.push_back(p.first);

			// escape the value with single quotes
			std::stringstream val;
			p.second->execute(ctx, val, this);
			std::string value = val.str();
			if (!value.empty() && value.front() != '\'' && value.back() != '\'') {
				std::string escaped_value = "";
				for (char c : value) {
					if (c == '\'')
						escaped_value += "''";
					else
						escaped_value += c;
				}
				pstr.push_back("'" + escaped_value + "'");
			} else {
				pstr.push_back(value);
			}
		}

		std::vector<const char*> pchr;
		for (const auto& s : pstr) {
			pchr.push_back(s.c_str());
		}
		pchr.push_back(nullptr);

		// transform file
		res = xsltApplyStylesheet(cur, doc, pchr.data());
		if (!res) {
			throw tag_transform_json_exception(
			  _("can't transform JSON data"));
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

void tag_transform_json::write_value(xmlNodePtr n, Json::Value &value) const {
	switch (value.type()) {
		case nullValue:
			break;
		case booleanValue:
		case intValue:
		case uintValue:
		case realValue:
		case stringValue: {
			std::string v = value.asString();
			xmlNodeSetContent(n, (const xmlChar*)(v.c_str()));
			break;
		}
		case arrayValue: {
			ArrayIndex size = value.size();
			for (ArrayIndex index = 0; index < size; ++index) {
				xmlNodePtr c = xmlNewChild(n, NULL, BAD_CAST "element", NULL);
				write_value(c, value[index]);
			}
			break;
		}
		case objectValue: {
			Value::Members members(value.getMemberNames());
			for (auto it = members.begin(); it != members.end(); ++it) {
				xmlNodePtr c = xmlNewChild(n, NULL, (const xmlChar*)it->c_str(), NULL);
				write_value(c, value[*it]);
			}
			break;
		}
	}
}

} // namespace
