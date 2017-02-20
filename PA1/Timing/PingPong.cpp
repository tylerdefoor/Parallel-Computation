/** @file PingPong.cpp
  * @brief Passes messages between the same machine to time message passing
  * @author Tyler DeFoor
  * @date 2/16/2017
  * @version 1.0
  */

#include "mpi.h"
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#define MASTER  0
#define TAG     1
#define TYPE    MPI_INT
#define SLAVE   1

//A bad habit I will break one day
using namespace std; 

int main ( int argc, char* argv[] )
{
    /* Variable Declarations */
    int taskid, worldSize;
    double start, end, total;
    vector<int> send ( 10000, 1 );
    vector<int> receive ( 10000 );
    /* End of Variable Declarations */

    //Initialize MPI
    MPI_Init ( &argc, &argv );

    //Get the world size
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );
    //cout << "World size: " << worldSize << endl;

    /**
      Check to see if the world size is 2
      The program will function if the world size is greater
      This is just a check to make sure I'm doing things exactly as I want them
      */
       
    //If the world size isn't 2
    if ( worldSize != 2 )
    {
        //Error message
        cout << "World size should be 2. Make sure to use -n 2" << endl;
        cout << "Current world size is: " << worldSize << endl;

        //Abort
        MPI_Abort ( MPI_COMM_WORLD, 1 );
    }

    //Get Rank
    MPI_Comm_rank ( MPI_COMM_WORLD, &taskid );

    for ( int i = 2000; i <= 3000; i+= 10 )
    {
        //If we are in the master task
        if ( taskid == MASTER )
        {
            //Get the start time
            start = MPI_Wtime (  );

            //Send the number
            MPI_Send ( &send, i, TYPE, SLAVE, TAG, MPI_COMM_WORLD );

            //Receive the number
            MPI_Recv ( &receive, i, TYPE, SLAVE, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

            //End the timer 
            end = MPI_Wtime (  );
            receive.clear (  );

            //Get the total time
            total = end - start;
            
            //Block until both are finished
            MPI_Barrier ( MPI_COMM_WORLD );

            cout << i << " " << total << endl;
        }   

        //If we are in the slave task
        else if ( taskid == SLAVE )
        {
            //Receive the number
            MPI_Recv ( &receive, i, TYPE, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
            receive.clear (  );

            //Send the number
            MPI_Send ( &send, i, TYPE, MASTER, TAG, MPI_COMM_WORLD );

            //Block until both are finished
            MPI_Barrier ( MPI_COMM_WORLD );
        }
    }

    //Finalize MPI because I'm a good programmer
    MPI_Finalize (  );

    return 0;
}
