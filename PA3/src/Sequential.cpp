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
#include <vector>
#include <algorithm>
#include "mpi.h"

#define MASTER      0
#define INT_TYPE    MPI_INT

using namespace std;

void bucketsort ( int* unsorted, int* &sorted, int max, int numBuckets, int totalNums );
void bubbleSort(vector<int>& a);

int main ( int argc, char** argv )
{

    /* Variable Declarations */
    //The filename
    char* filename = argv[1];

    //The total number of elements, the maximum, and the range of the numbers
    int totalNums, numBuckets, max;

    //The unsorted array of ints
    int* unsorted;
    int* sorted;

    //The start, end, and total time
    double start, end, total;

    numBuckets = 10;
    //Can put in another argument to set number of buckets
    /*
    if ( argc == 3 )
        numBuckets = atoi(argv[2]);
    */
    //File I/O - More like FUN I/O
    ifstream fin;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

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

    max = 0;
    //Read in the numbers
    for ( int i = 0; i < totalNums; i++ )
    {
        fin >> unsorted[i];

        //Get the max number
        if (unsorted[i] > max)
            max = unsorted[i];
    }

    max += 10;

    //Close the file
    fin.close (  );

    //Start the timer
    start = MPI_Wtime (  );

    //Bucket sort everything
    bucketsort ( unsorted, sorted, max, numBuckets, totalNums );

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
        bubbleSort ( buckets[i] );

        //memcpy it back into the original because its super speedy
        //Uncomment to turn it on/off
        //memcpy ( &sorted[current], buckets[i].data (  ), sizeof(int) *buckets[i].size (  ) );

        //Update current index
        current += buckets[i].size (  );
    }

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
