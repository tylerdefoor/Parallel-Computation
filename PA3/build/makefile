all: Seuqntial

Sequential: Sequential.o 
	mpicxx -Wall -lpmi Sequential.o -o Sequential

Sequential.o: Sequential.cpp
	mpicxx -Wall -lpmi -c ../src/Sequential.cpp

clean:
	\rm Sequential Dynamic *.o *.out *.txt *.pim 

