// Header Files
#include <iostream>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>

using namespace std;

// Global Constants
bool PRINT = true;
bool UNIQUE_MATRIX = true;
int TESTS = 1;

// Function Prototypes
void getData( int argc, char* argv[], int &rows, int &columns );
int getNumber( char string[], int size );
void multiplyMatrix( int** A, int** B, int** C, int rows, int columns );
void displayMatrix( int **matrix, int rows, int columns, char letter );
void fillWithZeros( int ** matrix, int rows, int columns );
void generateMatrix( int** A, int** B, int rows, int columns );
void allocateMatrix( int** &matrix, int rows, int columns );
void splitMatrix( int ** matrix, int rows, int columns, int processors, int ** subMatrix );
void recvMatrix( int ** matrix, int rows, int columns );
void rowShiftInit( int** matrix, int rows, int columns, int processors, int rank, int dest, int src  );
void colShiftInit( int** matrix, int rows, int columns, int processors, int rank, int dest, int src  );
void rowShift( int** matrix, int rows, int columns, int processors, int rank, int dest, int src  );
void colShift( int** matrix, int rows, int columns, int processors, int rank, int dest, int src  );
void collectResults( int ** matrixC, int ** subC, int rows, int columns, int processors );
void findDestAndSrc( int rank, int processors, int &destRow, int &destCol, int &srcRow, int &srcCol );
void copyMatrix( int ** src, int ** dest, int rows, int cols );

// Main Program
int main( int argc, char* argv[] )
   {
    // Initialize function/variables
    int rows, columns, rank, processors, root, index, subIndex;
    int destRow, destCol, srcRow, srcCol;
    int ** matrixA;
    int ** matrixB;
    int ** matrixC;
    int ** subA;
    int ** subB;
    int ** subC;
    int ** myA;
    int ** myB;
    double start = 0.0, end = 0.0;
    
    //  Initialize MPI
    MPI_Init( &argc, &argv );
    
    // get ranl 
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    // get total processors
    MPI_Comm_size( MPI_COMM_WORLD, &processors );
    
    // get command line data (everyone does this)
    getData( argc, argv, rows, columns );
    
    // find the square root of the number of processors
    root = sqrt( processors );
    
    /// allcoate sub matrices
    allocateMatrix( subA, rows/root, columns/root );
    allocateMatrix( subB, rows/root, columns/root );
    allocateMatrix( subC, rows/root, columns/root);
    
    // allocate myA and myB for multiple test purposes
    allocateMatrix( myA, rows/root, columns/root );
    allocateMatrix( myB, rows/root, columns/root );
    
    // fill matrix C with zeros
    fillWithZeros( subC, rows/root, columns/root );
        
    // find your destinations and src rows/columns
    findDestAndSrc( rank, processors, destRow, destCol, srcRow, srcCol );
    
    // check if master
    if( rank == 0 )
       {
        // display rows and columns
        cout << "Rows: " << rows << " Columns: " << columns << endl;
        
        // allocate matrixA,B, and C
        allocateMatrix( matrixA, rows, columns );
        allocateMatrix( matrixB, rows, columns );
        allocateMatrix( matrixC, rows, columns );
        
        // fil c with 0s
        fillWithZeros( matrixC, rows, columns );
        
        // Generate elements of matrix A and B and C
        generateMatrix( matrixA, matrixB, rows, columns );

        // send out appropiate portions of matrix a and b to everyone
        splitMatrix( matrixA, rows, columns, processors, subA );
        splitMatrix( matrixB, rows, columns, processors, subB );
       }
       
    // otherwise check if slaves
    else
       {
        // receive matrix a and b portions
        recvMatrix( subA, rows/root, columns/root );
        recvMatrix( subB, rows/root, columns/root );
        
        // copy A and B (for multiple tests)
        copyMatrix( subA, myA, rows/root, columns/root );
        copyMatrix( subB, myB, rows/root, columns/root );
       }
       
    // Wait for everyone to get here
    MPI_Barrier( MPI_COMM_WORLD ); 

    // loop repeating test
    for( index = 0; index < TESTS; index++ )
       {
        // start timer if master
        if( rank == 0 )
           {
            start = MPI_Wtime();
           }
           
        // Initialize row shift
        rowShiftInit( subA, rows/root, columns/root, processors, rank, destRow, srcRow );

        // Initialize column shift
        colShiftInit( subB, rows/root, columns/root, processors, rank, destCol, srcCol );

        // loop doing matrix mult and shifting
        for( subIndex = 0; subIndex < root; subIndex++ )
           {
            // multiply matrices
            multiplyMatrix( subA, subB, subC, rows/root, columns/root );       

            //if( subIndex != root - 1 )
               //{ 
            // Shift rows
            rowShift( subA, rows/root, columns/root, processors, rank, destRow, srcRow );
            
            // shift columns
            colShift( subB, rows/root, columns/root, processors, rank, destCol, srcCol );
               //}
           }
           
        // end timer
        MPI_Barrier( MPI_COMM_WORLD );

        // end timer if master
        if( rank == 0 )
           {
            end = MPI_Wtime();
        
            // display time
            cout << end - start << endl;
          }
        
        // Check if not last run
        if( index != TESTS - 1 )
           {
            // fill C with zeros
            fillWithZeros( subC, rows/root, columns/root );
            
            // copy original data back to sub a and b
            copyMatrix( myA, subA, rows/root, columns/root );
            copyMatrix( myB, subB, rows/root, columns/root );
           }   
       }

    // collect the results to the master
    if( rank == 0 )
       {
        collectResults( matrixC, subC, rows, columns, processors );  
       }
       
    // send results to master
    else
       {
        for( index = 0; index < rows/root; index++ )
           {
            MPI_Send( subC[index], rows/root, MPI_INT, 0, 5, MPI_COMM_WORLD );
           }
       }
       
    // display the 3 matrices if master and PRINT is true
    if( rank == 0 && PRINT )
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
        
        // check if rows
        if( temp[0] == 'm' )
           {
            // store the width
            rows = columns = getNumber( temp, length );
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
  * @brief generate matrices A and B
  *
  * @details Generate the same two matrices (everytime the program is ran) or two
  *          random matrices based on the global bool UNIQUE_MATRIX
  *
  * @pre Matrices must be allocated
  *
  * @post Matrix data is generated
  *
  * @par Algorithm
  *      Uses rand with a time seed for random data, and random() for same matrix
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */ 
void generateMatrix( int** A, int** B, int rows, int columns )
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
  * @details Multiplies matrix A and B and stores the result in matrix C
  *
  * @pre Matrices must be allocated
  *
  * @post Matrices multiplied
  *
  * @par Algorithm
  *      Triple for loop for multiplication
  *
  * @exception None
  *
  * @return none
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
  * @details prints a matrix to the screen
  *
  * @pre Matrices must be allocated
  *
  * @post Matrix multiplied
  *
  * @par Algorithm
  *      loops printing
  *
  * @exception None
  *
  * @return none
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
  * @details Fills a matrix with zeros 
  *
  * @pre Matrices must be allocated
  *
  * @post Matricex filled with zeros
  *
  * @par Algorithm
  *      loops filling
  *
  * @exception None
  *
  * @return none
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
  * @details Allocates memory for a matrix
  *
  * @pre Matrices must not be allocated
  *
  * @post Matricex allocated
  *
  * @par Algorithm
  *      dyanmically allocates memory
  *
  * @exception None
  *
  * @return none
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
   
/**
  * @name splitMatrix
  *
  * @details Splits a matrix and sends out specific portion to slave matrices
  *
  * @pre Number of processors must be a perfect squares, and matrix size must
  *      be divisible by the square root of the processors
  *
  * @post Matrix is spread to slaves
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */ 
void splitMatrix( int ** matrix, int rows, int columns, int processors, int ** subMatrix )
   {
    // Initialize processors/variables
    int ** buffer;
    int root, subRows, subCols, index, subIndex;
    int rowIndex, colIndex, processorIndex, j, k;
    
    // get the root
    root = sqrt( processors );
    subRows = rows/ root;
    subCols = columns/root;
    
    // initialize buffer
    buffer = new int*[subRows];
    for( index = 0; index < subRows; index++ )
       {
        buffer[index] = new int [subCols];
       }
       
    // get master data
    for( index = 0; index < subRows; index++ )
       {
        for( subIndex = 0; subIndex < subCols; subIndex++ )
           {
            subMatrix[index][subIndex] = matrix[index][subIndex];
           }
       }
       
    // loop sending the rows out all processes
    for( processorIndex = 1; processorIndex < processors; processorIndex++ )
       {
        // find index for rows
        for( j = 0, rowIndex = 0; j < processorIndex / root; j++ )
           {
            rowIndex += subRows;
           }
           
        // find the index for columns
        for( k = 0, colIndex = 0; k < processorIndex % root; k++ )
           {
            colIndex += subCols;
           }
         
        // store data in process
        for( j = 0, index = rowIndex; index < rowIndex + subRows; index++, j++ )
           {
            for( k = 0, subIndex = colIndex; subIndex < colIndex + subCols; subIndex++, k++ )
               {
                buffer[j][k] = matrix[index][subIndex];
               }
           }
           
        // send the data 
        for( index = 0; index < subRows; index++ )
           {
            MPI_Send( buffer[index], subCols, MPI_INT, processorIndex, 5, MPI_COMM_WORLD );
           }
       }
   }
 
/**
  * @name recvMatrix
  *
  * @details Slaves use to receivie portion of a matrix from master
  *
  * @pre Master must be breaking up matrix
  *
  * @post Matrix is spread to slaves
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */   
void recvMatrix( int ** matrix, int rows, int columns )
   {
    // Initialize function/variables
    int index;
    MPI_Status status;
    
    // loop receiving data
    for( index = 0; index < rows; index++ )
       {
        MPI_Recv( &(matrix[index][0]), columns, MPI_INT, 0, 5, MPI_COMM_WORLD, &status );
       }
   }

/**
  * @name rowShiftInit
  *
  * @details Shifts row left based on row number
  *
  * @pre Slaves must have correct data and dest/src calculated correctly
  *
  * @post Matrix is shifted left
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */    
void rowShiftInit( int** matrix, int rows, int columns, int processors, int rank, int dest, int src )
   {
    // Initialize function/variables
    int myRow, index, subIndex, i, j;
    int root;
    int** temp;
    MPI_Request req;
    MPI_Status status, s2;
    
    // Initialize temp
    temp = new int*[rows];
    for( index = 0; index < rows; index++ )
       {
        temp[index] = new int[columns];
       }
        
    // find the root   
    root = sqrt( processors );
    
    // find your row numbers
    myRow = rank / root; 
   
    // loop sending data
    for( index = 0; index < myRow; index++ )
       {   
        // send your matrix to appropiate place
        for( subIndex = 0; subIndex < rows; subIndex++ )
           {
            MPI_Isend( matrix[subIndex], columns, MPI_INT, dest, myRow, MPI_COMM_WORLD, &req ); 
           }
           
        // recv a matrix
        for( subIndex = 0; subIndex < rows; subIndex++ )
           {
            MPI_Recv( temp[subIndex], columns, MPI_INT, src, myRow, MPI_COMM_WORLD, &status );
           }
           
        MPI_Wait( &req, &s2 );
        
        // copy the data to matrix
        for( i = 0; i < rows; i++ )
           {
            for( j = 0; j < columns; j++ )
               {
                matrix[i][j] = temp[i][j];
               }
           }
       }
    
   }
   
/**
  * @name colShiftInit
  *
  * @details Shifts col upwared based on col number
  *
  * @pre Slaves must have correct data and dest/src calculated correctly
  *
  * @post Matrix is shifted up
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */       
void colShiftInit( int** matrix, int rows, int columns, int processors, int rank, int dest, int src )
   {
    // Initialize function/variables
    int myCol,index, subIndex, i, j;
    int root;
    int** temp;
    MPI_Request req;
    MPI_Status status, s2;
    
    // Initialize temp
    temp = new int*[rows];
    for( index = 0; index < rows; index++ )
       {
        temp[index] = new int[columns];
       }
       
    // find the root   
    root = sqrt( processors );
    
    // find your column number
    myCol = rank % root; 
        
    // loop sending data
    for( index = 0; index < myCol; index++ )
       {  
        // send your matrix to appropiate place
        for( subIndex = 0; subIndex < rows; subIndex++ )
           {
            MPI_Isend( matrix[subIndex], columns, MPI_INT, dest, myCol, MPI_COMM_WORLD, &req ); 
           }
           
        // recv a matrix
        for( subIndex = 0; subIndex < rows; subIndex++ )
           {
            MPI_Recv( temp[subIndex], columns, MPI_INT, src, myCol, MPI_COMM_WORLD, &status );
           }
           
        // wait till other process has received your data
        MPI_Wait( &req, &s2 );
           
        // copy the data to matrix
        for( i = 0; i < rows; i++ )
           {
            for( j = 0; j < columns; j++ )
               {
                matrix[i][j] = temp[i][j];
               }
           }
       }
   }
   
/**
  * @name rowShift
  *
  * @details Shifts row left once
  *
  * @pre Slaves must have correct data and dest/src calculated correctly
  *
  * @post Matrix is shifted left
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */       
void rowShift( int** matrix, int rows, int columns, int processors, int rank, int dest, int src )
   {
    // Initialize function/variables
    int myRow, index, subIndex, i, j;
    int root;
    int** temp;
    MPI_Request req;
    MPI_Status status, s2;
    
    // Initialize temp
    temp = new int*[rows];
    for( index = 0; index < rows; index++ )
       {
        temp[index] = new int[columns];
       }

    // find the root   
    root = sqrt( processors );
    
    // find your row numbers
    myRow = rank / root; 
           
    // send your matrix to appropiate place
    for( subIndex = 0; subIndex < rows; subIndex++ )
       {
        MPI_Isend( matrix[subIndex], columns, MPI_INT, dest, myRow, MPI_COMM_WORLD, &req ); 
       }
           
    // recv a matrix
    for( subIndex = 0; subIndex < rows; subIndex++ )
       {
        MPI_Recv( temp[subIndex], columns, MPI_INT, src, myRow, MPI_COMM_WORLD, &status );
       }
           
    MPI_Wait( &req, &s2 );
     
    // copy the data to matrix
    for( i = 0; i < rows; i++ )
       {
        for( j = 0; j < columns; j++ )
           {
            matrix[i][j] = temp[i][j];
           }
       }
   }   

/**
  * @name colShift
  *
  * @details Shifts col upwared once
  *
  * @pre Slaves must have correct data and dest/src calculated correctly
  *
  * @post Matrix is shifted up
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */      
void colShift( int** matrix, int rows, int columns, int processors, int rank, int dest, int src )
   {
    // Initialize function/variables
    int myCol, index, subIndex, i, j;
    int root;
    int** temp;
    MPI_Request req;
    MPI_Status status,s2;
    
    // Initialize temp
    temp = new int*[rows];
    for( index = 0; index < rows; index++ )
       {
        temp[index] = new int[columns];
       }
       
    // find the root   
    root = sqrt( processors );
    
    // find your column number
    myCol = rank % root; 
          
    // send your matrix to appropiate place
    for( subIndex = 0; subIndex < rows; subIndex++ )
       {
        MPI_Isend( matrix[subIndex], columns, MPI_INT, dest, myCol, MPI_COMM_WORLD, &req ); 
       }
          
    // recv a matrix
    for( subIndex = 0; subIndex < rows; subIndex++ )
       {
        MPI_Recv( temp[subIndex], columns, MPI_INT, src, myCol, MPI_COMM_WORLD, &status );
       }
       
    MPI_Wait( &req, &s2 );   
    
    // copy the data to matrix
    for( i = 0; i < rows; i++ )
       {
        for( j = 0; j < columns; j++ )
           {
            matrix[i][j] = temp[i][j];
           }
       }
   }   
 
/**
  * @name collectResults
  *
  * @details Collects Matrices from slaves and places data in large matrix
  *
  * @pre Slaves must all send their matric
  *
  * @post Matrix collected
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */    
void collectResults( int ** matrixC, int ** subC, int rows, int columns, int processors )
   {
    // Initialize function/variables
    int** temp;
    int root, subRows, subCols, index, i, j, rowIndex, colIndex, p, q, subIndex;
    MPI_Status status, s2;
    
    // get the root
    root = sqrt( processors );
    subRows = rows/root;
    subCols = columns/root;
    
    // initailize temp
    temp = new int*[subRows];
    for( index = 0; index < subRows; index++ )
       {
        temp[index] = new int[subCols];
       }
       
    // store master data into matrix answer
    for( i = 0; i < subRows; i++ )
       {
        for( j = 0; j < subCols; j++ )
           {
            matrixC[i][j] = subC[i][j];
           }
       }
       
    // Loop collecting matrices
    for( index = 1; index < processors; index++ )
       {
        // get the first row of a matrix
        MPI_Recv( temp[0], subCols, MPI_INT, MPI_ANY_SOURCE, 5, MPI_COMM_WORLD, &status );
        
        // loop receving rest of rows from same process
        for( subIndex = 1; subIndex < subRows; subIndex++ )
           {
            MPI_Recv( temp[subIndex], subCols, MPI_INT, status.MPI_SOURCE, 5, MPI_COMM_WORLD, &s2 );
           }
           
        // find index for rows
        for( i = 0, rowIndex = 0; i < status.MPI_SOURCE / root; i++ )
           {
            rowIndex += subRows;
           }
           
        // find the index for columns
        for( j = 0, colIndex = 0; j < status.MPI_SOURCE % root; j++ )
           {
            colIndex += subCols;
           }
         
        // store data in process
        for( i = 0, p = rowIndex; p < rowIndex + subRows; p++, i++ )
           {
            for( j = 0, q = colIndex; q < colIndex + subCols; q++, j++ )
               {
                matrixC[p][q] = temp[i][j];
               }
           } 
       }
   } 

/**
  * @name findDestAndSrc
  *
  * @details Finds the destinations and sources for shifting
  *
  * @pre Number of processors must be a perfect square
  *
  * @post Destinations and sources found
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */   
void findDestAndSrc( int rank, int processors, int &destRow, int &destCol, int &srcRow, int &srcCol )
   {
    // Initialzie function/variables
    int root, myRow, myCol;
    
    // fidn the root
    root = sqrt( processors );
    myRow = rank / root;
    myCol = rank % root;
  
    // find the dest row and src row
    destRow = rank - 1;
    srcRow = rank + 1;
    
    // ensure that dest and src for row are in bounds
    if( destRow < 0 || (destRow/root) != myRow )
       {
        destRow += root;
       }
       
    if( srcRow/root != myRow )
       {
        srcRow -= root;
       }
    
    // find the dest col and sr col
    destCol = rank - root;
    srcCol = rank + root;
        
      
    // ensure that the dest and src are in bounds  
    if( destCol < 0 )
       {
        destCol += processors;
       }
       
    if( srcCol >= processors )
       {
        srcCol -= processors;
       }
   }
  
/**
  * @name copyMatrix
  *
  * @details Copies the data from the source to dest
  *
  * @pre Matrices must be the same size
  *
  * @post Data is copied
  *
  * @par Algorithm
  *      Alogrithm is commented
  *
  * @exception None
  *
  * @return none
  *
  * @note None
  */  
void copyMatrix( int ** src, int ** dest, int rows, int cols )
   {
    // initialize function/variables
    int index, subIndex;
    
    // loop copying matrix
    for( index = 0; index < rows; index++ )
       {
        for( subIndex = 0; subIndex < cols; subIndex++ )
           {
            dest[index][subIndex] = src[index][subIndex];
           }
       }
   } 
   
   
   
   

   
