/** @file Parallel.cpp
  * @brief A sparallel implementation of Matrix Multiplication. The program assumes the matrices are square and there are a square number of processors
  * @author Tyler DeFoor
  * @date 4/25/2017
  * @version 1.3
  */
 
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <vector>
#include "mpi.h"
#include <cmath>

#define MAX     1000
#define SEED    42
#define MASTER  0
#define LEFT    0
#define RIGHT   1
#define RESULT  2

using namespace std;

int matrixWidth, size;
char* fileName;
bool toFile = true;

//The width of the submatrices, the total size of the submatrices, and the processor width
int subWidth, subSize, procWidth;

//The left, right, and result matrices
int* left;
int* right;
int* result;

//Timer values
double start, end;

//The taskid and world size
int taskid, worldSize;

void printFile ( int* left, int* right, int* result );
void printMatrix ( int* matrix, int width );
void parallelMultiplication (  );
void toSlave ( int row, int col, int* matrix, int* &dest );
int getRow ( int taskid, int sqrWorldSize );
void generateMatrix ( int* &matrix );
int getCol ( int taskid, int sqrWorldSize );
void toResult ( int row, int col, int* subMatrix, int* &dest );
void shiftLeft ( int* &matrix, int rotations, int myRow, int myCol) ;
void shiftUp ( int* &matrix, int rotations, int myRow, int myCol );

//The main function - Boom. There's some documentation
int main ( int argc, char* argv[] ) 
{
    //Set that sweet sweet buffer
    setvbuf( stdout, NULL, _IONBF, 0 );

    //Seed the random number generator
    srand ( SEED );

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    fileName = argv[1];

    matrixWidth = atoi ( argv[2] );

    //Get the taskid and world size
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );

    //Matrix size is the total # of ints int the matrix
    size = matrixWidth * matrixWidth;

    //Main stays nice and clean this way
    parallelMultiplication (  );

    //Finalize MPI because we're good programmers
    MPI_Finalize (  );
    return 0;
}

 /**parallelMultiplication
 *@fn void parallelMultiplication (  ) 
 *@brief Does the entire parallelMultiplication
 *@param N/A
 *@return void
 *@pre Everything is initialized correctly
 *@post The matrices are multiplied
 */
void parallelMultiplication (  ) 
{

    //The number of nodes per row/col
    procWidth = sqrt(worldSize);

    //Check to see that everything is as it should be
    if ( procWidth > matrixWidth || ( matrixWidth % procWidth ) != 0 || ( procWidth * procWidth ) != worldSize ) 
    {
        printf << "Invalid configuration";
        return;
    }

    //Width of the submatrix
    subWidth = matrixWidth / procWidth;

    //The matrices are square, so this is the total size
    subSize = subWidth * subWidth;

    //This nodes subMatrices
    int* leftSub = new int[subSize] (  );
    int* rightSub = new int[subSize] (  );
    int* resultSub = new int[subSize] (  );

    //If we are the master
    if ( taskid == MASTER ) 
    {
        //Generate the left and right matrices
        generateMatrix ( left );
        generateMatrix ( right );

        //Distribute Matrix A and B to all slaves
        int* tempLeft = new int[subSize];
        int* tempRight = new int[subSize];

        //Loop through the rows
        for(int i = 0; i < procWidth; i++) 
        {
            //Loop through the columns
            for(int j = 0; j < procWidth; j++) 
            {
                //If we are the master
                if ( i == 0 && j == 0 ) 
                {
                    //Get our stuff
                    toSlave ( 0, 0, left, leftSub );
                    toSlave ( 0, 0, right, rightSub );

                    //Super fancy continue statement
                    continue;
                }

                //Split the matrices and send them
                toSlave ( i, j, left, tempLeft );
                toSlave ( i, j, right, tempRight );

                //Send to the receiving nodes
                MPI_Send ( tempLeft, subSize, MPI_INT, ( i * procWidth + j ), LEFT, MPI_COMM_WORLD );
                MPI_Send ( tempRight, subSize, MPI_INT, ( i * procWidth + j ), RIGHT, MPI_COMM_WORLD );
            }
        }

        //Wait for all nodes to receive their submatrices
        MPI_Barrier ( MPI_COMM_WORLD );

        //Deleting dynamic arrays because I'm a genius coder
        delete tempLeft;
        delete tempRight;
    }

    //If we are a slave
    else 
    {
        //Receive left and right
        MPI_Recv ( leftSub, subSize, MPI_INT, 0, LEFT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv ( rightSub, subSize, MPI_INT, 0, RIGHT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

        //Girl gimme that barrier
        MPI_Barrier ( MPI_COMM_WORLD );
    }

    //Hit 'em with the wall
    MPI_Barrier ( MPI_COMM_WORLD );

    //Start the timer
    if ( taskid == MASTER ) 
        start = MPI_Wtime (  );

    //Get the row and column
    int myRow = getRow ( taskid, procWidth );
    int myCol = getCol ( taskid, procWidth );

    //Perform the preproccessing of the matrices
    //Hey dude, I don't get it either. In Cannon we trust
    shiftLeft ( leftSub, myRow, myRow, myCol );
    shiftUp ( rightSub, myCol, myRow, myCol );

    //Make sure everyone is on the same page
    MPI_Barrier ( MPI_COMM_WORLD );

    //Begin the multiplication
    for(int n = 0; n < procWidth; n++) 
    {
        //Perform that matrix multiplication
        for(int i = 0; i < subWidth; i++) 
        {
            for(int j = 0; j < subWidth; j++) 
            {
                for(int k = 0; k < subWidth; k++) 
                    resultSub[i*subWidth+j] += leftSub[i * subWidth + k] * rightSub[j + subWidth * k];
            }
        }

        //Rotate sub matrcies [A left 1] and [B up 1]
        shiftLeft ( leftSub, 1, myRow, myCol );
        shiftUp ( rightSub, 1, myRow, myCol );
    }

    //We did it friends!
    if ( taskid == MASTER ) 
    {
        end = MPI_Wtime (  );
        printf ( "%d %d %.6f \n", worldSize, matrixWidth, end - start );
    }

    //If we output to a file
    if ( toFile ) 
    {
        //If we are the master
        if ( taskid == MASTER ) 
        {
            //The full final matrix
            result = new int[size] (  );

            //Temporary for sending/receiving
            int* tempResult = new int[subSize] (  );

            //Add Master's result submatrix to the full result matrix
            toResult ( 0, 0, resultSub, result );

            //The row and column for the node in question
            int nodeRow, nodeCol;

            //Send the submatrices to the resulting matrix
            for ( int i = 1; i < worldSize; i++ ) 
            {
                //Receive the result submatrix
                MPI_Recv ( tempResult, subSize, MPI_INT, i, RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

                //Find currents node's row and column
                nodeRow = getRow ( i, procWidth );
                nodeCol = getCol ( i, procWidth );

                //Add the result submatrix to the full result matrix
                toResult ( nodeRow, nodeCol, tempResult, result );

                //Sync with other nodes before next send
                MPI_Barrier ( MPI_COMM_WORLD );
            }
            //Write to the file
            printFile ( left, right, result );
        }

        //If we are a slave
        else 
        {
            //Loop through all the slaves
            for ( int i = 1; i < worldSize; i++ ) 
            {
                //Send your result
                if ( i == taskid ) 
                    MPI_Send ( resultSub, subSize, MPI_INT, 0, RESULT, MPI_COMM_WORLD ); 

                //Wait for everyone else
                MPI_Barrier ( MPI_COMM_WORLD );
            }
        }
    }
}

 /**toResult
 *@fn void toResult ( int row, int col, int* subMatrix, int* &dest ) 
 *@brief Sends the submatrix back to the result
 *@param row The row that we're sending to
 *@param col The column we're sending to
 *@param subMatrix The submatrix we're sending
 *@param dest The destination matrix
 *@return void
 *@pre The submatrix holds relevant data and all are intialized
 *@post dest holds all of subMatrix in its rightful place
 */
void toResult ( int row, int col, int* subMatrix, int* &dest ) 
{
    //The index in the large destination matrix
    int destID = row * matrixWidth * subWidth + col * subWidth;

    //For each row in the subMatrix copy it to the full matrix
    for ( int i = 0; i < subWidth; i++ ) 
    {
        //Look at that, being super duper fancy with the memcpy
        memcpy ( &dest[destID], &subMatrix[i * subWidth], sizeof(int) * subWidth );

        //Increment destID to the next row
        destID += matrixWidth;
    }
}

 /**toSlave
 *@fn void toSlave ( int row, int col, int* matrix, int* &dest ) 
 *@brief Gets the submatrix to send to the slave
 *@param row The row of the slave
 *@param col The column of the slave
 *@param matrix The matrix to send
 *@param dest The destination matrix
 *@return void
 *@pre All parameters are intialized
 *@post dest holds matrix
 */
void toSlave ( int row, int col, int* matrix, int* &dest ) 
{
    //Indices for the matrices
    int destID = 0;
    int index;

    //Copy each row into the matrix
    for ( int i = 0; i < subWidth; i++ ) 
    {
        //Get the 1d index
        index = row * subWidth * matrixWidth + i * matrixWidth + col * subWidth;

        //Memcpy because its super duper fast
        memcpy ( &dest[destID], &matrix[index], sizeof(int) * subWidth );

        //Move on to the next one
        destID += subWidth;
    }
}

 /**shiftLeft
 *@fn void shiftLeft ( int* &matrix, int rotations, int myRow, int myCol ) 
 *@brief Shifts the matrix left
 *@param matrix The matrix to send/receive from
 *@param rotations The number of rotations
 *@param myRow The row of the sender
 *@param myCol The column of the sender
 *@return void
 *@pre The destination exists
 *@post The destination holds what we had, and we have the next matrix
 */
void shiftLeft ( int* &matrix, int rotations, int myRow, int myCol ) 
{
    //The destination
    //I figured out how to do it all fancy like!
    int dest = myRow * procWidth + ( myCol + procWidth - rotations ) % procWidth;

    //The source node
    int source = myRow * procWidth + ( myCol + rotations ) % procWidth;

    //Definitely use this. It's superdy duperdy handy
    MPI_Sendrecv_replace(matrix, subSize, MPI_INT, dest, LEFT, source, LEFT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}

 /**shiftUp
 *@fn void shiftUp(int* &matrix, int rotations, int myRow, int myCol) 
 *@brief Shifts the matrix up
 *@param matrix The matrix to send/receive from
 *@param rotations The number of rotations
 *@param myRow The row of the sender
 *@param myCol The column of the sender
 *@return void
 *@pre The destination exists
 *@post The destination holds what we had, and we have the next matrix
 */
void shiftUp(int* &matrix, int rotations, int myRow, int myCol) 
{
    //The destination
    int dest = ( ( myRow + procWidth - rotations ) % procWidth) * procWidth + myCol;

    //The source
    int source = ( ( myRow + rotations ) % procWidth ) * procWidth + myCol;

    //See above superdy duperdy comment
    MPI_Sendrecv_replace ( matrix, subSize, MPI_INT, dest, RIGHT, source, RIGHT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
}

 /**generateMatrix
 *@fn void generateMatrix(int* &matrix) 
 *@brief Generates the matrix
 *@param matrix The matrix to generate
 *@return void
 *@pre N/A
 *@post matrix holds data fo multiplication
 */
void generateMatrix(int* &matrix) 
{
    matrix = new int[size];

    for ( int i = 0; i < size; i++ ) 
        matrix[i] = rand (  ) % MAX;

}

 /**printMatrix
 *@fn void printMatrix ( int* matrix, int width ) 
 *@brief Prints out the given matrix
 *@param matrix The matrix to print out
 *@param width The width of said matrix
 *@return void
 *@pre matrix is instantiated
 *@post Nothing in matrix is changed
 */
void printMatrix ( int* matrix, int width ) 
{
    for ( int i = 0; i < width; i++ ) 
    {
        for ( int j = 0; j < width; j++ ) 
            printf("%d ", matrix[i * width + j]);

        //Gotta pretend we're 2D
        cout << endl;
    }

}

 /**printFile
 *@fn void printFile ( int* left, int* right, int* resultSub ) 
 *@brief Prints to a file
 *@param left The left matrix
 *@param right The right matrix
 *@param resultSub The result matrix
 *@return void
 *@pre left, right, and resultSub are instantiated
 *@post Nothing in left, right, and resultSub are changed
 */
void printFile ( int* left, int* right, int* resultSub ) 
{
    //It's fout. It's what it's all about
    fstream fout;

    //Open the file
    fout.open ( fileName );

    //Print out the left matrix
    fout << "Left" << endl;

    for ( int i = 0; i < matrixWidth; i++ ) 
    {
        for ( int j = 0; j < matrixWidth; j++ ) 
            fout << left[i * matrixWidth + j] << " ";

        fout << endl;
    }

    //Print out the right matrix
    fout << "Right" << endl;

    for ( int i = 0; i < matrixWidth; i++ ) 
    {
        for ( int j = 0; j < matrixWidth; j++ ) 
            fout << right[i * matrixWidth + j] << " ";

        fout << endl;
    }

    //Print out the result
    fout << "Result" << endl;

    for ( int i = 0; i < matrixWidth; i++ ) 
    {
        for ( int j = 0; j < matrixWidth; j++ )
            fout << resultSub[i * matrixWidth + j] << " ";

        fout << endl;
    }

    //Close the file because we're genius programmers
    fout.close (  );
}

 /**getRow
 *@fn int getRow ( int taskid, int sqrWorldSize )
 *@brief Gets the row of the process taskid
 *@param taskid The taskid of the process
 *@param sqrWorldSize The square root of the world size
 *@return The row, an int
 *@pre N/A
 *@post taskid and sqrWorldSize are not changed
 */
int getRow ( int taskid, int sqrWorldSize )
{
    return taskid / sqrWorldSize;
}

 /**getCol
 *@fn int getCol ( int taskid, int sqrWorldSize )
 *@brief Gets the column of the process taskid
 *@param taskid The taskid of the process
 *@param sqrWorldSize The square root of the world size
 *@return The column, an int
 *@pre N/A
 *@post taskid and sqrWorldSize are not changed
 */
int getCol ( int taskid, int sqrWorldSize )
{
    return taskid % sqrWorldSize;
}