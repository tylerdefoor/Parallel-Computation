#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <cstdio>
#include <mpi.h>

#define MAX_NUM 10

using namespace std;

int main(int argc, char* argv[])
{
  MPI_Init(&argc, &argv);

  unsigned long long int randomNum;
  srand(time(NULL));

  if(argc < 1)
  {
    cout << "invalid number of arguments. No size N was provided." << endl;
    return -1;
  }

  size_t size = atoi(argv[1]);
  string matrixAFilename = "../gen/A/" + string(argv[1]) + ".matrix";
  string matrixBFilename = "../gen/B/" + string(argv[1]) + ".matrix";

  ofstream a_out(matrixAFilename);

  for(size_t i = 0; i < size*size; i++)
  {
    randomNum = rand() % MAX_NUM;
    a_out << randomNum << endl;
  }

  a_out.close();

  ofstream b_out(matrixBFilename);

  for(size_t i = 0; i < size*size; i++)
  {
    randomNum = rand() % MAX_NUM;
    b_out << randomNum << endl;
  }

  b_out.close();

  MPI_Finalize();

  return 0;
}
