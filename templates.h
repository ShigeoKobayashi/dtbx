/*
 *
 * C++ template library
 *
 *   Templates for 
 *    memory allocation (TMemory),
 *    array handling (TArray),and 
 *    stack operation (TStack).
 *    queue operation (TQueue)
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 *   Note: g++ requires to set '::' and 'this->' like following examples
 *         which MS-C++ does not!
 *
 *       	TArray(int mx) : TArray::TMemory(mx)
 *	        T& operator [](int i) { return this->Address()[i]; };
 *
 */

#ifndef __INC_TEMPLATE_H__
#define __INC_TEMPLATE_H__

#define ERROR_MEMORY_ALLOC -99
#include <malloc.h>
#include <memory.h>
#include "debug.h"

//
// Memory management
template <typename T> 
class TMemory 
{

public:
	TMemory(int mx)
	{
		m_bytes   = 0;
		m_pmemory = NULL;
		if(mx<=0) throw new CException(-1,"ERROR[TMemory]: Zero or negative memory size.");
		m_pmemory = MemAlloc(mx*sizeof(T));
		m_bytes   = mx*sizeof(T);
	};

	~TMemory()
	{
		MemFree();
	};

	T *MemAlloc(int s)
	{
		if(m_pmemory!=NULL) {
			throw new CException(ERROR_MEMORY_ALLOC,"MemAlloc(),already allocated.");
		}
		void *p = malloc(s*sizeof(T));
		if(!p) {
			throw new CException(ERROR_MEMORY_ALLOC,"MemAlloc(),memory allocation failed.");
		}
		m_bytes   = s*sizeof(T);
		memset(p,0,m_bytes);
		m_pmemory = (T*)p;
		return m_pmemory;
	};

	T *MemReAlloc(int s)
	{
		if(m_pmemory==NULL) {
			throw new CException(ERROR_MEMORY_ALLOC,"MemReAlloc(),not yet allocated allocated.");
		}
		s = s*sizeof(T);
		void *p = realloc(m_pmemory,s);
		if(!p) {
			throw new CException(ERROR_MEMORY_ALLOC,"MemReAlloc() failed.");
		}
		m_bytes   = s;
		m_pmemory = (T*)p;
		return m_pmemory;
	};

	void MemFree()
	{
		if(m_pmemory==NULL) return;
		free(m_pmemory);
		m_pmemory = NULL;
		m_bytes   = 0;
	};

	T     *Address() {return m_pmemory;};
	int ByteSize(){return m_bytes;}; 

private:
	int m_bytes;
	T   *m_pmemory;
};

//
// Array handling
template <typename T> 
class TArray: private TMemory <T>
{

public:
	TArray(int mx) : TArray::TMemory(mx)
	{
		m_Size   = mx;
	};

	~TArray()
	{
		m_Size   = 0;
	};

	T& operator [](int i) { return this->Address()[i]; };

	int Resize(int mx)
	{
		if(mx>0) {
			this->MemReAlloc(mx);
			m_Size   = mx;
		}
		return m_Size;
	};

	void Set(int i,T v)
    	{
	   if( i<0 || i>=m_Size) throw new CException(-1,"Index out of range,TArray.Set()");
	   this->Address()[i] = v;
    	};

	T Get(int i)
	{
		if(i<0 || i>=m_Size) throw new CException(-1,"Index out of range,TArray.Get()");
		return this->Address()[i];
	};

	void Clear(T v)  {
		for(int i=0;i<m_Size;++i) this->Address()[i]=v;
	};
	int  Size()  {return m_Size;};
	T   *Array() {return this->Address();};

private:
	int  m_Size;
};

//
// Stack operation
template <typename T>
class TStack: public TArray <T> {

public:
	TStack(int mx,int incr):TStack::TArray(mx)
	{
		m_iCount = 0;
		if(incr>0) m_nIncr   = incr;
		else       m_nIncr   = 10;
	};

	TStack(int mx):TStack::TArray(mx)
	{
		m_iCount    = 0;
		m_nIncr     = mx/100 + 10;
	};

	/*
	TStack():TStack::TArray(10)
	{
		m_iCount    = 0;
		m_nIncr     = 10;
	};
	*/

	~TStack()
	{
	};

	T * Top()
	{
		if(m_iCount<=0) return NULL;
		return &((this->Array()[m_iCount-1]));
	}

	int Push(T v)
	{
		if(m_iCount>=this->Size()) {
			this->Resize(this->Size() + (this->Size()*m_nIncr)/100 + 1);
		}
		this->Set(m_iCount++,v);
		return m_iCount;
	};

	T   Pop()
	{
		if(m_iCount<=0)  {
			throw new CException(-1,"Pop(): underflow");
		}
		return this->Get(--m_iCount);
	};

	int    Count()     {return m_iCount;};
	int    Increment() {return m_nIncr;};

private:
	int         m_nIncr   ;  // Increment % size of the array when it is extended.
	int         m_iCount;
};


//
// Queue operation
//
template <typename T>
class TNode {

	template <typename T> friend class TQueue;

public:
	TNode(T v)
	{
		this->Value = v;
		pLeft  = (TNode<T> *)(-1);
		pRight = (TNode<T> *)(-1);
	};

	TNode<T> *Left()  {return this->pLeft;};
	TNode<T> *Right() {return this->pRight;};

public:
	T         Value;

private:
	TNode<T> *pLeft;
	TNode<T> *pRight;
};

template <typename T>
class TQueue
//	: private TStack< TNode<T> >  <== MS-VC compiles this of course,but g++ never !!!!
{

public:
	// mx: initial queue slot size.
	TQueue(int mx)   //		:TQueue::TStack<TNode<T>>(mx) g++ never arrow this !!! gave up...
	{
		m_pRightMost = (TNode<T>*)NULL;
		m_pLeftMost  = (TNode<T>*)NULL;
		m_pDeleted   = (TNode<T>*)NULL;
	};

   ~TQueue()
	{
		TNode<T>* pNext;
		TNode<T>* pNode = m_pDeleted;
		while(pNode!=(TNode<T>*)NULL) {
			pNext = pNode->pLeft;
			delete pNode;
			pNode = pNext;
		}
		pNode = m_pLeftMost;
		while(pNode!=(TNode<T>*)NULL) {
			pNext = pNode->pLeft;
			delete pNode;
			pNode = pNext;
		}
	};

	// returns the right most node in queue.
    TNode<T> *Right()
	{
		return this->m_pRightMost;
	};

	// returns the left most node in queue.
    TNode<T> *Left()
	{
		return this->m_pLeftMost;
	};

	// returns the right node of pNow in queue.
	// If pNow==NULL,then the right most node is retured.
	TNode<T> *Right(TNode<T> *pNow)
	{
		if(pNow==(TNode<T>*)NULL) return this->m_pLeftMost;
		return pNow->pRight;
	};

	// returns the left node of pNow in queue.
	// If pNow==NULL,then the left most node is retured.
	TNode<T> *Left (TNode<T> *pNow)
	{
		if(pNow==(TNode<T>*)NULL) return this->m_pRightMost;
		return pNow->pLeft;
	};

	//
	// Search v from pFrom to right direction.
	// If pFrom==NULL,then searching begins from the left most position.
	// Note '==' operator must be defined.
	//
	TNode<T> *SearchRight(T v,TNode<T> *pFrom)
	{
		TNode<T> *pNow = pFrom;
		if(pNow==(TNode<T> *)NULL) pNow = this->m_pLeftMost;
		this->CheckNode(pNow);
		while(pNow!=NULL) {
			if(v==pNow->Value) return pNow;
			pNow = pNow->pRight;
		}
		return (TNode<T>*)NULL;
	};

	//
	// Search v from pFrom to left direction.
	// If pFrom==NULL,then searching begins from the right most position.
	// Note '==' operator must be defined.
	//
	TNode<T> *SearchLeft(T v,TNode<T> *pFrom)
	{
		TNode<T> *pNow = pFrom;
		if(pNow==(TNode<T> *)NULL) pNow = this->m_pRightMost;
		this->CheckNode(pNow);
		while(pNow!=NULL) {
			if(v==pNow->Value) return pNow;
			pNow = pNow->pLeft;
		}
		return (TNode<T>*)NULL;
	};

	//
	// Pop the left most value.
	T PopLeft()
	{
		TNode<T> *pNode = this->m_pLeftMost; 
		if(pNode==(TNode<T>*)NULL) throw new CException(-3,"ERROR[PopLeft()]: No node in the queue.");
		this->m_pLeftMost = pNode->pRight;
		if(pNode->pRight!=(TNode<T>*)NULL) (pNode->pRight)->pLeft = (TNode<T>*)NULL;
		this->AddRemoveList(pNode);
		return pNode->Value;
	}

	//
	// Pop the right most value.
	T PopRight()
	{
		TNode<T> *pNode = this->m_pRightMost; 
		if(pNode==(TNode<T>*)NULL) throw new CException(-3,"ERROR[PopLeft()]: No node in the queue.");
		this->m_pRightMost = pNode->pLeft;
		if(pNode->pLeft!=(TNode<T>*)NULL) (pNode->pLeft)->pRight = (TNode<T>*)NULL;
		this->AddRemoveList(pNode);
		return pNode->Value;
	}

	// 
	// Remove the specified node by it's index in the queue list.
	// ix is obtained by (TNode<T>).Index.
	void RemoveNode(TNode<T> *pNode)
	{
		if(this->m_pLeftMost==(TNode<T>*)NULL)  throw new CException(-2,"ERROR[RemoveNode()]: Specified node not in the queue.");
		this->CheckNode(pNode);
		if(pNode==this->m_pLeftMost)  {this->PopLeft();  return;}
		if(pNode==this->m_pRightMost) {this->PopRight(); return;}

		// reset list pointers
		(pNode->pLeft)->pRight = pNode->pRight;
		(pNode->pRight)->pLeft = pNode->pLeft;

		this->AddRemoveList((TNode<T>*)pNode);
	}

	//
	// Push v to left side of the queue.
	TNode<T> *PushLeft(T v)
	{
		TNode<T> *pNode = this->GetNode(v);

		// Setup linked list
		if(this->m_pLeftMost==(TNode<T>*)NULL) {
			ASSERT(this->m_pRightMost==(TNode<T>*)NULL);
			pNode->pLeft   = (TNode<T>*)NULL;
			pNode->pRight  = (TNode<T>*)NULL;
			this->m_pLeftMost  = pNode;
			this->m_pRightMost = pNode;
		} else {
			ASSERT(this->m_pRightMost!=(TNode<T>*)NULL);
			ASSERT(this->m_pLeftMost->pLeft==(TNode<T>*)NULL);
			this->m_pLeftMost->pLeft = pNode;
			pNode->pLeft   = (TNode<T>*)NULL;
			pNode->pRight  = this->m_pLeftMost;
			this->m_pLeftMost        = pNode;
		}
		return pNode;
	};

	//
	// Push v to right side of the queue.
	TNode<T> *PushRight(T v)
	{
		TNode<T> *pNode = this->GetNode(v);
		// Setup linked list
		if(m_pRightMost==(TNode<T>*)NULL) {
			ASSERT(m_pLeftMost==(TNode<T>*)NULL);
			pNode->pLeft  = (TNode<T>*)NULL;
			pNode->pRight = (TNode<T>*)NULL;
			m_pLeftMost       = pNode;
			m_pRightMost      = pNode;
		} else {
			ASSERT(m_pLeftMost!=(TNode<T>*)NULL);
			ASSERT(m_pRightMost->pRight==(TNode<T>*)NULL);
			m_pRightMost->pRight = pNode;
			pNode->pRight    = (TNode<T>*)NULL;
			pNode->pLeft     = m_pRightMost;
			m_pRightMost     = pNode;
		}
		return pNode;
	};

	TNode<T> *InsertLeft(TNode<T> *pNode,T v)
	{
		if(this->m_pLeftMost==(TNode<T>*)NULL) throw new CException(-2,"ERROR[InsertLeft()]: Specified node not in the queue.");
		this->CheckNode(pNode);
		if(pNode==this->m_pLeftMost)  return this->PushLeft(v);
		TNode<T> * pNew = this->GetNode(v);
		pNew->pRight = pNode;
		pNew->pLeft  = pNode->pLeft;
		(pNode->pLeft)->pRight = pNew;
		pNode->pLeft           = pNew;
		return pNew;
	};

	TNode<T> *InsertRight(TNode<T> *pNode,T v)
	{
		if(this->m_pRightMost==(TNode<T>*)NULL) throw new CException(-2,"ERROR[InsertRight()]: Specified node not in the queue.");
		this->CheckNode(pNode);
		if(pNode==this->m_pRightMost)  return this->PushRight(v);
		TNode<T> * pNew = this->GetNode(v);
		pNew->pLeft  = pNode;
		pNew->pRight = pNode->pRight;
		(pNode->pRight)->pLeft = pNew;
		pNode->pRight          = pNew;
		return pNew;
	};

private:
	void CheckNode(TNode<T> *pNode)
	{
		if(pNode==(TNode<T>*)NULL || pNode->pLeft==(TNode<T>*)(-1))
			throw new CException(-2,"ERROR[CheckNode()]: Invalid node.");
	};

	TNode<T> *GetNode(T v)
	{
		TNode<T> *pNode = (TNode<T>*)NULL; 
		// Get available node 
		if(this->m_pDeleted==(TNode<T>*)NULL) {
			pNode = new TNode<T>(v);
		} else {
			ASSERT(this->m_pDeleted->pRight==(TNode<T>*)(-1));
			pNode = this->m_pDeleted;
			m_pDeleted   = m_pDeleted->pLeft;
			pNode->Value = v;
		}
		pNode->pLeft  = (TNode<T>*)NULL;
		pNode->pRight = (TNode<T>*)NULL;
		return pNode;
	};

	void AddRemoveList(TNode<T>* pNode)
	{
		// Set removed flag.
		pNode->pRight = (TNode<T>*)(-1);
		// Add to deleted list.
		pNode->pLeft = this->m_pDeleted;
		this->m_pDeleted = pNode;
	};

private:
	TNode <T>         *m_pLeftMost;
	TNode <T>         *m_pRightMost;
	TNode <T>         *m_pDeleted;
};

#endif
