# Tyler DeFoor
# PA4 - Matrix Multiplication

The overall report is Report.pdf. The raw execution time for the Sequential portion of the program is contained in Sequential.txt. The src folder contains both source files and build contains the makefile and the shell scripts. FancySequential.sh is used for overall testing purposes and should be run with bash rather than sbatch. Sequential.sh can be run with sbatch.

## Sequential
A sequential implementation of Matrix Multiplication. 

Run instructions
1. Navigate to build
2. make
3. sbatch Sequential.sh

## Parallel
A parallel implementation of Matrix Multiplication. The matrix size must be a square, and the number of processors must be square. The matrix size must be divisible by the square root of the number of processors. i.e. with 9 processors any multiple of 3 works, but it cannot be coprime with 3. A multiple of 3 the number shall be. A multiple of 4 is unaccptable. A multiple of 5 is right out. The number must be a multiple of the square root of the processors.

To run your own tests, the command line argument rules are as follows:
1. There must be 4 arguments
2. The first is the output file
3. The second is the left matrix
4. The third is the right matrix
5. The fourth is the square matrix width

Run instructions
1. Navigate to build
2. make
3. sbatch Parallel.sh
