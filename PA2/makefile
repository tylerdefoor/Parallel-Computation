Mandelbrot_Seq: PIMFuncs.o Mandelbrot_Seq.o
	g++ Mandelbrot_Seq.o PIMFuncs.o -o Mandelbrot_Seq

Mandelbrot_Seq.o: Mandelbrot_Seq.cpp
	g++ -c Mandelbrot_Seq.cpp

PIMFuncs.o: PIMFuncs.cpp
	g++ -c PIMFuncs.cpp

clean:
	\rm Mandelbrot_Seq *.o *.out *.txt