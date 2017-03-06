Mandelbrot_seq: PIMFuncs.o Mandelbrot_seq.o
	g++ Mandelbrot_seq.o PIMFuncs.o -o Mandelbrot_seq

Mandelbrot_seq.o: Mandelbrot_seq.cpp
	g++ -c Mandelbrot_seq.cpp

PIMFuncs.o: PIMFuncs.cpp
	g++ -c PIMFuncs.cpp

clean:
	\rm Mandelbrot_seq *.o *.out *.txt