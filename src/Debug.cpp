#include <Debug.h>

const version mlibVersion = {VERSION_TAGS,VERSION_DATE};

char date[30];
char * getDate(void){


	memset(date,0,30);
	time_t timer ;
	struct tm* timeinfo;
	time(&timer);
	timeinfo = localtime(&timer);
	strftime(date,29,"%Y%m%d-%T",localtime(&timer));
	return date;




}