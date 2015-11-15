/*
 * tag_javascript.cpp
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

#include <iostream>
#include <fstream>

#include <dcl/strutils.h>

#include "tag_javascript.h"

namespace dbpager {

using namespace std;
using namespace dbp;
using namespace v8;

void tag_javascript::execute(context &ctx, std::ostream &out,
  const tag *caller) const {

	HandleScope handle_scope;

	// Create a new context.
	Persistent<Context> context = Context::New();

	//context->AllowCodeGenerationFromStrings(true);

	// Enter the created context for compiling and
	// running the hello world script.
	Context::Scope context_scope(context);
	Handle<String> source;
	Handle<Script> script;
	Handle<Value> result;

	string src = get_parameter(ctx, "src");
	if (src.empty()) {
		stringstream str;
		tag_impl::execute(ctx, str, caller);
		script = Script::Compile(String::New(str.str().c_str()));
	} else {
		// read entire file into the string
		ifstream s(src.c_str(), ifstream::in | ifstream::binary);
		s.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
		string content = string(istreambuf_iterator<char>(s), istreambuf_iterator<char>());
		script = Script::Compile(String::New(content.c_str()));
	}

	//Handle<Value> result = script->Run();

//	out << *result;
}

} // namespace

