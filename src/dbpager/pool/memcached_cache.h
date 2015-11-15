/*
 * memcached_cache.h
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

#ifndef _MEMCACHED_CACHE_H_
#define _MEMCACHED_CACHE_H_

#include <string>

#include <libmemcached/memcached.h>

#include <pool/cache.h>
#include <dbpager/context.h>

namespace dbpager {

class memcached_cache: public cache {
public:
	memcached_cache();
	virtual ~memcached_cache();
	virtual bool get(context &ctx, const std::string &id);
	virtual void put(context &ctx, const std::string &id);
	virtual void setup(const dbp::app_config &options);
private:
	memcached_st *servers;
	int ttl;
};

}

#endif /*_MEMCACHED_CACHE_H_*/
