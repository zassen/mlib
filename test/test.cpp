#include <Debug.h>
#include <Version.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>
#include <utils/Mlooper.h>
#include <utils/Timers.h>
#include <utils/Handler.h>
#include <fcntl.h>
#include <termios.h>

//DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);



using namespace std;
using namespace mlib;

class testHandler: public Handler{

public:
	testHandler(string name):Handler(name){}
	void handleMessage(const Message& msg){
		if(msg.mData != NULL){
			INFO("message data=%s",msg.mData);
		}

	}

	~testHandler(){}
private:

protected:


};


int  main(int argc, char* argv[]){

	INFO("mlib version TAG %s",mlibVersionTag.c_str());
	INFO("mlib version Date %s",mlibVersionDate.c_str());
	INFO("mlib version CommitSubject %s",mlibVersionCommitSubject.c_str());
	HandlerHub hub;
	testHandler t1("t1");
	testHandler t2("t2");
	hub.addHandler(&t1);
	hub.addHandler(&t2);
	t1.run();
	t2.run();
	Message msg1,msg2;
	char data1[]="message 1";
	char data2[]="message 2";
	msg1.setData((uint8_t*)data1,10);
	msg2.setData((uint8_t*)data2,10);
	for(;;){

		sleep(1);
		t1.sendMessage(msg1);
		t2.sendMessage(msg2);
		Handler *tmp = t1.getHandler("t2");
		tmp->sendMessage(msg1);
	}
};
