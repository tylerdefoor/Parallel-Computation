all: Sequential Generator Dynamic

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Dynamic: Dynamic.o 
	mpicxx -Wall -lpmi Dynamic.o -o Dynamic

Dynamic.o: 
	mpicxx -Wall -lpmi -c ../src/Dynamic.cpp

Sequential.o:
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

clean:
	\rm Sequential Dynamic *.o *.out

