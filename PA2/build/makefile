all: Seuqntial Dynamic

Sequential: PIMFuncs.o Sequential.o 
	mpicxx -Wall -lpmi Sequential.o PIMFuncs.o -o Sequential

Dynamic: Dynamic.o PIMFuncs.o
	mpicxx -Wall -lpmi Dynamic.o PIMFuncs.o -o Dynamic

Dynamic.o: Dynamic.cpp
	mpicxx -Wall -lpmi -c ../src/Dynamic.cpp

Sequential.o: Sequential.cpp
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

PIMFuncs.o: PIMFuncs.cpp
	mpicxx -Wall -lpmi -c ../src/PIMFuncs.cpp

clean:
	\rm Sequential Dynamic *.o *.out *.txt *.pim 

