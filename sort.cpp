/*
 *
 * DTBX: Digital Toolbox (C/C++ Library).
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */

#include "stdafx.h"


// Used in DtxQuickSort
typedef struct {
		void *a;
		int  (*comp)(void *,int,int);
		void (*swap)(void *,int,int);
} QUICK_SORT_INFO;

static 
int PivDivide(QUICK_SORT_INFO *a, int left, int right) {
	int i, j, pivot;
	i = left;
	j = right + 1;
	pivot = left;

	do {
		while ((*a->comp)(a->a,++i,pivot)<0);
		while ((*a->comp)(a->a,pivot,--j)<0);
		if (i < j) (*a->swap)(a->a,i,j);
	} while (i < j);
	(*a->swap)(a->a,pivot,j);
	return j;
}

static
void DoQuickSort(QUICK_SORT_INFO *a, int left, int right) {
  int pivot;
  if (left < right) {
    pivot = PivDivide(a, left, right);
    DoQuickSort(a, left, pivot-1);
    DoQuickSort(a, pivot+1, right);
  }
}

DTX_EXPORT(void) 
	DtxQuickSort(void *a,int n,int (*comp)(void *,int,int),void (*swap)(void *,int,int))
{
	QUICK_SORT_INFO A;
	A.a = a;
	A.comp = comp;
	A.swap = swap;
	DoQuickSort(&A,0,n-1);
}

DTX_EXPORT(void) 
	DtxBubbleSort(void *a,int n,int (*comp)(void *,int,int),void (*swap)(void *,int,int))
{
  int i, j, n1=n-1;
  for (i = 0; i < n1; i++) {
    for (j = n1; j > i; j--) {
		int j1 = j-1;
		if ((*comp)(a,j1,j)>0) (*swap)(a,j1,j);
    }
  }
}

/*
  n is an element size of (void *a).
  void *a can be anything as far as it's elements can be identified by index from 0 to n-1.
  Note:
    Suppose N is an index of any element of a, 2 children of a[N] are a[2*N+1] and a[2*N+2].
	Suppose J is an index of any element of a, the parent of a[J] is a[(J-1)/2].
*/
DTX_EXPORT(void) 
	DtxHeapSort(void *a,int n,int (*comp)(void *,int,int),void (*swap)(void *,int,int))
{
    int leaf, root;

	// First: construct heap tree of all elements in a(a[0]-a[n-1]) => the root node is a[0] which is the biggest one. 
    leaf = n - 1;
    root = leaf/2;
    while (root >= 0 ) {
		int i;
		int r = root--;
		i = r * 2+1; // Left son.
		while (i <= leaf) {
			if (i < leaf && (*comp)(a,i + 1,i)>0) ++i;  // bigger son will be compared with parent (r).
			if ((*comp)(a,r,i)>=0) break;
			// parent node r is smaller = swap with selected son (i).
			(*swap)(a,r,i); 
			r = i; i = r * 2+1; // go down to farther sons.
		}
    }

	// Now,swap top(biggest) element and last element of the tree.
	// Shrink the tree(array) and reconstruct the tree.
	// leaf is the index of the last element of the tree. 
    while(leaf > 0) {
	    int i;
		int r = 0;
        (*swap)(a, 0, leaf--); // after swapping,the tree size is reduced by 1 (leaf--).
	    i = r * 2+1; // Left son.
		while (i <= leaf) {
			if (i < leaf && (*comp)(a,i + 1,i)>0) i++; // Right son is bigger.
			if ((*comp)(a,r,i)>=0) break;
			(*swap)(a,r,i);
			r = i; i = r * 2+1;
		}
    }
}
