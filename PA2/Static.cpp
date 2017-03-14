/** @file Static.cpp
  * @brief A statically assigned parallelized Mandelbrot calculator
  * @author Tyler DeFoor
  * @date 3/4/2017
  * @version 1.0
  */
 
#include "PIMFuncs.h"
#include "Mandelbrot.h"
#include <iostream>

#define WIDTH       1920
#define HEIGHT      1080
#define ITERATIONS  256
#define REAL_MIN    -2.0
#define REAL_MAX    2.0
#define IMAG_MIN    -2.0
#define IMAG_MAX    2.0
#define MASTER      0
#define TYPE        MPI_INT

using namespace std;

int main ( int argc, char** argv )
{
    /* Variable Declarations */
    //Variables for the world size and current task id
    int taskid, worldSize, numSlaves, rowsPerSlave, extras, start;

    //Timing variables
    double start, end, total;

    //File name
    const char* const fileName = "Static.pim";

    //Status variable to return with 
    MPI_Status status;

    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Get the world size
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );

    //The number of slaves does not count the master
    numSlaves = worldSize - 1;

    //Set a static number of rows per slave
    //If the number isn't divisible by the number of slaves, there will be extras
    //The first processes will deal with the extras, since the height will always equal rowsPerSlave + extras
    rowsPerSlave = HEIGHT / numSlaves;

    extras = HEIGHT % numSlaves;

    //Get Rank
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    //If we are in the master task
    if ( taskid == MASTER )
    {
        //Map for writing to file
        unsigned char** map;
        map = new unsigned char*[HEIGHT];
        for ( int i = 0; i < HEIGHT; i++ )
            map[i] = new unsigned char[WIDTH];

        unsigned char** receiver;
        receiver = new unsigned char*[rowsPerSlave];
        for ( int i = 0; i < rowsPerSlave; i++ )
            receiver[i] = new unsigned char[WIDTH];


        for ( int i = 0; i < numSlaves; i++ )
        {
            //Tell the slave where to start
            start = i * rowsPerSlave;

            //Send the slave where to start
            MPI_Send ( &start, 1, TYPE, i, 0, MPI_COMM_WORLD );
        }

        //If there are extras for the master to process
        if ( extras != 0 )
        {
            //Start at the end of the slave rows
            start = HEIGHT - extras - 1;

            //Step through the extras and put them into the map
            for ( start; start < HEIGHT; start ++ )
            {
                //Iterate columns from 0 to WIDTH - 1
                for ( int column = 0; column < WIDTH; column++ )
                {
                    //Create the current complex number with the coordinates
                    Complex current;
                    current.real = REAL_MIN + column * (REAL_MAX - REAL_MIN) / WIDTH;
                    current.imag = IMAG_MIN + row * (IMAG_MAX - IMAG_MIN) / HEIGHT;

                    //Set the map coordinate to the value of the Mandelbrot calculation
                    map[row][column] = calculate ( current );
                }
            }
        }

        for ( int i = 0; i < numSlaves; i++ )
        {
            for ( int j = 0; j < rowsPerSlave; j++ )
            {
                MPI_recv(&receiver, 32, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }
        }


    }
    else
    {
        //Map for writing to file
        unsigned char** map;
        map = new unsigned char*[HEIGHT];
        for ( int i = 0; i < HEIGHT; i++ )
            map[i] = new unsigned char[WIDTH];

        MPI_Recv ( &start, 1, TYPE, MASTER, 0, MPI_COMM_WORLD, &status );


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