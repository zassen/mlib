#include "Debug.h"
#include "Condition.h"
#include "Mutex.h"

DEBUG_SET_LEVEL(DEBUG_LEVEL_ERROR);
using namespace std;
using namespace mlib;
int main(void){
status_t t=NO_ERROR;
	Mutex mlock;
	int i = 10;
	ERROR("i= %d",i);
	ASSERT("error_status:%d",t);
	return 0;
}
