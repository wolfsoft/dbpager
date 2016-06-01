/*
 * functions.cpp
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

#include <locale.h>
#include <cstdlib>

#include <unistd.h>
#define _GNU_SOURCE
#include <crypt.h>

#include <algorithm>
#include <iterator>
#include <iostream>

#include <dcl/codec_base64.h>
#include <dcl/datetime.h>
#include <dcl/encoder_md5.h>
#include <dcl/strutils.h>
#include <dcl/uuid.h>
#include <dcl/url.h>

#include "tag/functions.h"

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace dbp::codec;

void function_iif::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 3)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 3).str());

	string what = get_parameter(ctx, "arg1");

	if (what.empty())
		out << get_parameter(ctx, "arg3");
	else
		out << get_parameter(ctx, "arg2");

}

void function_byte::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	ostringstream s(ostringstream::out | ostringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	out << char(from_string<int>(s.str()));
}

void function_length::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	ostringstream s(ostringstream::out | ostringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	out << s.str().length();
}

void function_concatenate::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	for (parameters::const_iterator i = params.begin();
	  i != params.end(); ++i) {
		(i->second)->execute(ctx, out, caller);
	}
}

void function_pos::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 2)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 2).str());
	string what = get_parameter(ctx, "arg1");
	string where = get_parameter(ctx, "arg2");
	out << where.find(what);
}

void function_encode_base64::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	codec_base64().encode(s, out);
}

void function_decode_base64::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	codec_base64().decode(s, out);
}

void function_md5::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	encoder_md5().encode(s, out);
}

void function_crypt::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 2)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 2).str());
	struct crypt_data data;
	data.initialized = 0;
	out << crypt_r(get_parameter(ctx, "arg1").c_str(), get_parameter(ctx, "arg2").c_str(), &data);
}

void function_uuid::execute(context&, std::ostream &out,
  const tag*) const {
	out << uuid().str();
}

void function_urlencode::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	out << url().encode(s.str());
}

void function_urldecode::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	out << url().decode(s.str());
}

void function_translate::execute(context &ctx, std::ostream &out,
  const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	locale_t old_locale = i18n::locale(ctx.get_value("@LANG_LOCALE@").c_str());
	out << i18n::translate(ctx.get_value("@LANG_DOMAIN@"), s.str());
	i18n::locale(old_locale);
}

struct uppercase {
	uppercase(const locale &loc): loc(loc) { }
	char operator()(char c) { return std::toupper(c, loc); }
private:
    const locale &loc;
};

void function_upper::execute(context &ctx, std::ostream &out, const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	stringstream s(stringstream::in | stringstream::out | stringstream::binary);
	(params.begin()->second)->execute(ctx, s, this);
	s.seekg(0);
	transform(istream_iterator<char>(s),
	  istream_iterator<char>(),
	  ostream_iterator<char>(out, ""),
	  uppercase(std::locale("")));
}

function_rnd::function_rnd(const std::string &tag_name):
  tag_impl(tag_name) {
	srand(time(NULL));
}

void function_rnd::execute(context &ctx, std::ostream &out, const tag*) const {
	int base = 0;
	if (params.size() == 1) {
		stringstream s(stringstream::in | stringstream::out | stringstream::binary);
		(params.begin()->second)->execute(ctx, s, this);
		base = from_string<int>(s.str());
		out << rand() % base;
	} else
		out << rand();
}

void function_date_utc::execute(context &ctx, std::ostream &out, const tag*) const {
	if (params.size() != 1)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 1).str());
	datetime d(get_parameter(ctx, "arg1"), "%s");
	out << d.as_gmt();
}

void function_date_from::execute(context &ctx, std::ostream &out, const tag*) const {
	if (params.size() != 2)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 2).str());

	datetime d(get_parameter(ctx, "arg1"), get_parameter(ctx, "arg2"));
	out << d.str("%s");
}

void function_date_fmt::execute(context &ctx, std::ostream &out, const tag*) const {
	if (params.size() != 2)
		throw function_exception(
		  (format(_("wrong number of arguments ({0} instead {1} expected)")) %
		    params.size() % 2).str());
	datetime d(get_parameter(ctx, "arg2"), "%s");
	out << d.str(get_parameter(ctx, "arg1"));
}

} // namespace

