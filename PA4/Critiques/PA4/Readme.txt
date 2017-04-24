# PA4: Matrix Multiplication

## File Organization
All source files are stored in the src directory.
All object/executable files are stored in the exec directory.
Bash files passed to `sbatch` are stored in the bash directory. 
Timings for the program are stored in the timings directory. 
Files containing the input numbers for the matrices are in the gen directory.

Within `gen` there are files A and B, which correspond to the first and second matrices read into the program.
If necessary, these files may be overwritten by running the `generator` program. 
The matrix dimensions (120 x 120 for example) dictate the filename of said matrix (120.matrix). 

A 4 x 4 matrix has been produced for testing purposes to ensure matrix
multiplication was performed correctly. It may be read by the `parallel`
program and runs successfully using 4 processors (run with `Parallel_n4.sh`).

## Compiling
all: `make all` (compiles all available source code)
generator: `make generator`. 
parallel version of matrix multiplication: `make parallel`

## Running
Programs must be run within the `bash` directory.
`cd bash`
generator: `sbatch gen.sh N`
parallel: `sbatch Parallel_n$1.sh  v g N`
where: $1 corresponds to the number of cores to be used by the program. This can be: 4, 9, 16, 25, 36. (Not a parameter that can be passed in)
       v is the setting for the variable VERBOSE in the parallel program 
       g is the matrix data option: matrix data can be read from a file or generated randomly within the program. 
       N is the dimension for N x N matrices to be multiplied. 
       
Note: `generator` can take in a list of `N` to generate multiple matrix data files and place them into the appropriate directory.
For example: `sbatch gen.sh 120 240` will generate matrix data for matrices of
size 120 x 120 and 240 x 240.

## Options
Verbose: if a 1 is passed as an argument to `Parallel.sh` the matrices along with their product can be output to a file (`parallel.out`).      
         If 0 is passed, nothing is output.

Matrix data source: `read` in order to read in the data for the first and second matrix, or anything else to generate the numbers randomly within the program.

N: Dimension for square matrices.

Example usage for `parallel`: `sbatch Parallel_n4.sh 1 read 4`.
The above command will run the parallel program with 4 cores and will print out the result matrix in `parallel.out`. It will read two files for the A & B input matrices of size `4 x 4`.

Note: passing the incorrect number of arguments (3) or an unreadable/nonexistent file will cause the program to shut down and output an error message to `parallel.out`.

## Data

The data files included in the `gen` directory produce matrices of size:
4 x 4
120 x 120
240 x 240
360 x 360
480 x 480
600 x 600
720 x 720
840 x 840
960 x 960
1080 x 1080
1200 x 1200
1320 x 1320
1440 x 1440
1560 x 1560
1680 x 1680
1800 x 1800
1920 x 1920
2040 x 2040
2160 x 2160
2280 x 2280
2400 x 2400
2520 x 2520
2640 x 2640

Raw data from the `parallel` program is written to the timings directory. 
Parallel times are written in files according to the number of processes that were used.
For example: `parallel_n4.time` contains the timings for the parallel program running on four processes.

Note: timings are either written to a new file or appended to an existing file.

