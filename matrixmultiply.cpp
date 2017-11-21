/*
  Matrix multiplication acceleration.
*/


//
//design for synthesis
//

#include "matrixmultiply.h"
void 
GetARowCoefficients(int A[N][M], unsigned row_ix, int RC[M]){
  //go through rows fish out coefficients
  for (unsigned i=0 ; i < M; i++)
    RC[i] = A[row_ix][i];
}


void
GetBColCoefficients(int B[M][P], unsigned col_ix, int CC[M]){
  //got through B cols fish out coefficieints
  for (unsigned i=0; i < M; i++){
    CC[i] = B[i][col_ix];
  }
}


void
MultiplyAndStore(int RC[M], int CC[M], int C [N][P], int i, int j, unsigned row_size, unsigned col_size){

  int temp_result=0;
  for (unsigned li=0; li < M ; li++){
    temp_result += (RC[li]*CC[li]);
  }
  
  C[i][j]=temp_result;
}



void matrixmultiply(int A[N][M], int B[M][P], int C[N][P]){

 row_loop: for (unsigned i=0; i < N; i++){
    int RC[M];
    GetARowCoefficients(A, i, RC);
  column_loop: for (unsigned j =0 ; j < P; j++){
      int CC[M];
      GetBColCoefficients(B,j, CC);
      MultiplyAndStore(RC,CC,C,i,j, M, M);
    }
  }
}

