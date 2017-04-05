all: Sequential Generator Dynamic

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Dynamic: Dynamic.o 
	mpicxx -Wall -lpmi Dynamic.o -o Dynamic

Dynamic.o: 
	mpicxx -Wall -lpmi -c ../src/Dynamic.cpp

Sequential.o:
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

Generator:
	gcc -o Generator ../src/generator.c

clean:
	\rm Sequential *.o *.out

