/** @file Dynamic.cpp
  * @brief A dynamically assigned parallelized Mandelbrot calculator. The program assumes that thaere are less slaves than rows.
  * @author Tyler DeFoor
  * @date 3/4/2017
  * @version 1.0
  */
 
#include "mpi.h"
#include "PIMFuncs.h"
#include "Mandelbrot.h"
#include <iostream>
#include <stdlib.h>

//#define WIDTH       20000
//#define HEIGHT      20000
#define ITERATIONS  256
#define REAL_MIN    -2.0
#define REAL_MAX    2.0
#define IMAG_MIN    -2.0
#define IMAG_MAX    2.0
#define MASTER      0
#define INT_TYPE    MPI_INT
#define CHAR_TYPE   MPI_UNSIGNED_CHAR
#define NUM_OBJECTS 1

using namespace std;

int main ( int argc, char** argv )
{
    /* Variable Declarations */
    //Variables for the world size, current task id, the number of slaves, and number of rows calculated so far
    int taskid, worldSize, numSlaves, rowsCalculated, currentRow;

    //Timing variables
    double start, end, total;

    int WIDTH = atoi(argv[1]);
    int HEIGHT = atoi(argv[2]);

    //File name
    const char* const fileName = "Dynamic.pim";

    //Status variable to return with 
    MPI_Status status;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Get the world size
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );

    //The number of slaves does not count the master
    numSlaves = worldSize - 1;

    //Get Rank
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    //If we are in the master task
    if ( taskid == MASTER )
    {
        rowsCalculated = 0;

        //Map for writing to file
        unsigned char** map;
        map = new unsigned char*[HEIGHT];
        for ( int i = 0; i < HEIGHT; i++ )
            map[i] = new unsigned char[WIDTH];

        //Array to receive the calculated row
        unsigned char* receiver = new unsigned char[WIDTH];

        start = MPI_Wtime (  );

        //Send each slave a row
        for ( int i = 0; i < numSlaves; i++ )
        {
            MPI_Send ( &rowsCalculated, NUM_OBJECTS, INT_TYPE, (i + 1), 0, MPI_COMM_WORLD );
            rowsCalculated++;
        }


        //While there are still rows to calculate
        while ( rowsCalculated < HEIGHT )
        {
            //Receive a row
            MPI_Recv ( receiver, WIDTH, CHAR_TYPE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

            //Put the row into the map
            for ( int i = 0; i < WIDTH; i++ )
            {
                map[status.MPI_TAG][i] = receiver[i];
            }

            //Send the returning slave another row
            MPI_Send ( &rowsCalculated, NUM_OBJECTS, INT_TYPE, status.MPI_SOURCE, 0, MPI_COMM_WORLD );

            //Increase rowsCalculated
            rowsCalculated++;
        }

        //Loop through all the slaves again
        for ( int i = 0; i < numSlaves; i++ )
        {
            //Receive a row
            MPI_Recv ( receiver, WIDTH, CHAR_TYPE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

            //Put the row into the map
            for ( int i = 0; i < WIDTH; i++ )
            {
                map[status.MPI_TAG][i] = receiver[i];
            }

            int terminator = -1;
            //Send the returning slave another row
            MPI_Send ( &terminator, NUM_OBJECTS, INT_TYPE, status.MPI_SOURCE, 0, MPI_COMM_WORLD );
        }

        end = MPI_Wtime (  );

        total = end - start;

        cout << "Total time for  " << WIDTH << " " << HEIGHT << ": " << total << endl;
        pim_write_black_and_white(fileName, WIDTH, HEIGHT, (const unsigned char**)map);
    }

    //If we are a slave
    else
    {
        //Row that is being calculated
        unsigned char* row = new unsigned char[WIDTH];

        //Receive the first row
        MPI_Recv ( &currentRow, 1, INT_TYPE, MASTER, 0, MPI_COMM_WORLD, &status );

        //The master will send -1 when all rows are calculated
        while ( currentRow != -1 )
        {
            //Loop through the row and calculate 
            for ( int column = 0; column < WIDTH; column++ )
            {
                //Create the current complex number with the coordinates
                Complex current;
                current.real = REAL_MIN + column * (REAL_MAX - REAL_MIN) / WIDTH;
                current.imag = IMAG_MIN + currentRow * (IMAG_MAX - IMAG_MIN) / HEIGHT;

                //Set the map coordinate to the value of the Mandelbrot calculation
                row[column] = calculate ( current );
            }

            //Send the row to master with the tag as the calculated row number
            MPI_Send ( row, WIDTH, CHAR_TYPE, MASTER, currentRow, MPI_COMM_WORLD );

            //Get the next row
            MPI_Recv ( &currentRow, 1, INT_TYPE, MASTER, 0, MPI_COMM_WORLD, &status );
        }
    }
}

 /**Calculate
 *@fn calculate ( Complex coordinate )
 *@brief Determines whether a complex number is a member of the Mandelbrot set
 *@param coordinate The complex number to be tested
 *@return count The number of times the number was tested - maximum 256
 *@pre The parameter, coordinate, has relevant information
 *@post Nothing in coordinate is changed. 
 */
unsigned char calculate ( Complex coordinate )
{
    //Keep track of the count 
    int count = 0;

    //A Complex number to hold Mandelbrot calculations
    //Intially set to 0 + 0i
    Complex current;
    current.real = 0.0;
    current.imag = 0.0;

    //A temporary float holder and the length of the complex number squared
    float temp, squarelength;

    //Do this while squarelength < 4 and we are below the number of iterations
    do
    {
        //The temp is currentreal ^ 2 - currentimag ^ 2 + coordinatereal
        temp = current.real * current.real  - current.imag * current.imag + coordinate.real;

        //Set currentimag to 2 * currentreal * currentimag + coordinageimag
        current.imag = 2 * current.real * current.imag + coordinate.imag;

        //Set currantreal to the previously calculated temp
        current.real = temp;

        //Get the square length of the imaginary number
        squarelength = current.real * current.real + current.imag * current.imag;

        //Increase count
        count++;

    } while ( ( squarelength < 4.0) && ( count < ITERATIONS ) );

    //Return the number of iterations we went through
    return (unsigned char)count;
}
