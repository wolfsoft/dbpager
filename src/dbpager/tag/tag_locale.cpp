/*
 * tag_locale.cpp
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
#include <dcl/i18n.h>

#include "tag/tag_locale.h"

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_locale::execute(context &ctx, std::ostream &out, const tag *caller) const {
	const string &domain = get_parameter(ctx, "domain");
	const string &path = get_parameter(ctx, "path");
	const string &locale = get_parameter(ctx, "locale");
	if (domain.empty()) {
		throw tag_locale_exception(
		  _("message domain (domain) is not defined"));
	}
	if (path.empty()) {
		throw tag_locale_exception(
		  _("base directory of the hierarchy containing message catalogs (path) is not defined"));
	}
	ctx.enter();
	try {
		// save the pointer to database for nested tags
		ctx.add_value("@LANG_DOMAIN@", domain);
		ctx.add_value("@LANG_LOCALE@", locale);
		i18n::register_catalog_at(domain, path);
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

} // namespace
