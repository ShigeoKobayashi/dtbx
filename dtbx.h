/*
 *
 * DTBX: Digital Toolbox (C/C++ Library).
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */

#ifndef  __DTBX_H
#define  __DTBX_H 1

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

#ifdef WINDOWS /**** WINDOWS ****/
/* WINDOWS specific part (Same define's must be defined for other platforms.) */
 #define DTX_EXPORT(t)  __declspec(dllexport) t __stdcall
/* Note:
__stdcall __cdecl and C#
------------------------
  WIN32 API's use __stdcall(which can't be used for vararg functions) with .def file representation.
  __stdcall without .def file changes function name exported in .lib file.
  __cdecl (c compiler default) never changes function name exported but consumes more memories than __stdcall.
  C# [Dllexport] atrribute uses __stdcall in default.
  To call __cdecl functions from C#, use CallingConvention.Cdecl like "[DllImport("MyDLL.dll", CallingConvention = CallingConvention.Cdecl)]".
*/
#endif /**** WINDOWS ****/

#ifdef LINUX /******** LINUX ********/
/* gcc/g++ specific part ==> compiled with '-fPIC -fvisibility=hidden' option. */
/*
 -fvisibility=hidden option hides everything except explicitly declared as exported API just like
 as Windows' dll.
*/
#define DTX_EXPORT(t) __attribute__((visibility ("default")))  t
#endif /**** LINUX ****/

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _DEBUG
DTX_EXPORT(void) DtxDebug();
#endif

/* in sort.cpp */
/*
   Last 2 arguments ,compare and swap, perform actual sort operations(compares elements and exchanges(swaps) elements of a respectively.
   a can be anything of which element can be identified by it's index.
   n is an element size of a.
*/
/* heap sort */
DTX_EXPORT(void) DtxHeapSort  (void *a,int n,int (*compare)(void *a,int i,int j),void (*swap)(void *a,int i,int j));
/* bubble sort */
DTX_EXPORT(void) DtxBubbleSort(void *a,int n,int (*compare)(void *a,int i,int j),void (*swap)(void *a,int i,int j));
/* quick  sort */
DTX_EXPORT(void) DtxQuickSort (void *a,int n,int (*compare)(void *a,int i,int j),void (*swap)(void *a,int i,int j));

/* in lusolver.cpp */
/* INDEX2(i,j,M) calculates square MATRIX index */ 
#define INDEX2(i,j,M)   ((i)*(M)+j)
DTX_EXPORT(int)  DtxLuDecomp(double A[], double scales[], int index[], int n);
DTX_EXPORT(void) DtxLuSolve(double x[], double A[], double b[], int index[], int n);
/* solves Ax=b by calling DtxLuDecomp() and DtxLuSolve(). Working arrays are internally allocated. */
DTX_EXPORT(int)  DtxLeqSolve(double x[], double A[], double b[], int n);

/* LU solver without scaling. No working arrays are required, */
DTX_EXPORT(int)  DtxLuIndexDecomp(double A[], int N, int I[], int J[], int n);
DTX_EXPORT(void) DtxLuIndexSolve(double x[], double LU[], double b[],int N,int I[],int J[],int n);

/* in block3.cpp */
DTX_EXPORT(int) DtxSelectPair(int nv,int lpair[],int rpair[],int num_r[],int *rv[]);
DTX_EXPORT(int) DtxDividePairedMatrix(int nv,int order[],int block[],int num_r[],int *rv[]);
DTX_EXPORT(int) DtxDivideMatrix(int n,void *A,int lhsvs[],int rhsvs[],int sizes[],int (*IsNonZeroElement)(void *A,int I,int J,int M));
DTX_EXPORT(int) DtxLeqDivSolve(double *x, double *A, double *b, int n);
DTX_EXPORT(int) DtxLeqDivIndexSolve(double *x, double *A, double *b, int n);

#if defined(__cplusplus)
}  /* extern "C" { */
#endif

#endif /* __DTBX_H */
