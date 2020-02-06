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

void PMatrix(char *s,double *A,int n)
{
	int i,j;
	int ix = 0;
	printf("\nMatrix: %s\n",s);
	for(i=0;i<n;++i) {
		printf("   ");
		for(j=0;j<n;++j) printf(" %lf ",A[ix++]);
		printf("\n");
	}
}

void PVector(char *s,double *V,int n)
{
	int j;
	printf("\nVector: %s\n",s);
	printf("   ");
	for(j=0;j<n;++j) printf(" %lf ",V[j]);
	printf("\n");
}


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
	double AS[64];
	double b [  ] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
	double bs[  ] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
	double x [8 ];

	/* Save original data */
	memcpy(AS,A,sizeof(double)*n*n);
	memcpy(bs,b,sizeof(double)*n);

	printf("\nThe first marix solved by DtxLeqSolve():\n");
	for(i=0;i<n;++i) {
		for(j=0;j<n;++j) {
			printf("  %lf",A[INDEX2(i,j,n)]);
		}
		printf(" : %lf \n",b[i]);
	}
	e = DtxLeqSolve(x,A,b,n);
	if(e) printf("ERROR DtxLeqSolve()\n");

	printf("\n x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += AS[INDEX2(i,j,n)]*x[j];
		}
		printf(" %lf",bs[i]-s);
	}

	/*
	restore original data and exchange rows 
	       ==> affects b[] but not x[],then exchange b[] to keep consistence.
	*/
	printf("\n\nExchange rows.\n");
	for(i=0;i<n;++i) {
		for(j=0;j<n;++j) {
			A[INDEX2(i,j,n)] = AS[INDEX2(n-i-1,j,n)];
		}
		b[i] = bs[n-i-1];
	}
	for(j = 0;j<n;++j) {
		t = A[INDEX2(4,j,n)];
		A[INDEX2(4,j,n)] = A[INDEX2(5,j,n)];
		A[INDEX2(5,j,n)] = t;
	}
	t = b[4];
	b[4] = b[5];
	b[5] = t;

	/* then exchange column[0] and column[n-1] ==> this exchange x[0] and x[n-1] */
	printf("And exchnage column[0] and column[n-1].\n");
	for(i=0;i<n;++i) {
		t = A[INDEX2(  i,0,n)];
		A[INDEX2(  i,0,n)]  = A[INDEX2(i,n-1,n)];
		A[INDEX2(i,n-1,n)] = t;
	}

	/* save re-arranged data */
	memcpy(AS,A,sizeof(double)*n*n);
	memcpy(bs,b,sizeof(double)*n);
	printf("\nRe-arranged A and b:\n");

	for(i=0;i<n;++i) {
		for(j=0;j<n;++j) {
			printf(" %lf ",A[INDEX2(i,j,n)]);
		}
		printf(" : %lf\n",b[i]);
	}

	e = DtxLeqSolve(x,A,b,n);
	if(e) printf("ERROR DtxLeqSolve()\n");
	printf("\nJust solve by DtxLeqSolve():\n");
	printf(" x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += AS[INDEX2(i,j,n)]*x[j];
		}
		printf(" %lf",bs[i]-s);
	}
	printf("\n");

	memcpy(A,AS,sizeof(double)*n*n);
	memcpy(b,bs,sizeof(double)*n);

	printf("\nTest for DtxLeqDivSolve():\n");
	e = DtxLeqDivSolve(x,A,b,n);

	if(e) printf("ERROR DtxLeqDivSolve()\n");
	printf("\nFinal answer check:\n x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += AS[INDEX2(i,j,n)]*x[j];
		}
		printf(" %lf",bs[i]-s);
	}
	printf("\n");

	memcpy(A,AS,sizeof(double)*n*n);
	memcpy(b,bs,sizeof(double)*n);

	printf("\nTest for DtxLeqDivIndexSolve():\n");
	e = DtxLeqDivIndexSolve(x,A,b,n);
	if(e) printf("ERROR DtxLeqDivIndexSolve()\n");

	printf(" x[] =");
	for(i=0;i<n;++i) {
		printf(" %lf",x[i]);
	}
	printf("\n b-Ax=");
	for(i=0;i<n;++i) {
		s = 0;
		for(j=0;j<n;++j) {
			s += AS[INDEX2(i,j,n)]*x[j];
		}
		printf(" %lf",bs[i]-s);
	}
	printf("\n");

}

 void EasyTest()
 {
	double A[] = {
     1.0,  1.0,  0.0,  3.0,
     2.0,  1.0, -1.0,  1.0,
     3.0, -1.0, -1.0,  2.0,
    -1.0,  2.0,  3.0, -1.0
    };

	double L[] = {
     1.0,  0.0,  0.0,  0.0,
     2.0,  1.0,  0.0,  0.0,
     3.0,  4.0,  1.0,  0.0,
    -1.0, -3.0,  0.0,  1.0
    };

	double U[] = {
     1.0,  1.0,  0.0,  3.0,
     0.0, -1.0, -1.0, -5.0,
     0.0,  0.0,  3.0, 13.0,
     0.0,  0.0,  0.0,-13.0
    };

	double b[] = { 4.0, 1.0, -3.0, 4.0};
	double x[] = {-1.0, 2.0,  0,0, 1.0};
	int    I[] = {0,1,2,3};
	int    J[] = {0,1,2,3};
	DtxLuIndexDecomp(A,4,I,J,4);
	PMatrix("LU",A,4);
	DtxLuIndexSolve(x,A,b,4,I,J,4);
	PVector("x",x,4);
 }


 int main(int argc, char* argv[])
 {
	MatrixTest();
/*
	EasyTest();
	DtxDebug();
*/
	return 0;
 }