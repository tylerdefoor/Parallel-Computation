// parallel.cpp
//
// This file contains a collection of functions for running matrix
// multiplication in parallel using Canon's algorithm.
//
// 4/20/2017


#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mpi.h"


static const int MASTER = 0;

static const int MAT_A_TAG = 0; // When working with matrix A
static const int MAT_B_TAG = 1; // When working with matrix B


// Data structure to hold a coordinate pair
struct Coords {
    int i, j;
    Coords(int i, int j) { this->i=i; this->j=j; }
};


// ==================== Prototypes ====================


// The main function to be executed by MASTER.
void master(
        int dim, int minNum, int maxNum, bool gatherAndPrint, int numWorkers);


// The main function to be executed by slaves.
void slave(
        int dim, int minNum, int maxNum, bool gatherAndPrint, int numWorkers,
        int workerId);


// Distribute submatrices to workers from MASTER, keeping the first one for
// MASTER.
void distributeSubmatrices(
        int submatDim, int minNum, int maxNum, int numWorkers, int* submatA,
        int* submatB);


// Each slave collects the submatrix distributed to it from MASTER.
void collectSubmatrices(int submatDim, int* submatA, int* submatB);


// Perform parallelized matrix multiplication. This worker is responsible for
// computing the inner product between its designated row and column, placing
// the result in submatResult.
void matrixMultiply(
        int* submatResult, int* submatA, int* submatB, int submatDim,
        int sqrtNumWorkers, int workerId);


// Send the contents of submatA to the worker to the left, and recieve
// elements from the worker to the right, storing them into submatA.
void rotateLeft(
        int* submatA, int myRow, int myCol, int sqrtNumWorkers,
        int submatDim);


// Send the contents of submatB to the worker to the up, and recieve
// elements from the worker to the down, storing them into submatB.
void rotateUp(
        int* submatB, int myRow, int myCol, int sqrtNumWorkers,
        int submatDim);


// Perform a normal, sequential matrix multiplication betweeen submatA and
// submatB, placing the results into submatResult.
void submatrixMultiply(
        int* submatResult, int* submatA, int* submatB, int submatDim);


// Perform an elementwise sum between submatA and submatB, placing the results
// into submatResult.
void matrixSum(int* submatResult, int* submatA, int* submatB, int submatDim);


// Given a workerId, compute the coordinates of that worker's result submatrix.
Coords getCoordsFromId(int sqrtNumWorkers, int workerId);


// Given a coordinate pair, compute the workerId of the worker to which the
// result submatrix at that coordinate pair belongs.
int getIdFromCoords(int sqrtNumWorkers, Coords submatCoords);


// Given a matrix mat, its size dim, a row index i, and a column index j,
// return a references to the (i,j) cell of mat.
int& getCellRef(int* mat, int dim, int i, int j);


// Given a matrix mat, iterate through mat and fill it with random numbers.
void generateMatrix(int* mat, int dim, int minNum, int maxNum);


// Given a matrix mat, iterate through mat and print it to the screen.
void printMatrix(int* mat, int dim, const char* matName);


// ==================== Main ====================


// Generate two square matrices of random integers, split them up into
// submatrices in number equal to the number of workers available, distribute
// those submatrices to workers, and perform parallelized matrix multiplication
// using Canon's algorithm.
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    srand(0);

    // The unique ID for this worker
    int workerId;
    MPI_Comm_rank(MPI_COMM_WORLD, &workerId);

    // Get arguments
    if(argc < 4) {
        if(workerId == MASTER) {
            printf(
                "Please give dim (int), minNum (int), maxNum (int), and "
                "gatherAndPrint (1 for true, 0 for false) as command line "
                "args.\n");
        }
        MPI_Finalize();
        return 1;
    }
    int dim = atoi(argv[1]);
    int minNum = atoi(argv[2]);
    int maxNum = atoi(argv[3]);
    bool gatherAndPrint = (bool)atoi(argv[4]);

    // How many workers?
    int numWorkers;
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);
    if(dim % numWorkers != 0) {
        if(workerId == MASTER) {
            printf(
                "Please make sure the dimension of the matrices is "
                "divisible by the number of workers.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Let MASTER and slaves do work
    if(workerId == MASTER)
        master(dim, minNum, maxNum, gatherAndPrint, numWorkers);
    else
        slave(dim, minNum, maxNum, gatherAndPrint, numWorkers, workerId);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}


// ==================== Definitions ====================


void master(
        int dim, int minNum, int maxNum, bool gatherAndPrint, int numWorkers) {
    // Calculate sqrtNumWorkers. O(n) algorithm is fine for this size.
    int sqrtNumWorkers = 1;
    while(sqrtNumWorkers * sqrtNumWorkers < numWorkers)
        ++sqrtNumWorkers;

    int submatDim = dim / sqrtNumWorkers;
    int submatDimSq = submatDim * submatDim;

    // Distribute submatrices to workers, keeping the first ones here at
    // MASTER
    int* submatA = new int[submatDimSq];
    int* submatB = new int[submatDimSq];
    distributeSubmatrices(
        submatDim, minNum, maxNum, numWorkers, submatA, submatB);

    // Wait for everybody to have their submatrices
    MPI_Barrier(MPI_COMM_WORLD);

    // Multiply the matrices into a result matrix of the same dimensions
    int* submatResult = new int[submatDimSq];
    double startTime = MPI_Wtime();

    matrixMultiply(
        submatResult, submatA, submatB, submatDim, sqrtNumWorkers, MASTER);

    double stopTime = MPI_Wtime();

    // Display results
    double computationTime = stopTime - startTime;
    printf("%d,%lf\n", dim, computationTime);

    delete[] submatA;
    delete[] submatB;
    delete[] submatResult;
}


void slave(
        int dim, int minNum, int maxNum, bool gatherAndPrint, int numWorkers,
        int workerId) {
    // Calculate sqrtNumWorkers. O(n) algorithm is fine for this size.
    int sqrtNumWorkers = 1;
    while(sqrtNumWorkers * sqrtNumWorkers < numWorkers)
        ++sqrtNumWorkers;

    int submatDim = dim / sqrtNumWorkers;
    int submatDimSq = submatDim * submatDim;

    // Collect submatrices
    int* submatA = new int[submatDimSq];
    int* submatB = new int[submatDimSq];
    void collectSubmatrices(
        int submatDim, int minNum, int maxNum, int numWorkers, int* submatA,
        int* submatB);

    // Wait for everybody to have their submatrices
    int* submatResult = new int[submatDimSq];
    MPI_Barrier(MPI_COMM_WORLD);

    // Multiply the two matrices
    matrixMultiply(
        submatResult, submatA, submatB, submatDim, sqrtNumWorkers, workerId);

    delete[] submatA;
    delete[] submatB;
    delete[] submatResult;
}


void distributeSubmatrices(
        int submatDim, int minNum, int maxNum, int numWorkers, int* submatA,
        int* submatB) {
    int submatDimSq = submatDim * submatDim;
    for(int slaveId = 1; slaveId < numWorkers; ++slaveId) {
        // Generate the submatrices
        generateMatrix(submatA, submatDim, minNum, maxNum);
        generateMatrix(submatB, submatDim, minNum, maxNum);

        // Send the submatrices to workers
        MPI_Send(
            submatA, submatDimSq, MPI_INT, slaveId, MAT_A_TAG, MPI_COMM_WORLD);
        MPI_Send(
            submatB, submatDimSq, MPI_INT, slaveId, MAT_B_TAG, MPI_COMM_WORLD);
    }
    generateMatrix(submatA, submatDim, minNum, maxNum);
    generateMatrix(submatB, submatDim, minNum, maxNum);
}


void collectSubmatrices(int submatDim, int* submatA, int* submatB) {
    int submatDimSq = submatDim * submatDim;
    MPI_Recv(
        submatA, submatDimSq, MPI_INT, MASTER, MAT_A_TAG, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
    MPI_Recv(
        submatB, submatDimSq, MPI_INT, MASTER, MAT_B_TAG, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
}


void matrixMultiply(
        int* submatResult, int* submatA, int* submatB, int submatDim,
        int sqrtNumWorkers, int workerId) {
    // Calculate all coordinates and worker IDs necessary to perform the
    // algorithm.
    Coords myCoords = getCoordsFromId(sqrtNumWorkers, workerId);
    int myRow = myCoords.i;
    int myCol = myCoords.j;

    // Initialize
    for(int it = 0; it < myRow; ++it) {
        // Rotate A left
        rotateLeft(submatA, myRow, myCol, sqrtNumWorkers, submatDim);
    }
    for(int it = 0; it < myCol; ++it) {
        // Rotate B up
        rotateUp(submatB, myRow, myCol, sqrtNumWorkers, submatDim);
    }

    // Multiply the submatrices together
    submatrixMultiply(submatResult, submatA, submatB, submatDim);
    
    // Continue forming the inner product of my row and column of submatrices
    int* tmpSubmatResult = new int[submatDim * submatDim];
    for(int it = 1; it < sqrtNumWorkers; ++it) {
        // Get next pair of submatrices to multiply
        rotateLeft(submatA, myRow, myCol, sqrtNumWorkers, submatDim);
        rotateUp(submatB, myRow, myCol, sqrtNumWorkers, submatDim);

        // Multiply them
        submatrixMultiply(tmpSubmatResult, submatA, submatB, submatDim);

        // Add to inner product
        matrixSum(submatResult, submatResult, tmpSubmatResult, submatDim);
    }

    delete[] tmpSubmatResult;
}


void rotateLeft(
        int* submatA, int myRow, int myCol, int sqrtNumWorkers,
        int submatDim) {
    int submatDimSq = submatDim * submatDim;

    int toColLeft = myCol-1 + (myCol==0 ? sqrtNumWorkers : 0);
    int fromColRight = (myCol+1) % sqrtNumWorkers;

    int toWorkerLeft = getIdFromCoords(
        sqrtNumWorkers, Coords(myRow, toColLeft));
    int fromWorkerRight = getIdFromCoords(
        sqrtNumWorkers, Coords(myRow, fromColRight));

    // Send from and receive into submatA buffer
    MPI_Sendrecv_replace(
        submatA, submatDimSq, MPI_INT, toWorkerLeft, MAT_A_TAG, 
        fromWorkerRight, MAT_A_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


void rotateUp(
        int* submatB, int myRow, int myCol, int sqrtNumWorkers,
        int submatDim) {
    int submatDimSq = submatDim * submatDim;

    int toRowUp = myRow-1 + (myRow==0 ? sqrtNumWorkers : 0);
    int fromRowDown = (myRow+1) % sqrtNumWorkers;

    int toWorkerUp = getIdFromCoords(
        sqrtNumWorkers, Coords(toRowUp, myCol));
    int fromWorkerDown = getIdFromCoords(
        sqrtNumWorkers, Coords(fromRowDown, myCol));

    // Send from and receive into submatB buffer
    MPI_Sendrecv_replace(
        submatB, submatDimSq, MPI_INT, toWorkerUp, MAT_B_TAG,
        fromWorkerDown, MAT_B_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


void submatrixMultiply(
        int* submatResult, int* submatA, int* submatB, int submatDim) {
    for(int i = 0; i < submatDim; ++i) {
        for(int j = 0; j < submatDim; ++j) {
            int innerProd = 0;
            for(int k = 0; k < submatDim; ++k)
                innerProd +=
                    getCellRef(submatA,submatDim,i,k) *
                    getCellRef(submatB,submatDim,k,j);
            getCellRef(submatResult,submatDim,i,j) = innerProd;
        }
    }
}


void matrixSum(int* submatResult, int* submatA, int* submatB, int submatDim) {
    for(int i = 0; i < submatDim; ++i)
        for(int j = 0; j < submatDim; ++j)
            getCellRef(submatResult, submatDim, i, j) =
                getCellRef(submatA, submatDim, i, j) +
                getCellRef(submatB, submatDim, i, j);
}


Coords getCoordsFromId(int sqrtNumWorkers, int workerId) {
    return Coords(workerId/sqrtNumWorkers, workerId%sqrtNumWorkers);
}


int getIdFromCoords(int sqrtNumWorkers, Coords submatCoords) {
    return submatCoords.i * sqrtNumWorkers + submatCoords.j;
}


int& getCellRef(int* mat, int dim, int i, int j) {
    return mat[i*dim + j];
}


void generateMatrix(int* mat, int dim, int minNum, int maxNum) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j)
            getCellRef(mat,dim,i,j) = random()%maxNum + minNum;
    }
}


void printMatrix(int* mat, int dim, const char* matName) {
    printf("Matrix %s:\n", matName);
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j)
            printf("%d\t", getCellRef(mat,dim,i,j));
        printf("\n");
    }
}

