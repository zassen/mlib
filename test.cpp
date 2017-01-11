#include "debug.h"

DEBUG_SET_LEVEL(DEBUG_LEVEL_ERROR);
int main(void){

	int i = 10;
	ERROR("i= %d",i);
	ASSERT("xxxx");
	return 0;
}
