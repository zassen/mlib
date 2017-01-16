#include "Debug.h"
#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

//DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);




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


	DEBUG("t thread is alive 1\n");
	sleep(1);
	DEBUG("t thread is alive 2\n");
	sleep(1);
	DEBUG("t thread is alive 3\n");
	sleep(1);
	DEBUG("t thread is alive 4\n");
	sleep(1);
	DEBUG("t thread is alive 5\n");
	sleep(1);
	DEBUG("t thread is alive 6\n");
	sleep(1);
	return 0;

};

class t;
typedef void* tx;
typedef tx tx1;
int main(void){
	status_t a=NO_ERROR;
	Mutex mlock;
	tx1 *id;
	int i = 10;
//	*id = (tx1) i;
	//INFO("id %x , i %x ,*id %x",id,i,*id);
	t t1;
	t1.run("t",44);
	INFO("test process id %lx",pthread_self());
	t1.join();
	ERROR("i= %d",i);
	sleep(10);
	INFO("send exit t1 thread request");
	//INFO("t1.requestExitAndWait()= %d",t1.requestExitAndWait());
	//t1.requestExit();
	sleep(10);
	ASSERT("error_status:%d",a);
	return 0;
}
