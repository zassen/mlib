#include <utils/Handler.h>

using namespace mlib;
using namespace std;

Handler::Handler(string name):mName(name),mTimeoutMillis(1000),mMlooper(NULL){

}

Handler::~Handler(){

}

int Handler::setTimeout(int timeout){
	mTimeoutMillis = timeout;
}

status_t Handler::initInThread(){

	return NO_ERROR;
}

int Handler::addListenFd(int fd, int ident, int events){
	int result = 0;
	mMlooper = Mlooper::getMlooperFromThread();
	if(mMlooper == NULL) ASSERT("mMlooper null");
	INFO("mMlooper %p",this->mMlooper);
	result = this->mMlooper->addFd(fd,ident,events,this,this);
	return result;

}
status_t Handler::readyToRun(){
	
	status_t result = NO_ERROR;

	if(mMlooper != NULL) ASSERT("member Mlooper is uninited before use it");

	mMlooper = new Mlooper();
	//this->mMlooper = Mlooper::prepare();
	INFO("Mlooper create:%p",mMlooper);
	if(mMlooper < 0 ) ASSERT("create mlooper fail at handler %s", this->mName.c_str());

	result = initInThread();	

	return result;
	
}

Handler* Handler::self(){
	return this;
}

void Handler::sendMessage(const Message& msg){

	if(mMlooper == NULL) ASSERT("mlooper not create before send message at handler %s",this->mName.c_str());
	INFO("mMlooper %p",this->mMlooper);
	//INFO("sendMessage handler %p",this);
	mMlooper->sendMessage(this,msg);


}
int Handler::handleEvent(int fd, int ident, int events, void* data) {
	return 0;
}


bool Handler::threadLoop(){
	bool result =0;
	INFO("PollOnce start-----------------------");
	result = threadWork();
	Handler::mMlooper->pollOnce(mTimeoutMillis);
	return result;
}

void Handler::setHub(HandlerHub* const hub){
	if(hub == NULL) ASSERT("input NULL hub ptr!");
	mHub = hub;
}

Handler* Handler::getHandler(string name){
	return mHub->getHandler(name);

}

Handler* HandlerHub::getHandler(string name){

	ssize_t handlerIndex = mHub.indexOfKey(name);
	if(handlerIndex < 0){
		INFO("can not find handler %s",name.c_str());
		return NULL;
	}
	return mHub.valueAt(handlerIndex);

}

void HandlerHub::addHandler(Handler* const handler){

	if(handler == NULL){
		ASSERT("input NULL handler ptr!");
	}
	mHub.add(handler->mName, handler);
	handler->setHub(this);

}
