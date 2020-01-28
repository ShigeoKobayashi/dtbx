/*
 *
 * Debugging tools.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#include "stdafx.h"
#include "debug.h"
#include "templates.h"

#ifdef _DEBUG
void AssertProc(int f,const char *file,int line)
{
	if(f) return;
	printf("** Assertion failed %s:%d **\n",file,line);
	getchar();
}

DEBUG_EXPORT(void) 
	DtxDebug()
{
	TNode<int> *t = NULL;
	TQueue<int> Q(10);
	Q.PushLeft(1);
	Q.PushLeft(2);
	t = Q.PushLeft(3);
	TNode<int>* p = Q.PushLeft(4);
	Q.RemoveNode(t);
	Q.InsertRight(p,3);
	TNode<int> *n5 = Q.PushRight(5);
	int i = Q.PopRight();
	i = Q.PopLeft();
	p = Q.SearchLeft(3,(TNode<int>*)NULL);

	t = NULL;
	while(t = Q.Right(t))
	{
		printf(" %d ",t->Value);
	}

	printf("\n");
	t = NULL;
	while(t = Q.Left(t))
	{
		printf(" %d ",t->Value);
	}

}



#endif
