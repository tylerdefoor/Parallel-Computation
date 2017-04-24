CC=mpiCC
CFLAGS=-lpmi -lm -w -g -std=c++0x

all: mkdir_exec mkdir_gen mkdir_timings parallel generator

mkdir_exec:
	if [ ! -d "exec" ]; then mkdir exec; fi

mkdir_gen:
	if [ ! -d "gen" ]; then mkdir gen; fi

mkdir_timings:
	if [ ! -d "timings" ]; then mkdir timings; fi

generator: mkdir_gen */generator.cpp
	$(CC) $(CFLAGS) -o exec/generator */generator.cpp

parallel: mkdir_exec mkdir_timings */parallel.cpp
	$(CC) $(CFLAGS) -o exec/parallel */parallel.cpp

clean:
	rm exec/* bash/*.out

