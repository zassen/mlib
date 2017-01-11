#ifndef _DEBUG_H_
#define _DEBUG_H_

#define CONFIG_DEBUG_ENABLE
enum debug_level{
	DEBUG_LEVEL_OFF = 0,
	DEBUG_LEVEL_ERROR,
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_DEBUG
};

#ifdef CONFIG_DEBUG_ENABLE 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PRINT printf

#define DEBUG_SET_LEVEL(x) static int debug = x

#define ASSERT(fmt,...)				\
						\
do{						\
	PRINT("ASSERT:%s:%s:%d ::" fmt"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(1)				\
	{					\
		exit(-1);			\
	};					\
}while(0)				

#define ERROR(fmt,...)				\
do {						\
	if(debug >= DEBUG_LEVEL_ERROR){		\
	PRINT("ERROR:%s:%s:%d ::" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define INFO(fmt,...)				\
do {						\
	if(debug >= DEBUG_LEVEL_INFO){		\
	PRINT("INFO:%s:%s:%d ::" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#define DEBUG(fmt,...)				\
do {						\
	if(debug >= DEBUG_LEVEL_DEBUG){		\
	PRINT("DEBUG:%s:%s:%d ::" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	}					\
}while(0)

#else  /* CONFIG_DEBUG_ENABLE */ 

#define DEBUG_SET_LEVEL(x)
#define ASSERT(fmt,...)
#define ERROR(fmt,...)
#define INFO(fmt,...)
#define DEBUG(fmt,...)

#endif /*CONFIG_DEBUG_ENABLE*/

#endif /*_DEBUG_H_*/
