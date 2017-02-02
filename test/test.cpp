#include <Debug.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>
#include <utils/Mlooper.h>
#include <utils/Timers.h>
#include <fcntl.h>
#include <termios.h>

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
class CallbackHandler:MlooperEventCallback{
public:
	CallbackHandler(){
	int pipeFd[2];
	pipe(pipeFd);
	mRead = pipeFd[0];
	mWrite = pipeFd[1];
	fcntl(mRead, F_SETFL, O_NONBLOCK);
	fcntl(mWrite, F_SETFL, O_NONBLOCK);
	mttyFd = open("/dev/ttyUSB0",O_RDWR|O_NDELAY|O_NOCTTY); 
	fcntl(mttyFd, F_SETFL, 0);
	setTTY();
	}
	void setCallback(Mlooper* const mlooper, int fd, int events){
		mlooper->addFd(fd,2,events,this,this);
	}
	int mRead;
	int mttyFd;
	int writePipe(void){

		ssize_t nWrite;
		do{
			nWrite = write(mWrite,"w",1);
		}while(nWrite == -1 && errno == EINTR);
		DEBUG("write pipe");
	}
	int setTTY(void){
		termios portSetting;
		bzero(&portSetting, sizeof(portSetting));
		cfmakeraw(&portSetting);
		cfsetispeed(&portSetting,B115200);
		cfsetospeed(&portSetting,B115200);
		portSetting.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		portSetting.c_oflag &= ~OPOST;
		portSetting.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		portSetting.c_cflag &= ~CSIZE;
		portSetting.c_cflag |= CS8;
		portSetting.c_cflag |= CLOCAL | CREAD;
		portSetting.c_cflag &= ~(PARENB);
		portSetting.c_iflag |= IGNPAR;
		portSetting.c_cflag &= ~(CSTOPB);
		portSetting.c_cflag &= ~(CRTSCTS);

		tcsetattr(mttyFd, TCSANOW, &portSetting);

	}
	~CallbackHandler(){
		close(mttyFd);
	}
protected:

	 //~CallbackHandler();
private:
	int mWrite;
	virtual int handleEvent(int fd, int events, void* data){
		char buffer[128]={0};
		ssize_t nRead;
		ssize_t count=0;
		DEBUG("ENTER callback handler fd=%d, event =%d",fd,events);
		do{
		nRead = read(fd, buffer, sizeof(buffer));
		count += nRead;
		}while((nRead == -1 && errno == EINTR) || nRead == sizeof(buffer));
		DEBUG(" callback handler count = %ld,data=%s ", count, buffer);
	}

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
	CallbackHandler handler1;

	//DEBUG("Msg data %s",msg.mData);
//	MessageTest(Message(10));
//	*id = (tx1) i;
	//INFO("id %x , i %x ,*id %x",id,i,*id);
	t t1;
//sleep(10);
	t1.run();
INFO("test process id %lx",pthread_self());
sleep(2);
//handler1.setCallback(t1.mLooper,handler1.mRead,Mlooper::EVENT_INPUT);
handler1.setCallback(t1.mLooper,handler1.mttyFd,Mlooper::EVENT_INPUT);
for(;;){

//	handler1.writePipe();
//	handler1.writePipe();
//	handler1.writePipe();
	sleep(2);
	//t1.msgHandler.sendMessage(t1.mLooper,&msg);
	//msg.mWhat++;
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
