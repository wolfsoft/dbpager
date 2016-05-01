/*
 * dbpx_parser.cpp
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

#include <dcl/strutils.h>

#include <dbpager/consts.h>
#include <dbpager/context.h>
#include <dbpager/tag_usr.h>

#include "parser/dbpx_parser.h"
#include "optimizer/optimizer.h"
#include "tag/tag_factory.h"
#include "tag/tag_unknown.h"
#include "tag/tag_expression.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void dbpx_parser::error_callback(void *arg, const char *msg,
  xmlParserSeverities severity, xmlTextReaderLocatorPtr locator) {
  	string message;
	switch (severity) {
		case XML_PARSER_SEVERITY_VALIDITY_WARNING:
			message = _("Validity warning at line {0}: {1}");
		case XML_PARSER_SEVERITY_VALIDITY_ERROR:
			message = _("Validity error at line {0}: {1}");
		case XML_PARSER_SEVERITY_WARNING:
			message = _("Warning at line {0}: {1}");
		case XML_PARSER_SEVERITY_ERROR:
			message = _("Error at line {0}: {1}");
	}
	static_cast<dbpx_parser*>(arg)->error_message =
		(format(message) % xmlTextReaderLocatorLineNumber(locator) % msg).str();
}

tag* dbpx_parser::parse() const {
	// parse the file
	xmlTextReaderPtr reader = xmlReaderForFile(_url.str().c_str(), NULL,
	  XML_PARSE_NOENT | XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR);
	// if can't - raise an exception
	if (!reader) {
		throw parser_exception(1, (format(_("unable to open file '{0}'. {1}")) %
		  _url.str() % error_message).str());
	}
	else
	try {
		// register custom error handler
		xmlTextReaderSetErrorHandler(reader, error_callback, (void*)this);
		// read one more tag
		int ret = xmlTextReaderRead(reader);
		tag *node = NULL, *root_node = NULL, *current_node = NULL;
		while (ret == 1) {
			// if a root node detected, save a pointer for future
			if (!root_node) {
				node = process_node(_url, reader, current_node);
				if (node) {
					root_node = node;
					current_node = root_node;
				}
			} else {
				// create tag object from node found
				node = process_node(_url, reader, current_node);
				if (node)
					current_node = node;
			}
			// read one more tag
			ret = xmlTextReaderRead(reader);
		}
		// raise an exception if something wrong
		if (ret != 0) {
			throw parser_exception(0,
			  (format(_("failed to parse file '{0}'. {1}")) % _url.str() %
			  error_message).str());
		}
		// cleanup parser
		xmlFreeTextReader(reader);
		// optimize a result tree and return pointer to a root tag
		return optimizer().optimize(root_node);
	}
	catch (...) {
		// cleanup parser
		xmlFreeTextReader(reader);
		throw;
	}
}

void dbpx_parser::make_relationship(xmlTextReaderPtr reader, tag *current_node,
  tag *rslt) const {
	if (current_node && rslt) {
		if (rslt->get_depth() == current_node->get_depth()) {
			if (!current_node->get_parent())
				throw parser_exception(0,
				  (format(_("invalid nested tags ({0})")) %
				  (const char*)xmlTextReaderName(reader)).str());
			current_node->get_parent()->add_child(rslt);
		} else
		if (rslt->get_depth() > current_node->get_depth()) {
			current_node->add_child(rslt);
		} else
		if (rslt->get_depth() < current_node->get_depth()) {
			tag *p;
			p = current_node->get_parent();
			for (int i = 0;
			  i < (current_node->get_depth() - rslt->get_depth()); ++i) {
				p = p->get_parent();
				if (!p)
					throw parser_exception(0,
					  (format(_("invalid nested tags ({0})")) %
					  (const char*)xmlTextReaderName(reader)).str());
			}
			p->add_child(rslt);
		}
	}
}

tag* dbpx_parser::process_node(const url &current_url, xmlTextReaderPtr reader,
  tag *current_node) const {
	tag *rslt;
	tag_factory &_factory = tag_factory::instance();
	// if node type is a tag, try to create a tag object
	switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_ELEMENT:
			{
				string object = to_string<const char*>((const char*)xmlTextReaderConstLocalName(reader));
				string namespace_uri = to_string<const char*>((const char*)xmlTextReaderConstNamespaceUri(reader));
				string namespace_prefix = to_string<const char*>((const char*)xmlTextReaderConstPrefix(reader));
			
				if (namespace_uri.compare(0, dbpager_custom_uri.length(), dbpager_custom_uri) == 0) {
					// custom tag processing
					rslt = new tag_usr(object);
				} else if (namespace_uri.compare(0, dbpager_uri.length(), dbpager_uri) == 0) {
					// create tag with tag factory
					rslt = _factory.create(namespace_uri, namespace_prefix, object);
				} else if (namespace_uri == its_uri) {
					return NULL;
				} else {
					// html tags processing
					rslt = new tag_unknown(to_string<const char*>((const char*)xmlTextReaderConstName(reader)));
				}
				if (!rslt)
					throw parser_exception(0, (dbp::format(_("unknown tag found ({0})")) % object).str());
			}

			rslt->set_depth(xmlTextReaderDepth(reader));
			// fill tag parameters
			process_attributes(reader, *rslt);
			// replace include tag by included script root tag
			if (rslt->get_name() == "include") {
				// extract base path from the current URL
				string path, file;
				tokenize()(current_url.path, path, file, "/");
				// extract URL from a context
				context ctx(NULL);
				const string &href = rslt->get_parameter(ctx, "href");
				if (href.empty())
					throw parser_exception(0,
					  _("href parameter missing for tag include"));
				url u(href);
				// copy missing URL parts from possible relative u
				url cur = current_url;
				cur.copy_from(u);
				// normalize destination URL
				cur.normalize(path + "/");
				// load the URL content and parse it
				tag *r = dbpx_parser(cur).parse();
				r->move_childs_to(current_node);
				delete rslt;
				delete r;
				return current_node;
			}
			make_relationship(reader, current_node, rslt);
			if (typeid(*rslt) == typeid(tag_usr)) {
				static_cast<tag_usr*>(rslt)->link();
			}
			return rslt;
			break;
		case XML_READER_TYPE_END_ELEMENT:
			if (current_node->get_name() == "module") {
				return current_node;
			} else
				return current_node->get_parent();
			break;
		case XML_READER_TYPE_TEXT:
			{
				tag_expression *t = new tag_expression();
				t->set_text(to_string<const char*>(
				  (const char*)xmlTextReaderConstValue(reader)));
				t->set_depth(xmlTextReaderDepth(reader));
				make_relationship(reader, current_node, t);
				return t;
			}
			break;
		case XML_READER_TYPE_CDATA:
			{
				current_node->set_text(current_node->get_text() +
					to_string<const char*>(
					  (const char*)xmlTextReaderConstValue(reader)));
				return current_node;
			}
			break;
		default:
			return current_node;
			break;
	}
}

void dbpx_parser::process_attributes(xmlTextReaderPtr reader, tag &node) const {
	while (xmlTextReaderMoveToNextAttribute(reader)) {
		tag *param = new tag_expression();
		param->set_parent(&node);
		param->set_text(to_string<const char*>(
		  (const char*)xmlTextReaderConstValue(reader)));
		// only attrs without ns: prefix
		if (!xmlTextReaderConstPrefix(reader))
			node.add_parameter(to_string<const char*>(
			  (const char*)xmlTextReaderConstName(reader)), param);
	}
}

} // namespace
