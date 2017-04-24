// Header Files
#include <iostream>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <time.h>

using namespace std;

// Global Constants
bool PRINT = false;
bool UNIQUE_MATRIX = true;
int TESTS = 1;

// Function Prototypes
void getData( int argc, char* argv[], int &rows, int &columns );
int getNumber( char string[], int size );
void multiplyMatrix( int** A, int** B, int** C, int rows, int columns );
void displayMatrix( int **matrix, int rows, int columns, char letter );
void fillWithZeros( int ** matrix, int rows, int columns );
void generateMatrix( int** &A, int** &B, int rows, int columns );
void allocateMatrix( int** &matrix, int rows, int columns );

// Main Program
int main( int argc, char* argv[] )
   {
    // Initialize function/variables
    int rows, columns, index;
    int ** matrixA;
    int ** matrixB;
    int ** matrixC;
    double start, end;

    
    //  Initialize MPI
    MPI_Init( &argc, &argv );
    
    // get command line data
    getData( argc, argv, rows, columns );
    
    // allocate 3 matrices
    allocateMatrix( matrixA, rows, columns );
    allocateMatrix( matrixB, rows, columns );
    allocateMatrix( matrixC, rows, columns );
    
    // fill matrix C with zeros
    fillWithZeros( matrixC, rows, columns );
    
    // Generate elements of matrix A and B and C
    generateMatrix( matrixA, matrixB, rows, columns );
    
    // display rows and columns
    cout << "Rows: " << rows << " Columns: " << columns << endl;
    
    // loop testing
    for( index = 0; index < TESTS; index++ )
       {
        // start timer
        start = MPI_Wtime();
    
        // multiply matrices
        multiplyMatrix( matrixA, matrixB, matrixC, rows, columns );
    
        // end timer
        end = MPI_Wtime();
        
        // dispaly the time
        cout << end - start << endl;
        
        // check that not last run
        if( index != TESTS - 1 )
           {
            // Fill C with 0's
            fillWithZeros( matrixC, rows, columns );
           }
       }
    
    // display the 3 matrices
    if( PRINT )
       {
        displayMatrix( matrixA, rows, columns, 'A' );
        displayMatrix( matrixB, rows, columns, 'B' );
        displayMatrix( matrixC, rows, columns, 'C' );
       }
    
    
    // finalize mpi
    MPI_Finalize();
   }
   
// Supporting Function Implementation
/**
  * @name getData
  *
  * @brief Gets data from the command line
  *
  * @details Gets the number of processors, and width/height of the image
  *
  * @pre Command Line Arguements must be structured in the way defined in ReadMe.md
  *
  * @post Data is returned by referene
  *
  * @par Algorithm
  *      Loops through the strings till it finds proper strings to take in
  *
  * @exception None
  *
  * @param[in] strings
  *            Contains the command line arguements
  *
  * @param[in] size
  *            Amount of strings
  *
  * @param[out] numProcessors
  *             returns the number of slaves 
  *
  * @param[out] width
  *            returns the width of the image
  *
  * @param[out] height
  *            returns the height of the image
  *
  * @return None
  *
  * @note None
  */  
void getData( int size, char* strings[], int &rows, int &columns )
   {
    // Initialize function/variables
    char temp[100];
    int index, length;
    
    // loop across till you find valid data to collect
    for( index = 0; index < size; index++ )
       {
        // get a string
        strcpy( temp, strings[index] );
        
        // get the length
        length = strlen( temp );
        
        // check if data
        if( temp[0] == 'm' )
           {
            // store the width
            columns = rows = getNumber( temp, length );
           }
       }
   }
 
/**
  * @name getNumber
  *
  * @brief Gets a number from a string
  *
  * @details Gets the number from the string that is passed in
  *
  * @pre Command Line Arguements must be structured in the way defined in ReadMe.md
  *
  * @post Number is returned
  *
  * @par Algorithm
  *      Loops through the strings and stores the number in a temp. Atoi converts
  *
  * @exception None
  *
  * @param[in] string
  *            Contains the command line arguement
  *
  * @param[in] size
  *            Size of string
  *
  * @return The number in the string
  *
  * @note None
  */ 
int getNumber( char string[], int size )
   {
    // Initialize variabels
    int index, num;
    char temp[100];
    
    // copy the string
    strcpy( temp, string );
    
    // loop removing the letter
    for( index = 0; index < size; index++ )
       {
        temp[index] = temp[index+1];
       }
       
    // add a null char
    temp[size-1] = '\0';
    
    // convert to a number
    num = atoi( temp );
    
    return num;
   }
   
/**
  * @name generateMatrix
  *
  * @brief Generates two matrices
  *
  * @details Generates two matries, unique or not unique depending on gloabl var
  *
  * @pre Columns and rows must be valid and matrices must be allocated
  *
  * @post Matrices get data
  *
  * @note None
  */ 
void generateMatrix( int** &A, int** &B, int rows, int columns )
   {
    // Initialize function/variables
    int index, subIndex, num1, num2;
    
    // check if unique matrix
    if( UNIQUE_MATRIX )
       {
        srand( time( NULL ));
       }
       
    // loop filling the matrix with random data
    for( index = 0; index < rows; index++ )
       {
        for( subIndex = 0; subIndex < columns; subIndex++ )
           {   
            // if unique matrix get two random numbers
            if( UNIQUE_MATRIX )
               {
                num1 = rand() % 10;
                num2 = rand() % 10;
               }
               
            // else get the same matrix
            else
               {
                num1 = num2 = random() % 10;
               }
            
            // store the nuber
            A[index][subIndex] = num1;
            B[index][subIndex] = num2;
           }
       }
   }
 
/**
  * @name multiplyMatrix
  *
  * @brief multiplies two matrices and stores results
  *
  * @details triple for loop to multiply matrices
  *
  * @pre  matrices must be initialized
  *
  * @post results stored
  *
  * @note None
  */   
void multiplyMatrix( int** A, int** B, int** C, int rows, int columns )
   {
    // Initialize functions/variables
    int i,j,k;
    
    // loop multiplying matrices
    for( i = 0; i < rows; i++ )
       {
        for( j = 0; j < columns; j++ )
           {
            for( k = 0; k < columns; k++ )
               {
                C[i][j] += A[i][k] * B[k][j];
               }
           }
       }
   }
  
/**
  * @name displayMatrix
  *
  * @brief prints matrix
  *
  * @details prints matrix to screen
  *
  * @pre matrix must be initialized
  *
  * @post matrix printed
  *
  * @note None
  */  
void displayMatrix( int **matrix, int rows, int columns, char letter )
   {
    // Initialize function/variables
    int index, subIndex;
    
    // display matrix letter
    cout << "Matrix " << letter << endl;
    
    // loop displaying the matrix
    for( index = 0; index < rows; index++ )
       {
        for( subIndex = 0; subIndex < columns; subIndex++ )
           {
            cout << matrix[index][subIndex] << " ";
            
           }
        cout << endl;
       }
    cout << endl;
   } 
   
/**
  * @name fillWithZeros
  *
  * @brief fiills a matrix with all 0s
  *
  * @details matrix is filled with 0s
  *
  * @pre matrix must be initialized
  *
  * @post matrix filled with data
  *
  * @note None
  */ 
void fillWithZeros( int ** matrix, int rows, int columns )
   {
    // Initialize function/variables
    int index, subIndex;
   
    // loop filling the matrix with random data
    for( index = 0; index < rows; index++ )
       {
        for( subIndex = 0; subIndex < columns; subIndex++ )
           {
            // store the nuber
            matrix[index][subIndex] = 0;
           }
       }
   }

/**
  * @name allocateMatrix
  *
  * @brief matrix is allocated
  *
  * @details matrix gets dynamically allocated
  *
  * @pre rows and columns must be valid
  *
  * @post matrix initialized
  *
  * @note None
  */ 
void allocateMatrix( int** &matrix, int rows, int columns )
   {
    // Initialize function/variables
    int index;
    
    // alocate the rows
    matrix = new int*[rows];
    
    // allocate the columns
    for( index = 0; index < rows; index++ )
       {
        matrix[index] = new int[columns];
       }
   }  
   
