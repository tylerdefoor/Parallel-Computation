CC=mpicxx
CXXFLAGS=-Wall -std=c++11
LIBS=-lpmi

INCLUDES=


# Outputs
PARALLEL=parallel


# Recipes
all: $(PARALLEL)


# Parallel
$(PARALLEL): parallel.o
	$(CC) $(CXXFLAGS) -o $(PARALLEL) $(INCLUDES) parallel.o $(LIBS)

parallel.o: parallel.cpp
	$(CC) -c $(CXXFLAGS) -o parallel.o $(INCLUDES) \
	      parallel.cpp $(LIBS)

clean:
	rm *.o *.out parallel
