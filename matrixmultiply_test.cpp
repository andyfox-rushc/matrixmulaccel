/*
Test harness
*/

#include <stdlib.h>
#include <stdio.h>

#include "matrixmultiply.h"


void matrixmultiply(int A[N][M], int B[M][P], int C[N][P]);

void DumpMatrix(const char* name,
		unsigned row_count,
		unsigned col_count,
		int* matrix
){
  printf("++++++++++++++++++++++++++\n");
  printf("Matrix: %s (R=%d x C=%d )\n",name ? name: "anon",row_count,col_count);
  for (unsigned r=0;r<row_count;r++){ 
      for (unsigned c=0; c< col_count; c++){
	int offset = r*col_count + c;
	printf("%d %s",*(matrix + offset), (c +1 < col_count)? " ,": "\n");
    }
  }
  printf("--------------------------\n");  
}


void multiplyOut(int A[N][M], int B[M][P], unsigned row, unsigned col, unsigned a_col_count, int C[N][P]){
  int result=0;

  for (unsigned i = 0 ; i < a_col_count; i++){
    result += A[row][i]*B[i][col];
  }
  C[row][col] = result;
}


void matrix_multiply_tb(int A[N][M],
			int B[M][P],
			int C[N][P]){
  printf("Running test bench for matrix multiply\n");
  printf("+++++++++++++++++++++++++++++++++++++++\n");
  DumpMatrix("A",N,M,(int*)A);
  DumpMatrix("B",M,P,(int*)B);

  for (unsigned row_ix=0; row_ix< N; row_ix++){ //row
    for (unsigned col_ix=0; col_ix < P; col_ix++){//col (result is N x P)
      multiplyOut(A,B,row_ix,col_ix, M /*a col size */,C);
    }
  }
  DumpMatrix("C = A*B",N,P,(int*)C);
  printf("-----------------------------------------\n");
}

  bool
  CheckEquivalenceOfMatrices(    int *m1  , int *m2 , unsigned r_limit, unsigned c_limit){
    for (unsigned r=0 ; r < r_limit; r++)
      for (unsigned c=0; c< c_limit; c++)
	if ((*(m1+ r*c)) != (*(m2 + r*c)))
	  return false;
    return true;
  }

int main(int argc, char** argv){

    int A[N][M];
    int B[M][P];
    int C[N][P];

    int value=1;
    for (unsigned i=0; i< N; i++){
      for (unsigned j=0; j< M; j++){
	A[i][j] = value;
	value++;
      }
    }

    for (unsigned i=0; i< M; i++){
      for (unsigned j=0; j< P; j++){
	B[i][j]=value;
	value++;
      }
    }

    matrix_multiply_tb(A,B,C);
    //Now execute the DUT
    int R[N][P];
    matrixmultiply(A,B,R);
    
    DumpMatrix("R",N,P,(int*)R);
    if (CheckEquivalenceOfMatrices((int*)R,(int*)C,N,P)){
      printf("Success: design and spec match !\n");
      return 0;
    }
    else{
      printf("Failure: design and spec do not match !\n");
      return -1;
    }
    exit(0);
  }
