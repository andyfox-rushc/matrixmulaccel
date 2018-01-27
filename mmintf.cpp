/*
//For standalone compilation
#include <stdlib.h>
#include <string.h>
#include "matrixmultiply.h"
void matrixmultiply(short A[N][M], short B[M][P], short C[N][P]);
*/

typedef struct converter_nm {
  union rep {
    short S2D_array [N][M];
    short* S2D_flat;
  }m_rep;
}ConverterNMStr;

typedef struct converter_mp {
  union rep {
    short S2D_array [M][P];
    short* S2D_flat;
  }m_rep;
}ConverterMPStr;

typedef struct converter_np {
  union rep {
    short S2D_array [N][P];
    short* S2D_flat;
  }m_rep;
}ConverterNPStr;


#define SIZE_NM_IN_BYTES  N*M*2
#define SIZE_MP_IN_BYTES  M*P*2
#define SIZE_NP_IN_BYTES  N*P*2
  
extern "C" void mmintf(short* A, short* B, short *C){
  ConverterNMStr converter1;
  ConverterMPStr converter2;
  ConverterNPStr converter3;
  memcpy(converter1.m_rep.S2D_flat, (short*) A, SIZE_NM_IN_BYTES);
  memcpy(converter2.m_rep.S2D_flat, (short*) B, SIZE_MP_IN_BYTES);
  matrixmultiply(converter1.m_rep.S2D_array,
		 converter2.m_rep.S2D_array,
		 converter3.m_rep.S2D_array);
  memcpy(C, (short*)converter3.m_rep.S2D_flat, SIZE_NP_IN_BYTES);
}
