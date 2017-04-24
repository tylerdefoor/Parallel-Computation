// OVERVIEW: This is a parallel implementation of matrix multiplication using 
// cannon's algorithm.
// This involves creating a mesh from the proccesses and aligning the matrices 
// by means of shifting left or upwards.
// This implementation only deals with square matrices that can be evenly split
// among processes.

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MASTER 0

using namespace std;

// flags
bool VERBOSE;
bool SHUTDOWN = false;

int SHUTDOWN_TAG = 3;

// multiplies matrix A and matrix B and adds result into result matrix
void matrixMultiplication(unsigned long long int* result, unsigned long long int* A, 
                          unsigned long long int* B, size_t size);

// pretty print a 1D array
void printMatrix(unsigned long long int* matrix, size_t size);

// randomly generates numbers to fill matrix A and matrix B
void fillMatrices(unsigned long long int* A, unsigned long long int* B, size_t size);

// reads in numbers from a file to fill matrix A and matrix B
bool readMatrices(unsigned long long int* A, unsigned long long int* B, size_t size);

// performs left shifting of a row or upwards shifting of a column by computing
// the destination process and source process
void shift(int rowSplitKey, int colSplitKey, unsigned long long int* chunk, 
           size_t chunkSize, long long int shiftCount, int numtasks, 
           string shiftType, int rank);

// copies rightArr into leftArr
void arrCopy(unsigned long long int* leftArr, size_t leftSize,
             unsigned long long int* rightArr, size_t rightSize, int rank, int numtasks);
// deallocates memory if necessary
void cleanupMatrix(unsigned long long int* matrix);

int main(int argc, char* argv[])
{
  int rank, numtasks, count;

  if(argc < 4)
  {
    cout << "Not enough arguments" << endl;
    cout << "Usage example with Sbatch: sbatch Parallel.sh 1 read 4" << endl;
    return -1;
  }

  // MPI initialization
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int source, dest;
  long long int val;
  double start, finish;
  size_t size, chunkSize, sqrt_p, rowSplitKey, colSplitKey, shiftCount;
  int a = 0; 
  int b = 0;
  string filename;
  
  MPI_Status status;
  MPI_Request request;

  // options:
  // VERBOSE: print out initial matrices and result of matrix mult
  // genOption: fill matrices with input files or generate random numbers
  // size: N dimension for square N x N matrices
  VERBOSE = atoi(argv[1]);
  string genOption = string(argv[2]);
  size = atoi(argv[3]);

  // M dimension for individual processor's M x M square matrices
  // M < N
  chunkSize = size/sqrt(numtasks);

  sqrt_p = sqrt(numtasks);
  // compute what row and column processor belongs to in mesh
  rowSplitKey = rank/sqrt(numtasks);
  colSplitKey = rank%sqrt_p;

  // unsigned long to avoid overflow
  // set to Null initially for cleanup at end
  unsigned long long int* matrixA = NULL;
  unsigned long long int* matrixB = NULL;
  unsigned long long int* matrixC = NULL;
  unsigned long long int* chunkA = NULL;
  unsigned long long int* chunkB = NULL;
  unsigned long long int* tempChunk = NULL;
  unsigned long long int* result = NULL;


  // allocate all chunks to be used
  chunkA = new unsigned long long int[chunkSize*chunkSize];
  chunkB = new unsigned long long int[chunkSize*chunkSize];
  tempChunk = new unsigned long long int[chunkSize*chunkSize];

  // to store accumulating sum
  result = new unsigned long long int[chunkSize*chunkSize];
  for(size_t i = 0; i < chunkSize*chunkSize; i++)
  {
    result[i] = 0;
  }

  if(rank == MASTER)
  {
    matrixA = new unsigned long long int[size*size];
    matrixB = new unsigned long long int[size*size];

    // read matrix numbers from files OR randomly generate numbers
    if(genOption == "read")
    {
      if(!readMatrices(matrixA, matrixB, size))
      {
        cout << "Error: Data file(s) not found in gen/ directory to fill matrices.\n"
             << "Use generator program to create matrix data to be read in" << endl;

        // flag in order to send signal to other processes to shut down
        SHUTDOWN = true;
      }
    }
    else
    {
      fillMatrices(matrixA, matrixB, size);
    }

    dest = 1;

    // distribute chunks for matrix A and matrix B to each corresponding
    // processor
    // goes from left to right: processor 0 gets 1st chunk, processor 1 next,
    // etc...
    while(dest < numtasks)
    {
      // send shut down tag to shut down other processes
      if(SHUTDOWN)
      {
        MPI_Send(chunkA, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, dest++, 
                 SHUTDOWN_TAG, MPI_COMM_WORLD);
        continue;
      }

      // copy matrix chunks to separate array for sending
      arrCopy(chunkA, chunkSize, matrixA, size, dest, numtasks);
      arrCopy(chunkB, chunkSize, matrixB, size, dest, numtasks);

      MPI_Send(chunkA, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, dest, a, 
               MPI_COMM_WORLD);
      MPI_Send(chunkB, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, dest, b, 
               MPI_COMM_WORLD);

      dest++;
    }

    if(SHUTDOWN)
    {
      cout << "Process " << rank << " shutting down." << endl;
      MPI_Finalize();
      return -1;
    }

    // copy respective chunks belonging to master
    arrCopy(chunkA, chunkSize, matrixA, size, MASTER, numtasks);
    arrCopy(chunkB, chunkSize, matrixB, size, MASTER, numtasks);

    // wait for all processes to recieve their chunks
    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();
  }
  else
  {
    MPI_Recv(chunkA, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, MASTER, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);

    // check for shutdown tag sent from master
    if(status.MPI_TAG == SHUTDOWN_TAG)
    {
      cout << "Process " << rank << " shutting down." << endl;
      MPI_Finalize();
      return -1;
    }

    MPI_Recv(chunkB, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, MASTER, b,
             MPI_COMM_WORLD, &status);

    // wait for all slaves to receive chunks
    MPI_Barrier(MPI_COMM_WORLD);
  }

  // shift row i, i cols to the left
  shiftCount = rowSplitKey;
  shift(rowSplitKey, colSplitKey, chunkA, chunkSize, shiftCount, numtasks, "left", rank);

  // shift col j, j rows upwards
  shiftCount = colSplitKey;
  shift(rowSplitKey, colSplitKey, chunkB, chunkSize, shiftCount, numtasks, "upwards", rank);

  // matrix multiplication and circular shifting
  shiftCount = 1;
  for(int k = 0; k < sqrt_p; k++)
  {
    matrixMultiplication(result, chunkA, chunkB, chunkSize);
    shift(rowSplitKey, colSplitKey, chunkA, chunkSize, shiftCount, numtasks, "left", rank);
    shift(rowSplitKey, colSplitKey, chunkB, chunkSize, shiftCount, numtasks, "upwards", rank);
  }
  
  // wait for all processes to finish matrix multiplication
  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == MASTER)
  {
    // all processes have computed their chunks, stop time
    finish = MPI_Wtime();

    filename = "../timings/parallel_n" + to_string(numtasks) + ".time";

    // append time to timings file for corresponding parallel file
    // for example: parallel_n4.time for 4 tasks
    FILE* fp = fopen(filename.c_str(), "a+");
    fprintf(fp, "%f\n", finish-start);
    fclose(fp);
  }

  // print out original matrices and product if(VERBOSE)
  {
    if(rank == MASTER)
    {
      cout << "******************\n";
      cout << "Original matrix A\n";
      printMatrix(matrixA, size);

      cout << "Original matrix B\n"; printMatrix(matrixB, size);
      cout << "******************" << endl;

      count = numtasks - 1;
      matrixC = new unsigned long long int[size*size];

      // receive all matrix mult. chunks from slaves and place into big result matrix
      while(count > 0)
      {
        MPI_Recv(tempChunk, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 
                 MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        source = status.MPI_SOURCE;

        arrCopy(matrixC, size, tempChunk, chunkSize, source, numtasks);

        count--;
      }

      // copy in master's result chunk to big result matrix
      arrCopy(matrixC, size, result, chunkSize, MASTER, numtasks);

      printMatrix(matrixC, size);
    }
    else
    {
      // send result chunk to master
      MPI_Send(result, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, MASTER, 0, MPI_COMM_WORLD);
    }

  }

  // cleanup allocated memory
  cleanupMatrix(matrixA);
  cleanupMatrix(matrixB);
  cleanupMatrix(matrixC);
  cleanupMatrix(chunkA);
  cleanupMatrix(chunkB);
  cleanupMatrix(result);
  cleanupMatrix(tempChunk);

  MPI_Finalize();

  return 0;
}

// Matrix mult. complexity: size**3
// Mutliplies matrix A and matrix B
// result matrix passed in to keep adding to accumulating sum
void matrixMultiplication(unsigned long long int* result, unsigned long long int* A, 
                          unsigned long long int* B, size_t size)
{
  unsigned long long int c;

  size_t row, col, k;
  for(row = 0; row < size; row++)
  {
    for(col = 0; col < size; col++)
    {
      c = 0;
      // submatrix multiplication 
      for(k = 0; k < size; k++)
      {
        // add to accumulating submatrx
        c += A[row*size+k]*B[k*size+col];
      }
      result[row*size+col] += c;
    }
  }

}

// pretty print 1D array as 2D matrix
void printMatrix(unsigned long long int* matrix, size_t size)
{
  for(size_t row = 0; row < size; row++)
  {
    for(size_t col = 0; col < size; col++)
    {
      cout << matrix[row*size+col] << " ";
    }
    cout << "\n";
  }
}

// read in to matrix A & matrix B from files according to size provided
// for ex: if N=120 file for matrix A: in ../gen/A/120.matrix
// returns false if one of the files does not exist
bool readMatrices(unsigned long long int* A, unsigned long long int* B, size_t size)
{
  string matrixAFile = "../gen/A/" + to_string(size) + ".matrix";
  string matrixBFile = "../gen/B/" + to_string(size) + ".matrix";

  ifstream a_in(matrixAFile);
  
  if(!a_in)
    return false;

  for(int i = 0; i < size; i++)
  {
    for(int j = 0; j < size; j++)
    {
      a_in >> A[i*size+j];
    }
  }

  a_in.close();

  ifstream b_in(matrixBFile);

  if(!b_in)
    return false;

  for(size_t i = 0; i < size; i++)
  {
    for(size_t j = 0; j < size; j++)
    {
      b_in >> B[i*size+j];
    }
  }

  b_in.close();

  return true;
}

// generate random numbers from 0-10 to fill in matrices
void fillMatrices(unsigned long long int* A, unsigned long long int* B, size_t size)
{
  srand(time(NULL));

  unsigned long long int MAX_SIZE = 10;
  unsigned long  long int val;

  for(int i = 0; i < size; i++)
  {
    for(int j = 0; j < size; j++)
    {
      val = rand() % MAX_SIZE;
      A[i*size+j] = val;
      val = rand() % MAX_SIZE;
      B[i*size+j] = val;
    }
  }
}

// shift either left or upwards (depending on shiftType passed in)
// shift shiftCount amount of times and compute dest and source accordingly for 
// sending and receiving chunks
// prevRow/prevCol and nextRow/nextCol computed by taking into account that wrap around
// is possible
void shift(int rowSplitKey, int colSplitKey, unsigned long long int* chunk, 
           size_t chunkSize, long long int shiftCount, int numtasks, string shiftType, int rank)
{
  unsigned long long int* tempChunk = new unsigned long long int[chunkSize*chunkSize];
  long long int prevCol, nextCol, prevRow, nextRow;
  int dest, source;
  long long int numrows = sqrt(numtasks);
  long long int numcols = sqrt(numtasks);
  int a = 0;
  int b = 1;
  MPI_Status status;

  if(shiftType == "left")
  {
    // accounts for wraparound (when modulo result is negative for getting
    // prevCol)
    prevCol = ((colSplitKey-shiftCount) % numcols + numcols) % numcols;
    nextCol = ((colSplitKey+shiftCount) % numcols + numcols) % numcols;

    // compute dest & source according to position in 2D mesh
    dest = rowSplitKey*numrows+prevCol;
    source = rowSplitKey*numrows+nextCol;
  }
  else
  {
    // accounts for wraparound (when modulo result is negative for getting
    // prevRow)
    prevRow = ((rowSplitKey-shiftCount) % numcols + numcols) % numcols;
    nextRow = ((rowSplitKey+shiftCount) % numcols + numcols) % numcols;

    // compute dest and source according to position in 2D mesh
    dest = prevRow*numrows+colSplitKey;
    source = nextRow*numrows+colSplitKey;
  }

  // blocking send and recieve packaged together to prevent deadlock
  // use different buffers
  MPI_Sendrecv(chunk, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, dest, a,
               tempChunk, chunkSize*chunkSize, MPI_UNSIGNED_LONG_LONG, source, a,
               MPI_COMM_WORLD, &status);


  // copy in received chunk to processor's chunk
  for(size_t i = 0; i < chunkSize*chunkSize; i++)
  {
    chunk[i] = tempChunk[i];
  }

  // cleanup
  delete[] tempChunk;
}

// copy smaller matrix into bigger matrix or vice versa
// dependent on the array sizes passed in
void arrCopy(unsigned long long int* leftArr, size_t leftSize,
             unsigned long long int* rightArr, size_t rightSize, int rank, int numtasks)
{
  size_t size = max(leftSize, rightSize);
  size_t chunkSize = min(leftSize, rightSize);
  size_t matrixCol;

  // compute bigger matrix row, col according to processor rank num
  int row = rank/sqrt(numtasks);
  int col = rank%int(sqrt(numtasks));
  int matrixRow = row*chunkSize;
  int matrixStartCol = col*chunkSize;
  
  // figure out whether bigger matrix is on left or right side of equation
  bool matrixLeft = false;
  if(size == leftSize)
    matrixLeft = true;

  for(size_t chunkRow = 0; chunkRow < chunkSize; chunkRow++)
  {
    matrixCol = matrixStartCol;
    for(size_t chunkCol = 0; chunkCol < chunkSize; chunkCol++)
    {
      if(matrixLeft)
        leftArr[matrixRow*size+matrixCol] = rightArr[chunkRow*chunkSize+chunkCol];
      else
        leftArr[chunkRow*chunkSize+chunkCol] = rightArr[matrixRow*size+matrixCol];

      matrixCol++;
    }
    matrixRow++;
  }
}

// cleanup allocated memory if necessary 
void cleanupMatrix(unsigned long long int* matrix)
{
  if(matrix != NULL)
  {
    delete[] matrix;
    matrix = NULL;
  }
}
