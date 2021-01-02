/*
 * environment.cpp
 * This file is part of dbPager Server
 *
 * Copyright (c) 2009 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <algorithm>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <mimetic/mimetic.h>
#include <json/json.h>

#include <dcl/filefs.h>

#include <dbpager/session.h>

#include <interpreter/environment.h>
#include <interpreter/interpreter.h>

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace mimetic;

environment::environment(interpreter &in): global(NULL), user(NULL), session(NULL), in(in) {
	global = new global_context();
	user = new user_context(global);
	session = new local_context(user);

	// initialize possible http environment variables
	session->add_value("CONTENT_TYPE", "text/plain");
}

environment::~environment() {
	delete session;
	delete user;
	delete global;
}


http_environment::http_environment(interpreter &in, const dbp::http_request &r):
  environment(in), is_session_new(false), req(r) {

	const http_cookies &c = r.get_cookies();
	for (http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
		if (i->name == "session") {
			session_id = i->value;
			break;
		}
	}

	// get session cache instance
	if (session_id.empty()) {
		session_id = uuid().str();
		is_session_new = true;
	} else {
		const string &value = in.get_session().get(session_id);
		strings s = tokenize()(value);
		for (strings::const_iterator i = s.begin(); i != s.end(); ++i) {
			string k, v;
			tokenize()(*i, k, v, false, "=");
			if (!k.empty()) {
				user->add_value(k, v);
			}
		}
	}

	// add session id to variables
	session->add_value("SESSION", session_id);
	session->add_value("SESSION_PERSISTENT", "0");
}

void http_environment::init_response(dbp::http_response &resp) {
	// setup http response variables from environment variables
	resp.set_content_type(session->get_value("CONTENT_TYPE"));

	// no need any cookies on HEAD request
	if (req.get_method() == http_method::head)
		return;

	// set session cache instance
	if (!user->empty()) {
		string s;
		context::variables c = user->get_values();
		context::variables::const_iterator i = c.begin();
		while (i != c.end()) {
			s += i->first + "=" + i->second;
			++i;
			if (i != c.end())
				s += ";";
		}
		in.get_session().put(session_id, s);
	}

	// setup cookies
	if (is_session_new && !user->empty()) {
		http_cookies cs;
		http_cookie c("session", session_id);
		c.path = "/";
		c.http_only = true;
		if (req.get_https())
			c.secure = true;
		if (session->get_value("SESSION_PERSISTENT") == string("1")) {
			datetime d;
			d.year(2038).month(1).day(1).hour(0).minute(0).second(0);
			c.expires = d;
		}
		cs.push_back(c);
		resp.set_cookies(cs);
	}
	if (session->get_value("SESSION").empty()) {
		http_cookie c("session", "");
		c.path = "/";
		c.http_only = true;
		if (req.get_https())
			c.secure = true;
		datetime d;
		d.year(1976).month(4).day(21).hour(0).minute(0).second(0);
		c.expires = d;
		resp.set_cookie(c);
	}
};

context* environment::get_context() {
	return session;
}

void http_environment::parse_urlencoded(context &ctx,
  const dbp::http_request &req) {
	const char *value = req.get_content();
	if (value) {
		// split content data to parameters
		strings pairs = tokenize()(value, "&");
		for (strings::const_iterator i = pairs.begin();
		  i != pairs.end(); ++i) {
		  	// split each parameter to name and value
			string param, value;
			tokenize()(*i, param, value, false, "=");
			// add variable, decode urlencoded value
			ctx.add_value(url().decode(param),
			  url().decode(value));
		}
	}
}

void parse_mime(MimeEntity &me, context &ctx) {
	Header& h = me.header();
	if (h.contentDisposition().type() == "form-data") {
		ctx.add_value(h.contentDisposition().param("name"), me.body());
		ctx.add_value(h.contentDisposition().param("name") + "_size",
		  to_string<size_t>(me.body().length()));
		ctx.add_value(h.contentDisposition().param("name") + "_filename",
		  h.contentDisposition().param("filename"));
		ctx.add_value(h.contentDisposition().param("name") + "_content_type",
		  h.contentType().str());
	}
	MimeEntityList& parts = me.body().parts();
	for (MimeEntityList::iterator it = parts.begin();
	  it != parts.end(); ++it) {
		parse_mime(**it, ctx);
	}
}

void http_environment::parse_data(context &ctx, const dbp::http_request &req) {
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	s << "Content-Type: " << req.get_content_type() << CRLF << CRLF;
	s.write(req.get_content(), req.get_content_size());
	s.seekg(0);
	istreambuf_iterator<char> bit(s), eit;
	MimeEntity me(bit, eit);
	parse_mime(me, ctx);
}

void http_environment::parse_plain(context &ctx, const dbp::http_request &req) {
	const char *value = req.get_content();
	// add entire content as a variable
	if (value) {
		string s;
		s.assign(value, req.get_content_size());
		ctx.add_value("content", s);
	}
}

void http_environment::parse_json(context &ctx, const dbp::http_request &req) {
	const char *value = req.get_content();
	if (value) {
		Json::Value root;
		Json::Reader reader;
		Json::FastWriter writer;
		if (!reader.parse(value, root)) {
			throw dbp::exception(_("invalid JSON structure of the request"));
		}
		ctx.add_value("content", writer.write(root));
		if (root.isObject()) {
			// parse json object members
			Json::Value::Members members = root.getMemberNames();
			for (Json::Value::Members::const_iterator i = members.begin(); i != members.end(); ++i) {
				if (root[*i].isConvertibleTo(Json::ValueType::stringValue)) {
					ctx.add_value(*i, root[*i].asString());
				} else {
					ctx.add_value(*i, writer.write(root[*i]));
				}
			}
		}
	}
}

void http_environment::init_custom_params() {
	// initialize default http request parameters
	for (http_request::http_headers::const_iterator i = req.all_headers().begin();
	  i != req.all_headers().end(); ++i) {
		string s = i->first;
		replace(s.begin(), s.end(), '-', '_');
		session->add_value(string("HTTP_") + s, i->second);
	}
	session->add_value("PATH_INFO", req.get_path_info());
	session->add_value("HTTP_METHOD", req.method());
	session->add_value("HTTP_REMOTE_ADDR", req.get_remote_addr());
	session->add_value("SERVER_NAME", req.get_server_name());
	session->add_value("SERVER_PORT", to_string<int>(req.get_server_port()));
	session->add_value("HTTPS", req.get_https() ? "on" : string(""));
	// parse request parameters
	strings pairs;
	switch (req.get_method()) {
		case http_method::head:
			session->add_value("HTTP_METHOD", "GET");
		case http_method::get: {
			// extract name=value parameters from a query string
			strings pairs = tokenize()(req.get_query_string(), "&");
			for (strings::const_iterator i = pairs.begin();
			  i != pairs.end(); ++i) {
				string param, value;
				tokenize()(*i, param, value, false, "=");
				session->add_value(url().decode(param),
				  url().decode(value));
			}
			break;
		}
		case http_method::put:
		case http_method::post: {
			// parse content type
			string ct, cv;
			tokenize()(req.get_content_type(), ct, cv, false, ";");
			ct = trim()(ct);
			cv = trim()(cv);
			// select appropriate content parser method
			if (compare()(ct, "application/x-www-form-urlencoded"))
				parse_urlencoded(*session, req);
			else if (compare()(ct, "multipart/form-data"))
				parse_data(*session, req);
			else if (compare()(ct, "application/json"))
				parse_json(*session, req);
			else
				parse_plain(*session, req);
			break;
		}
		default: {
			// other http method
			parse_plain(*session, req);
			break;
		}
	}
}

std::string http_environment::get_path() {
	// determine request type (plain, restful, xml-rpc, soap) to parse
	if (req.get_method() == http_method::post &&
	  req.get_content_type() == "text/xml") {
		xmlDoc *doc = xmlParseMemory(req.get_content(),
		  req.get_content_size());
		xmlNode *root = NULL;
		try {
			root = xmlDocGetRootElement(doc);
			if (!root)
				throw dbp::exception(_("invalid XML structure of the request"));
			if (strcmp((const char*)root->name, "methodCall") == 0) {
				// it's a XML-RPC request
				return req.get_path_translated();
			} else
			if (strcmp((const char*)root->name, "Envelope") == 0) {
				// it's a SOAP request
				return req.get_path_translated();
			}
			xmlFreeNode(root);
			xmlFreeDoc(doc);
	  	}
		catch (dbp::exception &e) {
			xmlFreeNode(root);
			xmlFreeDoc(doc);
		}
	}
	// it's a regular or RESTful request
	strings path = tokenize()(req.get_path_info(), "/");
	int cnt = 0;
	for (strings::const_iterator i = path.begin(); i != path.end(); ++i) {
		if (cnt > 0)
			session->add_value(string("arg") + to_string<int>(cnt), *i);
		++cnt;
	}
	return req.get_path_translated();
}

} // namespace
