#ifndef UTILS_MLOOPER_H
#define UTILS_MLOOPER_H

#include <Debug.h>
#include <utils/KeyedVector.h>
#include <utils/Timers.h>

namespace mlib{




struct Message{

	Message():what(0){}
	Message(int w):what(w) {}
	int what;

};

int MessageTest(const Message &msg)
{


	DEBUG("msg.what=%d",msg.what);


}












}

#endif

