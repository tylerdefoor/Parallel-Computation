CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1 = ../src/seq.cpp
FILE2 = ../src/para.cpp

OUT1 = sequ
OUT2 = para

all: $(OUT1) $(OUT2) 

$(OUT1): $(FILE1)
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)

$(OUT2): $(FILE2)
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE2) $(LIBS)
	
clean:
	\rm *.o $(OUT1) $(OUT2) *~
