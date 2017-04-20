/** @file Sequential.cpp
  * @brief A sequential implementation of Bucket Sort. The program assumes the matrices are square and there are a square number of processors
  * @author A Mystery - But they sure are handsome and a great programmer
  * @date 4/18/2017
  * @version 1.0
  */
 
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <vector>
#include "mpi.h"
#include <cmath>

#define MAX     100
#define SEED    42
#define MASTER  0

using namespace std;

int getRow ( int taskid, int sqrWorldSize );
int getCol ( int taskid, int sqrWorldSize );
int getID ( int row, int col, int sqrWorldSize );
int get2d ( int row, int col );
void sendToSlaves ( int worldSize, int** left, int** right, int width );
int shift ( int destRow, int destCol, vector<int>& source, int sqrWorldSize );
void multiply ( vector<int> left, vector<int> right, int** result, int row, int col, int width );

//The main function - Boom. There's some documentation
int main ( int argc, char** argv )
{

    /* Variable Declarations */
    
    //Get total nums for random number generation
    int width = atoi(argv[1]);

    //The taskID, square root of the world size, the world size, row and column, width of the left and right arrays for processes
    //and the left and up partners for the processes
    int taskid, sqrWorldSize, worldSize, myRow, myCol, procWidth, myLeft, myUp;

    //Vectors to hold the left and right 
    vector<int> left (  );
    vector<int> right (  );    

    //Everyone has their representation of the resulting vector
    int** result;

    result = new int*[width];
    for ( int i = 0; i < width; i++ )
        result[i] = new int[width];

    //Set the result matrix to 0s
    for ( int i = 0; i < width; i++ )
    {
        for ( int j = 0; j < width; j++ )
            result[i][j] = 0;
    }

    //The start, end, and total time
    double start, end, total;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Get the World size
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );

    //Get the square root of the world size for super special reasons
    sqrWorldSize = sqrt ( worldSize );

    //Get our rank in the world
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    //Get my row and column
    myRow = getRow ( taskid, sqrWorldSize );
    myCol = getCol ( taskid, sqrWorldSize );

    //This is the width of each processor's left and right matrix
    procWidth = width / sqrWorldSize;

    //If we are the master
    if ( taskid == MASTER )
    {
        //Seed the random number generator
        srand ( SEED );

        //The generated left and right matrices
        //Where's this Temple FT, am I right? Guys?
        int** templeft;
        int** tempright;

        //Is it the temple of feet? The temple featuring a new int?
        templeft = new int*[width];
        tempright = new int*[width];

        for ( int i = 0; i < width; i++ )
        {
            templeft[i] = new int[width];
            tempright[i] = new int[width];
        }

        //Fill templeft and tempright with ints
        for ( int row = 0; row < width; row++ )
        {
            for ( int col = 0; col < width; col++ )
            {
                templeft[row][col] = rand (  ) % MAX;
                tempright[row][col] = rand (  ) % MAX;
            }
        }

        //Get the master's left and right matrices
        for ( int i = 0; i < procWidth; i++ )
        {
            for ( int j = 0; j < procWidth; j++ )
            {
                left.push_back ( templeft[i][j] );
                right.push_back ( tempright[i][j] );
            }
        }

        //Send the slaves their submatrices
        sendToSlaves ( worldSize, templeft, tempright, procWidth );

        //Free up the space for templeft and tempright, as they are now useless
        //I will forever remember you Temple FT
        for ( int i = 0; i < width; i++ )
        {
            delete [] templeft[i];
            delete [] tempright[i];
        }
        delete [] templeft;
        delete [] tempright;

        MPI_Barrier ( MPI_COMM_WORLD );

        //Start the timer
        start = MPI_Wtime (  );
    }
    //If we are a slave
    else
    {
        //Receive the left and the right from master
        MPI_Recv ( &left[0], procWidth * procWidth, MPI_INT, MASTER, MPI_ANY_TAG, 0, MPI_STATUS_IGNORE );
        MPI_Recv ( &right[0], procWidth * procWidth, MPI_INT, MASTER, MPI_ANY_TAG, 0, MPI_STATUS_IGNORE );

        MPI_Barrier ( MPI_COMM_WORLD );
    }    

    //Calculate the space to the left and up from us
    myLeft = myCol - 1;
    myUp = myRow - 1;

    //Neither row nor column can be 0, so wrap back around
    if ( myLeft < 0 )
        myLeft = sqrWorldSize - 1;
    if ( myUp < 0 )
        myUp = sqrWorldSize - 1;

    //Preprocess the left matrix
    if ( myRow != 0 )
    {   
        //I know there is fancy modulo math to make this prettier. I'm just not sure what
        int destCol = myCol - myRow;

        if ( destCol < 0 )
            destCol += sqrWorldSize;

        //Shift the left matrix to its destination, which is the process' row number to the left
        //  with wraping
        shift ( myRow, destCol, left, sqrWorldSize );
    }

    //Preprocess the right matrix
    if ( myCol != 0 )
    {
        //See excuse above 
        int destRow = myRow - myCol;

        if ( destRow < 0 )
            destRow += sqrWorldSize;

        //Shift the right matrix to its destination, which is the process' column number up
        //  with wraping
        shift ( destRow, myCol, right, sqrWorldSize );
    }

    for ( int i = 0; i sqrWorldSize - 1; i++ )
    {
        //Multiply the matrices and put them in result
        multiply ( left, right, result, myRow * procWidth, myCol * procWidth, procWidth );

        //Shift A to the left
        shift ( myRow, myLeft, left, sqrWorldSize );

        //Shift B to the right
        shift ( myUp, myCol, right, sqrWorldSize );
    }

    if ( taskid == MASTER )
    {
        //End the timer
        end = MPI_Wtime (  );

        //Calculate the total time
        total = end - start;

        //Output the time for totalNums
        cout << width << " " << total << endl;
    }
    
    //Finalize MPI
    MPI_Finalize();

    return 0;
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

 /**getID
 *@fn int getID ( int row, int col, int sqrWorldSize )
 *@brief Gets the row of the process taskid
 *@param row The row representation of the taskid
 *@param col The column representation of the taskid
 *@param sqrWorldSize The square root of the world size
 *@return The taskid, an int
 *@pre N/A
 *@post taskid and sqrWorldSize are not changed
 */
int getID ( int row, int col, int sqrWorldSize )
{
    return row * sqrWorldSize + col;
}

 /**get2d
 *@fn int get2d ( int row, int col, int width )
 *@brief Gets the 1D representation of 2D coordinates for an array
 *@param row The row
 *@param col The column
 *@param width The width of the array
 *@return The 1D representation of a 2D array coordinate
 *@pre N/A
 *@post row, col, and width are not changed
 */
int get2d ( int row, int col, int width )
{
    return width * row + col;
}

 /**sendToSlaves
 *@fn void sendToSlaves ( int worldSize, int** left, int** right, int width )
 *@brief Sends the appropriate data from the master to the slaves
 *@param worldSize The size of the communicator world
 *@param left The left matrix
 *@param right The right matrix
 *@param width The width of the arrays
 *@return N/A
 *@pre left and right hold the left and right matrices
 *@post Nothing in any of the parameters is changed
 */
void sendToSlaves ( int worldSize, int** left, int** right, int width )
{
    int destRow, destCol;
    vector<int> templeft ( width * width );
    vector<int> tempright ( width * width );

    //Loop through all the slaves, starting at taskid 1
    for ( int currentSlave = 1; currentSlave < worldSize; currentSlave++ )
    {
        destRow = getRow ( currentSlave );
        destCol = getRow ( currentSlave );

        //Loop through the left and right matrices
        //All slaves get an equal portion, which starts at their column * width and row * width
        for ( int i = destRow * width; i < i + width; i++ )
        {
            for ( int j = destCol * width; j < j + width; j++ )
            {
                templeft.push_back ( left[i][j] );
                tempright.push_back ( right[i][j] );
            }
        }

        //Send the left and then the right
        MPI_Send ( &templeft.front (  ), templeft.size (  ), MPI_INT, currentSlave, 0, MPI_COMM_WORLD );
        MPI_Send ( &tempright.front (  ), tempright.size (  ), MPI_INT, currentSlave, 0, MPI_COMM_WORLD );
    }
}

 /**shift
 *@fn int shift ( int destRow, int destCol, vector<int>& source, int sqrWorldSize )
 *@brief Sends the source vector to another process
 *@param destRow The "row" of the destination
 *@param destCol The "column" of the destination
 *@param source The source vector
 *@param sqrWorldSize 
 *@return The tag of the receiving process
 *@pre There is a process with row and column destRow and destCol that will eventually receive and something will eventually send to this process
 *@post The vector source holds the information passed from another process. No other parameters are changed
 */
int shift ( int destRow, int destCol, vector<int>& source, int sqrWorldSize )
{
    MPI_Status status;

    //Calculate the destination
    int dest = getID ( destRow, destCol, sqrWorldSize );

    //Send and receive, using source as the only buffer
    //If you're reading this, use this method to be SUPER DUPER EFFICIENT
    MPI_Sendrecv_replace ( &source.front (  ), source.size (  ), MPI_INT, dest, 0, MPI_ANY_SOURCE,
                            MPI_ANY_TAG, MPI_COMM_WORLD, &status );

    //Return the tag from the sender, just for debugging purposes
    return status.MPI_TAG;
}

 /**multiply
 *@fn void multiply ( vector<int> left, vector<int> right, int** result, int row, int col, int width )
 *@brief Multiplies the left and right matrices together and puts them in the result
 *@param left The left matrix
 *@param right The right matrix
 *@param result The resulting matrix
 *@param row The initial row
 *@param col The initial column
 *@param width The width of the submatrix we are calculating
 *@return N/A
 *@pre Both left and right hold relevant data
 *@post result holds the multiplied matrix. No other parameters are changed
 */
void multiply ( vector<int> left, vector<int> right, int** result, int row, int col, int width )
{
    //Loop through the rows
    for ( int i = 0; i < width; i++ )
    {
        //Loop through the columns
        for ( int j = 0; j < width; j++ )
        {
            //Multiply it
            for ( int k = 0; k < width; k++ )
                result[i + row][j + col] += left[get2d(i,j)] * right[get2d(i,j)];
        }
    }
}