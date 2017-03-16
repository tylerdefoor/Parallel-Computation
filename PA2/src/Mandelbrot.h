#ifndef MANDELBROT_H
#define MANDELBROT_H

//Complex numbers struct
struct Complex
{
    //Floats for real and imaginary portions
    float real;
    float imag;
};

unsigned char calculate ( Complex current );

#endif
