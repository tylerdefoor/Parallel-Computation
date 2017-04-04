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

                //Check for the max
                if ( unsorted[j] > max )
                    max = unsorted[j]
            }

            MPI_Send ( unsorted, (totalNums / numBuckets), MPI_INT, i, 0, MPI_COMM_WORLD );
        }

        //Read in what the master takes care of
        for ( int i = 0; i < masterWidth; i++ )
        {
            fin >> unsorted;

            //Check for the max again
            if ( unsorted[i] > max )
                max = unsorted[i]
        }

        //For super secret purposes
        max += 10;

        //Broadcast the max to all processes
        MPI_Bcast ( &max, 1, MPI_INT, MASTER, MPI_COMM_WORLD );

        //Close the file
        fin.close (  );

        //Block because we all want to start at the same time
        MPI_Barrier ( MPI_COMM_WORLD );

        //Start the timer
        start = MPI_Wtime (  );

        //The bucket the number is supposed to go to
        int myBucket = max / numBuckets;

        //Create a vector of ints because vectors are great
        vector<int> buckets[numBuckets];

        //Put them in their buckets
        for ( int i = 0; i < totalNums; i++ ) 
            buckets[unsorted[i] / myBucket].push_back ( unsorted[i] );

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

        //Receive the max
        MPI_Bcast ( &max, 1, MPI_INT, MASTER, MPI_COMM_WORLD );

        //Block because we all want to start at the same time
        MPI_Barrier ( MPI_COMM_WORLD );

        //The bucket the number is supposed to go to
        int myBucket = max / numBuckets;

        //Create a vector of ints because vectors are great
        vector<int> buckets[numBuckets];

        //Put them in their buckets
        for ( int i = 0; i < totalNums; i++ ) 
            buckets[unsorted[i] / myBucket].push_back ( unsorted[i] );
    }
    //Finalize MPI
    MPI_Finalize();

    return 0;
}