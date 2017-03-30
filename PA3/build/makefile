all: Sequential Generator

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Sequential.o:
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

Generator:
	gcc -o Generator ../src/generator.c

clean:
	\rm Sequential *.o *.out

