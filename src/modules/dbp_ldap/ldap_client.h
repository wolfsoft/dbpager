/*
 * ldap_client.h
 * This file is part of dbPager Server
 *
 * Copyright (c) 2025 Dennis Prochko <dennis.prochko@gmail.com>
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

#ifndef _LDAP_CLIENT_H_
#define _LDAP_CLIENT_H_

#include <string>

#include <dcl/dclbase.h>

#include <ldap.h>

namespace dbpager {

class ldap_connection_exception: public dbp::exception {
public:
	ldap_connection_exception(const std::string &msg): dbp::exception(msg) { }
};

class ldap_connection {
public:
	ldap_connection(const std::string& url);
	virtual ~ldap_connection();
	void bind(const std::string& dn, const std::string& password);
	std::vector<std::map<std::string, std::string>> search(const std::string& basedn, const std::string& filter);
private:
	LDAP *_ldap;
};

} // namespace

#endif /*_LDAP_CLIENT_H_*/
