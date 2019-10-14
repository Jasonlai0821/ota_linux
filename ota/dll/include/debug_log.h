#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

#ifdef _USE_LINUX
#include "linux/kernel.h"
#include <stdio.h>
#define DEBUG 1
#endif

#define DEBUG_EMERG  		0    /* system is unusable */  
#define DEBUG_ALERT			1    /* action must be taken immediately */  
#define DEBUG_CRIT   			2    /* critical conditions */  
#define DEBUG_ERROR    		3    /* error conditions */  
#define DEBUG_WARNING  	4    /* warning conditions */  
#define DEBUG_NOTICE 		5    /* normal but significant condition */  
#define DEBUG_INFO   			6    /* informational */  
#define DEBUG_AL  				7    /* debug-level messages */  

static unsigned long DebugLevel = DEBUG_INFO;

#ifdef DEBUG
#define DEBUGPRINT_RAW(Level, Fmt)    \
do{                                   \
    if (Level <= DebugLevel)      \
    {                               \
        printf Fmt;               \
    }                               \
}while(0)
		
#define DEBUGPRINT(Level,Fmt) DEBUGPRINT_RAW(Level,Fmt)

#define DEBUGPRINT_ERR(Fmt)         \
{                                   \
    printf("ERROR!!! ");          \
    printf Fmt;                  \
}

#define DEBUGPRINT_S(Status, Fmt)        \
{                                    \
    printf Fmt;                    \
}

#else

#define DEBUGPRINT(Level, Fmt)
#define DEBUGPRINT_RAW(Level, Fmt)
#define DEBUGPRINT_S(Status, Fmt)
#define DEBUGPRINT_ERR(Fmt)
	
#endif
#endif