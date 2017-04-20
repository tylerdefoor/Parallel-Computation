all: Sequential Parallel

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Sequential.o:
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

Parallel: Parallel.o
	mpicxx -Wall -lpmi Parallel.o -o Parallel

Parallel.o: 
	mpicxx -Wall -lpmi -c ../src/Parallel.cpp

clean:
	\rm Sequential Parallel *.o *.out