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
	DtxLuDecomp(double *A, double *scales, int *index, int n)
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
			v = A[INDEX2(i,j,n)];
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
			v = A[INDEX2(ip,k,n)];
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
		double pivot = A[INDEX2(kp,k,n)];
		ASSERT(pivot!=0.0);
		for (int i = kp1; i < n; ++i) {
			ix = index[i];
			double mult = A[INDEX2(ix,k,n)] / pivot;
			A[INDEX2(ix, k,n)] = mult;
			if (mult != 0.0) {
				for (int j = kp1; j < n; ++j) A[INDEX2(ix,j,n)] -= mult * A[INDEX2(kp,j,n)];
			}
		}
	}
	if (A[INDEX2(index[n1],n1,n)] == 0.0) {
		ASSERT(FALSE);
		return -(n1+1); // Singular matrix(case 3) at n1.
	}
	return 0;
}


//
// decompose sub-matrix of A to LU form without scaling/pivotting.
// L is the lower triangular, and U is the upper triangular matrix that are subset of A.
// L and U are stored to A.
//  A         ... [IN/OUT], Matrix to be decomposed,and result of decomposition are stored on exit.
// returns 0,  normally decomposed.
//        -k(negative), failed to decompose when processing A[k-1,*].
//
DTX_EXPORT(int) 
	DtxLuIndexDecomp(double A[],int N,int I[], int J[], int n)
{
	int n1 = n - 1;
	for (int k = 0; k < n1; ++k) {
		// Gaussian elimination with partial pivoting.
		int kp1 = k+1;int ik = I[k];int jk = J[k];
		double div = A[INDEX2(ik,jk,N)]; ASSERT(div!=0.0);
		for (int i = kp1; i < n; ++i) {	int ii = I[i];
			double mult = A[INDEX2(ii,jk,N)] / div;
			A[INDEX2(ii, jk,N)] = mult;
			if (mult != 0.0) {
				for (int j = kp1; j < n; ++j) {
					int jj = J[j];
					A[INDEX2(ii,jj,N)] -= mult * A[INDEX2(ik,jj,N)];
				}
			}
		}
	}
	if (A[INDEX2(I[n1],J[n1],N)] == 0.0) {
		ASSERT(FALSE);
		return -(n1+1); // Singular matrix(case 3) at n1.
	}
	return 0;
}


//
// solves linear equation system Ax=b, A(=LU) must be priocessed by DtxLuDecomp() before hand.
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
		for (int j = 0; j < i; ++j) dot += LU[INDEX2(ix,j,n)] * x[j];
		x[i] = b[ix] - dot;
	}

	for (int i = (n - 1); i >= 0; --i) {
		dot = 0.0;
		ix = index[i];
		for (int j = (i + 1); j < n; ++j) dot += LU[INDEX2(ix,j,n)] * x[j];
		ASSERT(LU[INDEX2(ix,i,n)]!=0.0);
		x[i] = (x[i] - dot) / LU[INDEX2(ix,i,n)];
	}
}

//
// solves linear equation system Ax=b, where A is a sub-matrix of LU which must be priocessed by DtxLuIndexDecomp() before hand.
// 
DTX_EXPORT(void) 
	DtxLuIndexSolve(double x[], double LU[], double b[],int N,int I[],int J[],int n)
{
	double dot = 0.0;

	for (int i = 0; i < n; ++i) {
		dot = 0.0;
		int ii = I[i];
		int ji = J[i];
		for (int j = 0; j < i; ++j) {
			int jj = J[j];
			dot += LU[INDEX2(ii,jj,N)] * x[jj];
		}
		x[ji] = b[ii] - dot;
	}

	for (int i = (n - 1); i >= 0; --i) {
		dot = 0.0;
		int ii = I[i];
		int ji = J[i];
		for (int j = (i + 1); j < n; ++j) {
			int jj = J[j];
			dot += LU[INDEX2(ii,jj,N)] * x[jj];
		}
		ASSERT(LU[INDEX2(ii,J[i],N)]!=0.0);
		x[ji] = (x[ji] - dot) / LU[INDEX2(ii,ji,N)];
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
	} else {
		TArray<double> scales(n);
		TArray<int>    index(n);
		int    *ps = index.Array();
		double *sc = scales.Array();
		int e = DtxLuDecomp(A,sc,ps,n);
		if(e) return e;
		DtxLuSolve(x,A,b,ps,n);
	}
	return 0;
}

static
int IsNonZeroElement(void *A,int i,int j,int n)
{
	if( ((double*)A)[INDEX2(i,j,n)]!=0.0) return 1;
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
		TArray<int> I(n);
		TArray<int> J(n);
		TArray<int> sizes(n);
		int m = DtxDivideMatrix(n,A,I.Array(),J.Array(),sizes.Array(),IsNonZeroElement);
		if(m<=0) return m;
		if(m==1) return DtxLeqSolve(x,A,b,n); // unable to decompose.
		int me = 0;

#ifdef _DEBUG
		printf("\nDtxLeqDivSolve() :Re-arranged matrix\n");
		for(int i=0;i<n;++i) {
			printf("  ");
			for(int j=0;j<n;++j) {
				printf(" %lf ",A[INDEX2(I[i],J[j],n)]);
			}
			printf(" : %lf\n",b[I[i]]);
		}
#endif

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

			// Solve from top block.
			for(int blk=0;blk<m;++blk) {
				int n2 = sizes[blk];
				for(int i=0;i<n2;++i) {
					int II = I[i+ixs];
					b2[i] = b[II];
					for(int j=0;j<n2;++j) {
						int JJ = J[j+ixs];
						A2[INDEX2(i,j,n2)] = A[INDEX2(II,JJ,n)];
					}
			
					// adjust b2[]
					double s = 0;
					for(int j=0;j<ixs;++j) {
						int JJ = J[j];
						s += A[INDEX2(II,JJ,n)]*x[JJ];
					}
					b2[i] -= s;
				}

#ifdef _DEBUG
			printf("Sub-matrix to be solved:\n");
			for(int i=0;i<n2;++i) {
				printf("  ");
				for(int j=0;j<n2;++j) {
					printf(" %lf ",A2[INDEX2(i,j,n2)]);
				}
				printf(" : %lf\n",b2[i]);
			}
#endif
				int e = DtxLeqSolve(x2,A2,b2,n2);
				if(e) return e;
				// Save back to X
				for(int j=0;j<n2;++j) {
					int JJ = J[j+ixs];
					x[JJ] = x2[j];
				}

#ifdef _DEBUG
			{
				printf("\nAnswer check\n");
				for(int i=0;i<n2;++i) {
					double s = 0;
					int ii = I[i+ixs];
					int ji = J[i+ixs];
					for(int j=0;j<n2;++j) {
						int jj = J[j+ixs];
						s += A[INDEX2(ii,jj,n)]*x[jj];
					}
					printf(" x[%d]=%lf %lf\n",ji,x[ji],b2[i]-s);
				}
				printf("\n");
			}
#endif
				ixs += n2;
			}
			//////////////////////////////////////////////////////
		}
	}
	return 0;
}

DTX_EXPORT(int) 
	DtxLeqDivIndexSolve(double *x, double *A, double *b, int n)
{
	if(n<=0) return -1;
	if(n==1) {
		if(A[0]==0.0) return -1;
		x[0] = b[0]/A[0];
		return 0;
	}
	{
		TArray<int> I(n);
		TArray<int> J(n);
		TArray<int> sizes(n);

#ifdef _DEBUG
		TArray<double> AA(n*n);
		double *AS = AA.Array();
		TArray<double> B(n);
		double *bs = B.Array();
		memcpy(AS,A,sizeof(double)*n*n);
		memcpy(bs,b,sizeof(double)*n);
		printf("\nDtxLeqDivIndexSolve()\n");
#endif

		int m = DtxDivideMatrix(n,A,I.Array(),J.Array(),sizes.Array(),IsNonZeroElement);
		if(m<=0) return m;
		if(m==1) return DtxLeqSolve(x,A,b,n); // unable to decompose.

		//////////////////////////////////////////////////////
		int ixs = 0;

#ifdef _DEBUG
		printf("\nDtxLeqDivIndexSolve(): Re-arranged matrix\n");
		for(int i=0;i<n;++i) {
			printf("  ");
			for(int j=0;j<n;++j) {
				printf(" %lf ",A[INDEX2(I[i],J[j],n)]);
			}
			printf(" : %lf\n",b[I[i]]);
		}
#endif

		// Solve sub-matrices from the top.
		for(int blk=0;blk<m;++blk) {
			int n2 = sizes[blk];
			int *pI = &(I[ixs]);
			int *pJ = &(J[ixs]);

			// adjust b[]
			for(int i=0;i<n2;++i) {
				double s = 0;
				int ii = pI[i];
				for(int j=0;j<ixs;++j) {
					int jj = J[j];
					s += A[INDEX2(ii,jj,n)]*x[jj];
				}
				b[ii] -= s;
			}

#ifdef _DEBUG
			printf("Sub-matrix to be solved:\n");
			for(int i=0;i<n2;++i) {
				printf("  ");
				for(int j=0;j<n2;++j) {
					printf(" %lf ",A[INDEX2(pI[i],pJ[j],n)]);
				}
				printf(" : %lf\n",b[pI[i]]);
			}
#endif
			int e = DtxLuIndexDecomp(A,n,pI,pJ,n2);
			if(e) return e;
			DtxLuIndexSolve(x,A,b,n,pI,pJ,n2);
#ifdef _DEBUG
			{
				printf("\nAnswer check\n");
				for(int i=0;i<n2;++i) {
					double s = 0;
					for(int j=0;j<n2;++j) {
						s += AS[INDEX2(pI[i],pJ[j],n)]*x[pJ[j]];
					}
					printf(" x[%d]=%lf %lf\n",pJ[i],x[pJ[i]],b[pI[i]]-s);
				}
				printf("\n");
			}
#endif
			ixs += n2;
		}
		//////////////////////////////////////////////////////
	}
	return 0;
}
