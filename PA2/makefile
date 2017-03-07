Sequential: PIMFuncs.o Sequential.o
	g++ Sequential.o PIMFuncs.o -o Sequential

Static: Static.o PIMFuncs.o
	mpicxx -Wall -lpmi -o Static Static.o PIMFuncs.o

Static.o: Static.cpp
	mpicxx -Wall -lpmi -c Static.cpp

Sequential.o: Sequential.cpp
	g++ -c Sequential.cpp

PIMFuncs.o: PIMFuncs.cpp
	g++ -c PIMFuncs.cpp

clean:
	\rm Sequential Static *.o *.out *.txt *.pim 