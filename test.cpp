#include "Debug.h"
#include "Errors.h"

DEBUG_SET_LEVEL(DEBUG_LEVEL_ERROR);
int main(void){
status_t t=NO_ERROR;
	int i = 10;
	ERROR("i= %d",i);
	ASSERT("error_status:%d",t);
	return 0;
}
