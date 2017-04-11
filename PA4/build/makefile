all: Sequential

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Sequential.o:
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

clean:
	\rm Sequential Parallel *.o *.out