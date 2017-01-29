#include <Debug.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>
#include <utils/Mlooper.h>
#include <utils/Timers.h>


//DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);



using namespace std;
using namespace mlib;
class mhandler : public MessageHandler{
public:
	void handleMessage(const Message &message){

		printf("message mWhat=%d\n",message.mWhat);
	};
};

class t:public Thread{
public:
	t(){


	printf("t create\n");

};
virtual ~t(){};

	status_t readyToRun(){
		Mlooper *looper = Mlooper::prepare();
		mLooper = looper;
	};

	Mlooper *mLooper;
private:
virtual bool threadLoop(); 

};
bool t::threadLoop(){
	printf("wait epoll event\n");
	mLooper->pollOnce(10000);
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
	uint8_t data[]="Im message";
	Message msg ;
	msg.setData(data,sizeof(data));
	DEBUG("Msg data %s",msg.mData);
//	MessageTest(Message(10));
//	*id = (tx1) i;
	//INFO("id %x , i %x ,*id %x",id,i,*id);
	t t1;
	t1.run();
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
