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
    int taskid, worldSize;

    //Timing variables
    double start, end, total;

    //File name
    const char* const fileName = "Static.pim";

    //Map for writing to file
    unsigned char** map;
    map = new unsigned char*[HEIGHT];
    for ( int i = 0; i < HEIGHT; i++ )
        map[i] = new unsigned char[WIDTH];

    /* End of Variable Declarations */
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