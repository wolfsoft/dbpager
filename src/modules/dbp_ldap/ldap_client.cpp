/*
 * ldap_client.cpp
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

#include <ldap.h>

#include "ldap_client.h"

namespace dbpager {

using namespace dbp;

ldap_connection::ldap_connection(const std::string& url) {
	int rc;
	rc = ldap_initialize(&_ldap, url.c_str());
	if (rc != LDAP_SUCCESS) {
		throw ldap_connection_exception(ldap_err2string(rc));
	}

	int version = LDAP_VERSION3;
	rc = ldap_set_option(_ldap, LDAP_OPT_PROTOCOL_VERSION, &version);
	if (rc != LDAP_SUCCESS) {
		throw ldap_connection_exception(ldap_err2string(rc));
	}
}

ldap_connection::~ldap_connection() {
	if (_ldap != NULL) {
		ldap_unbind_ext_s(_ldap, NULL, NULL);
	}
}

void ldap_connection::bind(const std::string& dn, const std::string& password) {
	int rc;
	if (dn.empty() && password.empty()) {
		rc = ldap_sasl_bind_s(_ldap, NULL, LDAP_SASL_SIMPLE, NULL, NULL, NULL, NULL);
		if (rc != LDAP_SUCCESS) {
			throw ldap_connection_exception(ldap_err2string(rc));
		}
		return;
	}

	struct berval cred;
	cred.bv_val = const_cast<char*>(password.c_str());
	cred.bv_len = password.length();
	rc = ldap_sasl_bind_s(_ldap, dn.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
	if (rc != LDAP_SUCCESS) {
		throw ldap_connection_exception(ldap_err2string(rc));
	}
}

std::vector<std::map<std::string, std::string>> ldap_connection::search(const std::string& basedn, const std::string& filter) {
	std::vector<std::map<std::string, std::string>> rslt;

	LDAPMessage *result, *entry;
	char *dn;
	int rc;

	rc = ldap_search_ext_s(_ldap, basedn.c_str(), LDAP_SCOPE_SUBTREE, filter.c_str(), NULL, 0, NULL, NULL, NULL, 0, &result);
	if (rc != LDAP_SUCCESS) {
		throw ldap_connection_exception(ldap_err2string(rc));
	}

	for (entry = ldap_first_entry(_ldap, result); entry != NULL; entry = ldap_next_entry(_ldap, entry)) {
		std::map<std::string, std::string> item;

		dn = ldap_get_dn(_ldap, entry);
		if (dn != NULL) {
			item["dn"] = std::string(dn);
			ldap_memfree(dn);
		}

		BerElement *ber = nullptr;
		char *attribute = nullptr;
		for (attribute = ldap_first_attribute(_ldap, entry, &ber); attribute != NULL; attribute = ldap_next_attribute(_ldap, entry, ber)) {
			struct berval **vals = ldap_get_values_len(_ldap, entry, attribute);
			if (vals != NULL) {
				for (int i = 0; vals[i] != NULL; i++) {
					item[attribute] = std::string(vals[i]->bv_val, vals[i]->bv_len);
				}
				ldap_value_free_len(vals);
			}
			ldap_memfree(attribute);
		}
		if (ber != NULL) {
			ber_free(ber, 0);
		}

		rslt.push_back(item);
	}

	ldap_msgfree(result);

	return rslt;
}

} // namespace
