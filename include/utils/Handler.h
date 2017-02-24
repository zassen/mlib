#ifndef UTILS_HANDLER_H
#define UTILS_HANDLER_H

#include <utils/Mlooper.h>
#include <utils/Thread.h>
#include <utils/KeyedVector.h>
#include <string.h>

namespace mlib{
class HandlerHub;

class Handler : public Thread, public MessageHandler, public MlooperEventCallback {


public:
	Handler(string name);
	virtual	~Handler();
	void sendMessage(const Message& msg);
	int addListenFd(int fd, int ident, int events);
	status_t readyToRun();
	virtual void handleMessage(const Message &message)=0;
	virtual int handleEvent(int fd, int events, void* data) ;
	virtual bool threadLoop();
	virtual bool threadWork() ;
	virtual status_t initInThread(void);
	Handler* self(void);
	void setHub(HandlerHub* const hub);
	Handler* getHandler(string name);
	string mName;
private:

	HandlerHub* mHub;
	Mlooper* mMlooper;
protected:

};

class HandlerHub{

public:
	HandlerHub(){}
	virtual ~HandlerHub(){}
	void addHandler(Handler* const handler);
	Handler* getHandler(string name);
private:
	KeyedVector<string,Handler*> mHub;

protected:



};


}//end of namespace mlib


#endif // end of UNTILS_HANDLER_H
