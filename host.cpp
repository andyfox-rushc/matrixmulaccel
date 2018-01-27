
/*
  Copyright language and logic inc.
  Demonstration kernel for acceleration.
  Host for matrix multiply
 */

#include "matrixmultiply.h"


#define DATA_SIZE_NM_IN_BYTES  N*M*2
#define DATA_SIZE_MP_IN_BYTES  M*P*2
#define DATA_SIZE_NP_IN_BYTES  N*P*2



int main( int argc, char* argv[] ){

  if (argc !=2){
    std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
    return EXIT_FAILURE;
  }
  
  char* xclbinFileName = argv[1];

  //compute the size of the array in bytes
  vector<short,aligned_allocator<short>> source_a(DATA_SIZE_NM_IN_BYTES, 2);
  vector<short,aligned_allocator<int>> source_b(DATA_SIZE_MP_IN_BYTES, 2);
  vector<short,aligned_allocator<int>> source_results(DATA_SIZE_NP_IN_BYTES);

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
  std::cout << "Loading: '" << xclbinFilename << "'\n";
  std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
  bin_file.seekg (0, bin_file.end);
  unsigned nb = bin_file.tellg();
  bin_file.seekg (0, bin_file.beg);
  char *buf = new char [nb];
  bin_file.read(buf, nb);


  // These commands will allocate memory on the FPGA. The cl::Buffer objects can
  // be used to reference the memory locations on the device. The cl::Buffer
  // object cannot be referenced directly and must be passed to other OpenCL
  // functions.
    cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  
            DATA_SIZE_NM_IN_BYTES, source_a.data());
    cl::Buffer buffer_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  
            DATA_SIZE_MP_IN_BYTES, source_b.data());
    cl::Buffer buffer_result(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            DATA_SIZE_NP_IN_BYTES, source_results.data());
    
    //Separate Read/write Buffer vector is needed to migrate data between host/device
    std::vector<cl::Memory> inBufVec, outBufVec;
    inBufVec.push_back(buffer_a);
    inBufVec.push_back(buffer_b);
    outBufVec.push_back(buffer_result);
    
    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    devices.resize(1);
    cl::Program program(context, devices, bins);
    
    // These commands will load the source_a and source_b vectors from the host
    // application and into the buffer_a and buffer_b cl::Buffer objects. The data
    // will be be transferred from system memory over PCIe to the FPGA on-board
    // DDR memory.
    q.enqueueMigrateMemObjects(inBufVec,0/* 0 means from host*/);

    
    // This call will extract a kernel out of the program we loaded in the
    // previous line. A kernel is an OpenCL function that is executed on the
    // FPGA. This function is defined in the src/vetor_addition.cl file.
    cl::Kernel krnl_mm(program,"mmintf");
    
    //set the kernel Arguments
    int narg=0;
    krnl_mm.setArg(narg++,buffer_result);
    krnl_mm.setArg(narg++,buffer_a);
    krnl_mm.setArg(narg++,buffer_b);
    krnl_mm.setArg(narg++,DATA_SIZE);

    //Launch the Kernel
    q.enqueueTask(krnl_mm);

    // The result of the previous kernel execution will need to be retrieved in
    // order to view the results. This call will write the data from the
    // buffer_result cl_mem object to the source_results vector
    q.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    /*
    int match = 0;
    printf("Result = \n");
    for (int i = 0; i < DATA_SIZE; i++) {
        int host_result = source_a[i] + source_b[i];
        if (source_results[i] != host_result) {
            printf(error_message.c_str(), i, host_result, source_results[i]);
            match = 1;
            break;
        } else {
          printf("%d ", source_results[i]);
          if (((i + 1) % 16) == 0) printf("\n");
        }
    }
    */
    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);

    

}
