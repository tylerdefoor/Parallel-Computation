/** @file Dynamic.cpp
  * @brief A dynamic implementation of Bucket Sort. 
  * @author Tyler DeFoor
  * @date 4/2/2017
  * @version 1.0
  */
 
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include "mpi.h"

#define MASTER      0
#define INT_TYPE    MPI_INT

using namespace std;

void bucketsort ( int* unsorted, int* &sorted, int max, int numBuckets, int totalNums );
void read ( char* filename, int totalNums );

int main ( int argc, char** argv )
{

    /* Variable Declarations */
    //The filename
    char* filename = argv[1];

    //The total number of elements, the maximum, and the range of the numbers
    int totalNums, numBuckets, max, taskid, leftovers;

    //The unsorted array of ints
    int* unsorted;

    //The start, end, and total time
    double start, end, total;

    ifstream fin;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Get the world size
    MPI_Comm_size ( MPI_COMM_WORLD, &numBuckets );

    //Get Rank
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    //If we are the master
    if ( taskid == MASTER )
    {
        //A try catch for opening files because I'm a good programmer
        try
        {
            fin.open ( filename );
        }
        catch ( const ifstream::failure& e )
        {
            cout << "File failure: Try again later" << endl;
        }
        //Get the total number of ints
        fin >> totalNums;

        //Broadcast the total numbers to all processes
        MPI_Bcast ( &totalNums, 1, MPI_INT, MASTER, MPI_COMM_WORLD );

        //The master takes care of the leftovers
        int masterWidth = ( totalNums / numBuckets ) + ( totalNums % numBuckets );

        //The unsorted array
        unsorted = new int[masterWidth];

        //Read in the numbers and send to the slaves
        for ( int i = 1; i < numBuckets; i++ )
        {
            for ( int j = 0; j < (totalNums / numBuckets); j++ )
            {
                fin >> unsorted;
            }

            MPI_Send ( unsorted, (totalNums / numBuckets), MPI_INT, i, 0, MPI_COMM_WORLD );
        }

        //Read in what the master takes care of
        while ( fin >> unsorted )
        {
            //I just want to read in the stuff and nothing else and that's a fancy way of doing it so here's a comment
        }

        //Close the file
        fin.close (  );

        //Start the timer
        start = MPI_Wtime (  );

        //End the timer
        end = MPI_Wtime (  );

        //Calculate the total time
        total = end - start;

        //Output the time for totalNums
        cout << totalNums << " " << total << endl;
    }
    //If we are a slave
    else
    {
        //Receive the total numbers
        MPI_Bcast ( &totalNums, 1, MPI_INT, MASTER, MPI_COMM_WORLD );

        //Allocate the unsorted array
        unsorted = new int[(totalNums / numBuckets)];

        //Receive the unsorted array
        MPI_Recv ( unsorted, (totalNums / numBuckets), MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }
    //Finalize MPI
    MPI_Finalize();

    return 0;
}