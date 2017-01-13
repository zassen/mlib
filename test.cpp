#include "Debug.h"
#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

DEBUG_SET_LEVEL(DEBUG_LEVEL_ERROR);




using namespace std;
using namespace mlib;
class t:public Thread{
	public:
t(){


	printf("t create\n");

};
virtual ~t(){};
	private:
virtual bool threadLoop(); 
};

bool t::threadLoop(){


	printf("t thread is alive \n");
	sleep(1);
	return 1;

};

class t;
int main(void){
	status_t a=NO_ERROR;
	Mutex mlock;
	int i = 10;
	t t1;
	t1.run("t",44);
	ERROR("i= %d",i);
	sleep(20);
	ASSERT("error_status:%d",a);
	return 0;
}
