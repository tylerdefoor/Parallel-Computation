#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <fstream>
using namespace std;
int main ( int argc, char** argv )
{
    ofstream fout;
    int width = atoi(argv[1]);
    string filename = argv[2];

    fout.open ( filename.c_str (  ) );

    fout << width << endl;

    for ( int i = 1; i <= width * width; i++ )
    {
        fout << i << " ";
        if ( i % width == 0 )
            fout << endl;
    }

    fout.close (  );

    return 0;
}
