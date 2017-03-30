/** @file Sequential.cpp
  * @brief A sequential implementation of Bucket Sort. 
  * @author Tyler DeFoor
  * @date 3/28/2017
  * @version 1.0
  */
 
#include "mpi.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>

#define MASTER      0
#define INT_TYPE    MPI_INT

using namespace std;

int main ( int argc, char** argv )
{
    //Must have correct number of arguments because good programming
    if ( argc < 1 )
    {
        cout << "Must enter filename" << endl;
        return 1;
    }

    /* Variable Declarations */
    //The filename
    char* filename = argv[1];

    //The total number of elements, the maximum, and the range of the numbers
    int totalNums, range;

    //The unsorted array of ints
    int* unsorted, sorted;

    //The start, end, and total time
    double start, end, total;

    //Can put in another argument to set number of buckets
    if ( argc == 2 )
        int numBuckets = atoi(argv[2]);
    //Can't have 3 arguments. 3 is too many. 2 is just right. 4 is right out. 1 is acceptable.
    else if ( argc >= 3 )
    {
        cout << "Invalid number of arguments" << endl;
        return 1;
    }
    //Default number of buckets is 10
    else
        int numBuckets = 10;

    //File I/O - More like FUN I/O
    ifstream fin;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_INIT ( NULL, NULL );

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
    unsorted = new int[totalNums];
    sorted = new int[totalNums];

    //Read in the numbers
    for ( int i = 0; i < totalNums; i++ )
        fin >> unsorted[i];

    //Close the file
    fin.close (  );

    //Start the timer
    start = MPI_Wtime (  );

    //Bucket sort everything
    bucketsort ( unsorted, &sorted, max, numBuckets, totalNums );

    //End the timer
    end = MPI_Wtime (  );

    //Calculate the total time
    total = end - start;

    //Output the time for totalNums
    cout << totalNums << " " << total << endl;

    //Finalize MPI
    MPI_Finalize();

    return 0;
}

 /**bucketsort
 *@fn bucketsort ( int* unsorted, int* &sorted, int max, int numBuckets, int totalNums )
 *@brief Sorts unsorted using bucketsort into sorted
 *@param unsorted The unsorted list of numbers
 *@param sorted The sorted list of numbers
 *@param max The maximum number
 *@param numBuckets The number of buckets
 *@param totalNums The number of elements in the array
 *@return N/A
 *@pre unsorted and sorted are allocated and unsorted holds relevant data
 *@post sorted contains all of the numbers of unsorted, but sorted
 */
void bucketsort ( int* unsorted, int* &sorted, int max, int numBuckets, int totalNums )
{
    //The bucket the number is supposed to go to
    int myBucket = max / numBuckets;

    //Create a vector of ints because vectors are great
    vector<int> buckets[numBuckets];

    //Put them in their buckets
    for ( int i = 0; i < totalNums; i++ ) 
        buckets[unsorted[i] / myBucket].push_back ( unsorted[i] );

    //The current index
    int current = 0;

    //Loop through all of the buckets
    for ( int i = 0; i < numBuckets; i++ ) 
    {
        //Sort the bucket
        sort ( buckets[i].begin (  ), buckets[i].end (  ) );

        //memcpy it back into the original because its super speedy
        memcpy ( &sorted[current], buckets[i].data (  ), sizeof(int) *buckets[i].size (  ) );

        //Update current index
        current += buckets[i].size (  );
    }
}