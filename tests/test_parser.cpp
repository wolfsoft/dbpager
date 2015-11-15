#include <assert.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <dcl/dclbase.h>

#include <parser/dbpx_parser.h>

using namespace std;
using namespace dbp;
using namespace dbpager;

#define THREAD_COUNT 53

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
			t->on_execute(create_delegate(this, &job::do_execute));
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
		filefs fs;
		filefs::const_iterator it = fs.find(string(TEST_FILE_PATH) + string("/*.dbpx"));
		//filefs::const_iterator it = fs.find("/home/dennis/public_html/dietadiary.com/content/*.dbpx");
		while (it != fs.end()) {
			dbp::url u(*it);
			u.scheme = "file";
			dbpx_parser p(u);
			tag* app = p.parse();
			assert(app != NULL);
			delete app;
			++it;
		}
	};
	// the reference to the console application class
	application &app;
private:
	mutex cs;
	volatile int counter;
	vector<thread*> threads;
};

IMPLEMENT_APP(job().app);
