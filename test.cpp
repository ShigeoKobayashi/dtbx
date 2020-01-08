//#include "stdafx.h"
#include <stdio.h>
#include <memory.h>

#include "dtbx.h"

int main(int argc, char* argv[])
{
#define n 8
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
	for(int i=0;i<n;++i) {
		for(int j=0;j<n;++j) {
			printf("  %lf",A[MAT_INDEX(i,j,n)]);
		}
		printf("\n");
	}

//	int e = DtxLeqSolve(x,AA,b,n);
	int e = DtxLeqDivSolve(x,AA,b,n);
	printf("\n x[] =");
	for(int i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(int i=0;i<n;++i) {
		double s = 0;
		for(int j=0;j<n;++j) {
			s += A[MAT_INDEX(i,j,n)]*x[j];
		}
		printf(" %lf",b[i]-s);
	}

	/* reverse rows ==> reverse b (not affect the order of x) */
	printf("\n\nThen revert rows. This never affect the order of x[].\n");
	for(int j=0;j<n;++j) {
		for(int i=0;i<n;++i) {
			AA[MAT_INDEX(i,j,n)] = A[MAT_INDEX(n-i-1,j,n)];
		}
	}
	/* then exchange column[0] and column[n-1] ==> the exchanges x[0] and x[n-1] */
	printf("And exchnage column[0] and column[n-1],this exchanges x[0] and x[n-1].\n");
	for(int i=0;i<n;++i) {
		double t = AA[MAT_INDEX(  i,0,n)];
		AA[MAT_INDEX(  i,0,n)]  = AA[MAT_INDEX(i,n-1,n)];
		AA[MAT_INDEX(i,n-1,n)] = t;
	}
	memcpy(A,AA,sizeof(double)*n*n);
//    e = DtxLeqSolve(x,AA,b,n);
    e = DtxLeqDivSolve(x,AA,b,n);
	printf("\n x[] =");
	for(int i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(int i=0;i<n;++i) {
		double s = 0;
		for(int j=0;j<n;++j) {
			s += A[MAT_INDEX(i,j,n)]*x[j];
		}
		printf(" %lf",b[i]-s);
	}
	return 0;
}

