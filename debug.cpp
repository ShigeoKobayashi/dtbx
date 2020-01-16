/*
 *
 * Debugging tools.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#include "stdafx.h"

#ifdef _DEBUG
void AssertProc(int f,const char *file,int line)
{
	if(f) return;
	printf("** Assertion failed %s:%d **\n",file,line);
	getchar();
}
#endif
