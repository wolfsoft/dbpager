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
#include <dcl/url.h>

#include "tag/tag_http.h"

#include <curl/curl.h>

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
	url href = get_parameter(ctx, "href");
	if (href.scheme.empty()) {
		href.scheme = "http";
	}

	string method = get_parameter(ctx, "method");
	if (method.empty()) {
		method = "get";
	}

	string content = get_parameter(ctx, "content");

	struct curl_slist *list = NULL;
	CURLcode res;

	ostringstream sout(ostringstream::out | ostringstream::binary);

	try {

		if (method == "post") {
			curl_easy_setopt(curl, CURLOPT_POST, true);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
		} else if (method == "put") {
			curl_easy_setopt(curl, CURLOPT_UPLOAD, true);
		} else if (method != "get") {
			transform(method.begin(), method.end(), method.begin(), ::toupper);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
		}

		// extract other parameters
		for (parameters::const_iterator i = params.begin(); i != params.end(); ++i) {
			if (i->first == "href") continue;
			if (i->first == "method") continue;
			if (i->first == "content") continue;
			list = curl_slist_append(list, string(i->first + string(": ") + i->second->get_text()).c_str());
		}

		curl_easy_setopt(curl, CURLOPT_URL, href.str().c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sout);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			throw tag_exception((format(_("Http error: {0}")) % string(curl_easy_strerror(res))).str());

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);

	} catch (...) {
		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
		throw;
	}

	ctx.enter();
	try {
		ctx.add_value("content", sout.str());
		tag_impl::execute(ctx, out, caller);
	}
	catch (...) {
		ctx.leave();
		throw;
	}
	ctx.leave();

}

} // namespace
