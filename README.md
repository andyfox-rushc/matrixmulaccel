# MatrixMulAccel
## Large scale matrix multiplication acceleration
### Algorithm
The material in this repository allows one to run 100,000 15x20 matrix multiplies in hw. The measured performance is 1000 times faster than the CPU equivalent. The multiplication uses row access (that is to say rather than doing the usual row/column access for multiply we access both coefficient memories in row order). The inner loop is:

```
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
    }
 }
 ```

###How to build
Building has two parts: host construction and accelerator construction. Both need to be run on the Amazon instance (I used a c4.xlarge to do the build).
For the host:

* ./ahbuild.sh

For the accelerator

* ./abuild.sh

###How to run
Running on amazon is tricky and requires setting up so-called s3 buckets. Assuming you have successfully compiled to an xclbin then it is necessary to:

* First create an afi image. This is done using the create_sdaccel_afi.sh program.
>cd $AWS_FPGA_REPO_DIR

>source ./sdaccel_setup.sh

> source $XILINX_SDX/settings64.sh

>SDACCEL_DIR/tools/create_sdaccel_afi.sh -xclbin=mmintf.bin -o=mmintf -s3_bucket=rushc -s3_dcp_key=key -s3_logs_key=logs

* Check the afi image id.
> cat *afi_id.txt

* Check to see if the image id is ready:

> aws ec2 describe-fpga-images --fpga-image-ids afi-0081cd4e3d5577bb6

If the code state is "available" you are good to run on the F1 !


* Running on the fpga
  * Change the instance from c4 large to f1.2xlarge
  * Clear any images in the fpga slot:
   * sudo fpga-clear-local-image -S 0

* Launch the runtime system and execute, as root:
 * > source /opt/Xilinx/SDx/2017.1.rte/setup.sh
 * > ./mmintf.exe mmintf.awsxclbin


### Files

* matrixmultiply.cpp/.h -- the accelerator
* host.cpp -- the core of the host program
* setupnotes.txt notes on using Amazon.
