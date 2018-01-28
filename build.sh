#To build the accelerator code:
xocc --target sw_emu --platform xilinx:kcu1500:4ddr-xpr:4.0 -k mmintf -o mmintf.bin matrixmultiply.cpp 


#To build the host interface
/opt/Xilinx/SDx/2017.2/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -fmessage-length=0  oclErrorCodes.cpp oclHelper.cpp host.cpp

/opt/Xilinx/SDx/2017.2/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "oclHelper.o" "oclHelper.cpp"

/opt/Xilinx/SDx/2017.2/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "oclErrorCodes.o" "oclErrorCodes.cpp"

/opt/Xilinx/SDx/2017.2/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.2/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.2/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "host.o" "host.cpp"

/opt/Xilinx/SDx/2017.2/bin/xcpp -o "mmintf.exe" host.o oclErrorCodes.o oclHelper.o -lxilinxopencl -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.2/runtime/lib/x86_64 -lxilinxopencl -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.2/runtime/lib/x86_64

/opt/Xilinx/SDx/2017.2/bin/emconfigutil --platform xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0
export XCL_EMULATION_MODE=sw_emu

#to run:
./mmintf.exe mmintf.bin
