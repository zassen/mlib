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
	Handler(string name):mName(name),mMlooper(0){}
	void sendMessage(const Message& msg);
	status_t readyToRun();
	bool threadLoop() = 0;
	virtual void handleMessage(const Message &message)=0;
	virtual int handleEvent(int fd, int events, void* data) = 0;
	virtual bool threadWork() = 0;
	virtual status_t initInThread(void) = 0;
	Handler* self(void);
private:

	HandlerHub* mHub;
	Mlooper* mMlooper;
	string mName;
protected:
	virtual	~Handler();

};

class HandlerHub{

public:
	HandlerHub();
	int addHandler(Handler* const handler);
	Handler* getHandler(string name);

private:
	KeyedVector<string,Handler*> mHub;

protected:
	virtual ~HandlerHub();



};


}//end of namespace mlib


#endif // end of UNTILS_HANDLER_H
