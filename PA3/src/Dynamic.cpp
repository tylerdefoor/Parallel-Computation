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
void bubbleSort(vector<int>& a);

int main ( int argc, char** argv )
{
    cout << "Start of program" << endl;

    /* Variable Declarations */
    //The filename
    char* filename = argv[1];

    //The total number of elements, the maximum, and the range of the numbers
    int totalNums, numBuckets, max, taskid, count;

    //The unsorted array of ints
    int* unsorted;

    //The start, end, and total time
    double start, end, total;

    fstream fin;

    /* End of Variable Declarations */
    //Initialize MPI
    MPI_Init ( &argc, &argv );

    MPI_Status status;

    //Get the world size
    MPI_Comm_size ( MPI_COMM_WORLD, &numBuckets );

    //Get Rank
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    cout << "After intitializations" << endl;

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
        cout << "Master opened file" << endl;
        //Get the total number of ints
        fin >> totalNums;

        //Broadcast the total numbers to all processes
        MPI_Bcast ( &totalNums, 1, MPI_INT, MASTER, MPI_COMM_WORLD );

        cout << "Master bcast totalNums" << endl;

        //The unsorted array
        unsorted = new int[( totalNums / numBuckets )];

        //Read in the numbers and send to the slaves
        for ( int i = 1; i < numBuckets; i++ )
        {
            for ( int j = 0; j < (totalNums / numBuckets); j++ )
            {
                fin >> unsorted[j];

                //Check for the max
                if ( unsorted[j] > max )
                    max = unsorted[j];
            }

            MPI_Send ( unsorted, (totalNums / numBuckets), MPI_INT, i, 0, MPI_COMM_WORLD );
            cout << "Master sent to slave " << i << endl;
        }

        //Read in what the master takes care of
        for ( int i = 0; i < ( totalNums / numBuckets ); i++ )
        {
            fin >> unsorted[i];

            //Check for the max again
            if ( unsorted[i] > max )
                max = unsorted[i];
        }
        cout << "Master put into its own unsorted" << endl;

        //For super secret purposes
        max = 1000;

        //Broadcast the max to all processes
        MPI_Bcast ( &max, 1, MPI_INT, MASTER, MPI_COMM_WORLD );
        cout << "Master bcast max of " << max << endl;

        //Close the file
        fin.close (  );

        //The bucket the number is supposed to go to
        int myBucket = max / numBuckets;

        //Create a vector of ints because vectors are great
        vector<int> buckets[numBuckets];

        //Temp to hold a small bucket before transferring to big bucket
        int* temp = new int[( totalNums / numBuckets )];

        //Block because we all want to start at the same time
        MPI_Barrier ( MPI_COMM_WORLD );
        cout << "Master past barrier" << endl;

        //Start the timer
        start = MPI_Wtime (  );

        cout << "Master before putting into small buckets" << endl;

        //Put them in their buckets
        for ( int i = 0; i < (totalNums / numBuckets); i++ )
        {
            buckets[unsorted[i] / myBucket].push_back ( unsorted[i] );
        }
        
        cout << "Master put into small buckets" << endl;

        //Put our bucket in the big bucket
        vector<int> bigBucket (buckets[taskid]);
        cout << "Master put it's bucket into the big bucket" << endl;

        //Send and receive to the big buckets
        for ( int i = 0; i < numBuckets; i++ )
        {
            if ( i == taskid )
            {
                //Receive the small buckets
                for ( int j = 0; j < numBuckets - 1; j++ )
                {
                    //Receive a small bucket
                    MPI_Recv ( temp, ( totalNums / numBuckets ), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

                    //Get the number of things receieved
                    MPI_Get_count(&status, MPI_INT, &count);

                    //Put the small bucket into the big bucket
                    for (int k = 0; k < count; k++ )
                        bigBucket.push_back ( temp[k] );
                }
            }
            else
            {
                //Send the small bucket to the slave
                MPI_Send ( &(buckets[i].front (  )), buckets[i].size (  ), MPI_INT, i, 0, MPI_COMM_WORLD );
                cout << "Master sent to slave " << i << endl;
            }
        }

        //Bubble sort the bucket
        bubbleSort ( bigBucket );
        cout << "Master bubblesorted bucket" << endl;

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
        cout << "Slave " << taskid << " received max of " << max << endl;

        //The bucket the number is supposed to go to
        int myBucket = max / numBuckets;

        //Create a vector of ints because vectors are great
        vector<int> buckets[numBuckets];
        cout << "Slave " << taskid << " created buckets" << endl;

        int* temp = new int[(totalNums / numBuckets)];

        //Block because we all want to start at the same time
        MPI_Barrier ( MPI_COMM_WORLD );
        cout << "Slave " << taskid << " is past barrier" << endl;

        //Put them in their buckets
        for ( int i = 0; i < (totalNums / numBuckets); i++ ) 
            buckets[unsorted[i] / myBucket].push_back ( unsorted[i] );

        cout << "Slave " << taskid << " has put data into buckets" << endl;

        vector<int> bigBucket (buckets[taskid]);

        //Send and receive to the big buckets
        for ( int i = 0; i < numBuckets; i++ )
        {
            if ( i == taskid )
            {
                //Receive the small buckets
                for ( int j = 0; j < numBuckets - 1; j++ )
                {
                    //Receive a small bucket
                    MPI_Recv ( temp, ( totalNums / numBuckets ), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

                    //Get the number of things receieved
                    MPI_Get_count(&status, MPI_INT, &count);

                    //Put the small bucket into the big bucket
                    for (int k = 0; k < count; k++ )
                        bigBucket.push_back ( temp[k] );
                }
            }
            else
            {
                //Send the small bucket to the slave
                MPI_Send ( &(buckets[i].front (  )), buckets[i].size (  ), MPI_INT, i, 0, MPI_COMM_WORLD );
            }
        }

        //Bubble sort the bucket
        bubbleSort ( bigBucket );
    }
    //Finalize MPI
    MPI_Finalize();

    return 0;
}

 /**bubblesort
 *@fn void bubbleSort(vector<int>& a)
 *@brief Sorts a vector using bubblesort
 *@param a The vector to be sorted
 *@return N/A
 *@pre a contains relevant data
 *@post a is sorted
 */
void bubbleSort(vector<int>& a)
{
    bool swap = true;

    while(swap)
    {
        swap = false;
        for (size_t i = 0; i < a.size()-1; i++) 
        {
            if ( a[i]>a[i+1] )
            {
                a[i] += a[i+1];
                a[i+1] = a[i] - a[i+1];
                a[i] -=a[i+1];
                swap = true;
            }
        }
    }
}
