/*
 * tag_http.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 Dennis Prochko <wolfsoft@mail.ru>
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
#include <sstream>

#include <dcl/strutils.h>
#include "tag/tag_http.h"

#include <curl/curl.h>

#include "dbpager/consts.h"

namespace dbpager {

using namespace std;
using namespace dbp;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::ostream*)userp)->write((char*)contents, size * nmemb);
    return size * nmemb;
}

void tag_http::execute(context &ctx, std::ostream &out, const tag *caller) const {
	CURL *curl;
	curl = curl_easy_init();
	if (!curl)
		throw tag_exception("can't initialize cURL library");

	// read key parameters
	const string &href = get_parameter(ctx, "href");
	const string &user = get_parameter(ctx, "user");
	const string &password = get_parameter(ctx, "password");

	string method(get_parameter(ctx, "method"));
	if (method.empty()) {
		method = "GET";
	} else {
		transform(method.begin(), method.end(), method.begin(), ::toupper);
	}

	struct curl_slist *list = NULL;
	CURLcode res;

	ostringstream content(ostringstream::out | ostringstream::binary);
	tag_impl::execute(ctx, content, caller);
	string content_str = content.str();
	string http_agent = app_full_name;
	stringstream _out;
	try {
		curl_easy_setopt(curl, CURLOPT_URL, href.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, http_agent.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_out);

		if (!user.empty()) {
			string up = user + string(":") + password;
			curl_easy_setopt(curl, CURLOPT_USERPWD, up.c_str());
		}

		if (!content_str.empty()) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content_str.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content_str.length());
		}

		if (method == "POST")
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
		else if (method != "GET") {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
		}

		// extract other parameters
		for (parameters::const_iterator i = params.begin(); i != params.end(); ++i) {
			if (i->first == "href") continue;
			if (i->first == "method") continue;
			list = curl_slist_append(list, string(i->first + string(": ") + i->second->get_text()).c_str());
		}

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (res != CURLE_OK)
			throw tag_exception((format(_("http request failed: {0}")) % string(curl_easy_strerror(res))).str());
		if (http_code >= 400)
			throw tag_exception((format(_("http request failed: {0}")) % _out.str()).str());

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);

		out << _out.rdbuf();

	} catch (...) {
		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
		throw;
	}

}

} // namespace
