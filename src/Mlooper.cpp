#include <utils/Mlooper.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>


namespace mlib {

static const int EPOLL_SIZE_HINT = 8;
static const int EPOLL_MAX_EVENTS = 16;

static pthread_key_t gTLSKey = 0 ;
static pthread_once_t gTLSOnce = PTHREAD_ONCE_INIT;
Mlooper::Mlooper():mSendingMessage(false),mResponseIndex(0),mNextMessageUptime(LLONG_MAX){
	DEBUG("init Mlooper");
	int wakeFds[2];
	int result = pipe(wakeFds);
	if(result != 0 ) ERROR("Could not create wake pip. errno = %d", errno);

	mWakeReadPipeFd = wakeFds[0];
	mWakeWritePipeFd = wakeFds[1];

	result = fcntl(mWakeReadPipeFd, F_SETFL, O_NONBLOCK);
	if(result != 0 )
		ERROR("Could not set read pip non-blocking. errno = %d", errno);

	result = fcntl(mWakeWritePipeFd, F_SETFL, O_NONBLOCK);
	if(result != 0 ) ERROR("Could not set write pip non-blocking. errno = %d", errno);

	mIdling = false;

	mEpollFd = epoll_create(EPOLL_SIZE_HINT);
	if(mEpollFd < 0 ) ERROR("could not create epoll instance. errno = %d",errno);	

	struct epoll_event eventItem;
	memset(&eventItem, 0, sizeof(epoll_event));
	eventItem.events = EPOLLIN;
	eventItem.data.fd = mWakeReadPipeFd;
	result = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mWakeReadPipeFd, &eventItem);
	if(result != 0) ERROR("could not add wake read pipe to epoll instance. errno = %d",errno);
}

Mlooper::~Mlooper(){
	close(mWakeReadPipeFd);
	close(mWakeWritePipeFd);
	close(mEpollFd);
}

void Mlooper::freeTLS(void *mlooper){

	Mlooper* self = static_cast<Mlooper*>(mlooper);
	if(self != NULL) free(self);


}
/*create thread TLS key for bind the looper with thread*/	
void Mlooper::initTLSKey(){
	int result = pthread_key_create(&gTLSKey, freeTLS);
	if(result != 0) ERROR("could not allocate TLS key errno = %d",errno);
}

Mlooper* Mlooper::getMlooperFromThread(){

	int result = pthread_once(&gTLSOnce, initTLSKey);

	return (Mlooper*)pthread_getspecific(gTLSKey);
}

/*bind mlooper with thread. add mlooper ptr to thread TLS*/
void Mlooper::bindThread(const Mlooper* mlooper){

	Mlooper* oldMlooper = getMlooperFromThread();
	
	if(oldMlooper != NULL) INFO("current Mlooper belone to another thread before, will be set for new thread %ld",pthread_self());
	

	pthread_setspecific(gTLSKey, mlooper);
}

Mlooper* Mlooper::prepare(){

	Mlooper* mlooper = Mlooper::getMlooperFromThread();
	if(mlooper == NULL){
		mlooper = new Mlooper();
		Mlooper::bindThread(mlooper);
	}
	return mlooper;
}

void Mlooper::wake(){
	ssize_t nWrite;
	do{
		nWrite = write(mWakeWritePipeFd, "W", 1);
	}while(nWrite == -1 && errno == EINTR);

	if(nWrite != 1){
		if(errno != EAGAIN) ERROR("Could not write wake signal, errno = %d",errno);
		
	}
}

void Mlooper::awoken(){

	char buffer[16];
	ssize_t nRead;
	do{
		nRead = read(mWakeReadPipeFd, buffer, sizeof(buffer));
	}while((nRead == -1 && errno == EINTR) || nRead == sizeof(buffer));
}

int Mlooper::addFd(int fd, int ident, int events, MlooperEventCallback* const &eventCallback, void* data){

	if(ident < 0 ){
		ERROR("Invalid attempt to set callback with ident < 0. ");
		return -1;
	}
	int epollEvents = 0;
	if(events & EVENT_INPUT) epollEvents |= EPOLLIN;
	if(events & EVENT_OUTPUT) epollEvents |= EPOLLOUT;

	{
		/*below operation need guard by lock*/
		AutoMutex lock(mLock);
		
		Request request;
		request.fd = fd;
		request.ident = ident;
		request.eventCallback = eventCallback;
		request.data = data;

		struct epoll_event eventItem;
		memset(&eventItem, 0, sizeof(epoll_event));
		eventItem.events = epollEvents;
		eventItem.data.fd = fd;

		ssize_t requestIndex = mRequests.indexOfKey(fd);
		if(requestIndex < 0){
			int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &eventItem);
			if(epollResult < 0 ) {
				ERROR("adding epoll events for fd %d, errno = %d", fd, errno);
				return -1;
			}
			mRequests.add(fd, request);
		} else {
			
			int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_MOD, fd, &eventItem);
			if(epollResult < 0 ) {
				ERROR("modify epoll events for fd %d, errno = %d", fd, errno);
				return -1;
			}
			mRequests.replaceValueAt(requestIndex, request);
		}


	}// autoMutex release
	return 1;
}

int Mlooper::removeFd(int fd){

	{

		AutoMutex lock(mLock);
		ssize_t requestIndex = mRequests.indexOfKey(fd);
		if(requestIndex < 0){
			INFO("can not find fd");
			return  0;
		}

		int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL);
		if(epollResult < 0 ){
			ERROR("removing epoll events for fd %d, errno = %d", fd, errno);
			return -1;
		}

		mRequests.removeItemsAt(requestIndex);
	}//unlock autoMutex
	
	return 1;
}

void Mlooper::pushResponse(int events, const Request &request){
	
	Response response;
	response.events = events;
	response.request = request;
	mResponses.push(response);
}
void Mlooper::sendMessage(MessageHandler* const &handler, const Message& message){
	DEBUG("sendMessage 1");
	nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
	sendMessageAtTime(now, handler, message);
} 
void Mlooper::sendMessageAtTime(nsecs_t uptime, MessageHandler* const &handler, const Message& message){

	DEBUG("sendMessage2");
	size_t i = 0;
	{
		AutoMutex lock(mLock);

		size_t messageCount = mMessageEnvelopes.size();
		/*sort the message item index according to the uptime*/
		while(i < messageCount && uptime >= mMessageEnvelopes.itemAt(i).uptime){
			i += 1;
		}
		MessageEnvelope messageEnvelope(uptime, handler, message);
		mMessageEnvelopes.insertAt(messageEnvelope, i, 1);

		if(mSendingMessage){
			return;
		}
	}//release AutoMutex lock

	if(i == 0){
	
		wake();
	}
}

int Mlooper::pollOnce(int timeoutMillis){
	int result = 0 ;
	for(;;){
		if(result != 0){
			DEBUG("%p ~ pollOnce - returning result %d", this, result);
		}
		result = pollInner(timeoutMillis);
	}


}


int Mlooper::pollInner(int timeoutMillis){

	if(timeoutMillis !=0 && mNextMessageUptime != LLONG_MAX){
		nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
		int messageTimeoutMillis = toMillisecondTimeoutDelay(now, mNextMessageUptime); //recaculate the next message timeout time 
		if(messageTimeoutMillis >= 0 && (timeoutMillis < 0 || messageTimeoutMillis < timeoutMillis)){
			timeoutMillis = messageTimeoutMillis;
		}
		DEBUG("%p ~ pollOnce next message in %ldns, adjusted timeout: timeoutMillis = %d", this, mNextMessageUptime - now, timeoutMillis);
	}

	int result = POLL_WAKE;
	mResponses.clear(); // all response will event callback will invoke after message sent
	DEBUG("mResponse size %ld",mResponses.size());
	mResponseIndex = 0;

	mIdling = true;

	struct epoll_event eventItems[EPOLL_MAX_EVENTS];
	int eventCount = epoll_wait(mEpollFd, eventItems, EPOLL_MAX_EVENTS, timeoutMillis); 
	/*wait until new epoll events arrive to run below code*/








	DEBUG("get new epoll event");
	mLock.lock();//lock below operation

	if(eventCount < 0){
		if(errno == EINTR){
			goto Done;
		}
		ERROR("%p ~ Poll failed with an unexpected error, errno=%d", this, errno);
		result = POLL_ERROR;
		goto Done;
	}

	if(eventCount == 0){
		INFO("%p ~ pollOnce timeout",this);
		result = POLL_TIMEOUT;
		goto Done;
	}

	DEBUG("%p ~ pollOnce handling events from %d fds", this, eventCount);

	for(int i = 0; i < eventCount; i++){
		/*check all epoll events*/
		int fd = eventItems[i].data.fd;
		uint32_t epollEvents = eventItems[i].events;
		if(fd == mWakeReadPipeFd){
			/*check event from looper read pipe*/
			if(epollEvents & EPOLLIN){
				awoken();
			}else{
				ERROR("unexpected epoll events 0x%x on wake read pipe.", epollEvents);
			}
		}else{
			/*handle event from external added fd*/
			ssize_t requestIndex = mRequests.indexOfKey(fd); //get the request according to events fd
			if(requestIndex >= 0){
				int events = 0;
				if(epollEvents & EPOLLIN) events |= EVENT_INPUT;
				if(epollEvents & EPOLLOUT) events |= EVENT_OUTPUT;
				if(epollEvents & EPOLLERR) events |= EVENT_ERROR;
				if(epollEvents & EPOLLHUP) events |= EVENT_HANGUP;
				DEBUG("push request into response list");
				pushResponse(events, mRequests.valueAt(requestIndex)); //add the event of request into mResponses 
			}else{
				ERROR("unexpected epoll events 0x%x on fd %d that is no longer registered.", epollEvents, fd);
			}

		}
	}

Done: ;

      /*Invoke pending message callbacks.*/
      mNextMessageUptime = LLONG_MAX;
      while(mMessageEnvelopes.size() != 0){
	      nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
	      const MessageEnvelope &messageEnvelope = mMessageEnvelopes.itemAt(0);
	      if(messageEnvelope.uptime <= now){
		      {
			      MessageHandler* handler = messageEnvelope.handler;
			      Message message = messageEnvelope.message;
			      mMessageEnvelopes.removeAt(0);
			      mSendingMessage = true;
			      mLock.unlock();
			      DEBUG("%p ~ pollOnce - sending message to message handler=%p, what=%d", this, handler, message.mWhat);
			      handler->handleMessage(message);// Invoke the Message handler
		      }

		      mLock.lock();
		      mSendingMessage = false;
		      result = POLL_CALLBACK;
	      }else{
		      mNextMessageUptime = messageEnvelope.uptime;
		      break;
	      }
      }

      mLock.unlock();

      /*Invoke all response callbacks.*/
      for(size_t i = 0; i < mResponses.size(); i++){
      	      DEBUG("check response size %ld",mResponses.size());
	      Response &response = mResponses.editItemAt(i);
	      DEBUG("response event ident %d",response.events);
	      //if(response.request.ident == POLL_CALLBACK){
	      if(response.request.ident){
		      int fd = response.request.fd;
		      int events = response.events;
		      void* data = response.request.data;

		      int callbackResult = response.request.eventCallback->handleEvent(fd, events, data);
		      DEBUG("invoke callback handler");
		      if(callbackResult == 0 ){
			      removeFd(fd);
		      }
		      result = POLL_CALLBACK;
	      }
      }
      return result;
}
}
