/*
 * mod_session_memcached.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _MOD_SESSION_MEMCACHED_H_
#define _MOD_SESSION_MEMCACHED_H_

#include <string>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/session.h>

namespace dbpager {

class mod_session_memcached_exception: public dbp::exception {
public:
	mod_session_memcached_exception(const std::string &msg): dbp::exception(msg) { }
};

class mod_session_memcached: public session {
public:
	virtual std::string get(const std::string &key);
	virtual void put(const std::string &key, const std::string &value);
	void set_servers(const std::string &servers) {
		this->servers = servers;
	}
	void set_ttl(int ttl) {
		this->ttl = ttl;
	};
private:
	std::string empty;
	std::string servers;
	int ttl;
};

}

#endif /*_MOD_SESSION_MEMCACHED_H_*/
