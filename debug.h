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

#ifdef _DEBUG
extern void AssertProc(int f,char *file,int line);
#define ASSERT(f) AssertProc(f,__FILE__,__LINE__)
#define TRACE(s)  {printf("\n--%s:%d[",__FILE__,__LINE__);printf s;printf("]\n");}
#else
#define ASSERT(f)
#define TRACE(s)
#endif


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