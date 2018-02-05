echo "Building host program"

/opt/Xilinx/SDx/2017.1.op/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "oclHelper.o" "oclHelper.cpp"

/opt/Xilinx/SDx/2017.1.op/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "oclErrorCodes.o" "oclErrorCodes.cpp"
cd /opt/Xilinx/SD
/opt/Xilinx/SDx/2017.1.op/bin/xcpp -DC_KERNEL -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -std=c++14 -DSDX_PLATFORM=xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0 -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O0 -g -Wall -c -fmessage-length=0 -o "host.o" "host.cpp"

/opt/Xilinx/SDx/2017.1.op/bin/xcpp -o "mmintf.exe" host.o oclErrorCodes.o oclHelper.o -L/opt/Xilinx/SDx/2017.1.op/runtime/lib/x86_64  -lxilinxopencl -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64 -lxilinxopencl -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64





#/opt/Xilinx/SDx/2017.1.op/bin/xcpp -o "mmintf.exe" host.o oclErrorCodes.o oclHelper.o -lxilinxopencl.so -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64 -lxilinxopencl.so -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64
