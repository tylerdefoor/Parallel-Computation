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
    int totalNums, max, range;

    //The unsorted array of ints
    int* unsorted;

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

    fin >> totalNums;
    unsorted = new int[totalNums];

}

int* bucketsort ( int* unsorted, int max, int numBuckets )
{

}