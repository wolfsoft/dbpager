/*
 * functions.h
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

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/tag_impl.h>

namespace dbpager {

class function_exception: public dbp::exception {
public:
	function_exception(const std::string &msg): dbp::exception(msg) { }
};

class function_iif: public tag_impl {
public:
	function_iif(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_byte: public tag_impl {
public:
	function_byte(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_length: public tag_impl {
public:
	function_length(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_concatenate: public tag_impl {
public:
	function_concatenate(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_pos: public tag_impl {
public:
	function_pos(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_encode_base64: public tag_impl {
public:
	function_encode_base64(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_decode_base64: public tag_impl {
public:
	function_decode_base64(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_md5: public tag_impl {
public:
	function_md5(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

/*
	function crypt(data, salt); salt can be in format $6$salt$, salt can be up to 16 chars.
	Refer to: http://man7.org/linux/man-pages/man3/crypt.3.html

*/
class function_crypt: public tag_impl {
public:
	function_crypt(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_uuid: public tag_impl {
public:
	function_uuid(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_urlencode: public tag_impl {
public:
	function_urlencode(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_urldecode: public tag_impl {
public:
	function_urldecode(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_translate: public tag_impl {
public:
	function_translate(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_upper: public tag_impl {
public:
	function_upper(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_rnd: public tag_impl {
public:
	function_rnd(const std::string &tag_name = "");
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_date_utc: public tag_impl {
public:
	function_date_utc(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_date_from: public tag_impl {
public:
	function_date_from(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

class function_date_fmt: public tag_impl {
public:
	function_date_fmt(const std::string &tag_name = ""):
	  tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

} // namespace

#endif /*FUNCTIONS_H_*/

