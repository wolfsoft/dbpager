/*
 * tag_storage.cpp
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

#include "tag_storage.h"

namespace dbpager {
namespace bdb {

using namespace std;
using namespace dbp;
using namespace dbpager;

// map of environments

bdb_environments::~bdb_environments() {
	mutex_guard guard(m);
	map<string, bdb_environment*>::iterator it;
	for (it = envs.begin(); it != envs.end(); ++it)
		delete it->second;
}

bdb_environment& bdb_environments::environment(const string &href) {
	mutex_guard guard(m);
	string path, name;
	tokenize()(href, path, name);
	if (path.empty())
		path = "./";
	map<string, bdb_environment*>::iterator it = envs.find(path);
	if (it != envs.end())
		return *(it->second);
	else {
		bdb_environment *env = new bdb_environment(path);
		envs.insert(pair<string, bdb_environment*>(path, env));
		return *env;
	}
}

// environment

bdb_environment::bdb_environment(const std::string &path): env(NULL) {
	env = new DbEnv(0);
	try {
		env->open(path.c_str(), DB_CREATE | DB_INIT_MPOOL |
		  DB_INIT_LOCK | DB_THREAD, 0);
	} catch(DbException &e) {
		throw tag_storage_exception((format(_("storage can't be opened ({0})"))
		  % e.what()).str());
	}
}

bdb_environment::~bdb_environment() {
	env->close(0);
	delete env;
}

auto_ptr<bdb_database> bdb_environment::database(std::string &href) const {
	return auto_ptr<bdb_database>(new bdb_database(env, href));
}

// database file

bdb_database::bdb_database(DbEnv *environment, std::string &filename):
  db(NULL), fname(filename), _is_opened(false), _was_write(false) {
	db = new Db(environment, 0);
}

bdb_database::~bdb_database() {
	if (_is_opened)
		db->close(0);
	delete db;
}

bool bdb_database::is_opened() const {
	return _is_opened;
}

void bdb_database::open() {
	try {
		db->open(NULL, fname.c_str(), NULL, DB_BTREE, DB_CREATE, 0);
		_is_opened = true;
	} catch(DbException &e) {
		throw tag_storage_exception((format(_("storage can't be opened ({0})"))
		  % e.what()).str());
	}
}

void bdb_database::close() {
	if (_is_opened)
		db->close(0);
	_is_opened = false;
}

void bdb_database::flush() {
	if (_is_opened && _was_write)
		db->sync(0);
	_was_write = false;
}

std::string bdb_database::get(const std::string &key) {
	if (!_is_opened)
		throw tag_storage_exception(_("storage is not opened"));
	Dbt k((void*)(key.c_str()), key.length());
	k.set_flags(DB_DBT_USERMEM);
	Dbt d(NULL, 0);
	d.set_flags(DB_DBT_MALLOC);
	if (db->get(NULL, &k, &d, 0) == 0) {
		string rslt;
		rslt.append((const char*)d.get_data(), d.get_size());
		free(d.get_data());
		return rslt;
	}
	throw tag_storage_exception((format(_("can't retrieve data from "
	  "the storage ({0})")) % key).str());
}

void bdb_database::set(const std::string &key, const std::string &value) {
	if (!_is_opened)
		throw tag_storage_exception(_("storage is not opened"));
	Dbt k((void*)(key.c_str()), key.length());
	k.set_flags(DB_DBT_USERMEM);
	Dbt d((void*)(value.c_str()), value.length());
	d.set_flags(DB_DBT_USERMEM);
	if (db->put(NULL, &k, &d, 0) != 0)
		throw tag_storage_exception((format(_("can't add data to the "
		  "storage ({0})")) % key).str());
	_was_write = true;
}

void bdb_database::del(const std::string &key) {
	if (!_is_opened)
		throw tag_storage_exception(_("storage is not opened"));
	Dbt k((void*)(key.c_str()), key.length());
	k.set_flags(DB_DBT_USERMEM);
	if (db->del(NULL, &k, 0) != 0)
		throw tag_storage_exception((format(_("can't delete data "
		  "from the storage ({0})")) % key).str());
	_was_write = true;
}

bdb_database::iterator bdb_database::find(const std::string &key) {
	if (!_is_opened)
		throw tag_storage_exception(_("storage is not opened"));
	return bdb_database::iterator(*db, key);
}

bdb_database::iterator bdb_database::end() {
	return bdb_database::iterator(*db);
}

void tag_storage::execute(context &ctx, std::ostream &out,
  const tag *caller) const {
	string href = get_parameter(ctx, "href");
	string id = get_parameter(ctx, "id");
	if (href.empty())
		throw tag_storage_exception(
		  _("storage file name (href) is not defined"));
	bdb_environment &env = bdb_environments::instance().environment(href);
	auto_ptr<bdb_database> db = env.database(href);
	db->open();
	ctx.enter();
	try {
		ctx.add_value(string("@BDB:DATABASE@") + id, to_string<void*>(&*db));
		tag_impl::execute(ctx, out, caller);
		ctx.leave();
	}
	catch (...) {
		ctx.leave();
		throw;
	}
}

}} // namespace

