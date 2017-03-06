#ifndef MANDELBROT_SEQ_H
#define MANDELBROT_SEQ_H

//Complex numbers struct
struct Complex
{
    //Floats for real and imaginary portions
    float real = 0.0;
    float imaginary = 0.0;
};

unsigned char calculate ( Complex current );