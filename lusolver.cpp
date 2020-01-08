/*
 *
 * DTBX: Digital Toolbox (C/C++ Library).
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#include "stdafx.h"

/*
   To solve linear equation system Ax=b,where A is nxn matrix, x is a solution vector ,and b is a constant vector sized n.
   First, call DtxLuDecompose() and then call DtxLuSolve() then.
*/

//
// decompose given matrix A to LU form. L is the lower triangular, and U is the upper triangular matrix.
// L and U are stored A.
//  A         ... [IN/OUT], Matrix to be decomposed,and result of decomposition are stored on exit.
//  scales[n] ... [OUT], work array for pivotting.
//  index [n] ... [OUT], indices for pivotting,which is used later in DtxLuSolve()
//
// returns 0,  normally decomposed.
//        -k(negative), failed to decompose when processing A[k-1,*].
//
DTX_EXPORT(int) 
	DtxLuDecompose(double *A, double *scales, int *index, int n)
{
	double biggst;
	double v;
	int pividx = -1;
	int ix;

	for (int i = 0; i < n; ++i) {
		// pick up largest(abs. val.) element in each row.
		index[i] = i;
		biggst = 0.0;
		for (int j = 0; j < n; ++j) {
			v = A[MAT_INDEX(i,j,n)];
			if(v<0.0) v = -v;
			if (v > biggst) biggst = v;
		}
		if (biggst > 0.0) scales[i] = 1.0 / biggst;
        else {
			ASSERT(FALSE);
			return -(i+1); // Singular matrix(case 1): no non-zero elements in row(i) found.
		}
	}
	int n1 = n - 1;
	for (int k = 0; k < n1; ++k) {
		// Gaussian elimination with partial pivoting.
		biggst = 0.0;
		for (int i = k; i < n; ++i) {
			int ip = index[i];
			v = A[MAT_INDEX(ip,k,n)];
			if(v<0.0) v = -v;
			v = v * scales[ip];
			if (v > biggst) {
				biggst = v;
				pividx = i;
			}
		}
		if (biggst <= 0.0) {
			ASSERT(FALSE);
			return -(k+1); // Singular matrix(case 2): no non-zero elements in row(k) found.
		}
		if (pividx != k) {
			int j = index[k];
			index[k] = index[pividx];
			index[pividx] = j;
		}
		int kp  = index[k];
		int kp1 = k+1;
		double pivot = A[MAT_INDEX(kp,k,n)];
		ASSERT(pivot!=0.0);
		for (int i = kp1; i < n; ++i) {
			ix = index[i];
			double mult = A[MAT_INDEX(ix,k,n)] / pivot;
			A[MAT_INDEX(ix, k,n)] = mult;
			if (mult != 0.0) {
				for (int j = kp1; j < n; ++j) A[MAT_INDEX(ix,j,n)] -= mult * A[MAT_INDEX(kp,j,n)];
			}
		}
	}
	if (A[MAT_INDEX(index[n1],n1,n)] == 0.0) {
		ASSERT(FALSE);
		return -(n1+1); // Singular matrix(case 3) at n1.
	}
	return 0;
}

//
// solves linear equation system Ax=b, A must be priocessed by DtxLuDecompose() before hand.
//
// index[n] must be the array processed by DtxLuDecompose().
// 
DTX_EXPORT(void) 
	DtxLuSolve(double *x, double *LU, double *b, int *index, int n)
{
	double dot = 0.0;
	int ix;
	for (int i = 0; i < n; ++i) {
		dot = 0.0;
		ix = index[i];
		for (int j = 0; j < i; ++j) dot += LU[MAT_INDEX(ix,j,n)] * x[j];
		x[i] = b[ix] - dot;
	}

	for (int i = (n - 1); i >= 0; --i) {
		dot = 0.0;
		ix = index[i];
		for (int j = (i + 1); j < n; ++j) dot += LU[MAT_INDEX(ix,j,n)] * x[j];
		ASSERT(LU[MAT_INDEX(ix,i,n)]!=0.0);
		x[i] = (x[i] - dot) / LU[MAT_INDEX(ix,i,n)];
	}
}

DTX_EXPORT(int) 
	DtxLeqSolve(double *x, double *A, double *b, int n)
{
	if(n<=0) return -1;
	if(n==1) {
		if(A[0]==0.0) return -1;
		x[0] = b[0]/A[0];
		return 0;
	}

	{
		TArray<double> scales(n);
		TArray<int>    index(n);
		int    *ps = index.Array();
		double *sc = scales.Array();
		int e = DtxLuDecompose(A,sc,ps,n);
		if(e) return e;
		DtxLuSolve(x,A,b,ps,n);
	}
	return 0;
}

static
int IsNonZeroElement(void *A,int i,int j,int n)
{
	if( ((double*)A)[MAT_INDEX(i,j,n)]!=0.0) return 1;
	return 0;
}

DTX_EXPORT(int) 
	DtxLeqDivSolve(double *x, double *A, double *b, int n)
{
	if(n<=0) return -1;
	if(n==1) {
		if(A[0]==0.0) return -1;
		x[0] = b[0]/A[0];
		return 0;
	}
	{
		TArray<int> lhsvs(n);
		TArray<int> rhsvs(n);
		TArray<int> sizes(n);
		int m = DtxDivideMatrix(n,A,lhsvs.Array(),rhsvs.Array(),sizes.Array(),IsNonZeroElement);
		if(m<=0) return m;
		if(m==1) return DtxLeqSolve(x,A,b,n); // unable to decompose.
		int me = 0;
		for(int i=0;i<m;++i) if(sizes[i]>me) me = sizes[i];
		;;
		{
			//////////////////////////////////////////////////////
			TArray<double> TA2(me*me);
			TArray<double> Tx2(me);
			TArray<double> Tb2(me);
			double *A2 = TA2.Array();
			double *x2 = Tx2.Array();
			double *b2 = Tb2.Array();
			int ixs = 0;
			for(int blk=0;blk<m;++blk) {
				// Solve from top block.
				int n2 = sizes[blk];
				for(int i=0;i<n2;++i) {
					int I = lhsvs[i+ixs];
					b2[i] = b[I];
					for(int j=0;j<n2;++j) {
						int J = rhsvs[j+ixs];
						A2[MAT_INDEX(i,j,n2)] = A[MAT_INDEX(I,J,n)];
					}
					// adjust b2[]
					double s = 0;
					for(int j=0;j<ixs;++j) {
						int J = rhsvs[j];
						s += A[MAT_INDEX(I,J,n)]*x[J];
					}
					b2[i] -= s;
				}
				int e = DtxLeqSolve(x2,A2,b2,n2);
				if(e) return e;
				for(int j=0;j<n2;++j) {
					int J = rhsvs[j+ixs];
					x[J] = x2[j];
				}
				ixs += n2;
			}
			//////////////////////////////////////////////////////
		}
	}
	return 0;
}
