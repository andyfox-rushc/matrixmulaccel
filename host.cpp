
/*
  Copyright language and logic inc.
  Demonstration kernel for acceleration.
  Host for matrix multiply
 */


#include <getopt.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>
#include <cassert>
#include "oclHelper.h"
#include "matrixmultiply.h"


#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <math.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

#define DATA_SIZE_NM_IN_BYTES  (N_SZ*M_SZ*2)
#define DATA_SIZE_MP_IN_BYTES  (M_SZ*P_SZ*2)
#define DATA_SIZE_NP_IN_BYTES  (N_SZ*P_SZ*2)

extern "C" void mmintf(short* A, short* B, short *C,unsigned char* done);
void matrixmultiply(short *A_arg, short* B_arg, short* C_arg,unsigned char* done);
/*
Test harness
*/

#include <stdlib.h>
#include <stdio.h>

#include "matrixmultiply.h"




//test bench stuff

void DumpMatrix(const char* name,
		unsigned row_count,
		unsigned col_count,
		short* matrix
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


void multiplyOut(short A[N_SZ][M_SZ],
		short B[M_SZ][P_SZ], unsigned row, unsigned col, unsigned a_col_count, short C[N_SZ][P_SZ]){
  short result=0;

  for (unsigned i = 0 ; i < a_col_count; i++){
    result += A[row][i]*B[i][col];
  }
  C[row][col] = result;
}

int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}



void matrix_multiply_tb(short A[N_SZ][M_SZ],
			short B[M_SZ][P_SZ],
			short C[N_SZ][P_SZ]){
  printf("Running test bench for matrix multiply\n");
  printf("+++++++++++++++++++++++++++++++++++++++\n");
  DumpMatrix("A",N_SZ,M_SZ,(short*)A);
  DumpMatrix("B",M_SZ,P_SZ,(short*)B);

  clock_t begin_clock = clock();

  unsigned iteration_count = 1000000;
  for (unsigned i=0 ; i < iteration_count; i++){
  for (unsigned row_ix=0; row_ix< N_SZ; row_ix++){ //row
    for (unsigned col_ix=0; col_ix < P_SZ; col_ix++){//col (result is N x P)
      multiplyOut(A,B,row_ix,col_ix, M_SZ /*a col size */,C);
    }
  }
  }
  clock_t end_clock = clock();
  double time_spent = (double)(end_clock - begin_clock)/CLOCKS_PER_SEC;
  printf("CPU time (secs)%f \n", time_spent);

  DumpMatrix("C = A*B",N_SZ,P_SZ,(short*)C);
  printf("-----------------------------------------\n");
}

  bool
  CheckEquivalenceOfMatrices(    short *m1  , short *m2 , unsigned r_limit, unsigned c_limit){
    for (unsigned r=0 ; r < r_limit; r++)
      for (unsigned c=0; c< c_limit; c++)
	if ((*(m1+ r*c)) != (*(m2 + r*c)))
	  return false;
    return true;
  }



/*
hw to test
 */

//Matrix Multiply in row order.

void 
GetARowCoefficients_hw(short A[N_SZ][M_SZ], short row_ix, short RC[M_SZ]){
  //go through rows fish out coefficients
  GetARowCoefficients_label0:for (unsigned i=0 ; i < M_SZ; i++)
RC[i] = A[row_ix][i];
}

void
GetBRowCoefficients_hw(short B[M_SZ][P_SZ], unsigned row, short BC[P_SZ]){
  //go through B row fish out coefficieints
  GetBRowCoefficients_label0:for (unsigned i=0; i < P_SZ; i++){
    BC[i] = B[row][i];
  }
}


void matrixmultiply_hw(short *A_arg, short* B_arg, short* C_arg){
short A[N_SZ][M_SZ];
short B[M_SZ][P_SZ];
short C[N_SZ][P_SZ];
unsigned count=0;

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
count=0;
 row_loop: for (unsigned i=0; i < N_SZ; i++){ //A row matrix loop
	 short a_row[M_SZ];
	 GetARowCoefficients_hw(A, i, a_row);

	//initialize the acc
    long int acc[P_SZ];
    matrixmultiply_label1:for (unsigned j=0; j < P_SZ; j++)
    	acc[j]=0;

    //go through the B rows.
    brow_loop: for (unsigned j=0; j < M_SZ; j++){ // B row matrix loop
    	//choose a to be replicated.
    	short rep = a_row[j];
    	short b_row[P_SZ];
    	GetBRowCoefficients_hw(B,j,b_row);
    	acc_loop: for (unsigned k=0; k< P_SZ; k++)
    		dsp_assign: acc[k] += rep * b_row[k];
    }
    result_loop:for (unsigned j=0; j< P_SZ; j++){
    	C[i][j]=acc[j];
    	C_arg[count]=acc[j];
    	count++;

    }
 }
}

extern "C" void mmintf_hw(short* A, short* B, short *C){
  short A_short[N_SZ*M_SZ];
  short B_short[M_SZ*P_SZ];
  short C_short [N_SZ*P_SZ];
  memcpy(A_short, (short*) A, SIZE_NM_IN_BYTES);
  memcpy(B_short, (short*) B, SIZE_MP_IN_BYTES);
  matrixmultiply_hw(A_short,
		 B_short,
		 C_short);
  memcpy(C, (short*)C_short, SIZE_NP_IN_BYTES);
}



int main( int argc, char* argv[] ){

  if (argc !=2){
    std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
    return EXIT_FAILURE;
  }
  
  char* xclbinFileName = argv[1];

  //compute the size of the array in bytes
  std::vector<short> source_a(N_SZ*M_SZ);
  std::vector<short> source_b(M_SZ*P_SZ);
  std::vector<short> source_results(N_SZ*P_SZ);

  short tb_a[N_SZ][M_SZ];
  short tb_b[M_SZ][P_SZ];
  short tb_c[N_SZ][P_SZ];
  short tb1_c[N_SZ][P_SZ];
  short source_a_sarray[N_SZ*M_SZ];
  short source_b_sarray[M_SZ*P_SZ];
  
  short count =0;
  for (unsigned i=0; i < N_SZ;i++){
	  for (unsigned j=0; j < M_SZ; j++){
	    source_a[count] = count;
	    tb_a[i][j]=count;
	    count++;
	  }
  }
  count =0;
  for (unsigned i=0; i < M_SZ;i++){
  	  for (unsigned j=0; j < P_SZ; j++){
	    source_b[count] = count;
	    tb_b[i][j]=count;
	    count++;
  	  }
    }


  matrix_multiply_tb(tb_a,tb_b,tb_c);
  
  //mmintf_hw((short*)source_a.data(),(short*)source_b.data(),(short*)tb1_c);
  //  DumpMatrix("HW: C = A*B",N_SZ,P_SZ,(short*)tb1_c);

  

  // Getting First Platform
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  cl::Platform platform = platforms[0];
  std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";

  // Getting ACCELERATOR Devices and selecting 1st such device
  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
  cl::Device device = devices[0];
  std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

  // Creating Context and Command Queue for selected device
  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

  // Load 
  std::cout << "Loading: '" << xclbinFileName << "'\n";
  std::ifstream bin_file(xclbinFileName, std::ifstream::binary);
  bin_file.seekg (0, bin_file.end);
  unsigned nb = bin_file.tellg();
  bin_file.seekg (0, bin_file.beg);
  char *buf = new char [nb];
  bin_file.read(buf, nb);

// Creating Program from Binary File
  cl::Program::Binaries bins;
  bins.push_back({buf,nb});
  devices.resize(1);
  cl::Program program(context, devices, bins);

  // These commands will allocate memory on the FPGA. The cl::Buffer objects can
  // be used to reference the memory locations on the device. The cl::Buffer
  // object cannot be referenced directly and must be passed to other OpenCL
  // functions.
  
  int err;
  cl::Kernel kernel(program,"mmintf",&err);
  if (err != CL_SUCCESS){
    std::cout << "Error Failed to make kernel\n";
    return EXIT_FAILURE;
  }

  cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, source_a.size()*sizeof(short));
  cl::Buffer buffer_b(context, CL_MEM_READ_ONLY, source_b.size()*sizeof(short));
  cl::Buffer buffer_result(context,  CL_MEM_WRITE_ONLY, DATA_SIZE_NP_IN_BYTES);
  cl::Buffer buffer_done(context, CL_MEM_WRITE_ONLY,1);

  if (q.enqueueWriteBuffer(buffer_a, CL_TRUE,0, source_a.size()*sizeof(short),source_a.data())!= CL_SUCCESS){
    std::cout << "Error filling buffer\n";
    return EXIT_FAILURE;
  }
    
  if (q.enqueueWriteBuffer(buffer_b, CL_TRUE,0,source_b.size()*sizeof(short),source_b.data()) != CL_SUCCESS){
    std::cout << "Error filling buffer\n";
    return EXIT_FAILURE;
  }

  clock_t begin_clock = clock();
  kernel.setArg(0,buffer_a);
  kernel.setArg(1,buffer_b);
  kernel.setArg(2,buffer_result);
  kernel.setArg(3,buffer_done);  

  //kernel does 1000000 mults  
  q.enqueueTask(kernel);
  q.finish();
  
  //read back the results
  q.enqueueReadBuffer(buffer_result,CL_TRUE,0,DATA_SIZE_NP_IN_BYTES, source_results.data() );

  clock_t end_clock = clock();
  double time_spent = (double)(end_clock - begin_clock)/CLOCKS_PER_SEC;
  printf("Accelerator time (secs)%f \n", time_spent);

  bool mis_match = false;
  count =0;
  unsigned short_ix = 0;
  for (unsigned  i = 0; i < N_SZ; i = i +1){
    for (unsigned j=0; j < P_SZ; j= j+1){
      short result = source_results[count];
      count++;
      if (result != tb_c[i][j]){
	std::cout << "Expected " << tb_c[i][j] << " Actual " << result << "\n";
	mis_match = true;
      }
    }
  }
  std::cout << "TEST " << (mis_match ? "FAILED" : "PASSED") << std::endl; 
  return (mis_match ? EXIT_FAILURE :  EXIT_SUCCESS);
}
