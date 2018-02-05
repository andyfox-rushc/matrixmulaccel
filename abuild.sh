#To build the accelerator code:
xocc --target hw --platform /home/centos/src/project_data/aws-fpga/SDAccel/aws_platform/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0.xpfm -k mmintf -o mmintf.bin matrixmultiply.cpp 


