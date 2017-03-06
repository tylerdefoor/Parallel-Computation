/** @file Mandelbrot_Seq.cpp
  * @brief A sequential program for generating the Mandelbrot set
  * @author Tyler DeFoor
  * @date 3/2/2017
  * @version 1.0
  */
 
#include "PIMFuncs.h"
#include "Mandelbrot_Seq.h"
#include <cmath>
#include <iostream>

#define WIDTH       100
#define HEIGHT      100
#define ITERATIONS  256
#define REAL_MIN    -2.0
#define REAL_MAX    2.0
#define IMAG_MIN    -2.0
#define IMAG_MAX    2.0

using namespace std;

int main ( int argc, char** argv )
{
    /* Variable Declarations */
    //Timing variables
    double start, end, total;

    //File name
    const char* const fileName = "Mandelbrot_Seq.pim";

    //Map for writing to file
    unsigned char** map;
    map = new unsigned char*[HEIGHT];
    for ( int i = 0; i < HEIGHT; i++ )
        map[i] = new unsigned char[WIDTH];

    /* End of Variable Declarations */

    //Iterate rows from 0 to HEIGHT - 1
    for ( int row = 0; row < HEIGHT; row++ )
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
    pim_write_black_and_white(fileName, WIDTH, HEIGHT, (const unsigned char**)map);
    return 0;
}

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

    //count -= 1;

    //Return the number of iterations we went through
    return (unsigned char)count;
}
