#ifndef _DEBUG_H_
#define _DEBUG_H_
#include <Version.h>
#include <string>

#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"

#define BOLD                 "\e[1m"
#define UNDERLINE            "\e[4m"
#define BLINK                "\e[5m"
#define REVERSE              "\e[7m"
#define HIDE                 "\e[8m"
#define CLEAR                "\e[2J"
#define CLRLINE              "\r\e[K" //or "\e[1K\r"
typedef struct _version{
	
	std::string Tag;
	std::string Date;
}version;

extern const version mlibVersion ;
extern version appVersion ;

#define CONFIG_DEBUG_ENABLE

enum debug_level{
	DEBUG_LEVEL_OFF = 0,
	DEBUG_LEVEL_ERROR,
	DEBUG_LEVEL_WARN,
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_TRACE
};

#ifdef CONFIG_DEBUG_ENABLE 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//static char date[30];
  char * getDate(void);
//
//
//	memset(date,0,30);
//	time_t timer ;
//	struct tm* timeinfo;
//	time(&timer);
//	timeinfo = localtime(&timer);
//	strftime(date,29,"%Y%m%d-%T",localtime(&timer));
//	return date;
//
//
//
//
//}
#define PRINT printf
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
extern int debugLevel;
//#define DEBUG_SET_LEVEL(x)  debugLevel = x;
void debugSetLevel(int level);

void hexDump(const void *_data, size_t size);
#define ASSERT(fmt,...)				\
						\
do{						\
	PRINT("%s  ASSERT@%s:%s:%d>>" fmt"\n",getDate(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	PRINT("MLIB	-->TAG:%s DATE:%s\n",mlibVersion.Tag.c_str(), mlibVersion.Date.c_str());\
	PRINT("APP	-->TAG:%s DATE:%s\n" NONE,appVersion.Tag.c_str(), appVersion.Date.c_str());\
	while(1)				\
	{					\
		exit(-1);			\
	};					\
}while(0)				

#define ERROR(fmt,...)				\
do {						\
	if(debugLevel >= DEBUG_LEVEL_ERROR){		\
	PRINT("%s  ERROR[%s:%s:%d]\r\n" RED fmt "\n" NONE,getDate(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define WARN(fmt,...)				\
do {						\
	if(debugLevel >= DEBUG_LEVEL_WARN){		\
	PRINT("%s  WARN[%s:%s:%d]\r\n" BROWN fmt "\n" NONE,getDate(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define INFO(fmt,...)				\
do {						\
	if(debugLevel >= DEBUG_LEVEL_INFO){		\
	PRINT("%s  INFO[%s:%s:%d]\r\n" BLUE fmt "\n" NONE,getDate(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define TRACE(fmt,...)				\
do {						\
	if(debugLevel >= DEBUG_LEVEL_TRACE){		\
	PRINT("%s  TRACE[%s:%s:%d]\r\n" GREEN fmt "\n" NONE,getDate(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define DUMPHEX(a,b)				\
do {						\
	if(debugLevel >= DEBUG_LEVEL_TRACE){		\
	PRINT("%s  TRACE[%s:%s:%d]\r\n",getDate(),__FILENAME__,__FUNCTION__,__LINE__);			\
	hexDump(a,b); \
	PRINT("\r\n" NONE); \
	}					\
}while(0)
#else  /* CONFIG_DEBUG_ENABLE */ 

#define DEBUG_SET_LEVEL(x)
#define ASSERT(fmt,...)
#define ERROR(fmt,...)
#define WARN(fmt,...)
#define INFO(fmt,...)
#define TRACE(fmt,...)
#define DUMPHEX(a,b)				

#endif /*CONFIG_DEBUG_ENABLE*/


#endif /*_DEBUG_H_*/
