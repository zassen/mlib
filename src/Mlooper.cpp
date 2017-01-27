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
Mlooper::Mlooper(){
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

void Mlooper::sendMessage(MessageHandler* const &handler, Message* const &message){
	nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
	sendMessageAtTime(now, handler, message);
} 
void Mlooper::sendMessageAtTime(nsecs_t uptime, MessageHandler* const &handler, Message* const &message){

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

}
