/*
 * tag_async.cpp
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

#include <dcl/strutils.h>

#include <dbpager/tag_tag.h>
#include <dbpager/tag_usr.h>

#include "tag/tag_async.h"

#include <iostream>
#include <typeinfo>
#include <thread>
#include <future>
#include <vector>

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_async::execute(context &ctx, std::ostream &out, const tag *caller) const {
	std::vector<std::future<std::string>> futures;
	const auto& childs = get_childs();
	for (auto it = childs.begin(); it != childs.end(); ++it) {
		futures.push_back(std::async(std::launch::async, [&ctx, caller, it] {
			std::ostringstream _out(ostringstream::out | ostringstream::binary);
			local_context local_ctx(&ctx);
			try {
				(*it)->execute(local_ctx, _out, caller);
			} catch (...) {
				return std::string();
			}
			return _out.str();
		}));
	}
	for (auto &f: futures) {
		out << f.get();
	}
}

} // namespace
