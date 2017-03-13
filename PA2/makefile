Sequential: PIMFuncs.o Sequential.o
	mpicxx -Wall -lpmi Sequential.o PIMFuncs.o -o Sequential

Static: Static.o PIMFuncs.o
	mpicxx -Wall -lpmi -o Static Static.o PIMFuncs.o

Static.o: Static.cpp
	mpicxx -Wall -lpmi -c Static.cpp

Sequential.o: Sequential.cpp
	mpicxx -Wall -lpmi -c Sequential.cpp

PIMFuncs.o: PIMFuncs.cpp
	mpicxx -Wall -lpmi -c PIMFuncs.cpp

clean:
	\rm Sequential Static *.o *.out *.txt *.pim 