#ifndef UTILS_MLOOPER_H
#define UTILS_MLOOPER_H

#include <Debug.h>
#include <utils/KeyedVector.h>
#include <utils/Timers.h>
#include <string>
using namespace std;
namespace mlib{




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












}

#endif

