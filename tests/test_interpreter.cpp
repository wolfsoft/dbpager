#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <dcl/dclbase.h>
#include <dcl/dclbase.h>

#include <interpreter/environment.h>
#include <interpreter/interpreter.h>

using namespace std;
using namespace dbp;
using namespace dbpager;

#define THREAD_COUNT 53
#define ITER_COUNT 5000

class job {
public:
	job(): app(application::instance()), counter(0), dbpager(NULL) {
		dbpager = new interpreter((string(TEST_FILE_PATH) + string("/test_config.conf")), app.get_logger());
		app.on_execute(create_delegate(this, &job::on_execute));
	};
	virtual ~job() {
		if (dbpager)
			delete dbpager;
	}
	int on_execute() {
		// Create threads
		for (int i = 0; i < THREAD_COUNT; i++) {
			thread *t = new thread();
			t->on_execute(create_delegate(this, &job::do_execute));
			threads.push_back(t);
			t->start();
		}
		// wait for threads job done
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			(*i)->wait_for();
			delete (*i);
		}
		// return the result
		return 0;
	};
	void do_execute(thread_int&) {
		for (int i = 0; i < ITER_COUNT; i++) {
			int file_num = rand() % 4 + 1;
			dbp::url u(string(TEST_FILE_PATH) + (format("/test{0}.dbpx") % file_num).str());
			u.scheme = "file";
			ostringstream out(ostringstream::out | ostringstream::binary);
			console_environment env(dbpager->get_config());
			dbpager->get_app(u)->execute(env, out);
			assert(!out.str().empty());
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
	interpreter *dbpager;
	vector<thread*> threads;
};

IMPLEMENT_APP(job().app);
