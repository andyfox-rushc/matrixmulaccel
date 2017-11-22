/*
  Matrix multiplication acceleration.
*/


//
//design for synthesis
//

#include "matrixmultiply.h"
void 
GetARowCoefficients(short A[N][M], short row_ix, short RC[M]){
#pragma HLS INLINE
  //go through rows fish out coefficients
  GetARowCoefficients_label0:for (unsigned i=0 ; i < M; i++)

#pragma HLS UNROLL
RC[i] = A[row_ix][i];
}


void
GetBRowCoefficients(short B[M][P], unsigned row, short BC[P]){
#pragma HLS INLINE
  //go through B row fish out coefficieints
  GetBRowCoefficients_label0:for (unsigned i=0; i < P; i++){
#pragma HLS UNROLL
    BC[i] = B[row][i];
  }
}



//Matrix Multiply in row order.

void matrixmultiply(short A[N][M], short B[M][P], short C[N][P]){
#pragma HLS ARRAY_PARTITION variable=C complete dim=2
#pragma HLS ARRAY_PARTITION variable=B complete dim=2
#pragma HLS ARRAY_PARTITION variable=A complete dim=2
#pragma HLS PIPELINE

 row_loop: for (unsigned i=0; i < N; i++){ //A row matrix loop
	 short a_row[M];
	 GetARowCoefficients(A, i, a_row);

	//initialize the acc
    long int acc[P];
    matrixmultiply_label1:for (unsigned j=0; j < P; j++)
    	acc[j]=0;

    //go through the B rows.
    brow_loop: for (unsigned j=0; j < M; j++){ // B row matrix loop
    	//choose a to be replicated.
    	short rep = a_row[j];
    	short b_row[P];
    	GetBRowCoefficients(B,j,b_row);
    	acc_loop: for (unsigned k=0; k< P; k++)
    		dsp_assign: acc[k] += rep * b_row[k];
    }
    result_loop:for (unsigned j=0; j< P; j++)
    	C[i][j]=acc[j];
 }
}

