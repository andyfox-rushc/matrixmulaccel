/*
  Matrix multiplication acceleration.
*/


//
//design for synthesis
//

#include "matrixmultiply.h"
#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
void 
GetARowCoefficients(short A[N_SZ][M_SZ], short row_ix, short RC[M_SZ]){
#pragma HLS INLINE
  //go through rows fish out coefficients
  GetARowCoefficients_label0:for (unsigned i=0 ; i < M_SZ; i++)
RC[i] = A[row_ix][i];
}


void
GetBRowCoefficients(short B[M_SZ][P_SZ], unsigned row, short BC[P_SZ]){
#pragma HLS INLINE
  //go through B row fish out coefficieints
  GetBRowCoefficients_label0:for (unsigned i=0; i < P_SZ; i++){
#pragma HLS UNROLL
    BC[i] = B[row][i];
  }
}

/*
void brow_loop(short &a_row [M_SZ],
	       short &B[M_SZ][P_SZ],
	       unsigned j,
	       short b_row& [P_SZ],
	       long int acc[P_SZ] &acc){
      	short rep = a_row[j];
    	short b_row[P_SZ];
    	GetBRowCoefficients(B,j,b_row);
    	acc_loop: for (unsigned k=0; k< P_SZ; k++)
    		dsp_assign: acc[k] += rep * b_row[k];
}
*/
//Matrix Multiply in row order.

void matrixmultiply(short *A_arg, short* B_arg, short* C_arg,unsigned char* done){
short A[N_SZ][M_SZ];
short B[M_SZ][P_SZ];
short C[N_SZ][P_SZ];
unsigned count=0;
#pragma HLS ARRAY_PARTITION variable=A_arg complete
#pragma HLS ARRAY_PARTITION variable=B_arg complete
#pragma HLS ARRAY_PARTITION variable=C_arg complete
*done = 0;
for (unsigned i =0; i < N_SZ; i++){
	for (unsigned j=0; j < M_SZ; j++){
		A[i][j]=A_arg[count];
		count++;
	}
}

count = 0;

for (unsigned i =0; i < M_SZ; i++){
	for (unsigned j=0; j < P_SZ; j++){
		B[i][j]=B_arg[count];
		count++;
	}
}

#pragma HLS PIPELINE

 row_loop: for (unsigned i=0; i < N_SZ; i++){ //A row matrix loop
	 short a_row[M_SZ];
	 GetARowCoefficients(A, i, a_row);

	//initialize the acc
    long int acc[P_SZ];
    matrixmultiply_label1:for (unsigned j=0; j < P_SZ; j++)
    	acc[j]=0;

    //go through the B rows.
    brow_loop: for (unsigned j=0; j < M_SZ; j++){ // B row matrix loop
    	//choose a to be replicated.
    	short rep = a_row[j];
    	short b_row[P_SZ];
    	GetBRowCoefficients(B,j,b_row);
    	acc_loop: for (unsigned k=0; k< P_SZ; k++)
    		dsp_assign: acc[k] += rep * b_row[k];
    }
    result_loop:for (unsigned j=0; j< P_SZ; j++){
    	C[i][j]=acc[j];

	count++;
    }
 }
 //copy out to result
 count =0;
 for (unsigned i =0 ;i < N_SZ; i++)
   for (unsigned j=0; j < P_SZ; j++){
     C_arg[count]=C[i][j];
     count++;
   }
 *done = 1;
 
}


//The interface to the design for synthesis
typedef struct converter_nm {
  union rep {
    short S2D_array [N_SZ][M_SZ];
    short* S2D_flat;
  }m_rep;
}ConverterNMStr;

typedef struct converter_mp {
  union rep {
    short S2D_array [M_SZ][P_SZ];
    short* S2D_flat;
  }m_rep;
}ConverterMPStr;

typedef struct converter_np {
  union rep {
    short S2D_array [N_SZ][P_SZ];
    short* S2D_flat;
  }m_rep;
}ConverterNPStr;


  
extern "C" void mmintf(short* A, short* B, short *C,unsigned char* done){

 /*

	 Proper INTERFACE pragmas are required for each function argument for the function
	 to work as a kernel in SDAccel environment.

	 The following pragmas are required for each pointer argument:
	 #pragma HLS INTERFACE m_axi port=<pointer_name> offset=slave bundle=gmem
	 #pragma HLS INTERFACE m_axi port=<pointer_name> offset=slave bundle=gmem

	 The following pragma is required for each scallar
	 #pragma HLS INTERFACE s_axilite port=<scalar_name> bundle=control

	 The following pragma is required for kernel control
	 #pragma HLS INTERFACE s_axilite port=return bundle=control

	 */
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=done offset=slave bundle=gmem  

#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=done bundle=control  

#pragma HLS INTERFACE s_axilite port=return bundle=control

  short A_short[N_SZ*M_SZ];
  short B_short[M_SZ*P_SZ];
  short C_short [N_SZ*P_SZ];
  unsigned char done_local=0;
  memcpy(A_short, (short*) A, SIZE_NM_IN_BYTES);
  memcpy(B_short, (short*) B, SIZE_MP_IN_BYTES);
  for (unsigned i=0; i < 1000000; i++){
  matrixmultiply(A_short,
  		 B_short,
  		 C_short,
		 &done_local);
  }
  memcpy(C, (short*)C_short, SIZE_NP_IN_BYTES);
  *done = done_local;
}


