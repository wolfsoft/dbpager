/*
 * tag_storage.h
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

#ifndef _TAG_STORAGE_H_
#define _TAG_STORAGE_H_

#include <iterator>
#include <string>
#include <map>
#include <memory>
#include <ostream>
#include <stdlib.h>

#include <db_cxx.h>
#include <dcl/dclbase.h>
#include <dbpager/tag_impl.h>

namespace dbpager {
namespace bdb {

class bdb_database;

class bdb_environment {
public:
	bdb_environment(const std::string &path);
	~bdb_environment();
	std::auto_ptr<bdb_database> database(std::string &href) const;
private:
	DbEnv *env;
};


class bdb_environments: public dbp::singleton<bdb_environments> {
	friend class dbp::singleton<bdb_environments>;
public:
	virtual ~bdb_environments();
	bdb_environment& environment(const std::string &href);
private:
	std::map<std::string, bdb_environment*> envs;
	dbp::mutex m;
};


class bdb_database {
	friend class bdb_environment;
	friend class iterator;
public:
	class iterator: public std::iterator<
	  std::forward_iterator_tag,
	  std::pair<std::string, std::string> > {
		friend class bdb_database;
	public:
		~iterator() {
			try {
				if (cursorp)
					cursorp->close();
			}
			catch(...) {}
		}
		iterator& operator++() {
			if (ret != 0)
				return *this;
			Dbt dkey = Dbt((void*)(key.c_str()), key.length());
			dkey.set_flags(DB_DBT_MALLOC);
			Dbt data(NULL, 0);
			data.set_flags(DB_DBT_MALLOC);
			ret = cursorp->get(&dkey, &data, DB_NEXT);
			if (ret == 0) {
				if (::strncmp((char*)dkey.get_data(), key.c_str(), key.length()) != 0) {
					ret = DB_NOTFOUND;
					free(dkey.get_data());
					free(data.get_data());
					return *this;
				}
				rslt.first.clear();
				rslt.first.append((char*)dkey.get_data(), dkey.get_size());
				rslt.second.clear();
				rslt.second.append((char*)data.get_data(), data.get_size());
				free(dkey.get_data());
				free(data.get_data());
			} else
				ret = DB_NOTFOUND;
			return *this;
		}
		iterator operator++(int) {
			iterator save = *this;
			++*this;
			return save;
		}
		const std::pair<std::string, std::string>* operator->() const {
			return &**this;
		}
		const std::pair<std::string, std::string>& operator*() const {
			return rslt;
		}
		bool operator==(const iterator &value) const {
			return (ret == value.ret);
		}
		bool operator!=(const iterator &value) const {
			return (ret != value.ret);
		}
	private:
		Db &db;
		Dbc *cursorp;
		int ret;
		std::string key;
		std::pair<std::string, std::string> rslt;
		iterator(Db &database): db(database), cursorp(NULL),
		  ret(DB_NOTFOUND) { }
		iterator(Db &database, const std::string &search_key):
		  db(database), cursorp(NULL), ret(DB_NOTFOUND), key(search_key) {
			db.cursor(NULL, &cursorp, 0);
			Dbt dkey = Dbt((void*)(key.c_str()), key.length());
			dkey.set_flags(DB_DBT_MALLOC);
			Dbt data(NULL, 0);
			data.set_flags(DB_DBT_MALLOC);
			ret = cursorp->get(&dkey, &data, DB_SET_RANGE);
			if (ret == 0) {
				if (::strncmp((char*)dkey.get_data(), key.c_str(), key.length()) != 0) {
					ret = DB_NOTFOUND;
					free(dkey.get_data());
					free(data.get_data());
					return;
				}
				rslt.first.clear();
				rslt.first.append((char*)dkey.get_data(), dkey.get_size());
				rslt.second.clear();
				rslt.second.append((char*)data.get_data(), data.get_size());
				free(dkey.get_data());
				free(data.get_data());
			}
		}
		iterator operator=(const iterator&) {
			return iterator(db, key);
		}
	};
	~bdb_database();
	bool is_opened() const;
	void open();
	void close();
	void flush();
	std::string get(const std::string &key);
	void set(const std::string &key, const std::string &value);
	void del(const std::string &key);
	bdb_database::iterator find(const std::string &key);
	bdb_database::iterator end();
private:
	bdb_database(DbEnv *environment, std::string &filename);
	Db *db;
	std::string fname;
	bool _is_opened;
	bool _was_write;
};

class tag_storage_exception: public dbp::exception {
public:
	tag_storage_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_storage: public tag_impl {
public:
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
};

}} // namespace

#endif /*_TAG_STORAGE_H_*/

