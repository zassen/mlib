#ifndef UTILS_MLOOPER_H
#define UTILS_MLOOPER_H

#include <Debug.h>
#include <utils/KeyedVector.h>
#include <utils/Timers.h>
#include <string>
#include <sys/epoll.h>
using namespace std;
namespace mlib{


typedef int (*Mlooper_callbackFunc)(int fd, int events, void *datea);

struct Message{

	Message():mWhat(0){}
	Message(int what):mWhat(what) {}
	Message(string str):mStr(str){}
	void 	setData(uint8_t *data,int len){
		memset(&mData,0,128);
		if(data != NULL){

			memcpy(&mData,data,len);

		}
	}
	int mWhat;
	string mStr;
	uint8_t mData[128];

	

};

int MessageTest(const Message &msg)
{


	DEBUG("msg.what=%d",msg.mWhat);

	

}


class Mlooper {

	protected:
		virtual ~Mlooper();
	public:
		MLooper();
		int pollOnce(int timeoutMillis, int* outFd, int* outEvents, void** outData);
		inline int pollOnce(int timeoutMillis){
			return pollOnce(timeoutMillis, NULL, NULL, NULL);
		}

		int pollAll(int timeoutMillis, int* outFd, int* outEvents, void** outData);
		inline int pollAll(int timeoutMillis){
			return pollAll(timeoutMillis, NULL, NULL, NULL);
		}

		void wake();
		int addFd(int fd, int ident, int events, Mlooper_callbackFunc callback, void* data);
		int removeFd(int fd);
		bool isIdling() const;

		static Mlooper* prepare(int opts);

		static void setForThread(const Mlooper* mlooper);

		static Mlooper* getForThread();
	private:
		struct Request{

			int fd;
			int ident;
			MlooperCallback* callback;
			void* data;
		};

		struct Response{
			int evevts;
			Request request;

		};

		struct MessageEnvelope{

			MessageEnveloe(): uptime(0){}

			MessageEnvelope(nsecs_t uptime, const MessageHandler* handler, const Message* message):uptime(uptime),handler(handler).message(message){}
			nsecs_t uptime;
			MessageHandler* handler;
			Message* message;
		};

		int mWakeReadPipeFd;
	        int mWakeWritePipeFd;	
		Mutex mlock;
		size_t mResponseIndex;
		Vector<Response> mResponse;
		KeyedVector<int,Requeset> mRequests;

		int pollInner(int timeoutMillis);
		void awoken();



}












}

#endif

