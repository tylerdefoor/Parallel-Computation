/** @file Mandelbrot_Seq.cpp
  * @brief A sequential program for generating the Mandelbrot set
  * @author Tyler DeFoor
  * @date 3/2/2017
  * @version 1.0
  */
 
#include "PIMFuncs.h"
#include <cmath>

#define WIDTH       1920
#define HEIGHT      1080
#define ITERATIONS  256

int main ( int argc, char* argv )
{
    /* Variable Declarations */
    double start, end, total;
    const char* const fileName = "Mandelbrot_Seq.out";
    unsigned char map[HEIGHT][WIDTH];
    /* End of Variable Declarations */

    //Iterate rows from 0 to HEIGHT - 1
    for ( int row = 0; i < HEIGHT; row++ )
    {
        //Iterate columns from 0 to WIDTH - 1
        for ( int column = 0; i < WIDTH; column++ )
        {
            //Create the current complex number with the coordinates
            Complex current;
            current.real = column;
            current.imaginary = row;

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
    current.imaginary = 0.0;

    //A temporary float holder and the length of the complex number squared
    float temp, squarelength;

    //Do this while squarelength < 4 and we are below the number of iterations
    do
    {
        //The temp is currentreal ^ 2 - currentimag ^ 2 + coordinatereal
        temp = pow ( current.real, 2.0 ) - pow ( current.imaginary, 2.0 ) + coordinate.real;

        //Set currentimag to 2 * currentreal * currentimag + coordinageimag
        current.imaginary = 2 * current.real * current.imaginary + coordinate.imaginary;

        //Set currantreal to the previously calculated temp
        current.real = temp;

        //Get the square length of the imaginary number
        squarelength = pow ( current.real, 2.0 ) + ( current.imaginary, 2.0 );

        //Increase count
        count++;
    } while ( ( squarelength < 4.0) && ( count < ITERATIONS ) );

    //Return the number of iterations we went through
    return (unsigned char)count;
}