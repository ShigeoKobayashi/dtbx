/*
 *
 * DTBX: Digital Toolbox (C/C++ Library).
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */

#include "stdafx.h"

 typedef struct {
	 int *a1;
	 int *a2;
 } TWO_ARRAY;
 
 static 
	 int Comp2Array(void *a,int i,int j)
 {
	 TWO_ARRAY *b = (TWO_ARRAY*)a;
	 if(b->a1[i]>b->a1[j]) return 1;
	 if(b->a1[i]<b->a1[j]) return -1;
	 if(b->a2[i]>b->a2[j]) return 1;
	 if(b->a2[i]<b->a2[j]) return -1;
	 return 0;
 }

 static 
	 void Swap2Array(void *a,int i,int j)
 {
	 TWO_ARRAY *b = (TWO_ARRAY*)a;
	 int t = b->a1[i];
	 b->a1[i] = b->a1[j];
	 b->a1[j] = t;
	 t = b->a2[i];
	 b->a2[i] = b->a2[j];
	 b->a2[j] = t;
 }

 //
 // DtxDivideMatrix() divides a matrix A into a series of smaller sized matrices.
 // This function is not interested in the type of matrix(and it's elements).
 // Because the element is zero or non-zero is the only information needed,
 // the user must provide the function address(IsNonZeroElement) which tells the specified element
 // is zero or non-zero.
 // The resulting divided submatrices can be obtained by lhsvs[],rhsvs[],and sizes[].
 //
 //  n      ... [IN] the size of the (square)matrix divided.
 //  A      ... [IN] (square)matrix divided or any information for the matrix only passed to the user 
 //                  supplied function IsNonZeroElement(). This function never touch this A.
 //  rhsv[] ... [OUT] Row numbers of A are stored on exit.
 //  lhsv[] ... [OUT] Column numbers of A are stored on exit.
 //  sizes[]... [OUT] Sizes of divided sub-matrices.
 //                   If sizes[0] is 3,then rows rhsv[0] to rhsv[2] and columns rhsv[0] to rhsv[2]
 //                   forms the first sub-matrix divided. 
 //  IsNonZeroElement
 //         ... [IN]  User specified function which indicates the element A[I,J] is zero(returns 0)
 //                   or non-zero(return non-zero value). n is the size of A.
 //  returns >0     ... totoal number of sub-matrices when A is successfully divided.
 //          other  ... A is not divided (A is a singular matrix).
 //
 DTX_EXPORT(int) 
	DtxDivideMatrix(int n,void *A,int lhsvs[],int rhsvs[],int sizes[],int (*IsNonZeroElement)(void *A,int I,int J,int n))
 {
	int       e = 0;

	try {
		int c = 0;
		int *ppr;
		if(n<=0) throw new CException(-1,"ERROR[DtxDivideMatrix()]: Zero or negative number of variavles.");
		TStack<int>  rarray(n);
		TArray<int>  nr(n);
		TArray<int*> pr(n);
		nr.Clear(0);

		// create data structure for DtxSelectPair()
#ifdef _DEBUG
		printf("\n DtxDivideMatrix(), matrix before divided(0:zero & 1:non-zero element):\n ");
#endif
		for(int i=0;i<n;++i) {
			c = 0;
			for(int j=0;j<n;++j) {
				if(IsNonZeroElement(A,i,j,n)) {
#ifdef _DEBUG
					printf(" 1");
#endif
					rarray.Push(j);
					c++;
				}
#ifdef _DEBUG
				else printf(" 0");
#endif
			}
			if(c<=0) throw new CException(-2,"ERROR[DtxDivideMatrix()]: Left hand side variable having no right hand side variables.");
			nr[i] = c;
#ifdef _DEBUG
			printf("\n ");
#endif
		}
		ppr = rarray.Array();
		for(int i=0;i<n;++i) {
			pr[i] = ppr;
			ppr += nr[i];
		}
		e = DtxSelectPair(n,lhsvs,rhsvs,nr.Array(),pr.Array());
		if(e) return e;

#ifdef _DEBUG
		printf("\n\n DtxSelectPair(), selected pairs:\n");
		for(int i=0;i<n;++i) {
			printf("  %d) %d %d\n",i,lhsvs[i],rhsvs[i]);
		}
#endif
		// renumber the rhsvs,because DdsDividePairedMatrix() assumes lhsvs[i] is paired with rhsvs[i].
		for(int i=0;i<n;++i) {
			int *ppr = pr[i];
			int    c = nr[i];
			for(int j=0;j<c;++j) {
				ppr[j] = rhsvs[ppr[j]];
			}
		}
		e = DtxDividePairedMatrix(n,rhsvs,sizes,nr.Array(),pr.Array());
		if(e<0) return e;
		int *pl = nr.Array();
		memcpy(pl,rhsvs,sizeof(int)*n);
		for(int i=0;i<n;++i) {
			rhsvs[i] = lhsvs[pl[i]];
		}
		memcpy(lhsvs,pl,sizeof(int)*n);

#ifdef _DEBUG
		printf("\n\n DtxDivideMatrix(), resulting re-arranged matrix:\n");
		for(int i=0;i<n;++i) {
			printf("  Y%d=",lhsvs[i]);
			for(int j=0;j<n;++j) {
				if(IsNonZeroElement(A,lhsvs[i],rhsvs[j],n)) printf(" 1");
				else                                        printf(" 0");
			}
			printf("\n");
		}
		printf("  X =");
		for(int j=0;j<n;++j) printf(" %d",rhsvs[j]);
		printf("\n");

		printf("\n\n DtxDivideMatrix(), sub-matrices after division:\n");
		for(int i=0;i<n;++i) {
			printf("  %d) %d %d %d\n",i,lhsvs[i],rhsvs[i],sizes[i]);
		}
#endif
	} catch(CDbgException *ex) {
		e = ex->ecode;
		delete ex;
	}
	return e;
 }


/*
  Creates 1 to 1 relations between left and right hand side variables.
   nv        ... IN:total number of variables.
   paired_x[nv] ... OUT:1 to 1 relations resulted. left variable i is related to the right variable paired_x[i].
   paired_y[nv] ... OUT:1 to 1 relations resulted. right variable i is related to the left variable paired_y[i].
   num_r[nv] ... IN:num_r[i] is a number of right hand side variables of the variable i.
   rv[]      ... IN:rv[i] is a pointer of the array holding the right hand side variable of i. 
                  rv[i][j] is the jth right hand side variable of the left variable i.
   returns 0      ... All varables are successfully paired.
           other  ... Failed to make pairs.

				  Note: All variables must be noted by the number(can be indeces) from 0 to (nv-1).
*/
 DTX_EXPORT(int) 
	 DtxSelectPair(int nv,int paired_x[],int paired_y[],int num_r[],int *rv[])
 {
	int e = 0;
	try {
		if(nv<=0) throw new CException(-1,"ERROR[DtxSelectPair()]: Zero or negative number of variavles.");
		TStack<int> Stack(nv+1);
		TArray<int> nw(nv);
		int *nr = nw.Array();
		int mb = nv*sizeof(nr[0]);
		int ixr;
		int il;
		int ir;

		for(int i=0;i<nv;++i) {
			paired_x[i] = -1;
			paired_y[i] = -1;
		}

		memset(nr,0,mb);
		for(int i=0;i<nv;++i)
		{
			il      = i;
			Stack.Push(-1);
			do {
				while((ixr = nr[il]++)>=num_r[il]) {
					il = Stack.Pop();
					if(il<0) throw new CException(-1,"ERROR[DtxSelectPair()]: Unable to select pair.");
				}
				Stack.Push(il);
				ir = (rv[il])[ixr];
				il = paired_y[ir];
			} while(il>=0);

			while((il = Stack.Pop())>=0) {
				int t = paired_x[il];
				paired_x[il] = ir;
				paired_y[ir] = il;
				ir = t;
			}
		}
	} catch(CDbgException *ex) {
		e = ex->ecode;
		delete ex;
	}
	return e;
 }
 
 /*
 Divide a matrix(Jacobian matrix) into a series of smaller sized matrices.
 This function assumes that the left hand side valiable i and the right hand side variable i are paired each other. 
   nv      ... IN: the total number of variables.
   order[] ... OUT: indeces of variables sorted to the order of matrices devided.
   block[] ... OUT: sizes of the devided matrices.
   num_r[] ... IN: the number of the right hand side variables. The nuber of the right hand side variables of the variable i is num_r[i].
   rv[]    ... IN: pointer to the array having the right hand side variables. The right hand size variavles of the variavle i are stored to rv[i].
   RETURNs number of total matrices divied (>0),otherwise negative error code.
 */
 DTX_EXPORT(int) 
	 DtxDividePairedMatrix(int nv,int order[],int block[],int num_r[],int *rv[])
 {
	int e = 0;
	int ivNow,ibNext;

	try {
		if(nv<=0) throw new CException(-1,"ERROR[DtxDivideMatrix()]: Zero or negative number of variavles.");
		TStack<int> Stack(nv+1);
		TArray<int> nw(nv);

		int *nr = nw.Array();

		for(int j=0;j<nv;++j) {
			int *pr = rv[j];
			order[j] = j; // self loop (left 1 must be paired with right i)
			block[j] = j; // block number
		}

#ifdef _DEBUG
		printf("\n DtxDividePairedMatrix(), input data structure:\n");
		for(int i=0;i<nv;++i) {
			int *pr = rv[i];
			int c   = num_r[i];
			printf("  %d)",i);
			for(int j=0;j<c;++j) {
				printf(" %d",pr[j]);
			}
			printf("\n");
		}
#endif
		int mb = nv*sizeof(nr[0]);
		for(int i=0;i<nv;++i)
		{
			memset(nr,0,mb);
			ivNow = i;
			Stack.Push(-1);
			// Starting variable is i(then find routes to i).
			while (ivNow>=0) {
				ibNext = nr[ivNow];
				if(ibNext>=num_r[ivNow]) {
					ivNow=Stack.Pop();
				} else { // Next edge of the ivNow
					Stack.Push(ivNow);
					nr[ivNow]++;
					ivNow = rv[ivNow][ibNext]; // RHSV of the ivNow
					if(ivNow==i || block[ivNow]==i) {
						// Loop fond ==> set block number to the variables on the stack.
						int *routeVars = Stack.Array();
						int ns = Stack.Count();
						for(int ir = 1;ir<ns;++ir) {
							if(block[routeVars[ir]]!=i) {
								// different loop ivr crossed with i.
								//   ==> merge j and i (the loop of j is merged to the loop i).
								int iPrev,jPrev;
								int ix = i;
								int j  = routeVars[ir];
								do {iPrev = ix;               } while((ix = order[ix])!=i);
								ix = routeVars[ir];
								do {jPrev = ix; block[ix] = i;} while((ix = order[ix])!=j);
								order[iPrev] = j;
								order[jPrev] = i;
							}
						}
					}
				}
			}
		}

		// finished to identify loops(sub matrices),then decide the order of each matrices.
		int changed = 1;
		do {
			changed = 0;
			for(int i=0;i<nv;++i)
			{
				int i1  = order[i];
				int BI   = block[i];
				int *pr = rv[i]; // RHSV of the i.
				for(int l=0;l<num_r[i];++l)
				{
					int j = pr[l];
					if(block[j]>BI) {
						// Swap block numbers for ordering.
						int BJ = block[j];
						int ix = i;
						do {block[ix] = BJ;ix = order[ix];} while (ix!=i);
						ix = j;
						do {block[ix] = BI;ix = order[ix];} while (ix!=j);
						changed = 1;
						BI = BJ;
					}
				}
			}
		} while(changed);
		TWO_ARRAY B;
		B.a1 = block;
		B.a2 = order;
		for(int i=0;i<nv;++i) order[i] = i;
		DtxHeapSort(&B,nv,Comp2Array,Swap2Array);
		int L  = -1;
		int ix = -1;
		int ne = -1;
		while(++ix<nv) {
			if(block[ix]!=L) {
				L = block[ix];
				block[++ne] = 1;
			} else {
				block[ne] ++;
			}
		};
		for(int i = ++ne;i<nv;++i) block[i] = 0;
		e = ne;
	} catch(CDbgException *ex) {
		e = ex->ecode;
		delete ex;
	}
	return e;
 }
