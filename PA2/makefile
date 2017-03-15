Sequential: PIMFuncs.o Sequential.o Dynamic
	mpicxx -Wall -lpmi Sequential.o PIMFuncs.o -o Sequential

Dynamic: Dynamic.o PIMFuncs.o
	mpicxx -Wall -lpmi Dynamic.o PIMFuncs.o -o Dynamic

Dynamic.o: Dynamic.cpp
	mpicxx -Wall -lpmi -c Dynamic.cpp

Sequential.o: Sequential.cpp
	mpicxx -Wall -lpmi -c Sequential.cpp

PIMFuncs.o: PIMFuncs.cpp
	mpicxx -Wall -lpmi -c PIMFuncs.cpp

clean:
	\rm Sequential Static *.o *.out *.txt *.pim 
