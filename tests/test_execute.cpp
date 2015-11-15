#include <assert.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <dcl/dclbase.h>

#include <interpreter/environment.h>
#include <parser/dbpx_parser.h>

using namespace std;
using namespace dbp;
using namespace dbpager;

#define THREAD_COUNT 53
#define ITER_COUNT 1000

class job {
public:
	job(): app(application::instance()), counter(0) {
		app.on_execute(create_delegate(this, &job::on_execute));
	};
	int on_execute() {
		xmlInitParser();
		// Create threads
		for (int i = 0; i < THREAD_COUNT; i++) {
			thread *t = new thread();
			if (i % 2 != 0)
				t->on_execute(create_delegate(this, &job::do_execute));
			else
				t->on_execute(create_delegate(this, &job::do_execute2));
			threads.push_back(t);
			t->start();
		}
		// wait for threads job done
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			(*i)->wait_for();
		}
		// dispose threads
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			delete (*i);
		}
		xmlCleanupParser();
		// return the result
		return 0;
	};
	void do_execute(thread_int&) {
		dbp::url u(string(TEST_FILE_PATH) + string("/test.dbpx"));
		u.scheme = "file";
		dbpx_parser p(u);
		for (int i = 0; i < ITER_COUNT; i++) {
			app_config config;
			console_environment env(config);
			ostringstream out(ostringstream::out | ostringstream::binary);

			tag* app = p.parse();
			assert(app != NULL);

			local_context local(env.get_context());
			app->execute(local, out, NULL);

			assert(!out.str().empty());
			delete app;
		}
	};
	void do_execute2(thread_int&) {
		for (int i = 0; i < ITER_COUNT; i++) {
			dbp::url u(string(TEST_FILE_PATH) + string("/test.dbpx"));
			u.scheme = "file";
			dbpx_parser p(u);
			app_config config;
			console_environment env(config);
			ostringstream out(ostringstream::out | ostringstream::binary);

			tag* app = p.parse();
			assert(app != NULL);

			local_context local(env.get_context());
			app->execute(local, out, NULL);

			assert(!out.str().empty());
			delete app;
		}
	};
	// the reference to the console application class
	application &app;
private:
	class console_environment: public environment {
	public:
		console_environment(const dbp::app_config& config):
		  environment(config) { };
		virtual void init_custom_params() { };
	};
	mutex cs;
	volatile int counter;
	vector<thread*> threads;
};

IMPLEMENT_APP(job().app);
