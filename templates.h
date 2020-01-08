/*
 *
 * C++ template library
 *
 *   Templates for 
 *    memory allocation (TMemory),
 *    array handling (TArray),and 
 *    stack operation (TStack).
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */

#ifndef __INC_TEMPLATE_H__
#define __INC_TEMPLATE_H__

#define ERROR_MEMORY_ALLOC -99
#include <malloc.h>
#include <memory.h>
#include "debug.h"

template <typename T> 
class TMemory 
{

public:
	TMemory(int mx)
	{
		m_bytes = 0;
		if(mx<=0) throw new CException(-1,"ERROR[TMemory]: Zero or negative memory size.");
		m_pmemory = MemAlloc(mx*sizeof(T));
		m_bytes = mx*sizeof(T);
	};

	~TMemory()
	{
		MemFree();
	};

	T *MemAlloc(size_t s)
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

	T *MemReAlloc(size_t s)
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
	size_t ByteSize(){return m_bytes;}; 

private:
	size_t m_bytes;
	T   *m_pmemory;
};

template <typename T> 
class TArray: public TMemory <T>
{

public:
	TArray(size_t mx) : TMemory(mx)
	{
		m_Size   = mx;
	};

	~TArray()
	{
		m_Size   = 0;
	};

	T& operator [](int i) { return Address()[i]; };

	size_t Resize(size_t mx)
	{
		if(mx>0) {
			MemReAlloc(mx);
			m_Size   = mx;
		}
		return m_Size;
	};

	void Set(int i,T v)
    {
	   if( i<0 || i>=m_Size) throw new CException(-1,"Index out of range,TArray.Set()");
	   Address()[i] = v;
    };

	T Get(int i)
	{
		if(i<0 || i>=m_Size) throw new CException(-1,"Index out of range,TArray.Get()");
		return Address()[i];
	};

	void Clear(T v)  {
		for(int i=0;i<m_Size;++i) Address()[i]=v;
	};
	int  Size()  {return m_Size;};
	T   *Array() {return Address();};

private:
	int  m_Size;
};

template <typename T>
class TStack: public TArray <T> {

public:
	TStack(int mx,int incr):TArray(mx)
	{
		m_iCount = 0;
		if(incr>0) m_nIncr   = incr;
		else       m_nIncr   = 10;
	};

	TStack(int mx):TArray(mx)
	{
		m_iCount    = 0;
		m_nIncr     = mx/100 + 10;
	};

	TStack():TArray(10)
	{
		m_iCount    = 0;
		m_nIncr     = 10;
	};

	~TStack()
	{
	};

	T * Top()
	{
		if(m_iCount<=0) return NULL;
		return &((Array()[m_iCount-1]));
	}

	int Push(T v)
	{
		if(m_iCount>=Size()) {
			Resize(Size() + (Size()*m_nIncr)/100 + 1);
		}
		Set(m_iCount++,v);
		return m_iCount;
	};

	T   Pop()
	{
		if(m_iCount<=0)  {
			throw new CException(-1,"Pop(): underflow");
		}
		return Get(--m_iCount);
	};

	int    Count()     {return m_iCount;};
	int    Increment() {return m_nIncr;};

private:
	int         m_nIncr   ;  // Increment % size of the array when it is extended.
	int         m_iCount;
};

#endif