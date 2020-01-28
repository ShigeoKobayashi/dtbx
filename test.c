/*
 *
 * DTBX: Digital Toolbox (C/C++ Library) sample testing program.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
/* #include "stdafx.h" */

#include <stdio.h>
#include <memory.h>

#include "dtbx.h"

void MatrixTest()
{
#define n 8
	int e,i,j;
	double s,t;
/* First examole is a simple and already dividable one. */
	double A[] = {
	           1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
               1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
               1.0, 1.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0,

               0.0, 0.0, 0.0, 3.0, 2.0, 0.0, 0.0, 0.0,
               0.0, 0.0, 0.0, 2.0, 4.0, 0.0, 0.0, 0.0,

               1.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0,

               1.0, 0.0, 0.0, 0.0, 0.0, 6.0, 6.0, 4.0,
               1.0, 0.0, 0.0, 3.0, 0.0, 0.0, 4.0, 7.0
			};
	double b[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	double x[8];
	double AA[64];

	memcpy(AA,A,sizeof(double)*n*n);
	printf("\nOriginal marix:\n");
	for(i=0;i<n;++i) {
		for(j=0;j<n;++j) {
			printf("  %lf",A[MAT_INDEX(i,j,n)]);
		}
		printf("\n");
	}
/*
	e = DtxLeqSolve(x,AA,b,n);
*/
	e = DtxLeqDivSolve(x,AA,b,n);
	printf("\n x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += A[MAT_INDEX(i,j,n)]*x[j];
		}
		printf(" %lf",b[i]-s);
	}

	/* exchange rows ==> affects b (not affect the order of x) */
	printf("\n\nThen exchange rows. This never affect the order of x[].\n");
	for(j=0;j<n;++j) {
		for(i=0;i<n;++i) {
			AA[MAT_INDEX(i,j,n)] = A[MAT_INDEX(n-i-1,j,n)];
		}
	}
	for(j = 0;j<n;++j) {
		t = AA[MAT_INDEX(  4,j,n)];
		AA[MAT_INDEX(4,j,n)] = AA[MAT_INDEX(5,j,n)];
		AA[MAT_INDEX(5,j,n)] = t;
	}
	/* then exchange column[0] and column[n-1] ==> the exchanges x[0] and x[n-1] */
	printf("And exchnage column[0] and column[n-1],this exchanges x[0] and x[n-1].\n");
	for(i=0;i<n;++i) {
		t = AA[MAT_INDEX(  i,0,n)];
		AA[MAT_INDEX(  i,0,n)]  = AA[MAT_INDEX(i,n-1,n)];
		AA[MAT_INDEX(i,n-1,n)] = t;
	}
	memcpy(A,AA,sizeof(double)*n*n);
/*
	e = DtxLeqSolve(x,AA,b,n);
*/
    e = DtxLeqDivSolve(x,AA,b,n);
	printf("\n x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += A[MAT_INDEX(i,j,n)]*x[j];
		}
		printf(" %lf",b[i]-s);
	}
}

 
 int main(int argc, char* argv[])
 {
	MatrixTest();
	DtxDebug();
	return 0;
 }