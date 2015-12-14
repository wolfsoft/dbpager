/*
 * services.h
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

#ifndef _SERVICES_H_
#define _SERVICES_H_

#include <dcl/dclbase.h>

#include <dbpager/session.h>

namespace dbpager {

class services {
public:
	//! Obtain the options from the config file
	virtual dbp::app_config& get_config() = 0;

	//! Obtain the session service
	virtual dbpager::session& get_session() = 0;

	//! Obtain the logger service
	virtual dbp::logger& get_logger() = 0;
};

}

#endif /*_SERVICES_H_*/

