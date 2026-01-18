/*
 * mod_session_jwt.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2026 - Dennis Prochko <dennis.prochko@gmail.com>
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

#include <dcl/dclbase.h>

#include "mod_session_jwt.h"
#include "jwt-cpp/jwt.h"

namespace dbpager {

using namespace std;
using namespace dbp;

mod_session_jwt::mod_session_jwt() {
	is_new = true;
}

mod_session_jwt::mod_session_jwt(const std::string &token) {
	id = token;
}

void mod_session_jwt::load(context &ctx) {
	if (id.empty())
		return;
	auto decoded = jwt::decode(id);
	try {
		jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256(secret))
			.verify(decoded);

		for (auto& e : decoded.get_payload_json()) {
			ctx.add_value(e.first, e.second.to_str());
		}
	} catch (const std::exception &e) {
		throw mod_session_jwt_exception(std::string("JWT verification failed: ") + e.what());
	}
}

void mod_session_jwt::save(const context &ctx, dbp::http_response &resp) {
	if (!ctx.empty()) {
		auto token = jwt::create()
			.set_type("JWS")
			.set_issued_now()
			.set_not_before(std::chrono::system_clock::now());
		if (ttl > 0) {
			token.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ttl});
		}

		for (const auto& e : ctx.get_values()) {
			token.set_payload_claim(e.first, jwt::claim(e.second));
		}

		id = token.sign(jwt::algorithm::hs256{secret});

		// setup cookies
		if (is_new) {
			dbp::http_cookie c("session-token", id);
			c.path = "/";
			c.http_only = true;
			if (is_https) {
				c.same_site = "none";
				c.secure = true;
				c.partitioned = true;
			}
			resp.set_cookie(c);
		}
	}
}

std::unique_ptr<session_holder> mod_session_jwt_factory::create_session(const dbp::http_request &req) {
	const dbp::http_cookies &c = req.get_cookies();
	for (dbp::http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
		if (i->name == "session-token") {
			mod_session_jwt* session = new mod_session_jwt(i->value);
			session->is_https = req.get_https();
			session->set_secret(secret);
			session->set_ttl(ttl);
			return std::unique_ptr<session_holder>(session);
		}
	}

	mod_session_jwt* session = new mod_session_jwt();
	session->is_https = req.get_https();
	session->set_secret(secret);
	session->set_ttl(ttl);
	return std::unique_ptr<mod_session_jwt>(session);
}

} // namespace
