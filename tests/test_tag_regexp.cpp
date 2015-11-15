#include <iostream>
#include <string>
#include <sstream>

#include <dcl/application.h>

#include <dbpager/context.h>
#include <dbpager/tag_impl.h>
#include <tag/tag_expression.h>
#include <tag/tag_regexp.h>

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
		/* testing tag_regexp */
		int tests = 3;
		// 1. The tag should find all regular expression occurences in the
		//    source string
		{
			tag_regexp t;
			tag_expression *txt = new tag_expression();
			txt->set_text("{$test}");
			t.add_child(txt);
			// name
			tag *name = new tag_impl();
			name->set_text("test");
			t.add_parameter("name", name);
			// expression
			tag *expression = new tag_impl();
			expression->set_text("is");
			t.add_parameter("expr", expression);
			// value
			tag *value = new tag_impl();
			value->set_text("This is a kiss");
			t.add_parameter("value", value);
			// execute
			ostringstream s;
			t.execute(ctx, s, NULL);
			if (s.str() == string("isisis"))
				tests--;
			else
				cerr << "test #1 failed (" << s.str() << ")" << endl;
		}
		// 2. The tag should work with complex regular expressions
		{
			tag_regexp t;
			tag_expression *txt = new tag_expression();
			txt->set_text("{$test}");
			t.add_child(txt);
			// name
			tag *name = new tag_impl();
			name->set_text("test");
			t.add_parameter("name", name);
			// expression
			tag *expression = new tag_impl();
			expression->set_text("[0-9a-z_]+@[0-9a-z_^.]+\\.[a-z]{2,6}");
			t.add_parameter("expr", expression);
			// value
			tag *value = new tag_impl();
			value->set_text("This is an email@dbpager.com letter.");
			t.add_parameter("value", value);
			// execute
			ostringstream s;
			t.execute(ctx, s, NULL);
			if (s.str() == string("email@dbpager.com"))
				tests--;
			else
				cerr << "test #2 failed (" << s.str() << ")" << endl;
		}
		// 3. The tag should find all regular expression occurences in the
		//    source string
		{
			tag_regexp t;
			tag_expression *txt = new tag_expression();
			txt->set_text("{$test}");
			t.add_child(txt);
			// name
			tag *name = new tag_impl();
			name->set_text("test");
			t.add_parameter("name", name);
			// expression
			tag *expression = new tag_impl();
			expression->set_text("is");
			t.add_parameter("expr", expression);
			// value
			tag *value = new tag_impl();
			value->set_text("This is a kiss");
			t.add_parameter("value", value);
			// replace with
			tag *wth = new tag_impl();
			wth->set_text("--");
			t.add_parameter("with", wth);
			// execute
			ostringstream s;
			t.execute(ctx, s, NULL);
			if (s.str() == string("Th-- -- a k--s"))
				tests--;
			else
				cerr << "test #3 failed (" << s.str() << ")" << endl;
		}
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

