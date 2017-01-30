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
	void sendMessage(Mlooper *mlooper, Message* msg){
		DEBUG("sendMessage0");
		mlooper->sendMessage(this,msg);
	};
};

class t:public Thread{
public:
	t(){


	printf("t create\n");

};

status_t readyToRun(){
	INFO("MLOOPER prepared !");
	mLooper = new Mlooper();
	return NO_ERROR;
};

	Mlooper* mLooper;

	mhandler msgHandler;
private:
virtual bool threadLoop(); 

protected:

//virtual	~t();

};
bool t::threadLoop(){
	printf("wait epoll event\n");
	mLooper->pollOnce(1000000);
	return 1;

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
	//DEBUG("Msg data %s",msg.mData);
//	MessageTest(Message(10));
//	*id = (tx1) i;
	//INFO("id %x , i %x ,*id %x",id,i,*id);
	t t1;
//sleep(10);
	t1.run();
INFO("test process id %lx",pthread_self());
sleep(1);
for(;;){

t1.msgHandler.sendMessage(t1.mLooper,&msg);
msg.mWhat++;
}
	//t1.join();
	//ERROR("i= %d",i);
	//INFO("send exit t1 thread request");
	//INFO("t1.requestExitAndWait()= %d",t1.requestExitAndWait());
	//t1.requestExit();
	//sleep(10);
	//ASSERT("error_status:%d",a);
	return 0;
}
