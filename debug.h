/*
 *
 * Debugging tools.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#ifndef __INC_DEBUG__
#define  __INC_DEBUG__

#include <stdio.h>

#ifdef _WIN32
#ifndef WINDOWS
    #define WINDOWS
    #ifdef _WIN64
        /* 64bit Windows */
    #define BIT64
    #else
        /* 32bit Windows */
    #define BIT32
    #endif
#endif
#else
#ifndef LINUX
    #define LINUX
    #ifdef __x86_64__
            /* 64bit Linux */
        #define BIT64
       #else
           /* 32bit Linux */
        #define BIT32
    #endif
#endif
#endif

#ifdef _DEBUG
extern void AssertProc(int f,const char *file,int line);
#define ASSERT(f) AssertProc(f,__FILE__,__LINE__)
#define TRACE(s)  {printf("\n--%s:%d[",__FILE__,__LINE__);printf s;printf("]\n");}
#else
#define ASSERT(f)
#define TRACE(s)
#endif

#ifdef WINDOWS /**** WINDOWS ****/
#ifndef DEBUG_EXPORT
 #define DEBUG_EXPORT(t)  __declspec(dllexport) t __stdcall
#endif
#endif /**** WINDOWS ****/

#ifdef LINUX /******** LINUX ********/
#ifndef DEBUG_EXPORT
 #define DEBUG_EXPORT(t) __attribute__((visibility ("default")))  t
#endif
#endif /**** LINUX ****/

/*
  Error handling.
*/
#define CException(a,s) CDbgException(a,s,__FILE__,__LINE__)


class CDbgException
{
public:
	CDbgException(int code,const char *inf,const char *file,int line)
	{
#ifdef _DEBUG
		printf(" ***** CDbgException(%d(%s),%s,%d) ******\n",code,inf,file,line);
		getchar();
#endif
		ecode = code;
		info  = inf;
	};

	~CDbgException()
	{
	};

public:
	int         ecode;
	const char *info ;
};

#endif
