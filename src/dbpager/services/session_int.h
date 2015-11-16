/*
 * session_int.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko
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

#ifndef _SESSION_INT_H_
#define _SESSION_INT_H_

#include <string>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <dcl/rwlock.h>

#include <dbpager/session.h>

namespace dbpager {

class session_int: public session {
public:
	session_int() { };
	virtual ~session_int() { };
	virtual std::string get(const std::string &key) {
		dbp::rwlock_guard_read cs(m);
		const app_cache::const_iterator it = buffer.find(key);
		if (it == buffer.end())
			return std::string("");
		else
			return it->second;
	};
	virtual void put(const std::string &key, const std::string &value) {
		dbp::rwlock_guard_write cs(m);
		buffer[key] = value;
	};
private:
#ifdef HAVE_CXX11
	typedef std::unordered_map<std::string, std::string> app_cache;
#else
	typedef std::tr1::unordered_map<std::string, std::string> app_cache;
#endif
	dbp::rwlock m;
	app_cache buffer;
};

}

#endif /*_SESSION_INT_H_*/
