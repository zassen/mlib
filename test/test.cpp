#include <Debug.h>
#include <Version.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>
#include <utils/Mlooper.h>
#include <utils/Timers.h>
#include <utils/Handler.h>
#include <utils/RingBuffer.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>

//DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);



using namespace std;
using namespace mlib;

class testHandler: public Handler{

public:
	testHandler(string name):Handler(name){}
	void messageHandler(const Message& msg){
		if(msg.mData != NULL){
			INFO("message data=%s",msg.mData);
		}
	}

	~testHandler(){}
private:

protected:


};

class testHandler2 {

public:
	testHandler2(){};
	void sendmsg(Handler *handler){
		INFO("TEST HANDLER2");
	Message msg1;
	char data1[]="TEST H2";
	msg1.setData((uint8_t*)data1,10);
		handler->sendMessage(msg1);
	};

	~testHandler2(){}
private:

protected:


};

int  main(int argc, char* argv[]){
	RingBuffer buffer(64);
	int testvalue;
	char testa = 0xff;

	INFO("testa=%d buffer size:%d",testa, buffer.size());
	unsigned char testData[1]={0x45};
	unsigned char halfFull[25] = "123456789012345678901234";
	unsigned char readData[24] = {0};
	INFO("sizeof halfFull:%d",sizeof(halfFull));
	buffer.write(halfFull,sizeof(halfFull));
	buffer.read(readData,sizeof(readData));
	INFO(" availableRead:%d, availableWrite:%d", buffer.availableRead(), buffer.availableWrite());
	for(int i=0;i<24;i++){

		INFO("data6[%d]:%c",i, readData[i]);
	}
	buffer.write(testData,sizeof(testData));
	INFO("availableRead:%d, availableWrite:%d", buffer.availableRead(), buffer.availableWrite());
	testvalue = buffer.findSymbol('E');
	buffer.read(readData,sizeof(unsigned char)*(testvalue+1));
	INFO("testvalue%d availableRead:%d, availableWrite:%d",testvalue, buffer.availableRead(), buffer.availableWrite());
	for(int i=0;i<testvalue;i++){

		INFO("data6[%d]:%c",i, readData[i]);
	}
	INFO("testvalue%d availableRead:%d, availableWrite:%d",testvalue, buffer.availableRead(), buffer.availableWrite());
	buffer.write(halfFull,sizeof(halfFull));
	buffer.write(testData,1);
	INFO("testvalue%d availableRead:%d, availableWrite:%d",testvalue, buffer.availableRead(), buffer.availableWrite());
	testvalue = buffer.findSymbol(0x45);
	memset(readData,0,sizeof(readData));
	buffer.read(readData,sizeof(unsigned char)*(testvalue));
	INFO("testvalue%d availableRead:%d, availableWrite:%d",testvalue, buffer.availableRead(), buffer.availableWrite());
	for(int i=0;i<testvalue;i++){

		INFO("data6[%d]:%c",i, readData[i]);
	}

	cout<<"HelloWorld!"<<endl;
	HandlerHub hub;
	testHandler t1("t1");
	testHandler t2("t2");
	testHandler2 test2;
	hub.addHandler(&t1);
	hub.addHandler(&t2);
	t1.run();
	t2.run();
	Message msg1,msg2;
	char data1[]="message 1";
	char data2[]="message 2";
	msg1.setData((uint8_t*)data1,10);
	msg2.setData((uint8_t*)data2,10);
	for(int i=0; i < 10; i++){

		sleep(1);
		t1.sendMessage(msg1);
		t2.sendMessage(msg2);
		Handler *tmp = t1.getHandler("t2");
		tmp->sendMessage(msg1);
		test2.sendmsg(&t1);
	}
	
	ASSERT("test assert");
};
