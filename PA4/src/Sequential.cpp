/** @file Sequential.cpp
  * @brief A sequential implementation of Bucket Sort. 
  * @author Tyler DeFoor
  * @date 3/28/2017
  * @version 1.0
  */
 
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <algorithm>
#include "mpi.h"

#define MAX     100
#define SEED    42

using namespace std;

int main ( int argc, char** argv )
{

    /* Variable Declarations */
    //The filename
    //char* filename = argv[1];
    
    //Get total nums for random number generation
    int width = atoi(argv[1]);

    //The left, right, and resulting matrix
/*    int left[width][width];
    int right[width][width];
    int result[width][width];*/

    int** left;
    int** right;
    int** result;

    left = new int*[width];
    right = new int*[width];
    result = new int*[width];

    for ( int i = 0; i < width; i++ )
    {
        left[i] = new int[width];
        right[i] = new int[width];
        result[i] = new int[width];
    }

    //The start, end, and total time
    double start, end, total;

    //ifstream fin;

    /* End of Variable Declarations */

    //fin.open ( filename );

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Close the file
    //fin.close (  );
    
    //Seed the random number generator
    srand ( SEED );

    //Fill left and right with ints
    for ( int row = 0; row < width; row++ )
    {
        for ( int col = 0; col < width; col++ )
        {
            left[row][col] = rand (  ) % MAX;
            right[row][col] = rand (  ) % MAX;
        }
    }

    //Start the timer
    start = MPI_Wtime (  );

    //Loop through the rows
    for ( int i = 0; i < width; i++ )
    {
        //Loop through the columns
        for ( int j = 0; j < width; j++ )
        {
            //Set the result to 0
            result[i][j] = 0;

            //Multiply it
            for ( int k = 0; k < width; k++ )
                result[i][j] = result[i][j] + left[i][k] * right[k][j];
        }
    }

    //End the timer
    end = MPI_Wtime (  );
/*
    //Print out left array
    for ( int i = 0; i < width; i++ )
    {
        for ( int j = 0; j < width; j++ )
            cout << left[i][j] << " ";

        cout << endl;
    }

        cout << endl;
    //Print out right array
    for ( int i = 0; i < width; i++ )
    {
        for ( int j = 0; j < width; j++ )
            cout << right[i][j] << " ";

        cout << endl;
    }

        cout << endl;
    //Print out result array
    for ( int i = 0; i < width; i++ )
    {
        for ( int j = 0; j < width; j++ )
            cout << result[i][j] << " ";

        cout << endl;
    }
*/
    //Calculate the total time
    total = end - start;

    //Output the time for totalNums
    cout << "1 " << width << " " << total << endl;

    //Finalize MPI
    MPI_Finalize();

    return 0;
}
