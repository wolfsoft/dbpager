#include <iostream>
#include <string>
#include <sstream>

#include <dcl/application.h>

#include <dbpager/context.h>
#include <dbpager/tag_impl.h>
#include <tag/functions.h>

using namespace std;
using namespace dbp;
using namespace dbpager;

class test: public application {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		local_context ctx;
		/* testing function_length */
		int tests = 4;
		// 1. The function should return the length of the string
		//    passed by first argument
		{
			ostringstream s;
			function_length f;
			tag *e = new tag_impl();
			e->set_text("teST");
			f.add_parameter("arg1", e);
			f.execute(ctx, s, NULL);
			if (s.str() == string("4"))
				tests--;
			else
				cerr << "test #1 failed" << endl;
		}
		// 2. The function should fail if the no arguments given
		try {
			ostringstream s;
			function_length f;
			f.execute(ctx, s, NULL);
			cerr << "test #2 failed" << endl;
		}
		catch (...) {
			tests--;
		}
		// 3. The function should fail if more than one arguments given
		try {
			ostringstream s;
			function_length f;
			tag *e = new tag_impl();
			e->set_text("teST");
			f.add_parameter("arg1", e);
			tag *e2 = new tag_impl();
			e2->set_text("teST2");
			f.add_parameter("arg2", e2);
			f.execute(ctx, s, NULL);
			cerr << "test #3 failed" << endl;
		}
		catch (...) {
			tests--;
		}
		// 4. The function should work on empty strings
		{
			ostringstream s;
			function_length f;
			tag *e = new tag_impl();
			e->set_text("");
			f.add_parameter("arg1", e);
			f.execute(ctx, s, NULL);
			if (s.str() == string("0"))
				tests--;
			else
				cerr << "test #4 failed" << endl;
		}
		// 5. The function should work on non-ASCII strings
		return tests;
	};
	// static method to create an instance of the our application
	static application& get_instance() {
		return test().app;
	};
private:
	// the link to the console application class
	application &app;
};

IMPLEMENT_APP(test::get_instance());

