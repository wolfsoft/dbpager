/*
 * tag_query.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2017 - Dennis Prochko <wolfsoft@mail.ru>
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

#include <vector>

#include <dcl/strutils.h>

#include "redis_connection.h"
#include "tag_query.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_query::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	// obtain the current database of current context
	string db_ptr = ctx.get_value(string("@REDIS:CONNECTION@") + get_parameter(ctx, "database"));
	if (db_ptr.empty()) {
		string id = get_parameter(ctx, "database");
		if (id.empty()) {
			id = _("(default)");
		}
		throw tag_query_exception(
		  (format(_("database (id='{0}') is not defined in the current "
			"context")) % id).str());
	}
	redis_connection *conn = (redis_connection*)from_string<void*>(db_ptr);
	redisReply *reply = NULL;

	ctx.enter();
	try {
		const string &statement = get_parameter(ctx, "statement");
		reply = (redisReply*)redisCommand(conn->get_ptr(), statement.c_str());

		if (!reply)
			throw dbp::exception(conn->get_ptr()->errstr);

		switch (reply->type) {
			case REDIS_REPLY_STATUS:
				ctx.add_value("value", reply->str);
				tag_impl::execute(ctx, out, caller);
				break;
			case REDIS_REPLY_ERROR:
				throw dbp::exception(string(reply->str));
				break;
			case REDIS_REPLY_INTEGER:
				ctx.add_value("value", to_string<long long>(reply->integer));
				tag_impl::execute(ctx, out, caller);
				break;
			case REDIS_REPLY_STRING:
				ctx.add_value("value", reply->str);
				tag_impl::execute(ctx, out, caller);
				break;
			case REDIS_REPLY_NIL:
				tag_impl::execute(ctx, out, caller);
				break;
			case REDIS_REPLY_ARRAY:
				ctx.add_value("count", to_string<int>(reply->elements));
				for(int j = 0; j < reply->elements; j++) {
					ctx.enter();
					try {
						ctx.add_value("value", reply->element[j]->str);
						tag_impl::execute(ctx, out, caller);
						ctx.leave();
					} catch(...) {
						ctx.leave();
						throw;
					}
				}
				break;
			default:
				throw dbp::exception("Redis: unknown reply type");
		}

		freeReplyObject(reply);
		ctx.leave();
	} catch(const std::exception &e) {
		if (reply)
			freeReplyObject(reply);
		ctx.leave();
		throw tag_query_exception(e.what());
	} catch(...) {
		if (reply)
			freeReplyObject(reply);
		ctx.leave();
		throw;
	}

}

} // namespace

