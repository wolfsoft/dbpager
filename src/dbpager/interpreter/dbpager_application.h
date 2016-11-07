/*
 * dbpager_application.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _DBPAGER_APPLICATION_H_
#define _DBPAGER_APPLICATION_H_

#include <ostream>

#include <dcl/dclbase.h>

#include <dbpager/services.h>

#include <interpreter/environment.h>

namespace dbpager {

//!	dbPager application class.
/*!
	Executes the given program.
*/
class dbpager_application: private dbp::noncopyable {
public:
	explicit dbpager_application(services *svc, const dbp::url &u);
	void execute(dbpager::environment &env, std::ostream &out) const;
private:
	dbpager_application() { };
#ifdef HAVE_CXX11
	std::unique_ptr<tag> t;
#else
	std::auto_ptr<tag> t;
#endif
	services *_services;
};

} // namespace

#endif /*_DBPAGER_APPLICATION_H_*/

