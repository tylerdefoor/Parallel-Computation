## Compiling Instructions for PA04 </br>
Use the following commands to enter the build folder and compile the code: <br/>
cd build </br>
make </br> </br>

Stay in the build folder to sbatch the programs </br> </br>

## Special Instructions for Parallel and Sequential Code </br>
Both algorithms can be ran with differnt number of tests, displaying capabilites, and unique matrices. </br>
In order to display the matrices please set the global bool variable "PRINT" to true </br>
In order to run more than one test, please set the global int "TESTS" to whatever value you like  </br>
In order to generate unique matrices, set the gloabl bool "UNIQUE_MATIRX" to true. This variable was used for testing purposes </br>
so that I could generate the same matrix between parallel and sequential </br>
I recommend to keep TESTS to 1 and PRINT to false, and unique matrix to true </br> 
Furthermore, if you change the number of TESTS, you must also update the timing line in the parallel.sh and sequential.sh </br>
sbatch files depending on data size. If your data is too large and you change TESTS, then your job might cancel because </br>
it is taking too long. </br> </br>

## Running Instructions for Sequential </br>
Inside the build folder is a file named seq.sh </br>
Inside this file you can see the code 'm' followed by a number. This tells the rows/columns of the matrix </br>
You can change the number freely, but please keep the the 'm' infront of the number with no space between </br>
For example: m120 </br>
After you are satisfied, use sbatch seq.sh to put the job in the queue </br> </br>

## Running Instructions for Parallel </br>
Inside the build folder is a file named para.sh </br>
Inside this file you can see the code 'm' followed by a number. This tells the rows/columns of the matrix </br>
You can change the number freely, but please keep the the 'm' infront of the number with no space between </br>
For example: m120 </br>
The number of processors must be a perfect square, ie: 25 </br>
And the rows/cols of the matrix must be divisible by the square root of the number of processorsm </br>
otherwise the code will not work </br>
After you are satisfied, use sbatch para.sh to put the job in the queue </br> </br>

## Output </br>
The output goes to the bin folder that is in the root of PA04. To see the output, copy this folder to your local directory </br>
The output of sequential is seq.out The output for parallel is parallel.out
